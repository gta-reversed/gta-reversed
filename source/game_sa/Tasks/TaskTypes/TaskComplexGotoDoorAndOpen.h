#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"
#include "Vector.h"

class CEntity;
class CObject;

class NOTSA_EXPORT_VTABLE CTaskComplexGotoDoorAndOpen : public CTaskComplex {
public:
    CObject*   m_Object; //!< 0xC  - The door
    CVector    m_Start;  //!< 0x10 - Position in front of the door
    CVector    m_End;    //!< 0x1C - Position behind the door
    CTaskTimer m_Timer;  //!< 0x28

    union {
        struct {
            uint8 m_bHasDoorObject : 1;              //!< Was this task created with a door object (Otherwise only with the start/end positions)
            uint8 m_bGoingThroughDoor : 1;           //!< Walking from `m_Start` to `m_End`
            uint8 m_bDoorWasCollidable : 1;          //!< The door was collidable, its collision was disabled
            uint8 m_bPlayerOnInteriorTransition : 1; //!< `CPad::bPlayerOnInteriorTransition` was set by this task
        };

        uint8 m_nFlags; //!< 0x34
    };

public:
    static constexpr auto Type = TASK_COMPLEX_GOTO_DOOR_AND_OPEN;

    explicit CTaskComplexGotoDoorAndOpen(CObject* door);
    CTaskComplexGotoDoorAndOpen(const CVector& start, const CVector& end);
    ~CTaskComplexGotoDoorAndOpen() override;

    eTaskType GetTaskType() const override{ return Type; } // 0x66BB90
    CTask* Clone() const override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskComplexGotoDoorAndOpen* Constructor(CObject* door) { this->CTaskComplexGotoDoorAndOpen::CTaskComplexGotoDoorAndOpen(door); return this; }
    CTaskComplexGotoDoorAndOpen* Constructor(CVector const& start, CVector const& end) { this->CTaskComplexGotoDoorAndOpen::CTaskComplexGotoDoorAndOpen(start, end); return this; }
    CTaskComplexGotoDoorAndOpen* Destructor() { this->CTaskComplexGotoDoorAndOpen::~CTaskComplexGotoDoorAndOpen(); return this; }
};
VALIDATE_SIZE(CTaskComplexGotoDoorAndOpen, 0x38);
