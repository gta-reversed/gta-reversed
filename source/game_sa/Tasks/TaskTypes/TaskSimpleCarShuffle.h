#pragma once

#include "TaskSimple.h"

class CVehicle;
class CAnimBlendAssociation;
class CEvent;
class CPed;
class CTaskUtilityLineUpPedWithCar;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarShuffle final : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_CAR_SHUFFLE;

    static void InjectHooks();

    CTaskSimpleCarShuffle(CVehicle* car, int32 targetDoor, CTaskUtilityLineUpPedWithCar* lineUpTask);
    CTaskSimpleCarShuffle(const CTaskSimpleCarShuffle&);
    ~CTaskSimpleCarShuffle();

    void StartAnim(const CPed* a2);

    CTask*    Clone() const override { return new CTaskSimpleCarShuffle{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;
    bool      SetPedPosition(CPed * ped) override; 

private:
    static void FinishAnimCarShuffleCB(CAnimBlendAssociation* assoc, void* task);

protected:
    bool                          m_bFinished{};
    CAnimBlendAssociation*        m_Anim{};
    CVehicle*                     m_Car{};
    int32                         m_TargetDoor{};
    CTaskUtilityLineUpPedWithCar* m_LineUpUtility{};

private: // Wrappers for hooks
    // 0x646810
    CTaskSimpleCarShuffle* Constructor(CVehicle* vehicle, int32 unknown, CTaskUtilityLineUpPedWithCar* task) {
        this->CTaskSimpleCarShuffle::CTaskSimpleCarShuffle(vehicle, unknown, task);
        return this;
    }

    // 0x646890
    CTaskSimpleCarShuffle* Destructor() {
        this->CTaskSimpleCarShuffle::~CTaskSimpleCarShuffle();
        return this;
    }
};
