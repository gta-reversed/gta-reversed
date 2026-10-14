#include "StdInc.h"

#include <difftests/DiffTest.hpp>

/*
 * Differential tests for CVector / CVector2D / CMatrix - the math almost every other system depends on.
 * Matrices are compared byte-by-byte (including the padding/flags words), starting from IDENTICAL bytes.
 */

using namespace notsa::difftest;

namespace {
CVector RandomVec(Context& c, float range = 100.f) {
    return { c.InterestingFloat(-range, range), c.InterestingFloat(-range, range), c.InterestingFloat(-range, range) };
}

//! Two matrices with byte-identical contents (padding included), from a random orthonormal-ish start
void MakeMatrixPair(Context& c, CMatrix& a, CMatrix& b) {
    CMatrix init{};
    init.SetRotate(c.RandomFloat(-PI, PI), c.RandomFloat(-PI, PI), c.RandomFloat(-PI, PI));
    init.GetPosition() = RandomVec(c, 3000.f);
    std::memcpy(&a, &init, sizeof(CMatrix));
    std::memcpy(&b, &init, sizeof(CMatrix));
}

//! Matrices built from sin/cos/sqrt differ by ~1 ULP between the original (x87, extended precision) and ours (SSE).
//! Compare right/forward/up/pos as floats with a small ULP tolerance (+ a tiny absolute one for values near 0).
constexpr Tolerance s_MatrixTol{ .Abs = 1e-6f, .Ulps = 4 };

bool CompareMatrices(Context& c, std::string_view label, const CMatrix& orig, const CMatrix& ours) {
    const auto* o = reinterpret_cast<const float*>(&orig);
    const auto* u = reinterpret_cast<const float*>(&ours);
    bool ok = true;
    for (const size_t row : { 0u, 4u, 8u }) { // right @0x0, forward @0x10, up @0x20 (each followed by a flags/padding word)
        ok &= CompareFloats(c, std::format("{} row@0x{:X}", label, row * 4), o + row, u + row, 3, s_MatrixTol);
    }
    // pos @0x30: when it's transformed (eg. Rotate) the absolute error grows with its magnitude
    const float posMag = std::max({ std::fabs(o[12]), std::fabs(o[13]), std::fabs(o[14]), 1.f });
    ok &= CompareFloats(c, std::format("{} pos", label), o + 12, u + 12, 3, { .Abs = posMag * 4e-6f, .Ulps = 4 });
    return ok;
}
}; // namespace

NOTSA_DIFFTEST("CVector::Magnitude / Magnitude2D", "Core/CVector/Magnitude", "Core/CVector/Magnitude2D") {
    const auto origMag   = GTAFn<float(__thiscall*)(const CVector*)>(0x4082C0);
    const auto origMag2D = GTAFn<float(__thiscall*)(const CVector*)>(0x406D50);
    for (size_t i = 0; i < 10'000; i++) {
        const CVector v = RandomVec(ctx, 5000.f);
        CompareCall(ctx, "Magnitude", [&](CVector x) { return origMag(&x); }, [](CVector x) { return x.Magnitude(); }, std::tuple{ v }, { .Ulps = 1 });
        CompareCall(ctx, "Magnitude2D", [&](CVector x) { return origMag2D(&x); }, [](CVector x) { return x.Magnitude2D(); }, std::tuple{ v }, { .Ulps = 1 });
    }
}

NOTSA_DIFFTEST("CVector::Normalise / NormaliseAndMag", "Core/CVector/Normalise", "Core/CVector/NormaliseAndMag") {
    const auto origNorm       = GTAFn<void(__thiscall*)(CVector*)>(0x59C910);
    const auto origNormAndMag = GTAFn<float(__thiscall*)(CVector*)>(0x59C970);
    for (size_t i = 0; i < 10'000; i++) {
        const CVector v = i < 8 ? CVector{} : RandomVec(ctx, 1000.f); // include the zero vector (special-cased in the original)

        CVector a = v, b = v;
        origNorm(&a);
        b.Normalise();
        ctx.Check(ApproxEqual(a.x, b.x, { .Ulps = 4 }) && ApproxEqual(a.y, b.y, { .Ulps = 4 }) && ApproxEqual(a.z, b.z, { .Ulps = 4 }),
            std::format("Normalise({}, {}, {}): original = ({}, {}, {}), ours = ({}, {}, {})", v.x, v.y, v.z, a.x, a.y, a.z, b.x, b.y, b.z));

        CVector c = v, d = v;
        const float magOrig = origNormAndMag(&c), magOurs = d.NormaliseAndMag();
        ctx.Check(ApproxEqual(magOrig, magOurs, { .Ulps = 4 }), std::format("NormaliseAndMag({}, {}, {}) returned: original = {}, ours = {}", v.x, v.y, v.z, magOrig, magOurs));
        ctx.Check(ApproxEqual(c.x, d.x, { .Ulps = 4 }) && ApproxEqual(c.y, d.y, { .Ulps = 4 }) && ApproxEqual(c.z, d.z, { .Ulps = 4 }),
            std::format("NormaliseAndMag({}, {}, {}) vector: original = ({}, {}, {}), ours = ({}, {}, {})", v.x, v.y, v.z, c.x, c.y, c.z, d.x, d.y, d.z));
    }
}

NOTSA_DIFFTEST("CVector2D::Magnitude", "Core/CVector2D/Magnitude") {
    const auto original = GTAFn<float(__thiscall*)(const CVector2D*)>(0x420860);
    Fuzz(ctx, "CVector2D::Magnitude", 10'000,
        [](Context& c) { return std::tuple{ CVector2D{ c.InterestingFloat(-5000.f, 5000.f), c.InterestingFloat(-5000.f, 5000.f) } }; },
        [&](CVector2D v) { return original(&v); },
        [](CVector2D v) { return v.Magnitude(); },
        Tolerance{ .Ulps = 1 }
    );
}

NOTSA_DIFFTEST("CMatrix::SetRotateX/Y/Z", "Core/CMatrix/SetRotateX", "Core/CMatrix/SetRotateY", "Core/CMatrix/SetRotateZ") {
    struct { const char* Name; uintptr Addr; void (CMatrix::*Ours)(float); } fns[]{
        { "SetRotateX", 0x59B060, &CMatrix::SetRotateX },
        { "SetRotateY", 0x59B0A0, &CMatrix::SetRotateY },
        { "SetRotateZ", 0x59B0E0, &CMatrix::SetRotateZ },
    };
    for (const auto& fn : fns) {
        const auto original = GTAFn<void(__thiscall*)(CMatrix*, float)>(fn.Addr);
        for (size_t i = 0; i < 2'000; i++) {
            CMatrix a, b;
            MakeMatrixPair(ctx, a, b);
            const float angle = ctx.InterestingFloat(-10.f, 10.f);
            original(&a, angle);
            (b.*fn.Ours)(angle);
            CompareMatrices(ctx, std::format("{}({})", fn.Name, angle), a, b);
        }
    }
}

NOTSA_DIFFTEST("CMatrix::SetRotate(x, y, z)", "Core/CMatrix/SetRotate-xyz") {
    const auto original = GTAFn<void(__thiscall*)(CMatrix*, float, float, float)>(0x59B120);
    for (size_t i = 0; i < 5'000; i++) {
        CMatrix a, b;
        MakeMatrixPair(ctx, a, b);
        const float x = ctx.InterestingFloat(-10.f, 10.f), y = ctx.InterestingFloat(-10.f, 10.f), z = ctx.InterestingFloat(-10.f, 10.f);
        original(&a, x, y, z);
        b.SetRotate(x, y, z);
        CompareMatrices(ctx, std::format("SetRotate({}, {}, {})", x, y, z), a, b);
    }
}

NOTSA_DIFFTEST("CMatrix::SetScale / SetTranslate / SetUnity", "Core/CMatrix/SetScale-f", "Core/CMatrix/SetScale-fff", "Core/CMatrix/SetTranslate", "Core/CMatrix/SetUnity") {
    for (size_t i = 0; i < 2'000; i++) {
        {
            CMatrix a, b;
            MakeMatrixPair(ctx, a, b);
            const float s = ctx.InterestingFloat(-100.f, 100.f);
            GTAFn<void(__thiscall*)(CMatrix*, float)>(0x59AED0)(&a, s);
            b.SetScale(s);
            CompareMatrices(ctx, std::format("SetScale({})", s), a, b);
        }
        {
            CMatrix a, b;
            MakeMatrixPair(ctx, a, b);
            const CVector s = RandomVec(ctx);
            GTAFn<void(__thiscall*)(CMatrix*, float, float, float)>(0x59AF00)(&a, s.x, s.y, s.z);
            b.SetScale(s.x, s.y, s.z);
            CompareMatrices(ctx, std::format("SetScale({}, {}, {})", s.x, s.y, s.z), a, b);
        }
        {
            CMatrix a, b;
            MakeMatrixPair(ctx, a, b);
            const CVector t = RandomVec(ctx, 3000.f);
            GTAFn<void(__thiscall*)(CMatrix*, CVector)>(0x59AF40)(&a, t);
            b.SetTranslate(t);
            CompareMatrices(ctx, "SetTranslate", a, b);
        }
        {
            CMatrix a, b;
            MakeMatrixPair(ctx, a, b);
            GTAFn<void(__thiscall*)(CMatrix*)>(0x59AE70)(&a);
            b.SetUnity();
            CompareMatrices(ctx, "SetUnity", a, b);
        }
    }
}

NOTSA_DIFFTEST("CMatrix::Rotate / Reorthogonalise", "Core/CMatrix/Rotate", "Core/CMatrix/Reorthogonalise") {
    for (size_t i = 0; i < 3'000; i++) {
        {
            CMatrix a, b;
            MakeMatrixPair(ctx, a, b);
            const CVector r{ ctx.RandomFloat(-PI, PI), ctx.RandomFloat(-PI, PI), ctx.RandomFloat(-PI, PI) };
            GTAFn<void(__thiscall*)(CMatrix*, CVector)>(0x59B460)(&a, r);
            b.Rotate(r);
            CompareMatrices(ctx, std::format("Rotate({}, {}, {})", r.x, r.y, r.z), a, b);
        }
        {
            CMatrix a, b;
            MakeMatrixPair(ctx, a, b);
            // De-orthogonalise a bit so the function has work to do
            const CVector skew = RandomVec(ctx, 0.2f);
            a.GetForward() += skew;
            b.GetForward() += skew;
            GTAFn<void(__thiscall*)(CMatrix*)>(0x59B6A0)(&a);
            b.Reorthogonalise();
            CompareMatrices(ctx, "Reorthogonalise", a, b);
        }
    }
}

NOTSA_DIFFTEST("CMatrix::ConvertToEulerAngles", "Core/CMatrix/ConvertToEulerAngles") {
    const auto original = GTAFn<void(__thiscall*)(CMatrix*, float*, float*, float*, uint32)>(0x59A840);
    for (size_t i = 0; i < 5'000; i++) {
        CMatrix a, b;
        MakeMatrixPair(ctx, a, b);
        // One of the 6 valid orders (ORDER_XYZ..ORDER_ZYX = 0x00..0x14, step 4) + the low flag bits (0x1 / EULER_ANGLES 0x2)
        const uint32 flags = (uint32)(ctx.RandomInt(0, 5) * 4) | (uint32)ctx.RandomInt(0, 3);
        float ox{}, oy{}, oz{}, ux{}, uy{}, uz{};
        original(&a, &ox, &oy, &oz, flags);
        b.ConvertToEulerAngles(&ux, &uy, &uz, flags);
        ctx.Check(ApproxEqual(ox, ux, { .Ulps = 4 }) && ApproxEqual(oy, uy, { .Ulps = 4 }) && ApproxEqual(oz, uz, { .Ulps = 4 }),
            std::format("ConvertToEulerAngles(flags {:#x}): original = ({}, {}, {}), ours = ({}, {}, {})", flags, ox, oy, oz, ux, uy, uz));
    }
}
