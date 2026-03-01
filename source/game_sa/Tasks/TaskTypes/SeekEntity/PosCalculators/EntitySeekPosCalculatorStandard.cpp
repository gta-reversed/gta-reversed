#include "StdInc.h"

#include "EntitySeekPosCalculatorStandard.h"

void CEntitySeekPosCalculatorStandard::InjectHooks() {
    RH_ScopedVirtualClass(CEntitySeekPosCalculatorStandard, 0x859DC4, 2);
    RH_ScopedCategory("Tasks/TaskTypes/SeekPosCalculators");

    RH_ScopedVMTInstall(ComputeEntitySeekPos, 0x46af20);
}

/*
// 0x46AC10
CEntitySeekPosCalculatorStandard::CEntitySeekPosCalculatorStandard(CEntity* entity, int32 time, int32 a3, float radius, float a5, float a6, bool a7, bool a8) : CTaskComplex() {
    plugin::CallMethod<0x46AC10, CEntitySeekPosCalculatorStandard*, CEntity*, int32, int32, float, float, float, bool, bool>(this, entity, time, a3, radius, a5, a6, a7, a8);
}
*/

void CEntitySeekPosCalculatorStandard::ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) {
    outPos = target.GetPosition();
}
