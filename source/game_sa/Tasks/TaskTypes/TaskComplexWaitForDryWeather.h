#pragma once

#include "TaskComplex.h"

class CPed;
class CTaskComplexWaitForDryWeather;

class NOTSA_EXPORT_VTABLE CTaskComplexWaitForDryWeather : public CTaskComplex {

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_WAIT_FOR_DRY_WEATHER;

    CTaskComplexWaitForDryWeather() = default;
    CTaskComplexWaitForDryWeather(const CTaskComplexWaitForDryWeather&);
    ~CTaskComplexWaitForDryWeather() = default;

    CTask*    Clone() const override { return new CTaskComplexWaitForDryWeather{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x632800
    CTaskComplexWaitForDryWeather* Constructor() {
        this->CTaskComplexWaitForDryWeather::CTaskComplexWaitForDryWeather();
        return this;
    }
    // 0x632830
    CTaskComplexWaitForDryWeather* Destructor() {
        this->CTaskComplexWaitForDryWeather::~CTaskComplexWaitForDryWeather();
        return this;
    }
};
