#include "StdInc.h"

#include "EntitySeekPosCalculatorFixedPos.h"
#include "PedGeometryAnalyser.h"

void CEntitySeekPosCalculatorFixedPos::InjectHooks() {
    RH_ScopedVirtualClass(CEntitySeekPosCalculatorFixedPos, 0x0086FD18, 2);
    RH_ScopedCategory("Tasks/TaskTypes/SeekPosCalculators");

    RH_ScopedVMTInstall(ComputeEntitySeekPos, 0x694830);
}

CEntitySeekPosCalculatorFixedPos::CEntitySeekPosCalculatorFixedPos(const CVector& pos) :
    m_fixedPos{ pos }
{
}

void CEntitySeekPosCalculatorFixedPos::ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) {
    CPedGeometryAnalyser::ComputeClearTarget(seeker, m_fixedPos, outPos);
}
