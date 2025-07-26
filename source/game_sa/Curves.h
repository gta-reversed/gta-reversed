#pragma once

#include "Vector.h"

class CVector;

/*!
 * All instances of `CVector` that could be changed were changed to be `CVector2D` as the z component is ignored every time.
 */
class CCurves {
  public:
    static void InjectHooks();

    /// Runs the unit tests
    static void TestCurves();

    /// Calculate the smallest distance needed for two mathematical lines to cross.
    ///
    /// \param LineBaseX The x-coordinate of the first line's base point.
    /// \param LineBaseY The y-coordinate of the first line's base point.
    /// \param LineDirX The x-component of the first line's direction vector.
    /// \param LineDirY The y-component of the first line's direction vector.
    /// \param OtherLineBaseX The x-coordinate of the other line's base point.
    /// \param OtherLineBaseY The y-coordinate of the other line's base point.
    /// \param OtherLineDirX The x-component of the other line's direction vector.
    /// \param OtherLineDirY The y-component of the other line's direction vector.
    ///
    /// \return The distance along the first line to the point where it crosses the other line.
    ///         Returns -1.0 if the lines are parallel and do not intersect.
    ///
    /// This function determines whether and where two infinite lines, defined by their base points and
    /// direction vectors, will intersect. It computes the scaling factor for the first line's direction vector such
    /// that, when applied, the corresponding point lies on both lines. If the direction vectors are linearly dependent
    /// (i.e., the lines are parallel), the function concludes that an intersection does not occur and returns -1.0. The
    /// returned distance represents the smallest distance along the first line necessary to reach the crossing point
    /// with the second line, if such a crossing exists.
    static float DistForLineToCrossOtherLine(CVector2D lineBase, CVector2D lineDir, CVector2D otherLineBase, CVector2D otherLineDir);

    /// Calculates the speed variation when traversing a bend defined by the start and end coordinates and directions.
    /// \param startCoors The starting coordinates of the curve.
    /// \param endCoors The ending coordinates of the curve.
    /// \param startDir The coordinates of the starting direction.
    /// \param endDir The coordinates of the ending direction.
    /// \return The computed speed variation factor influenced by the bend in the curve.
    ///
    /// This function computes how much the speed should vary while moving through a curved path
    /// based on the bend's geometry. It analyzes the curve defined by start/end points and their
    /// corresponding direction vectors to determine an appropriate speed variation factor.
    ///
    /// The variation factor helps adjust vehicle speed based on curve sharpness - sharper curves
    /// generally require more speed reduction compared to gentle curves. This creates more
    /// realistic and smoother motion through curved paths.
    static float CalcSpeedVariationInBend(
        const CVector& startCoors,
        const CVector& endCoors,
        CVector2D      startDir,
        CVector2D      endDir
    );

    /// Computes the speed scaling factor for a curve defined by its start and end coordinates and directions.
    /// \param startCoors The starting coordinates of the curve.
    /// \param endCoors The ending coordinates of the curve.
    /// \param startDir The coordinates of the starting direction.
    /// \param endDir The coordinates of the ending direction.
    /// \return The calculated speed scaling factor for the curve.
    ///
    /// This function calculates a scaling factor that adjusts the speed of an object moving along a curve.
    /// The scaling factor is influenced by the geometry of the curve, specifically the angle between the
    /// start and end direction vectors. A sharper bend (larger angle) results in a higher scaling factor,
    /// indicating that the object should slow down more to navigate the curve smoothly. Conversely, a gentle
    /// bend (smaller angle) results in a lower scaling factor, allowing the object to maintain a higher speed.
    ///
    /// The function first calculates the speed variation using `CalcSpeedVariationInBend` and then uses this
    /// variation to compute the final scaling factor. If the lines defined by the start and end points do not
    /// intersect, the function falls back to a straight-line distance calculation.
    static float CalcSpeedScaleFactor(
        const CVector& startCoors,
        const CVector& endCoors,
        CVector2D      startDir,
        CVector2D      endDir
    );

    /// Calculates a point on the curve and the corresponding speed at a specified time.
    /// \param startCoors The starting coordinates of the curve.
    /// \param endCoors The ending coordinates of the curve.
    /// \param startDir The starting direction vector.
    /// \param endDir The ending direction vector.
    /// \param time The time parameter (typically normalized between 0.0 and 1.0) used to interpolate along the curve.
    /// \param traverselTimeInMillis The total traversal time in milliseconds for the curve.
    /// \param resultCoor The resulting interpolated coordinates on the curve.
    /// \param resultSpeed The resulting computed speed vector at the corresponding point on the curve.
    ///
    /// This function computes the position and speed of an object moving along a curve at a specific time.
    /// The curve is defined by its start and end coordinates and direction vectors. The `time` parameter
    /// determines how far along the curve the object has progressed, with 0.0 representing the start and 1.0
    /// representing the end.
    ///
    /// The function uses the `CalcCorrectedDist` function to account for speed variations along the curve,
    /// ensuring that the object's speed is adjusted appropriately based on the curve's geometry. The resulting
    /// position and speed are stored in `resultCoor` and `resultSpeed`, respectively.
    static void CalcCurvePoint(
        const CVector& startCoors,
        const CVector& endCoors,
        const CVector& startDir,
        const CVector& endDir,
        float          time,
        int32          traverselTimeInMillis,
        CVector&       resultCoor,
        CVector&       resultSpeed
    );

    /// Calculates a corrected distance along the curve that accounts for speed variation.
    /// \param current The current progression along the curve.
    /// \param total The total progression or length of the curve.
    /// \param speedVariation The factor reflecting the speed variation along the curve.
    /// \param pInterPol Pointer to a variable that may receive additional interpolation data.
    /// \return The corrected distance along the curve considering the speed variation.
    ///
    /// This function computes a corrected distance along a curve that accounts for variations in speed
    /// caused by the curve's geometry. The `current` parameter represents the object's current position
    /// along the curve, while the `total` parameter represents the total length of the curve.
    ///
    /// The `speedVariation` factor is used to adjust the distance calculation, ensuring that the object's
    /// speed is appropriately scaled based on the sharpness of the curve. The function also provides an
    /// interpolation value (`pInterPol`) that can be used for further calculations or visual effects.
    static float CalcCorrectedDist(float current, float total, float speedVariation, float* pInterPol);
};
