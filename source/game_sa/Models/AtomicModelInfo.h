/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "BaseModelInfo.h"

struct tVehicleComponentFlag {
    const char* m_ucName;
    uint32      m_nFlag;
};

class NOTSA_EXPORT_VTABLE CAtomicModelInfo : public CBaseModelInfo {
public:
    CAtomicModelInfo() : CBaseModelInfo() {}

public:
    static void InjectHooks();

    // vtable overrides
    CAtomicModelInfo* AsAtomicModelInfoPtr() override;
    ModelInfoType GetModelType() override;
    void Init() override;
    void DeleteRwObject() override;
    uint32 GetRwModelType() override;
    RwObject* CreateInstance() override;
    RwObject* CreateInstance(RwMatrix* matrix) override;

    // vtable added methods
    virtual void SetAtomic(RpAtomic* atomic);

    // class methods
    struct RpAtomic* GetAtomicFromDistance(float distance);
    void SetupVehicleUpgradeFlags(const char* name);
};

void SetAtomicModelInfoFlags(CAtomicModelInfo* modelInfo, uint32 dwFlags);

VALIDATE_SIZE(CAtomicModelInfo, 0x20);
