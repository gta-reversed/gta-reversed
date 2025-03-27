#pragma once

#include "Base.h"
#include "TaskComplex.h"
#include "Vector.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexDestroyCarMelee : public CTaskComplex {
public:
    bool      m_AbortAtLeisure{};
    bool      m_HasNewTarget{};
    CVehicle* m_VehToDestroy{};
    CVector   m_VehPos{};
    float     m_MaxAtkRange{};
    float     m_MaxAtkAngleRad{};
    float     m_MaxArriveRange{};
    uint32    m_LastAtkTime{};
    int32     m_PauseTimer{ -1 };

public:
    static void InjectHooks();

    static constexpr eTaskType Type = TASK_COMPLEX_DESTROY_CAR_MELEE;

    explicit CTaskComplexDestroyCarMelee(CVehicle* vehToDestroy);
    ~CTaskComplexDestroyCarMelee() override;

    void CalculateSearchPositionAndRanges(CPed* ped);
    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexDestroyCarMelee(m_VehToDestroy); } // 0x6235A0
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private:
    CTaskComplexDestroyCarMelee* Constructor(CVehicle* vehToDestroy) { this->CTaskComplexDestroyCarMelee::CTaskComplexDestroyCarMelee(vehToDestroy); return this; }
    CTaskComplexDestroyCarMelee* Destructor() { this->CTaskComplexDestroyCarMelee::~CTaskComplexDestroyCarMelee(); return this; }
};
VALIDATE_SIZE(CTaskComplexDestroyCarMelee, 0x34);
