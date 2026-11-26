#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"
#include "Vector.h"
#include "Object.h"

class NOTSA_EXPORT_VTABLE CTaskComplexGotoDoorAndOpen : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_GOTO_DOOR_AND_OPEN;

    explicit CTaskComplexGotoDoorAndOpen(CObject* door);
    CTaskComplexGotoDoorAndOpen(const CVector& start, const CVector& end);
    ~CTaskComplexGotoDoorAndOpen() override;

    eTaskType GetTaskType() const override { return Type; } // 0x66BB90

    CTask* Clone() const override;
    bool   MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

public:
    static void InjectHooks();

private:
    CObject::Ref m_DoorToOpen{};                        //!< 0xC  - The door object
    CVector      m_Start{};                             //!< 0x10 - Position in front of the door
    CVector      m_End{};                               //!< 0x1C - Position behind the door
    CTaskTimer   m_DoorOpenTimer{};                     //!< 0x28
    uint8        m_HasDoorObject : 1 {};                //!< Was this task created with a door object (Otherwise only with the start/end positions)
    uint8        m_IsGoingThroughDoor : 1 {};           //!< Walking from `m_Start` to `m_End`
    uint8        m_WasDoorCollidable : 1 {};            //!< The door was collidable, its collision was disabled
    uint8        m_IsPlayerOnInteriorTransition : 1 {}; //!< `CPad::bPlayerOnInteriorTransition` was set by this task
};

VALIDATE_SIZE(CTaskComplexGotoDoorAndOpen, 0x38);
