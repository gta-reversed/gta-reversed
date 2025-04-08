#include "StdInc.h"

#include "Dummy.h"

void CDummy::InjectHooks() {
    RH_ScopedVirtualClass(CDummy, 0x8638C0, 22);
    RH_ScopedCategory("Entity/Dummy");

    RH_ScopedGlobalInstall(IsDummyPointerValid, 0x532730);
}

// 0x5326E0
void* CDummy::operator new(size_t size) {
    return GetDummyPool()->New();
}

// 0x5326F0
void CDummy::operator delete(void* obj) {
    GetDummyPool()->Delete(static_cast<CDummy*>(obj));
}

// 0x532540
CDummy::CDummy() : CEntity() {
    m_nType = ENTITY_TYPE_DUMMY;
}

bool IsDummyPointerValid(CDummy* dummy) {
    return GetDummyPool()->IsObjectValid(dummy);
}
