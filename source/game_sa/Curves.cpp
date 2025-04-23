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

float CCurves::DistForLineToCrossOtherLine(float LineBaseX, float LineBaseY, float LineDirX, float LineDirY, float OtherLineBaseX, float OtherLineBaseY, float OtherLineDirX, float OtherLineDirY) {
    float v8; // s2

    v8 = -((LineDirY * OtherLineDirX) - (LineDirX * OtherLineDirY));
    if (v8 == 0.0) {
        return -1.0;
    } else {
        return -(((LineBaseX - OtherLineBaseX) * OtherLineDirY) - ((LineBaseY - OtherLineBaseY) * OtherLineDirX)) / v8;
    }
}

float CCurves::CalcSpeedVariationInBend(const CVector &startCoors, const CVector &endCoors, float StartDirX, float StartDirY, float EndDirX, float EndDirY) {
    float v6;       // s0
    long double v9; // q0

    v6 = (StartDirY * EndDirY) + (StartDirX * EndDirX);
    if (v6 <= 0.0) {
        return 0.33333;
    }
    if (v6 <= 0.7) {
        return ((v6 / -0.7) * 0.33333) + 0.33333;
    }
    v9 = CCollision::DistToMathematicalLine2D(endCoors.x, endCoors.y, EndDirX, EndDirY, startCoors.x, startCoors.y);
    return (*&v9 / sqrtf(((startCoors.y - endCoors.y) * (startCoors.y - endCoors.y)) + ((startCoors.x - endCoors.x) * (startCoors.x - endCoors.x)))) * 0.33333;
}

float CCurves::CalcCorrectedDist(float Current, float Total, float SpeedVariation, float &pInterPol) {
    float v7; // s10
    float v8; // s0

    if (Total >= 0.00001) {
        v7 = (((Total / 6.2832) * SpeedVariation) * sinf((Current * 6.2832) / Total)) + (((((SpeedVariation * -2.0) + 1.0) * 0.5) + 0.5) * Current);
        v8 = 0.5 - (cosf((Current / Total) * 3.1416) * 0.5);
    } else {
        v7 = 0.0;
        v8 = 0.5;
    }
    pInterPol = v8;
    return v7;
}

float CCurves::CalcSpeedScaleFactor(const CVector &startCoors, const CVector &endCoors, float StartDirX, float StartDirY, float EndDirX, float EndDirY) {
    float v10;       // s0
    long double v13; // q0
    float v14;       // s0
    float v15;       // s5
    float x;         // s1
    float y;         // s4
    float v18;       // s2
    float v19;       // s3
    float v20;       // s2
    float v21;       // s1
    float v22;       // s0
    float v23;       // s2
    float v24;       // s6
    float v25;       // s5
    float v26;       // s0


    v15 = -((StartDirY * EndDirX) - (StartDirX * EndDirY));
        x = startCoors.x;
        y = startCoors.y;
        v19 = endCoors.x;
        v18 = endCoors.y;
    if (v15 != 0.0) {
        v24 = -(((startCoors.x - endCoors.x) * EndDirY) - ((y - v18) * EndDirX)) / v15;
        if (v24 > 0.0) {
            v25 = -(((v19 - x) * StartDirY) - ((v18 - y) * StartDirX)) / v15;
            if (v25 > 0.0) {
                v26 = fminf(fminf(v24, v25), 5.0);
                v23 = v24 - v26;
                v21 = v25 - v26;
                v22 = v26 + v26;
                return v22 + (v21 + v23);
            }
        }
    }
    v22 = sqrtf(((y - v18) * (y - v18)) + ((x - v19) * (x - v19))) / (1.0 - CCurves::CalcSpeedVariationInBend(startCoors, endCoors, StartDirX, StartDirY, EndDirX, EndDirY));
    return v22;
}

void CCurves::CalcCurvePoint(const CVector &startCoors, const CVector &endCoors, const CVector &startDir, const CVector &endDir, float Time, Int32 TraverselTimeInMillis, CVector &resultCoor, CVector &resultSpeed) {
    float x;   // s19
    float y;   // s20
    float v12; // s21
    float v13; // s24
    float v14; // s8
    float v16; // s0
    float v21; // s22
    float v25; // s23
    float v26; // s8
    float v27; // s11
    float v28; // s10
    float v29; // s14
    float v30; // s13
    float v31; // s12
    float v32; // s25
    float v33; // s0
    float v34; // s9
    float v35; // s15
    float v36; // s0
    float v37; // s1
    float v38; // s2
    float v39; // s3
    float v40; // s0
    float v41; // s0
    float v42; // s2
    float v43; // s3
    float v44; // s4
    float v47; // s1
    float v48; // s4
    float v49; // s2
    float v50; // s3
    float v51; // s2
    float v52; // s4
    float v53; // s19
    float v54; // s5
    float v55; // s17
    float v56; // s3
    float v57; // s16
    float v58; // s3
    float v59; // [xsp+Ch] [xbp-94h]
    float v60; // [xsp+10h] [xbp-90h]
    float v61; // [xsp+14h] [xbp-8Ch]
    float v62; // [xsp+18h] [xbp-88h]
    float v63; // [xsp+1Ch] [xbp-84h]

    x = startDir.x;
    y = startDir.y;
    v13 = endDir.x;
    v12 = endDir.y;
    v14 = fmaxf(Time, 0.0);
    v16 = (y * v12) + (startDir.x * endDir.x);
    v21 = 1.0;
    v25 = fminf(v14, v21);
    v29 = endCoors.x;
    v28 = endCoors.y;
    v27 = startCoors.x;
    v26 = startCoors.y;
    float v24 = -((y * v13) - (x * v12));
    if (v24 != 0.0) {
        float v45 = -((v12 * (startCoors.x - endCoors.x)) - (v13 * (v26 - v28))) / v24;
        if (v45 > 0.0) {
            float v46 = -((y * (v29 - v27)) - (x * (v28 - v26))) / v24;
            if (v46 > 0.0) {
                v47 = fminf(fminf(v45, v46), 5.0);
                v40 = v45 - v47;
                v30 = v47 + v47;
                v32 = v46 - v47;
                v48 = (v45 - v47) + (v47 + v47);
                v49 = (v46 - v47) + v48;
                v50 = v25 * v49;
                if ((v25 * v49) >= v40) {
                    if (v50 >= v48) {
                        v58 = v50 - v49;
                        v37 = v29 + (v58 * v13);
                        v38 = v28 + (v58 * v12);
                        v39 = endCoors.z + (v58 * endDir.z);
                    } else {
                        v51 = (v50 - v40) / v30;
                        v52 = (v27 + (v40 * x)) + (v47 * (v51 * x));
                        v53 = v21 - v51;
                        v54 = (v26 + (v40 * y)) + (v47 * (v51 * y));
                        v55 = (v28 - (v32 * v12)) - (v47 * ((v21 - v51) * v12));
                        v56 = (startCoors.z + (v40 * startDir.z)) + (v47 * (v51 * startDir.z));
                        v57 = v51 * ((endCoors.z - (v32 * endDir.z)) - (v47 * ((v21 - v51) * endDir.z)));
                        resultCoor.x = (v51 * ((v29 - (v32 * v13)) - (v47 * ((v21 - v51) * v13)))) + ((v21 - v51) * v52);
                        resultCoor.y = (v51 * v55) + ((v21 - v51) * v54);
                        resultCoor.z = v57 + (v53 * v56);
                    }
                } else {
                    resultCoor.x = v27 + (v50 * x);
                    resultCoor.y = v26 + (v50 * y);
                    resultCoor.z = startCoors.z + (v50 * startDir.z);
                }
                v41 = v32 + (v30 + v40);
                v42 = TraverselTimeInMillis * 0.001;
                v43 = endDir.y;
                v44 = startDir.y;
                resultSpeed.x = (v41 * ((v25 * endDir.x) + ((v21 - v25) * startDir.x))) / v42;
                resultSpeed.z = 0.0;
                resultSpeed.y = (v41 * ((v25 * v43) + ((v21 - v25) * v44))) / v42;
                return;
            } 
        }
    }
    v23 = CCurves::CalcSpeedVariationInBend(startCoors, endCoors, startDir.x, startDir.y, endDir.x, endDir.y);
    v30 = sqrtf(((v26 - v28) * (v26 - v28)) + ((v27 - v29) * (v27 - v29)));
    v31 = v30 / (v21 - v23);
    v32 = 0.0;
    if (v31 >= 0.00001) {
        v35 = v25 * v31;
        v59 = x;
        v62 = v12;
        v63 = v25;
        v60 = y;
        v61 = v13;
        v34 = ((v23 * (v31 / 6.2832)) * sinf(((v25 * v31) * 6.2832) / v31)) + ((((v21 + (v23 * -2.0)) * 0.5) + 0.5) * (v25 * v31));
        v36 = cosf((v35 / v31) * 3.1416);
        x = v59;
        y = v60;
        v13 = v61;
        v12 = v62;
        v25 = v63;
        v21 = 1.0;
        v33 = 0.5 - (v36 * 0.5);
    } else {
        v33 = 0.5;
        v34 = 0.0;
    }
    resultCoor.x = (v33 * (v29 + ((v34 - v30) * v13))) + ((v21 - v33) * (v27 + (v34 * x)));
    resultCoor.y = (v33 * (v28 + ((v34 - v30) * v12))) + ((v21 - v33) * (v26 + (v34 * y)));
    resultCoor.z = (v33 * (endCoors.z + ((v34 - v30) * endDir.z))) + ((v21 - v33) * (startCoors.z + (v34 * startDir.z)));
    v40 = 0.0;
    v41 = v32 + (v30 + v40);
    v42 = TraverselTimeInMillis * 0.001;
    v43 = endDir.y;
    v44 = startDir.y;
    resultSpeed.x = (v41 * ((v25 * endDir.x) + ((v21 - v25) * startDir.x))) / v42;
    resultSpeed.z = 0.0;
    resultSpeed.y = (v41 * ((v25 * v43) + ((v21 - v25) * v44))) / v42;
}

// unused
// 0x43C600
void CCurves::TestCurves() {
    // https://github.com/ifarbod/sa-curves-test

#define epsilon 0.01f
#define FLOAT_EQUAL(a, b) (fabs((a) - (b)) < (epsilon))

    // const auto FLOAT_EQUAL = [](float a, float b) {
    //     return fabs(a - b) < 0.0002f;
    // };

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

    auto CalcCurvePoint_test = [] {
        // Test Case 1: CalcCurvePoint - Straight Line
        {
            CVector startCoors(0.0f, 0.0f, 0.0f);
            CVector endCoors(1.0f, 0.0f, 0.0f);
            CVector startDir(1.0f, 0.0f, 0.0f);
            CVector endDir(1.0f, 0.0f, 0.0f);
            CVector resultCoor;
            CVector resultSpeed;
            CalcCurvePoint(startCoors, endCoors, startDir, endDir, 0.5f, 1'000, resultCoor, resultSpeed);

            assert(FLOAT_EQUAL(resultCoor.x, 0.5f) && FLOAT_EQUAL(resultCoor.y, 0.0f) && FLOAT_EQUAL(resultCoor.z, 0.0f) && "Test Case 1 Failed: Incorrect curve point.");
        }

        // Test Case 2: CalcCurvePoint - Curve with Bend
        {
            CVector startCoors(0.0f, 0.0f, 0.0f);
            CVector endCoors(1.0f, 1.0f, 0.0f);
            CVector startDir(1.0f, 0.0f, 0.0f);
            CVector endDir(0.0f, 1.0f, 0.0f);
            CVector resultCoor;
            CVector resultSpeed;
            CalcCurvePoint(startCoors, endCoors, startDir, endDir, 0.5f, 1'000, resultCoor, resultSpeed);

            // Expected result depends on the interpolation logic
            assert(resultCoor.x > 0.0f && resultCoor.x < 1.0f && resultCoor.y > 0.0f && resultCoor.y < 1.0f && FLOAT_EQUAL(resultCoor.z, 0.0f) && "Test Case 2 Failed: Incorrect curve point.");
        }

        // Test Case 3: CalcCurvePoint - Large Values
        {
            CVector startCoors(0.0f, 0.0f, 0.0f);
            CVector endCoors(1000.0f, 1000.0f, 0.0f);
            CVector startDir(1.0f, 0.0f, 0.0f);
            CVector endDir(0.0f, 1.0f, 0.0f);
            CVector resultCoor;
            CVector resultSpeed;
            CalcCurvePoint(startCoors, endCoors, startDir, endDir, 0.5f, 1'000, resultCoor, resultSpeed);

            // Expected result depends on the interpolation logic
            assert(resultCoor.x > 0.0f && resultCoor.x < 1000.0f && resultCoor.y > 0.0f && resultCoor.y < 1000.0f && FLOAT_EQUAL(resultCoor.z, 0.0f) && "Test Case 3 Failed: Incorrect curve point.");
        }

        // Test Case 4: CalcCurvePoint - Time = 0.0 (Start Point)
        {
            CVector startCoors(0.0f, 0.0f, 0.0f);
            CVector endCoors(1.0f, 1.0f, 0.0f);
            CVector startDir(1.0f, 0.0f, 0.0f);
            CVector endDir(0.0f, 1.0f, 0.0f);
            CVector resultCoor;
            CVector resultSpeed;
            CalcCurvePoint(startCoors, endCoors, startDir, endDir, 0.0f, 1'000, resultCoor, resultSpeed);

            assert(FLOAT_EQUAL(resultCoor.x, 0.0f) && FLOAT_EQUAL(resultCoor.y, 0.0f) && FLOAT_EQUAL(resultCoor.z, 0.0f) && "Test Case 4 Failed: Incorrect curve point.");
        }

        // Test Case 5: CalcCurvePoint - Time = 1.0 (End Point)
        {
            CVector startCoors(0.0f, 0.0f, 0.0f);
            CVector endCoors(1.0f, 1.0f, 0.0f);
            CVector startDir(1.0f, 0.0f, 0.0f);
            CVector endDir(0.0f, 1.0f, 0.0f);
            CVector resultCoor;
            CVector resultSpeed;
            CalcCurvePoint(startCoors, endCoors, startDir, endDir, 1.0f, 1'000, resultCoor, resultSpeed);

            assert(FLOAT_EQUAL(resultCoor.x, 1.0f) && FLOAT_EQUAL(resultCoor.y, 1.0f) && FLOAT_EQUAL(resultCoor.z, 0.0f) && "Test Case 5 Failed: Incorrect curve point.");
        }

        // Test Case 6: CalcCurvePoint - Z-Axis Movement
        {
            CVector startCoors(0.0f, 0.0f, 0.0f);
            CVector endCoors(0.0f, 0.0f, 1.0f);
            CVector startDir(0.0f, 0.0f, 1.0f);
            CVector endDir(0.0f, 0.0f, 1.0f);
            CVector resultCoor;
            CVector resultSpeed;
            CalcCurvePoint(startCoors, endCoors, startDir, endDir, 0.5f, 1'000, resultCoor, resultSpeed);

            assert(FLOAT_EQUAL(resultCoor.x, 0.0f) && FLOAT_EQUAL(resultCoor.y, 0.0f) && FLOAT_EQUAL(resultCoor.z, 0.5f) && "Test Case 6 Failed: Incorrect curve point.");
        }

        // Test Case 7: CalcCurvePoint - Sharp Bend
        {
            CVector startCoors(0.0f, 0.0f, 0.0f);
            CVector endCoors(1.0f, 0.0f, 0.0f);
            CVector startDir(1.0f, 0.0f, 0.0f);
            CVector endDir(-1.0f, 0.0f, 0.0f);
            CVector resultCoor;
            CVector resultSpeed;
            CalcCurvePoint(startCoors, endCoors, startDir, endDir, 0.5f, 1'000, resultCoor, resultSpeed);

            // Expected result depends on the interpolation logic
            assert(FLOAT_EQUAL(resultCoor.x, 1.0f) && FLOAT_EQUAL(resultCoor.y, 0.0f) && FLOAT_EQUAL(resultCoor.z, 0.0f) && "Test Case 7 Failed: Incorrect curve point.");
        }
    };

    auto CalcCorrectedDist_test = [] {
        // Test Case 1: CalcCorrectedDist - Simple Case
        {
            // interpol please don't arrest us, we have done nothing!
            float interpol = 0.0f;
            float correctedDist = CalcCorrectedDist(0.5f, 1.0f, 0.5f, interpol);

            assert(FLOAT_EQUAL(correctedDist, 0.25f) && FLOAT_EQUAL(interpol, 0.5f) && "Test Case 1 Failed: Corrected distance out of range.");
        }
        // Test Case 2: CalcCorrectedDist - Start of Curve (Time = 0.0)
        {
            float interpol = 0.0f;
            float correctedDist = CalcCorrectedDist(0.0f, 1.0f, 0.5f, interpol);

            assert(FLOAT_EQUAL(correctedDist, 0.0f) && FLOAT_EQUAL(interpol, 0.0f) && "Test Case 2 Failed: Corrected distance should be 0.0 at the start.");
        }

        // Test Case 3: CalcCorrectedDist - End of Curve (Time = 1.0)
        {
            float interpol = 0.0f;
            float correctedDist = CalcCorrectedDist(1.0f, 1.0f, 0.5f, interpol);

            assert(FLOAT_EQUAL(correctedDist, 0.5f) && FLOAT_EQUAL(interpol, 1.0f) && "Test Case 3 Failed: Corrected distance should be 1.0 at the end.");
        }

        // Test Case 4: CalcCorrectedDist - No Speed Variation (SpeedVariation = 0.0)
        {
            float interpol = 0.0f;
            float correctedDist = CalcCorrectedDist(0.5f, 1.0f, 0.0f, interpol);

            assert(FLOAT_EQUAL(correctedDist, 0.5f) && FLOAT_EQUAL(interpol, 0.5f) && "Test Case 4 Failed: Corrected distance should match input when SpeedVariation is 0.0.");
        }

        // Test Case 5: CalcCorrectedDist - Maximum Speed Variation (SpeedVariation = 1.0)
        {
            float interpol = 0.0f;
            float correctedDist = CalcCorrectedDist(0.5f, 1.0f, 1.0f, interpol);

            assert(FLOAT_EQUAL(correctedDist, 0.0f) && FLOAT_EQUAL(interpol, 0.5f) && "Test Case 5 Failed: Corrected distance should be 0.0 when SpeedVariation is 1.0.");
        }

        // Test Case 6: CalcCorrectedDist - Large Total Distance
        {
            float interpol = 0.0f;
            float correctedDist = CalcCorrectedDist(500.0f, 1000.0f, 0.5f, interpol);

            assert(FLOAT_EQUAL(correctedDist, 250.0f) && FLOAT_EQUAL(interpol, 0.5f) && "Test Case 6 Failed: Corrected distance should scale with large Total distance.");
        }

        // Test Case 7: CalcCorrectedDist - Small Total Distance
        {
            float interpol = 0.0f;
            float correctedDist = CalcCorrectedDist(0.1f, 0.2f, 0.5f, interpol);

            assert(FLOAT_EQUAL(correctedDist, 0.05f) && FLOAT_EQUAL(interpol, 0.5f) && "Test Case 7 Failed: Corrected distance should scale with small Total distance.");
        }

        // Test Case 8: CalcCorrectedDist - Negative Current Distance
        {
            float interpol = 0.0f;
            float correctedDist = CalcCorrectedDist(-0.5f, 1.0f, 0.5f, interpol);

            assert(FLOAT_EQUAL(correctedDist, -0.25f) && FLOAT_EQUAL(interpol, 0.5f) && "Test Case 8 Failed: Corrected distance should handle negative Current distance.");
        }

        // Test Case 9: CalcCorrectedDist - Negative Total Distance
        {
            float interpol = 0.0f;
            float correctedDist = CalcCorrectedDist(0.5f, -1.0f, 0.5f, interpol);

            assert(FLOAT_EQUAL(correctedDist, 0.0f) && FLOAT_EQUAL(interpol, 0.5f) && "Test Case 9 Failed: Corrected distance should handle negative Total distance.");
        }

        // Test Case 10: CalcCorrectedDist - SpeedVariation > 1.0
        {
            float interpol = 0.0f;
            float correctedDist = CalcCorrectedDist(0.5f, 1.0f, 1.5f, interpol);

            assert(FLOAT_EQUAL(correctedDist, -0.25f) && FLOAT_EQUAL(interpol, 0.5f) && "Test Case 10 Failed: Corrected distance should handle SpeedVariation > 1.0.");
        }
    };

    // DistForLineToCrossOtherLine_test();
    // CalcSpeedVariationInBend_test();
    // CalcSpeedScaleFactor_test();
    CalcCurvePoint_test();
    CalcCorrectedDist_test();
}
