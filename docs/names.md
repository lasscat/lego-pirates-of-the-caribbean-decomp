# Function names and translation-unit map

## Provenance

Retail `config/usa/arm9/symbols.txt` has no original symbol table beyond `dsd`-recovered SDK/entry symbols. Function names and original translation-unit groupings (`config/usa/tu_map.tsv`) come from ASCII string literals preserved in an earlier unstripped debug build of the same game (not distributed by this repository).

Although the debug build also lacks an ELF/DWARF symbol table, game and engine functions pass `Module_Function` and `SourceFile.cpp` strings to internal allocator and debug-tag helpers (and TWL-SDK functions pass `*.c` filenames to assert/warning helpers).

## Method (`tools/debug_names.py`)

1. **Tag extraction**: `tools/debug_names.py` walks ARM instructions in the debug build, tracking literal-pool string loads (`relocs.txt` `load` entries) into `bl` call sites (`arm_call*` entries). Call targets where >=80% of non-file string arguments are valid C identifiers (and at least 2 file+identifier pairs occur) are classified as tag consumers; SDK assert/warning helpers with prose messages are excluded.
2. **Exact masked anchors**: Both retail and debug ARM9 binaries have their instructions masked (`bl`/`blx` targets, out-of-function branches, and literal-pool words). Unique exact matches of >=4 instructions (>=16 bytes) form candidate anchors, filtered via Longest Increasing Subsequence (LIS) to `2,753` strictly monotonic anchors across `5,757` retail and `6,022` debug functions.
3. **Gap alignment**: Functions between consecutive anchors are paired via order-preserving Needleman-Wunsch DP scored by masked-instruction `SequenceMatcher` similarity.

## Tiers and hold-back rules

- **Tier 1 (`tier1`, auto-applicable)**:
  - Referenced at a verified tag-consumer call site by **exactly one** debug function, and that debug function references **exactly one** consumer name tag.
  - Valid unique C identifier containing an underscore (single-word generic names and `NitroMain` go to review), not colliding with existing symbols or other proposals, and not matching a different known `.cpp` file stem (catches inlined callees and cross-TU wrappers).
  - Retail partner is an exact anchor, an unambiguous `1x1` gap (`sim >= 0.45`), or a multi-item gap match with `sim >= 0.50` and a clear margin (`>= 0.10`) over the runner-up in the same gap, still named `func_<addr>`.
- **Review tier (`review.tsv`)**: Everything else — shared names (`Module_Load`, `Load`, `geSemiTransparent_AddToList`, `init`), generic single-word names (`LoadFile`, `ProcessAttributes`), strings loaded outside verified consumer calls (`BackButton`, `SelectButton`), low similarity (`< 0.45` / `< 0.50`), narrow gap margin (`< 0.10`), or already-named retail symbols (`main` -> `NitroMain`).
- **TU map (`tu_map.tsv`)**: `tagged` (direct debug partner carries a single file tag with anchor or `sim >= 0.45`), `bracketed` (lies between two `tagged` retail functions from the same source file), or `uncertain`.

## Current counts and self-check

- Verified tag consumers: 8 (`0x020a4af0`, `0x020a5bc8`, `0x020a7a48`, `0x020a7b74`, `0x020c293c`, `0x020cce60`, `0x020cd0b4`, `0x0213e7e0`)
- Monotonic LIS anchors: `2,753` (`5,757` retail / `6,022` debug functions)
- Proposed names (`build/names/debug_names.tsv`): `329` total (`186` `tier1`, `143` `review`)
- TU map confidence (`tu_map.tsv`): `231` `tagged`, `618` `bracketed`, `4,908` `uncertain`
- Self-check (`--self-check`, holding out every 5th interior anchor): recovered `550/550` (`100.0%`) held-out anchors; fuzzy DP disagreed with linear position interpolation on `318/2,156` (`14.75%`) multi-gap pairs.

## Running and applying

Run from the repository root with the debug `dsd` project directory:

```sh
venv/bin/python tools/debug_names.py --debug-project <debug-dsd-dir> --dry-run --self-check
venv/bin/python tools/debug_names.py --debug-project <debug-dsd-dir> --apply
```

After `--apply`, regenerate `build.ninja` and verify:

```sh
venv/bin/python tools/configure.py usa --no-extract --dsd $PWD/dsd --objdiff $PWD/objdiff-cli --compiler tools/mwccarm -w ./wibo
venv/bin/python tools/match.py verify
```
