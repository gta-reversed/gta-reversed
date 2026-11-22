#include "StdInc.h"

#include "EventDeadPed.h"

#include "Ragdoll/IKChainManager.h"

// 0x4ADEA0
CEventDeadPed::CEventDeadPed(CPed* ped, bool bUnknown, uint32 deathTimeInMs) :
    CEventEditableResponse() {
    m_ped           = ped;
    field_18        = bUnknown;
    m_deathTimeInMs = deathTimeInMs;
    CEntity::SafeRegisterRef(m_ped);
}

// 0x4ADF70
CEventDeadPed::~CEventDeadPed() {
    CEntity::SafeCleanUpRef(m_ped);
}

// 0x4B4830
bool CEventDeadPed::AffectsPed(CPed* ped) {
    if (!m_ped || ped == m_ped || !ped->IsAlive()) {
        return false;
    }

    if (g_ikChainMan.IsLooking(ped) && g_ikChainMan.GetLookAtEntity(ped) == m_ped) {
        return false;
    }

    if (m_ped->physicalFlags.bSubmergedInWater) {
        return false;
    }

    const CVector distance = m_ped->GetPosition() - ped->GetPosition();
    if (m_ped->bKilledByStealth) {
        const float squaredMagnitude = distance.SquaredMagnitude();
        if (DotProduct(distance, ped->GetForward()) >= 0.0f && squaredMagnitude <= sq(8.0f)) {
            return sq(20.0f) > squaredMagnitude;
        }
    }
    return false;
}

// 0x4B6E70
CEventEditableResponse* CEventDeadPed::CloneEditable() const noexcept {
    return new CEventDeadPed(m_ped, field_18, m_deathTimeInMs);
}

void CEventDeadPed::InjectHooks() {
    RH_ScopedVirtualClass(CEventDeadPed, 0x85ae00, 17);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4ADEA0, "1", {}, CPed*, uint8, uint32);
    RH_ScopedDestructorInstall(0x4ADF70);

    RH_ScopedVMTInstall(GetEventType, 0x4ADF20);
    RH_ScopedVMTInstall(GetEventPriority, 0x4ADF60);
    RH_ScopedVMTInstall(GetLifeTime, 0x4ADF30);
    RH_ScopedVMTInstall(AffectsPed, 0x4B4830);
    RH_ScopedVMTInstall(GetSourceEntity, 0x4ADF40);
    RH_ScopedVMTInstall(GetLocalSoundLevel, 0x4ADF50);
    RH_ScopedVMTInstall(CloneEditable, 0x4B6E70);
}
