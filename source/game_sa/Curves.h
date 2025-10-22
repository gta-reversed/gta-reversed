#pragma once

#include "Vector.h"

class CVector;

class CCurves {
public:
    static void InjectHooks();

    static void TestCurves();

    static float DistForLineToCrossOtherLine(float LineBaseX, float LineBaseY, float LineDirX, float LineDirY, float OtherLineBaseX, float OtherLineBaseY, float OtherLineDirX, float OtherLineDirY);

    static float CalcSpeedVariationInBend(const CVector& startCoors, const CVector& endCoors, float StartDirX, float StartDirY, float EndDirX, float EndDirY);

    static float CalcSpeedScaleFactor(const CVector& startCoors, const CVector& endCoors, float StartDirX, float StartDirY, float EndDirX, float EndDirY);

    static void CalcCurvePoint(const CVector& startCoors, const CVector& endCoors, const CVector& startDir, const CVector& endDir, float Time, int32 TraverselTimeInMillis, CVector& resultCoor, CVector& resultSpeed);

    static float CalcCorrectedDist(float Current, float Total, float SpeedVariation, float* pInterPol);
};
