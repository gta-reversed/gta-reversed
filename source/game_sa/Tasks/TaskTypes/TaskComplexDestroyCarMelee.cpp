#include "StdInc.h"

#include "TaskComplexDestroyCarMelee.h"
#include "SeekEntity/TaskComplexSeekEntity.h"
#include "TaskSimplePause.h"
#include "TaskSimpleFightingControl.h"

void CTaskComplexDestroyCarMelee::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDestroyCarMelee, 0x86d994, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x621D10);
    RH_ScopedInstall(Destructor, 0x621DA0);

    RH_ScopedInstall(CreateSubTask, 0x628A70);
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

                if ((ped->GetPosition() - m_VehToDestroy->GetPosition()).Dot(m_VehToDestroy->m_vecMoveSpeed) > 0.f) {
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
        m_AbortAtLeisure = true;
        return false;
    }
    default:
        NOTSA_UNREACHABLE("Invalid priority: {}", (int)priority);
    }
}

// 0x62DC20
CTask* CTaskComplexDestroyCarMelee::CreateNextSubTask(CPed* ped) {
    if (m_AbortAtLeisure) {
        return nullptr;
    }
    return CreateSubTask([this, ped] {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_PAUSE:
        case TASK_SIMPLE_FIGHT_CTRL: {
            if (ped->bStayInSamePlace) { // Inverted
                return m_PauseTimer != -1 && CTimer::GetTimeInMS() - m_PauseTimer > 3000
                    ? TASK_FINISHED
                    : TASK_SIMPLE_PAUSE;
            }
            CalculateSearchPositionAndRanges(ped);
            return TASK_COMPLEX_SEEK_ENTITY;
        }
        case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // 0x62DC70
            CalculateSearchPositionAndRanges(ped);
            return IsPointInSphere(ped->GetPosition(), m_VehToDestroy->GetPosition(), m_MaxAtkRange)
                ? TASK_SIMPLE_FIGHT_CTRL
                : ped->bStayInSamePlace
                    ? TASK_SIMPLE_PAUSE
                    : TASK_COMPLEX_SEEK_ENTITY;
        }
        case TASK_COMPLEX_SEEK_ENTITY: { // 0x62DD11
            return TASK_SIMPLE_FIGHT_CTRL;
        }
        default:
            NOTSA_UNREACHABLE();
        }
    }(), ped);
}

// 0x62DB20
CTask* CTaskComplexDestroyCarMelee::CreateFirstSubTask(CPed* ped) {
    m_HasNewTarget = false;

    // CWeaponInfo::GetWeaponInfo(ped->GetActiveWeapon().m_nType); // unused

    CalculateSearchPositionAndRanges(ped);

    const auto& pedPos = ped->GetPosition();

    if (IsPointInSphere(pedPos, m_VehToDestroy->GetPosition(), m_MaxArriveRange)) {
        return CreateSubTask(ped->bStayInSamePlace ? TASK_SIMPLE_PAUSE : TASK_COMPLEX_SEEK_ENTITY, ped);
    }

    ped->m_fAimingRotation = CGeneral::GetRadianAngleBetweenPoints(m_VehPos, pedPos);
    return CreateSubTask(TASK_SIMPLE_FIGHT_CTRL, ped);
}

// 0x62DDB0
CTask* CTaskComplexDestroyCarMelee::ControlSubTask(CPed* ped) {
    if (m_HasNewTarget) {
        return CreateFirstSubTask(ped);
    }

    if (m_VehToDestroy->m_fHealth <= 0.0f)
        return nullptr;

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_SEEK_ENTITY:
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // 0x62DEAB
        CalculateSearchPositionAndRanges(ped);
        return IsPointInSphere(ped->GetPosition(), m_VehToDestroy->GetPosition(), m_MaxAtkRange)
            ? CreateSubTask(TASK_SIMPLE_FIGHT_CTRL, ped)
            : m_pSubTask;
    }
    case TASK_SIMPLE_FIGHT_CTRL: { // 0x62DE1A
        CalculateSearchPositionAndRanges(ped);
        if (!IsPointInSphere(ped->GetPosition(), m_VehToDestroy->GetPosition(), m_MaxAtkRange + 0.6f)) {
            return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
        }
        const auto tFightCtrl = notsa::cast<CTaskSimpleFightingControl>(m_pSubTask);
        tFightCtrl->m_angleRad = m_MaxAtkAngleRad;
        tFightCtrl->m_maxAttackRange = m_MaxAtkRange;
        return m_pSubTask;
    }
    default:
        return nullptr;
    }
}

// 0x6289F0
void CTaskComplexDestroyCarMelee::CalculateSearchPositionAndRanges(CPed* ped) {
    m_MaxAtkRange
        = m_MaxArriveRange
        = m_VehToDestroy->GetModelInfo()->GetColModel()->GetBoundRadius() + 0.35f;
    m_MaxAtkAngleRad = (m_VehToDestroy->GetPosition2D() - ped->GetPosition2D()).Heading(); // veh - ped, because heading uses atan(-x, y), 180deg offset
}

// 0x628A70
CTask* CTaskComplexDestroyCarMelee::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_SEEK_ENTITY: { // 0x628B89
        m_PauseTimer = -1;
        return new CTaskComplexSeekEntity{
            m_VehToDestroy,
            50'000,
            1'000,
            1.f,
            2.f,
            2.f,
            true,
            true
        };
    }
    case TASK_SIMPLE_PAUSE: { // 0x628A9A
        CTaskSimpleStandStill{}.ProcessPed(ped);
        if (m_PauseTimer == -1) {
            m_PauseTimer = CTimer::GetTimeInMS();
        }
        return new CTaskSimplePause{ 100 };
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // 0x628AB4
        m_PauseTimer = -1;
        return new CTaskComplexGoToPointAndStandStill{
            PEDMOVE_RUN,
            m_VehPos,
            0.25f,
            0.5f
        };
    }
    case TASK_SIMPLE_FIGHT_CTRL: { // 0x628B1D
        m_PauseTimer = -1;
        return new CTaskSimpleFightingControl{
            m_VehToDestroy,
            m_MaxAtkAngleRad,
            m_MaxAtkRange
        };
    }
    case TASK_FINISHED: { // 0x628BF6
        m_PauseTimer = -1;
        return nullptr;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}
