/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Task.h"

// 0x61A5A0
void* CTask::operator new(size_t size) {
    return GetTaskPool()->New();
}

// 0x61A5B0
void CTask::operator delete(void* object) {
    GetTaskPool()->Delete(static_cast<CTask*>(object));
}

// 0x421180
void CTask::StopTimer(const CEvent* event) {
    // NOP
}

// 0x61A360
bool CTask::IsGoToTask(CTask* task) {
    switch (task->GetTaskType()) {
    case TASK_SIMPLE_GO_TO_POINT:
    case TASK_SIMPLE_GO_TO_POINT_FINE:
        return true;
    default:
        return false;
    }
}

// 0x61A4B0
bool CTask::IsTaskPtr(CTask* task) {
    return plugin::CallAndReturn<bool, 0x61A4B0>(task);
}
