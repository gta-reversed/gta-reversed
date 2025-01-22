#pragma once

#include "TaskSimple.h"
#include "Vehicle.h"

class CEvent;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleHurtPedWithCar : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_HURT_PED_WITH_CAR;

    static void InjectHooks();

    CTaskSimpleHurtPedWithCar(CVehicle*, float);
    CTaskSimpleHurtPedWithCar(const CTaskSimpleHurtPedWithCar&);
    ~CTaskSimpleHurtPedWithCar() override = default;

    CTask*    Clone() const override { return new CTaskSimpleHurtPedWithCar{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override { return false; }
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x653EA0
    CTaskSimpleHurtPedWithCar* Constructor(CVehicle* a, float b) {
        this->CTaskSimpleHurtPedWithCar::CTaskSimpleHurtPedWithCar(a, b);
        return this;
    }

    // 0x653F30
    CTaskSimpleHurtPedWithCar* Destructor() {
        this->CTaskSimpleHurtPedWithCar::~CTaskSimpleHurtPedWithCar();
        return this;
    }

private:
    CVehicle::Ref m_Car{};          //< The car that has hit the ped
    float         m_ImpulseMag{};   //< Unused
    bool          m_bWillKillPed{}; //< Whenever the damage will kill the ped
};
