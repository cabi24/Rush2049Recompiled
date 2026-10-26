/* Generated with the N64Recomp config: replaced functions (none in the stock build). */
#pragma once
#include <stdint.h>
#define RUSH_REPL_COUNT 0
static const char* const rush_repl_names[] = {
    0
};
#define RUSH_REPL_ROM 0x00000000u
#define RUSH_REPL_VRAM 0x00000000u
#define RUSH_REPL_SIZE 0x0u
#define RUSH_CAP_MAX_LOCS 48
/* {count, location codes...} per function */
static const int rush_cap_locs[][RUSH_CAP_MAX_LOCS + 1] = {
    {0}
};
static const int rush_cap_ret[] = {
    0
};
/* 1 = probe (not replaced; a callee): its effect leaves out the stack frames
   of the replaced functions it runs under */
static const int rush_cap_probe[] = {
    0
};
/* labels (call log header) */
static const char* const rush_cap_labels[] = {
    0
};
