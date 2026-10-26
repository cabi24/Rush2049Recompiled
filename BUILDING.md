# Building Rush 2049 Recompiled

The same four steps on every OS:

1. Build `lib/N64Recomp` into the two recompilers: `N64Recomp` (game code) and `RSPRecomp` (audio microcode).
2. Run `tools/make_rom.py` on your ROM to write `config/rom/rush2049.recomp.z64`, the *synthetic ROM*: your ROM
   plus the game's three compressed code images unpacked. The script refuses anything that is not byte-identical
   to the ROM the config was generated against (sha256 `120270514798d1c08811c56f8ff1b0984aae063576cbfd642a6a6a1056b0e2ec`).
3. Run `N64Recomp config/rush2049.recomp.toml` to write `config/RecompiledFuncs/*.c`, the game as C
   (2,135 functions). Run `RSPRecomp config/rush2049.rsp_audio.toml` to write `config/rsp/rush_audio_ucode.cpp`,
   the game's audio microcode as C++. Without it the game is silent.
4. Build `runtime/` with N64ModernRuntime, RT64 and the C from step 3 into the program `rush2049_recomp(.exe)`.

Everything the steps write from your ROM (`config/rom/`, `config/RecompiledFuncs/`, `config/rsp/`, `build/`, `run/`)
is git-ignored. Keep it private.

**The ROM**: San Francisco Rush 2049 (USA), in big-endian `.z64` byte order, SHA-1
`3f99351d7bb61656614bdb2aa1a90cfe55d1922c`. Convert a byte-swapped `.v64` or `.n64` dump to `.z64` first. The
commands below expect it at `rom/baserom.us.z64`.

**Getting the sources**: clone with submodules. The submodules, mostly RT64 and its own, take about 450 MB.

```
git clone --recursive https://github.com/cabi24/Rush2049Recompiled.git
```

For a clone made without `--recursive`, run `git submodule update --init --recursive`. Afterwards,
`git submodule status` should list three lines, none starting with `-`.

Avoid spaces in the path you clone to.

---

## Windows 10/11

This path is the one that was built and played on Windows 11 with the v2 sources (clang-cl 19.1.5, VS 2022
Build Tools 17.14, CMake 3.31, Ninja 1.12). The build needs clang-cl, Visual Studio's LLVM toolset: the
recompiled C relies on clang extensions, and the configure step refuses MSVC's `cl.exe`.

### 1. Install the tools (once)

In PowerShell:

```powershell
winget install --id Microsoft.VisualStudio.2022.BuildTools -e --override "--passive --wait --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.Windows11SDK.22621 --add Microsoft.VisualStudio.Component.VC.CMake.Project --add Microsoft.VisualStudio.Component.VC.Llvm.Clang --add Microsoft.VisualStudio.Component.VC.Llvm.ClangToolset"
winget install --id Kitware.CMake -e
winget install --id Ninja-build.Ninja -e
winget install --id Git.Git -e
winget install --id Python.Python.3.12 -e
```

In the Visual Studio Installer GUI, the same set is "Desktop development with C++" with "C++ Clang Compiler for
Windows" and "C++ CMake tools for Windows" ticked. The Visual Studio Build Tools download is about 8 GB.

Open a new PowerShell so the new PATH applies, then check:

```powershell
git --version; cmake --version; ninja --version; python --version
```

### 2. Open the build shell (every time)

Every build command below runs in a Visual Studio x64 developer PowerShell:

```powershell
& "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\Launch-VsDevShell.ps1" -Arch amd64 -HostArch amd64
cd C:\rush2049\Rush2049Recompiled
$CLANGCL = "$($env:VCINSTALLDIR)Tools\Llvm\x64\bin\clang-cl.exe"
& $CLANGCL --version
```

The last line should print `clang version 1x.x ... Target: x86_64-pc-windows-msvc`. If `Launch-VsDevShell.ps1` is
not at that path, use the Start menu entry "Developer PowerShell for VS 2022" and then run the `cd` and
`$CLANGCL` lines. A message saying `'vswhere.exe' is not recognized` is harmless.

### 3. Check the ROM

```powershell
(Get-FileHash -Algorithm SHA1 rom\baserom.us.z64).Hash
```

It should print `3F99351D7BB61656614BDB2AA1A90CFE55D1922C`.

### 4. Build N64Recomp and RSPRecomp (a few minutes)

```powershell
cmake -S lib\N64Recomp -B build\N64Recomp -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER="$CLANGCL" -DCMAKE_CXX_COMPILER="$CLANGCL"
cmake --build build\N64Recomp --target N64RecompCLI RSPRecomp
```

This produces `build\N64Recomp\N64Recomp.exe` and `build\N64Recomp\RSPRecomp.exe`. The recompiler's CMake target
is `N64RecompCLI`. The target named `N64Recomp` is only a library.

### 5. Make the synthetic ROM and recompile (about a minute)

```powershell
python tools\make_rom.py --rom rom\baserom.us.z64 --manifest config\manifest.json --out config\rom\rush2049.recomp.z64
cd config
..\build\N64Recomp\N64Recomp.exe rush2049.recomp.toml
..\build\N64Recomp\RSPRecomp.exe rush2049.rsp_audio.toml
cd ..
```

What to expect:

- `make_rom` prints `make_rom: wrote ... = manifest synthetic ROM`.
- N64Recomp prints `Function count: 2135` and leaves about 43 `funcs_*.c` files plus `recomp_overlays.inl` and
  `lookup.cpp` in `config\RecompiledFuncs\`.
- RSPRecomp prints nothing and writes `config\rsp\rush_audio_ucode.cpp`, about 2,600 lines.

### 6. Build the program (5 to 20 minutes)

```powershell
cmake -S runtime -B build\host -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_C_COMPILER="$CLANGCL" -DCMAKE_CXX_COMPILER="$CLANGCL" -DN64MODERNRUNTIME_DIR="$PWD\lib\N64ModernRuntime" -DRECOMP_OUT="$PWD\config" -DRUSH_RENDERER=rt64 -DRT64_DIR="$PWD\lib\rt64"
cmake --build build\host
```

The configure step prints these lines:

- `Rush 2049 host: renderer rt64, capture OFF`. Per-call capture is a POSIX-only diagnostic, so it is off on
  Windows.
- `Rush 2049 host: recompiled audio ucode`. If it says `no audio ucode` instead, the RSPRecomp command in step 5
  did not run. Run it, delete `build\host`, and configure again.
- `check_contract: ... shims and 4 hooks defined`. This checks that the host defines every function the
  recompiled code needs from it.

The build compiles RT64 and its shaders, N64ModernRuntime, the generated C and the host, several hundred steps. It ends
with `build\host\rush2049_recomp.exe`, with `SDL2.dll`, `dxil.dll` and `dxcompiler.dll` copied next to it.

### 7. Headless check (about 40 seconds)

```powershell
mkdir run\logs -Force | Out-Null
cmd /c "build\host\rush2049_recomp.exe --renderer null --rom config\rom\rush2049.recomp.z64 --home run\home --seconds 30 2> run\logs\smoke.txt"
Select-String -Path run\logs\smoke.txt -Pattern "load game|load ovl|audio ucode|non-silent|removed the ROM|pak:|final"
```

You should see these lines, in roughly this order:

- `pak: controller 1, new run\home\rush2049.mpk (0 notes used, 123 free pages)` (`loaded` on later runs)
- `load game: stream ROM 0xB0CB10 -> 0x80086A50`
- `load ovl_a: ...`
- `removed the ROM copy ...`
- `audio ucode: recompiled`
- `first non-silent audio buffer`, about 3 s in, at the title
- `load ovl_b: ...`, about 8 s in, when the attract demo race starts
- a `final t=30...` line with `gfx_tasks=` around 900, `audio_nonzero=` well above 0, and `image=ovl_b`

### 8. Play

```powershell
cmd /c "build\host\rush2049_recomp.exe --rom config\rom\rush2049.recomp.z64 --home run\home 2> run\logs\play.txt"
```

A 1280×960 window opens. Close it or press Escape to quit. The [README](README.md#running) lists the options and
controls.

---

## Linux

Built on Rocky Linux 10 with clang 21, CMake 3.31 and Ninja 1.11. Other distributions with a recent clang should
work. The picture has been checked only on a software GPU (Mesa lavapipe under Xvfb).

```bash
sudo dnf install -y clang lld cmake ninja-build git python3 SDL2-devel gtk3-devel
# Debian/Ubuntu: sudo apt-get install clang lld cmake ninja-build git python3 libsdl2-dev libgtk-3-dev

git clone --recursive https://github.com/cabi24/Rush2049Recompiled.git
cd Rush2049Recompiled
mkdir -p rom && cp /path/to/your/rush2049.z64 rom/baserom.us.z64
sha1sum rom/baserom.us.z64          # 3f99351d7bb61656614bdb2aa1a90cfe55d1922c

CC=clang CXX=clang++ cmake -S lib/N64Recomp -B build/N64Recomp -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja -C build/N64Recomp N64RecompCLI RSPRecomp

python3 tools/make_rom.py --rom rom/baserom.us.z64 --manifest config/manifest.json \
        --out config/rom/rush2049.recomp.z64
(cd config && ../build/N64Recomp/N64Recomp rush2049.recomp.toml && ../build/N64Recomp/RSPRecomp rush2049.rsp_audio.toml)

CC=clang CXX=clang++ cmake -S runtime -B build/host -G Ninja \
    -DN64MODERNRUNTIME_DIR=$PWD/lib/N64ModernRuntime -DRECOMP_OUT=$PWD/config \
    -DRUSH_RENDERER=rt64 -DRT64_DIR=$PWD/lib/rt64
ninja -C build/host

build/host/rush2049_recomp --renderer null --rom config/rom/rush2049.recomp.z64 --home run/home --seconds 30
build/host/rush2049_recomp --rom config/rom/rush2049.recomp.z64 --home run/home 2> run/play.txt
```

The first program run is the headless check from Windows step 7, with the same expected lines. The second opens
the window. On Linux, RT64 uses Vulkan.

For a headless-only build without SDL2, GTK or RT64, leave out `-DRUSH_RENDERER=rt64 -DRT64_DIR=...`. The
default renderer is `null`.

### WSL2

Use WSL2 only if the native Windows build fails. Follow the Linux steps inside an Ubuntu distribution. The window
appears through WSLg, but RT64 then runs on Mesa's `dozen` (Vulkan on D3D12) or on the CPU, which may be very
slow or unsupported. The headless check is still meaningful there.

---

## Troubleshooting

| Symptom | What to do |
|---|---|
| `make_rom: ROM SHA-1 ... is not the US retail ROM` | wrong region or byte order. Only the USA `.z64` is supported. |
| configure: `set -DRECOMP_OUT to the absolute path of config/ ...` | the paths must be absolute, and step 5 must have produced `config/RecompiledFuncs/recomp_overlays.inl` |
| `lib/N64Recomp` has no `CMakeLists.txt`, or configure says `set -DN64MODERNRUNTIME_DIR to an N64ModernRuntime checkout` / `set -DRT64_DIR to an RT64 checkout` | the submodules are missing: `git submodule update --init --recursive` |
| configure: `runtime contract check failed` | the host lacks a function the recompiled code calls. The `check_contract:` line above it names the function. |
| `use clang-cl ..., not MSVC cl.exe` | pass `-DCMAKE_C_COMPILER` / `-DCMAKE_CXX_COMPILER` as in step 6, after deleting `build\host` |
| `[rush] RT64 setup: FAILED` in the log | try `--gfx-api vulkan`, then `--gfx-api d3d12`, and update the GPU driver |
| the window stays black | give it 60 s, since the front end takes about 5 s. Then try the other `--gfx-api`, then `--clock frames`. |
| `gfx_tasks=` stops growing | the game is stuck. The last 40 log lines show where. `--clock frames` is the fallback timing mode. |
| no sound | the `audio:` log line names the SDL audio device, or says why none opened. `audio_nonzero=` in the status lines says whether the game produced sound at all. |
| crackling, or sound too fast or too slow | compare the `gfx_tasks` rate (about 30 per second is normal). `--wav out.wav` records what the game produced. |
| `busy-wait at 0x...: yielding` lines | normal: the yield hooks are working |
| "INSERT RUMBLE PAK NOW" at a race start | expected. The pak in controller 1 is a controller pak, and the message goes away after a few seconds. |

A null-renderer build (`-DRUSH_RENDERER=null`, no RT64) shows whether the recompiled game itself builds, apart
from the renderer.
