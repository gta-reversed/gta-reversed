/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplex.h"
#include "Ped.h"

class NOTSA_EXPORT_VTABLE CTaskComplexProstituteSolicit : public CTaskComplex {
public:
    CPed*   m_pClient; // player only
    CVector m_vecVehiclePosn;
    int32   m_nLastSavedTime;
    int32   m_nNextTimeToCheckForSecludedPlace; // when we will scan nearby peds again
    int32   m_nLastPaymentTime;                 // when CJ will pay $2 again
    int16   m_nVehicleMovementTimer;            // wait some time and push vehicle again
    int16   m_nCurrentTimer;

    uint16 bSearchingForSecludedPlace : 1;
    uint16 bSexProcessStarted : 1;
    uint16 bTaskCanBeFinished : 1;
    uint16 bPedsCanPotentiallySeeThis : 1;
    uint16 bPedsCanSeeThis : 1;
    uint16 bCopsCanSeeThis : 1;
    uint16 b07 : 1;
    uint16 b08 : 1;

    uint16 bMoveCameraDown : 1;
    uint16 b10 : 1;
    uint16 bPlayerHasAcceptedSexProposition : 1;
    uint16 bVehicleShifted : 1;
    uint16 bSecludedPlaceMessageShown : 1;

public:
    static constexpr auto Type = TASK_COMPLEX_PROSTITUTE_SOLICIT;

    explicit CTaskComplexProstituteSolicit(CPed* client);
    ~CTaskComplexProstituteSolicit() override;

    eTaskType GetTaskType() const override { return Type; } // 0x661AE0
    CTask* Clone() const override { return new CTaskComplexProstituteSolicit(m_pClient); } // 0x6622F0
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    CTask* CreateSubTask(eTaskType taskType, CPed* prostitute);
    static void GetRidOfPlayerProstitute();
    static bool IsTaskValid(CPed* prostitute, CPed* ped);
};

VALIDATE_SIZE(CTaskComplexProstituteSolicit, 0x30);
