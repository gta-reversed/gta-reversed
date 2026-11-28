#include "StdInc.h"
#include "TaskComplexGoPickUpEntity.h"
#include "TaskSimplePickUpEntity.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include <numbers>

void CTaskComplexGoPickUpEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoPickUpEntity, 0x870b98, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6919C0, "1", {}, CEntity*, AssocGroupId);
    RH_ScopedDestructorInstall(0x691A50);

    RH_ScopedVMTInstall(Clone, 0x692C80);
    RH_ScopedVMTInstall(GetTaskType, 0x691A40);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x691AE0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x693610);
    RH_ScopedVMTInstall(ControlSubTask, 0x691D50);
}

// 0x6919C0
CTaskComplexGoPickUpEntity::CTaskComplexGoPickUpEntity(CEntity* entity, AssocGroupId animGroupId) :
    m_EntityToPickUp{ entity },
    m_AnimGroupId{ animGroupId } {
}

// 0x692C80
CTaskComplexGoPickUpEntity::CTaskComplexGoPickUpEntity(const CTaskComplexGoPickUpEntity& o) :
    CTaskComplexGoPickUpEntity{ o.m_EntityToPickUp, o.m_AnimGroupId } {
}

// 0x691A50
CTaskComplexGoPickUpEntity::~CTaskComplexGoPickUpEntity() {
    if (m_IsAnimBlockReferenced) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_AnimGroupId));
    }
}

// 0x691AE0
CTask* CTaskComplexGoPickUpEntity::CreateNextSubTask(CPed* ped) {
    if (!m_EntityToPickUp) {
        return nullptr;
    }

    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_PICKUP_ENTITY: { // 0x691CC3
        m_SubTaskStartTimeMs = CTimer::GetTimeInMS();
        if (!ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM)) {
            ped->GetTaskManager().SetTaskSecondary(
                new CTaskSimpleHoldEntity{ m_EntityToPickUp, &m_HoldPosition, PED_NODE_RIGHT_HAND, HOLD_ENTITY_FLAG_1, ANIM_ID_CRRY_PRTIAL, ANIM_GROUP_CARRY, false },
                TASK_SECONDARY_PARTIAL_ANIM
            );
        }
        return nullptr;
    }
    case TASK_SIMPLE_ACHIEVE_HEADING: { // 0x691BCC
        const auto CreatePickUpTask = [&](AssocGroupId animGroupId, float movePedUntilAnimProgress) {
            m_AnimGroupId           = animGroupId;
            auto* const tPickUp     = new CTaskSimplePickUpEntity{ m_EntityToPickUp, &m_HoldPosition, PED_NODE_RIGHT_HAND, HOLD_ENTITY_FLAG_1, ANIM_ID_LIFTUP, animGroupId, movePedUntilAnimProgress };
            tPickUp->m_vecPickuposn = m_PickUpPosition;
            return tPickUp;
        };
        if (m_AnimGroupId == ANIM_GROUP_CARRY) {
            const auto entityBottomZ = m_EntityToPickUp->GetColModel()->GetBoundingBox().m_vecMin.z + m_EntityToPickUp->GetPosition().z;
            if (entityBottomZ > ped->GetPosition().z) { // 0x691C12
                return CreatePickUpTask(ANIM_GROUP_CARRY105, 0.2f);
            }
            if (ped->GetPosition().z < entityBottomZ + 0.55f) { // 0x691C47
                return CreatePickUpTask(ANIM_GROUP_CARRY05, 0.26666668f);
            }
        }
        return CreatePickUpTask(m_AnimGroupId, 0.6f);
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: // 0x691B2E
        return new CTaskSimpleAchieveHeading{ (m_EntityToPickUp->GetPosition() - ped->GetPosition()).Heading(), 1.f, 0.001f };
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x693610
CTask* CTaskComplexGoPickUpEntity::CreateFirstSubTask(CPed* ped) {
    CEntity* const entity = m_EntityToPickUp;
    if (!entity) {
        return nullptr;
    }

    m_SubTaskStartTimeMs      = CTimer::GetTimeInMS();

    const CVector entityToPed = ped->GetPosition() - entity->GetPosition();
    const auto&   bb          = entity->GetModelInfo()->GetColModel()->GetBoundingBox();

    m_HoldPosition            = CVector{ -0.2f, -bb.m_vecMin.y - 0.2f, -bb.m_vecMin.z };

    // Turn the entity so the ped picks it up by its longer side
    if (bb.m_vecMax.x > 2.f * bb.m_vecMax.y) { // 0x6936BC
        entity->SetHeading(entityToPed.Heading());
    } else if (bb.m_vecMax.y > 2.f * bb.m_vecMax.x) { // 0x6936FD
        entity->SetHeading(entityToPed.Heading() - HALF_PI);
    } else if (entity->GetMatrix().GetUp().z < 0.9f) { // 0x69375C
        entity->SetHeading(entity->GetHeading());
    }

    if (entity->GetIsTypeObject()) { // 0x6937A7
        auto* const physical = entity->AsPhysical();
        physical->ResetMoveSpeed();
        physical->ResetTurnSpeed();
        physical->SetIsStatic(true);
    }
    entity->UpdateRwMatrix();

    // Walk to the side of the entity the ped is at
    const auto& mat      = entity->GetMatrix();
    const auto  dotRight = DotProduct(entityToPed, mat.GetRight());
    const auto  dotFwd   = DotProduct(entityToPed, mat.GetForward());
    if (dotRight > std::abs(dotFwd)) { // 0x6938C2
        m_PickUpPosition.x = bb.m_vecMax.x + 0.4f;
        m_PickUpPosition.y = 0.f;
    } else if (dotRight < -std::abs(dotFwd)) { // 0x6938E7
        m_PickUpPosition.x = bb.m_vecMin.x - 0.4f;
        m_PickUpPosition.y = 0.f;
    } else {
        m_PickUpPosition.x = 0.f;
        m_PickUpPosition.y = dotFwd > 0.f
            ? bb.m_vecMax.y + 0.4f
            : bb.m_vecMin.y - 0.4f;
    }
    m_PickUpPosition.z = bb.m_vecMin.z + 1.f;

    return new CTaskComplexGoToPointAndStandStill{ PEDMOVE_WALK, entity->GetMatrix().TransformPoint(m_PickUpPosition), 0.2f, 0.f, false, true };
}

// 0x691D50
CTask* CTaskComplexGoPickUpEntity::ControlSubTask(CPed* ped) {
    if (m_AnimGroupId != ANIM_GROUP_DEFAULT && !m_IsAnimBlockReferenced) {
        auto* animBlock = CAnimManager::GetAnimationBlock(m_AnimGroupId);
        if (!animBlock) {
            animBlock = CAnimManager::GetAnimationBlock(CAnimManager::GetAnimBlockName(m_AnimGroupId));
        }
        // NOTE: Not null checked (like the original, 0x691D86): the task is only made with `ANIM_GROUP_CARRY` (0x46C9C2, 0x570590),
        //       `CreateNextSubTask` turns it into `CARRY05`/`CARRY105` - all in the `carry` block, which is always registered
        const auto blockIndex = CAnimManager::GetAnimationBlockIndex(animBlock);
        if (animBlock->IsLoaded) {
            CAnimManager::AddAnimBlockRef(blockIndex);
            m_IsAnimBlockReferenced = true;
        } else {
            CStreaming::RequestModel(IFPToModelId(blockIndex), STREAMING_KEEP_IN_MEMORY);
        }
    }

    if (!m_pSubTask) {
        return nullptr;
    }

    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_PICKUP_ENTITY: { // 0x691F7C
        const auto* const tPickUp = notsa::cast<CTaskSimplePickUpEntity>(m_pSubTask);
        if (!tPickUp->m_pAnimBlendAssociation && !tPickUp->m_bEntityDropped && CTimer::GetTimeInMS() > m_SubTaskStartTimeMs + MAX_PICKUP_TIME) {
            m_pSubTask->MakeAbortable(ped); // NOTE: Returns null even if the sub task could not be aborted
            return nullptr;
        }
        break;
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // 0x691DDE
        if (CTimer::GetTimeInMS() > m_SubTaskStartTimeMs + MAX_GOTO_TIME) {
            m_pSubTask->MakeAbortable(ped);
            return nullptr;
        }
        if (!ped->AsPlayer()->GetPlayerData()) {
            break;
        }
        const auto* const pad = ped->AsPlayer()->GetPadFromPlayer();
        if (!pad) {
            break;
        }

        // The player cancels the walk by jumping, sprinting or pushing the stick away from the target
        const auto* const tGoTo         = notsa::cast<CTaskComplexGoToPointAndStandStill>(m_pSubTask);
        const float       walkUpDown    = pad->GetPedWalkUpDown();
        const float       walkLeftRight = pad->GetPedWalkLeftRight();
        const auto        stickMag      = CVector2D{ walkLeftRight, walkUpDown }.Magnitude() / 128.f;
        const auto        targetHeading = (tGoTo->m_vecTargetPoint - ped->GetPosition()).Heading();
        auto              stickHeading  = CGeneral::GetRadianAngleBetweenPoints(0.f, 0.f, -walkLeftRight, walkUpDown) - TheCamera.m_fOrientation;
        if (stickHeading > targetHeading + PI) {
            stickHeading -= TWO_PI;
        } else if (stickHeading < targetHeading - PI) {
            stickHeading += TWO_PI;
        }
        const auto headingDiff = stickHeading - targetHeading;

        // NOTE/BUG: The original does `abs(diff > PI / 4)`, so only one side counts
        const auto isStickAway = notsa::bugfixes::GenericUB
            ? std::abs(headingDiff) > FRAC_PI_4
            : headingDiff > FRAC_PI_4;
        if (pad->JumpJustDown() || pad->SprintJustDown() || (stickMag > 0.75f && isStickAway)) { // 0x691F63
            m_pSubTask->MakeAbortable(ped);
            return nullptr;
        }
        break;
    }
    case TASK_SIMPLE_ACHIEVE_HEADING: // 0x691FA6
        break;
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
    return m_pSubTask;
}
