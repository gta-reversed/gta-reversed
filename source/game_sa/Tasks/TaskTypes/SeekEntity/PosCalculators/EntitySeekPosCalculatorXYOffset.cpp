#include "StdInc.h"

#include "EntitySeekPosCalculatorXYOffset.h"

void CEntitySeekPosCalculatorXYOffset::InjectHooks() {
    RH_ScopedVirtualClass(CEntitySeekPosCalculatorXYOffset, 0x86F8F8, 1);
    RH_ScopedCategory("Tasks/TaskTypes/SeekPosCalculators");

    RH_ScopedVMTInstall(ComputeEntitySeekPos, 0x6946a0, { .reversed = false });
}

void CEntitySeekPosCalculatorXYOffset::ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) {
    return plugin::CallMethod<0x6946a0, CEntitySeekPosCalculatorXYOffset*, const CPed&, const CEntity&, CVector&>(this, seeker, target, outPos);
}
