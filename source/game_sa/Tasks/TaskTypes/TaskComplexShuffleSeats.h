#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CPed;
class CVehicle;
class CEvent;
class CTaskSimpleCarSetPedInAsDriver;
class CTaskUtilityLineUpPedWithCar;
class CTaskComplexShuffleSeats;

class NOTSA_EXPORT_VTABLE CTaskComplexShuffleSeats : public CTaskComplex {
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_SHUFFLE_SEATS;

    CTaskComplexShuffleSeats(CVehicle* veh);
    CTaskComplexShuffleSeats(const CTaskComplexShuffleSeats&);
    ~CTaskComplexShuffleSeats();

    CTask* CreateSubTask(eTaskType a2, CPed* ped);
    void   CreateTaskUtilityLineUpPedWithCar(CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexShuffleSeats{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x63D130
    CTaskComplexShuffleSeats* Constructor(CVehicle* veh) {
        this->CTaskComplexShuffleSeats::CTaskComplexShuffleSeats(veh);
        return this;
    }
    // 0x63D1A0
    CTaskComplexShuffleSeats* Destructor() {
        this->CTaskComplexShuffleSeats::~CTaskComplexShuffleSeats();
        return this;
    }

private:
    CVehicle::Ref                 m_Veh{};
    int32                         m_OriginDoor{};
    int32                         m_TargetDoor{};
    CVector                       m_TargetDoorPos{};
    CTaskUtilityLineUpPedWithCar* m_TaskUtilityLineUpPedWithCar{};
    bool                          m_bAbort{};
};
