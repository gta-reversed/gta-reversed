#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexHitPedWithCar final : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_HIT_PED_WITH_CAR;

    static void InjectHooks();

    CTaskComplexHitPedWithCar(CVehicle* veh, float impulseMagnitude);
    CTaskComplexHitPedWithCar(const CTaskComplexHitPedWithCar&);
    ~CTaskComplexHitPedWithCar();

    static CVector ComputeEvasiveStepMoveDir(const CPed* ped, CVehicle* veh);

    bool HitHurtsPed(CPed* ped);
    CTask* CreateSubTask(eTaskType);

    CTask*    Clone() const override { return new CTaskComplexHitPedWithCar{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6539A0
    CTaskComplexHitPedWithCar* Constructor(CVehicle* vehicle, float damageIntensity) {
        this->CTaskComplexHitPedWithCar::CTaskComplexHitPedWithCar(vehicle, damageIntensity);
        return this;
    }

    // 0x653A30
    CTaskComplexHitPedWithCar* Destructor() {
        this->CTaskComplexHitPedWithCar::~CTaskComplexHitPedWithCar();
        return this;
    }

public:
    CVehicle* m_Veh{};
    float     m_ImpulseMag{};
    int32     m_PedHitSide{};
    int32     m_DownTime{1000};
    CVector   m_MoveDir{};
};            
