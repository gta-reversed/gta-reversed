#pragma once

#include "Vector.h"
#include "TaskComplex.h"

class CPed;
class CEvent;
class CTaskSimpleStandStill;
class CTaskComplexKillPedOnFootArmed;

class NOTSA_EXPORT_VTABLE CTaskComplexKillPedOnFootArmed : public CTaskComplex {
    enum class eStrafeDir : uint8 {
        LEFT,
        RIGHT,
        FORWARD,
        BACK,
    };

public:
    CPed*      m_target{}; 
    uint32     m_shootTimer{}; 
    uint32     m_lastAttackTime{};

    uint32     m_lastRollTime{};

    uint32     m_duckingConditions{};   //< Under what conditions to duck
    uint32     m_lengthOfDuck{};        //< How long to duck for (Value must fit into `uint16`)
    uint32     m_duckChancePerc{};      //< Change of ducking (Under the right conditions) - [0, 100]
    uint32     m_lastDuckTime{};        //< Last time we've ducked (Using `CTimer::GetTimeMs()`) or the last time we've tried to duck (and failed due to random chance)

    uint32     m_lastStrafeTime{}; 
    int8       m_competence{}; 
    eStrafeDir m_strafeDir{}; 
    bool       m_bStrafeBack{}; 
    bool       m_bShotFiredByPlayer{}; 
    bool       m_newTarget{}; 
    bool       m_aimImmediate{};

    CVector    m_losBlockedOurPos{0.f, 0.f, -9999.f}; 
    CVector    m_losBlockedTargetPos{ 0.f, 0.f, -9999.f };
    uint32     m_losBlockedTime{}; //> If != 0 the last time `LineOfSightClearForAttack` was called the LOS was blocked, otherwise it wasn't

    uint32     m_losClearTime{}; //> If != 0 the last time `LineOfSightClearForAttack` was called the LOS was clear, otherwise it wasn't

    int32      m_needToMoveInCloserTime{}; 

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED;

    CTaskComplexKillPedOnFootArmed(
        CPed*  target,
        uint32 duckingConditions,
        uint32 duckTime,
        uint32 duckChancePerc,
        int8   competence
    );
    CTaskComplexKillPedOnFootArmed(const CTaskComplexKillPedOnFootArmed& o);
    ~CTaskComplexKillPedOnFootArmed() override;

    bool   LineOfSightClearForAttack(CPed* target);
    bool   IsPedInLeaderFiringLine(CPed* ped);
    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexKillPedOnFootArmed{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

 
private: // Wrappers for hooks
    // 0x621190
    CTaskComplexKillPedOnFootArmed* Constructor(CPed* target, uint32 duckingConditions, uint32 duckTime, uint32 duckChancePerc, int8 competence) {
        this->CTaskComplexKillPedOnFootArmed::CTaskComplexKillPedOnFootArmed(target, duckingConditions, duckTime, duckChancePerc, competence);
        return this;
    }
    // 0x621250
    CTaskComplexKillPedOnFootArmed* Destructor() {
        this->CTaskComplexKillPedOnFootArmed::~CTaskComplexKillPedOnFootArmed();
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexKillPedOnFootArmed, 0x5C);
