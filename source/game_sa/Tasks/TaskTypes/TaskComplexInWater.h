#pragma once

#include "TaskComplex.h"

class CPed;
class CTaskComplexInWater;

class NOTSA_EXPORT_VTABLE CTaskComplexInWater : public CTaskComplex {

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_IN_WATER;

    CTaskComplexInWater() = default;
    CTaskComplexInWater(const CTaskComplexInWater&);
    ~CTaskComplexInWater() = default;

    CTask*    Clone() const override { return new CTaskComplexInWater{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x6350D0
    CTaskComplexInWater* Constructor() {
        this->CTaskComplexInWater::CTaskComplexInWater();
        return this;
    }

    // 0x635100
    CTaskComplexInWater* Destructor() {
        this->CTaskComplexInWater::~CTaskComplexInWater();
        return this;
    }
};
