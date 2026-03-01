#pragma once

#include "AnimationEnums.h"
#include "TaskSimpleRunTimedAnim.h"

class CPed;
class CTaskSimpleHandsUp;

class NOTSA_EXPORT_VTABLE CTaskSimpleHandsUp : public CTaskSimpleRunTimedAnim {
public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_HANDS_UP;

    CTaskSimpleHandsUp(uint32 duration) : CTaskSimpleRunTimedAnim{ANIM_GROUP_DEFAULT, ANIM_ID_HANDSUP, 4.0, -4.0, duration, Type, "HandsUp", 0} {}
    CTaskSimpleHandsUp(const CTaskSimpleHandsUp& o) : CTaskSimpleHandsUp{o.m_durationMs} {}
    ~CTaskSimpleHandsUp() override = default;

    CTask* Clone() const override { return new CTaskSimpleHandsUp{*this}; }
    virtual bool IsInterruptable(CPed const* ped) { return false; }

private: // Wrappers for hooks
    // 0x48E970
    CTaskSimpleHandsUp* Constructor(int32 nTime) {
        this->CTaskSimpleHandsUp::CTaskSimpleHandsUp(nTime);
        return this;
    }
    // 0x48EA60
    CTaskSimpleHandsUp* Destructor() {
        this->CTaskSimpleHandsUp::~CTaskSimpleHandsUp();
        return this;
    }
};
