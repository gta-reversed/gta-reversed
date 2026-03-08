#include "StdInc.h"

#include "EntitySeekPosCalculatorXYOffset.h"
#include "PedGeometryAnalyser.h"

void CEntitySeekPosCalculatorXYOffset::InjectHooks() {
    RH_ScopedVirtualClass(CEntitySeekPosCalculatorXYOffset, 0x86F8F8, 1);
    RH_ScopedCategory("Tasks/TaskTypes/SeekPosCalculators");

    RH_ScopedVMTInstall(ComputeEntitySeekPos, 0x6946a0);
}

void CEntitySeekPosCalculatorXYOffset::ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) {
    const auto seekPos = target.GetPosition() + m_offsetXY;
    CPedGeometryAnalyser::ComputeClearTarget(seeker, seekPos, outPos);
}
