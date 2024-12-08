#include "StdInc.h"

#include "IKChainManager_c.h"
#include "IKChain_c.h"
#include "BoneNodeManager_c.h"
#include "TaskSimpleIKManager.h"
#include "TaskSimpleIKLookAt.h"
#include "TaskSimpleIKPointArm.h"

IKChainManager_c& g_ikChainMan = *(IKChainManager_c*)0xC15448;

void IKChainManager_c::InjectHooks() {
    RH_ScopedClass(IKChainManager_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x6180A0);
    RH_ScopedInstall(Exit, 0x6180D0);
    RH_ScopedInstall(Reset, 0x618140);
    RH_ScopedInstall(Update, 0x6186D0);
    RH_ScopedInstall(AddIKChain, 0x618750);
    RH_ScopedInstall(RemoveIKChain, 0x618170);
    RH_ScopedInstall(CanAccept, 0x618800);
    RH_ScopedInstall(IsLooking, 0x6181A0);
    RH_ScopedInstall(GetLookAtEntity, 0x6181D0);
    RH_ScopedInstall(GetLookAtOffset, 0x618210);
    RH_ScopedInstall(AbortLookAt, 0x618280);
    RH_ScopedInstall(CanAcceptLookAt, 0x6188B0);
    RH_ScopedInstall(LookAt, 0x618970);
    RH_ScopedInstall(IsArmPointing, 0x6182B0);
    RH_ScopedInstall(AbortPointArm, 0x6182F0);
    RH_ScopedInstall(IsFacingTarget, 0x618330);
    RH_ScopedInstall(PointArm, 0x618B60);
}

// 0x6180A0
bool IKChainManager_c::Init() {
    for (auto& chain : m_IKChains) {
        m_FreeList.AddItem(&chain);
    }
    return true;
}

// 0x6180D0
void IKChainManager_c::Exit() {
    rng::for_each(m_ActiveList, &IKChain_c::Exit);
    m_ActiveList.RemoveAll();
    m_FreeList.RemoveAll();
}

// 0x618140
void IKChainManager_c::Reset() {
    Exit();
    Init();
}

// 0x6186D0
void IKChainManager_c::Update(float timeStep) {
    for (auto s = 0u; s < (uint32)eIKChainSlot::COUNT; s++) {
        CWorld::IncrementCurrentScanCode();

        for (auto& chain : m_ActiveList) {
            if (chain.GetIKSlot() != (eIKChainSlot)s) {
                continue;
            }

            // Update RpHAnim of ped (if any) - TODO: Maybe move this code into `IKChain_c::Update` as well..
            if (const auto p = chain.GetPed()) {
                if (!p->IsScanCodeCurrent()) {
                    p->UpdateRpHAnim();
                    p->SetCurrentScanCode();
                }
            }

            // Now update the IKChain itself
            chain.Update(timeStep);
        }
    }
}

// 0x618750
IKChain_c* IKChainManager_c::AddIKChain(
    const char*  name,
    eIKChainSlot slot,
    CPed*        ped,
    eBoneTag32   effectorBone,
    RwV3d        effectorOffset,
    eBoneTag32   pivotBone,
    CEntity*     entity,
    eBoneTag32   offsetBone,
    RwV3d        offset,
    float        speed,
    int32        priority
) {
    if (auto chain = m_FreeList.RemoveHead()) {
        if (chain->Init(name, slot, ped, effectorBone, effectorOffset, pivotBone, entity, offsetBone, offset, speed, priority)) {
            m_ActiveList.AddItem(chain);
            return chain;
        }
        m_FreeList.AddItem(chain); // Failed, add back to free list
    }
    return nullptr; // No more free chains
}

// 0x618170
void IKChainManager_c::RemoveIKChain(IKChain_c* chain) {
    chain->Exit();
    m_ActiveList.RemoveItem(chain);
    m_FreeList.AddItem(chain);
}

// 0x618800
bool IKChainManager_c::CanAccept(CPed* ped, float dist) const {
    if (!ped->GetModellingMatrix() || !ped->IsAlive() || !ped->GetIsOnScreen()) {
        return false;
    }

    return TheCamera.m_PlayerWeaponMode.m_nMode == MODE_SNIPER || dist >= 999.f || dist * dist >= (TheCamera.GetPosition() - ped->GetPosition()).SquaredMagnitude();
}

// NOTSA
CTaskSimpleIKManager* GetPedIKManagerTask(CPed* ped, bool createIfNotExists = false) {
    if (const auto mgr = static_cast<CTaskSimpleIKManager*>(ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_IK))) {
        return mgr;
    }

    if (createIfNotExists) {
        auto taskIKMgr = new CTaskSimpleIKManager{};
        ped->GetTaskManager().SetTaskSecondary(taskIKMgr, TASK_SECONDARY_IK);
        return taskIKMgr;
    }

    return nullptr;
}

// NOTSA
CTaskSimpleIKLookAt* GetPedIKLookAtTask(CPed* ped) {
    if (const auto mgr = GetPedIKManagerTask(ped)) {
        return static_cast<CTaskSimpleIKLookAt*>(mgr->GetTaskAtSlot(eIKChainSlot::LOOK_AT));
    }
    return nullptr;
}

// 0x6181A0
bool IKChainManager_c::IsLooking(CPed* ped) const {
    return !!GetPedIKLookAtTask(ped);
}

// 0x6181D0
CEntity* IKChainManager_c::GetLookAtEntity(CPed* ped) {
    if (const auto task = GetPedIKLookAtTask(ped)) {
        return task->GetLookAtEntity();
    }
    return nullptr;
}

// 0x618210
CVector IKChainManager_c::GetLookAtOffset(CPed* ped) {
    if (const auto task = GetPedIKLookAtTask(ped)) {
        return task->GetLookAtOffset();
    }
    return {};
}

// 0x618280
void IKChainManager_c::AbortLookAt(CPed* ped, uint32 blendOutTime) {
    if (const auto task = GetPedIKLookAtTask(ped)) {
        task->BlendOut(blendOutTime);
    }
}

// notsa
void IKChainManager_c::AbortLookAtIfLooking(CPed* ped, uint32 blendOutTime) {
    if (IsLooking(ped)) {
        AbortLookAt(ped, blendOutTime);
    }
}

// 0x6188B0
bool IKChainManager_c::CanAcceptLookAt(CPed* ped) {
    if (!CanAccept(ped, 20.f)) {
        return false;
    }

    if (!ped->bDontAcceptIKLookAts) {
        if (IsLooking(ped)) {
            AbortLookAt(ped);
        }
        return false;
    }

    if (ped->m_pedIK.bEverythingUsed) {
        return false;
    }

    return !RpAnimBlendClumpGetAssociation(ped->m_pRwClump, {
        ANIM_ID_DRNKBR_PRTL,
        ANIM_ID_SMKCIG_PRTL,
        ANIM_ID_DRNKBR_PRTL_F,
        ANIM_ID_SMKCIG_PRTL_F
    });
}

// 0x618970
void IKChainManager_c::LookAt(
    const char* purpose,
    CPed*       ped,
    CEntity*    lookAtEntity,
    int32       time,
    eBoneTag    offsetBone,
    CVector*    offset,
    bool        useTorso,
    float       fSpeed,
    int32       blendTime,
    int32       priority,
    bool        bForceLooking
) {
    if (!bForceLooking && !CanAcceptLookAt(ped)) { // byte_C1542C
        return;
    }

    auto& taskIKMgr = *GetPedIKManagerTask(ped, true);

    const auto lookAtOffset = offset ? *offset : CVector{};

    // Now, either update existing task or createIfNotExists one
    if (const auto tLookAt = CTask::Cast<CTaskSimpleIKLookAt>(taskIKMgr.GetTaskAtSlot(eIKChainSlot::LOOK_AT))) {
        if (priority < tLookAt->m_nPriority) {
            return;
        }
        if (useTorso || !tLookAt->m_bUseTorso) {
            tLookAt->UpdateLookAtInfo(
                purpose,
                ped,
                lookAtEntity,
                time,
                offsetBone,
                lookAtOffset,
                useTorso && tLookAt->m_bUseTorso,
                fSpeed,
                blendTime,
                priority
            );
        } else {
            AbortLookAt(ped);
        }
    } else { // Doesn't have task yet, createIfNotExists it
        taskIKMgr.AddIKChainTask(
            new CTaskSimpleIKLookAt{purpose, lookAtEntity, time, offsetBone, lookAtOffset, useTorso, fSpeed, blendTime, (int8)priority},
            eIKChainSlot::LOOK_AT
        );
    }
}

// 0x6182B0
bool __stdcall IKChainManager_c::IsArmPointing(eIKArm arm, CPed* ped) {
    const auto mgr = GetPedIKManagerTask(ped);
    return mgr && mgr->GetTaskAtSlot(IKArmToIKSlot(arm));
}

// 0x6182F0
void __stdcall IKChainManager_c::AbortPointArm(eIKArm arm, CPed* ped, int32 blendOutTime) {
    const auto mgr = GetPedIKManagerTask(ped);
    if (const auto lookAt = static_cast<CTaskSimpleIKChain*>(mgr->GetTaskAtSlot(IKArmToIKSlot(arm)))) {
        lookAt->BlendOut(blendOutTime);
    }
}

// notsa
void IKChainManager_c::AbortPointArmIfPointing(eIKArm arm, CPed* ped, int32 blendOutTime) {
    if (IsArmPointing(arm, ped)) {
        AbortPointArm(arm, ped, blendOutTime);
    }
}

// 0x618330
bool IKChainManager_c::IsFacingTarget(CPed* ped, eIKChainSlot slot) const {
    if (const auto mgr = GetPedIKManagerTask(ped)) {
        if (const auto task = mgr->GetTaskAtSlot(slot)) {
            return task->GetIKChain()->IsFacingTarget();
        }
    }
    return false;
}

// 0x618B60
void IKChainManager_c::PointArm(Const char* purpose, eIKArm pedArmId, CPed* ped, CEntity* lookAtEntity, eBoneTag offsetBoneTag, CVector* pOffset, float speed, int32 blendTime, float cullDist) const {
    if (!CanAccept(ped, cullDist)) {
        return;
    }

    const auto offset = pOffset ? *pOffset : CVector{};
    const auto tIKMgr = GetPedIKManagerTask(ped, true);

    // Now, either createIfNotExists or update existing
    if (const auto tPointArm = static_cast<CTaskSimpleIKPointArm*>(tIKMgr->GetTaskAtSlot(IKArmToIKSlot(pedArmId)))) {
        tPointArm->UpdatePointArmInfo(purpose, lookAtEntity, offsetBoneTag, offset, speed, blendTime);
    } else { // Create task
        tIKMgr->AddIKChainTask(new CTaskSimpleIKPointArm{purpose, pedArmId, lookAtEntity, offsetBoneTag, offset, speed, blendTime}, IKArmToIKSlot(pedArmId));
    }
}
