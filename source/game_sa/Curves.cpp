#include "StdInc.h"
#include "Curves.h"

void CCurves::InjectHooks() {
    RH_ScopedClass(CCurves);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(TestCurves, 0x43C600, {.locked=true});
    RH_ScopedGlobalInstall(DistForLineToCrossOtherLine, 0x43C610);
    RH_ScopedGlobalInstall(CalcSpeedVariationInBend, 0x43C660);
    RH_ScopedGlobalInstall(CalcSpeedScaleFactor, 0x43C710);
    RH_ScopedGlobalInstall(CalcCorrectedDist, 0x43C880);
    RH_ScopedGlobalInstall(CalcCurvePoint, 0x43C900);
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
    } else if (dot <= 0.7f) { // Directions are more than ~45 degrees apart
        return (1.f - dot / 0.7f) / 3.f;
    } else { // Directions are less than ~45 degrees apart
        return CCollision::DistToMathematicalLine2D(ptB.x, ptB.y, dirB.x, dirB.y, ptA.x, ptA.y) / (ptA - ptB).Magnitude() / 3.f;
    }
}

// 0x43C710
float CCurves::CalcSpeedScaleFactor(const CVector2D& ptA, const CVector2D& ptB, CVector2D dirA, CVector2D dirB) {
    // Find where the ray from start position would intersect with end ray
    const float distToCrossA = DistForLineToCrossOtherLine(ptA, dirA, ptB, dirB);

    // Find where the ray from end position would intersect with start ray (negative because direction is flipped)
    const float distToCrossB = -DistForLineToCrossOtherLine(ptB, dirB, ptA, dirA);

    // If rays don't intersect properly, fall back to a simpler curved path approximation
    // This happens when the directions would never cross or are almost parallel
    if (distToCrossA <= 0.f || distToCrossB <= 0.f) {
        return (ptA - ptB).Magnitude() / (1.f - CalcSpeedVariationInBend(ptA, ptB, dirA, dirB));
    }

    // Otherwise, the total distance is the length of the two straight segments and the bend
    const auto bendDist = std::min({ distToCrossA, distToCrossB, 5.f });
    return 2.f * bendDist
        + (distToCrossA - bendDist)
        + (distToCrossB - bendDist);
}

// 0x43C880
float CCurves::CalcCorrectedDist(float curr, float total, float variance, float& outT) {
    if (total >= 0.00001f) {
        const auto prog = curr / total;
        outT            = 0.5f - std::cos(PI * prog) * 0.5f;
        return std::sin(prog * TWO_PI) * (total / TWO_PI) * variance
            + (1.f - (variance * 2.f) + 1.f) * 0.5f * curr;
    } else {
        outT = 0.5f;
        return 0.f;
    }
}

// 0x43C900
void CCurves::CalcCurvePoint(
    const CVector& ptA3D,
    const CVector& ptB3D,
    const CVector& dirA3D,
    const CVector& dirB3D,
    float          t,
    int32          traversalTimeMs,
    
    CVector&       outPos,
    CVector&       outSpeed
) {
    // This function calculates a point on a smooth curve between two positions with direction vectors.
    // The curve consists of straight segments and a bend connecting them for natural-looking movement.

    const CVector2D ptA{ ptA3D };
    const CVector2D dirA{ dirA3D };

    const CVector2D ptB{ ptB3D };
    const CVector2D dirB{ dirB3D };
    
    // Normalize time parameter to ensure calculations remain within valid range
    t = std::clamp(t, 0.0f, 1.0f);

    // Get speed adjustment factor needed for realistic bends (slower in curves, faster on straights)
    float speedVariation = CalcSpeedVariationInBend(ptA3D, ptB3D, dirA, dirB);

    // Find where the ray from start position would intersect with end ray
    float distToCrossA = DistForLineToCrossOtherLine(ptA, dirA, ptB, dirB);

    // Find where the ray from end position would intersect with start ray (negative because direction is flipped)
    float distToCrossB = -DistForLineToCrossOtherLine(ptB, dirB, ptA, dirA);

    const auto CalculateSpeed = [&](float totalDist) {
        outSpeed = CVector{ lerp(dirA, dirB, t) * (totalDist / (traversalTimeMs / 1000.f)) };
    };

    // If rays don't intersect properly, fall back to a simpler curved path approximation
    // This happens when the directions would never cross or are almost parallel
    if (distToCrossA <= 0.0f || distToCrossB <= 0.0f)
    {
        const float straightDist = (ptA3D - ptB3D).Magnitude2D();

        // Calculate path distances adjusted for speed variation
        const float dist = straightDist / (1.0f - speedVariation);
        const float currDist = CalcCorrectedDist(dist * t, dist, speedVariation, speedVariation);

        // Blend between the two projected positions based on speed variation
        outPos = lerp(ptA3D + (dirA3D * currDist), ptB3D + (dirB3D * (currDist - straightDist)), speedVariation);

        // Zero speed for this special case (likely a placeholder as this value isn't used)
        CalculateSpeed(0.f);

        return;
    }

    // For properly intersecting rays, create a three-segment path:
    // 1. Straight segment from start
    // 2. Curved bend in the middle
    // 3. Straight segment to end

    // Limit how sharp the bend can be for natural movement
    const float bendDistOneSegment = std::min({ distToCrossA, distToCrossB, 5.f });

    // Calculate the three segment lengths
    const float straightDistA = distToCrossA - bendDistOneSegment;
    const float straightDistB = distToCrossB - bendDistOneSegment;
    const float bendDist      = bendDistOneSegment * 2.0f;
    const float totalDist     = straightDistA + bendDist + straightDistB;
    const float currDist      = totalDist * t;

    if (currDist < straightDistA) {
        // 1. Position is on the first straight segment (linear interpolation from start)
        outPos = ptA3D + (dirA3D * currDist);
    } else if (currDist > (straightDistA + bendDist)) {
        // 2. Position is on the final straight segment (linear interpolation to end)
        outPos = ptB3D + (dirB3D * (currDist - (straightDistA + bendDist)));
    } else {
        // 3. Position is in the curved bend section - requires double interpolation
        //    First interpolate through the bend progress, then between the influenced points
        const float bendT = (currDist - straightDistA) / bendDist;

        // Blend between influence points that extendoutward
        // from the bend endpoints to create a curved path
        outPos = lerp(
            ptA3D + (dirA3D * straightDistA) + (dirA3D * (bendDistOneSegment * bendT)),
            ptB3D - (dirB3D * straightDistB) - (dirB3D * (bendDistOneSegment * (1.0f - bendT))),
            bendT
        );
    }

    CalculateSpeed(totalDist);
}

// unused
// 0x43C600
void CCurves::TestCurves() {
    // https://github.com/ifarbod/sa-curves-test

#define epsilon 0.01f
#define FLOAT_EQUAL(a, b) (fabs((a) - (b)) < (epsilon))

    //const auto FLOAT_EQUAL = [](float a, float b) {
    //    return fabs(a - b) < 0.0002f;
    //};

#if 0
    auto DistForLineToCrossOtherLine_test = []
        {
            // Test case 1: Lines intersect
            {
                float result = DistForLineToCrossOtherLine(0.0f, 0.0f, 1.0f, 1.0f,  // Line 1: base (0,0), direction (1,1)
                    1.0f, 0.0f, -1.0f, 1.0f                                       // Line 2: base (1,0), direction (-1,1)
                );

                // 0.5
                assert(FLOAT_EQUAL(result, 0.5f));  // Expected distance to crossing
            }

            // Test case 2: Lines are parallel (no intersection)
            {
                float result = DistForLineToCrossOtherLine(0.0f, 0.0f, 1.0f, 1.0f,  // Line 1: base (0,0), direction (1,1)
                    1.0f, 1.0f, 1.0f, 1.0f                                        // Line 2: base (1,1), direction (1,1)
                );

                // -1
                assert(FLOAT_EQUAL(result, -1.0f));  // Expected -1 for parallel lines
            }

            // Test case 3: Lines intersect at a point
            {
                float result = DistForLineToCrossOtherLine(0.0f, 0.0f, 2.0f, 2.0f,  // Line 1: base (0,0), direction (2,2)
                    0.0f, 4.0f, 2.0f, -2.0f                                       // Line 2: base (0,4), direction (2,-2)
                );

                // 1.0
                assert(FLOAT_EQUAL(result, 1.0f));  // Expected distance to crossing
            }

            // Test case 4: Lines are coincident (infinite intersections)
            {
                float result = DistForLineToCrossOtherLine(0.0f, 0.0f, 1.0f, 1.0f,  // Line 1: base (0,0), direction (1,1)
                    1.0f, 1.0f, 2.0f, 2.0f                                        // Line 2: base (1,1), direction (2,2)
                );

                // -1
                assert(FLOAT_EQUAL(result, -1.0f));  // Expected -1 for coincident lines
            }

            // Test case 5: Lines with large numbers
            {
                float result = DistForLineToCrossOtherLine(
                    2500.5f, 1500.0f, 3.5f, 2.5f,  // Line 1: base (2500.5,1500), direction (3.5,2.5)
                    3000.0f, 2000.0f, -4.0f, 3.0f  // Line 2: base (3000,2000), direction (-4,3)
                );

                // Should still give a reasonable intersection distance
                assert(FLOAT_EQUAL(result, 170.658539f));  // Expected distance to crossing
            }
        };
#endif
#if 0
    auto CalcSpeedVariationInBend_test = []
        {
            // Test Case 1: Dot product <= 0 (opposite directions)
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 0.0f, 0.0f);
                float speedVariation =
                    CalcSpeedVariationInBend(startCoors, endCoors, 1.0f, 0.0f, -1.0f, 0.0f);  // Opposite directions
                assert(FLOAT_EQUAL(speedVariation, 1.0f / 3.0f) && "Test Case 1 Failed: Expected 0.33333.");
            }

            // Test Case 2: Dot product <= 0 (perpendicular directions)
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 1.0f, 0.0f);
                float speedVariation =
                    CalcSpeedVariationInBend(startCoors, endCoors, 1.0f, 0.0f, 0.0f, 1.0f);  // Perpendicular directions
                assert(FLOAT_EQUAL(speedVariation, 1.0f / 3.0f) && "Test Case 2 Failed: Expected 0.33333.");
            }

            // Test Case 3: Dot product <= 0.7 (small angle)
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 0.0f, 0.0f);
                float speedVariation = CalcSpeedVariationInBend(startCoors, endCoors, 1.0f, 0.0f, 0.9f, 0.1f);  // Small angle
                assert(FLOAT_EQUAL(speedVariation, 0.145296633f) &&
                    "Test Case 3 Failed: Expected value between 0 and 0.33333.");
            }

            // Test Case 4: Dot product > 0.7 (larger angle)
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 1.0f, 0.0f);
                float speedVariation =
                    CalcSpeedVariationInBend(startCoors, endCoors, 1.0f, 0.0f, 0.8f, 0.2f);  // Larger angle
                assert(FLOAT_EQUAL(speedVariation, 0.235702246f) &&
                    "Test Case 4 Failed: Expected value between 0 and 0.33333.");
            }

            // Test Case 5: Dot product = 0.7 (boundary case)
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 0.0f, 0.0f);
                float speedVariation =
                    CalcSpeedVariationInBend(startCoors, endCoors, 1.0f, 0.0f, 0.7f, 0.7141428f);  // Dot product = 0.7
                assert(FLOAT_EQUAL(speedVariation, 0.0f) && "Test Case 5 Failed: Expected 0.0.");
            }

            // Test Case 6: Dot product > 0.7 (sharp bend)
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 0.0f, 0.0f);
                float speedVariation = CalcSpeedVariationInBend(startCoors, endCoors, 1.0f, 0.0f, 0.6f, 0.8f);  // Sharp bend
                assert(
                    FLOAT_EQUAL(speedVariation, 0.047619f) && "Test Case 6 Failed: Expected value between 0 and 0.33333.");
            }

            // Test Case 7: Dot product = 1 (same direction)
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 0.0f, 0.0f);
                float speedVariation =
                    CalcSpeedVariationInBend(startCoors, endCoors, 1.0f, 0.0f, 1.0f, 0.0f);  // Same direction
                assert(speedVariation == 0.0f && "Test Case 7 Failed: Expected 0.0.");
            }

            // Test Case 8: Dot product > 0.7 (perpendicular distance calculation)
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 1.0f, 0.0f);
                float speedVariation =
                    CalcSpeedVariationInBend(startCoors, endCoors, 1.0f, 0.0f, 0.0f, 1.0f);  // Perpendicular directions
                assert(speedVariation > 0.0f && speedVariation <= (1.0f / 3.0f) &&
                    "Test Case 8 Failed: Expected value between 0 and 0.33333.");
            }

            // Test Case 9: Dot product > 0.7 (large perpendicular distance)
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(10.0f, 0.0f, 0.0f);
                float speedVariation =
                    CalcSpeedVariationInBend(startCoors, endCoors, 1.0f, 0.0f, 0.0f, 1.0f);  // Large perpendicular distance
                assert(speedVariation > 0.0f && speedVariation <= (1.0f / 3.0f) &&
                    "Test Case 9 Failed: Expected value between 0 and 0.33333.");
            }

            // Test Case 10: Dot product > 0.7 (small perpendicular distance)
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 0.1f, 0.0f);
                float speedVariation =
                    CalcSpeedVariationInBend(startCoors, endCoors, 1.0f, 0.0f, 0.0f, 1.0f);  // Small perpendicular distance
                assert(speedVariation > 0.0f && speedVariation <= (1.0f / 3.0f) &&
                    "Test Case 10 Failed: Expected value between 0 and 0.33333.");
            }
        };
#endif
#if 0
    auto CalcSpeedScaleFactor_test = []
        {
            // Test Case 1: CalcSpeedScaleFactor - Simple Bend
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 1.0f, 0.0f);
                float speedScaleFactor = CalcSpeedScaleFactor(startCoors, endCoors, 1.0f, 0.0f, 0.0f, 1.0f);

                // 2.0
                assert(FLOAT_EQUAL(speedScaleFactor, 2.0f) && "Test Case Failed: Incorrect speed scale factor.");
            }

            // Test Case 2: CalcSpeedScaleFactor - Straight Line
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 0.0f, 0.0f);
                float speedScaleFactor = CalcSpeedScaleFactor(startCoors, endCoors, 1.0f, 0.0f, 1.0f, 0.0f);

                // 1.0
                assert(FLOAT_EQUAL(speedScaleFactor, 1.0f) && "Test Case Failed: Incorrect speed scale factor.");
            }

            // Test Case 3: CalcSpeedScaleFactor - Sharp Bend
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 0.0f, 0.0f);
                float speedScaleFactor = CalcSpeedScaleFactor(startCoors, endCoors, 1.0f, 0.0f, 0.0f, 1.0f);

                // 1.5
                assert(FLOAT_EQUAL(speedScaleFactor, 1.5f) &&
                    "Test Case Failed: Speed scale factor should be greater than 1.0.");
            }

            // Test Case 4: CalcSpeedScaleFactor - Parallel Lines
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 0.0f, 0.0f);
                float speedScaleFactor = CalcSpeedScaleFactor(startCoors, endCoors, 1.0f, 0.0f, 1.0f, 0.0f);

                // 1.0
                assert(FLOAT_EQUAL(speedScaleFactor, 1.0f) && "Test Case Failed: Incorrect speed scale factor.");
            }

            // Test Case 5: CalcSpeedScaleFactor - Coincident Lines
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 1.0f, 0.0f);
                float speedScaleFactor = CalcSpeedScaleFactor(startCoors, endCoors, 1.0f, 1.0f, 1.0f, 1.0f);

                // ~1.41
                assert(FLOAT_EQUAL(speedScaleFactor, 1.4142135f) && "Test Case Failed: Incorrect speed scale factor.");
            }

            // Test Case 6: CalcSpeedScaleFactor - Large Values
            {
                CVector startCoors(2500.0f, 1500.0f, 0.0f);
                CVector endCoors(3500.0f, 2000.0f, 0.0f);
                float speedScaleFactor = CalcSpeedScaleFactor(startCoors, endCoors, 2.0f, 1.0f, 3.0f, 2.0f);

                // Should be a reasonable scale factor despite large coordinates
                assert(FLOAT_EQUAL(speedScaleFactor, 1118.03394f) &&
                    "Test Case Failed: Speed scale factor out of reasonable range for large values.");
            }
        };
#endif

    auto CalcCurvePoint_test = []
        {
            // Test Case 1: CalcCurvePoint - Straight Line
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 0.0f, 0.0f);
                CVector startDir(1.0f, 0.0f, 0.0f);
                CVector endDir(1.0f, 0.0f, 0.0f);
                CVector resultCoor;
                CVector resultSpeed;
                CalcCurvePoint(startCoors, endCoors, startDir, endDir, 0.5f, 1000, resultCoor, resultSpeed);

                assert(FLOAT_EQUAL(resultCoor.x, 0.5f) && FLOAT_EQUAL(resultCoor.y, 0.0f) &&
                    FLOAT_EQUAL(resultCoor.z, 0.0f) && "Test Case 1 Failed: Incorrect curve point.");
            }

            // Test Case 2: CalcCurvePoint - Curve with Bend
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 1.0f, 0.0f);
                CVector startDir(1.0f, 0.0f, 0.0f);
                CVector endDir(0.0f, 1.0f, 0.0f);
                CVector resultCoor;
                CVector resultSpeed;
                CalcCurvePoint(startCoors, endCoors, startDir, endDir, 0.5f, 1000, resultCoor, resultSpeed);

                // Expected result depends on the interpolation logic
                assert(resultCoor.x > 0.0f && resultCoor.x < 1.0f && resultCoor.y > 0.0f && resultCoor.y < 1.0f &&
                    FLOAT_EQUAL(resultCoor.z, 0.0f) && "Test Case 2 Failed: Incorrect curve point.");
            }

            // Test Case 3: CalcCurvePoint - Large Values
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1000.0f, 1000.0f, 0.0f);
                CVector startDir(1.0f, 0.0f, 0.0f);
                CVector endDir(0.0f, 1.0f, 0.0f);
                CVector resultCoor;
                CVector resultSpeed;
                CalcCurvePoint(startCoors, endCoors, startDir, endDir, 0.5f, 1000, resultCoor, resultSpeed);

                // Expected result depends on the interpolation logic
                assert(resultCoor.x > 0.0f && resultCoor.x < 1000.0f && resultCoor.y > 0.0f && resultCoor.y < 1000.0f &&
                    FLOAT_EQUAL(resultCoor.z, 0.0f) && "Test Case 3 Failed: Incorrect curve point.");
            }

            // Test Case 4: CalcCurvePoint - Time = 0.0 (Start Point)
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 1.0f, 0.0f);
                CVector startDir(1.0f, 0.0f, 0.0f);
                CVector endDir(0.0f, 1.0f, 0.0f);
                CVector resultCoor;
                CVector resultSpeed;
                CalcCurvePoint(startCoors, endCoors, startDir, endDir, 0.0f, 1000, resultCoor, resultSpeed);

                assert(FLOAT_EQUAL(resultCoor.x, 0.0f) && FLOAT_EQUAL(resultCoor.y, 0.0f) &&
                    FLOAT_EQUAL(resultCoor.z, 0.0f) && "Test Case 4 Failed: Incorrect curve point.");
            }

            // Test Case 5: CalcCurvePoint - Time = 1.0 (End Point)
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 1.0f, 0.0f);
                CVector startDir(1.0f, 0.0f, 0.0f);
                CVector endDir(0.0f, 1.0f, 0.0f);
                CVector resultCoor;
                CVector resultSpeed;
                CalcCurvePoint(startCoors, endCoors, startDir, endDir, 1.0f, 1000, resultCoor, resultSpeed);

                assert(FLOAT_EQUAL(resultCoor.x, 1.0f) && FLOAT_EQUAL(resultCoor.y, 1.0f) &&
                    FLOAT_EQUAL(resultCoor.z, 0.0f) && "Test Case 5 Failed: Incorrect curve point.");
            }

            // Test Case 6: CalcCurvePoint - Z-Axis Movement
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(0.0f, 0.0f, 1.0f);
                CVector startDir(0.0f, 0.0f, 1.0f);
                CVector endDir(0.0f, 0.0f, 1.0f);
                CVector resultCoor;
                CVector resultSpeed;
                CalcCurvePoint(startCoors, endCoors, startDir, endDir, 0.5f, 1000, resultCoor, resultSpeed);

                assert(FLOAT_EQUAL(resultCoor.x, 0.0f) && FLOAT_EQUAL(resultCoor.y, 0.0f) &&
                    FLOAT_EQUAL(resultCoor.z, 0.5f) && "Test Case 6 Failed: Incorrect curve point.");
            }

            // Test Case 7: CalcCurvePoint - Sharp Bend
            {
                CVector startCoors(0.0f, 0.0f, 0.0f);
                CVector endCoors(1.0f, 0.0f, 0.0f);
                CVector startDir(1.0f, 0.0f, 0.0f);
                CVector endDir(-1.0f, 0.0f, 0.0f);
                CVector resultCoor;
                CVector resultSpeed;
                CalcCurvePoint(startCoors, endCoors, startDir, endDir, 0.5f, 1000, resultCoor, resultSpeed);

                // Expected result depends on the interpolation logic
                assert(FLOAT_EQUAL(resultCoor.x, 1.0f) && FLOAT_EQUAL(resultCoor.y, 0.0f) &&
                    FLOAT_EQUAL(resultCoor.z, 0.0f) && "Test Case 7 Failed: Incorrect curve point.");
            }
        };

    auto CalcCorrectedDist_test = []
        {
            // Test Case 1: CalcCorrectedDist - Simple Case
            {
                // interpol please don't arrest us, we have done nothing!
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(0.5f, 1.0f, 0.5f, interpol);

                assert(FLOAT_EQUAL(correctedDist, 0.25f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 1 Failed: Corrected distance out of range.");
            }
            // Test Case 2: CalcCorrectedDist - Start of Curve (Time = 0.0)
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(0.0f, 1.0f, 0.5f, interpol);

                assert(FLOAT_EQUAL(correctedDist, 0.0f) && FLOAT_EQUAL(interpol, 0.0f) &&
                    "Test Case 2 Failed: Corrected distance should be 0.0 at the start.");
            }

            // Test Case 3: CalcCorrectedDist - End of Curve (Time = 1.0)
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(1.0f, 1.0f, 0.5f, interpol);

                assert(FLOAT_EQUAL(correctedDist, 0.5f) && FLOAT_EQUAL(interpol, 1.0f) &&
                    "Test Case 3 Failed: Corrected distance should be 1.0 at the end.");
            }

            // Test Case 4: CalcCorrectedDist - No Speed Variation (SpeedVariation = 0.0)
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(0.5f, 1.0f, 0.0f, interpol);

                assert(FLOAT_EQUAL(correctedDist, 0.5f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 4 Failed: Corrected distance should match input when SpeedVariation is 0.0.");
            }

            // Test Case 5: CalcCorrectedDist - Maximum Speed Variation (SpeedVariation = 1.0)
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(0.5f, 1.0f, 1.0f, interpol);

                assert(FLOAT_EQUAL(correctedDist, 0.0f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 5 Failed: Corrected distance should be 0.0 when SpeedVariation is 1.0.");
            }

            // Test Case 6: CalcCorrectedDist - Large Total Distance
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(500.0f, 1000.0f, 0.5f, interpol);

                assert(FLOAT_EQUAL(correctedDist, 250.0f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 6 Failed: Corrected distance should scale with large Total distance.");
            }

            // Test Case 7: CalcCorrectedDist - Small Total Distance
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(0.1f, 0.2f, 0.5f, interpol);

                assert(FLOAT_EQUAL(correctedDist, 0.05f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 7 Failed: Corrected distance should scale with small Total distance.");
            }

            // Test Case 8: CalcCorrectedDist - Negative Current Distance
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(-0.5f, 1.0f, 0.5f, interpol);

                assert(FLOAT_EQUAL(correctedDist, -0.25f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 8 Failed: Corrected distance should handle negative Current distance.");
            }

            // Test Case 9: CalcCorrectedDist - Negative Total Distance
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(0.5f, -1.0f, 0.5f, interpol);

                assert(FLOAT_EQUAL(correctedDist, 0.0f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 9 Failed: Corrected distance should handle negative Total distance.");
            }

            // Test Case 10: CalcCorrectedDist - SpeedVariation > 1.0
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(0.5f, 1.0f, 1.5f, interpol);

                assert(FLOAT_EQUAL(correctedDist, -0.25f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 10 Failed: Corrected distance should handle SpeedVariation > 1.0.");
            }
        };

    //DistForLineToCrossOtherLine_test();
    //CalcSpeedVariationInBend_test();
    //CalcSpeedScaleFactor_test();
    CalcCurvePoint_test();
    CalcCorrectedDist_test();
}
