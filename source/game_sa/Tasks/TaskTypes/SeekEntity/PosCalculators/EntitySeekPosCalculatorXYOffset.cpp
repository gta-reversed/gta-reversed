#include "StdInc.h"

#include "EntitySeekPosCalculatorXYOffset.h"
#include "PedGeometryAnalyser.h"

void CEntitySeekPosCalculatorXYOffset::InjectHooks() {
    RH_ScopedVirtualClass(CEntitySeekPosCalculatorXYOffset, 0x86F8F8, 1);
    RH_ScopedCategory("Tasks/TaskTypes/SeekPosCalculators");

    RH_ScopedVMTInstall(ComputeEntitySeekPos, 0x6946a0);
}

// 0x6946A0
void CEntitySeekPosCalculatorXYOffset::ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) {
    CPedGeometryAnalyser::ComputeClearTarget(seeker, target.GetPosition() + m_offsetXY, outPos);
}
