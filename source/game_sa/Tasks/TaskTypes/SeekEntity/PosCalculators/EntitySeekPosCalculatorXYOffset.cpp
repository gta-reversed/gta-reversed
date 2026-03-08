#include "StdInc.h"

#include "EntitySeekPosCalculatorXYOffset.h"

void CEntitySeekPosCalculatorXYOffset::InjectHooks() {
    RH_ScopedVirtualClass(CEntitySeekPosCalculatorXYOffset, 0x86F8F8, 1);
    RH_ScopedCategory("Tasks/TaskTypes/SeekPosCalculators");

    RH_ScopedVMTInstall(ComputeEntitySeekPos, 0x6946a0);
}

void CEntitySeekPosCalculatorXYOffset::ComputeEntitySeekPos(const CPed&, const CEntity& target, CVector& outPos) {
    outPos = target.GetMatrix().TransformPoint(m_offsetXY);
}
