#include "StdInc.h"

#include "EntitySeekPosCalculatorRadiusAngleOffset.h"

void CEntitySeekPosCalculatorRadiusAngleOffset::InjectHooks() {
    RH_ScopedVirtualClass(CEntitySeekPosCalculatorRadiusAngleOffset, 0x85A384, 2);
    RH_ScopedCategory("Tasks/TaskTypes/SeekPosCalculators");

    RH_ScopedVMTInstall(ComputeEntitySeekPos, 0x6946f0);
}

CEntitySeekPosCalculatorRadiusAngleOffset::CEntitySeekPosCalculatorRadiusAngleOffset(float radius, float angle) :
    m_radius{ radius },
    m_angle{ angle }
{
}

void CEntitySeekPosCalculatorRadiusAngleOffset::ComputeEntitySeekPos(const CPed&, const CEntity& target, CVector& outPos) {
    const auto right   = std::sin(m_angle) * m_radius;
    const auto forward = std::cos(m_angle) * m_radius;
    outPos = target.GetPosition()
        + target.GetRight() * right
        + target.GetForward() * forward;
}
