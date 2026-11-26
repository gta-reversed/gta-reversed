#pragma once

#include "TaskComplex.h"
#include "Ped.h"


class NOTSA_EXPORT_VTABLE CTaskComplexSignalAtPed : public CTaskComplex {
public:
    constexpr static auto Type = eTaskType::TASK_COMPLEX_SIGNAL_AT_PED;

    CTaskComplexSignalAtPed(CPed* pedToSignalAt, int32 initialPause = -1, bool playAnimAtEnd = false);
    CTaskComplexSignalAtPed(const CTaskComplexSignalAtPed& o);
    ~CTaskComplexSignalAtPed() override = default;

    CTask*    Clone() const override { return new CTaskComplexSignalAtPed{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

public:
    static void InjectHooks();

private:
    CPed::Ref m_PedToSignalAt{};      //< 0xC
    int32     m_InitialPause{};       //< 0x10
    bool      m_PlayAnimAtEnd{};      //< 0x14
    bool      m_AreAnimsReferenced{}; //< 0x15
};
