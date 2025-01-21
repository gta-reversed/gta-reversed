#pragma once

#include "TaskSimple.h"

class CTaskUtilityLineUpPedWithCar;
class CVehicle;
class CPed;
class CAnimBlendAssociation;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarFallOut : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_CAR_FALL_OUT;

    static void InjectHooks();

    CTaskSimpleCarFallOut(CVehicle* target, eTargetDoor door, CTaskUtilityLineUpPedWithCar* lineUpUtility);
    ~CTaskSimpleCarFallOut() override = default;

    auto ComputeAnimID();
    void StartAnim(const CPed* pPed);

    CTask*    Clone() const override { return new CTaskSimpleCarFallOut{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;
    bool      SetPedPosition(CPed * ped) override; 

private:
    void DoOpenCarDoor(CPed* ped);
    static void FinishAnimFallOutCB(CAnimBlendAssociation* anim, void* data);

private:
    bool                          m_HasFinished{};
    CAnimBlendAssociation*        m_Anim{};
    CVehicle*                     m_Vehicle{};
    eTargetDoor                   m_Door{};
    CTaskUtilityLineUpPedWithCar* m_LineUpUtility{};

private: // Wrappers for hooks
    // 0x648D60
    CTaskSimpleCarFallOut* Constructor(CVehicle* a, eTargetDoor b, CTaskUtilityLineUpPedWithCar* c) {
        this->CTaskSimpleCarFallOut::CTaskSimpleCarFallOut(a, b, c);
        return this;
    }

    // 0x648DE0
    CTaskSimpleCarFallOut* Destructor() {
        this->CTaskSimpleCarFallOut::~CTaskSimpleCarFallOut();
        return this;
    }
};
