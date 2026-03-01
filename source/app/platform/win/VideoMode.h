#pragma once

#include "Platform.h"
#include <array>
#include <rwplcore.h>

//
// Sub Systems
//

//! Maximum number of subsystems we can deal with
//! 1 SubSystem / Display (And maybe / GPU ?)
constexpr auto MAX_SUBSYSTEMS = 16;

//! Subsystem infos (Populated using `RwEngineGetSubSystemInfo`)
static inline auto& GsubSysInfo = StaticRef<std::array<RwSubSystemInfo, MAX_SUBSYSTEMS>, 0xC8CFC0>();

//! Number of subsystems
static inline auto& GnumSubSystems = StaticRef<RwInt32, 0xC920F0>();

//! Currently selected subsystem
static inline auto& GcurSelSS = StaticRef<RwInt32, 0xC920F4>();

//! Whenever there are multiple subsystems available
static inline auto& MultipleSubSystems = StaticRef<RwBool, 0xC92118>();

//
// Video Mode
//

//! Currently selected videomode
static inline auto& GcurSelVM = StaticRef<RwInt32, 0x8D6220>(); // VM = Video Mode

//! Whenever to use the default videomode (Instead of the user selecting it)
static inline auto& UseDefaultVM = StaticRef<RwBool, 0xC920FC>();

//! Unused shit
static inline auto& DefaultVM = StaticRef<RwBool, 0x8D2E34>();

//! Whenever FrontEndMemnuManager videomode stuff was **NOT** yet set (See WinMain)
static inline auto& IsVMNotSelected = StaticRef<RwBool, 0x8D6218>();

/*
* Dynamic array of video modes with format "width x height x depth"
*/
static inline char**& gVideoModes = *(char***)0xC920D0;
static inline uint32& gCurrentGpu = *(uint32*)0x8D6248;
static inline int32&  gCurrentVideoMode = *(int32*)0x8D6220;

void VideoModeInjectHooks();

extern char** GetVideoModeList();
extern bool FreeVideoModeList();
extern void SetVideoMode(int32 mode);
extern bool IsVideoModeExclusive();
