#!/usr/bin/env python3
"""Recover function names and source-file groupings from an unstripped debug build.

Compares the retail ARM9 module against an earlier debug build that lacks a
symbol table but preserves file and function-name strings passed to internal
debug/allocator consumers.

Outputs (under --out-dir, default build/names/):
  - debug_names.tsv : all proposed function renames (auto and review tiers)
  - review.tsv      : held-back candidates with explicit hold-back reasons
  - tu_map.tsv      : retail translation-unit (source file) map

With --apply, renames only Tier-1 symbols currently named func_<addr> in
config/usa/arm9/symbols.txt and writes config/usa/tu_map.tsv.
"""

from __future__ import annotations

import argparse
import bisect
import collections
import difflib
import re
import shutil
import struct
import sys
from dataclasses import dataclass
from pathlib import Path

ARM9_BASE = 0x02000000
SYM_RE = re.compile(
    r"^(\S+)\s+kind:function\((\w+),size=0x([0-9a-fA-F]+)\)\s+addr:0x([0-9a-fA-F]+)(.*)$"
)
RELOC_RE = re.compile(r"^from:0x([0-9a-fA-F]+)\s+kind:(\S+)\s+to:0x([0-9a-fA-F]+)")
FILE_TAG_RE = re.compile(r"^[A-Za-z0-9_./-]+\.(?:cpp|c|h|hpp)$", re.I)
IDENT_RE = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
FUNC_ADDR_RE = re.compile(r"^func_[0-9a-fA-F]{8}$")


@dataclass(frozen=True)
class Func:
    idx: int
    addr: int
    size: int
    mode: str
    name: str
    line_idx: int


def load_symbols(path: Path) -> tuple[list[Func], list[str]]:
    lines = path.read_text(encoding="ascii").splitlines()
    funcs: list[Func] = []
    for i, line in enumerate(lines):
        m = SYM_RE.match(line)
        if m:
            funcs.append(
                Func(
                    idx=-1,
                    addr=int(m.group(4), 16),
                    size=int(m.group(3), 16),
                    mode=m.group(2),
                    name=m.group(1),
                    line_idx=i,
                )
            )
    funcs.sort(key=lambda f: f.addr)
    funcs = [
        Func(idx=i, addr=f.addr, size=f.size, mode=f.mode, name=f.name, line_idx=f.line_idx)
        for i, f in enumerate(funcs)
    ]
    return funcs, lines


def load_relocs(path: Path) -> tuple[dict[int, int], list[tuple[int, int]]]:
    calls: dict[int, int] = {}
    loads: list[tuple[int, int]] = []
    for line in path.read_text(encoding="ascii").splitlines():
        m = RELOC_RE.match(line)
        if not m:
            continue
        fr = int(m.group(1), 16)
        kind = m.group(2)
        to = int(m.group(3), 16)
        if kind.startswith("arm_call"):
            calls[fr] = to
        elif kind == "load":
            loads.append((fr, to))
    return calls, loads


def read_ascii_cstr(data: bytes, addr: int) -> str | None:
    off = addr - ARM9_BASE
    if off < 0 or off >= len(data):
        return None
    end = data.find(b"\0", off)
    if end < 0:
        return None
    raw = data[off:end]
    if len(raw) < 2 or len(raw) > 200:
        return None
    try:
        text = raw.decode("ascii")
    except UnicodeDecodeError:
        return None
    if not all(32 <= ord(c) < 127 for c in text):
        return None
    return text


def mask_arm_words(data: bytes, addr: int, size: int) -> tuple[int, ...]:
    off = addr - ARM9_BASE
    code = data[off : off + size]
    n = len(code) // 4
    if n == 0 or len(code) % 4 != 0:
        return ()
    words = struct.unpack(f"<{n}I", code)
    pool: set[int] = set()
    for i, w in enumerate(words):
        if (w & 0x0F7F0000) == 0x051F0000:
            imm = w & 0xFFF
            t = i + 2 + (imm // 4 if (w & 0x00800000) else -(imm // 4))
            if 0 <= t < n:
                pool.add(t)
    masked: list[int] = []
    for i, w in enumerate(words):
        if i in pool:
            masked.append(0xFFFFFFFF)
        elif (w & 0xFE000000) == 0xFA000000:
            masked.append(0xEB000000)
        elif (w & 0x0F000000) == 0x0B000000:
            masked.append(w & 0xFF000000)
        elif (w & 0x0F000000) == 0x0A000000:
            disp = w & 0xFFFFFF
            if disp & 0x800000:
                disp -= 0x1000000
            tgt = i + 2 + disp
            masked.append((w & 0xFF000000) if not (0 <= tgt < n) else w)
        else:
            masked.append(w)
    return tuple(masked)


def mask_thumb_words(data: bytes, addr: int, size: int) -> tuple[int, ...]:
    off = addr - ARM9_BASE
    code = data[off : off + size]
    n = len(code) // 2
    if n == 0 or len(code) % 2 != 0:
        return ()
    words = struct.unpack(f"<{n}H", code)
    pool: set[int] = set()
    for i, w in enumerate(words):
        if (w & 0xF800) == 0x4800:
            word_off = ((addr + i * 2 + 4) & ~3) + (w & 0xFF) * 4 - addr
            pool.add(word_off // 2)
            pool.add(word_off // 2 + 1)
    masked: list[int] = []
    for i, w in enumerate(words):
        if i in pool:
            masked.append(0xFFFF)
        elif (w & 0xF800) in (0xF000, 0xF800, 0xE800):
            masked.append(w & 0xF800)
        else:
            masked.append(w)
    return tuple(masked)


def func_signature(data: bytes, f: Func) -> tuple[int, ...]:
    if f.mode == "thumb":
        return mask_thumb_words(data, f.addr, f.size)
    return mask_arm_words(data, f.addr, f.size)


@dataclass
class DebugTags:
    file_tags: list[str]
    consumer_names: list[str]
    consumer_targets: list[int]
    other_idents: list[str]
    assert_msgs: list[str]


def extract_debug_tags(
    dfuncs: list[Func], d_bin: bytes, d_calls: dict[int, int], d_loads: list[tuple[int, int]]
) -> tuple[dict[int, DebugTags], set[int], set[str]]:
    starts = [f.addr for f in dfuncs]

    def func_of(addr: int) -> Func | None:
        i = bisect.bisect_right(starts, addr) - 1
        if i >= 0 and addr < dfuncs[i].addr + dfuncs[i].size:
            return dfuncs[i]
        return None

    pool_str: dict[int, str] = {}
    func_all_files: dict[int, set[str]] = collections.defaultdict(set)
    func_all_idents: dict[int, set[str]] = collections.defaultdict(set)
    known_files: set[str] = set()

    for fr, to in d_loads:
        s = read_ascii_cstr(d_bin, to)
        if s is None:
            continue
        pool_str[fr] = s
        fn = func_of(fr)
        if fn is None:
            continue
        if FILE_TAG_RE.match(s):
            func_all_files[fn.idx].add(s)
            known_files.add(s)
        elif IDENT_RE.match(s):
            func_all_idents[fn.idx].add(s)

    # First pass: observe all bl targets that receive a file tag at a call site.
    consumer_calls: dict[int, list[tuple[int, list[str], list[str], list[str]]]] = (
        collections.defaultdict(list)
    )
    for f in dfuncs:
        if f.mode != "arm":
            continue
        pending: dict[int, str] = {}
        for a in range(f.addr, f.addr + f.size, 4):
            ins = struct.unpack_from("<I", d_bin, a - ARM9_BASE)[0]
            if (ins & 0x0F7F0000) == 0x051F0000:
                imm = ins & 0xFFF
                tgt = a + 8 + (imm if (ins & (1 << 23)) else -imm)
                if tgt in pool_str:
                    pending[(ins >> 12) & 15] = pool_str[tgt]
            elif (ins & 0x0FFF0FF0) == 0x01A00000:
                rm, rd = ins & 15, (ins >> 12) & 15
                if rm in pending:
                    pending[rd] = pending[rm]
            if a in d_calls:
                bl_tgt = d_calls[a]
                if any(FILE_TAG_RE.match(s) for s in pending.values()):
                    fs = [s for s in pending.values() if FILE_TAG_RE.match(s)]
                    ids = [s for s in pending.values() if not FILE_TAG_RE.match(s) and IDENT_RE.match(s)]
                    oth = [s for s in pending.values() if not FILE_TAG_RE.match(s) and not IDENT_RE.match(s)]
                    consumer_calls[bl_tgt].append((f.idx, fs, ids, oth))
                pending.clear()

    # Classify verified tag consumers: >=2 calls with file+ident and >=80% of non-file strings are idents.
    verified_consumers: set[int] = set()
    for tgt, hits in consumer_calls.items():
        n_id = sum(len(ids) for _, _, ids, _ in hits)
        n_oth = sum(len(oth) for _, _, _, oth in hits)
        if n_id >= 2 and n_id / max(1, n_id + n_oth) >= 0.8:
            verified_consumers.add(tgt)

    per_func_c_files: dict[int, list[str]] = collections.defaultdict(list)
    per_func_c_names: dict[int, list[str]] = collections.defaultdict(list)
    per_func_c_tgts: dict[int, list[int]] = collections.defaultdict(list)
    per_func_asserts: dict[int, list[str]] = collections.defaultdict(list)

    for tgt, hits in consumer_calls.items():
        for f_idx, fs, ids, oth in hits:
            for fl in fs:
                if fl not in per_func_c_files[f_idx]:
                    per_func_c_files[f_idx].append(fl)
            if tgt in verified_consumers:
                for nm in ids:
                    if nm not in per_func_c_names[f_idx]:
                        per_func_c_names[f_idx].append(nm)
                if tgt not in per_func_c_tgts[f_idx]:
                    per_func_c_tgts[f_idx].append(tgt)
            else:
                for s in ids + oth:
                    if s not in per_func_asserts[f_idx]:
                        per_func_asserts[f_idx].append(s)

    tags: dict[int, DebugTags] = {}
    all_indices = (
        set(func_all_files)
        | set(func_all_idents)
        | set(per_func_c_files)
        | set(per_func_c_names)
        | set(per_func_asserts)
    )
    for f_idx in all_indices:
        ftags = list(per_func_c_files.get(f_idx, []))
        for fl in sorted(func_all_files.get(f_idx, ())):
            if fl not in ftags:
                ftags.append(fl)
        cnames = list(per_func_c_names.get(f_idx, []))
        oth_ids = sorted(x for x in func_all_idents.get(f_idx, ()) if x not in cnames)
        tags[f_idx] = DebugTags(
            file_tags=ftags,
            consumer_names=cnames,
            consumer_targets=list(per_func_c_tgts.get(f_idx, ())),
            other_idents=oth_ids,
            assert_msgs=list(per_func_asserts.get(f_idx, ())),
        )
    return tags, verified_consumers, known_files


def find_anchors(
    r_sigs: list[tuple[int, ...]],
    d_sigs: list[tuple[int, ...]],
    rfuncs: list[Func],
    dfuncs: list[Func],
    min_words: int = 4,
) -> list[tuple[int, int]]:
    d_map: dict[tuple[int, ...], list[int]] = collections.defaultdict(list)
    for i, sig in enumerate(d_sigs):
        if dfuncs[i].mode == "arm" and len(sig) >= min_words:
            d_map[sig].append(i)
    r_map: dict[tuple[int, ...], list[int]] = collections.defaultdict(list)
    for i, sig in enumerate(r_sigs):
        if rfuncs[i].mode == "arm" and len(sig) >= min_words:
            r_map[sig].append(i)

    raw_pairs = sorted(
        (r_idxs[0], d_map[sig][0])
        for sig, r_idxs in r_map.items()
        if len(r_idxs) == 1 and len(d_map.get(sig, ())) == 1
    )
    if not raw_pairs:
        return []
    # Longest strictly increasing subsequence on debug index to keep ordering monotonic.
    tails: list[int] = []
    tail_idx: list[int] = []
    prev = [-1] * len(raw_pairs)
    for idx, (_ri, di) in enumerate(raw_pairs):
        pos = bisect.bisect_left(tails, di)
        if pos == len(tails):
            tails.append(di)
            tail_idx.append(idx)
        else:
            tails[pos] = di
            tail_idx[pos] = idx
        if pos > 0:
            prev[idx] = tail_idx[pos - 1]
    lis: list[tuple[int, int]] = []
    k = tail_idx[-1]
    while k != -1:
        lis.append(raw_pairs[k])
        k = prev[k]
    lis.reverse()
    return lis


def pair_similarity(
    r_sig: tuple[int, ...], d_sig: tuple[int, ...], r_mode: str, d_mode: str
) -> float:
    if r_mode != d_mode:
        return 0.0
    if not r_sig or not d_sig:
        return 0.0
    if r_sig == d_sig:
        return 1.0
    return difflib.SequenceMatcher(None, r_sig, d_sig, autojunk=False).ratio()


def align_gap(
    r_idxs: range,
    d_idxs: range,
    r_sigs: list[tuple[int, ...]],
    d_sigs: list[tuple[int, ...]],
    rfuncs: list[Func],
    dfuncs: list[Func],
) -> tuple[dict[int, tuple[int, float, float, int, int]], list[tuple[int, int, float]]]:
    """Order-preserving DP alignment inside one inter-anchor gap.

    Returns:
      - d_to_r: debug_idx -> (retail_idx, sim, runner_up_r_sim, rg, dg)
      - pairs:  list of (retail_idx, debug_idx, sim)
    """
    rg = len(r_idxs)
    dg = len(d_idxs)
    if rg == 0 or dg == 0:
        return {}, []

    sims = [
        [
            pair_similarity(r_sigs[ri], d_sigs[di], rfuncs[ri].mode, dfuncs[di].mode)
            for di in d_idxs
        ]
        for ri in r_idxs
    ]
    # Small skip penalty encourages pairing when similarity > 0.12 while avoiding forced garbage.
    skip = 0.06
    dp = [[0.0] * (dg + 1) for _ in range(rg + 1)]
    bt = [[0] * (dg + 1) for _ in range(rg + 1)]
    for i in range(1, rg + 1):
        dp[i][0] = dp[i - 1][0] - skip
        bt[i][0] = 1
    for j in range(1, dg + 1):
        dp[0][j] = dp[0][j - 1] - skip
        bt[0][j] = 2
    for i in range(1, rg + 1):
        for j in range(1, dg + 1):
            match_score = dp[i - 1][j - 1] + sims[i - 1][j - 1]
            del_r = dp[i - 1][j] - skip
            del_d = dp[i][j - 1] - skip
            if match_score >= del_r and match_score >= del_d:
                dp[i][j] = match_score
                bt[i][j] = 0
            elif del_r >= del_d:
                dp[i][j] = del_r
                bt[i][j] = 1
            else:
                dp[i][j] = del_d
                bt[i][j] = 2

    pairs_local: list[tuple[int, int, float]] = []
    i, j = rg, dg
    while i > 0 and j > 0:
        step = bt[i][j]
        if step == 0:
            pairs_local.append((i - 1, j - 1, sims[i - 1][j - 1]))
            i -= 1
            j -= 1
        elif step == 1:
            i -= 1
        else:
            j -= 1
    pairs_local.reverse()

    d_to_r: dict[int, tuple[int, float, float, int, int]] = {}
    pairs_global: list[tuple[int, int, float]] = []
    for li, lj, sim in pairs_local:
        ri = r_idxs.start + li
        di = d_idxs.start + lj
        others = [sims[k][lj] for k in range(rg) if k != li]
        runner_up = max(others) if others else 0.0
        d_to_r[di] = (ri, sim, runner_up, rg, dg)
        pairs_global.append((ri, di, sim))
    return d_to_r, pairs_global


def align_all(
    anchors: list[tuple[int, int]],
    r_sigs: list[tuple[int, ...]],
    d_sigs: list[tuple[int, ...]],
    rfuncs: list[Func],
    dfuncs: list[Func],
) -> tuple[dict[int, tuple[int, float, str, float, int, int]], dict[int, tuple[int, float, str]]]:
    """Returns:
      - d_match: di -> (ri, sim, kind, runner_up, rg, dg)
      - r_match: ri -> (di, sim, kind)
    """
    d_match: dict[int, tuple[int, float, str, float, int, int]] = {}
    r_match: dict[int, tuple[int, float, str]] = {}

    prev_r, prev_d = -1, -1
    for ri, di in anchors + [(len(rfuncs), len(dfuncs))]:
        r_gap = range(prev_r + 1, ri)
        d_gap = range(prev_d + 1, di)
        if len(r_gap) > 0 and len(d_gap) > 0:
            d_to_r, pairs = align_gap(r_gap, d_gap, r_sigs, d_sigs, rfuncs, dfuncs)
            for g_di, (g_ri, sim, runner_up, rg, dg) in d_to_r.items():
                kind = "gap_1to1" if (rg == 1 and dg == 1) else "gap_dp"
                d_match[g_di] = (g_ri, sim, kind, runner_up, rg, dg)
                r_match[g_ri] = (g_di, sim, kind)
        if ri < len(rfuncs) and di < len(dfuncs):
            d_match[di] = (ri, 1.0, "anchor", 0.0, 1, 1)
            r_match[ri] = (di, 1.0, "anchor")
        prev_r, prev_d = ri, di
    return d_match, r_match


def run_self_check(
    anchors: list[tuple[int, int]],
    r_sigs: list[tuple[int, ...]],
    d_sigs: list[tuple[int, ...]],
    rfuncs: list[Func],
    dfuncs: list[Func],
) -> dict[str, float | int]:
    """Hold out every 5th interior anchor, re-align, measure exact recovery and DP vs interpolation."""
    if len(anchors) < 10:
        return {"held_out": 0, "recovered": 0, "recovery_pct": 0.0, "interp_disagree": 0}
    kept: list[tuple[int, int]] = []
    held: list[tuple[int, int]] = []
    for i, pair in enumerate(anchors):
        if 0 < i < len(anchors) - 1 and i % 5 == 0:
            held.append(pair)
        else:
            kept.append(pair)

    d_match, _ = align_all(kept, r_sigs, d_sigs, rfuncs, dfuncs)
    recovered = sum(1 for ri, di in held if di in d_match and d_match[di][0] == ri)

    # Compare fuzzy DP vs position-only linear interpolation across all non-1-to-1 gap pairs.
    interp_checked = 0
    interp_disagree = 0
    prev_r, prev_d = -1, -1
    for ri, di in anchors + [(len(rfuncs), len(dfuncs))]:
        rg = ri - prev_r - 1
        dg = di - prev_d - 1
        if rg > 1 and dg > 1:
            d_to_r, _ = align_gap(
                range(prev_r + 1, ri), range(prev_d + 1, di), r_sigs, d_sigs, rfuncs, dfuncs
            )
            for g_di, (g_ri, sim, _ru, _rg, _dg) in d_to_r.items():
                if sim >= 0.45:
                    j = g_di - (prev_d + 1)
                    interp_ri = (prev_r + 1) + min(rg - 1, max(0, round((j + 0.5) * rg / dg - 0.5)))
                    interp_checked += 1
                    if g_ri != interp_ri:
                        interp_disagree += 1
        prev_r, prev_d = ri, di

    return {
        "held_out": len(held),
        "recovered": recovered,
        "recovery_pct": round(100.0 * recovered / len(held), 2) if held else 0.0,
        "interp_checked": interp_checked,
        "interp_disagree": interp_disagree,
        "interp_disagree_pct": round(100.0 * interp_disagree / interp_checked, 2)
        if interp_checked
        else 0.0,
    }


def main() -> int:
    ap = argparse.ArgumentParser(description="Recover names and TU grouping from a debug build")
    ap.add_argument("--debug-project", type=Path, required=True, help="Path to debug dsd project")
    ap.add_argument("--out-dir", type=Path, default=Path("build/names"), help="Output directory")
    ap.add_argument("--dry-run", action="store_true", help="Write TSVs in --out-dir without modifying config/")
    ap.add_argument("--apply", action="store_true", help="Apply Tier-1 renames to symbols.txt and write tu_map.tsv")
    ap.add_argument("--self-check", action="store_true", help="Run hold-out anchor recovery self-check")
    args = ap.parse_args()

    if not args.dry_run and not args.apply:
        args.dry_run = True

    retail_root = Path.cwd()
    dbg_root = args.debug_project.resolve()

    r_sym_path = retail_root / "config/usa/arm9/symbols.txt"
    r_bin_path = retail_root / "extract/usa/arm9/arm9.bin"
    d_sym_path = dbg_root / "config/arm9/symbols.txt"
    d_bin_path = dbg_root / "extract/arm9/arm9.bin"
    d_rel_path = dbg_root / "config/arm9/relocs.txt"

    for p in (r_sym_path, r_bin_path, d_sym_path, d_bin_path, d_rel_path):
        if not p.is_file():
            raise SystemExit(f"Missing required input: {p}")

    rfuncs, r_lines = load_symbols(r_sym_path)
    dfuncs, _ = load_symbols(d_sym_path)
    r_bin = r_bin_path.read_bytes()
    d_bin = d_bin_path.read_bytes()
    d_calls, d_loads = load_relocs(d_rel_path)

    tags, verified_consumers, known_files = extract_debug_tags(dfuncs, d_bin, d_calls, d_loads)
    known_stems = {Path(fn).stem.lower(): fn for fn in known_files}

    r_sigs = [func_signature(r_bin, f) for f in rfuncs]
    d_sigs = [func_signature(d_bin, f) for f in dfuncs]

    anchors = find_anchors(r_sigs, d_sigs, rfuncs, dfuncs)
    d_match, r_match = align_all(anchors, r_sigs, d_sigs, rfuncs, dfuncs)

    # Count how many debug functions reference each consumer name.
    name_ref_counts: collections.Counter[str] = collections.Counter()
    for _d_idx, dt in tags.items():
        for nm in set(dt.consumer_names):
            name_ref_counts[nm] += 1

    existing_names = {f.name for f in rfuncs}

    # Build candidate proposals for every debug function with any name/ident evidence.
    proposed_rows: list[dict[str, str]] = []
    tier1_by_retail: dict[int, list[dict[str, str]]] = collections.defaultdict(list)
    tier1_by_name: dict[str, list[dict[str, str]]] = collections.defaultdict(list)

    for d_idx in sorted(tags):
        dt = tags[d_idx]
        df = dfuncs[d_idx]
        file_str = dt.file_tags[0] if len(dt.file_tags) == 1 else "|".join(dt.file_tags)

        # Pick candidate name(s) to evaluate.
        candidates: list[tuple[str, str]] = []
        if dt.consumer_names:
            for nm in dt.consumer_names:
                candidates.append((nm, "consumer"))
        elif dt.other_idents and dt.file_tags:
            for nm in dt.other_idents:
                candidates.append((nm, "outside_consumer"))
        else:
            continue

        match_info = d_match.get(d_idx)
        if match_info is None:
            r_addr_str = "-"
            cur_name = "-"
            sim_str = "0.000"
            kind = "unaligned"
            runner_up = 0.0
            rg = 0
            dg = 0
            ri = -1
        else:
            ri, sim, kind, runner_up, rg, dg = match_info
            rf = rfuncs[ri]
            r_addr_str = f"0x{rf.addr:08x}"
            cur_name = rf.name
            sim_str = f"{sim:.3f}"

        for cand_name, src_kind in candidates:
            reasons: list[str] = []
            if src_kind != "consumer":
                reasons.append("outside_consumer_call")
            if len(dt.consumer_names) > 1:
                reasons.append("multi_consumer_names_in_debug_func")
            if name_ref_counts[cand_name] > 1:
                reasons.append(f"shared_name({name_ref_counts[cand_name]}_debug_funcs)")
            if len(dt.file_tags) != 1:
                reasons.append("missing_or_multiple_file_tag")
            if not IDENT_RE.match(cand_name):
                reasons.append("invalid_c_identifier")
            if "_" not in cand_name and cand_name != "NitroMain":
                reasons.append("generic_single_word_name")
            if cand_name in existing_names:
                reasons.append("already_exists_in_symbols")
            if len(dt.file_tags) == 1 and "_" in cand_name:
                prefix = cand_name.split("_", 1)[0].lower()
                f_stem = Path(dt.file_tags[0]).stem.lower()
                if prefix in known_stems and prefix != f_stem:
                    reasons.append(f"prefix_{prefix}_matches_{known_stems[prefix]}_not_{dt.file_tags[0]}")

            if match_info is None:
                reasons.append("no_retail_partner")
            else:
                if kind == "anchor":
                    pass
                elif kind == "gap_1to1":
                    if sim < 0.45:
                        reasons.append(f"low_sim({sim:.3f})")
                else:
                    if sim < 0.50:
                        reasons.append(f"low_sim({sim:.3f})")
                    if (sim - runner_up) < 0.10:
                        reasons.append(f"ambiguous_gap_margin({sim:.3f}_vs_{runner_up:.3f})")
                if not FUNC_ADDR_RE.match(cur_name):
                    reasons.append("already_named_in_retail")

            tier = "tier1" if not reasons else "review"
            ev_parts = [
                f"align={kind}({rg}x{dg})",
                f"consumer={','.join(f'0x{t:08x}' for t in dt.consumer_targets) or 'none'}",
                f"d_refs={name_ref_counts[cand_name]}",
                f"f_tags={len(dt.consumer_names)}",
            ]
            if reasons:
                ev_parts.append("hold=" + ";".join(reasons))

            row = {
                "retail_addr": r_addr_str,
                "current_name": cur_name,
                "proposed_name": cand_name,
                "tier": tier,
                "similarity": sim_str,
                "debug_addr": f"0x{df.addr:08x}",
                "file": file_str or "-",
                "evidence": " ".join(ev_parts),
                "ri": str(ri),
            }
            proposed_rows.append(row)
            if tier == "tier1":
                tier1_by_retail[ri].append(row)
                tier1_by_name[cand_name].append(row)

    # Enforce uniqueness across all Tier-1 proposals on both retail target and name.
    for row in proposed_rows:
        if row["tier"] != "tier1":
            continue
        ri = int(row["ri"])
        nm = row["proposed_name"]
        extra: list[str] = []
        if len(tier1_by_retail[ri]) > 1:
            extra.append("duplicate_retail_partner")
        if len(tier1_by_name[nm]) > 1:
            extra.append("duplicate_proposed_name")
        if extra:
            row["tier"] = "review"
            row["evidence"] += " hold=" + ";".join(extra)

    # Also compute retail TU map (tagged / bracketed / uncertain).
    tier1_rename = {
        int(row["ri"]): row["proposed_name"]
        for row in proposed_rows
        if row["tier"] == "tier1"
    }
    direct_file: dict[int, tuple[str, int, str]] = {}
    for ri, (di, sim, kind) in r_match.items():
        dt = tags.get(di)
        if dt and len(dt.file_tags) == 1 and (kind == "anchor" or sim >= 0.45):
            direct_file[ri] = (dt.file_tags[0], dfuncs[di].addr, f"{kind}:{sim:.3f}")

    sorted_tagged = sorted(direct_file)
    tu_rows: list[tuple[str, str, str, str, str, str]] = []
    conf_counts: collections.Counter[str] = collections.Counter()
    for ri, rf in enumerate(rfuncs):
        sym_name = tier1_rename.get(ri, rf.name)
        if ri in direct_file:
            fl, d_addr, ev = direct_file[ri]
            conf = "tagged"
            d_addr_s = f"0x{d_addr:08x}"
        else:
            pos = bisect.bisect_left(sorted_tagged, ri)
            left_ri = sorted_tagged[pos - 1] if pos > 0 else None
            right_ri = sorted_tagged[pos] if pos < len(sorted_tagged) else None
            left_f = direct_file[left_ri][0] if left_ri is not None else None
            right_f = direct_file[right_ri][0] if right_ri is not None else None
            if left_f and right_f and left_f == right_f:
                fl = left_f
                conf = "bracketed"
                ev = f"between 0x{rfuncs[left_ri].addr:08x} and 0x{rfuncs[right_ri].addr:08x}"
            else:
                fl = "-"
                conf = "uncertain"
                ev = f"left={left_f or '-'} right={right_f or '-'}"
            d_addr_s = "-"
        conf_counts[conf] += 1
        tu_rows.append((f"0x{rf.addr:08x}", sym_name, fl, conf, d_addr_s, ev))

    out_dir = args.out_dir if args.out_dir.is_absolute() else (retail_root / args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    header_names = "retail_addr\tcurrent_name\tproposed_name\ttier\tsimilarity\tdebug_addr\tfile\tevidence\n"
    all_lines = [header_names]
    rev_lines = [header_names]
    for row in proposed_rows:
        line = "\t".join(
            [
                row["retail_addr"],
                row["current_name"],
                row["proposed_name"],
                row["tier"],
                row["similarity"],
                row["debug_addr"],
                row["file"],
                row["evidence"],
            ]
        ) + "\n"
        all_lines.append(line)
        if row["tier"] != "tier1":
            rev_lines.append(line)

    (out_dir / "debug_names.tsv").write_text("".join(all_lines), encoding="ascii")
    (out_dir / "review.tsv").write_text("".join(rev_lines), encoding="ascii")

    tu_header = "retail_addr\tsymbol\tfile\tconfidence\tdebug_addr\tevidence\n"
    tu_lines = [tu_header] + ["\t".join(r) + "\n" for r in tu_rows]
    (out_dir / "tu_map.tsv").write_text("".join(tu_lines), encoding="ascii")

    tier_counts = collections.Counter(r["tier"] for r in proposed_rows)
    print(f"Verified tag consumers: {', '.join(f'0x{t:08x}' for t in sorted(verified_consumers))}")
    print(f"Anchors (LIS): {len(anchors)} / {len(rfuncs)} retail, {len(dfuncs)} debug")
    print(f"Proposed names: tier1={tier_counts['tier1']} review={tier_counts['review']} total={len(proposed_rows)}")
    print(f"TU map confidence: tagged={conf_counts['tagged']} bracketed={conf_counts['bracketed']} uncertain={conf_counts['uncertain']}")

    if args.self_check:
        sc = run_self_check(anchors, r_sigs, d_sigs, rfuncs, dfuncs)
        print(
            f"Self-check: recovered {sc['recovered']}/{sc['held_out']} held-out anchors "
            f"({sc['recovery_pct']}%); fuzzy DP vs linear interpolation disagreed on "
            f"{sc['interp_disagree']}/{sc['interp_checked']} multi-gap pairs ({sc['interp_disagree_pct']}%)"
        )

    if args.apply:
        applied = 0
        new_lines = list(r_lines)
        for ri, new_name in tier1_rename.items():
            rf = rfuncs[ri]
            if not FUNC_ADDR_RE.match(rf.name):
                continue
            m = SYM_RE.match(new_lines[rf.line_idx])
            if not m or m.group(1) != rf.name:
                raise SystemExit(f"Symbol line mismatch at {rf.line_idx}: {new_lines[rf.line_idx]}")
            new_lines[rf.line_idx] = (
                f"{new_name} kind:function({m.group(2)},size=0x{m.group(3)}) addr:0x{m.group(4)}{m.group(5)}"
            )
            applied += 1
        r_sym_path.write_text("\n".join(new_lines) + "\n", encoding="ascii")
        shutil.copyfile(out_dir / "tu_map.tsv", retail_root / "config/usa/tu_map.tsv")
        print(f"Applied {applied} Tier-1 renames to {r_sym_path} and wrote config/usa/tu_map.tsv")

    return 0


if __name__ == "__main__":
    sys.exit(main())
