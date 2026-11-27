#pragma once

#include <extensions/EntityRef.hpp>
#include "Vector.h"
#include "TaskComplex.h"

class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskComplexKillPedOnFootArmed : public CTaskComplex {
    enum class eStrafeDir : uint8 {
        LEFT,
        RIGHT,
        FORWARD,
        BACK,
    };

    enum eDuckingConditions : uint32 {
        DUCK_WHEN_SHOT_AT_BY_PLAYER = 1 << 0, //!< Duck (by chance) while `m_ShotFiredByPlayer` is set
        DUCK_PERIODICALLY           = 1 << 2, //!< Duck (by chance) 2 seconds after the last duck is over
    };

public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED;

    CTaskComplexKillPedOnFootArmed(
        CPed*  target,
        uint32 duckingConditions,
        uint32 lengthOfDuck,
        uint32 duckChancePerc,
        int8   competence
    );
    CTaskComplexKillPedOnFootArmed(const CTaskComplexKillPedOnFootArmed& o); // NOTSA
    ~CTaskComplexKillPedOnFootArmed() override = default;

    CTask*    Clone() const override { return new CTaskComplexKillPedOnFootArmed{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    bool   LineOfSightClearForAttack(CPed* ped);
    bool   IsPedInLeaderFiringLine(CPed* ped);
    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

    static void InjectHooks();

private:
    CTask* CreateSeekSubTask(float maxTargetDist2D); // NOTSA
    CTask* CreateMoveInCloserSubTask(CPed* ped);     // NOTSA

private:
    notsa::EntityRef<CPed> m_Target{};                                 //!< The ped to kill
    uint32                 m_ShootTimer{};                             //!< Once this time has passed the ped stops shooting to move in closer (unless the target is close already)
    uint32                 m_LastAttackTime{};                         //!< Last time an attack was started (or given up)
    uint32                 m_LastRollTime{};                           //!< Last time the ped has rolled sideways while ducking
    uint32                 m_DuckingConditions{};                      //!< Under what conditions to duck (See `eDuckingConditions`)
    uint32                 m_LengthOfDuck{};                           //!< How long to duck for (Value must fit into `uint16`)
    uint32                 m_DuckChancePerc{};                         //!< Chance of ducking (Under the right conditions) - [0, 100]
    uint32                 m_LastDuckTime{};                           //!< Last time we've tried to duck (it may have failed due to random chance)
    uint32                 m_StrafeEndTime{};                          //!< The ped strafes towards `m_StrafeDir` until this time
    int8                   m_Competence{};                             //!< Above 0 the ped uses cover, at 2 it looks for cover to attack from
    eStrafeDir             m_StrafeDir{};                              //!< NOTE: Not initialized by the original constructor
    bool                   m_StrafeBack{};                             //!< Strafe in the opposite direction once the current strafe is over
    bool                   m_ShotFiredByPlayer{};                      //!< See `DUCK_WHEN_SHOT_AT_BY_PLAYER`
    bool                   m_NewTarget{};                              //!< Start over with `CreateFirstSubTask`
    bool                   m_AimImmediate{};                           //!< Passed on to the next gun control task
    CVector                m_LosBlockedOurPos{ 0.f, 0.f, -9999.f };    //!< Our position the last time the LOS was found to be blocked
    CVector                m_LosBlockedTargetPos{ 0.f, 0.f, -9999.f }; //!< The target's position the last time the LOS was found to be blocked
    uint32                 m_LosBlockedTime{};                         //!< If != 0 the last time `LineOfSightClearForAttack` has checked the LOS it was blocked
    uint32                 m_LosClearTime{};                           //!< If != 0 the last time `LineOfSightClearForAttack` has checked the LOS it was clear
    uint32                 m_NeedToMoveInCloserTime{};                 //!< Since when the ped is moving in closer to get a clear LOS (0 if it isn't)
};
VALIDATE_SIZE(CTaskComplexKillPedOnFootArmed, 0x5C);
