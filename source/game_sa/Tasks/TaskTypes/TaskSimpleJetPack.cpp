#include "StdInc.h"

#include "TaskSimpleJetPack.h"

void CTaskSimpleJetPack::InjectHooks() {
    ReversibleHooks::Install("CTaskSimpleJetPack", "CTaskSimpleJetPack", 0x67B4E0, &CTaskSimpleJetPack::Constructor);
}

// 0x67B4E0
CTaskSimpleJetPack::CTaskSimpleJetPack(const CVector* pVecTargetPos, float fCruiseHeight, int32 nHoverTime, CEntity* entity) : CTaskSimple() {
    m_bIsFinished = false;
    m_bAddedIdleAnim = false;
    m_bAnimsReferenced = false;
    m_bAttackButtonPressed = false;
    m_bSwitchedWeapons = false;
    m_vecOldSpeed.Set(0.0f, 0.0f, 0.0f);
    m_fOldHeading = 0.0f;
    m_pJetPackClump = nullptr;
    m_pAnim = nullptr;
    m_vecTargetPos.Set(0.0f, 0.0f, 0.0f);
    m_nHoverTime = nHoverTime;
    m_fCruiseHeight = fCruiseHeight;
    m_nStartHover = 0;
    m_pTargetEnt = entity;
    m_nThrustStop = 0;
    m_nThrustFwd = 0;
    m_fThrustStrafe = 0.0f;
    m_fThrustAngle = 0.0f;
    m_fLegSwingFwd = 0.0f;
    m_fLegSwingSide = 0.0f;
    m_fLegTwist = 0.0f;
    m_fLegSwingFwdSpeed = 0.0f;
    m_fLegSwingSideSpeed = 0.0f;
    m_fLegTwistSpeed = 0.0f;

    if (entity)
        entity->RegisterReference(&m_pTargetEnt);

    if (pVecTargetPos)
        m_vecTargetPos = *pVecTargetPos;

    m_pFxSysL = nullptr;
    m_pFxSysR = nullptr;

    m_fxKeyTime = 0.0f; // Izzotop: this one is not initialized, let's do this (used in DoJetPackEffect)
}

CTaskSimpleJetPack* CTaskSimpleJetPack::Constructor(const CVector* pVecTargetPos, float fCruiseHeight, int32 nHoverTime, CEntity* entity) {
    this->CTaskSimpleJetPack::CTaskSimpleJetPack(pVecTargetPos, fCruiseHeight, nHoverTime, entity);
    return this;
}

// 0x67E600, 0x6801D0
CTaskSimpleJetPack::~CTaskSimpleJetPack() {
    plugin::CallMethod<0x67E600, CTaskSimpleJetPack*>(this);
}

// 0x67E6E0
bool CTaskSimpleJetPack::MakeAbortable(class CPed* ped, eAbortPriority priority, const CEvent* event) {
    return MakeAbortable_Reversed(ped, priority, event);
}
bool CTaskSimpleJetPack::MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x67E6E0, CTaskSimpleJetPack*, CPed*, eAbortPriority, const CEvent*>(this, ped, priority, event);
}

// 0x67C690
CTask* CTaskSimpleJetPack::Clone() {
    return Clone_Reversed();
}
CTask* CTaskSimpleJetPack::Clone_Reversed() {
    return plugin::CallMethodAndReturn<CTask*, 0x67C690, CTaskSimpleJetPack*>(this);
}

// 0x6801F0
bool CTaskSimpleJetPack::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x6801F0, CTaskSimpleJetPack*, CPed*>(this, ped);
}

// 0x67F6A0
void CTaskSimpleJetPack::Process() {
    plugin::CallMethod<0x67F6A0, CTaskSimpleJetPack*>(this);
}

// 0x67EF20
void CTaskSimpleJetPack::ProcessThrust(CPed* ped) {
    plugin::CallMethod<0x67EF20, CTaskSimpleJetPack*, CPed*>(this, ped);
}

// 0x67B5C0
void CTaskSimpleJetPack::ProcessAnims(CPed* ped) {
    plugin::CallMethod<0x67B5C0, CTaskSimpleJetPack*, CPed*>(this, ped);
}

// 0x67E7B0
void CTaskSimpleJetPack::ProcessControlInput(CPlayerPed* player) {
    plugin::CallMethod<0x67E7B0, CTaskSimpleJetPack*, CPlayerPed*>(this, player);
}

// 0x67B720
void CTaskSimpleJetPack::ApplyRollAndPitch(CPed* ped) {
    plugin::CallMethod<0x67B720, CTaskSimpleJetPack*, CPed*>(this, ped);
}

// 0x67B660
void CTaskSimpleJetPack::DropJetPack(CPed* ped) {
    plugin::CallMethod<0x67B660, CTaskSimpleJetPack*, CPed*>(this, ped);
}

// 0x67B7F0
void CTaskSimpleJetPack::DoJetPackEffect(CPed* ped) {
    plugin::CallMethod<0x67B7F0, CTaskSimpleJetPack*, CPed*>(this, ped);
}
