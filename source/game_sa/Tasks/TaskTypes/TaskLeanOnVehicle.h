#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskLeanOnVehicle : public CTaskSimple {
public:
    CVehicle*  m_Vehicle;
    int32      m_LeanAnimDurationInMs;
    int8       field_10;
    CAnimBlendAssociation* m_LeanAnim;
    int32      m_LastAnimId;
    CTaskTimer m_LeanTimer;
    bool       m_bFinished;
    int8       m_LeanAnimId;
    bool       m_StopLeaning;
    int8       field_2B;

public:
    static constexpr auto Type = TASK_COMPLEX_LEAN_ON_VEHICLE;

    CTaskLeanOnVehicle(CEntity* vehicle, int32 leanAnimDurationInMs, uint8 a4);
    ~CTaskLeanOnVehicle() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskLeanOnVehicle(m_Vehicle, m_LeanAnimDurationInMs, field_10); } // 0x6610A0
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

    static void FinishAnimCB(CAnimBlendAssociation* assoc, void* data);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskLeanOnVehicle* Constructor(CEntity* vehicle, int32 leanAnimDurationInMs, uint8 a4) { this->CTaskLeanOnVehicle::CTaskLeanOnVehicle(vehicle, leanAnimDurationInMs, a4); return this; }
    CTaskLeanOnVehicle* Destructor() { this->CTaskLeanOnVehicle::~CTaskLeanOnVehicle(); return this; }
};
VALIDATE_SIZE(CTaskLeanOnVehicle, 0x2C);
