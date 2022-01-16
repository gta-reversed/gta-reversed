#include "StdInc.h"

#include "TaskComplexBeInGroup.h"

CTaskComplexBeInGroup::CTaskComplexBeInGroup(int32 groupId, bool isLeader)
{
    m_groupId           = groupId;
    m_ped               = 0;
    m_mainTask          = nullptr;
    m_secondaryTask     = nullptr;
    m_isLeader          = isLeader;
    m_mainTaskId        = TASK_NONE;
    m_secondaryTaskSlot = -1;
}

CTask* CTaskComplexBeInGroup::Clone()
{
    return plugin::CallMethodAndReturn<CTask*, 0x636BE0, CTask*>(this);
}

bool CTaskComplexBeInGroup::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event)
{ 
    return plugin::CallMethodAndReturn<bool, 0x632EB0, CTask*, CPed*, int32, const CEvent*>(this, ped, priority, event);
}

CTask* CTaskComplexBeInGroup::CreateNextSubTask(CPed* ped)
{
    return plugin::CallMethodAndReturn<CTask*, 0x632F40, CTask*, CPed*>(this, ped);
}

CTask* CTaskComplexBeInGroup::CreateFirstSubTask(CPed* ped)
{
    return plugin::CallMethodAndReturn<CTask*, 0x632FB0, CTask*, CPed*>(this, ped);
}

CTask* CTaskComplexBeInGroup::ControlSubTask(CPed* ped)
{
    return plugin::CallMethodAndReturn<CTask*, 0x638AA0, CTask*, CPed*>(this, ped);
}
