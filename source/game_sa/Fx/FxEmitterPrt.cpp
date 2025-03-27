#include "StdInc.h"

#include "FxEmitterPrt.h"
#include "FxManager.h"

// 0x4A18B0
void* FxEmitterPrt_c::operator new[](size_t size) {
    return g_fxMan.GetMemPool().GetMem(size);
}

void FxEmitterPrt_c::InjectHooks() {
    RH_ScopedClass(FxEmitterPrt_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(operator new[], 0x4A18B0);
}
