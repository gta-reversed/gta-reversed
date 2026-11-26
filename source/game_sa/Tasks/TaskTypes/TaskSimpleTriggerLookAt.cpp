#include "StdInc.h"
#include "TaskSimpleTriggerLookAt.h"
#include <Ragdoll/IKChainManager.h>

void CTaskSimpleTriggerLookAt::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleTriggerLookAt, 0x86e3cc, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x634440, "1", {}, CEntity*, int32, eBoneTag32, RwV3d, uint8, float, int32, int32);
    RH_ScopedDestructorInstall(0x634500);

    RH_ScopedVMTInstall(Clone, 0x634560);
    RH_ScopedVMTInstall(GetTaskType, 0x6344F0);
    RH_ScopedVMTInstall(MakeAbortable, 0x634610);
    RH_ScopedVMTInstall(ProcessPed, 0x634620);
}
// 0x634440
CTaskSimpleTriggerLookAt::CTaskSimpleTriggerLookAt(CEntity* entity, int32 time, eBoneTag32 offsetBoneTag, RwV3d offsetPos, bool bUseTorso, float speed, int32 blendTime, int32 priority) :
    m_Entity{ entity },
    m_Time{ time },
    m_OffsetBoneTag{ offsetBoneTag },
    m_OffsetPos{ offsetPos },
    m_UseTorso{ bUseTorso },
    m_Speed{ speed },
    m_BlendTime{ blendTime },
    m_NonNullEntity{ entity != nullptr },
    m_Priority{ static_cast<int8>(priority) } {
}

// 0x634560
CTaskSimpleTriggerLookAt::CTaskSimpleTriggerLookAt(const CTaskSimpleTriggerLookAt& o) :
    CTaskSimpleTriggerLookAt{
        o.m_Entity,
        o.m_Time,
        o.m_OffsetBoneTag,
        o.m_OffsetPos,
        o.m_UseTorso,
        o.m_Speed,
        o.m_BlendTime,
        o.m_Priority
    } {
}

// 0x634620
bool CTaskSimpleTriggerLookAt::ProcessPed(CPed* ped) {
    g_ikChainMan.LookAt(
        "TaskTriggerLookAt",
        ped,
        m_Entity,
        m_Time,
        m_OffsetBoneTag,
        &m_OffsetPos,
        m_UseTorso,
        m_Speed,
        m_BlendTime,
        m_Priority
    );
    return true;
}
