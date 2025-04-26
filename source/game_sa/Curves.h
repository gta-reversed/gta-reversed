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
    * @param LineBaseX The first line's base point X coordinate
    * @param LineBaseY The first line's base point Y coordinate
    * @param LineDirX The first line's direction X component
    * @param LineDirY The first line's direction Y component
    * @param OtherLineBaseX The other line's base point X coordinate
    * @param OtherLineBaseY The other line's base point Y coordinate
    * @param OtherLineDirX The other line's direction X component
    * @param OtherLineDirY The other line's direction Y component
    *
    * Calculates the distance to the closest point on the math. line defined by (`OtherLineBaseX/Y`, `OtherLineDirX/Y`)
    * that is crossed by the math. line defined by (`LineBaseX/Y`, `LineDirX/Y`).
    * Returns -1.0f if the lines are parallel.
    */
    static float DistForLineToCrossOtherLine(
        float LineBaseX,
        float LineBaseY,
        float LineDirX,
        float LineDirY,
        float OtherLineBaseX,
        float OtherLineBaseY,
        float OtherLineDirX,
        float OtherLineDirY
    );

    /*!
    * @brief Calculate speed variation in a bend based on direction vectors
    * @brief Higher values indicate sharper bends requiring more speed reduction
    */
    static float CalcSpeedVariationInBend(
        const CVector& startCoors,
        const CVector& endCoors,
        float StartDirX,
        float StartDirY,
        float EndDirX,
        float EndDirY
    );

    /*!
    * @brief Calculate a speed scale factor between two points considering their directions
    */
    static float CalcSpeedScaleFactor(
        const CVector& startCoors,
        const CVector& endCoors,
        float StartDirX,
        float StartDirY,
        float EndDirX,
        float EndDirY
    );

    /*!
    * @brief Calculate corrected distance for smooth curve traversal
    */
    static float CalcCorrectedDist(float Current, float Total, float SpeedVariation, float& pInterPol);

    /*!
    * @addr 0x43C900
    * 
    * @brief Calculate a point on a smooth curve between two positions with direction vectors
    * @brief The curve consists of straight segments and a bend connecting them for natural-looking movement
    ***/
    static void  CalcCurvePoint(
         const CVector& startCoors,    ///< Start position
         const CVector& endCoors,      ///< End position
         const CVector& startDir,      ///< Start direction
         const CVector& endDir,        ///< End direction
         float          Time,          ///< Current (t)ime (interpolation) parameter
         int32          TraverselTimeInMillis, ///< Total time to traverse the curve
         CVector&       resultCoor,    ///< [out] Output position
         CVector&       resultSpeed    ///< [out] Output speed
    );
};
