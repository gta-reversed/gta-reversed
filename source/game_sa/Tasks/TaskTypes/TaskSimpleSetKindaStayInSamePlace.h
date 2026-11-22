#pragma once

#include "TaskSimple.h"

class CPed;
class CEvent;
class CTaskSimpleSetKindaStayInSamePlace;

class NOTSA_EXPORT_VTABLE CTaskSimpleSetKindaStayInSamePlace final : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_SET_KINDA_STAY_IN_SAME_PLACE;

    static void InjectHooks();

    CTaskSimpleSetKindaStayInSamePlace(bool bKindaStayInSamePlace);
    CTaskSimpleSetKindaStayInSamePlace(const CTaskSimpleSetKindaStayInSamePlace&);
    ~CTaskSimpleSetKindaStayInSamePlace() override = default;

    CTask*    Clone() const override { return new CTaskSimpleSetKindaStayInSamePlace{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override { return true; }
    bool      ProcessPed(CPed* ped) override { ped->bKindaStayInSamePlace = m_bKindaStayInSamePlace; return true; }

private: // Wrappers for hooks
    // 0x62F610
    CTaskSimpleSetKindaStayInSamePlace* Constructor(bool a) {
        this->CTaskSimpleSetKindaStayInSamePlace::CTaskSimpleSetKindaStayInSamePlace(a);
        return this;
    }

    // 0x62F650
    CTaskSimpleSetKindaStayInSamePlace* Destructor() {
        this->CTaskSimpleSetKindaStayInSamePlace::~CTaskSimpleSetKindaStayInSamePlace();
        return this;
    }

protected:
    bool m_bKindaStayInSamePlace{};
};
