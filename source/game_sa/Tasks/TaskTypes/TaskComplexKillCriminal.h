#pragma once

#include "TaskComplex.h"
#include "eCarMission.h"
#include "eCarDrivingStyle.h"

#include <extensions/EntityRef.hpp>

class CEvent;
class CTask;
class CCopPed;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexKillCriminal : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_KILL_CRIMINAL;

    CTaskComplexKillCriminal(CPed* criminal, bool randomize = false);
    CTaskComplexKillCriminal(const CTaskComplexKillCriminal& o); // NOTSA
    ~CTaskComplexKillCriminal() override;

    CTask*    Clone() const override { return new CTaskComplexKillCriminal{ *this }; } // 0x68CE50
    eTaskType GetTaskType() const override { return Type; } // 0x68BF20
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    CTask* CreateSubTask(eTaskType tt, CPed* ped, bool force = false);
    CPed*  FindNextCriminalToKill(CPed* ped, bool any);
    bool   ChangeTarget(CPed* newTarget);

    static void InjectHooks();

private:
    eTaskType GetSubTaskTypeWithoutCriminal(CPed* ped, const CCopPed* partner, eTaskType tt);   // NOTSA
    eTaskType GetSubTaskTypeAsPassenger(CPed* ped, CCopPed* partner) const;                     // NOTSA
    eTaskType GetSubTaskTypeForCriminalInCar(CPed* ped, const CCopPed* partner, eTaskType tt);  // NOTSA
    eTaskType GetSubTaskTypeForCriminalOnFoot(CPed* ped, const CCopPed* partner, eTaskType tt); // NOTSA
    eTaskType ProcessBikeDriveBy(CPed* ped, eTaskType tt) const;                                // NOTSA
    void      MakeCriminalsLeaveCar(CPed* ped) const;                                           // NOTSA
    void      MakeCriminalPassengersDoDriveBy(CPed* ped) const;                                 // NOTSA

private:
    notsa::EntityRef<CPed>    m_Criminal{};               //!< The ped to kill
    notsa::EntityRef<CCopPed> m_Cop{};                    //!< The cop this task belongs to (Set by `CreateFirstSubTask`)
    float                     m_TimeToGetOutOfCar{ 3.f }; //!< Counts down while the criminal's car stands still, at 0 the cop gets out
    bool                      m_Randomize{};              //!< Only chase a ped that isn't wanted if it's in a car driven madly (and then only by chance)
    bool                      m_HasFinished{};            //!< No criminal is left, the cop is on its way back to the car
    bool                      m_CantGetInCar{};           //!< Entering the car has failed
    eCarDrivingStyle          m_OrigDrivingMode{};        //!< NOTE: Not initialized by the original constructor
    eCarMission               m_OrigMission{};            //!< NOTE: Not initialized by the original constructor
    uint8                     m_OrigCruiseSpeed{};        //!< NOTE: Not initialized by the original constructor
    bool                      m_IsSetUp{};                //!< The 3 `m_Orig` values above have been stored
};
VALIDATE_SIZE(CTaskComplexKillCriminal, 0x20);
