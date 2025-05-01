#pragma once

#include "TaskComplex.h"

class CEntity;

class NOTSA_EXPORT_VTABLE CTaskGangHasslePed : public CTaskComplex {
public:
    CPed* m_Ped;
    int32 dword10; // values are 0, 1, 2
    int32 m_RndMin;
    int32 m_RndMax;
    bool  m_bAnimBlockRefAdded;
    int32 m_nTime;
    int32 m_nSomeRandomShit;
    uint8 m_bFirstSubTaskInitialised;
    bool  m_bRefreshTime;

public:
    static constexpr auto Type = TASK_COMPLEX_GANG_HASSLE_PED;

    CTaskGangHasslePed(CPed* ped, int32 a3, int32 a4, int32 a5);
    ~CTaskGangHasslePed() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskGangHasslePed(m_Ped, dword10, m_RndMin, m_RndMax); } // 0x6620D0
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskGangHasslePed* Constructor(CPed* ped, int32 a3, int32 a4, int32 a5) { this->CTaskGangHasslePed::CTaskGangHasslePed(ped, a3, a4, a5); return this; }
    CTaskGangHasslePed* Destructor() { this->CTaskGangHasslePed::~CTaskGangHasslePed(); return this; }
};
VALIDATE_SIZE(CTaskGangHasslePed, 0x2C);
