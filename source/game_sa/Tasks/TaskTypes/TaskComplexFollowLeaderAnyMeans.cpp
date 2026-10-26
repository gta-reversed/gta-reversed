#include "StdInc.h"
#include "TaskComplexFollowLeaderAnyMeans.h"
#include "TaskComplexFollowLeaderInFormation.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleCarDriveTimed.h"
#include "SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h"

void CTaskComplexFollowLeaderAnyMeans::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowLeaderAnyMeans, 0x870c68, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x694AA0);
    RH_ScopedInstall(Destructor, 0x694B30);

    RH_ScopedInstall(CreateSubTask, 0x696680);

    RH_ScopedVMTInstall(Clone, 0x6957B0);
    RH_ScopedVMTInstall(GetTaskType, 0x694B20);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x696CC0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x696D80);
    RH_ScopedVMTInstall(ControlSubTask, 0x696DA0);
}

// 0x694AA0
CTaskComplexFollowLeaderAnyMeans::CTaskComplexFollowLeaderAnyMeans(CPedGroup* pedGroup, CPed* ped, CVector const& pos) :
    m_group{pedGroup},
    m_ped{ped},
    m_pos{pos}
{
    DebugBreak(); // Pirulax: Let me know if this is ever hit, cause this task is seemingly unused.
    CEntity::SafeRegisterRef(m_ped);
}

CTaskComplexFollowLeaderAnyMeans::CTaskComplexFollowLeaderAnyMeans(const CTaskComplexFollowLeaderAnyMeans& o) :
    CTaskComplexFollowLeaderAnyMeans{ o.m_group, o.m_ped, o.m_pos }
{
}

// 0x694B30
CTaskComplexFollowLeaderAnyMeans::~CTaskComplexFollowLeaderAnyMeans() {
    CEntity::SafeCleanUpRef(m_ped);
}

// 0x696680
CTask* CTaskComplexFollowLeaderAnyMeans::CreateSubTask(eTaskType taskType, CPed const* ped) {
    switch (taskType) {
    case TASK_COMPLEX_FOLLOW_LEADER_IN_FORMATION:
        return new CTaskComplexFollowLeaderInFormation{ m_group, m_ped, m_pos, -1.f };
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{ 2000 };
    case TASK_SIMPLE_CAR_DRIVE_TIMED:
        return new CTaskSimpleCarDriveTimed{ ped->m_pVehicle, 2000 };
    case TASK_COMPLEX_SEEK_ENTITY_ANY_MEANS:
        // NOTE: There's also some weird `mov [pXYOffsetTask + 0x50], 7`
        // even though only 0x34 bytes are allocated...
        return new CTaskComplexSeekEntityAnyMeansXYOffset{ m_ped, m_pos };
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x696CC0
CTask* CTaskComplexFollowLeaderAnyMeans::CreateNextSubTask(CPed* ped) {
    return CreateSubTask([ped, this] {
        // Cases ordered in the same way they're created
        switch (m_pSubTask->GetTaskType()) {
        case TASK_COMPLEX_FOLLOW_LEADER_IN_FORMATION:
            return TASK_FINISHED;
        case TASK_COMPLEX_SEEK_ENTITY_ANY_MEANS: // May be created by `ControlSubTask`
            return ped->bInVehicle
                ? TASK_SIMPLE_CAR_DRIVE_TIMED
                : TASK_SIMPLE_STAND_STILL;
        case TASK_SIMPLE_STAND_STILL:
        case TASK_SIMPLE_CAR_DRIVE_TIMED:
            return ped->bInVehicle
                ? TASK_COMPLEX_SEEK_ENTITY_ANY_MEANS
                : TASK_COMPLEX_FOLLOW_LEADER_IN_FORMATION;
        default:
            NOTSA_UNREACHABLE();
        }
    }(), ped);
}

// 0x696D80
CTask* CTaskComplexFollowLeaderAnyMeans::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_COMPLEX_FOLLOW_LEADER_IN_FORMATION, ped);
}

// 0x696DA0
CTask* CTaskComplexFollowLeaderAnyMeans::ControlSubTask(CPed* ped) {
    if (!m_ped) {
        return nullptr;
    }
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_FOLLOW_LEADER_IN_FORMATION) {
        if (ped->bInVehicle) {
            return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY_ANY_MEANS, ped);
        }
    }
    return m_pSubTask;
}
