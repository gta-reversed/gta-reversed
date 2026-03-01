#pragma once

#include "TaskComplexEvasiveDiveAndGetUp.h"

class CPed;
class CTaskComplexDiveFromAttachedEntityAndGetUp;

class NOTSA_EXPORT_VTABLE CTaskComplexDiveFromAttachedEntityAndGetUp final : public CTaskComplexEvasiveDiveAndGetUp {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP;

    static void InjectHooks();

    CTaskComplexDiveFromAttachedEntityAndGetUp(int32 nTimeOnGround = 0);
    CTaskComplexDiveFromAttachedEntityAndGetUp(const CTaskComplexDiveFromAttachedEntityAndGetUp&);
    ~CTaskComplexDiveFromAttachedEntityAndGetUp() = default;

    CTask*    Clone() const override { return new CTaskComplexDiveFromAttachedEntityAndGetUp{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    // ControlSubTask was overwritten too, but it does the same as in `CTaskComplexEvasiveDiveAndGetUp`

private: // Wrappers for hooks
    // 0x492E20
    CTaskComplexDiveFromAttachedEntityAndGetUp* Constructor(int32 nTimeOnGround) {
        this->CTaskComplexDiveFromAttachedEntityAndGetUp::CTaskComplexDiveFromAttachedEntityAndGetUp(nTimeOnGround);
        return this;
    }

    // 0x492F10
    CTaskComplexDiveFromAttachedEntityAndGetUp* Destructor() {
        this->CTaskComplexDiveFromAttachedEntityAndGetUp::~CTaskComplexDiveFromAttachedEntityAndGetUp();
        return this;
    }
};
