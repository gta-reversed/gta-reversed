/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <cstdlib> // RAND_MAX

//! If you see this value USE THE INTEGER VERSION of `GetRandomNumberInRange`
//! It's very important to do that, because the int version will is exclusive
//constexpr float RAND_MAX_INT_RECIPROCAL = 1.0f / static_cast<float>(RAND_MAX + 1); // 1.0 / 32768.0 == 1.0 / (RAND_MAX + 1) = 0.0000305175781

//! If you see this value, USE THE FLOAT VERSION of `GetRandomNumberInRange`
constexpr float RAND_MAX_FLOAT_RECIPROCAL = 1.0f / static_cast<float>(RAND_MAX);   // 1.0 / 32767.0 == 1.0 / RAND_MAX       = 0.0000305185094

//#define BETTER_RNG

namespace CGeneral { // More like `Math` (Or `Meth`, given how bad the code is, these guys must've been high!)
    static thread_local std::random_device randomDevice{};
    static thread_local std::mt19937 randomEngine(randomDevice());

    void InjectHooks();

    /*!
     * @addr 0x53CB00
     * @brief Normalize the `angle` to be between [-180, 180] deg
     */
    float LimitAngle(float angle);

    /*!
     * @addr 0x53CB50
     * @returns The `angle` normalized to be between [-pi, pi]
     */
    float LimitRadianAngle(float angle);

    /*!
     * @addr 0x53CC70
     * @returns atan2(??, ??) - TODO
     */
    float GetATanOfXY(float x, float y);

    /*!
     * @notsa
     * @copydoc GetATanOfXY
     */
    inline float GetATanOf(CVector2D point) { return GetATanOfXY(point.x, point.y); } // This just returns the heading most likely, so `point.Heading()` should work too [?]

    /*!
     * @addr 0x53CDC0
     * @returns Get the octant the vector's heading lies in
     */
    uint32 GetNodeHeadingFromVector(float x, float y);

    /*!
     * @addr 0x53CE30
     * @brief Try solving a quadratic equation
     * @returns If the equation had a solution
     */
    bool SolveQuadratic(float a, float b, float c, float& x1, float& x2);

    /*!
     * @addr 0x53CBE0
     * @returns Angle [in radians] between 2 points (Prefer using `(p1 - p2).Heading()` instead!)
     */
    float GetRadianAngleBetweenPoints(float x1, float y1, float x2, float y2);

    /*!
     * @addr 0x53CEA0
     * @returns Angle [in degrees] between 2 points (Prefer using `(p1 - p2).Heading()` instead [NOTE: The latter returns radians, not degrees])
     */
    float GetAngleBetweenPoints(float x1, float y1, float x2, float y2);

    /*!
     * @returns Angle [in degrees] between 2 points (Prefer using `(p1 - p2).Heading()` instead [NOTE: The latter returns radians, not degrees])
     */
    float GetRadianAngleBetweenPoints(CVector2D a, CVector2D b);

    /*!
     * @returns A random number in range [0, RAND_MAX] (Same as `rand()`)
     */
    uint16 GetRandomNumber();

    /*!
     * @returns True `chanceOfTrue` % of the time, false othertimes. Valid value are [0, 100)
     */
    bool RandomBool(float chanceOfTrue);

    /*!
     * @returns True 50% of the time
     */
    bool DoCoinFlip();

    /**
     * @addr 0x41BD90
     * @param min Minimum value
     * @param max Maximum value. Must be greater than min.
     * @return A pseudo-random number between min and max, inclusive [min, max].
     */
    template<typename T>
        requires std::is_floating_point_v<T>
    inline T GetRandomNumberInRange(T min, T max) {
#ifdef BETTER_RNG
        return std::uniform_real_distribution<float>{min, max}(randomEngine);
#else
        assert(max >= min); // Check is not empty range (We must use `>=` because the `int` version relies on it)
        return lerp<T>(min, max, static_cast<float>(GetRandomNumber()) * RAND_MAX_FLOAT_RECIPROCAL);
#endif
    }

    /**
     * @addr 0x407180
     * @param min Minimum value
     * @param max Maximum value. Must be greater than min.
     * @return A pseudo-random number between min and max, exclusive [min, max).
     */
    template<std::integral T>
    inline T GetRandomNumberInRange(T min, T max) {
        return static_cast<T>(GetRandomNumberInRange<float>(static_cast<float>(min), static_cast<float>(max) - 1.f)); // substraction in floating point because integer might underflow (if unsigned)
    }

    /**
     * @notsa
     * @param max Maximum value. Must be greater than 0.
     * @return A pseudo-random number between min and max, exclusive [0, max).
     */
    template<std::integral T>
        requires std::is_unsigned_v<T> // Signed types don't make sense here
    inline T GetRandomNumberInRange(T max) {
        return GetRandomNumberInRange<T>(0, max);
    }

    /*!
    * @notsa
    * @return An item from the range choosen randomly. Same as Python's `random.choice`
    */
    template<rng::random_access_range R>
    inline auto& RandomChoice(R&& range) { // TODO: Add warning or smth for a possible dangling reference here
        // If range is empty `GetRandomNumberInRange` will assert
        return range[CGeneral::GetRandomNumberInRange(rng::size(range))];
    }

    template<typename T>
    static T RandomChoiceFromList(std::initializer_list<T> list) {
        return RandomChoice(rng::subrange{list.begin(), list.end()});
    }

    /*
    * @notsa
    * @brief Return a random node heading, or direction as commonly referred to. See `GetNodeHeadingFromVector`
    */
    inline auto RandomNodeHeading() {
        return (uint8)CGeneral::GetRandomNumberInRange(0, 8);
    }
};
