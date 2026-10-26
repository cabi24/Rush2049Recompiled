# Changes

Newest first.

## v5 (2026-10-26)

- **Picture options** for the RT64 window. With none of them, the picture is the same as v4.
  - `--msaa 0|2|4|8`: multisample antialiasing. If the GPU does not support the count, the log says
    `RT64: MSAA Nx not supported by this device; antialiasing off`.
  - `--supersample 1-8`: render N times larger and scale down. Without `--res-scale` it renders at 4 × N times
    320×240.
  - `--res-scale 0-8`: a fixed internal resolution of N × 320×240. The default, 0, uses the largest whole
    multiple that fits the window.
  - The log prints the choice at start (`[rush] video: ...`) and what RT64 actually uses
    (`[rush] RT64 video: resolution ..., downsample ..., MSAA ...`).
- **Config regenerated** from the current decompilation inventory. Function names changed, with no effect on play.
  One function boundary was corrected: the high-score name-entry routine at 0x80104704 (DrawName) is now
  translated as one function, not a fragment plus a separate piece. A scripted default race is frame-for-frame
  identical to v4.
- Checked only on Linux with a software GPU: every combination of the new options starts, reaches a race and runs.

## v4 (2026-10-12)

- No gameplay change. Added a scripted run of stunt mode until its timer runs out.

## v3 (2026-10-12)

- **Controller pak and saves.** A 32 KiB controller pak is plugged into controller 1. It is kept in `--home` as
  `rush2049.mpk`, in the standard `.mpk` layout emulators use, and written after every change.
  - The boot notice "NO CONTROLLER PAK DETECTED" is gone. The game goes from the Midway logo to the title, and
    attract demos start after about 8 s of no input.
  - CREATE PLAYER, player statistics and RECORDS work, and survive a restart.
  - "INSERT RUMBLE PAK NOW" at a race start is expected. The pak in the controller is a controller pak, and there
    is no rumble.
  - `--no-pak` restores the old notice, and `--pak FILE` uses another image.
- **Controller polish.** The `controller -> player N` line is printed once per pad. The left-stick dead zone (0.15)
  is rescaled, so the value starts near 0 past it and still reaches 1.0 at the edge. Back, Guide and the stick
  clicks stay unmapped on purpose, since the N64 pad has no such buttons.
- Practice, ghost race, circuit, backward, another car, records and two-player battle were checked by scripted
  runs.
- Lockstep fix: with `--clock frames` and the RT64 window, the game now waits for a slow renderer instead of going
  on after 10 s.
- New status fields `pak_writes=` and `max_burst_ms=`.

## v2 (2026-10-12)

- **Boot hang fixed.** v1 hung on a black window in the default wall-clock mode. The game waits for its graphics
  scheduler in a loop with no system call, and the runtime switches threads only inside system calls. The host now
  yields in that loop and in the 13 other loops of the same kind (log `busy-wait at 0x...: yielding`, status field
  `spin_yields=`).
- **Sound.** The game's own audio microcode is recompiled with RSPRecomp, and the game's audio plays through SDL
  (`--volume`, `--no-sound`, `--wav PATH`). The game now gets one audio interrupt per buffer, not one per screen
  refresh, which had cut its audio output in half.
- Lockstep fix: with `--clock frames` the game could rebuild a display list while RT64 was still reading it.
- The program deletes its copy of the ROM from `--home` once the game has loaded it (`--keep-rom-copy` keeps it).
- New `--log-input`.
- Played on Windows 11 (D3D12 and Vulkan): 17 windowed runs with no hangs, 30 frames/s, and full stereo sound in
  every mode tried.

## v1 (2026-10-12)

- First playable build: the stock recompiled game with an RT64 window and live keyboard and controller input.
- Windows build support: the call interception is done by renaming calls at compile time, not with the GNU
  linker's `--wrap`. Per-call capture is POSIX-only and is off on Windows.
- Known limits at the time: no sound, no saves, and a boot hang in wall-clock mode (fixed in v2).
