// Interception of runtime-reimplemented libultra calls (portable).
//
// lockstep.cpp, oracle.cpp and hooks.cpp (run-burst census) define __wrap_NAME_recomp for the names below and
// call the runtime's own NAME_recomp as __real_NAME_recomp. The build renames
// every call in the recompiled game to the wrapper with a compile definition
// on the RecompiledFuncs target (CMakeLists.txt RUSH_WRAPPED: -DNAME=__wrap_NAME),
// which works with every compiler/linker (GNU ld --wrap would not: MSVC's
// link.exe does not have it). Keep this list and RUSH_WRAPPED in sync.
#pragma once
#define __real_osGetCount_recomp osGetCount_recomp
#define __real_osGetTime_recomp osGetTime_recomp
#define __real_osViSetEvent_recomp osViSetEvent_recomp
#define __real_osSetEventMesg_recomp osSetEventMesg_recomp
#define __real_osSpTaskStartGo_recomp osSpTaskStartGo_recomp
#define __real_osViSwapBuffer_recomp osViSwapBuffer_recomp
#define __real_osViGetCurrentFramebuffer_recomp osViGetCurrentFramebuffer_recomp
#define __real_osViGetNextFramebuffer_recomp osViGetNextFramebuffer_recomp
#define __real_osCreateThread_recomp osCreateThread_recomp
#define __real_osRecvMesg_recomp osRecvMesg_recomp
#define __real_osSendMesg_recomp osSendMesg_recomp
