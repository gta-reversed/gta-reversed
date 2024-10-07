#include "StdInc.h"

#include "TaskSimpleHoldEntity.h"

#include "TaskSimplePickUpEntity.h"
#include "TaskSimplePutDownEntity.h"

void CTaskSimpleHoldEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHoldEntity, 0x870B2C, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedOverloadedInstall(Constructor, "1", 0x6913A0, CTaskSimpleHoldEntity*(CTaskSimpleHoldEntity::*)(CEntity*, CVector*, uint8, uint8, AnimationId, AssocGroupId, bool));
    RH_ScopedOverloadedInstall(Constructor, "2", 0x691470, CTaskSimpleHoldEntity * (CTaskSimpleHoldEntity::*)(CEntity*, CVector*, uint8, uint8, const char*, const char*, eAnimationFlags));
    RH_ScopedOverloadedInstall(Constructor, "3", 0x691550, CTaskSimpleHoldEntity * (CTaskSimpleHoldEntity::*)(CEntity*, CVector*, uint8, uint8, CAnimBlock*, CAnimBlendHierarchy*, eAnimationFlags));
    RH_ScopedVMTInstall(Clone, 0x6929B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x693BD0);
    RH_ScopedVMTInstall(ProcessPed, 0x693C40);
    RH_ScopedVMTInstall(SetPedPosition, 0x6940A0);
    RH_ScopedInstall(ReleaseEntity, 0x6916E0);
    RH_ScopedInstall(CanThrowEntity, 0x691700);
    RH_ScopedInstall(PlayAnim, 0x691720);
    RH_ScopedInstall(FinishAnimHoldEntityCB, 0x691740);
    RH_ScopedInstall(StartAnim, 0x692FF0);
    RH_ScopedInstall(DropEntity, 0x6930F0);
    RH_ScopedInstall(ChoosePutDownHeight, 0x693440);
}
CTaskSimpleHoldEntity* CTaskSimpleHoldEntity::Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, AnimationId animId, AssocGroupId groupId, bool bDisAllowDroppingOnAnimEnd) { this->CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, animId, groupId, bDisAllowDroppingOnAnimEnd); return this; }
CTaskSimpleHoldEntity* CTaskSimpleHoldEntity::Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, const char* animName, const char* animBlockName, eAnimationFlags animFlags) { this->CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, animName, animBlockName, animFlags); return this; }
CTaskSimpleHoldEntity* CTaskSimpleHoldEntity::Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags) { this->CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, animBlock, animHierarchy, animFlags); return this; }

// 0x6913A0
CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(
    CEntity* entityToHold,
    const CVector* posn,
    uint8 boneFrameId,
    uint8 boneFlags,
    AnimationId animId,
    AssocGroupId groupId,
    bool bDisAllowDroppingOnAnimEnd
)
    : CTaskSimple()
{
    m_pEntityToHold = entityToHold;
    m_vecPosition = CVector(0.0f, 0.0f, 0.0f);
    m_nBoneFrameId = boneFrameId;
    m_bBoneFlags = boneFlags;
    m_nAnimId = animId;
    m_nAnimGroupId = groupId;
    m_bDisallowDroppingOnAnimEnd = bDisAllowDroppingOnAnimEnd;
    m_pAnimBlock = nullptr;
    m_pAnimBlendHierarchy = nullptr;
    m_bEntityDropped = false;
    m_bEntityRequiresProcessing = true;
    m_pAnimBlendAssociation = nullptr;
    if (posn)
        m_vecPosition = *posn;
    if (m_pEntityToHold) {
        m_pEntityToHold->m_bStreamingDontDelete = true;
        m_pEntityToHold->RegisterReference(&m_pEntityToHold);
    }
}

CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(CEntity* entityToHold,
    const CVector& posn,
    uint8 boneFrameId,
    uint8 boneFlags,
    AnimationId animId,
    AssocGroupId groupId,
    bool bDisAllowDroppingOnAnimEnd
) :
    m_pEntityToHold{ entityToHold },
    m_vecPosition{ posn },
    m_nBoneFrameId{ boneFrameId },
    m_bBoneFlags{ boneFlags },
    m_nAnimId{ animId },
    m_nAnimGroupId{ groupId },
    m_bDisallowDroppingOnAnimEnd{ bDisAllowDroppingOnAnimEnd },
    m_pAnimBlock{},
    m_pAnimBlendHierarchy{},
    m_bEntityDropped{},
    m_bEntityRequiresProcessing{ true },
    m_pAnimBlendAssociation{}
{
    if (m_pEntityToHold) {
        m_pEntityToHold->m_bStreamingDontDelete = true;
        CEntity::RegisterReference(m_pEntityToHold);
    }
}

// 0x691470
CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, const char* animName, const char* animBlockName, eAnimationFlags animFlags) : CTaskSimple()
{
    m_pEntityToHold = entityToHold;
    m_vecPosition = CVector(0.0f, 0.0f, 0.0f);
    m_nBoneFrameId = boneFrameId;
    m_bBoneFlags = boneFlags;
    m_nAnimId = ANIM_ID_NO_ANIMATION_SET;
    m_nAnimGroupId = ANIM_GROUP_DEFAULT;
    m_bEntityDropped = false;
    m_bEntityRequiresProcessing = true;
    m_bDisallowDroppingOnAnimEnd = false;
    m_pAnimBlendAssociation = nullptr;
    if (posn)
        m_vecPosition = *posn;
    if (m_pEntityToHold) {
        m_pEntityToHold->m_bStreamingDontDelete = true;
        m_pEntityToHold->RegisterReference(&m_pEntityToHold);
    }
    m_animFlags = animFlags;
    m_pAnimBlock = CAnimManager::GetAnimationBlock(animBlockName);
    m_pAnimBlendHierarchy = CAnimManager::GetAnimation(animName, m_pAnimBlock);
    CAnimManager::AddAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_pAnimBlock));
}

// 0x691550
CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags) : CTaskSimple()
{
    m_pEntityToHold = entityToHold;
    m_vecPosition = CVector(0.0f, 0.0f, 0.0f);
    m_nBoneFrameId = boneFrameId;
    m_bBoneFlags = boneFlags;
    m_fRotation = 0.0f;
    m_nAnimId = ANIM_ID_NO_ANIMATION_SET;
    m_nAnimGroupId = ANIM_GROUP_DEFAULT;
    m_bEntityDropped = false;
    m_bEntityRequiresProcessing = true;
    m_bDisallowDroppingOnAnimEnd = false;
    m_pAnimBlendAssociation = nullptr;
    if (posn)
        m_vecPosition = *posn;
    if (m_pEntityToHold)
    {
        m_pEntityToHold->m_bStreamingDontDelete = true;
        m_pEntityToHold->RegisterReference(&m_pEntityToHold);
    }
    m_animFlags = animFlags;
    m_pAnimBlock = animBlock;
    m_pAnimBlendHierarchy = animHierarchy;
    CAnimManager::AddAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_pAnimBlock));
}

CTaskSimpleHoldEntity::~CTaskSimpleHoldEntity() {
    if (!m_bEntityRequiresProcessing && m_pEntityToHold) {
        m_pEntityToHold->m_bStreamingDontDelete = false;
        m_pEntityToHold->CleanUpOldReference(&m_pEntityToHold);
        m_pEntityToHold->m_bRemoveFromWorld = true;
        m_pEntityToHold = nullptr;
    }
    if (m_pAnimBlendAssociation) {
        m_pAnimBlendAssociation->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
    if (m_pAnimBlock) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_pAnimBlock));
    }
}

// 0x6929B0
CTask* CTaskSimpleHoldEntity::Clone() const {
    if (m_pAnimBlendHierarchy)
        return new CTaskSimpleHoldEntity(m_pEntityToHold, &m_vecPosition, m_nBoneFrameId, m_bBoneFlags, m_pAnimBlock, m_pAnimBlendHierarchy, static_cast<eAnimationFlags>(m_animFlags));
    else
        return new CTaskSimpleHoldEntity(m_pEntityToHold, &m_vecPosition, m_nBoneFrameId, m_bBoneFlags, m_nAnimId, m_nAnimGroupId, false);
}

// 0x693BD0
bool CTaskSimpleHoldEntity::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_pAnimBlendAssociation) {
            m_pAnimBlendAssociation->m_BlendDelta = -4.0f;
            m_pAnimBlendAssociation->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_pAnimBlendAssociation = nullptr;
        }
        DropEntity(ped, true);
        m_bEntityDropped = true;
        return true;
    }
    else {
        if (m_pAnimBlendAssociation)
            m_pAnimBlendAssociation->m_BlendDelta = -4.0f;
        return false;
    }
}

// 0x693C40
bool CTaskSimpleHoldEntity::ProcessPed(CPed* ped) {
    CTaskManager* taskManager = &ped->GetTaskManager();
    if (m_bEntityRequiresProcessing) {
        m_bEntityRequiresProcessing = false;
        if (GetTaskType() == TASK_SIMPLE_PUTDOWN_ENTITY && !m_pEntityToHold && !m_pAnimBlendHierarchy && !m_nAnimGroupId) {
            auto holdEntityTask = (CTaskSimpleHoldEntity*)taskManager->GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM);
            if (holdEntityTask && holdEntityTask->GetTaskType() == TASK_SIMPLE_HOLD_ENTITY &&
                holdEntityTask->m_pEntityToHold && holdEntityTask->m_nAnimGroupId)
            {
                CEntity* entityToHold = holdEntityTask->m_pEntityToHold;
                m_pEntityToHold = entityToHold;
                entityToHold->RegisterReference(&m_pEntityToHold);

                m_vecPosition = holdEntityTask->m_vecPosition;
                m_nAnimId = ANIM_ID_PUTDWN;
                m_nAnimGroupId = holdEntityTask->m_nAnimGroupId;
                ChoosePutDownHeight(ped);
                holdEntityTask->ReleaseEntity();
            }
            else {
                m_bEntityDropped = true;
            }
        }
        CEntity* entityToHold = m_pEntityToHold;
        if (!entityToHold || m_bEntityDropped) {
            m_bEntityDropped = true;
        }
        else {
            entityToHold->m_bUsesCollision = false;
            if (entityToHold->IsObject()) {
                auto* objectToHold = entityToHold->AsObject();
                if (objectToHold->IsStatic()) {
                    objectToHold->SetIsStatic(false);
                    objectToHold->AddToMovingList();
                }
                objectToHold->physicalFlags.bAttachedToEntity = true;
                objectToHold->m_nFakePhysics = 0;
            }

            m_fRotation = entityToHold->GetHeading() - ped->m_fCurrentRotation;
        }
    }

    if (m_bEntityDropped)
        return true;

    if (m_nAnimId != ANIM_ID_NO_ANIMATION_SET || m_pAnimBlendHierarchy) {
        if (m_pAnimBlendAssociation) {
            if (m_pAnimBlendAssociation->m_BlendDelta < 0.0f && !m_bDisallowDroppingOnAnimEnd)
                DropEntity(ped, true);
        }
        else {
            StartAnim(ped);
        }
    }

    if (m_pEntityToHold) {
        CEntity* entityToHold = m_pEntityToHold;
        eTaskType taskId = GetTaskType();
        if (taskId != TASK_SIMPLE_HOLD_ENTITY) {
            if (taskId != TASK_SIMPLE_PICKUP_ENTITY) {
                auto* taskHoldEntity = static_cast<CTaskSimpleHoldEntity*>(taskManager->GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM));
                if (taskHoldEntity && taskHoldEntity->GetTaskType() == TASK_SIMPLE_HOLD_ENTITY)
                    taskHoldEntity->MakeAbortable(ped);
            }
            else
            {
                auto* taskPickUpEntity = static_cast<CTaskSimplePickUpEntity*>(this);
                CAnimBlendAssociation* animAssoc = taskPickUpEntity->m_pAnimBlendAssociation;
                if ((!animAssoc || taskPickUpEntity->m_fMovePedUntilAnimProgress > animAssoc->m_CurrentTime)
                    && (taskPickUpEntity->m_vecPickuposn.x != 0.0f || taskPickUpEntity->m_vecPickuposn.y != 0.0f))
                {
                    CVector outPoint = entityToHold->GetMatrix().TransformPoint(taskPickUpEntity->m_vecPickuposn);
                    outPoint -= ped->GetPosition();
                    ped->m_vecAnimMovingShiftLocal.x += DotProduct(&outPoint, &ped->GetRight()) / CTimer::GetTimeStep() * 0.1f;
                    ped->m_vecAnimMovingShiftLocal.y += DotProduct(&outPoint, &ped->GetForward()) / CTimer::GetTimeStep() * 0.1f ;
                    CVector direction = entityToHold->GetPosition() - ped->GetPosition();
                    ped->m_fAimingRotation = atan2(-direction.x, direction.y);
                }
            }
        }
        else {
            if ((m_nAnimGroupId == ANIM_GROUP_CARRY || m_nAnimGroupId == ANIM_GROUP_CARRY05 || m_nAnimGroupId == ANIM_GROUP_CARRY105) && ped->IsPlayer()) {
                CPad* pad = ped->AsPlayer()->GetPadFromPlayer();
                if (pad->ExitVehicleJustDown()) {
                    auto pTaskSimplePutDownEntity = new CTaskSimplePutDownEntity();
                    CEventScriptCommand eventScriptCommand(TASK_PRIMARY_PRIMARY, pTaskSimplePutDownEntity, false);
                    ped->GetEventGroup().Add(&eventScriptCommand, false);
                }
            }
        }
    }
    return false;
}

// 0x6940A0
bool CTaskSimpleHoldEntity::SetPedPosition(CPed* ped) {
    bool bUpdateEntityToHoldPosition = false;
    if (GetTaskType() == TASK_SIMPLE_PICKUP_ENTITY)
    {
        auto pickupEntityTask = static_cast<CTaskSimplePickUpEntity*>(this);
        if (!m_pAnimBlendAssociation || pickupEntityTask->m_fMovePedUntilAnimProgress > m_pAnimBlendAssociation->m_CurrentTime)
            return false;
        bUpdateEntityToHoldPosition = true;
    }
    else {
        auto pPutDownEntityTask = static_cast<CTaskSimplePutDownEntity*>(this);
        if (GetTaskType() != TASK_SIMPLE_PUTDOWN_ENTITY || m_bEntityRequiresProcessing
            || m_pAnimBlendAssociation
            && pPutDownEntityTask->m_fPutDownHeightZ >= m_pAnimBlendAssociation->m_CurrentTime
        ) {
            bUpdateEntityToHoldPosition = true;
        }
    }

    if (bUpdateEntityToHoldPosition) {
        if (m_pEntityToHold) {
            m_pEntityToHold->m_bIsVisible = ped->m_bIsVisible;
            if (ped->bCalledPreRender) {
                if (m_bBoneFlags & HOLD_ENTITY_UPDATE_TRANSLATION_ONLY) {
                    CVector entityToHoldPos = m_vecPosition;
                    RpHAnimHierarchy* pHAnimHierarchy = GetAnimHierarchyFromSkinClump(ped->m_pRwClump);
                    int32 animIndex = RpHAnimIDGetIndex(pHAnimHierarchy, ped->m_apBones[m_nBoneFrameId]->BoneTag);
                    RwMatrix* pBoneMatrix = &RpHAnimHierarchyGetMatrixArray(pHAnimHierarchy)[animIndex];
                    RwV3dTransformPoints((RwV3d*)&entityToHoldPos, (RwV3d*)&entityToHoldPos, 1, pBoneMatrix);
                    m_pEntityToHold->GetMatrix().UpdateMatrix(pBoneMatrix);
                    m_pEntityToHold->SetPosn(entityToHoldPos);
                }
                else {
                    CVector entityToHoldPos = ped->GetMatrix().TransformVector(m_vecPosition);
                    RpHAnimHierarchy* pHAnimHierarchy = GetAnimHierarchyFromSkinClump(ped->m_pRwClump);
                    int32 animIndex = RpHAnimIDGetIndex(pHAnimHierarchy, ped->m_apBones[m_nBoneFrameId]->BoneTag);
                    RwMatrix* pBoneMatrix = RpHAnimHierarchyGetMatrixArray(pHAnimHierarchy);
                    entityToHoldPos += *RwMatrixGetPos(&pBoneMatrix[animIndex]);
                    CMatrix rotationMatrix(ped->GetMatrix());
                    rotationMatrix.RotateZ(m_fRotation);
                    *static_cast<CMatrix*>(&m_pEntityToHold->GetMatrix()) = rotationMatrix;
                    m_pEntityToHold->SetPosn(entityToHoldPos);
                }
            }
            else {
                if (ped->m_matrix)
                    m_pEntityToHold->SetPosn(ped->GetPosition());
                else
                    m_pEntityToHold->SetPosn(ped->m_placement.m_vPosn);
            }
            m_pEntityToHold->UpdateRW();
            m_pEntityToHold->UpdateRwFrame();
            return true;
        }
        else {
            MakeAbortable(ped);
            return false;
        }
        return false;
    }
    if (m_pEntityToHold)
        DropEntity(ped, false);
    return false;
}

// 0x6916E0
void CTaskSimpleHoldEntity::ReleaseEntity() {
    CEntity::ClearReference(m_pEntityToHold);
}

// 0x691700
bool CTaskSimpleHoldEntity::CanThrowEntity() const {
    return    m_pEntityToHold
           && m_bBoneFlags == HOLD_ENTITY_UPDATE_TRANSLATION_ONLY
           && m_nBoneFrameId == PED_NODE_RIGHT_HAND;
}

// 0x691720
void CTaskSimpleHoldEntity::PlayAnim(AnimationId animId, AssocGroupId grpId) {
    m_nAnimId = animId;
    m_nAnimGroupId = grpId;
}

// 0x691740
void CTaskSimpleHoldEntity::FinishAnimHoldEntityCB(CAnimBlendAssociation* animAssoc, void* data) {
    auto taskHoldEntity = reinterpret_cast<CTaskSimpleHoldEntity*>(data);
    if (taskHoldEntity->m_bDisallowDroppingOnAnimEnd) {
        taskHoldEntity->m_nAnimId = ANIM_ID_NO_ANIMATION_SET;
        taskHoldEntity->m_nAnimGroupId = ANIM_GROUP_DEFAULT;
        taskHoldEntity->m_pAnimBlendAssociation = nullptr;
    }
    else
    {
        if (taskHoldEntity->GetTaskType() == TASK_SIMPLE_PICKUP_ENTITY && animAssoc->m_BlendAmount > 0.0f) {
            CEntity::ClearReference(taskHoldEntity->m_pEntityToHold);
        }
        taskHoldEntity->m_bEntityDropped = true;
        taskHoldEntity->m_pAnimBlendAssociation = nullptr;
    }
}

// 0x692FF0
void CTaskSimpleHoldEntity::StartAnim(CPed* ped) {
    if (m_pAnimBlendHierarchy) {
        m_animFlags |= ANIMATION_DONT_ADD_TO_PARTIAL_BLEND | ANIMATION_IS_BLEND_AUTO_REMOVE | ANIMATION_IS_PARTIAL;
        m_pAnimBlendAssociation = CAnimManager::BlendAnimation(ped->m_pRwClump, m_pAnimBlendHierarchy, m_animFlags, 4.0f);
    } else {
        if (m_nAnimGroupId && !m_pAnimBlock) {
            CAnimBlock* animBlock = CAnimManager::GetAnimationBlock(m_nAnimGroupId);
            if (!animBlock) {
                animBlock = CAnimManager::GetAnimationBlock(CAnimManager::GetAnimBlockName(m_nAnimGroupId));
            }
            const auto blockIndex = CAnimManager::GetAnimationBlockIndex(animBlock);
            if (!animBlock->IsLoaded) {
                CStreaming::RequestModel(IFPToModelId(blockIndex), STREAMING_KEEP_IN_MEMORY);
                return;
            }
            CAnimManager::AddAnimBlockRef(blockIndex);
            m_pAnimBlock = animBlock;
        }
        m_pAnimBlendAssociation = CAnimManager::BlendAnimation(ped->m_pRwClump, m_nAnimGroupId, m_nAnimId, 4.0f);
        m_pAnimBlendAssociation->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        if (GetTaskType() == TASK_SIMPLE_HOLD_ENTITY) {
            m_pAnimBlendAssociation->m_Flags |= ANIMATION_DONT_ADD_TO_PARTIAL_BLEND;
        }
    }

    if (GetTaskType() == TASK_SIMPLE_PICKUP_ENTITY)
        m_pAnimBlendAssociation->SetFinishCallback(CTaskSimpleHoldEntity::FinishAnimHoldEntityCB, this);
    else
        m_pAnimBlendAssociation->SetDeleteCallback(CTaskSimpleHoldEntity::FinishAnimHoldEntityCB, this);
}

// 0x6930F0
void CTaskSimpleHoldEntity::DropEntity(CPed* ped, bool bAddEventSoundQuiet) {
    bool bUpdateEntityPosition = true;
    CObject* objectToHold = nullptr;
    if (m_pEntityToHold) {
        m_pEntityToHold->m_bUsesCollision = true;
        if (!m_pEntityToHold->IsObject()) {
            CEntity::ClearReference(m_pEntityToHold);
            return;
        }
        objectToHold = m_pEntityToHold->AsObject();
        objectToHold->m_pEntityIgnoredCollision = ped;
        if (objectToHold->physicalFlags.bDisableCollisionForce && bAddEventSoundQuiet) {
            if (!objectToHold->objectFlags.bIsLiftable) {
                uint8 objectType = objectToHold->m_nObjectType;
                if (objectType != OBJECT_MISSION && objectType != OBJECT_MISSION2) {
                    if (objectType != OBJECT_TEMPORARY) {
                        ++CObject::nNoTempObjects;
                    }
                    objectToHold->m_nObjectType = OBJECT_TEMPORARY;
                    objectToHold->m_nRemovalTime = 0;
                    objectToHold->m_bUsesCollision = false;
                    objectToHold->m_bIsVisible = false;
                    bUpdateEntityPosition = false;
                }
            }
        }
        else {
            objectToHold->physicalFlags.bAttachedToEntity = false;
            if (!bAddEventSoundQuiet) {
                bUpdateEntityPosition = false;
            }
            else {
                if (objectToHold->IsStatic()) {
                    objectToHold->SetIsStatic(false);
                    objectToHold->AddToMovingList();
                }
                auto* entity = m_pEntityToHold->AsPhysical();
                entity->m_vecMoveSpeed += CGeneral::GetRandomNumberInRange(+0.03f, 0.06f) * ped->GetForward();
                entity->m_vecMoveSpeed += CGeneral::GetRandomNumberInRange(-0.03f, 0.03f) * ped->GetRight();
                entity->m_vecMoveSpeed += 0.01f * ped->GetUp();
                if (!objectToHold->TestCollision(false)) {
                    bUpdateEntityPosition = false;
                }
            }
        }

        CVector objectToHoldPosition(0.0f, 0.0f, 0.0f);
        if (objectToHold->objectFlags.bIsLiftable && ped->m_pPlayerData && bAddEventSoundQuiet) {
            CEventSoundQuiet eventSoundQuiet(ped, 60.0f, -1, objectToHoldPosition);
            GetEventGlobalGroup()->Add(&eventSoundQuiet, false);
        }

        if (bUpdateEntityPosition) {
            objectToHold->ResetMoveSpeed();
            objectToHold->SetIsStatic(true);
            objectToHold->RemoveFromMovingList();
            objectToHoldPosition = ped->GetPosition();
            objectToHoldPosition.z = objectToHoldPosition.z - 1.0f - objectToHold->GetColModel()->m_boundBox.m_vecMin.z;
            objectToHold->SetPosn(objectToHoldPosition);
            objectToHold->UpdateRW();
            objectToHold->UpdateRwFrame();
        }
        CEntity::ClearReference(m_pEntityToHold);
        return;
    }
}

// 0x693440
void CTaskSimpleHoldEntity::ChoosePutDownHeight(CPed* ped) {
    CVector origin = (ped->GetForward() * 0.65f) + ped->GetPosition();
    origin.z += 0.2f;
    float distance = origin.z - 1.5f;
    CEntity* outEntity = nullptr;
    CColPoint colPoint{};
    if (!CWorld::ProcessVerticalLine(origin, distance, colPoint, outEntity, true, false, false, true, false, false, nullptr)) {
        m_nAnimGroupId = ANIM_GROUP_CARRY;
    }
    else {
        if (ped->GetPosition().z - 0.2f <= colPoint.m_vecPoint.z) {
            m_nAnimGroupId = ANIM_GROUP_CARRY105;
        }
        else {
            if (ped->GetPosition().z - 0.7f <= colPoint.m_vecPoint.z)
                m_nAnimGroupId = ANIM_GROUP_CARRY05;
            else
                m_nAnimGroupId = ANIM_GROUP_CARRY;
        }
    }
    if (GetTaskType() == TASK_SIMPLE_PUTDOWN_ENTITY) {
        auto taskPutDownEntity = static_cast<CTaskSimplePutDownEntity*>(this);
        if (m_nAnimGroupId == ANIM_GROUP_CARRY105)
            taskPutDownEntity->m_fPutDownHeightZ = 0.433333f; // todo: math
        else if (m_nAnimGroupId == ANIM_GROUP_CARRY05)
            taskPutDownEntity->m_fPutDownHeightZ = 0.366667f;
        else
            taskPutDownEntity->m_fPutDownHeightZ = 0.6f;
    }
}
