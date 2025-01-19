/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplex.h"
#include "Ped.h"

class NOTSA_EXPORT_VTABLE CTaskComplexProstituteSolicit final : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_PROSTITUTE_SOLICIT;

    static void InjectHooks();

    explicit CTaskComplexProstituteSolicit(CPed* client);
    ~CTaskComplexProstituteSolicit() override;

    eTaskType GetTaskType() const override { return Type; } // 0x661AE0
    CTask* Clone() const override { return new CTaskComplexProstituteSolicit(m_Client); } // 0x6622F0
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    CTask* CreateSubTask(eTaskType taskType, CPed* prostitute);
    static void GetRidOfPlayerProstitute();
    static bool IsTaskValid(CPed* prostitute, CPed* ped);

private:
    CPed::Ref m_Client{}; //!< player only
    CVector   m_InitialVehiclePos{};
    uint32    m_LastTimeCarMoving{};
    uint32    m_NextTimeToScanForPeds{}; //!< when we will scan nearby peds again
    uint32    m_SecondsCounter{};        //!< when CJ will pay $2 again
    int16     m_ShaggingFreq{ 850 };     //!< Wait some time and push vehicle again
    int16     m_ShagTimeLeft{};          //!< Time to fuck (ms?)
    bool      m_WaitingToStopInSecludedSpot : 1 {};
    bool      m_HavingSex : 1 {};
    bool      m_HadEnough : 1 {};
    bool      m_PedsNearby : 1 {};
    bool      m_PedsReallyNear : 1 {};
    bool      m_CopNearby : 1 {};
    bool      m_FirstTime : 1 { true };
    bool      m_WaitAfterEnteringCar : 1 { true };
    bool      m_SexCamModeEnabled : 1 {};
    bool      m_DoSexAudio : 1 { true };
    bool      m_AgreedToSex : 1 {};
    bool      m_NoMoreHealth : 1 {};
    bool      m_SecludedMessageShown : 1 {};

private:
    // 0x661A60
    CTaskComplexProstituteSolicit* Constructor(CPed* client) {
        this->CTaskComplexProstituteSolicit::CTaskComplexProstituteSolicit(client);
        return this;
    }

    // 0x661AF0
    CTaskComplexProstituteSolicit* Destructor() {
        this->CTaskComplexProstituteSolicit::~CTaskComplexProstituteSolicit();
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexProstituteSolicit, 0x30);
