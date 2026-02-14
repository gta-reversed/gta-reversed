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
public:
    eWeaponType m_weaponInfo;

public:
    static void InjectHooks();

    CWeaponModelInfo() : CClumpModelInfo() {}

    // VTable
    ModelInfoType GetModelType() override;
    void Init() override;
    void SetClump(RpClump* clump) override;

    // inlined
    void SetWeaponInfo(eWeaponType weapon) { m_weaponInfo = weapon; }
    eWeaponType GetWeaponInfo() { return m_weaponInfo; }
};

VALIDATE_SIZE(CWeaponModelInfo, 0x28);
