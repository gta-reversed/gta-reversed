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
    static float DistForLineToCrossOtherLine(CVector2D lineStart, CVector2D lineDir, CVector2D otherLineStart, CVector2D otherLineDir);
    static float CalcSpeedVariationInBend(CVector2D const& startCoors, CVector2D const& endCoors, CVector2D startDir, CVector2D endDir);
    static float CalcSpeedScaleFactor(CVector2D const& startCoors, CVector2D const& endCoors, CVector2D startDir, CVector2D endDir);
    static float CalcCorrectedDist(float curr, float total, float variance, float& outT);

    /*!
    * @addr 0x43C900
    * 
    * @brief Calculate a point on a smooth curve between two positions with direction vectors
    * @brief The curve consists of straight segments and a bend connecting them for natural-looking movement
    ***/
    static void  CalcCurvePoint(
         const CVector& ptA3D,           ///< Start position
         const CVector& ptB3D,           ///< End position
         const CVector& dirA3D,          ///< Start direction
         const CVector& dirB3D,          ///< End direction
         float          t,               ///< Curent (t)ime (interpolation) parameter
         int32          traversalTimeMs, ///< Total time to traverse the curve
         CVector&       outPos,          ///< [out] Output position
         CVector&       outSpeed         ///< [out] Output speed
    );
};
