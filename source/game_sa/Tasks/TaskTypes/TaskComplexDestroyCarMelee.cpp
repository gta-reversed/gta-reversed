#include "StdInc.h"

#include "TaskComplexDestroyCarMelee.h"

void CTaskComplexDestroyCarMelee::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDestroyCarMelee, 0x86d994, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x621D10);
    RH_ScopedInstall(Destructor, 0x621DA0);

    RH_ScopedInstall(CreateSubTask, 0x628A70, { .reversed = false });
    RH_ScopedInstall(CalculateSearchPositionAndRanges, 0x6289F0);

    RH_ScopedVMTInstall(Clone, 0x6235A0);
    RH_ScopedVMTInstall(GetTaskType, 0x621D90);
    RH_ScopedVMTInstall(MakeAbortable, 0x621E00);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62DC20);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62DB20);
    RH_ScopedVMTInstall(ControlSubTask, 0x62DDB0);
}

// 0x621D10
CTaskComplexDestroyCarMelee::CTaskComplexDestroyCarMelee(CVehicle* vehToDestroy) :
    m_VehToDestroy{ vehToDestroy }
{
    CEntity::SafeRegisterRef(m_VehToDestroy);
}

// 0x621DA0
CTaskComplexDestroyCarMelee::~CTaskComplexDestroyCarMelee() {
    CEntity::SafeCleanUpRef(m_VehToDestroy);
}

// 0x621E00
bool CTaskComplexDestroyCarMelee::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE:
        return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);

    case ABORT_PRIORITY_URGENT: {
        if (event) {
            switch (event->GetEventType()) {
            case EVENT_VEHICLE_COLLISION: {
                if (!m_VehToDestroy || static_cast<const CEventVehicleCollision*>(event)->m_vehicle != m_VehToDestroy) {
                    break;
                }

                const auto vehVelocitySq = m_VehToDestroy->m_vecMoveSpeed.SquaredMagnitude();
                if (vehVelocitySq == 0.f) {
                    return false;
                }

                if (vehVelocitySq >= sq(0.15f)) {
                    break;
                }

                // Check if the vehicle's velocity is pointing at the ped
                if (DotProduct(
                        ped->GetPosition() - m_VehToDestroy->GetPosition(),
                        Normalized(m_VehToDestroy->m_vecMoveSpeed) // TODO: I don't think this needs to be normalized either, since we're only checking the direction
                ) > 0.f) {
                    return false;
                }

                break;
            }
            case EVENT_POTENTIAL_WALK_INTO_VEHICLE: {
                if (m_VehToDestroy && static_cast<const CEventPotentialWalkIntoVehicle*>(event)->m_vehicle == m_VehToDestroy) {
                    return false;
                }
                break;
            }
            }
        }

        return m_pSubTask->MakeAbortable(ped);
    }
    case ABORT_PRIORITY_LEISURE: {
        m_bNeedsToCreatefirstSubTask = true;
        return false;
    }
    default:
        NOTSA_UNREACHABLE("Invalid priority: {}", (int)priority);
    }
}

// 0x62DC20
CTask* CTaskComplexDestroyCarMelee::CreateNextSubTask(CPed* ped) {
    if (m_bNeedsToCreatefirstSubTask) {
        return nullptr;
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_PAUSE:
    case TASK_SIMPLE_FIGHT_CTRL: {
        if (ped->bStayInSamePlace) { // Inverted
            const auto finished = m_nTimeMs != -1 && CTimer::GetTimeInMS() - m_nTimeMs > 3000;
            return CreateSubTask(finished ? TASK_FINISHED : TASK_SIMPLE_PAUSE, ped);
        }
        CalculateSearchPositionAndRanges(ped);
        return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        CalculateSearchPositionAndRanges(ped);
        if (IsPointInSphere(ped->GetPosition(), m_VehToDestroy->GetPosition(), m_MaxTargetFightDist)) {
            return CreateSubTask(TASK_SIMPLE_FIGHT_CTRL, ped);
        }
        return CreateSubTask(ped->bStayInSamePlace ? TASK_SIMPLE_PAUSE : TASK_COMPLEX_SEEK_ENTITY, ped);
    }
    case TASK_COMPLEX_SEEK_ENTITY: {
        return CreateSubTask(TASK_SIMPLE_FIGHT_CTRL, ped);
    }
    default:
        return nullptr;
    }
}

// 0x62DB20
CTask* CTaskComplexDestroyCarMelee::CreateFirstSubTask(CPed* ped) {
    byteD = false;

    // CWeaponInfo::GetWeaponInfo(ped->GetActiveWeapon().m_nType); // unused

    CalculateSearchPositionAndRanges(ped);

    const auto& pedPos = ped->GetPosition();

    if (IsPointInSphere(pedPos, m_VehToDestroy->GetPosition(), m_MaxFightCtrlRadius)) {
        return CreateSubTask(ped->bStayInSamePlace ? TASK_SIMPLE_PAUSE : TASK_COMPLEX_SEEK_ENTITY, ped);
    }

    ped->m_fAimingRotation = CGeneral::GetRadianAngleBetweenPoints(m_VehiclePos, pedPos);
    return CreateSubTask(TASK_SIMPLE_FIGHT_CTRL, ped);
}

// 0x62DDB0
CTask* CTaskComplexDestroyCarMelee::ControlSubTask(CPed* ped) {
    if (byteD) {
        return CreateFirstSubTask(ped);
    }

    if (m_VehToDestroy->m_fHealth <= 0.0f)
        return nullptr;

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_SEEK_ENTITY:
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        CalculateSearchPositionAndRanges(ped);
        if (DistanceBetweenPoints(ped->GetPosition(), m_VehToDestroy->GetPosition()) > sq(m_MaxTargetFightDist)) {
            return CreateSubTask(TASK_SIMPLE_FIGHT_CTRL, ped);
        }
        return m_pSubTask;
    }
    case TASK_SIMPLE_FIGHT_CTRL:
        CalculateSearchPositionAndRanges(ped);
        if (DistanceBetweenPoints(ped->GetPosition(), m_VehToDestroy->GetPosition()) > m_MaxTargetFightDist + 0.6f) {
            return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
        }
        // todo:
        return m_pSubTask;
    default:
        return nullptr;
    }
}

// 0x6289F0
void CTaskComplexDestroyCarMelee::CalculateSearchPositionAndRanges(CPed* ped) {
    m_MaxTargetFightDist = m_MaxFightCtrlRadius = m_VehToDestroy->GetModelInfo()->GetColModel()->GetBoundRadius() + 0.35f;
    m_PedVehicleAngleRad = (m_VehToDestroy->GetPosition() - ped->GetPosition()).Heading();
}

// 0x628A70
CTask* CTaskComplexDestroyCarMelee::CreateSubTask(eTaskType taskType, CPed* ped) {
    // Missing stub for `CTaskComplexSeekEntity`
    return plugin::CallMethodAndReturn<CTask*, 0x628A70, CTaskComplexDestroyCarMelee*, eTaskType, CPed*>(this, taskType, ped);
}
