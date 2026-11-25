# Compiler identification

**Not identified yet.** `game.toml` holds a placeholder (`mwcc_version = "2.0/sp1p5"`, `-O4,p`) inherited from the
project template; no function has been matched with it.

What is known:

- The SDK version constant in the module parameters is `0x05057531` (TWL-SDK 5.5, building an NTR-mode ROM), which
  points at the CodeWarrior for DSi family (`tools/mwccarm/dsi/*`).
- Game and engine translation units were C++ (`.init`/`.ctor` sections are present); SDK sources were C.
- 5,739 functions are ARM and 18 are Thumb.
- The binary contains no compiler version strings, so the version has to be proven by code generation.

Method: `python tools/probe_compiler.py FILE FUNCTION ADDRESS` compiles a probe with every installed mwccarm
version and compares it with the original bytes (call targets and pool words masked). A version counts as
identified when it is the only byte-exact winner on several independent probes. Probe sources go in
`tools/probes/`; the evidence table goes here.
