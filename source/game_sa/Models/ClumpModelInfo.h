/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "BaseModelInfo.h"
#include "RwObjectNameIdAssocation.h"

struct RwFrame;
struct RpAtomic;
struct RpClump;

struct tCompSearchStructByName {
    const char* m_pName;
    RwFrame*    m_pFrame;

    inline tCompSearchStructByName(const char* name, RwFrame* frame) : m_pName(name), m_pFrame(frame) {}
};

struct tCompSearchStructById {
    int32    m_nId;
    RwFrame* m_pFrame;

    inline tCompSearchStructById(int32 id, RwFrame* frame) : m_nId(id), m_pFrame(frame) {}
};

class NOTSA_EXPORT_VTABLE CClumpModelInfo : public CBaseModelInfo {
public:
    union {
        char*  m_animFileName;
        uint32 m_nAnimFileIndex;
    };

public:
    static void InjectHooks();

    CClumpModelInfo() : CBaseModelInfo() {}

    // Overridden vtable methods
    ModelInfoType GetModelType() override;
    void Init() override;
    void Shutdown() override;
    void DeleteRwObject() override;
    uint32 GetRwModelType() override;
    RwObject* CreateInstance() override;
    RwObject* CreateInstance(RwMatrix* matrix) override;
    void SetAnimFile(const char* filename) override;
    void ConvertAnimFileIndex() override;
    int32 GetAnimFileIndex() override;

    // Added vtable methods
    virtual CBox* GetBoundingBox();
    virtual void SetClump(RpClump* clump);

    // Class functions
    void SetFrameIds(RwObjectNameIdAssocation* data);

    void SetClumpModelInfoFlags(uint32 flags); // Also calls SetBaseModelInfoFlags

    // static functions
    static RpAtomic* SetAtomicRendererCB(RpAtomic* atomic, void* renderFunc);
    static RpAtomic* AtomicSetupLightingCB(RpAtomic* atomic, void* data);
    static RpAtomic* SetHierarchyForSkinAtomic(RpAtomic* atomic, void* data);
    /* struct tSearchData { char *name; RwFrame *result; };
      returns 0 if we found frame, or last frame if we need to continue searching */
    static RwFrame* FindFrameFromNameCB(RwFrame* frame, void* searchData);
    static RwFrame* FindFrameFromNameWithoutIdCB(RwFrame* frame, void* searchData);
    static RwFrame* FindFrameFromIdCB(RwFrame* frame, void* searchData);
    static RwFrame* FillFrameArrayCB(RwFrame* frame, void* data);
    static RwFrame* GetFrameFromId(RpClump* clump, int32 id);
    static RwFrame* GetFrameFromName(RpClump* clump, const char* name);
    static void FillFrameArray(RpClump* clump, RwFrame** frames);
};

void SetClumpModelInfoFlags(CClumpModelInfo* modelInfo, uint32 dwFlags);

VALIDATE_SIZE(CClumpModelInfo, 0x24);
