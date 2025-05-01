#include "StdInc.h"

#include "ActiveOccluder.h"

void CActiveOccluder::InjectHooks() {
    RH_ScopedClass(CActiveOccluder);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(IsPointWithinOcclusionArea, 0x71E580);
    RH_ScopedInstall(IsPointBehindOccluder, 0x71FA40);
}

// 0x71E580
bool CActiveOccluder::IsPointWithinOcclusionArea(CVector2D pt, float radius) const {
    return rng::all_of(GetLines(), [=](auto&& l) {
        return IsPointInsideLine(l.Origin, l.Dir, pt, radius);
    });
}

// 0x71FA40
bool CActiveOccluder::IsPointBehindOccluder(CVector pt, float radius) const {
    const auto& cam = TheCamera.GetPosition();
    for (auto i = 0; i < m_NumFaces; ++i) {
        const auto& normal = m_FaceNormals[i];
        const auto& offset = m_FaceOffsets[i];

        const auto ptToNormal  = pt.Dot(normal) - offset;
        const auto camToNormal = cam.Dot(normal) - offset;

        if (camToNormal * ptToNormal >= 0.0F || std::fabsf(ptToNormal) < radius) {
            return false;
        }
    }
    return true;
}
