#include "StdInc.h"
#include "NodeRoute.h"

void CNodeRoute::InjectHooks() {
    RH_ScopedClass(CNodeRoute);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(operator new, 0x41B860);
    RH_ScopedInstall(operator delete, 0x41B870);
}

void* CNodeRoute::operator new(uint32 size) {
    return CPools::ms_pNodeRoutePool->New();
}

void CNodeRoute::operator delete(void* ptr, size_t sz) {
    CPools::ms_pNodeRoutePool->Delete(reinterpret_cast<CNodeRoute*>(ptr));
}
