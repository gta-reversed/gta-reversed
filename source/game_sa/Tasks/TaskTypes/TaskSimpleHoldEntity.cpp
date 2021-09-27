#include "StdInc.h"

#include "TaskSimpleHoldEntity.h"

#include "TaskSimplePickUpEntity.h"
#include "TaskSimplePutDownEntity.h"

void CTaskSimpleHoldEntity::InjectHooks() {
    using namespace ReversibleHooks;
    Install("CTaskSimpleHoldEntity", "Constructor_1", 0x6913A0, (CTaskSimpleHoldEntity*(CTaskSimpleHoldEntity::*)(CEntity*, CVector*, uint8, uint8, AnimationId, AssocGroupId, bool)) & CTaskSimpleHoldEntity::Constructor);
    Install("CTaskSimpleHoldEntity", "Constructor_2", 0x691470, (CTaskSimpleHoldEntity * (CTaskSimpleHoldEntity::*)(CEntity*, CVector*, uint8, uint8, const char*, const char*, eAnimationFlags)) & CTaskSimpleHoldEntity::Constructor);
    Install("CTaskSimpleHoldEntity", "Constructor_3", 0x691550, (CTaskSimpleHoldEntity * (CTaskSimpleHoldEntity::*)(CEntity*, CVector*, uint8, uint8, CAnimBlock*, CAnimBlendHierarchy*, eAnimationFlags)) & CTaskSimpleHoldEntity::Constructor);
    Install("CTaskSimpleHoldEntity", "Clone", 0x6929B0, &CTaskSimpleHoldEntity::Clone_Reversed);
    Install("CTaskSimpleHoldEntity", "GetTaskType", 0x691460, &CTaskSimpleHoldEntity::GetId_Reversed);
    Install("CTaskSimpleHoldEntity", "MakeAbortable", 0x693BD0, &CTaskSimpleHoldEntity::MakeAbortable_Reversed);
    Install("CTaskSimpleHoldEntity", "ProcessPed", 0x693C40, &CTaskSimpleHoldEntity::ProcessPed_Reversed);
    Install("CTaskSimpleHoldEntity", "SetPedPosition", 0x6940A0, &CTaskSimpleHoldEntity::SetPedPosition_Reversed);
    Install("CTaskSimpleHoldEntity", "ReleaseEntity", 0x6916E0, &CTaskSimpleHoldEntity::ReleaseEntity);
    Install("CTaskSimpleHoldEntity", "CanThrowEntity", 0x691700, &CTaskSimpleHoldEntity::CanThrowEntity);
    Install("CTaskSimpleHoldEntity", "PlayAnim", 0x691720, &CTaskSimpleHoldEntity::PlayAnim);
    Install("CTaskSimpleHoldEntity", "FinishAnimHoldEntityCB", 0x691740, &CTaskSimpleHoldEntity::FinishAnimHoldEntityCB);
    Install("CTaskSimpleHoldEntity", "StartAnim", 0x692FF0, &CTaskSimpleHoldEntity::StartAnim);
    Install("CTaskSimpleHoldEntity", "DropEntity", 0x6930F0, &CTaskSimpleHoldEntity::DropEntity);
    Install("CTaskSimpleHoldEntity", "ChoosePutDownHeight", 0x693440, &CTaskSimpleHoldEntity::ChoosePutDownHeight);
}

// 0x6913A0
CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, AnimationId animId, AssocGroupId groupId, bool bDisAllowDroppingOnAnimEnd) : CTaskSimple()
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

// 0x691470
CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, const char* animName, const char* animBlockName, eAnimationFlags animFlags) : CTaskSimple()
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
    CAnimManager::AddAnimBlockRef(m_pAnimBlock - CAnimManager::ms_aAnimBlocks);
}

// 0x691550
CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags) : CTaskSimple()
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
    CAnimManager::AddAnimBlockRef(animBlock - CAnimManager::ms_aAnimBlocks);
}

CTaskSimpleHoldEntity::~CTaskSimpleHoldEntity() {
    if (!m_bEntityRequiresProcessing) {
        if (m_pEntityToHold) {
            m_pEntityToHold->m_bStreamingDontDelete = false;
            m_pEntityToHold->CleanUpOldReference(&m_pEntityToHold);
            m_pEntityToHold->m_bRemoveFromWorld = true;
            m_pEntityToHold = nullptr;
        }
    }
    if (m_pAnimBlendAssociation)
        m_pAnimBlendAssociation->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    if (m_pAnimBlock)
        CAnimManager::RemoveAnimBlockRef(m_pAnimBlock - CAnimManager::ms_aAnimBlocks);
}

CTaskSimpleHoldEntity* CTaskSimpleHoldEntity::Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, AnimationId animId, AssocGroupId groupId, bool bDisAllowDroppingOnAnimEnd)
{
    this->CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, animId, groupId, bDisAllowDroppingOnAnimEnd);
    return this;
}

CTaskSimpleHoldEntity* CTaskSimpleHoldEntity::Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, const char* animName, const char* animBlockName, eAnimationFlags animFlags)
{
    this->CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, animName, animBlockName, animFlags);
    return this;
}

CTaskSimpleHoldEntity* CTaskSimpleHoldEntity::Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags)
{
    this->CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, animBlock, animHierarchy, animFlags);
    return this;
}

// 0x6929B0
CTask* CTaskSimpleHoldEntity::Clone() {
    return  CTaskSimpleHoldEntity::Clone_Reversed();
}

// 0x691460
eTaskType CTaskSimpleHoldEntity::GetTaskType() {
    return GetId_Reversed();
}

// 0x693BD0
bool CTaskSimpleHoldEntity::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return CTaskSimpleHoldEntity::MakeAbortable_Reversed(ped, priority, event);
}

// 0x693C40
bool CTaskSimpleHoldEntity::ProcessPed(CPed* ped) {
    return ProcessPed_Reversed(ped);
}

// 0x6940A0
bool CTaskSimpleHoldEntity::SetPedPosition(CPed* ped) {
    return CTaskSimpleHoldEntity::SetPedPosition_Reversed(ped);
}

CTask* CTaskSimpleHoldEntity::Clone_Reversed() {
    if (m_pAnimBlendHierarchy)
        return new CTaskSimpleHoldEntity(m_pEntityToHold, &m_vecPosition, m_nBoneFrameId, m_bBoneFlags, m_pAnimBlock, m_pAnimBlendHierarchy, static_cast<eAnimationFlags>(m_animFlags));
    else
        return new CTaskSimpleHoldEntity(m_pEntityToHold, &m_vecPosition, m_nBoneFrameId, m_bBoneFlags, m_nAnimId, m_nAnimGroupId, false);
}

bool CTaskSimpleHoldEntity::MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_pAnimBlendAssociation) {
            m_pAnimBlendAssociation->m_fBlendDelta = -4.0f;
            m_pAnimBlendAssociation->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_pAnimBlendAssociation = nullptr;
        }
        DropEntity(ped, true);
        m_bEntityDropped = true;
        return true;
    }
    else {
        if (m_pAnimBlendAssociation)
            m_pAnimBlendAssociation->m_fBlendDelta = -4.0f;
        return false;
    }
}

bool CTaskSimpleHoldEntity::ProcessPed_Reversed(CPed* ped) {
    CTaskManager* pTaskManager = &ped->GetTaskManager();
    if (m_bEntityRequiresProcessing) {
        m_bEntityRequiresProcessing = false;
        if (GetTaskType() == TASK_SIMPLE_PUTDOWN_ENTITY && !m_pEntityToHold && !m_pAnimBlendHierarchy && !m_nAnimGroupId) {
            auto pHoldEntityTask = (CTaskSimpleHoldEntity*)pTaskManager->GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM);
            if (pHoldEntityTask && pHoldEntityTask->GetTaskType() == TASK_SIMPLE_HOLD_ENTITY &&
                pHoldEntityTask->m_pEntityToHold && pHoldEntityTask->m_nAnimGroupId)
            {
                CEntity* entitToHold = pHoldEntityTask->m_pEntityToHold;
                m_pEntityToHold = entitToHold;
                entitToHold->RegisterReference(&m_pEntityToHold);

                m_vecPosition = pHoldEntityTask->m_vecPosition;
                m_nAnimId = ANIM_ID_PUTDWN;
                m_nAnimGroupId = pHoldEntityTask->m_nAnimGroupId;
                ChoosePutDownHeight(ped);
                pHoldEntityTask->ReleaseEntity();
            }
            else {
                m_bEntityDropped = true;
            }
        }
        CEntity* entitToHold = m_pEntityToHold;
        if (!entitToHold || m_bEntityDropped) {
            m_bEntityDropped = true;
        }
        else {
            entitToHold->m_bUsesCollision = false;
            if (entitToHold->m_nType == ENTITY_TYPE_OBJECT) {
                CObject* objectToHold = (CObject*)entitToHold;
                if (objectToHold->IsStatic()) {
                    objectToHold->SetIsStatic(false);
                    objectToHold->AddToMovingList();
                }
                objectToHold->physicalFlags.bAttachedToEntity = true;
                objectToHold->m_nFakePhysics = 0;
            }

            m_fRotation = entitToHold->GetHeading() - ped->m_fCurrentRotation;
        }
    }

    if (m_bEntityDropped)
        return true;

    if (m_nAnimId != ANIM_ID_NO_ANIMATION_SET || m_pAnimBlendHierarchy) {
        auto pAnimBlendAssoc = m_pAnimBlendAssociation;
        if (pAnimBlendAssoc) {
            if (pAnimBlendAssoc->m_fBlendDelta < 0.0f && !m_bDisallowDroppingOnAnimEnd)
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
                auto* taskHoldEntity = static_cast<CTaskSimpleHoldEntity*>(pTaskManager->GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM));
                if (taskHoldEntity && taskHoldEntity->GetTaskType() == TASK_SIMPLE_HOLD_ENTITY)
                    taskHoldEntity->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
            }
            else
            {
                auto* taskPickUpEntity = static_cast<CTaskSimplePickUpEntity*>(this);
                CAnimBlendAssociation* animAssoc = taskPickUpEntity->m_pAnimBlendAssociation;
                if ((!animAssoc || taskPickUpEntity->m_fMovePedUntilAnimProgress > animAssoc->m_fCurrentTime)
                    && (taskPickUpEntity->m_vecPickuposn.x != 0.0f || taskPickUpEntity->m_vecPickuposn.y != 0.0f))
                {
                    CVector outPoint = entityToHold->GetMatrix() * taskPickUpEntity->m_vecPickuposn;
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
                CPlayerPed* player = (CPlayerPed*)ped;
                CPad* pad = player->GetPadFromPlayer();
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

bool CTaskSimpleHoldEntity::SetPedPosition_Reversed(CPed* ped) {
    bool bUpdateEntityToHoldPosition = false;
    if (GetTaskType() == TASK_SIMPLE_PICKUP_ENTITY)
    {
        auto pPickupEntityTask = static_cast<CTaskSimplePickUpEntity*>(this);
        if (!m_pAnimBlendAssociation || pPickupEntityTask->m_fMovePedUntilAnimProgress > m_pAnimBlendAssociation->m_fCurrentTime)
            return false;
        bUpdateEntityToHoldPosition = true;
    }
    else {
        auto pPutDownEntityTask = static_cast<CTaskSimplePutDownEntity*>(this);
        if (GetTaskType() != TASK_SIMPLE_PUTDOWN_ENTITY || m_bEntityRequiresProcessing
            || m_pAnimBlendAssociation && pPutDownEntityTask->m_fPutDownHeightZ >= m_pAnimBlendAssociation->m_fCurrentTime)
        {
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
                    int32 animIndex = RpHAnimIDGetIndex(pHAnimHierarchy, ped->m_apBones[m_nBoneFrameId]->m_nNodeId);
                    RwMatrix* pBoneMatrix = &RpHAnimHierarchyGetMatrixArray(pHAnimHierarchy)[animIndex];
                    RwV3dTransformPoints((RwV3d*)&entityToHoldPos, (RwV3d*)&entityToHoldPos, 1, pBoneMatrix);
                    m_pEntityToHold->GetMatrix().UpdateMatrix(pBoneMatrix);
                    m_pEntityToHold->SetPosn(entityToHoldPos);
                }
                else {
                    CVector entityToHoldPos = Multiply3x3(ped->GetMatrix(), m_vecPosition);
                    RpHAnimHierarchy* pHAnimHierarchy = GetAnimHierarchyFromSkinClump(ped->m_pRwClump);
                    int32 animIndex = RpHAnimIDGetIndex(pHAnimHierarchy, ped->m_apBones[m_nBoneFrameId]->m_nNodeId);
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
            MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
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
    if (m_pEntityToHold) {
        m_pEntityToHold->CleanUpOldReference(&m_pEntityToHold);
        m_pEntityToHold = nullptr;
    }
}

// 0x691700
bool CTaskSimpleHoldEntity::CanThrowEntity() {
    return    m_pEntityToHold
           && m_bBoneFlags == HOLD_ENTITY_UPDATE_TRANSLATION_ONLY
           && m_nBoneFrameId == PED_NODE_RIGHT_HAND;
}

// TODO: FIX THAT
// 0x691720
void CTaskSimpleHoldEntity::PlayAnim(AnimationId groupId, AssocGroupId animId) {
    m_nAnimId = groupId;
    m_nAnimGroupId = animId;
}

// 0x691740
void CTaskSimpleHoldEntity::FinishAnimHoldEntityCB(CAnimBlendAssociation* animAssoc, void* pData) {
    auto taskHoldEntity = reinterpret_cast<CTaskSimpleHoldEntity*>(pData);
    if (taskHoldEntity->m_bDisallowDroppingOnAnimEnd) {
        taskHoldEntity->m_nAnimId = ANIM_ID_NO_ANIMATION_SET;
        taskHoldEntity->m_nAnimGroupId = ANIM_GROUP_DEFAULT;
        taskHoldEntity->m_pAnimBlendAssociation = nullptr;
    }
    else
    {
        if (taskHoldEntity->GetTaskType() == TASK_SIMPLE_PICKUP_ENTITY && animAssoc->m_fBlendAmount > 0.0f) {
            CEntity* entityToHold = taskHoldEntity->m_pEntityToHold;
            if (entityToHold) {
                entityToHold->CleanUpOldReference(&taskHoldEntity->m_pEntityToHold);
                taskHoldEntity->m_pEntityToHold = nullptr;
            }
        }
        taskHoldEntity->m_bEntityDropped = true;
        taskHoldEntity->m_pAnimBlendAssociation = nullptr;
    }
}

// 0x692FF0
void CTaskSimpleHoldEntity::StartAnim(CPed* ped) {
    if (m_pAnimBlendHierarchy) {
        m_animFlags |= ANIM_FLAG_400 | ANIM_FLAG_FREEZE_LAST_FRAME | ANIM_FLAG_PARTIAL;
        m_pAnimBlendAssociation = CAnimManager::BlendAnimation(ped->m_pRwClump, m_pAnimBlendHierarchy, m_animFlags, 4.0f);
    }
    else {
        if (m_nAnimGroupId && !m_pAnimBlock) {
            CAnimBlock* animBlock = CAnimManager::ms_aAnimAssocGroups[m_nAnimGroupId].m_pAnimBlock;
            if (!animBlock)
                animBlock = CAnimManager::GetAnimationBlock(CAnimManager::GetAnimBlockName(m_nAnimGroupId));
            if (!animBlock->bLoaded) {
                CStreaming::RequestModel(animBlock - CAnimManager::ms_aAnimBlocks + RESOURCE_ID_IFP, STREAMING_KEEP_IN_MEMORY);
                return;
            }
            CAnimManager::AddAnimBlockRef(animBlock - CAnimManager::ms_aAnimBlocks);
            m_pAnimBlock = animBlock;
        }
        m_pAnimBlendAssociation = CAnimManager::BlendAnimation(ped->m_pRwClump, m_nAnimGroupId, m_nAnimId, 4.0f);
        m_pAnimBlendAssociation->m_nFlags |= ANIM_FLAG_FREEZE_LAST_FRAME;
        if (GetTaskType() == TASK_SIMPLE_HOLD_ENTITY)
            m_pAnimBlendAssociation->m_nFlags |= ANIM_FLAG_400;
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
        if (m_pEntityToHold->m_nType != ENTITY_TYPE_OBJECT) {
            m_pEntityToHold->CleanUpOldReference(&m_pEntityToHold);
            m_pEntityToHold = nullptr;
            return;
        }
        objectToHold = static_cast<CObject*>(m_pEntityToHold);
        objectToHold->m_pEntityIgnoredCollision = ped;
        if (objectToHold->physicalFlags.bDisableCollisionForce && bAddEventSoundQuiet) {
            if (!objectToHold->objectFlags.bIsLiftable) {
                uint8 objectType = objectToHold->m_nObjectType;
                if (objectType != OBJECT_MISSION && objectType != OBJECT_MISSION2) {
                    if (objectType != OBJECT_TEMPORARY)
                        ++CObject::nNoTempObjects;
                    objectToHold->m_nObjectType = OBJECT_TEMPORARY;
                    objectToHold->m_dwRemovalTime = 0;
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
                auto* physicalEntity = static_cast<CPhysical*>(m_pEntityToHold);
                const float randomSpeedUp = rand() * 4.6566e-10f * 0.03f + 0.03f;
                const float randomSpeedRight = rand() * 4.6566e-10f * 0.06f - 0.03f;
                physicalEntity->m_vecMoveSpeed += randomSpeedUp * ped->GetForward();
                physicalEntity->m_vecMoveSpeed += randomSpeedRight * ped->GetRight();
                physicalEntity->m_vecMoveSpeed += 0.01f * ped->GetUp();
                if (!objectToHold->TestCollision(false))
                    bUpdateEntityPosition = false;
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
        m_pEntityToHold->CleanUpOldReference(&m_pEntityToHold);
        m_pEntityToHold = nullptr;
        return;
    }
}

// 0x693440
void CTaskSimpleHoldEntity::ChoosePutDownHeight(CPed* ped) {
    CVector origin = (ped->GetForward() * 0.65f) + ped->GetPosition();
    origin.z += 0.2f;
    float distance = origin.z - 1.5f;
    CEntity* outEntity = nullptr;
    CColPoint colPoint;
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
        auto pTaskPutDownEntity = static_cast<CTaskSimplePutDownEntity*>(this);
        if (m_nAnimGroupId == ANIM_GROUP_CARRY105)
            pTaskPutDownEntity->m_fPutDownHeightZ = 0.433333f;
        else if (m_nAnimGroupId == ANIM_GROUP_CARRY05)
            pTaskPutDownEntity->m_fPutDownHeightZ = 0.366667f;
        else
            pTaskPutDownEntity->m_fPutDownHeightZ = 0.6f;
    }
}
