#include "Curves.h"
#include "common.h"

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

// 0x43C610 - inlined
// Modified to use CVector2D instead of individual floats
float CCurves::DistForLineToCrossOtherLine(CVector2D lineBase, CVector2D lineDir, CVector2D otherLineBase, CVector2D otherLineDir) {
    const auto crossAB = lineDir.Cross(otherLineDir);
    if (crossAB == 0.f) {
        return -1.f;
    }
    return (lineBase - otherLineBase).Cross(otherLineDir) * (-1.f / crossAB);
}

// 0x43C660 - inlined
// Modified to use CVector2D for direction parameters
float CCurves::CalcSpeedVariationInBend(const CVector &startCoors, const CVector &endCoors, const CVector2D StartDir, const CVector2D EndDir) {
    // Calculate dot product between direction vectors
    float dotProduct = StartDir.Dot(EndDir);

    if (dotProduct <= 0.0f) {
        return ONE_THIRD;
    } else if (dotProduct <= 0.7f) {
        return (dotProduct / -0.7f) * ONE_THIRD + ONE_THIRD;
    }

    // Calculate distance from a line to a point, scaled by the distance between endpoints
    return (CCollision::DistToMathematicalLine2D(endCoors.x, endCoors.y, EndDir.x, EndDir.y, startCoors.x, startCoors.y) / (startCoors - endCoors).Magnitude2D()) * ONE_THIRD;
}

// 0x43C880 - inlined
float CCurves::CalcCorrectedDist(float current, float total, float speedVariation, float &pInterPol) {
    if (total >= EPSILON) {
        // Calculate corrected distance using sine wave variation
        float correctedDist = (total / TWO_PI * speedVariation * std::sin(current * TWO_PI / total)) + (((1.0f - 2.0f * speedVariation) * 0.5f + 0.5f) * current);
        // Calculate interpolation parameter using cosine
        pInterPol = 0.5f - (std::cos(current / total * PI) * 0.5f);
        return correctedDist;
    } else {
        // Handle case when total distance is too small
        pInterPol = 0.5f;
        return 0.0f;
    }
}

// 0x43C710
float CCurves::CalcSpeedScaleFactor(const CVector &startCoors, const CVector &endCoors, const CVector2D StartDir, const CVector2D EndDir) {
    // Calculate intersection distances for both lines
    float distOne = DistForLineToCrossOtherLine(CVector2D(startCoors.x, startCoors.y), StartDir, CVector2D(endCoors.x, endCoors.y), EndDir);
    float distTwo = DistForLineToCrossOtherLine(CVector2D(endCoors.x, endCoors.y), EndDir, CVector2D(startCoors.x, startCoors.y), StartDir);

    // If lines don't intersect properly
    if (distOne <= 0.0f || distTwo >= 0.0f) {
        // Calculate the straight-line distance
        float dx = startCoors.x - endCoors.x;
        float dy = startCoors.y - endCoors.y;
        float totalDist = std::sqrt(dx * dx + dy * dy);

        // Adjust distance using speed variation
        float scale = 1.0f - CalcSpeedVariationInBend(startCoors, endCoors, StartDir, EndDir);

        // Avoid division by zero
        if (scale == 0.0f) {
            scale = 0.00001f;
        }

        return totalDist / scale;
    }

    // Return the path distance
    return distOne - distTwo;
}

// 0x43C900
void CCurves::CalcCurvePoint(const CVector &startCoors, const CVector &endCoors, const CVector &startDir, const CVector &endDir, float time, int32 traversalTimeInMillis, CVector &resultCoor,
                             CVector &resultSpeed) {
    // Clamp time between 0 and 1
    float scaleFactor;
    float clampedTime = std::clamp(time, 0.0f, 1.0f);


    // Calculate intersection parameters
    float intersectionT = DistForLineToCrossOtherLine(CVector2D(startCoors), CVector2D(startDir), CVector2D(endCoors), CVector2D(endDir));
    float intersectionS = -DistForLineToCrossOtherLine(CVector2D(endCoors), CVector2D(endDir), CVector2D(startCoors), CVector2D(startDir));

    if (intersectionT > 0.0f && intersectionS > 0.0f) {
        // Use three-segment path
        float blend = std::min({intersectionT, intersectionS, 5.0f});
        float startSegment = intersectionT - blend;
        float curveSegment = 2.0f * blend;
        float endSegment = intersectionS - blend;
        scaleFactor = startSegment + curveSegment + endSegment;
        float param = clampedTime * totalParam;

        if (param < startSegment) {
            // First segment: move along start direction
            resultCoor = startCoors + startDir * param;
        } else if (param < startSegment + curveSegment) {
            // Middle segment: interpolate between adjusted points
            float t = (param - startSegment) / curveSegment;
            float u = 1.0f - t;

            // Create base points
            CVector startBase = startCoors + startDir * startSegment;
            CVector endBase = endCoors - endDir * endSegment;

            // Create control points
            CVector startPoint = startBase + startDir * (blend * t);
            CVector endPoint = endBase - endDir * (blend * u);

            // Interpolate between control points using lerp function
            resultCoor = lerp(startPoint, endPoint, t);
        } else {
            // Final segment: move along end direction
            float offset = param - scaleFactor;
            resultCoor = endCoors + endDir * offset;
        }
    } else {
        // Lines are parallel, use speed variation method
        float speedVariation = CalcSpeedVariationInBend(startCoors, endCoors, CVector2D(startDir), CVector2D(endDir));
        scaleFactor = (startCoors - endCoors).Magnitude2D();
        float speedScale = scaleFactor / (1.0f - speedVariation);

        float interpFactor;
        float correctedDist = CalcCorrectedDist(clampedTime * speedScale, speedScale, speedVariation, interpFactor);

        // Create adjusted start and end positions
        CVector startAdjusted = startCoors + startDir * correctedDist;
        CVector endAdjusted = endCoors + endDir * (correctedDist - distance);

        // Interpolate between adjusted positions using lerp function
        resultCoor = lerp(startAdjusted, endAdjusted, interpFactor);
        scaleFactor = scaleFactor;
    }

    // Calculate speed using lerp for direction interpolation
    float timeInSeconds = std::max(traversalTimeInMillis * 0.001f, 0.00001f);

    // Interpolate between start and end directions
    CVector interpolatedDir = lerp(startDir, endDir, clampedTime);

    // Scale the interpolated direction by scaleFactor/time
    resultSpeed = interpolatedDir * (scaleFactor / timeInSeconds);
    resultSpeed.z = 0.0f; // Zero out vertical speed component
}

// unused
// 0x43C600
void CCurves::TestCurves() {

    // Test the CalcCurvePoint comparison - this is the main test we need
    std::cout << "Testing CalcCurvePoint against CCurves::CalcCurvePoint..." << std::endl;

    // Define test cases with various input parameters
    struct TestCase {
        CVector start, end, startDir, endDir;
        float time;
        int32 traversalTime;
        const char *name;
        CVector expectedPosition;
        CVector expectedSpeed;
    };

    auto printVector = [](const CVector &v) { std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl; };

    auto vectorsAlmostEqual = [](const CVector &v1, const CVector &v2, float tolerance) -> bool {
        return std::abs(v1.x - v2.x) < tolerance && std::abs(v1.y - v2.y) < tolerance && std::abs(v1.z - v2.z) < tolerance;
    };

    TestCase testCases[] = {
        // Straight line
        {CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 0.0f, 0.0f), CVector(1.0f, 0.0f, 0.0f), CVector(1.0f, 0.0f, 0.0f), 0.5f, 1000, "Straight line", CVector(5.0f, 0.0f, 0.0f),
         CVector(10.0f, 0.0f, 0.0f)},
        // 90-degree curve
        {CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 10.0f, 0.0f), CVector(1.0f, 0.0f, 0.0f), CVector(0.0f, 1.0f, 0.0f), 0.5f, 1000, "90-degree curve", CVector(8.75f, 1.25f, 0.0f),
         CVector(10.0f, 10.0f, 0.0f)},
        // S-curve
        {CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 0.0f, 0.0f), CVector(1.0f, 1.0f, 0.0f), CVector(1.0f, -1.0f, 0.0f), 0.5f, 2000, "S-curve", CVector(5.0f, 2.5f, 0.0f), CVector(5.0f, 0.0f, 0.0f)},
        // Opposite directions
        {CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 0.0f, 0.0f), CVector(1.0f, 0.0f, 0.0f), CVector(-1.0f, 0.0f, 0.0f), 0.5f, 1500, "Opposite directions", CVector(10.0f, 0.0f, 0.0f),
         CVector(0.0f, 0.0f, 0.0f)},
        // With Z component
        {CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 10.0f, 5.0f), CVector(1.0f, 0.0f, 0.5f), CVector(0.0f, 1.0f, 0.5f), 0.5f, 1000, "With Z component", CVector(8.75f, 1.25f, 2.5f),
         CVector(10.0f, 10.0f, 0.0f)},
        // Time at beginning
        {CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 10.0f, 0.0f), CVector(1.0f, 0.0f, 0.0f), CVector(0.0f, 1.0f, 0.0f), 0.0f, 1000, "Time at beginning", CVector(0.0f, 0.0f, 0.0f),
         CVector(20.0f, 0.0f, 0.0f)},
        // Time at end
        {CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 10.0f, 0.0f), CVector(1.0f, 0.0f, 0.0f), CVector(0.0f, 1.0f, 0.0f), 1.0f, 1000, "Time at end", CVector(10.0f, 10.0f, 0.0f),
         CVector(0.0f, 20.0f, 0.0f)},
        // Large values
        {CVector(1000.0f, 2000.0f, 100.0f), CVector(2000.0f, 1000.0f, 200.0f), CVector(1.0f, -0.5f, 0.1f), CVector(-0.5f, -1.0f, 0.1f), 0.5f, 5000, "Large values", CVector(1800.0f, 1600.0f, 180.0f),
         CVector(80.0f, -240.0f, 0.0f)}};

    constexpr float compareTolerance = 0.01f;
    int passed = 0;
    int total = sizeof(testCases) / sizeof(testCases[0]);

    for (const auto &test : testCases) {
        CVector resultCoor2, resultSpeed2;

        // Call both implementations with the same inputs
        CVector resultCoor1 = test.expectedPosition;
        CVector resultSpeed1 = test.expectedSpeed;

        CCurves::CalcCurvePoint(test.start, test.end, test.startDir, test.endDir, test.time, test.traversalTime, resultCoor2, resultSpeed2);

        // Compare results
        bool coordsMatch = vectorsAlmostEqual(resultCoor1, resultCoor2, compareTolerance);
        bool speedsMatch = vectorsAlmostEqual(resultSpeed1, resultSpeed2, compareTolerance);

        std::cout << "Test: " << test.name << " - ";
        if (coordsMatch && speedsMatch) {
            std::cout << "PASSED" << std::endl;
            passed++;
        } else {
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
