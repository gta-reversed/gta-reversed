#pragma once

#include "TaskComplex.h"
#include <Enums/eTargetDoor.h>
#include <extensions/EntityRef.hpp>

class CVehicle;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskComplexCarSlowBeDraggedOutAndStandUp : public CTaskComplex {
public:
    notsa::EntityRef<CVehicle> m_Vehicle;
    eTargetDoor                m_Door;

public:
    static constexpr auto Type = TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT_AND_STAND_UP;

    CTaskComplexCarSlowBeDraggedOutAndStandUp(CVehicle* vehicle, int32 door);
    ~CTaskComplexCarSlowBeDraggedOutAndStandUp() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskComplexCarSlowBeDraggedOutAndStandUp(m_Vehicle, m_Door); } // 0x64A190
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override { return m_pSubTask; } // 0x648B80

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

private:
    CTask*                      CreateLeaveCarSubTask(CPed* ped) const;
    std::optional<eAudioEvents> GetJackedAudioEvent(AssocGroupId animGroupId) const;

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};
