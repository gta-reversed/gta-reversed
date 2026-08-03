/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "ClumpModelInfo.h"
#include "eWeaponType.h"

class NOTSA_EXPORT_VTABLE CWeaponModelInfo : public CClumpModelInfo {
private:
    eWeaponType m_weaponInfo;

public:
    CWeaponModelInfo() : CClumpModelInfo() {}

    // VTable

    void Init() override;

    ModelInfoType GetModelType() override;
    void SetClump(RpClump* clump) override;

    // inlined
    void SetWeaponInfo(eWeaponType weapon) { m_weaponInfo = weapon; }
    eWeaponType GetWeaponInfo() { return m_weaponInfo; }

private: // NOTSA:
    friend void InjectHooksMain();
    static void InjectHooks();
};

VALIDATE_SIZE(CWeaponModelInfo, 0x28);
