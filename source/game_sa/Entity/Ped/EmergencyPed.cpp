#include "StdInc.h"

#include "EmergencyPed.h"

// 0x5DE340
CEmergencyPed::CEmergencyPed(ePedType pedType, uint32 modelIndex) : CPed(pedType) {
    SetModelIndex(modelIndex); // V1053 Calling the 'SetModelIndex' virtual function in the constructor may lead to unexpected result at runtime
    m_pedSpeech.Initialise(this);
}

void CEmergencyPed::MedicAI() {
    // NOP
}

void CEmergencyPed::FiremanAI() {
    // NOP
}

// 0x5DE400
void CEmergencyPed::ProcessControl() {
    CPed::ProcessControl();
    if (m_bWasPostponed || m_nPedState == PEDSTATE_DEAD)
        return;

    auto& weapon = GetActiveWeapon();
    weapon.Update(nullptr);
    if (m_nPedState != PEDSTATE_DIE) {
        weapon.Update(nullptr);
        if (m_nPedType != PEDTYPE_MEDIC && m_nPedType == PEDTYPE_FIREMAN) { // todo: bad OG expression
            CPed::IsPedInControl();
        }
    }
}

// 0x5DE470
bool CEmergencyPed::InRange(CPed* ped) {
    if (!m_pVehicle)
        return true;

    return (m_pVehicle->GetPosition() - ped->GetPosition()).Magnitude() <= 30.0f;
}
