#include "StdInc.h"

#include <difftests/DiffTest.hpp>
#include "General.h"

/*
 * Differential tests for `CGeneral` - pure math functions, ideal for fuzzing.
 * These double as examples of the 3 common patterns:
 *  - plain return value fuzzing                 (LimitAngle, LimitRadianAngle, GetATanOfXY, ...)
 *  - hand-picked edge cases + fuzzing           (GetNodeHeadingFromVector)
 *  - out-parameters compared manually           (SolveQuadratic)
 */

using namespace notsa::difftest;

NOTSA_DIFFTEST("CGeneral::LimitAngle", "Global/CGeneral/LimitAngle") {
    Fuzz(ctx, "LimitAngle", 5'000,
        [](Context& c) { return std::tuple{ c.RandomFloat(-2000.f, 2000.f) }; },
        GTAFn<float(__cdecl*)(float)>(0x53CB00),
        &CGeneral::LimitAngle
    );
    for (const auto angle : { -180.f, 180.f, 0.f, -0.f, 359.99f, -540.f, 540.f }) {
        CompareCall(ctx, "LimitAngle", GTAFn<float(__cdecl*)(float)>(0x53CB00), &CGeneral::LimitAngle, std::tuple{ angle });
    }
}

NOTSA_DIFFTEST("CGeneral::LimitRadianAngle", "Global/CGeneral/LimitRadianAngle") {
    Fuzz(ctx, "LimitRadianAngle", 10'000,
        [](Context& c) { return std::tuple{ c.InterestingFloat(-50.f, 50.f) }; },
        GTAFn<float(__cdecl*)(float)>(0x53CB50),
        &CGeneral::LimitRadianAngle
    );
}

NOTSA_DIFFTEST("CGeneral::GetATanOfXY", "Global/CGeneral/GetATanOfXY") {
    Fuzz(ctx, "GetATanOfXY", 10'000,
        [](Context& c) { return std::tuple{ c.InterestingFloat(), c.InterestingFloat() }; },
        GTAFn<float(__cdecl*)(float, float)>(0x53CC70),
        &CGeneral::GetATanOfXY,
        Tolerance{ .Ulps = 4 } // x87 (original) vs SSE (ours) `atan` may differ in the last bits
    );
}

NOTSA_DIFFTEST("CGeneral::GetRadianAngleBetweenPoints", "Global/CGeneral/GetRadianAngleBetweenPoints-") {
    Fuzz(ctx, "GetRadianAngleBetweenPoints", 10'000,
        [](Context& c) { return std::tuple{ c.RandomFloat(-3000.f, 3000.f), c.RandomFloat(-3000.f, 3000.f), c.RandomFloat(-3000.f, 3000.f), c.RandomFloat(-3000.f, 3000.f) }; },
        GTAFn<float(__cdecl*)(float, float, float, float)>(0x53CBE0),
        static_cast<float (*)(float, float, float, float)>(&CGeneral::GetRadianAngleBetweenPoints),
        Tolerance{ .Ulps = 4 }
    );
}

NOTSA_DIFFTEST("CGeneral::GetAngleBetweenPoints", "Global/CGeneral/GetAngleBetweenPoints") {
    Fuzz(ctx, "GetAngleBetweenPoints", 10'000,
        [](Context& c) { return std::tuple{ c.RandomFloat(-3000.f, 3000.f), c.RandomFloat(-3000.f, 3000.f), c.RandomFloat(-3000.f, 3000.f), c.RandomFloat(-3000.f, 3000.f) }; },
        GTAFn<float(__cdecl*)(float, float, float, float)>(0x53CEA0),
        &CGeneral::GetAngleBetweenPoints,
        Tolerance{ .Ulps = 8 }
    );
}

NOTSA_DIFFTEST("CGeneral::GetNodeHeadingFromVector", "Global/CGeneral/GetNodeHeadingFromVector") {
    const auto original = GTAFn<uint32(__cdecl*)(float, float)>(0x53CDC0);

    // Edge cases: axes, diagonals (sector boundaries), zero vector
    for (const auto x : { -1.f, -0.f, 0.f, 1.f }) {
        for (const auto y : { -1.f, -0.f, 0.f, 1.f }) {
            CompareCall(ctx, "GetNodeHeadingFromVector", original, &CGeneral::GetNodeHeadingFromVector, std::tuple{ x, y });
        }
    }
    Fuzz(ctx, "GetNodeHeadingFromVector", 10'000,
        [](Context& c) { return std::tuple{ c.InterestingFloat(-10.f, 10.f), c.InterestingFloat(-10.f, 10.f) }; },
        original,
        &CGeneral::GetNodeHeadingFromVector
    );
}

NOTSA_DIFFTEST("CGeneral::SolveQuadratic", "Global/CGeneral/SolveQuadratic") {
    const auto original = GTAFn<bool(__cdecl*)(float, float, float, float&, float&)>(0x53CE30);
    for (size_t i = 0; i < 10'000; i++) {
        const auto a = ctx.InterestingFloat(-100.f, 100.f), b = ctx.InterestingFloat(-100.f, 100.f), c = ctx.InterestingFloat(-100.f, 100.f);

        float origX1{ -1.f }, origX2{ -1.f }, ourX1{ -1.f }, ourX2{ -1.f }; // Same initial value, so untouched out-params compare equal
        const bool origRet = original(a, b, c, origX1, origX2);
        const bool ourRet  = CGeneral::SolveQuadratic(a, b, c, ourX1, ourX2);

        // The original keeps `b*b - 4*a*c` and the rest in 80-bit x87 registers; we compute in float (SSE). When the
        // discriminant suffers from cancellation the results legitimately differ in absolute terms, so on top of a few
        // ULPs allow an absolute error relative to the inputs' magnitude.
        const float scale = std::max({ std::fabs(b / a), std::sqrt(std::fabs(c / a)), 1.f });
        const Tolerance tol{ .Abs = scale * 1e-5f, .Ulps = 8 };
        const auto args = std::format("SolveQuadratic({}, {}, {})", a, b, c);
        ctx.Check(origRet == ourRet, std::format("{}: return value: original = {}, ours = {}", args, origRet, ourRet));
        if (origRet && ourRet) {
            ctx.Check(ApproxEqual(origX1, ourX1, tol), std::format("{}: x1: original = {}, ours = {}", args, origX1, ourX1));
            ctx.Check(ApproxEqual(origX2, ourX2, tol), std::format("{}: x2: original = {}, ours = {}", args, origX2, ourX2));
        }
    }
}
