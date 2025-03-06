#include "StdInc.h"
#include "TaskComplexFleeEntity.h"
#include "TaskComplexFleePoint.h"
#include "IKChainManager_c.h"
#include "InterestingEvents.h"
#include "EventSeenPanickedPed.h"

void CTaskComplexFleeEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFleeEntity, 0x86f708, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65B930);
    RH_ScopedInstall(Destructor, 0x65B9D0);

    RH_ScopedInstall(CreateSubTask, 0x65BA30);

    RH_ScopedVMTInstall(Clone, 0x65CE50);
    RH_ScopedVMTInstall(GetTaskType, 0x65B9C0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65BAC0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65BB00);
    RH_ScopedVMTInstall(ControlSubTask, 0x65BBB0);
}

// 0x65B930
CTaskComplexFleeEntity::CTaskComplexFleeEntity(
    CEntity* fleeEntity,
    bool scream,
    float safeDistance,
    int32 fleeTime,
    int32 posCheckPeriod,
    float posChangeTolerance
) :

    m_entity{ fleeEntity },
    m_pedScream{ scream },
    m_safeDistance{ safeDistance },
    m_time{ fleeTime },
    m_posCheckPeriod{ posCheckPeriod },
    m_posChangeTolerance{ posChangeTolerance }
{
    CEntity::SafeRegisterRef(m_entity);
}

CTaskComplexFleeEntity::CTaskComplexFleeEntity(const CTaskComplexFleeEntity& o) :
    CTaskComplexFleeEntity{ o.m_entity, o.m_pedScream, o.m_safeDistance, o.m_time, o.m_posCheckPeriod, o.m_posChangeTolerance }
{
}

// 0x65B9D0
CTaskComplexFleeEntity::~CTaskComplexFleeEntity() {
    CEntity::SafeCleanUpRef(m_entity);
}

// 0x65BA30
CTask* CTaskComplexFleeEntity::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_FLEE_POINT: {
        // Below pos update code from `CreateFirstSubTask`
        m_posCheckTimer.Start(m_posCheckPeriod);
        m_pos = m_entity->GetPosition();
        return new CTaskComplexFleePoint{
            m_pos,
            m_pedScream,
            m_safeDistance,
            m_time
        };
    }
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x65BAC0
CTask* CTaskComplexFleeEntity::CreateNextSubTask(CPed* ped) {
    // Not even sure what's the point here, both returns null xD

    if (m_entity && !notsa::isa<CTaskComplexFleePoint>(m_pSubTask)) {
        return nullptr;
    }
    return CreateSubTask(TASK_FINISHED);
}

// 0x65BB00
CTask* CTaskComplexFleeEntity::CreateFirstSubTask(CPed* ped) {
    if (!m_entity) {
        return nullptr;
    }

    g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_13, ped);

    if (!g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.LookAt(
            "TaskFleeEntity",
            ped,
            m_entity,
            3000,
            m_entity->m_nStatus == STATUS_PHYSICS
            ? BONE_HEAD
            : BONE_UNKNOWN,
            nullptr,
            true,
            0.25f,
            500,
            3,
            false
        );
    }

    // Pos update code moved to `CreateSubTask`
    return CreateSubTask(TASK_COMPLEX_FLEE_POINT);
}

// 0x65BBB0
CTask* CTaskComplexFleeEntity::ControlSubTask(CPed* ped) {
     if (m_entity) {
        if (const auto fleePointTask = CTask::DynCast<CTaskComplexFleePoint>(m_pSubTask)) {
            // Check if position update timer is out of time, if so, update position (if outside tolerance)
            if (m_posCheckTimer.IsOutOfTime()) {
                m_posCheckTimer.Start(m_posCheckPeriod);

                const auto& currPos = m_entity->GetPosition();
                if ((currPos - m_pos).SquaredMagnitude() >= sq(m_posChangeTolerance)) {
                    m_pos = currPos;
                    fleePointTask->SetFleePosition(m_pos, m_safeDistance, m_pedScream);
                    GetEventGlobalGroup()->Add(CEventSeenPanickedPed{ ped });
                }
            }
        }
    } else {
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
    }

    return m_pSubTask;
}
