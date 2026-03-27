/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include <random>

static std::random_device randomDevice;
static std::mt19937 randomEngine(randomDevice());

void CGeneral::InjectHooks() {
    RH_ScopedNamespace(CGeneral);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(LimitAngle, 0x53CB00);
    RH_ScopedInstall(LimitRadianAngle, 0x53CB50);
    RH_ScopedOverloadedInstall(GetRadianAngleBetweenPoints, "", 0x53CBE0, float(*)(float, float, float, float));
    RH_ScopedInstall(GetATanOfXY, 0x53CC70);
    RH_ScopedInstall(GetNodeHeadingFromVector, 0x53CDC0);
    RH_ScopedInstall(SolveQuadratic, 0x53CE30);
    RH_ScopedInstall(GetAngleBetweenPoints, 0x53CEA0);
    //RH_ScopedOverloadedInstall(GetRandomNumberInRange<int32>, "", 0x407180, int32(*)(int32, int32)); // Can't hook these because we've added an extra argument to `GetRandomNumberInRange`
    RH_ScopedOverloadedInstall(GetRandomNumberInRange<float>, "", 0x41BD90, float (*)(float, float), { .enabled = false, .locked = true }); // There's a bug in the code at 0x6DF26D which causes the assert to be triggered, so I'm unhooking this for now
}

// 0x53CB00
float CGeneral::LimitAngle(float angle) {
    while (angle >= 180.0f) {
        angle -= 360.0f;
    }

    while (angle < -180.0f) {
        angle += 360.0f;
    }

    return angle;
}

// 0x53CB50
float CGeneral::LimitRadianAngle(float angle) {
    float result = std::clamp(angle, -25.0f, 25.0f);

    while (result >= PI) {
        result -= 2 * PI;
    }

    while (result < -PI) {
        result += 2 * PI;
    }

    return result;
}

// 0x53CBE0
float CGeneral::GetRadianAngleBetweenPoints(float x1, float y1, float x2, float y2) {
    float x = x2 - x1;
    float y = y2 - y1;

    if (y == 0.0f)
        y = 0.0001f;

    if (x > 0.0f) {
        if (y > 0.0f)
            return PI - atan2(x / y, 1.0f);
        else
            return -atan2(x / y, 1.0f);
    } else {
        if (y > 0.0f)
            return -(PI + atan2(x / y, 1.0f));
        else
            return -atan2(x / y, 1.0f);
    }
}

// 0x53CC70
float CGeneral::GetATanOfXY(float x, float y) {
    if (x == 0.0f && y == 0.0f)
        return 0.0f;

    // Wikipedia explains this function in great detail: https://en.wikipedia.org/wiki/Atan2

    float xabs = abs(x);
    float yabs = abs(y);

    if (xabs < yabs) {
        if (y > 0.0f) {
            if (x > 0.0f)
                return 0.5f * PI - atan2(x / y, 1.0f);
            else
                return 0.5f * PI + atan2(-x / y, 1.0f);
        } else {
            if (x > 0.0f)
                return 1.5f * PI + atan2(x / -y, 1.0f);
            else
                return 1.5f * PI - atan2(-x / -y, 1.0f);
        }
    } else {
        if (y > 0.0f) {
            if (x > 0.0f)
                return atan2(y / x, 1.0f);
            else
                return PI - atan2(y / -x, 1.0f);
        } else {
            if (x > 0.0f)
                return 2.0f * PI - atan2(-y / x, 1.0f);
            else
                return PI + atan2(-y / -x, 1.0f);
        }
    }
}

// 0x53CDC0
uint32 CGeneral::GetNodeHeadingFromVector(float x, float y) {
    float angle = GetRadianAngleBetweenPoints(x, y, 0.0f, 0.0f);
    if (angle < 0.0f)
        angle += TWO_PI;

    angle = TWO_PI - angle + DegreesToRadians(22.5f);

    if (angle >= TWO_PI)
        angle -= TWO_PI;

    return (uint32)floor(angle / DegreesToRadians(45.0f));
}

/*!
* Solves: ax² + bx + c = 0
* @returns If there is a solution for the equation
* @addr 0x53CE30
*/
bool CGeneral::SolveQuadratic(float a, float b, float c, float& x1, float& x2) {
    // x12 = (-b ± √(b²-4ac)) / 2a

    const auto discr = b * b - 4.f * a * c;
    if (discr < 0.0f) {
        return false; // No solution
    }


    const auto discriminantSqrt = std::sqrt(discr);
    const auto denom = 1.f / (2.f * a);
    x1 = (-b - discriminantSqrt) * denom;
    x2 = (-b + discriminantSqrt) * denom;
    return true;
}

// 0x53CEA0
float CGeneral::GetAngleBetweenPoints(float x1, float y1, float x2, float y2) {
    return RadiansToDegrees(GetRadianAngleBetweenPoints(x1, y1, x2, y2));
}

uint16 CGeneral::GetRandomNumber() { // Why does this return `uint16`? Should be `int32` (same as retval of `rand()`)
    static_assert(RAND_MAX == 0x7FFF, "PC-generated random numbers should not exceed 32767");
#ifdef BETTER_RNG
    static_assert(false, "PC-generated random numbers should not exceed 32767");
#else
    return rand();
#endif
}

float CGeneral::GetRadianAngleBetweenPoints(CVector2D a, CVector2D b) {
    return GetRadianAngleBetweenPoints(a.x, a.y, b.x, b.y);
}

bool CGeneral::RandomBool(float chanceOfTrue) {
    assert(chanceOfTrue <= 100.f);
    return CGeneral::GetRandomNumberInRange(0.f, 100.f) <= chanceOfTrue;
}

/*!
* @return true/false with 50/50 change
* @addr notsa
*/
bool CGeneral::DoCoinFlip() {
	return CGeneral::GetRandomNumber() >= RAND_MAX / 2;
}
