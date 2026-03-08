#include "StdInc.h"

#include "EntitySeekPosCalculatorRadiusAngleOffset.h"
#include "PedGeometryAnalyser.h"

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

void CEntitySeekPosCalculatorRadiusAngleOffset::ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) {
    const auto heading = CGeneral::LimitRadianAngle(
        CGeneral::GetRadianAngleBetweenPoints(target.GetForward().x, target.GetForward().y, 0.0f, 0.0f)
    );
    const auto angle   = CGeneral::LimitRadianAngle(heading + m_angle);
    const auto seekPos = target.GetPosition() + CVector{
        -std::sin(angle) * m_radius,
         std::cos(angle) * m_radius,
         0.0f
    };
    CPedGeometryAnalyser::ComputeClearTarget(seeker, seekPos, outPos);
}
