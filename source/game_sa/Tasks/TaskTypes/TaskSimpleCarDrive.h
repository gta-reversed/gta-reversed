#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

class CVehicle;
class CTaskUtilityLineUpPedWithCar;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarDrive : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_CAR_DRIVE;

    explicit CTaskSimpleCarDrive(CVehicle* vehicle, CTaskUtilityLineUpPedWithCar* utilityTask = {}, bool updateCurrentVehicle = {});
    CTaskSimpleCarDrive(const CTaskSimpleCarDrive&);
    ~CTaskSimpleCarDrive() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override;
    bool      ProcessPed(CPed* ped) override;
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      SetPedPosition(CPed* ped) override;

    void TriggerIK(CPed* ped) const;
    void UpdateBopping();
    void StartBopping(CPed* ped);
    void ProcessHeadBopping(CPed* ped, bool canBop, float camDistSq);
    void ProcessArmBopping(CPed* ped, bool canBop, float camDistSq);
    void ProcessBopping(CPed* ped, bool canBop);

    [[nodiscard]] auto GetVehicle() const { return m_Vehicle; }

    static void InjectHooks();

private:
    void        StartRollingDoor(CPed* ped);
    static void FinishRollingDoorCB(CAnimBlendAssociation* anim, void* data);

protected:
    CVehicle*                     m_Vehicle{};
    CAnimBlendAssociation*        m_AnimCloseDoorRolling{};
    CTaskUtilityLineUpPedWithCar* m_TaskUtilityLineUpPedWithCar{};
    CTaskTimer                    m_NoDriverTimer{}; //!< Passenger in the front seat of a car without a driver takes it over after this
    int32                         m_BoppingStartTime{ -1 };
    int32                         field_24;
    int32                         m_BoppingInterval;       //!< Length of one beat [ms]
    float                         m_BoppingProgress;       //!< Progress of the current beat [0, 1]
    int32                         m_BoppingCompletedTimes; //!< Number of beats done, modulo 2
    int32                         m_HeadBoppingStartTime;
    int32                         m_HeadBoppingDirection;
    float                         m_HeadBoppingOrientation{};
    float                         m_HeadBoppingMultiplier{};
    float                         m_HeadBoppingFactor{};
    int32                         m_ArmBoppingStartTime;
    int32                         m_UpsideDownTime{}; //!< How long the car has been upside down [ms]
    CTaskTimer                    m_CopCarStolenTimer{};

    bool m_IsHeadBopping : 1          = false;
    bool m_IsArmBopping : 1           = false;
    bool m_UpdateCurrentVehicle : 1   = false; //!< Update `m_Vehicle` to the vehicle the ped is in
    bool m_b08 : 1                    = true;
    bool m_IsPassengerAnimBlended : 1 = false; //!< Bike passenger anim was blended
    bool m_IsRollingDoor : 1          = false; //!< Playing the roll-door anim (`m_AnimCloseDoorRolling`)
};
VALIDATE_SIZE(CTaskSimpleCarDrive, 0x60);
