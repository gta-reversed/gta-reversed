#pragma once

#include "TaskComplex.h"

class CTaskComplexKillCriminal;
class CEvent;
class CTask;
class CCopPed;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexKillCriminal : public CTaskComplex {

public:
    CPed* m_criminal{};
    CCopPed* m_cop{};           
    float m_timeToGetOutOfCar{3.f};
    bool m_randomize{};         
    bool m_finished{};          
    bool m_cantGetInCar{};      
    int8 m_origDrivingMode{};   
    int8 m_origMission{};       
    uint8 m_origCruiseSpeed{};  
    bool m_isSetUp{};           

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_KILL_CRIMINAL;

    CTaskComplexKillCriminal(CPed* criminal, bool randomize);
    CTaskComplexKillCriminal(const CTaskComplexKillCriminal&);
    ~CTaskComplexKillCriminal();

    CTask* CreateSubTask(eTaskType tt, CPed* ped, bool force);
    CPed* FindNextCriminalToKill(CPed* ped, bool any);
    bool ChangeTarget(CPed* ped);

    CTask*    Clone() override { return new CTaskComplexKillCriminal{ *this }; } // 0x68CE50
    eTaskType GetTaskType() override { return Type; } // 0x68BF20
    bool      MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x68BE70
    CTaskComplexKillCriminal* Constructor(CPed* criminal, bool randomize) {
        this->CTaskComplexKillCriminal::CTaskComplexKillCriminal(criminal, randomize);
        return this;
    }

    // 0x68BF30
    CTaskComplexKillCriminal* Destructor() {
        this->CTaskComplexKillCriminal::~CTaskComplexKillCriminal();
        return this;
    }
};
