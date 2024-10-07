#include "StdInc.h"

#include "EntitySeekPosCalculatorRadiusAngleOffset.h"

void CEntitySeekPosCalculatorRadiusAngleOffset::InjectHooks() {
    RH_ScopedVirtualClass(CEntitySeekPosCalculatorRadiusAngleOffset, 0x85A384, 2);
    RH_ScopedCategory("Tasks/TaskTypes/SeekPosCalculators");

    RH_ScopedVMTInstall(ComputeEntitySeekPos, 0x6946f0, { .reversed = false });
}

CEntitySeekPosCalculatorRadiusAngleOffset::CEntitySeekPosCalculatorRadiusAngleOffset(float radius, float angle) :
    m_radius{ radius },
    m_angle{ angle }
{
}

void CEntitySeekPosCalculatorRadiusAngleOffset::ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) {
    return plugin::CallMethod<0x6946f0, CEntitySeekPosCalculatorRadiusAngleOffset*, const CPed&, const CEntity&, CVector&>(this, seeker, target, outPos);
}
