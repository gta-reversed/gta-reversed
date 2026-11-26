#include "StdInc.h"
#include "PedSaveStructure.h"

void CPedSaveStructure::InjectHooks() {
    RH_ScopedClass(CPedSaveStructure);
    RH_ScopedCategory("Entity/Ped");

    RH_ScopedInstall(Construct, 0x5D43D0, { .Reversed = false });
    RH_ScopedInstall(Extract, 0x5D44B0, { .Reversed = false });
}

void CPedSaveStructure::Extract(CPed* ped) {
    plugin::CallMethod<0x5D44B0, CPedSaveStructure*, CPed*>(this, ped);
}

void CPedSaveStructure::Construct(CPed* ped) {
    plugin::CallMethod<0x5D43D0, CPedSaveStructure*, CPed*>(this, ped);
}
