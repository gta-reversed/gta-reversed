#pragma once

#include "TaskComplex.h"

class CEvent;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexPolicePursuit : public CTaskComplex {
public:
    bool     m_bRoadBlockCop : 1{};
    bool     m_bPlayerInCullZone : 1{};
    bool     m_bCouldJoinPursuit : 1{true};
    CCopPed* m_Pursuer{};
    CPed*    m_Persecuted{};

public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_POLICE_PURSUIT;

    static void InjectHooks();

    CTaskComplexPolicePursuit() = default;
    CTaskComplexPolicePursuit(const CTaskComplexPolicePursuit&);
    ~CTaskComplexPolicePursuit() override;

    static void __stdcall SetWeapon(CPed* ped);
    static void __stdcall ClearPursuit(CCopPed* pursuer);

// 0x0
eTaskType GetTaskType() const override { return Type; }


// 0x68BAB0
bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return m_pSubTask->MakeAbortable(ped, priority, event); }

 

// 0x68CDD0
CTask* Clone() const override { return new CTaskComplexPolicePursuit{*this}; }

 
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* CreateNextSubTask(CPed* ped) override;

    bool SetPursuit(CPed* ped);
    bool PersistPursuit(CCopPed* pursuer);
    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexPolicePursuit{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private:
    eTaskType GetNextSubTaskType(CCopPed* ped);

private: // Wrappers for hooks
    // 0x68BA70
    CTaskComplexPolicePursuit* Constructor() {
        this->CTaskComplexPolicePursuit::CTaskComplexPolicePursuit();
        return this;
    }

    // 0x68D880
    CTaskComplexPolicePursuit* Destructor() {
        this->CTaskComplexPolicePursuit::~CTaskComplexPolicePursuit();
        return this;
    }
};
