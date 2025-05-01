#pragma once

#include "TaskComplex.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexSignalAtPed : public CTaskComplex {

public:
    CPed* m_pedToSignalAt = {};      // 0xC
    int32 m_initialPause = {};       // 0x10
    bool  m_playAnimAtEnd = {};      // 0x14
    bool  m_areAnimsReferenced = {}; // 0x15

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_COMPLEX_SIGNAL_AT_PED;

    CTaskComplexSignalAtPed(CPed* pedToSignalAt, int32 unused1 = -1, bool playAnimAtEnd = false);
    CTaskComplexSignalAtPed(const CTaskComplexSignalAtPed& o);
    ~CTaskComplexSignalAtPed();

    CTask*    Clone() const override { return new CTaskComplexSignalAtPed{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed * ped) override;
    CTask*    CreateFirstSubTask(CPed * ped) override;
    CTask*    ControlSubTask(CPed * ped) override;

private: // Wrappers for hooks
    // 0x660A30
    CTaskComplexSignalAtPed* Constructor(CPed* pedToSignalAt, int32 unused1, uint8 playAnimAtEnd) {
        this->CTaskComplexSignalAtPed::CTaskComplexSignalAtPed(pedToSignalAt, unused1, playAnimAtEnd);
        return this;
    }
    // 0x660AB0
    CTaskComplexSignalAtPed* Destructor() {
        this->CTaskComplexSignalAtPed::~CTaskComplexSignalAtPed();
        return this;
    }
};
