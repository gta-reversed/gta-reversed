#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"
#include "Ped.h"


class NOTSA_EXPORT_VTABLE CTaskGangHasslePed : public CTaskComplex {
public:
    //! What the ped does while hassling
    enum class eHassleType : int32 {
        EYE              = 0, //!< Eye the ped (e.g. a "sexy" ped)
        ABUSE            = 1, //!< Verbally abuse the ped
        ABUSE_AND_ATTACK = 2, //!< Verbally abuse the ped, then attack them once the timer is up
    };

public:
    static constexpr auto Type = TASK_COMPLEX_GANG_HASSLE_PED;

    CTaskGangHasslePed(CPed* ped, eHassleType hassleType, int32 rndMin, int32 rndMax);
    CTaskGangHasslePed(const CTaskGangHasslePed&);
    ~CTaskGangHasslePed() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskGangHasslePed{ *this }; } // 0x6620D0
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

private:
    CTask* CreateSubTask(eTaskType taskType, CPed* ped); // notsa

public:
    static void InjectHooks();

public:
    CPed::Ref   m_PedToHassle{};        //!< 0xC  - The ped being hassled
    eHassleType m_HassleType{};         //!< 0x10
    int32       m_RndMin{};             //!< 0x14 - Min. hassle time [ms]
    int32       m_RndMax{};             //!< 0x18 - Max. hassle time [ms]
    bool        m_bAnimBlockRefAdded{}; //!< 0x1C
    CTaskTimer  m_HassleTimer{};        //!< 0x20 - Hassle timer (Random time between `m_RndMin` and `m_RndMax`)
};

VALIDATE_SIZE(CTaskGangHasslePed, 0x2C);
