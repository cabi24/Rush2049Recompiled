# config

The pre-generated N64Recomp / RSPRecomp configuration. It holds names, addresses, sizes and hashes only, with no
ROM bytes. The build writes its ROM-derived outputs here as well, and those are git-ignored:

| Path | Written by | What |
|---|---|---|
| `rom/rush2049.recomp.z64` | `tools/make_rom.py` | the synthetic ROM: your ROM plus the three inflated code images at 0xC00000, 0xCA0000 and 0xCD0000 |
| `RecompiledFuncs/` | `N64Recomp rush2049.recomp.toml` | the game as C |
| `rsp/rush_audio_ucode.cpp` | `RSPRecomp rush2049.rsp_audio.toml` | the audio microcode as C++ |

| File | What |
|---|---|
| `rush2049.recomp.toml` | N64Recomp config: input files, stubs, ignored, renamed, instruction patches and hooks |
| `rush2049.syms.toml` | four sections (`.static`, `.game`, `.ovl_a`, `.ovl_b`) with every function's name, address and size |
| `rush2049.datasyms.toml` | data symbols (for logs; not read by N64Recomp in symbol-file mode) |
| `rush2049.rsp_audio.toml` | RSPRecomp config for the game's audio microcode |
| `rush2049_images.h` | compressed stream → recompiled section map, used by the runtime's inflate hook |
| `rush2049_repl.h` | replaced-function table (empty: this is the stock build) |
| `manifest.json` | hashes `make_rom.py` checks, the runtime contract (the shims and hooks the host must define, checked at configure time), section summaries |

Run both recompilers from inside this directory: the paths in the `.toml` files are relative to it.
