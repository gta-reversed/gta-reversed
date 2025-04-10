#pragma once

#include "TaskComplex.h"

#include "Enums/eCarDrivingStyle.h"
#include "Enums/eCarMission.h"

class CTask;
class CTaskComplexKillCriminal;
class CCopPed;
class CPed;
class CEvent;


class NOTSA_EXPORT_VTABLE CTaskComplexKillCriminal : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_KILL_CRIMINAL;

    static void InjectHooks();

    CTaskComplexKillCriminal(CPed* criminal, bool randomize = false);
    CTaskComplexKillCriminal(const CTaskComplexKillCriminal&);
    ~CTaskComplexKillCriminal();

    CTask* CreateSubTask(eTaskType tt, CPed* ped, bool force);
    CPed*  FindNextCriminalToKill(CPed* ped, bool any);
    int8   ChangeTarget(CPed* newTarget);

    CTask*    Clone() const override { return new CTaskComplexKillCriminal{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x68BE70
    CTaskComplexKillCriminal* Constructor(CPed * criminal, bool randomize) {
        this->CTaskComplexKillCriminal::CTaskComplexKillCriminal(criminal, randomize);
        return this;
    }

    // 0x68BF30
    CTaskComplexKillCriminal* Destructor() {
        this->CTaskComplexKillCriminal::~CTaskComplexKillCriminal();
        return this;
    }

private:
    CPed*            m_Criminal{}; 
    CCopPed*         m_Cop{}; 
    float            m_TimeToGetOutOfCar{3.f};
    bool             m_Randomize{}; 
    bool             m_Finished{}; 
    bool             m_CantGetInCar{}; 
    eCarDrivingStyle m_OrigDrivingStyle{}; 
    eCarMission      m_OrigMission{}; 
    uint8            m_OrigCruiseSpeed{}; 
    bool             m_IsSetUp{}; 
};
