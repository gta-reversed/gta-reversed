#include "StdInc.h"
#include "Curves.h"

void CCurves::InjectHooks() {
    RH_ScopedClass(CCurves);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(TestCurves, 0x43C600);
    RH_ScopedGlobalInstall(DistForLineToCrossOtherLine, 0x43C610);
    RH_ScopedGlobalInstall(CalcSpeedVariationInBend, 0x43C660);
    RH_ScopedGlobalInstall(CalcSpeedScaleFactor, 0x43C710);
    RH_ScopedGlobalInstall(CalcCorrectedDist, 0x43C880);
    RH_ScopedGlobalInstall(CalcCurvePoint, 0x43C900);
}

// unused
// 0x43C600
void CCurves::TestCurves() {
    // https://github.com/ifarbod/sa-curves-test
}

// 0x43C610
float CCurves::DistForLineToCrossOtherLine(CVector2D originA, CVector2D dirA, CVector2D originB, CVector2D dirB) {
    const auto crossAB = dirA.Cross(dirB);
    if (crossAB == 0.f) {
        return -1.f;
    }
    return (originA - originB).Cross(dirB) * (-1.f / crossAB);
}

// 0x43C660
float CCurves::CalcSpeedVariationInBend(const CVector2D& ptA, const CVector2D& ptB, CVector2D dirA, CVector2D dirB) {
    const auto dot = dirA.Dot(dirB);
    if (dot <= 0.f) { // Directions point in the opposite direction
        return 1.f / 3.f;
    } else if (dot <= 0.7f) { // Point in different directions, with ~45 deg tolerance
        return (1.f - dot / 0.7f) / 3.f;
    }

    // Points in the same dir with ~45 deg tolerance
    return CCollision::DistToMathematicalLine2D(ptB.x, ptB.y, dirB.x, dirB.y, ptA.x, ptA.y) / (ptA - ptB).Magnitude() / 3.f;
}

// 0x43C710
float CCurves::CalcSpeedScaleFactor(const CVector2D& ptA, const CVector2D& ptB, CVector2D dirA, CVector2D dirB) {
    if (const auto distToCrossAB = DistForLineToCrossOtherLine(ptA, dirA, ptB, dirB); distToCrossAB > 0.f) {
        if (const auto distToCrossBA = DistForLineToCrossOtherLine(ptB, dirB, ptA, dirA); distToCrossBA > 0.f) {
            const auto d = std::min({ distToCrossAB, distToCrossBA, 5.f });
            return 2.f * d + (distToCrossAB - d) + (distToCrossBA - d);
        }
    }
    return (ptA - ptB).Magnitude() / (1.f - CalcSpeedVariationInBend(ptA, ptB, dirA, dirB));
}

// 0x43C880
float CCurves::CalcCorrectedDist(float curr, float total, float variance, float& outT) {
    if (total >= 0.00001f) {
        const auto prog = curr / total;
        outT            = 0.5f - std::cos(PI * prog) * 0.5f;
        return std::sin(prog * TWO_PI) * (total / TWO_PI) * variance
            + (1.f - (variance * 2.f) + 1.f) / 2.f * curr;
    } else {
        outT = 0.5f;
        return 0.f;
    }
}

// NOTE: This code has inaccuracies and therefore the outPos may vary slightly.
// 0x43C900
void CCurves::CalcCurvePoint(
    const CVector& ptA3D,
    const CVector& ptB3D,
    const CVector& dirA3D,
    const CVector& dirB3D,
    float          timeProgress,
    int32          totalTimeMs,

    CVector&       outPos,
    CVector&       outSpeed
) {
    // Clamp time progress to [0, 1] range
    timeProgress = std::clamp(timeProgress, 0.f, 1.f);

    // Convert 3D vectors to 2D for planar calculations
    const CVector2D ptA(ptA3D);
    const CVector2D ptB(ptB3D);
    const CVector2D dirA(dirA3D);
    const CVector2D dirB(dirB3D);

    // Calculate 2D distance between start and end points
    float distAB = DistanceBetweenPoints2D(ptA, ptB);

    // Calculate speed variation factor for the curve
    float speedVar = CalcSpeedVariationInBend(ptA, ptB, dirA, dirB);

    // Calculate parameters for curve bending
    const float crossDirAB = dirA.Cross(dirB);
    const float distAlongDirAB = crossDirAB == 0.f
        ? -1.f
        : (ptA - ptB).Cross(dirB) / -crossDirAB;

    const float crossDirBA = dirB.Cross(dirA);
    const float distAlongDirBA = crossDirBA == 0.f
        ? -1.f
        : dirA.Cross(ptB - ptA) / -crossDirBA;

    if (distAB > 0.f) {
        if (distAlongDirAB > 0.f && distAlongDirBA > 0.f) { // Case 1: Smooth curve with bend
            const float d              = std::min({ distAlongDirAB, distAlongDirBA, 5.f });
            const float distBeforeBend = distAlongDirAB - d;
            const float distAfterBend  = distAlongDirBA - d;
            const float bendLength     = d * 2.f;
            const float totalDist      = distBeforeBend + bendLength + distAfterBend;
            const float currDist       = totalDist * timeProgress;

            if (currDist <= distBeforeBend) { // Segment before the bend: move along start direction
                outPos = ptA3D + dirA3D * currDist;
            } else if (currDist <= distBeforeBend + bendLength) { // Bend segment: interpolate between start and end curves
                const float   t    = (currDist - distBeforeBend) / bendLength;
                const CVector pos1 = ptA3D + dirA3D * (distBeforeBend + t * d);
                const CVector pos2 = ptB3D - dirB3D * (distAfterBend + (1.f - t) * d);
                outPos             = Lerp(pos1, pos2, t);
            } else { // Segment after the bend: move along end direction
                const float remainingDist = currDist - (distBeforeBend + bendLength);
                outPos                    = ptB3D - dirB3D * (distAfterBend - remainingDist);
            }
            distAB = totalDist;
        } else { // Case 2: Straight path with speed variation or sharp turn
            const float totalDistAdjusted = distAB / (1.f - speedVar);
            const float currDist          = totalDistAdjusted * timeProgress;
            float       outT;
            const float correctedDist = CalcCorrectedDist(currDist, totalDistAdjusted, speedVar, outT);

            const CVector posStart = ptA3D + dirA3D * correctedDist;
            const CVector posEnd   = ptB3D + dirB3D * (correctedDist - distAB);

            outPos = dirA.Dot(dirB) <= -0.99f && timeProgress >= 0.5f // Handle sharp turn (opposite directions)
                ? ptB3D
                : Lerp(posStart, posEnd, speedVar); // Linear interpolation based on speed variation
        }
    } else { // No distance between points, stay at start position
        outPos = ptA3D;
    }

    // Ensure exact start and end points at boundaries
    if (timeProgress == 0.f) {
        outPos = ptA3D;
    } else if (timeProgress == 1.f) {
        outPos = ptB3D;
    }

    // Calculate speed vector
    outSpeed   = Lerp(dirA3D, dirB3D, timeProgress) * (distAB / totalTimeMs * 0.001f);
    outSpeed.z = 0.f; // Speed is planar (2D) as in original implementation
}
