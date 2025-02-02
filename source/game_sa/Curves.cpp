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
    return;
}

// 0x43C610
float CCurves::DistForLineToCrossOtherLine(CVector2D lineStart, CVector2D lineDir, CVector2D otherLineStart, CVector2D otherLineDir) {
    const auto t = lineDir.Cross(otherLineDir);
    return (t == 0.f) ? -1.f : (lineStart - otherLineStart).Cross(otherLineDir) / -t;
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
             + curr * (1.f - 2.f * variance + 1.f) * 0.5f;
    } else {
        outT = 0.5f;
        return 0.f;
    }
}

// 0x43C900
void CCurves::CalcCurvePoint(
    const CVector& startPos,
    const CVector& endPos,
    const CVector& startDir,
    const CVector& endDir,
    float timeProgress,
    int32 totalTimeMs,
    CVector& outPos,
    CVector& outSpeed
) {
    timeProgress = std::clamp(timeProgress, 0.f, 1.f);

    /*
    const auto [pos, speedScale] = [&]() -> std::tuple<CVector, float> {
        const auto startCrossEndDist = DistForLineToCrossOtherLine(startPos, startDir, endPos, endDir);
        const auto endCrossStartDist = DistForLineToCrossOtherLine(endPos, endDir, startPos, startDir);
        if (startCrossEndDist > 0.f && endCrossStartDist > 0.f) {
            const auto minDist = std::min({ startCrossEndDist, endCrossStartDist, 5.f }); // Min. of the 2 distances, but not higher than 5
            const auto speedScale = 2.f * minDist + (startCrossEndDist - minDist) + (endCrossStartDist - minDist);
            const auto speedScaleWithTime = speedScale * timeProgress;


            if (startCrossEndDist - minDist >= speedScaleWithTime) {
                return { startPos + startDir * speedScaleWithTime, speedScale };
            }

            if (speedScaleWithTime >= startCrossEndDist + minDist) {
                return { endPos + endDir * speedScaleWithTime, speedScale };
            }

            // NOTE: Seems like some cubic bezier curve thingy? Not sure.
            const auto t = speedScaleWithTime * (startCrossEndDist - minDist) / (2.f * minDist);
            const auto d = endCrossStartDist - minDist;
            return {
                lerp(
                    (startPos + (d * startDir)) + (minDist * (t * startDir)),
                    (endPos + (d * endDir)) - (minDist * (t * endDir)),
                    t
                ),
                speedScale
            };
        } else {
            const auto startEndPosDist2D = (startPos - endPos).Magnitude2D();
            const auto speedVarianceInBend = CalcSpeedVariationInBend(startPos, endPos, startDir, endDir);
            const auto CalcSpeed = [&](float t) {
                return (startEndPosDist2D / (1.f - speedVarianceInBend)) * t;
            };
            float correctedDistT{};
            const auto correctedDist = CalcCorrectedDist(
                CalcSpeed(timeProgress),
                CalcSpeed(1.f),
                speedVarianceInBend,
                correctedDistT
            );
            return {
                lerp(
                    startPos + startDir * correctedDist,
                    endPos + endDir * (correctedDist - startEndPosDist2D),
                    timeProgress
                ),
                startEndPosDist2D
            };
        }
    }();

    outPos = pos;
    outSpeed = lerp(endDir, startDir, timeProgress) * speedScale / ((float)totalTimeMs / 1000.f);
    */

    // NOTE: This code (partially?) repeats the code written above.
    // If the current code is not working properly, the code above will crash the game after a while, so you will have to fix it.
    const auto curveFactor = CalcSpeedVariationInBend(startPos, endPos, startDir, endDir);
    const auto curvePoint = lerp(startPos, endPos, timeProgress);

    outPos = curvePoint + (curveFactor * (endDir - startDir));
    outSpeed = lerp(startDir, endDir, timeProgress) / ((float)totalTimeMs / 1000.f);
}
