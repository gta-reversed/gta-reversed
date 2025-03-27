#pragma once

#include "TaskComplexWander.h"
#include "eMoveState.h"

class CPed;
class CTaskComplexWanderFlee;

class NOTSA_EXPORT_VTABLE CTaskComplexWanderFlee : public CTaskComplexWander {
public:
    static void InjectHooks();

    CTaskComplexWanderFlee(eMoveState moveState, uint8 dir);
    CTaskComplexWanderFlee(const CTaskComplexWanderFlee&);
    ~CTaskComplexWanderFlee() = default;

    CTask*      Clone() const override { return new CTaskComplexWanderFlee{ *this }; }
    eWanderType GetWanderType() override { return WANDER_TYPE_FLEE; }
    void        ScanForStuff(CPed* ped) override { /*nop*/ }

private: // Wrappers for hooks
    // 0x65B320
    CTaskComplexWanderFlee* Constructor(eMoveState moveState, uint8 dir) {
        this->CTaskComplexWanderFlee::CTaskComplexWanderFlee(moveState, dir);
        return this;
    }

    // 0x65B370
    CTaskComplexWanderFlee* Destructor() {
        this->CTaskComplexWanderFlee::~CTaskComplexWanderFlee();
        return this;
    }
};
