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
float CCurves::DistForLineToCrossOtherLine(float LineBaseX, float LineBaseY, float LineDirX, float LineDirY, float OtherLineBaseX, float OtherLineBaseY, float OtherLineDirX, float OtherLineDirY) {
    float Dir = LineDirX * OtherLineDirY - LineDirY * OtherLineDirX;

    if (Dir == 0.0f) {
        return -1.0f; // Lines are parallel, no intersection
    }

    float Dist           = (LineBaseX - OtherLineBaseX) * OtherLineDirY - (LineBaseY - OtherLineBaseY) * OtherLineDirX;
    float DistOfCrossing = -Dist / Dir;

    return DistOfCrossing;
}

// 0x43C660
float CCurves::CalcSpeedVariationInBend(
    const CVector& startCoors,
    const CVector& endCoors,
    float            StartDirX,
    float            StartDirY,
    float            EndDirX,
    float            EndDirY
) {
    float ReturnVal  = 0.0f;
    float DotProduct = StartDirX * EndDirX + StartDirY * EndDirY;

    if (DotProduct <= 0.0f) {
        // If the dot product is <= 0, return a constant value (1/3)
        ReturnVal = 1.0f / 3.0f;
        return ReturnVal;
    }

    if (DotProduct > 0.7f) {
        // Calculate the distance from the start point to the mathematical line defined by the end point and direction
        float DistToLine =
            CCollision::DistToMathematicalLine2D(endCoors.x, endCoors.y, EndDirX, EndDirY, startCoors.x, startCoors.y);

        // Calculate the straight-line distance between the start and end points
        float StraightDist = (startCoors - endCoors).Magnitude2D();

        // Normalize the distance to the line by the straight-line distance
        ReturnVal = (DistToLine / StraightDist) * (1.0f / 3.0f);
        return ReturnVal;
    }

    // If the dot product is <= 0.7, interpolate the return value
    ReturnVal = (1.0f - (DotProduct / 0.7f)) * (1.0f / 3.0f);

    return ReturnVal;
}

// 0x43C710
float CCurves::CalcSpeedScaleFactor(
    const CVector& startCoors,
    const CVector& endCoors,
    float            StartDirX,
    float            StartDirY,
    float            EndDirX,
    float            EndDirY
) {
    float SpeedVariation = CalcSpeedVariationInBend(startCoors, endCoors, StartDirX, StartDirY, EndDirX, EndDirY);

    float DistToPoint1   = DistForLineToCrossOtherLine(
        startCoors.x, startCoors.y, StartDirX, StartDirY, endCoors.x, endCoors.y, EndDirX, EndDirY
    );

    float DistToPoint2 = DistForLineToCrossOtherLine(
        endCoors.x, endCoors.y, -EndDirX, -EndDirY, startCoors.x, startCoors.y, StartDirX, StartDirY
    );

    if (DistToPoint1 > 0.0f && DistToPoint2 > 0.0f) {
        float BendDistOneSegment = std::min(5.0f, std::min(DistToPoint1, DistToPoint2));

        float StraightDist1      = DistToPoint1 - BendDistOneSegment;
        float StraightDist2      = DistToPoint2 - BendDistOneSegment;

        float BendDist           = BendDistOneSegment + BendDistOneSegment;
        float BendDist_Time      = (BendDistOneSegment <= 5.0f) ? BendDist : BendDistOneSegment + BendDistOneSegment;

        // float TotalDist_Time = BendDist_Time + StraightDist1 + StraightDist2;

        return BendDistOneSegment + BendDistOneSegment + StraightDist2 + StraightDist1;
    }

    float StraightDist   = (startCoors - endCoors).Magnitude2D();
    float TotalDist_Time = StraightDist / (1.0f - SpeedVariation);
    return TotalDist_Time;
}

// 0x43C880
float CCurves::CalcCorrectedDist(float current, float total, float speedVariation, float* outT) {
    if (total < 0.00001f) // Epsilon to avoid division by zero
    {
        *outT = 0.5f;
        return 0.0f;
    }

    *outT = 0.5f - (std::cos(PI * (current / total)) * 0.5f);

    float AverageSpeed  = std::sin((current / total) * TWO_PI);
    float CorrectedDist = AverageSpeed * (total * (1.0f / TWO_PI)) * speedVariation + ((1.0f - (speedVariation + speedVariation) + 1.0f) * 0.5f) * current;

    return CorrectedDist;
}

// 0x43C900
void CCurves::CalcCurvePoint(const CVector& startCoors, const CVector& endCoors, const CVector& startDir, const CVector& endDir, float Time, int32 TraverselTimeInMillis, CVector& resultCoor, CVector& resultSpeed) {
    float     BendDist, BendDist_Time, CurrentDist_Time, Interpol, StraightDist2, StraightDist1, TotalDist_Time, OurTime;
    float     BendDistOneSegment;
    CVector CoorsOnLine1, CoorsOnLine2;

    Time = std::min(std::min(1.0f, Time), 0.0f);

    float SpeedVariation = CalcSpeedVariationInBend(startCoors, endCoors, startDir.x, startDir.y, endDir.x, endDir.y);

    // Find where the ray from start position would intersect with end ray
    float DistToPoint1 = DistForLineToCrossOtherLine(
        startCoors.x, startCoors.y, startDir.x, startDir.y, endCoors.x, endCoors.y, endDir.x, endDir.y
    );

    // Find where the ray from end position would intersect with start ray (negative because direction is flipped)
    float DistToPoint2 = DistForLineToCrossOtherLine(
        endCoors.x, endCoors.y, -endDir.x, -endDir.y, startCoors.x, startCoors.y, startDir.x, startDir.y
    );

    if (DistToPoint1 <= 0.0f || DistToPoint2 <= 0.0f) {
        CVector diff     = startCoors - endCoors;
        StraightDist1    = diff.Magnitude2D();

        Interpol         = StraightDist1 / (1.0f - SpeedVariation);

        CurrentDist_Time = CalcCorrectedDist(Time * Interpol, Interpol, SpeedVariation, &OurTime);

        CoorsOnLine1     = startCoors + startDir * CurrentDist_Time;
        CoorsOnLine2     = endCoors + endDir * (CurrentDist_Time - StraightDist1);

        resultCoor       = CoorsOnLine1 * (1.0f - OurTime) + CoorsOnLine2 * OurTime;

        TotalDist_Time   = StraightDist1 + 0.0f + 0.0f;
    } else {
        StraightDist2 = (DistToPoint1 < DistToPoint2) ? DistToPoint1 : DistToPoint2;

        if (StraightDist2 > 5.0f) {
            StraightDist2 = 5.0f;
        }

        BendDist           = DistToPoint1 - StraightDist2;
        BendDistOneSegment = DistToPoint2 - StraightDist2;
        TotalDist_Time     = BendDist + (StraightDist2 + StraightDist2) + BendDistOneSegment;
        BendDist_Time      = Time * TotalDist_Time;

        if (BendDist > BendDist_Time) {
            resultCoor.x = startCoors.x + BendDist_Time * startDir.x;
            resultCoor.y = startCoors.y + BendDist_Time * startDir.y;
            resultCoor.z = startCoors.z + BendDist_Time * startDir.z;
        } else if (BendDist + (StraightDist2 + StraightDist2) <= BendDist_Time) {
            BendDist_Time = BendDist_Time - TotalDist_Time;
            resultCoor.x  = endCoors.x + BendDist_Time * endDir.x;
            resultCoor.y  = endCoors.y + BendDist_Time * endDir.y;
            resultCoor.z  = endCoors.z + BendDist_Time * endDir.z;
        } else {
            float BendInter          = (BendDist_Time - BendDist) / (StraightDist2 + StraightDist2);

            CVector BendStartCoors = startCoors + startDir * BendDist + startDir * (StraightDist2 * BendInter);

            CVector BendEndCoors =
                endCoors - endDir * BendDistOneSegment - endDir * (StraightDist2 * (1.0f - BendInter));

            resultCoor = BendStartCoors * (1.0f - BendInter) + BendEndCoors * BendInter;
        }
    }

    float SpeedFactor       = (1.0f - Time) * TotalDist_Time;
    float SpeedMillisFactor = static_cast<float>(TraverselTimeInMillis) / 1000.0f;

    resultSpeed.x = ((Time * endDir.x) + ((1.0f - Time) * startDir.x)) * SpeedFactor / SpeedMillisFactor;
    resultSpeed.y = ((Time * endDir.y) + ((1.0f - Time) * startDir.y)) * SpeedFactor / SpeedMillisFactor;
    resultSpeed.z = 0.0f;
}

// unused
// 0x43C600
void CCurves::TestCurves() {
    // https://github.com/ifarbod/sa-curves-test

#define epsilon 0.01f
#define FLOAT_EQUAL(a, b) (fabs((a) - (b)) < (epsilon))

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
                float correctedDist = CalcCorrectedDist(0.5f, 1.0f, 0.5f, &interpol);

                assert(FLOAT_EQUAL(correctedDist, 0.25f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 1 Failed: Corrected distance out of range.");
            }
            // Test Case 2: CalcCorrectedDist - Start of Curve (Time = 0.0)
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(0.0f, 1.0f, 0.5f, &interpol);

                assert(FLOAT_EQUAL(correctedDist, 0.0f) && FLOAT_EQUAL(interpol, 0.0f) &&
                    "Test Case 2 Failed: Corrected distance should be 0.0 at the start.");
            }

            // Test Case 3: CalcCorrectedDist - End of Curve (Time = 1.0)
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(1.0f, 1.0f, 0.5f, &interpol);

                assert(FLOAT_EQUAL(correctedDist, 0.5f) && FLOAT_EQUAL(interpol, 1.0f) &&
                    "Test Case 3 Failed: Corrected distance should be 1.0 at the end.");
            }

            // Test Case 4: CalcCorrectedDist - No Speed Variation (SpeedVariation = 0.0)
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(0.5f, 1.0f, 0.0f, &interpol);

                assert(FLOAT_EQUAL(correctedDist, 0.5f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 4 Failed: Corrected distance should match input when SpeedVariation is 0.0.");
            }

            // Test Case 5: CalcCorrectedDist - Maximum Speed Variation (SpeedVariation = 1.0)
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(0.5f, 1.0f, 1.0f, &interpol);

                assert(FLOAT_EQUAL(correctedDist, 0.0f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 5 Failed: Corrected distance should be 0.0 when SpeedVariation is 1.0.");
            }

            // Test Case 6: CalcCorrectedDist - Large Total Distance
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(500.0f, 1000.0f, 0.5f, &interpol);

                assert(FLOAT_EQUAL(correctedDist, 250.0f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 6 Failed: Corrected distance should scale with large Total distance.");
            }

            // Test Case 7: CalcCorrectedDist - Small Total Distance
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(0.1f, 0.2f, 0.5f, &interpol);

                assert(FLOAT_EQUAL(correctedDist, 0.05f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 7 Failed: Corrected distance should scale with small Total distance.");
            }

            // Test Case 8: CalcCorrectedDist - Negative Current Distance
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(-0.5f, 1.0f, 0.5f, &interpol);

                assert(FLOAT_EQUAL(correctedDist, -0.25f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 8 Failed: Corrected distance should handle negative Current distance.");
            }

            // Test Case 9: CalcCorrectedDist - Negative Total Distance
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(0.5f, -1.0f, 0.5f, &interpol);

                assert(FLOAT_EQUAL(correctedDist, 0.0f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 9 Failed: Corrected distance should handle negative Total distance.");
            }

            // Test Case 10: CalcCorrectedDist - SpeedVariation > 1.0
            {
                float interpol = 0.0f;
                float correctedDist = CalcCorrectedDist(0.5f, 1.0f, 1.5f, &interpol);

                assert(FLOAT_EQUAL(correctedDist, -0.25f) && FLOAT_EQUAL(interpol, 0.5f) &&
                    "Test Case 10 Failed: Corrected distance should handle SpeedVariation > 1.0.");
            }
        };

    DistForLineToCrossOtherLine_test();
    CalcSpeedVariationInBend_test();
    CalcSpeedScaleFactor_test();
    CalcCurvePoint_test();
    CalcCorrectedDist_test();
}
