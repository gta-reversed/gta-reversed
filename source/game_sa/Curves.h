#pragma once

#include "Vector.h"

class CVector;

/*!
* All instances of `CVector` that could be changed were changed to be `CVector2D` as the z component is ignored every time.
*/
class CCurves {
public:
    static void InjectHooks();

    static void TestCurves();
    /*!
    * @addr 0x43C610
    * 
    * @brief Calculate the smallest distance needed for 2 mathematical lines to cross
    * 
    * @param lineStart The first line's base point
    * @param lineDir The first line's direction
    * @param otherLineStart The other line's base point
    * @param otherLineDir The other line's direction
    *
    * Calculates the distance to the closest point on the math. line defined by (`otherLineStart`, `otherLineEnd`)
    * that is crossed by the math. line defined by (`lineStart`, `lineDir`).
    */
    static float DistForLineToCrossOtherLine(CVector2D lineStart, CVector2D lineDir, CVector2D otherLineStart, CVector2D otherLineDir); // Originally last 2 vectors were 2 floats each, changed it for readability
    static float CalcSpeedVariationInBend(CVector2D const& startCoors, CVector2D const& endCoors, CVector2D startDir, CVector2D endDir); // Same as above
    static float CalcSpeedScaleFactor(CVector2D const& startCoors, CVector2D const& endCoors, CVector2D startDir, CVector2D endDir); // Same as above
    static float CalcCorrectedDist(float curr, float total, float variance, float& outT);
    static void  CalcCurvePoint(
        const CVector& startPos,
        const CVector& endPos,
        const CVector& startDir,
        const CVector& endDir,
        float time,
        int32 totalTimeMs,
        CVector& outPos,
        CVector& outSpeed
    );
};
