#pragma once

#include <extensions/EntityRef.hpp>
#include "TaskComplex.h"

class CEvent;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexPolicePursuit : public CTaskComplex {
public:
    bool                      m_IsRoadBlockCop : 1 {};
    bool                      m_IsPlayerInCullZone : 1 {};
    bool                      m_CouldJoinPursuit : 1 { true };
    notsa::EntityRef<CCopPed> m_Pursuer{};
    notsa::EntityRef<CPed>    m_Pursued{};

public:
    static constexpr auto Type = TASK_COMPLEX_POLICE_PURSUIT;

    static void InjectHooks();

    CTaskComplexPolicePursuit() = default;
    CTaskComplexPolicePursuit(const CTaskComplexPolicePursuit&);
    ~CTaskComplexPolicePursuit() override;

    static void __stdcall SetWeapon(CPed* ped);
    static void __stdcall ClearPursuit(CCopPed* pursuer);

    // 0x68BAA0
    eTaskType GetTaskType() const override { return Type; }

    // 0x68BAB0
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return m_pSubTask->MakeAbortable(ped, priority, event); }

    // 0x68CDD0
    CTask* Clone() const override { return new CTaskComplexPolicePursuit{*this}; }

    // 0x68BAC0
    CTask* CreateNextSubTask(CPed* ped) override { return nullptr; }

    bool SetPursuit(CPed* ped);
    bool PersistPursuit(CCopPed* pursuer);
    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

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

VALIDATE_SIZE(CTaskComplexPolicePursuit, 0x18);
