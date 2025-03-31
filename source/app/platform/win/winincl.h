#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#define NOATOM
#define NOCLIPBOARD
#define NODRAWTEXT
#define NOKERNEL
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOMINMAX
#define STRICT

#define INC_OLE2 // for `V_VT` and `V_UNION` macros at some headers of Windows SDK

// #define NOCOLOR -- Used at ImGui Win32 impl.
// #define NOCTLMGR -- for LPCDLGTEMPLATE at some headers of Windows SDK
// #define NONLS -- for GetACP at common.h

#include <Windows.h>

#ifdef USE_DSOUND
    #include <mmsystem.h> // for DirectSound types
#endif

#undef GetCurrentTime
#undef MoveMemory
