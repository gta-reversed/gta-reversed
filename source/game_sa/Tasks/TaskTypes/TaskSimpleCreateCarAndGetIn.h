#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleCreateCarAndGetIn : public CTaskSimple {
public:
    CVector   m_Pos;
    int32     m_nModel;
    CVehicle* m_CreatedVeh;
    CVector   m_VehCreationPos;
    uint32    m_nTimeMs;
    uint32    m_nWaitTime;
    bool      m_bWaitTimeSet;
    bool      m_bResetWaitTime;

public:
    static constexpr auto Type = TASK_SIMPLE_CREATE_CAR_AND_GET_IN;

    CTaskSimpleCreateCarAndGetIn(CVector const& pos, int32 model);
    ~CTaskSimpleCreateCarAndGetIn() override;

    CTask* Clone() const override { return new CTaskSimpleCreateCarAndGetIn{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

private:
    void CleanupCreatedVehicle();

    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskSimpleCreateCarAndGetIn* Constructor(CVector const& pos, int32 model) { this->CTaskSimpleCreateCarAndGetIn::CTaskSimpleCreateCarAndGetIn(pos, model); return this; }
    CTaskSimpleCreateCarAndGetIn* Destructor() { this->CTaskSimpleCreateCarAndGetIn::~CTaskSimpleCreateCarAndGetIn(); return this; }
};
VALIDATE_SIZE(CTaskSimpleCreateCarAndGetIn, 0x34);
