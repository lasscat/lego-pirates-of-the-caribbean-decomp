# Compiler identification

## Conclusion

| Setting | Value | Proven / Inferred |
|---|---|---|
| Compiler | `dsi/1.1` (`mwccarm.exe`, build 1018) | Proven (unique winner across 25 versions) |
| Optimization | `-O4,p` (speed) | Proven (`-O4,s`, `-O3`, `-O2` fail loop rotation/unrolling probes) |
| Language | C++ (`-lang=c++`) for game/engine `.cpp`; C (`-lang=c`) for SDK `.c` | Proven (vtable dispatch, `.init`/`.ctor` static initializers, C SDK) |
| Char signedness | `-char signed` | Proven (`func_020c0304` fails under `-char unsigned`) |
| String pooling | `-str noreuse` | Proven (duplicate string literals in same TU, e.g. `'%s - %d'` at `0x020181a8`/`0x020181b0`) |
| Base flags | `-enum int -proc arm946e -gccext,on -fp soft -inline noauto -Cpp_exceptions off -RTTI off -interworking -w off` | Matches `tools/configure.py` defaults (`-enum int`, `-inline noauto` inferred/consistent; no contrary probe) |
| SDK library code | Same compiler (`dsi/1.1`, `-O4,p`, `-lang=c`) | Proven (`func_02133ca4`, `func_021346e0`, `func_02137ca0`, `func_02137f4c` all match `dsi/1.1 -O4,p`) |
| Thumb stubs | 18 SDK `SVC_*` syscall stubs (`0x0200007c`..`0x020007a8`) require `-thumb` | Proven (`WaitByLoop`, `VBlankIntrWait`, `Mod` match `dsi/1.1 -thumb`) |

`game.toml` should set:
- `mwcc_version = "dsi/1.1"`
- `opt = "-O4,p"`
- `lang = "c"` (so `.c` files compile as C and `.cpp` files compile as `-lang=c++` per `tools/configure.py`)

## Evidence table

All probes below were compiled with `BASE_FLAGS` (`-enum int -char signed -str noreuse -proc arm946e -gccext,on -fp soft -inline noauto -Cpp_exceptions off -RTTI off -interworking -w off -gccinc -nolink -msgstyle gcc`) and `-O4,p`, comparing masked ARM/Thumb instructions (call targets and literal-pool words masked) against `extract/usa/arm9/arm9.bin`.

| Probe | Address | Size | File | `dsi/1.1` `-O4,p` | `dsi/1.1` `-O4,s` | `dsi/1.1p1..1.6sp2` `-O4,p` | `2.0/sp2p4` `-O4,p` | `1.2/sp4` `-O4,p` |
|---|---|---|---|---|---|---|---|---|
| `func_0200946c` (vcall) | `0x0200946c` | `0x34` | `tools/probes/game_arm.cpp` | OK | OK | OK | OK | 13/13 (`0x38`) |
| `func_0201c92c` | `0x0201c92c` | `0x10` | `tools/probes/game_arm.cpp` | OK | OK | 4/4 (`0x18`) | OK | OK |
| `func_02020e94` | `0x02020e94` | `0x28` | `tools/probes/game_arm.cpp` | OK | OK | OK | OK | 10/10 (`0x30`) |
| `func_0202564c` | `0x0202564c` | `0x2c` | `tools/probes/game_arm.cpp` | OK | OK | OK | OK | OK |
| `func_0202c670` | `0x0202c670` | `0x18` | `tools/probes/game_arm.cpp` | OK | OK | OK | OK | OK |
| `func_020306c0` | `0x020306c0` | `0x1c` | `tools/probes/game_arm.cpp` | OK | OK | 7/7 (`0x24`) | 5/7 | 5/7 |
| `func_02061f9c` | `0x02061f9c` | `0x24` | `tools/probes/game_arm.cpp` | OK | OK | 9/9 (`0x2c`) | OK | OK |
| `func_020654c0` | `0x020654c0` | `0x20` | `tools/probes/game_arm.cpp` | OK | OK | 8/8 (`0x28`) | OK | OK |
| `func_020c0304` (`-char signed`) | `0x020c0304` | `0x34` | `tools/probes/game_arm.cpp` | OK | OK | OK | 13/13 (`0x30`) | OK |
| `func_020d04b8` | `0x020d04b8` | `0x18` | `tools/probes/game_arm.cpp` | OK | OK | 6/6 (`0x24`) | OK | 6/6 (`0x20`) |
| `func_020d4ba8` | `0x020d4ba8` | `0x34` | `tools/probes/game_arm.cpp` | OK | OK | OK | 13/13 (`0x38`) | 13/13 (`0x38`) |
| `func_02126c34` | `0x02126c34` | `0x30` | `tools/probes/game_arm.cpp` | OK | 12/12 (`0x2c`) | OK | OK | 8/12 |
| `func_02126c64` | `0x02126c64` | `0x50` | `tools/probes/game_arm.cpp` | OK | OK | OK | 20/20 (`0x4c`) | 20/20 (`0x4c`) |
| `func_02126cb4` | `0x02126cb4` | `0x3c` | `tools/probes/game_arm.cpp` | OK | OK | OK | 10/15 | 15/15 (`0x38`) |
| `func_02126cf0` | `0x02126cf0` | `0x1c` | `tools/probes/game_arm.cpp` | OK | OK | OK | OK | OK |
| `func_0212c1e8` (SDK) | `0x0212c1e8` | `0x24` | `tools/probes/sdk_arm.c` | OK | OK | OK | OK | OK |
| `func_0212c874` (SDK) | `0x0212c874` | `0x1c` | `tools/probes/sdk_arm.c` | OK | OK | OK | OK | OK |
| `func_02133ca4` (SDK) | `0x02133ca4` | `0x48` | `tools/probes/sdk_arm.c` | OK | OK | 18/18 (`0x50`) | OK | OK |
| `func_021346e0` (SDK) | `0x021346e0` | `0x20` | `tools/probes/sdk_arm.c` | OK | 6/8 | OK | OK | OK |
| `func_02137ca0` (SDK) | `0x02137ca0` | `0x1c` | `tools/probes/sdk_arm.c` | OK | OK | OK | OK | OK |
| `func_02137f4c` (SDK) | `0x02137f4c` | `0x38` | `tools/probes/sdk_arm.c` | OK | 14/14 (`0x30`) | OK | OK | OK |
| `WaitByLoop` (Thumb) | `0x0200007c` | `0x04` | `tools/probes/sdk_thumb.c` | OK | - | - | OK | OK |
| `VBlankIntrWait` (Thumb) | `0x020002e4` | `0x06` | `tools/probes/sdk_thumb.c` | OK | - | - | OK | OK |
| `Mod` (Thumb) | `0x02000728` | `0x06` | `tools/probes/sdk_thumb.c` | OK | - | - | OK | OK |

### Key discriminators

- **`dsi/*` vs `1.2/*` & `2.0/*`**: `func_020d4ba8`, `func_02126c64`, and `func_02126cb4` match all `dsi/*` compilers and fail every `1.2/*` and `2.0/*` compiler.
- **`dsi/1.1` vs `dsi/1.1p1..dsi/1.6sp2`**: On `u16` bit-clear (`p->f10 &= ~imm`), `dsi/1.1` emits `bic`, whereas `dsi/1.1p1` through `dsi/1.6sp2` load the inverted mask (`0xfffffffc`, `0xfffffeff`, `0xffffffef`, etc.) from a literal pool and emit `and`. In the retail binary, `0xfffffffc`, `0xfffffeff`, `0xfffffdff`, `0xffffffef`, `0xfffffffe`, and `0xfffffff0` appear 0 times in `build/usa/asm/_dsd_gap@main_5.s`, while `func_0201c92c`, `func_020306c0`, `func_02061f9c`, `func_020654c0`, `func_020d04b8`, and `func_02133ca4` all use `bic`/`bicne` and match `dsi/1.1` uniquely.
- **`-O4,p` vs `-O4,s` / `-O3` / `-O2`**: `func_02126c34`, `func_021346e0`, and `func_02137f4c` match `-O4,p` and fail `-O4,s`, `-O3`, and `-O2` (no loop rotation / different loop setup).
- **`-char signed` vs `-char unsigned`**: `func_020c0304` matches `-char signed` (`ldrsb`) and fails `-char unsigned` (`1/13` diff).

## Reproducing

On Linux/Windows, `tools/probe_compiler.py` can be used for `.c` ARM probes (`python tools/probe_compiler.py FILE FUNCTION ADDRESS --flags=-O4,p`). On macOS, invoke the compiler directly via `./wibo` using the project flags (with `-lang=c++` for `.cpp` probes and `-thumb` for Thumb probes):

```sh
./wibo tools/mwccarm/dsi/1.1/mwccarm.exe -O4,p -enum int -char signed -str noreuse -proc arm946e -gccext,on -fp soft -inline noauto -Cpp_exceptions off -RTTI off -interworking -w off -gccinc -nolink -msgstyle gcc -lang=c++ -i include -c tools/probes/game_arm.cpp -o build/probe_game.o
./wibo tools/mwccarm/dsi/1.1/mwccarm.exe -O4,p -enum int -char signed -str noreuse -proc arm946e -gccext,on -fp soft -inline noauto -Cpp_exceptions off -RTTI off -interworking -w off -gccinc -nolink -msgstyle gcc -lang=c -i include -c tools/probes/sdk_arm.c -o build/probe_sdk.o
./wibo tools/mwccarm/dsi/1.1/mwccarm.exe -O4,p -enum int -char signed -str noreuse -proc arm946e -gccext,on -fp soft -inline noauto -Cpp_exceptions off -RTTI off -interworking -w off -gccinc -nolink -msgstyle gcc -thumb -lang=c -i include -c tools/probes/sdk_thumb.c -o build/probe_thumb.o
```

## Open questions

- `-inline noauto` vs `-inline on` / `-inline auto` / `-ipa file`: no probe yet splits `-inline noauto` from `-inline on` in a single TU; keep `-inline noauto` (standard DS default) until an intra-TU non-`inline` callee probe proves otherwise.
- `-enum int` vs `-enum min`: no contrary evidence; keep `-enum int`.
- Thumb files: only the 18 SDK `SVC_*` syscall stubs (`0x0200007c`..`0x020007a8`) are Thumb. They require `-thumb` via `config/usa/compilers.txt` when reconstructed.
