#include "StdInc.h"

#include "EntitySeekPosCalculatorXYOffset.h"

void CEntitySeekPosCalculatorXYOffset::InjectHooks() {
    RH_ScopedVirtualClass(CEntitySeekPosCalculatorXYOffset, 0x86F8F8, 1);
    RH_ScopedCategory("Tasks/TaskTypes/SeekPosCalculators");

    RH_ScopedVMTInstall(ComputeEntitySeekPos, 0x6946a0);
}

void CEntitySeekPosCalculatorXYOffset::ComputeEntitySeekPos(const CPed&, const CEntity& target, CVector& outPos) {
    outPos = target.GetPosition()
        + target.GetRight() * m_offsetXY.x
        + target.GetForward() * m_offsetXY.y
        + CVector{ 0.0f, 0.0f, m_offsetXY.z };
}
