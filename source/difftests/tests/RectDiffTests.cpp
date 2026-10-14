#include "StdInc.h"

#include <difftests/DiffTest.hpp>
#include "Rect.h"

/*
 * Differential tests for `CRect` - examples of testing MEMBER functions:
 *  - mutating methods: run the original on one copy and ours on another, then compare the objects byte-by-byte
 *  - const methods: compare the return value
 */

using namespace notsa::difftest;

namespace {
//! A valid (not flipped) rect - the CRect constructor asserts `!IsFlipped()`
CRect RandomRect(Context& c) {
    const auto x1 = c.RandomFloat(-100.f, 100.f), x2 = c.RandomFloat(-100.f, 100.f);
    const auto y1 = c.RandomFloat(-100.f, 100.f), y2 = c.RandomFloat(-100.f, 100.f);
    return { std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2) };
}
}; // namespace

NOTSA_DIFFTEST("CRect::Restrict", "Core/CRect/Restrict") {
    const auto original = GTAFn<void(__thiscall*)(CRect*, const CRect*)>(0x404200);
    for (size_t i = 0; i < 5'000; i++) {
        const CRect initial = RandomRect(ctx), restriction = RandomRect(ctx);

        CRect orig = initial, ours = initial;
        original(&orig, &restriction);
        ours.Restrict(restriction);
        CompareObjects(ctx, "Restrict", orig, ours);
    }
}

NOTSA_DIFFTEST("CRect::StretchToPoint", "Core/CRect/StretchToPoint") {
    const auto original = GTAFn<void(__thiscall*)(CRect*, float, float)>(0x5327F0);
    for (size_t i = 0; i < 5'000; i++) {
        const CRect initial = RandomRect(ctx);
        const auto  x = ctx.RandomFloat(-150.f, 150.f), y = ctx.RandomFloat(-150.f, 150.f);

        CRect orig = initial, ours = initial;
        original(&orig, x, y);
        ours.StretchToPoint(x, y);
        CompareObjects(ctx, "StretchToPoint", orig, ours);
    }
}

NOTSA_DIFFTEST("CRect::Resize", "Core/CRect/Resize") {
    const auto original = GTAFn<void(__thiscall*)(CRect*, float, float)>(0x404260);
    for (size_t i = 0; i < 5'000; i++) {
        const CRect initial = RandomRect(ctx);
        const auto  rx = ctx.RandomFloat(-50.f, 50.f), ry = ctx.RandomFloat(-50.f, 50.f);

        CRect orig = initial, ours = initial;
        original(&orig, rx, ry);
        ours.Resize(rx, ry);
        CompareObjects(ctx, "Resize", orig, ours);
    }
}

NOTSA_DIFFTEST("CRect::IsPointInside", "Core/CRect/IsPointInside-") {
    const auto original = GTAFn<bool(__thiscall*)(const CRect*, const CVector2D*)>(0x404290);
    for (size_t i = 0; i < 10'000; i++) {
        const CRect     rect = RandomRect(ctx);
        // Half of the points exactly on an edge - boundary comparisons (`<` vs `<=`) only show up there
        const CVector2D point = ctx.RandomBool()
            ? CVector2D{ ctx.RandomFloat(-150.f, 150.f), ctx.RandomFloat(-150.f, 150.f) }
            : CVector2D{ ctx.RandomBool() ? rect.left : rect.right, ctx.RandomBool() ? rect.bottom : rect.top };

        CompareCall(ctx, "IsPointInside",
            [&](const CVector2D& p) { return original(&rect, &p); },
            [&](const CVector2D& p) { return rect.IsPointInside(p); },
            std::tuple{ point }
        );
    }
}
