#include "StdInc.h"

#include "FxSphere.h"
#include "FxPlane.h"

void FxSphere_c::InjectHooks() {
    RH_ScopedClass(FxSphere_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(operator new, 0x4A9F80);
    RH_ScopedOverloadedInstall(operator delete, "", 0x4A9FB0, void(*)(void*));
    RH_ScopedInstall(IsCollision, 0x4A9FC0);
    RH_ScopedInstall(GetDistToPlane, 0x4AA010);
}

// 0x4A9F70
FxSphere_c::FxSphere_c() {
    m_nNumPlanesPassed = 0;
}

// 0x4A9F80
void* FxSphere_c::operator new(uint32 size, bool32 bUseGlobalHeep) {
    if (bUseGlobalHeep)
        return CMemoryMgr::Malloc(size);
    else
        return g_fxMan.GetMemPool().GetMem(size);
}

// 0x4A9FB0
void FxSphere_c::operator delete(void* data) {
    CMemoryMgr::Free(data);
}

void FxSphere_c::operator delete(void* data, bool32 bUseGlobalHeep) {
    if (bUseGlobalHeep) {
        CMemoryMgr::Free(data);
    }
}

// 0x4A9FC0
bool FxSphere_c::IsCollision(FxSphere_c* sphere) const {
    assert(sphere);
    return std::powf(m_fRadius + sphere->m_fRadius, 2) > (sphere->m_vecCenter - m_vecCenter).SquaredMagnitude();
}

// 0x4AA010
float FxSphere_c::GetDistToPlane(FxPlane_c* plane) const {
    assert(plane);
    return DotProduct(plane->normal, m_vecCenter) - plane->distance;
}
