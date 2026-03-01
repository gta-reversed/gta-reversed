/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Object.h"

class NOTSA_EXPORT_VTABLE CCutsceneObject : public CObject {
public:
    union {
        RwFrame* m_pAttachToFrame;
        uint32   m_nAttachBone; // this one if m_pAttachmentObject != 0
    };
    CObject* m_pAttachmentObject;
    CVector  m_vWorldPosition;
    CVector  m_vForce;

public:
    static constexpr int32 NUM_CUTSCENE_VEHS = 6;
    static char* (&ms_sCutsceneVehNames)[NUM_CUTSCENE_VEHS];

public:
    static void InjectHooks();

    CCutsceneObject();

    void SetModelIndex(uint32 index) override;
    void ProcessControl() override;
    void PreRender() override;
    bool SetupLighting() override;
    void RemoveLighting(bool bRemove) override;

private:

private: // Functions fully inlined in android idb, and unnamed in PC idb
    static void      SetupCarPipeAtomicsForClump(uint32 modelId, RpClump* clump);
    static RpAtomic* SetupCarPipeAtomicCB(RpAtomic* atomic, void* data);
};

VALIDATE_SIZE(CCutsceneObject, 0x19C);
