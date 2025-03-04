#pragma once

#include "TaskSimple.h"
#include "Vehicle.h"

class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskSimpleKillPedWithCar final : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_KILL_PED_WITH_CAR;

    static void InjectHooks();

    CTaskSimpleKillPedWithCar(CVehicle* car, float impulseMag);
    CTaskSimpleKillPedWithCar(const CTaskSimpleKillPedWithCar&);
    ~CTaskSimpleKillPedWithCar() override = default;

    void DamageCarBonnet(CPed const*);

    CTask*    Clone() const override { return new CTaskSimpleKillPedWithCar{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override { return false; }
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x653C10
    CTaskSimpleKillPedWithCar* Constructor(CVehicle* a, float b) {
        this->CTaskSimpleKillPedWithCar::CTaskSimpleKillPedWithCar(a, b);
        return this;
    }

    // 0x653C90
    CTaskSimpleKillPedWithCar* Destructor() {
        this->CTaskSimpleKillPedWithCar::~CTaskSimpleKillPedWithCar();
        return this;
    }

private:
    CVehicle::Ref m_Car{};
    float         m_ImpulseMag{};
};
