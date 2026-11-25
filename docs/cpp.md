# C and C++ translation units

The original game and engine sources were C++ files (`GOCharacter.cpp`, `fnaFile.cpp`, `geScript.cpp`, ...) and
the Nintendo SDK sources were C (`os_mutex.c`, `card_backup.c`, ...): see [names.md](names.md). The code itself
is written in a C style (`Module_Function` names, plain structs), so most functions read the same in either
language. Which language each kind of code must be compiled as, and the evidence, is in [compiler.md](compiler.md).

## Build contract

`tools/configure.py` chooses the language by extension on each Ninja edge:

| Source | Per-file selector |
|---|---|
| `.c` | `-lang=` the language from `game.toml` (default `c`) |
| `.cpp` | `-lang=c++` |

There must be **exactly one** language selector in a compilation command. Keep it out of global `CC_FLAGS`,
`extra_cc_flags` and per-file compiler overrides. Do not change any other global compiler flag to match a target.
`tools/tests/test_configure.py` protects this contract without needing the compiler or the ROM; it does not
replace binary verification.

Do not leave a `.c` and a `.cpp` with the same basename: both map to the same object path.

## Plain symbols

Symbols in `config/usa/arm9/symbols.txt` are plain names. In a `.cpp` file use `extern "C"` on the ROM entry
point and on declarations of the functions and data it references; without it C++ names are mangled and the
link fails or, worse, silently binds to nothing. Do not rename symbols or add aliases to accommodate accidental
mangling. C++ declarations need real parameter types: `f()` means no parameters, not "unspecified".

## Classes and virtual calls

Use a class only where the binary shows one: an object pointer in `r0`, its vptr loaded from `[r0]`, a method
loaded from a consistent slot, and a call through it (`ldr ip, [r0]; ldr ip, [ip, #slot]; blx ip`). An indirect
call alone is not proof; ordinary callbacks and function-pointer tables stay plain.

- Declare virtual methods, never define them, not even as empty methods. Unknown methods keep their observed
  slot positions with provisional names. The vptr points at slot zero: do not add a vtable header to offsets.
- Do not add a virtual destructor or a guessed base class. Keep repeated class declarations consistent across
  files; move them to a shared header once the interface is evidenced and verified.
- A this-adjusting tail thunk suggests multiple inheritance. Treat it as its own investigation.

## No accidental vtables or initialisation

- Dispatch through existing object pointers. Do not construct or destroy objects unless the function evidently
  does (see [exceptions.md](exceptions.md)).
- No global/static class instances, RTTI, exception machinery or replacement vtable arrays. `-RTTI off` alone
  does not prevent vtable emission, and a `try`/`catch` makes the linker add an exception table.
- The object for a reconstructed function must contain only that function's allocatable `.text` (debug, string
  and symbol metadata are fine). Static initialisers belong to the `.init`/`.ctor` sections and are reconstructed
  deliberately, never as a side effect.

Check before integration:

```sh
ninja -t commands build/usa/src/auto/<symbol>.o
readelf -SWs build/usa/src/auto/<symbol>.o
llvm-objdump --triple=armv5te-none-eabi -dr build/usa/src/auto/<symbol>.o
```

If a candidate emits extra runtime sections or helpers, stop: either reconstruct their real ownership and ranges
explicitly or leave the candidate unfinished. Never discard emitted sections or weaken verification.

## Integration and verification

One source per function, a plain existing symbol, an exclusive end:

```text
src/auto/func_02020a9c.cpp:
    complete
    .text       start:0x02020a9c end:0x02020ad4
```

Check existing source-owned ranges for overlaps before adding a block, and keep blocks sorted by `start:`.

```sh
venv/bin/python tools/match.py verify
shasum lego_pirates_of_the_caribbean_usa.nds
```

Only `VERIFY PASSED` from the unchanged full-ROM verifier establishes a match; the ROM SHA1 must remain
`0a83684764a1a2c1cd16442f42891eb6c2affa46`. Standalone or masked comparisons and objdiff percentages are
candidate diagnostics. Re-run the full verification after any formatter change to a source.

## Tooling limits

Some draft/probe tools assume `.c` and pass `-lang=c`. Extension support in `configure.py` does not make those
tools C++-aware: inspect the actual compiler command before trusting a result for a `.cpp` candidate.
