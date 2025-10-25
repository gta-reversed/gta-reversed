#pragma once

#include "Vector.h"

class CVector;

class CCurves {
public:
    static void InjectHooks();

    static void TestCurves();

    static float DistForLineToCrossOtherLine(float lineBaseX, float lineBaseY, float lineDirX, float lineDirY, float otherLineBaseX, float otherLineBaseY, float otherLineDirX, float otherLineDirY);

    static float CalcSpeedVariationInBend(const CVector& startCoors, const CVector& endCoors, float startDirX, float startDirY, float endDirX, float endDirY);

    static float CalcSpeedScaleFactor(const CVector& startCoors, const CVector& endCoors, float startDirX, float startDirY, float endDirX, float endDirY);

    static void CalcCurvePoint(const CVector& startCoors, const CVector& endCoors, const CVector& startDir, const CVector& endDir, float time, int32 traversalTimeInMS, CVector& resultCoor, CVector& resultSpeed);

    static float CalcCorrectedDist(float current, float total, float speedVariation, float* outT);
};
