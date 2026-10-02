#pragma once

#include "Vector.h"

class CVehicle;

struct CStoredCar {
    // NOTSA
    CStoredCar(CVehicle* veh) { StoreCar(veh); }
    CStoredCar() { Clear(); }

    // 0x449760
    void StoreCar(CVehicle* vehicle) {
        plugin::CallMethod<0x449760, CStoredCar*, CVehicle*>(this, vehicle);
    }

    // 0x447E40
    CVehicle* RestoreCar() {
        return plugin::CallMethodAndReturn<CVehicle*, 0x447E40, CStoredCar*>(this);
    }

    // NOTSA/Inlined
    void Init() { m_wModelIndex = 0; }
    void Clear() { m_wModelIndex = 0; }
    bool HasCar() const { return m_wModelIndex != 0; }

public:
    CVector m_vPosn{};
    uint32  m_nHandlingFlags{};
    uint8   m_nStoredCarFlags{};
    uint8   _pad0{};
    uint16  m_wModelIndex{};
    int16   m_awCarMods[15]{};
    uint8   m_nPrimaryColor{};
    uint8   m_nSecondaryColor{};
    uint8   m_nTertiaryColor{};
    uint8   m_nQuaternaryColor{};
    uint8   m_nRadioStation{};
    uint8   m_anCompsToUse[2]{};
    uint8   m_nBombType{};
    uint8   m_nPaintJob{};
    uint8   m_nNitroBoosts{};
    uint8   m_nPackedForwardX{};
    uint8   m_nPackedForwardY{};
    uint8   m_nPackedForwardZ{};
    uint8   _pad1{};
};
VALIDATE_SIZE(CStoredCar, 0x40);
