#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CTaskComplexKillPedOnFootMelee;
class CEvent;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexKillPedOnFootMelee : public CTaskComplex {
public:
    CPed*   m_target{};
    CVector m_targetPos{};      //< m_target's position
    float   m_arriveRange{};    //< Melee combo's range
    float   m_attackRange{};
    float   m_attackAngle{};
    int32   m_attackPointIndex{ -1 };
    uint32  m_lastAttackTime{};
    bool    m_bShotFiredByPlayerFlag{};
    bool    m_bNewTarget{};
    int32   m_pauseTimer{ -1 };

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE;

    CTaskComplexKillPedOnFootMelee(CPed * target);
    CTaskComplexKillPedOnFootMelee(const CTaskComplexKillPedOnFootMelee&);
    ~CTaskComplexKillPedOnFootMelee();

    void   CalculateSearchPositionAndRanges(CPed* ped);
    CTask* CreateSubTask(eTaskType taskType, CPed * ped);
    bool   IsTargetInRange(CPed* ped, float range); // NOTSA

    CTask*    Clone() const override { return new CTaskComplexKillPedOnFootMelee{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x621000
    CTaskComplexKillPedOnFootMelee* Constructor(CPed * target) {
        this->CTaskComplexKillPedOnFootMelee::CTaskComplexKillPedOnFootMelee(target);
        return this;
    }
    // 0x621090
    CTaskComplexKillPedOnFootMelee* Destructor() {
        this->CTaskComplexKillPedOnFootMelee::~CTaskComplexKillPedOnFootMelee();
        return this;
    }
};
