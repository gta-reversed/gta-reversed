#pragma once

#include "TaskComplex.h"

class CVector;
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexGoToCarDoorAndStandStill : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL;

    CTaskComplexGoToCarDoorAndStandStill(
        CVehicle*  vehicle,
        eMoveState moveState,
        bool       bIsDriver,
        int32      targetSeat,
        float      targetRadius,
        float      slowDownDist,
        float      maxSeekDist,
        int32      maxSeekTime
    );
    CTaskComplexGoToCarDoorAndStandStill(const CTaskComplexGoToCarDoorAndStandStill&);
    ~CTaskComplexGoToCarDoorAndStandStill() override;

    CTask*    Clone() const override { return new CTaskComplexGoToCarDoorAndStandStill{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    auto      GetTargetPt()     const { return m_TargetPt; }
    void      SetTargetPt(CVector pt) { m_TargetPt = pt; }

    void      SetTargetDoor(int32 door) { m_TargetDoor = door; }
    auto      GetTargetDoor() const     { return m_TargetDoor; }

    bool      HasAchievedDoor() const { return m_bAchievedTargetDoor; }

    void      SetTryingToEnterInWater(bool v) { m_bTryingToEnterInWater = v; }

protected:
    CTask*    CreateSubTask(eTaskType taskType, CPed* ped);
    bool      IsVehicleInRange(const CPed& ped);
    void      ComputeRouteToDoor(const CPed& ped);
    bool      CalculateTargetDoor(CPed* ped, bool& bCanWaitForDoorToBeFree);
private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexGoToCarDoorAndStandStill* Constructor(CVehicle* vehicle, eMoveState a3, bool a4, int32 a5, float a6, float a7, float a8, int32 a9) { this->CTaskComplexGoToCarDoorAndStandStill::CTaskComplexGoToCarDoorAndStandStill(vehicle, a3, a4, a5, a6, a7, a8, a9); return this; }
    CTaskComplexGoToCarDoorAndStandStill* Destructor() { this->CTaskComplexGoToCarDoorAndStandStill::~CTaskComplexGoToCarDoorAndStandStill(); return this; }

public:
    CVehicle*    m_Vehicle{};
    eMoveState   m_MoveState{};
    bool         m_bIsDriver{};
    float        m_TargetRadius{};
    float        m_SlowDownDist{};
    float        m_MaxSeekDist{};
    int32        m_MaxSeekTime{};
    CTaskTimer   m_SeekTimer{};
    int32        m_TargetDoor{};
    CVector      m_TargetPt{};
    int32        m_TargetSeat{};
    bool         m_bTryingToEnterInWater{};
    bool         m_bAchievedTargetDoor{};
    CPointRoute* m_RouteToDoor{};
};
VALIDATE_SIZE(CTaskComplexGoToCarDoorAndStandStill, 0x50);
