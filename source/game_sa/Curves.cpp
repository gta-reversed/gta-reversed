#include "Curves.h"
#include "StdInc.h"

void CCurves::InjectHooks() {
    RH_ScopedClass(CCurves);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(TestCurves, 0x43C600, {.locked = true});
    RH_ScopedGlobalInstall(DistForLineToCrossOtherLine, 0x43C610);
    RH_ScopedGlobalInstall(CalcSpeedVariationInBend, 0x43C660);
    RH_ScopedGlobalInstall(CalcSpeedScaleFactor, 0x43C710);
    RH_ScopedGlobalInstall(CalcCorrectedDist, 0x43C880);
    RH_ScopedGlobalInstall(CalcCurvePoint, 0x43C900);
}

// 0x43C610
float CCurves::DistForLineToCrossOtherLine(float LineBaseX, float LineBaseY, float LineDirX, float LineDirY, float OtherLineBaseX, float OtherLineBaseY, float OtherLineDirX, float OtherLineDirY) {
    float determinant = -((LineDirY * OtherLineDirX) - (LineDirX * OtherLineDirY));
    return (determinant != 0.0f) ? -(((LineBaseX - OtherLineBaseX) * OtherLineDirY) - ((LineBaseY - OtherLineBaseY) * OtherLineDirX)) / determinant : -1.0f;
}

// 0x43C660
float CCurves::CalcSpeedVariationInBend(const CVector &startCoors, const CVector &endCoors, float StartDirX, float StartDirY, float EndDirX, float EndDirY) {
    float dotProduct = (StartDirY * EndDirY) + (StartDirX * EndDirX);

    if (dotProduct <= 0.0f) {
        return 0.33333f;
    }

    if (dotProduct <= 0.7f) {
        return ((dotProduct / -0.7f) * 0.33333f) + 0.33333f;
    }

    float distToLine = CCollision::DistToMathematicalLine2D(endCoors.x, endCoors.y, EndDirX, EndDirY, startCoors.x, startCoors.y);
    float totalDist = CVector::Dist(startCoors, endCoors);
    return (distToLine / totalDist) * 0.33333f;
}

// 0x43C880
float CCurves::CalcCorrectedDist(float Current, float Total, float SpeedVariation, float &pInterPol) {
    if (Total >= 0.00001f) {
        float progress = Current / Total;
        pInterPol = 0.5f * (1.0f - std::cos(PI * progress));
        return (Total / TWO_PI) * SpeedVariation * std::sin(progress * TWO_PI) + (1.0f - SpeedVariation) * Current;
    } else {
        pInterPol = 0.5f;
        return 0.0f;
    }
}

// 0x43C710
float CCurves::CalcSpeedScaleFactor(const CVector &startCoors, const CVector &endCoors, float StartDirX, float StartDirY, float EndDirX, float EndDirY) {
    float distOne = DistForLineToCrossOtherLine(startCoors.x, startCoors.y, StartDirX, StartDirY, endCoors.x, endCoors.y, EndDirX, EndDirY);
    float disTwo = DistForLineToCrossOtherLine(endCoors.x, endCoors.y, EndDirX, EndDirY, startCoors.x, startCoors.y, StartDirX, StartDirY);
    if (distOne <= 0.0f || disTwo >= 0.0f) {
        float totalDist = CVector::Dist(startCoors, endCoors);
        float scale = 1.0f - CalcSpeedVariationInBend(startCoors, endCoors, StartDirX, StartDirY, EndDirX, EndDirY);
        if (scale < 0.00001f) {
            scale = 0.00001f;
        }
        return totalDist / scale;
    }
    return distOne - disTwo;
}

// 0x43C900
void CCurves::CalcCurvePoint(const CVector &startCoors, const CVector &endCoors, const CVector &startDir, const CVector &endDir, float time, int32 traversalTimeInMillis, CVector &resultCoor,
                             CVector &resultSpeed) {
    float clampedTime = std::clamp(time, 0.0f, 1.0f);
    float crossProduct = -(startDir.y * endDir.x - startDir.x * endDir.y);
    float scaleFactor = 0.0f;
    float intersectionT = DistForLineToCrossOtherLine(startCoors.x, startCoors.y, startDir.x, startDir.y, endCoors.x, endCoors.y, endDir.x, endDir.y);
    float intersectionS = -DistForLineToCrossOtherLine(endCoors.x, endCoors.y, endDir.x, endDir.y, startCoors.x, startCoors.y, startDir.x, startDir.y);

    if (crossProduct != 0.0f && intersectionT > 0.0f && intersectionS > 0.0f) {
        float blend = std::min({intersectionT, intersectionS, 5.0f});
        float startSegment = intersectionT - blend;
        float curveSegment = 2.0f * blend;
        float endSegment = intersectionS - blend;
        float totalParam = startSegment + curveSegment + endSegment;
        float param = clampedTime * totalParam;
        if (param < startSegment) {
            resultCoor = startCoors + startDir * param;
        } else if (param < startSegment + curveSegment) {
            float t = (param - startSegment) / curveSegment;
            float u = 1.0f - t;
            CVector startBase = startCoors + startDir * startSegment;
            CVector endBase = endCoors - endDir * endSegment;
            CVector startPoint = startBase + startDir * (blend * t);
            CVector endPoint = endBase - endDir * (blend * u);
            resultCoor = Lerp(startPoint, endPoint, t);
        } else {
            float offset = param - totalParam;
            resultCoor = endCoors + endDir * offset;
        }
        scaleFactor = totalParam;
    } else {
        float speedVariation = CalcSpeedVariationInBend(startCoors, endCoors, startDir.x, startDir.y, endDir.x, endDir.y);
        float distance = CVector::Dist(startCoors, endCoors);
        float speedScale = distance / (1.0f - speedVariation);
        float interpFactor;
        float correctedDist = CalcCorrectedDist(clampedTime * speedScale, speedScale, speedVariation, interpFactor);
        CVector startAdjusted = startCoors + startDir * correctedDist;
        CVector endAdjusted = endCoors + endDir * (correctedDist - distance);
        resultCoor = Lerp(startAdjusted, endAdjusted, interpFactor);
        scaleFactor = distance;
    }

    float timeInSeconds = std::max(traversalTimeInMillis * 0.001f, 0.00001f);
    CVector interpolatedDir = Lerp(startDir, endDir, clampedTime);
    resultSpeed = interpolatedDir * (scaleFactor / timeInSeconds);
    resultSpeed.z = 0.0f;
}

// unused
// 0x43C600
void CCurves::TestCurves() {
    // Test the CalcCurvePoint comparison - this is the main test we need
    std::cout << "Testing CalcCurvePoint against CCurves::CalcCurvePoint..." << std::endl;

    // Define test cases with various input parameters
    struct TestCase
    {
        CVector start, end, startDir, endDir;
        float time;
        int32 traversalTime;
        const char *name;
        CVector expectedPosition;
        CVector expectedSpeed;
    };

    auto printVector = [](const CVector& v) {
        std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
    };

    auto vectorsAlmostEqual = [](const CVector& v1, const CVector& v2, float tolerance) -> bool {
        return std::abs(v1.x - v2.x) < tolerance && 
               std::abs(v1.y - v2.y) < tolerance && 
               std::abs(v1.z - v2.z) < tolerance;
    };

    TestCase testCases[] = {
        // Straight line
        {
            CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 0.0f, 0.0f),
            CVector(1.0f, 0.0f, 0.0f), CVector(1.0f, 0.0f, 0.0f),
            0.5f, 1000, "Straight line",
            CVector(5.0f, 0.0f, 0.0f), CVector(10.0f, 0.0f, 0.0f)
        },
        // 90-degree curve
        {
            CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 10.0f, 0.0f),
            CVector(1.0f, 0.0f, 0.0f), CVector(0.0f, 1.0f, 0.0f),
            0.5f, 1000, "90-degree curve",
            CVector(8.75f, 1.25f, 0.0f), CVector(10.0f, 10.0f, 0.0f)
        },
        // S-curve
        {
            CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 0.0f, 0.0f),
            CVector(1.0f, 1.0f, 0.0f), CVector(1.0f, -1.0f, 0.0f),
            0.5f, 2000, "S-curve",
            CVector(5.0f, 2.5f, 0.0f), CVector(5.0f, 0.0f, 0.0f)
        },
        // Opposite directions
        {
            CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 0.0f, 0.0f),
            CVector(1.0f, 0.0f, 0.0f), CVector(-1.0f, 0.0f, 0.0f),
            0.5f, 1500, "Opposite directions",
            CVector(10.0f, 0.0f, 0.0f), CVector(0.0f, 0.0f, 0.0f)
        },
        // With Z component
        {
            CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 10.0f, 5.0f),
            CVector(1.0f, 0.0f, 0.5f), CVector(0.0f, 1.0f, 0.5f),
            0.5f, 1000, "With Z component",
            CVector(8.75f, 1.25f, 2.5f), CVector(10.0f, 10.0f, 0.0f)
        },
        // Time at beginning
        {
            CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 10.0f, 0.0f),
            CVector(1.0f, 0.0f, 0.0f), CVector(0.0f, 1.0f, 0.0f),
            0.0f, 1000, "Time at beginning",
            CVector(0.0f, 0.0f, 0.0f), CVector(20.0f, 0.0f, 0.0f)
        },
        // Time at end
        {
            CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 10.0f, 0.0f),
            CVector(1.0f, 0.0f, 0.0f), CVector(0.0f, 1.0f, 0.0f),
            1.0f, 1000, "Time at end",
            CVector(10.0f, 10.0f, 0.0f), CVector(0.0f, 20.0f, 0.0f)
        },
        // Large values
        {
            CVector(1000.0f, 2000.0f, 100.0f), CVector(2000.0f, 1000.0f, 200.0f),
            CVector(1.0f, -0.5f, 0.1f), CVector(-0.5f, -1.0f, 0.1f),
            0.5f, 5000, "Large values",
            CVector(1800.0f, 1600.0f, 180.0f), CVector(80.0f, -240.0f, 0.0f)
        }
    };

    constexpr float compareTolerance = 0.01f;
    int passed = 0;
    int total = sizeof(testCases) / sizeof(testCases[0]);

    for (const auto &test : testCases)
    {
        CVector resultCoor2, resultSpeed2;

        // Call both implementations with the same inputs
        CVector resultCoor1 = test.expectedPosition;
        CVector resultSpeed1 = test.expectedSpeed;

        CCurves::CalcCurvePoint(test.start, test.end, test.startDir, test.endDir,
                                test.time, test.traversalTime, resultCoor2, resultSpeed2);

        // Compare results
        bool coordsMatch = vectorsAlmostEqual(resultCoor1, resultCoor2, compareTolerance);
        bool speedsMatch = vectorsAlmostEqual(resultSpeed1, resultSpeed2, compareTolerance);

        std::cout << "Test: " << test.name << " - ";
        if (coordsMatch && speedsMatch)
        {
            std::cout << "PASSED" << std::endl;
            passed++;
        }
        else
        {
            std::cout << "FAILED" << std::endl;
        }
        std::cout << "  Expected position: ";
        printVector(resultCoor1);
        std::cout << "  Actual position:   ";
        printVector(resultCoor2);

        std::cout << "  Expected speed:    ";
        printVector(resultSpeed1);
        std::cout << "  Actual speed:      ";
        printVector(resultSpeed2);
    }

    std::cout << "CalcCurvePoint comparison test: " << passed << "/" << total << " tests passed." << std::endl;

    assert(passed == total);
}
