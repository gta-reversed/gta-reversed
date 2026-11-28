/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"
#include "Ped.h"

class NOTSA_EXPORT_VTABLE CTaskComplexKillPedOnFoot : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_KILL_PED_ON_FOOT;

    CTaskComplexKillPedOnFoot(
        CPed* target,
        int32 time       = -1,
        int32 pedFlags   = 0,
        int32 delay      = 0,
        int32 chance     = 0,
        uint8 competence = 1,
        /* VVV notsa VVV */
        bool waitForPlayerToBeSafe    = false,
        bool waitingForPlayerToBeSafe = false
    );
    CTaskComplexKillPedOnFoot(const CTaskComplexKillPedOnFoot&);
    ~CTaskComplexKillPedOnFoot() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexKillPedOnFoot{ *this }; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;
    CTask*    CreateSubTask(eTaskType taskType, CPed* ped);

    CPed* GetPedToKill() const { return m_PedToKill; }

private:
    bool UpdateRoomToDragPedOutOfCar(const CVehicle* veh);

    static void SayAttackGang(CPed* ped, CPed* target);

public:
    static void InjectHooks();

public:
    bool m_ShotFiredByPlayer : 1        = true;
    bool m_NewTarget : 1                = true;
    bool m_RoomToDragPedOutOfCar : 1    = true;
    bool m_TargetKilled : 1             = false;
    bool m_AimImmediate : 1             = false;
    bool m_WaitForPlayerToBeSafe : 1    = false;
    bool m_WaitingForPlayerToBeSafe : 1 = false;

    CPed::Ref  m_PedToKill{};
    int32      m_PedFlags{};     //!< Ducking conditions of the armed sub task
    int32      m_ActionDelay{};  //!< Duck time of the armed sub task
    int32      m_ActionChance{}; //!< Duck chance (%) of the armed sub task
    uint8      m_Competence{};
    uint32     m_StartTime{};
    int32      m_Time{};
    CTaskTimer m_Timer{};
};
VALIDATE_SIZE(CTaskComplexKillPedOnFoot, 0x38);
