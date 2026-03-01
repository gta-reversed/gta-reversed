#pragma once

#include "TaskComplex.h"

#include <extensions/EntityRef.hpp>

class CTaskComplexKillCriminal;
class CEvent;
class CTask;
class CCopPed;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexKillCriminal : public CTaskComplex {
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_KILL_CRIMINAL;

    CTaskComplexKillCriminal(CPed* criminal, bool randomize = false);
    CTaskComplexKillCriminal(const CTaskComplexKillCriminal&);
    ~CTaskComplexKillCriminal();

    CTask* CreateSubTask(eTaskType tt, CPed* ped, bool force = false);
    CPed* FindNextCriminalToKill(CPed* ped, bool any);
    bool ChangeTarget(CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexKillCriminal{ *this }; } // 0x68CE50
    eTaskType GetTaskType() const override { return Type; } // 0x68BF20
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

private:
    notsa::EntityRef<CPed>    m_Criminal{};
    notsa::EntityRef<CCopPed> m_Cop{};
    float                     m_TimeToGetOutOfCar{ 3.f };
    bool                      m_Randomize{};
    bool                      m_HasFinished{};
    bool                      m_CantGetInCar{};
    int8                      m_OrigDrivingMode{};
    int8                      m_OrigMission{};
    uint8                     m_OrigCruiseSpeed{};
    bool                      m_IsSetUp{};
};
