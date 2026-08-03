#include "StdInc.h"

#include "Sphere.h"
#include "Lines.h"

// 0x40FCF0
void CSphere::Set(float radius, const CVector& center) {
    m_fRadius = radius;
    m_vecCenter = center;
}

// NOTSA
bool CSphere::IsPointWithin(const CVector& p) const {
    return (p - m_vecCenter).SquaredMagnitude() <= sq(m_fRadius);
}

// NOTSA
void CSphere::DrawWireFrame(CRGBA color, const CMatrix& transform, size_t resolution) const {
    assert(resolution > 3 && "Resolution must be greater than 3 to draw a sphere wireframe!");

    RenderBuffer::ClearRenderBuffer();

    // Generate vertices
    for (size_t x = 0; x < resolution; x++) {
        const auto ax = TWO_PI / (float)(resolution - 1) * x;
        const auto sx = std::sin(ax),
                   cx = std::cos(ax);
        for (size_t y = 0; y < resolution; y++) {
            const auto ay = PI / (float)(resolution - 1) * y;
            const auto sy = std::sin(ay),
                       cy = std::cos(ay);
            RenderBuffer::PushVertex(transform.TransformPoint(m_vecCenter + CVector{
                m_fRadius * cx * sy,
                m_fRadius * sx * sy,
                m_fRadius * cy
            }), color);
        }
    }

    // Generate indices
    const auto res = (int32)(resolution);
    for (int32 i = 0; i < res - 1; i++) {
        for (int32 j = 0; j < res - 1; j++) {
            const auto off = i * res + j;

            // Horizontal line
            RenderBuffer::PushIndices({
                off,
                off + 1
            }, false);

            // Vertical line
            RenderBuffer::PushIndices({
                off,
                off + res
            }, false);
        }
    }

    // Render the lines
    RenderBuffer::Render(rwPRIMTYPELINELIST);
}

// notsa
auto CSphere::GetTransformed(const CMatrix& transform) const -> CSphere {
    return { transform.TransformPoint(m_vecCenter), m_fRadius };
}

// notsa
auto CSphere::GetBoundingBox() const -> CBox {
    return {
        m_vecCenter - CVector{ m_fRadius },
        m_vecCenter + CVector{ m_fRadius }
    };
}

// notsa
auto TransformObject(const CSphere& sp, const CMatrix& transform) -> CSphere {
    return { transform.TransformPoint(sp.m_vecCenter), sp.m_fRadius };
}
