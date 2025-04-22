#include "StdInc.h"

#include "EventDeadPed.h"

#include "IKChainManager_c.h"

// 0x4ADEA0
CEventDeadPed::CEventDeadPed(CPed* ped, bool bUnknown, uint32 deathTimeInMs) : CEventEditableResponse() {
    m_ped = ped;
    field_18 = bUnknown;
    m_deathTimeInMs = deathTimeInMs;
    CEntity::SafeRegisterRef(m_ped);
}

// 0x4ADF70
CEventDeadPed::~CEventDeadPed() {
    CEntity::SafeCleanUpRef(m_ped);
}

// 0x4B4830
bool CEventDeadPed::AffectsPed(CPed* ped) {
    if (!m_ped || ped == m_ped || !ped->IsAlive())
        return false;

    if (g_ikChainMan.IsLooking(ped) && g_ikChainMan.GetLookAtEntity(ped) == m_ped)
        return false;

    if (m_ped->physicalFlags.bSubmergedInWater)
        return false;

    const CVector distance = m_ped->GetPosition() - ped->GetPosition();
    if (m_ped->bKilledByStealth) {
        const float squaredMagnitude = distance.SquaredMagnitude();
        if (DotProduct(distance, ped->GetForward()) >= 0.0f && squaredMagnitude <= sq(8.0f))
            return sq(20.0f) > squaredMagnitude;
    }
    return false;
}

// 0x4B6E70
CEventEditableResponse* CEventDeadPed::CloneEditable() const noexcept {
    return new CEventDeadPed(m_ped, field_18, m_deathTimeInMs);

}
