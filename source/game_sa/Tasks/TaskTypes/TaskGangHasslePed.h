#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"

class CEntity;

class NOTSA_EXPORT_VTABLE CTaskGangHasslePed : public CTaskComplex {
public:
    //! What the ped does while hassling
    enum eHassleType : int32 {
        HASSLE_EYE              = 0, //!< Eye the ped (e.g. a "sexy" ped)
        HASSLE_ABUSE            = 1, //!< Verbally abuse the ped
        HASSLE_ABUSE_AND_ATTACK = 2, //!< Verbally abuse the ped, then attack them once the timer is up
    };

public:
    CPed*       m_Ped;                //!< 0xC  - The ped being hassled
    eHassleType m_HassleType;         //!< 0x10
    int32       m_RndMin;             //!< 0x14 - Min. hassle time [ms]
    int32       m_RndMax;             //!< 0x18 - Max. hassle time [ms]
    bool        m_bAnimBlockRefAdded; //!< 0x1C
    CTaskTimer  m_Timer;              //!< 0x20 - Hassle timer (Random time between `m_RndMin` and `m_RndMax`)

public:
    static constexpr auto Type = TASK_COMPLEX_GANG_HASSLE_PED;

    CTaskGangHasslePed(CPed* ped, int32 hassleType, int32 rndMin, int32 rndMax);
    ~CTaskGangHasslePed() override;

    eTaskType GetTaskType() const override { return Type; }

    CTask* Clone() const override { return new CTaskGangHasslePed(m_Ped, m_HassleType, m_RndMin, m_RndMax); } // 0x6620D0
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

private:
    //! @notsa
    CTask* CreateSeekTask(float radius) const;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskGangHasslePed* Constructor(CPed* ped, int32 hassleType, int32 rndMin, int32 rndMax) {
        this->CTaskGangHasslePed::CTaskGangHasslePed(ped, hassleType, rndMin, rndMax);
        return this;
    }
    CTaskGangHasslePed* Destructor() { this->CTaskGangHasslePed::~CTaskGangHasslePed(); return this; }
};
VALIDATE_SIZE(CTaskGangHasslePed, 0x2C);
