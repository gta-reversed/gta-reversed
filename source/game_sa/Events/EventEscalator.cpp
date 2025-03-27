#include "StdInc.h"
#include "EventEscalator.h"


void CEventEscalator::InjectHooks() {
    RH_ScopedVirtualClass(CEventEscalator, 0x86CD00, 16);
    RH_ScopedCategory("Events");

    RH_ScopedVMTInstall(AffectsPed, 0x4B2580);
}

// 0x4B2580
bool CEventEscalator::AffectsPed(CPed* ped) {
    if (ped->IsAlive() && !ped->IsPlayer() && ped->m_pContactEntity) {
        auto modelId = ped->m_pContactEntity->m_nModelIndex;
        if (modelId == ModelIndices::MI_ESCALATORSTEP || modelId == ModelIndices::MI_ESCALATORSTEP8)
            return true;
    }
    return false;
}