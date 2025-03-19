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
float CCurves::CalcSpeedVariationInBend(CVector2D const& startCoors, CVector2D const& endCoors, CVector2D startDir, CVector2D endDir) {
    const auto dirDot = startDir.Dot(endDir);
    if (dirDot <= 0.f) { // Directions point in the opposite direction
        return 1.f / 3.f;
    } else if (dirDot <= 0.7f) { // Point in different directions, with ~45 deg tolerance
        return (1.f - dirDot / 0.7f) / 3.f;
    }

    // Points in the same dir with ~45 deg tolerance
    return CCollision::DistToMathematicalLine2D(endCoors.x, endCoors.y, endDir.x, endDir.y, startCoors.x, startCoors.y) / (startCoors - endCoors).Magnitude() / 3.f;
}

// 0x43C710
float CCurves::CalcSpeedScaleFactor(CVector2D const& startCoors, CVector2D const& endCoors, CVector2D startDir, CVector2D endDir) {
    if (const auto startCrossEndDist = DistForLineToCrossOtherLine(startCoors, startDir, endCoors, endDir); startCrossEndDist > 0.f) {
        if (const auto endCrossStartDist = DistForLineToCrossOtherLine(endCoors, endDir, startCoors, startDir); endCrossStartDist > 0.f) {
            const auto minDist = std::min({ startCrossEndDist, endCrossStartDist, 5.f }); // Min. of the 2 distances, but not higher than 5
            return 2.f * minDist + (startCrossEndDist - minDist) + (endCrossStartDist - minDist);
        }
    }
    return (startCoors - endCoors).Magnitude() / (1.f - CalcSpeedVariationInBend(startCoors, endCoors, startDir, endDir));
}

// 0x43C880
float CCurves::CalcCorrectedDist(float curr, float total, float variance, float& outT) {
    if (total >= 0.00001f) {
        const auto prog = curr / total;
        outT = 0.5f - std::cos(PI * prog) * 0.5f;
        return std::sin(prog * TWO_PI) * (total / TWO_PI) * variance
            + (1.f - (variance * 2.f) + 1.f) / 2.f * curr;
    } else {
        outT = 0.5f;
        return 0.f;
    }
}

// NOTE: This code has inaccuracies and therefore the outPos may vary slightly.
// 0x43C900
void CCurves::CalcCurvePoint(const CVector& startPos, const CVector& endPos, const CVector& startDir, const CVector& endDir, float timeProgress, int32 totalTimeMs, CVector& outPos, CVector& outSpeed) {
    // Clamp time progress to [0, 1] range
    timeProgress = std::clamp(timeProgress, 0.f, 1.f);

    // Convert 3D vectors to 2D for planar calculations
    const CVector2D startPos2D(startPos);
    const CVector2D endPos2D(endPos);
    const CVector2D startDir2D(startDir);
    const CVector2D endDir2D(endDir);

    // Calculate 2D distance between start and end points
    float startEndDist = DistanceBetweenPoints2D(startPos2D, endPos2D);
    
    // Calculate speed variation factor for the curve
    float speedVar = CalcSpeedVariationInBend(startPos, endPos, startDir, endDir);

    // Calculate parameters for curve bending
    const float v12 = startDir2D.Cross(endDir2D);
    const float a3 = v12 == 0.f ? -1.f : (startPos2D - endPos2D).Cross(endDir2D) / -v12;
    const float v13 = -(startDir2D.Cross(endDir2D));
    const float v14 = v13 == 0.f ? -1.f : startDir2D.Cross(endPos2D - startPos2D) / -v13;

    // Check if directions are nearly opposite
    const float dot = startDir2D.Dot(endDir2D);

    if (startEndDist > 0.f) {
        if (a3 > 0.f && v14 > 0.f) {
            // Case 1: Smooth curve with bend
            const float minDist = std::min({a3, v14, 5.f});
            const float distBeforeBend = a3 - minDist;
            const float distAfterBend = v14 - minDist;
            const float bendLength = minDist * 2.f;
            const float totalDist = distBeforeBend + bendLength + distAfterBend;
            const float currDist = totalDist * timeProgress;

            if (currDist <= distBeforeBend) {
                // Segment before the bend: move along start direction
                outPos = startPos + startDir * currDist;
            }
            else if (currDist <= distBeforeBend + bendLength) {
                // Bend segment: interpolate between start and end curves
                const float t = (currDist - distBeforeBend) / bendLength;
                const CVector pos1 = startPos + startDir * (distBeforeBend + t * minDist);
                const CVector pos2 = endPos - endDir * (distAfterBend + (1.f - t) * minDist);
                outPos = Lerp(pos1, pos2, t);
            }
            else {
                // Segment after the bend: move along end direction
                const float remainingDist = currDist - (distBeforeBend + bendLength);
                outPos = endPos - endDir * (distAfterBend - remainingDist);
            }
            startEndDist = totalDist;
        }
        else {
            // Case 2: Straight path with speed variation or sharp turn
            const float totalDistAdjusted = startEndDist / (1.f - speedVar);
            const float currDist = totalDistAdjusted * timeProgress;
            float outT;
            const float correctedDist = CalcCorrectedDist(currDist, totalDistAdjusted, speedVar, outT);

            const CVector posStart = startPos + startDir * correctedDist;
            const CVector posEnd = endPos + endDir * (correctedDist - startEndDist);

            // Handle sharp turn (opposite directions)
            if (dot <= -0.99f && timeProgress >= 0.5f) {
                outPos = endPos;
            }
            else {
                // Linear interpolation based on speed variation
                outPos = Lerp(posStart, posEnd, speedVar);
            }
        }
    }
    else {
        // No distance between points, stay at start position
        outPos = startPos;
    }

    // Ensure exact start and end points at boundaries
    if (timeProgress == 0.f) {
        outPos = startPos;
    }
    else if (timeProgress == 1.f) {
        outPos = endPos;
    }

    // Calculate speed vector
    outSpeed = Lerp(startDir, endDir, timeProgress) * (startEndDist / totalTimeMs * 0.001f);
    outSpeed.z = 0.f; // Speed is planar (2D) as in original implementation
}
