/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "rw/rwcore.h"
#include "rw/rphanim.h"
#include "rw/rpuvanim.h"
#include "rw/rpskin.h"
#include "rw/rpmatfx.h"
#include "rw/skeleton.h"

#ifdef _DX9_SDK_INSTALLED
#include "d3d9.h"
#endif

extern RwGlobals*& RwEngineInstance;

extern RsGlobalType& RsGlobal;
static inline bool& RwInitialized = *(bool*)0xC920E8;

extern RwPluginRegistry& geometryTKList;

extern RtDictSchema& RpUVAnimDictSchema;

struct IDirect3DDevice9* GetD3DDevice();

#ifndef D3DMATRIX_DEFINED
struct _D3DMATRIX;
#endif

_D3DMATRIX* GetD3DViewTransform();
_D3DMATRIX* GetD3DProjTransform();

#define RWRSTATE(a) (reinterpret_cast<void *>(a))
#define PSGLOBAL(var) (((psGlobalType *)(RsGlobal.ps))->var)

struct RwResEntrySA : RwResEntry {
    RxD3D9ResEntryHeader header;
    RxD3D9InstanceData meshData;
};

void RwCoreInjectHooks();
