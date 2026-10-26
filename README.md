# Rush 2049 Recompiled

A static recompilation of **San Francisco Rush 2049** (Nintendo 64, USA) into a native PC program. The game's
MIPS code is translated to C by [N64Recomp](https://github.com/N64Recomp/N64Recomp), runs on
[N64ModernRuntime](https://github.com/N64Recomp/N64ModernRuntime), and draws through
[RT64](https://github.com/rt64/rt64) in an SDL2 window. Its own audio microcode is recompiled as well, so the
game makes its own sound.

**This repository contains no game code or data.** You need your own ROM. The build turns it into the recompiled
program on your machine. Everything made from the ROM stays local and is git-ignored: the synthetic ROM, the
generated C, the audio microcode C++, the program and your saves. Do not share any of it.

The symbol names and function boundaries come from the
[Rush 2049 decompilation](https://github.com/cabi24/SFRush2049-n64-decomp).

## Status (v5, 2026-10-26)

It plays from boot to the finish line: Midway logo, title, attract demos, menus, races on every track, stunt,
battle (two players), circuit, practice, ghost races, records and audio options.

- **Sound**: the game's audio microcode is recompiled with RSPRecomp and played through SDL.
- **Saves**: a 32 KiB controller pak in the standard `.mpk` format, kept as `rush2049.mpk` in the `--home`
  folder and written on every change. Players, statistics and records survive a restart.
- **Input**: keyboard and up to four game controllers. The left-stick dead zone is rescaled, so movement starts
  smoothly past it.
- **Picture options**: `--msaa`, `--supersample`, `--res-scale` and `--fullscreen` (see [Running](#running)).

Where it has been tried:

| Platform | Result |
|---|---|
| Windows 11, clang-cl 19, NVIDIA RTX 3050 Laptop (D3D12 and Vulkan) | Built and played with the v2 sources. 17 windowed runs had no hangs or crashes at a steady 30 frames/s, with full stereo sound. |
| Linux (Rocky Linux 10, clang 21) | Built. Runs headless, and in the RT64 window on a software GPU (Mesa lavapipe under Xvfb). Not yet tried on a real GPU. |

The additions since v2 have run only on Linux on a software GPU: saves, scripted checks of every mode, and the
picture options. Nobody has played them on real hardware yet. See [CHANGES.md](CHANGES.md).

**Not there yet**: rumble (the game offers to swap to a rumble pak; there is none), widescreen, high frame rate,
an in-game settings menu (the options are command-line flags), texture packs and mods.

## Building

Short version (Linux). [BUILDING.md](BUILDING.md) has the full Windows and Linux steps, what each step should
print, and troubleshooting.

```bash
git clone --recursive https://github.com/cabi24/Rush2049Recompiled.git
cd Rush2049Recompiled
mkdir -p rom && cp /path/to/your/rush2049.z64 rom/baserom.us.z64      # SHA-1 3f99351d7bb61656614bdb2aa1a90cfe55d1922c

# 1. the recompilers
CC=clang CXX=clang++ cmake -S lib/N64Recomp -B build/N64Recomp -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja -C build/N64Recomp N64RecompCLI RSPRecomp

# 2. the synthetic ROM (your ROM + the game's three compressed code images unpacked), then the game as C
python3 tools/make_rom.py --rom rom/baserom.us.z64 --manifest config/manifest.json \
        --out config/rom/rush2049.recomp.z64
(cd config && ../build/N64Recomp/N64Recomp rush2049.recomp.toml && ../build/N64Recomp/RSPRecomp rush2049.rsp_audio.toml)

# 3. the program
CC=clang CXX=clang++ cmake -S runtime -B build/host -G Ninja \
    -DN64MODERNRUNTIME_DIR=$PWD/lib/N64ModernRuntime -DRECOMP_OUT=$PWD/config \
    -DRUSH_RENDERER=rt64 -DRT64_DIR=$PWD/lib/rt64
ninja -C build/host

build/host/rush2049_recomp --rom config/rom/rush2049.recomp.z64 --home run/home
```

Requirements:

- **Windows 10/11**: Visual Studio 2022 Build Tools with the C++ workload and the LLVM clang-cl toolset, plus
  CMake (3.20 or later), Ninja, Git and Python 3.9 or later.
- **Linux**: clang, lld, CMake, Ninja, Git, Python 3.9 or later, and the SDL2 and GTK3 development packages.
- About 1 GB of disk space for the sources and the build (on Linux, 760 MB), most of it RT64 and its submodules.

## Running

```
rush2049_recomp --rom config/rom/rush2049.recomp.z64 --home run/home [options]
```

`--home` holds the controller pak (`rush2049.mpk`). The program copies the ROM there at start, as the runtime
library requires, and deletes the copy as soon as the game has loaded it.

| Option | Effect |
|---|---|
| `--fullscreen` | borderless fullscreen on the current display |
| `--res-scale 0-8` | internal resolution N × 320×240. The default, 0, uses the largest whole multiple that fits the window (4× in the default 1280×960 window). |
| `--supersample 1-8` | render N times larger and scale down; default 1 (off) |
| `--msaa 0\|2\|4\|8` | multisample antialiasing; default 0 (off). If the GPU lacks that sample count, the log says so and antialiasing stays off. |
| `--gfx-api auto\|d3d12\|vulkan` | graphics API; by default RT64 chooses (D3D12 on Windows) |
| `--volume PERCENT`, `--no-sound` | audio volume (default 100), or no audio device |
| `--keyboard-alone` | the keyboard is player 1, and controllers start at player 2 |
| `--no-pak`, `--pak FILE` | no controller pak (the game shows its "no controller pak" notice), or use another `.mpk` image |
| `--log-input` | print every button and stick change (useful for checking a pad's mapping) |
| `--clock frames` | deterministic "lockstep" timing; a fallback if the default wall-clock mode misbehaves |
| `--renderer null --seconds N` | headless run for N seconds (no window) |

The log goes to stderr, with one status line per second. Running with no options prints the full list,
including the diagnostic options.

Controls:

| N64 | Keyboard (player 1) | Controller |
|---|---|---|
| Analog stick | arrow keys | left stick |
| A / B | X / C | A (south) / X (west) |
| Z | Z | left trigger |
| L / R | Q / E | LB / RB or right trigger |
| START | Enter | START |
| C-up/left/down/right | I / J / K / L | right stick, or Y (C-up) and B (C-down) |
| D-pad | W / A / S / D | D-pad |
| quit | Escape | |

## How it works

- **One recompilation for all four code populations.** The boot and library segment, the compressed game image
  (inflated to 0x80086A50), and two runtime images that both load at 0x8038A400 become four sections of a single
  N64Recomp symbol file. N64Recomp translates 2,135 functions, and the libultra functions that N64ModernRuntime
  reimplements come from the runtime instead. `tools/make_rom.py` builds the *synthetic ROM* that
  N64Recomp reads: your ROM unchanged, plus the three inflated images appended at 64 KiB-aligned offsets.
- **Two images at one address**: each image's internal calls resolve within the image. Calls into the shared
  range from outside go through a runtime lookup that follows whichever image the game loaded last.
- **Busy-waits**: the game polls variables that another thread or an interrupt changes. Under the runtime, game
  threads switch only inside libultra calls, so a hook inserted before 14 such loops yields to the other threads
  (`inputs/patches.json`, `runtime/src/hooks.cpp`).
- **What N64Recomp refuses** (cache operations, 64-bit float conversions, cop0 reads, a never-taken
  `bltzal zero` used to read the PC) is handled in the config by stubs, renames, host shims and single-word
  patches. Each one is listed with its reason in `inputs/patches.json`. The generated C is never edited.
- **Controller pak**: `runtime/src/pak.cpp` replaces the runtime's "no pak" `osPfs*` stubs with a real file
  system over an `.mpk` image.

| Path | What |
|---|---|
| `config/` | the N64Recomp and RSPRecomp configuration, pre-generated (names, addresses and sizes only), and `manifest.json` with the hashes the build checks |
| `inputs/` | the hand-curated inputs the config was generated from, each with its reason; see [inputs/README.md](inputs/README.md) |
| `runtime/` | the host program: CMake project, N64ModernRuntime integration, RT64 renderer, SDL2 window, input, audio, controller pak, hooks and shims |
| `tools/make_rom.py` | builds the synthetic ROM from your ROM and refuses anything that differs from what the config expects |
| `lib/` | N64Recomp, N64ModernRuntime and RT64 as git submodules at the pinned commits |

The host also keeps the decompilation project's verification modes: frame-hash logs, per-call capture,
scripted input and memory pokes. They are off unless you ask for them on the command line.

## Third-party components

| Component | Pinned commit | License |
|---|---|---|
| [N64Recomp](https://github.com/N64Recomp/N64Recomp) | `ffb39cdad1da5de07eaaa48bd1db4a89a7986771` | MIT |
| [N64ModernRuntime](https://github.com/N64Recomp/N64ModernRuntime) | `cdf5abbd5026fef5c364c676e4667c45e42b6863` | GPL-3.0 |
| [RT64](https://github.com/rt64/rt64) | `23cab603c4f9f4a8b369b38e036f1aa484603878` | MIT |

Their own submodules carry their own licenses. Any program you build links N64ModernRuntime, so it falls under
the GPL-3.0. It also contains code translated from your ROM.

The structure follows [Zelda64Recomp](https://github.com/Zelda64Recomp/Zelda64Recomp) (`rt64_renderer.cpp`
follows its RT64 render context).
