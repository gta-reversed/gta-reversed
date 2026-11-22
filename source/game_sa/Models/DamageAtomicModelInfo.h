#pragma once

#include "AtomicModelInfo.h"

class NOTSA_EXPORT_VTABLE CDamageAtomicModelInfo : public CAtomicModelInfo {
public:
    RpAtomic* m_pDamagedAtomic;

    static inline auto& ms_bCreateDamagedVersion = StaticRef<bool>(0xA9B0B0);

public:
    CDamageAtomicModelInfo() : CAtomicModelInfo() {}

    CDamageAtomicModelInfo* AsDamageAtomicModelInfoPtr() override;
    void Init() override;
    void DeleteRwObject() override;
    struct RwObject* CreateInstance() override;
    struct RwObject* CreateInstance(RwMatrix* matrix) override;

    void SetDamagedAtomic(RpAtomic* atomic);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};

VALIDATE_SIZE(CDamageAtomicModelInfo, 0x24);
