#pragma once

#include "Vector.h"

class CVector;

class CCurves {
public:
    static void InjectHooks();

    static void TestCurves();

    /*!
    * @brief Calculate the shortest distance needed for 2 mathematical lines to cross
    * 
    * @param lineStart The first line's base point
    * @param lineDir The first line's direction
    * @param otherLineStart The other line's base point
    * @param otherLineDir The other line's direction
    *
    * Calculates the distance to the closest point on the math. line defined by (`otherLineStart`, `otherLineEnd`)
    * that is crossed by the math. line defined by (`lineStart`, `lineDir`).
    */
    static float DistForLineToCrossOtherLine(float lineBaseX, float lineBaseY, float lineDirX, float lineDirY, float otherLineBaseX, float otherLineBaseY, float otherLineDirX, float otherLineDirY);

    static float CalcSpeedVariationInBend(const CVector& startCoors, const CVector& endCoors, float startDirX, float startDirY, float endDirX, float endDirY);

    static float CalcSpeedScaleFactor(const CVector& startCoors, const CVector& endCoors, float startDirX, float startDirY, float endDirX, float endDirY);

    /*!
    * @brief Calculate a point on a smooth curve between two positions with direction vectors
    * @brief The curve consists of straight segments and a bend connecting them for natural-looking movement
    */
    static void CalcCurvePoint(const CVector& startCoors, const CVector& endCoors, const CVector& startDir, const CVector& endDir, float time, int32 traversalTimeInMS, CVector& resultCoor, CVector& resultSpeed);

    static float CalcCorrectedDist(float current, float total, float speedVariation, float* outT);
};
