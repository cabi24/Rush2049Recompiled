/* Generated with the N64Recomp config: compressed stream -> recompiled
 * section (synthetic ROM offset) for the runtime's inflate hook. */
#pragma once
#include <stdint.h>
typedef struct { const char* name; uint32_t stream_rom; uint32_t section_rom;
                 uint32_t vram; uint32_t size; } RushImage;
static const RushImage rush_images[] = {
    { "game", 0x00B0CB10, 0x00C00000, 0x80086A50, 0x9DFA0 },
    { "ovl_a", 0x00B5C534, 0x00CA0000, 0x8038A400, 0x2F650 },
    { "ovl_b", 0x00B6FEC4, 0x00CD0000, 0x8038A400, 0xAB70 },
};
#define RUSH_NUM_IMAGES 3
