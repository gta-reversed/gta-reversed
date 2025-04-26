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
     *
     * @brief Calculate the shortest distance needed for 2 mathematical lines to cross
     *
     * @param lineBase The first line's base point
     * @param lineDir The first line's direction
     * @param otherLineBase The other line's base point
     * @param otherLineDir The other line's direction
     *
     * Calculates the distance to the closest point on the math. line defined by (`otherLineBase`, `otherLineDir`)
     * that is crossed by the math. line defined by (`lineBase`, `lineDir`).
     * Returns -1.0f if the lines are parallel.
     */
    static float DistForLineToCrossOtherLine(const CVector2D lineBase, const CVector2D lineDir, const CVector2D otherLineBase, const CVector2D otherLineDir);

    /*!
     * @brief Calculate speed variation in a bend based on direction vectors
     * @brief Higher values indicate sharper bends requiring more speed reduction
     */
    static float CalcSpeedVariationInBend(const CVector &startCoors, const CVector &endCoors, const CVector2D StartDir, const CVector2D EndDir);

    /*!
     * @brief Calculate a speed scale factor between two points considering their directions
     */
    static float CalcSpeedScaleFactor(const CVector &startCoors, const CVector &endCoors, const CVector2D StartDir, const CVector2D EndDir);

    /*!
     * @brief Calculate corrected distance for smooth curve traversal
     */
    static float CalcCorrectedDist(float Current, float Total, float SpeedVariation, float &pInterPol);

    /*!
     * @addr 0x43C900
     *
     * @brief Calculate a point on a smooth curve between two positions with direction vectors
     * @brief The curve consists of straight segments and a bend connecting them for natural-looking movement
     ***/
    static void CalcCurvePoint(const CVector &startCoors,   ///< Start position
                               const CVector &endCoors,     ///< End position
                               const CVector &startDir,     ///< Start direction
                               const CVector &endDir,       ///< End direction
                               float Time,                  ///< Current (t)ime (interpolation) parameter
                               int32 TraverselTimeInMillis, ///< Total time to traverse the curve
                               CVector &resultCoor,         ///< [out] Output position
                               CVector &resultSpeed         ///< [out] Output speed
    );
};
