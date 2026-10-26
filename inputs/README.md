# Recompiler inputs

`config/` was generated from these four hand-curated files plus the
[Rush 2049 decompilation](https://github.com/cabi24/SFRush2049-n64-decomp)'s symbol inventory: function names,
extents and the three compressed code images' layout. The generator lives in the decompilation project. These
files are here so the config's choices can be read with their reasons. The build does not read them.

| File | What |
|---|---|
| `patches.json` | what the config does about each function N64Recomp refuses or that needs host help. **stubs**: emitted empty, such as cache and TLB operations. **ignored**: not emitted, provided by a host shim instead. **renamed**: collides with a host libc name. **instruction**: single-word patches, such as the `bltzal zero` PC-read trick in the boot codec kernels. **hook**: C calls N64Recomp inserts, namely the busy-wait yields, the frame hook and the inflate hook. Every entry has its reason. |
| `identity.json` | recompiler-only names for static libultra functions. N64Recomp replaces libultra functions by name, so the name decides whether the runtime's implementation runs. |
| `extra_functions.json` | provisional extents for 10 boot-segment routines past the inventoried code. They are called from inventoried code, and without them N64Recomp would invent a function that runs into data. |
| `runtime_buffers.json` | names for the z-buffer, the framebuffers and the graphics task buffers, found from the running game (data symbols for logs only) |

Regenerating `config/` needs the decompilation repository and its tooling. If you change these files here, nothing
in this repository's build picks the change up.
