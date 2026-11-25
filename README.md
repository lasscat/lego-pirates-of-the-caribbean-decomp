# LEGO Pirates of the Caribbean: The Video Game decompilation (Nintendo DS, USA)

Nintendo DS, game code `BZDE`. A matching decompilation: the C/C++ in `src/` is compiled with the original
Metrowerks compiler (mwccarm) and must rebuild the retail ROM byte for byte.

No game assets in this repository: see [`extract/README.md`](extract/README.md) for the ROM you have to supply.

## Progress

<!-- progress:start -->
`░░░░░░░░░░░░░░░░░░░░` **0.29%** of the code matched · data 0.00% · 27/5767 functions

| Part of the code | Files | Functions | Share of all code |
|---|---|---|---|
| Auto-matched (raw m2c output, to clean up) | 27 | 27/27 | 0.29% |

*Updated 2026-11-24 by `python tools/match.py progress` ([DS Decomp Studio](https://github.com/liolu/ds-decomp-studio)). A function counts only when it compiles to the exact same bytes and the ROM still rebuilds byte for byte.*
<!-- progress:end -->

## Building

```shell
python3 -m venv venv && venv/bin/pip install -r tools/requirements.txt ninja
venv/bin/python tools/configure.py usa
venv/bin/ninja
venv/bin/python tools/match.py verify     # must print VERIFY PASSED
venv/bin/python tools/match.py status
```

`tools/configure.py` downloads `dsd`, `objdiff-cli`, `wibo` and the compilers on first run. macOS notes are in
[`docs/macos.md`](docs/macos.md).

## Layout

| Path | What it holds |
|---|---|
| `config/usa/arm9/` | `dsd` project: `symbols.txt`, `relocs.txt`, `delinks.txt` (which source owns which bytes) |
| `config/usa/compilers.txt` | per-file compiler version / flag overrides |
| `src/auto/` | one function per file, named after its symbol |
| `include/` | shared headers |
| `tools/` | build, verify and probe tools |
| `docs/` | compiler identification, C++ rules, matching exceptions, naming, troubleshooting |

Everything not yet decompiled is linked from the original bytes, so the ROM always rebuilds.

## Rules

- Only `tools/match.py verify` printing `VERIFY PASSED` proves a match. Diff scores are diagnostics.
- Natural, clean-room C/C++ written from the binary. No leaked source. ASCII only in `src/` and `include/`.
- Exceptions (steering constructs, `volatile`, hand-written assembly) are marked in the source: see
  [`docs/exceptions.md`](docs/exceptions.md).
- Agents: read [`AGENTS.md`](AGENTS.md) first.

Function names and the source-file map come from strings in an earlier build: see [`docs/names.md`](docs/names.md).

Build tooling from [DS Decomp Studio](https://github.com/liolu/ds-decomp-studio) and
[ds-decomp](https://github.com/AetiasHax/ds-decomp). License: CC0-1.0 for everything in this repository.
