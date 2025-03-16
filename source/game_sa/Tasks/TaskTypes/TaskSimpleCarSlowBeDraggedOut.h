#pragma once

#include "TaskSimple.h"

class CEvent;
class CPed;
class CVehicle;
class CTaskUtilityLineUpPedWithCar;
class CAnimBlendAssociation;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarSlowBeDraggedOut : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_CAR_SLOW_BE_DRAGGED_OUT;

    static void InjectHooks();

    CTaskSimpleCarSlowBeDraggedOut(CVehicle* target, eTargetDoor door, CTaskUtilityLineUpPedWithCar* lineUpUtility, bool isDraggedBySelf);
    ~CTaskSimpleCarSlowBeDraggedOut();

    CTask*    Clone() const override { return new CTaskSimpleCarSlowBeDraggedOut{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;
    bool      SetPedPosition(CPed * ped) override; 

private:
    void DoOpenCarDoor(CPed* ped);
    auto ComputeAnimID();
    void StartAnim(CPed* ped);
    static void FinishAnimCarSlowBeDraggedOutCB(CAnimBlendAssociation* anim, void* data);

private:
    bool                          m_HasAnimFinished{};
    CAnimBlendAssociation*        m_Anim{};
    CVehicle::Ref                 m_Vehicle{};
    eTargetDoor                   m_Door{};
    bool                          m_IsDraggedBySelf{};
    bool                          m_FrontAnim{};
    CTaskUtilityLineUpPedWithCar* m_LineUpUtility{};

private: // Wrappers for hooks
    // 0x648230
    CTaskSimpleCarSlowBeDraggedOut* Constructor(CVehicle* a, eTargetDoor b, CTaskUtilityLineUpPedWithCar*c, bool d) {
        this->CTaskSimpleCarSlowBeDraggedOut::CTaskSimpleCarSlowBeDraggedOut(a, b, c, d);
        return this;
    }

    // 0x6482C0
    CTaskSimpleCarSlowBeDraggedOut* Destructor() {
        this->CTaskSimpleCarSlowBeDraggedOut::~CTaskSimpleCarSlowBeDraggedOut();
        return this;
    }
};
