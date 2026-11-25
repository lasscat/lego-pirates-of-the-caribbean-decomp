# Troubleshooting

Known errors, their cause and the fix. Add an entry every time you solve a new problem
(format: symptom, cause, fix).

## Build

**`sjiswrap: File ... contains Shift JIS encoding errors`, or macOS refuses a source as non-ASCII**
- Cause: non-ASCII characters (accents, UTF-8) in a source or header. The compiler reads sources as Shift-JIS.
- Fix: ASCII only, comments in English. Check with `grep -rnP "[^\x00-\x7F]" src include`.

**`ninja` prints `Downloading dsd ...` although dsd is present**
- Cause: `configure.py` downloads the exact `DSD_VERSION` it expects.
- Fix: nothing to do. To use pre-installed tools, pass `--dsd`, `--objdiff`, `--compiler` (or set `MWCCARM`).

**A tool reports `... is not in the subpath of 'tools/mwccarm'`**
- Cause: `build.ninja` was generated with an absolute `--compiler` path.
- Fix: re-run `tools/configure.py usa` with `--dsd` and `--objdiff` absolute and `--compiler tools/mwccarm` relative.

**After marking a file `complete`: `Symbol 'data_...' ... not found by name in linked binary (Possible name: @44)`**
- Cause: the compiler gives internal names to anonymous string literals (`@43`, `@44`) and to local statics
  (`name$37`). The ROM itself is already correct; only the name check fails.
- Fix: `ninja apply` copies these names into `symbols.txt` as `local` symbols, then run `ninja` again.

**A source compiles through Wibo but no object appears**
- Cause: a comma in the `-o` output path fails silently under Wibo.
- Fix: keep commas out of object and scratch paths.

## Matching

**`.data` at ~99% although the bytes are identical**
- Cause: dsd includes alignment padding in data symbol sizes (0x20 instead of 0x1f).
- Fix: give the real size in `symbols.txt`, e.g. `kind:data(byte[31])`.

**A judge reports zero differences but the ROM no longer matches**
- Cause: the judge masks call targets and pool words, so it cannot see a reference to the wrong symbol, and it
  never links, so it cannot see extra sections (`.ctor`, `.init`, vtables, exception tables from `try`/`catch`).
- Fix: only `tools/match.py verify` counts. Compare relocations of the candidate object against `relocs.txt`,
  and inspect its sections with `readelf -SWs`.

**A `switch` refuses to match with every compiler version**
- Cause: mwcc decides jump-table density with x87 arithmetic; Wibo's floating-point precision differs from real
  Windows, so a switch sitting exactly on the density threshold compiles differently.
- Fix: none in source. Record the function as blocked by the toolchain rather than forcing it.

## Probe tool

**Every compiler gives `N/N diff` even with correct C**
- Cause: a wrong function size was typed by hand.
- Fix: leave the size out, `probe_compiler.py` reads it from `symbols.txt`.

**`error: argument --flags: expected one argument`**
- Cause: a value starting with `-` (`-O4,p`) is parsed as an option.
- Fix: write `--flags=-O4,p`.

## Automation

**Files declared in `delinks.txt` disappear from the link (`Unreferenced symbols ... [whole file]`), symbols shift**
- Cause: mwldarm dead-strips unreferenced files by default, e.g. a callback only reached through the ROM header.
- Fix: `-nodead` in the linker flags (`tools/configure.py`).

**Drafts or `auto_match` use old symbol names (link error `Undefined : "func_..."`)**
- Cause: `dsd dis` does not delete its old output, and the gap file names change when `delinks.txt` changes, so
  a stale `.s` file with the old names was read first.
- Fix: delete `build/usa/asm/` before `ninja dis`.

**`unknown relocation type 'R_ARM_V4BX'` in the permuter**
- Cause: GNU as targets ARMv4 by default and tags every `bx` with a V4BX relocation.
- Fix: assemble the target with `-mcpu=arm946e-s`.

**Assembly functions no longer compile after `pre-commit` (instructions joined on one line)**
- Cause: clang-format joins the instructions of an `asm` function.
- Fix: wrap every `asm` function in `// clang-format off` / `// clang-format on`, and re-verify after formatting.

**On macOS, two scratch files overwrite each other**
- Cause: the default filesystem is case-insensitive: `t1.c` and `T1.c` are the same file.
- Fix: unique lowercase file names; never redirect output into a file the same command reads.
