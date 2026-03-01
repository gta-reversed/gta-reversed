#include "StdInc.h"

#include "FxFrustumInfo.h"

// 0x4AA030
bool FxFrustumInfo_c::IsCollision(FxSphere_c* sphere) {
    return plugin::CallMethodAndReturn<bool, 0x4AA030, FxFrustumInfo_c*, FxSphere_c*>(this, sphere);

    // todo: maybe wrong
    if (!m_Sphere.IsCollision(sphere))
        return false;

    for (auto i = 0u; i < std::size(m_Planes); i++) {
        if (sphere->GetDistToPlane(&m_Planes[i]) > sphere->m_fRadius) {
            sphere->m_nNumPlanesPassed = i;
            return false;
        }

        sphere->m_nNumPlanesPassed = i;
    }
    return true;
}
