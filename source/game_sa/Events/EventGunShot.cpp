#include "StdInc.h"

#include "EventGunShot.h"

float& CEventGunShot::ms_fGunShotSenseRangeForRiot2 = *(float*)0x8A625C;

void CEventGunShot::InjectHooks() {
    RH_ScopedVirtualClass(CEventGunShot, 0x85ABE0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AC610);
    RH_ScopedVMTInstall(AffectsPed, 0x4B2CD0);
    RH_ScopedVMTInstall(IsCriminalEvent, 0x4AC810);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4AC780);
    RH_ScopedVMTInstall(CloneEditable, 0x4B6B20);
}

CEventGunShot* CEventGunShot::Constructor(CEntity* entity, CVector startPoint, CVector endPoint, bool bHasNoSound) { this->CEventGunShot::CEventGunShot(entity, startPoint, endPoint, bHasNoSound); return this; }

// 0x4AC610
CEventGunShot::CEventGunShot(CEntity* entity, CVector startPoint, CVector endPoint, bool bHasNoSound) : CEventEditableResponse() {
    m_startPoint = startPoint;
    m_endPoint = endPoint;
    m_firedBy = entity;
    m_bHasNoSound = bHasNoSound;
    CEntity::SafeRegisterRef(m_firedBy);
}

CEventGunShot::~CEventGunShot() {
    CEntity::SafeCleanUpRef(m_firedBy);
}

// 0x4B2CD0
bool CEventGunShot::AffectsPed(CPed* ped) {
    if (!m_firedBy)
        return false;

    if (m_firedBy->IsPed() && CPedGroups::AreInSameGroup(ped, m_firedBy->AsPed()))
        return false;

    if (!ped->IsInVehicleThatHasADriver()) {
        CWanted* playerWanted = FindPlayerWanted();
        if (ped->m_nPedType == PED_TYPE_COP && playerWanted->m_nWantedLevel > 0) {
            CCopPed* cop = static_cast<CCopPed*>(ped);
            if (playerWanted->IsInPursuit(cop) || playerWanted->CanCopJoinPursuit(cop)) {
                if (m_firedBy != FindPlayerPed())
                    return false;
            }
        }
        if (ped->IsAlive() && ped != m_firedBy && !ped->IsPlayer()) {
            float fGunShotRange = 45.0f;
            if (CEventGunShot::ms_fGunShotSenseRangeForRiot2 <= 0.0f) {
                if (ped->IsCreatedByMission()) {
                    fGunShotRange = ped->GetIntelligence()->GetPedFOVRange();
                }
            } else if (ped->IsCreatedByMission()) {
                fGunShotRange = CEventGunShot::ms_fGunShotSenseRangeForRiot2;
            }

            const auto& pedPos = ped->GetPosition();
            if (DistanceBetweenPointsSquared(m_firedBy->GetPosition(), pedPos) <= sq(fGunShotRange)) {
                if (!m_bHasNoSound)
                    return true;

                if (DotProduct(m_startPoint - pedPos, ped->GetForward()) > 0.0f) {
                    if (CWorld::GetIsLineOfSightClear(m_startPoint, pedPos, true, true, true, true, true, false, false))
                        return true;
                }
            }
        }
    }
    return false;
}

// 0x4AC810
bool CEventGunShot::IsCriminalEvent() {
    return m_firedBy && m_firedBy->IsPed() && m_firedBy->AsPed()->IsPlayer();
}

// 0x4AC780
bool CEventGunShot::TakesPriorityOver(const CEvent& refEvent) {
    if (refEvent.GetEventType() == GetEventType()) {
        bool bIsPlayer = false;
        bool otherPedIsPlayer = false;
        const auto refEventGunShot = static_cast<const CEventGunShot*>(&refEvent);
        if (m_firedBy && m_firedBy->AsPed()->IsPed()) {
            bIsPlayer = m_firedBy->AsPed()->IsPlayer();
        }

        CPed* otherPed = refEventGunShot->m_firedBy->AsPed();
        if (otherPed && otherPed->IsPed())
            otherPedIsPlayer = otherPed->IsPlayer();

        return bIsPlayer && !otherPedIsPlayer;
    }
    return GetEventPriority() >= refEvent.GetEventPriority();
}

// 0x4B6B20
CEventEditableResponse* CEventGunShot::CloneEditable() {
    return new CEventGunShot(m_firedBy, m_startPoint, m_endPoint, m_bHasNoSound);
}