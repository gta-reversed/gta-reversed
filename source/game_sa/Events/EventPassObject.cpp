#include "StdInc.h"
#include "EventPassObject.h"


void CEventPassObject::InjectHooks() {
    RH_ScopedVirtualClass(CEventPassObject, 0x86F8B8, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x65DC70);
    RH_ScopedVMTInstall(IsValid, 0x4B1700);
}

// 0x65DC70
CEventPassObject* CEventPassObject::Constructor(CPed* giver, bool dontPassObject) { this->CEventPassObject::CEventPassObject(giver, dontPassObject); return this; }

// 0x65DC70
CEventPassObject::CEventPassObject(CPed* giver, bool dontPassObject) :
    m_giver{giver},
    m_dontPassObject{dontPassObject}
{
    CEntity::SafeRegisterRef(m_giver);
}

CEventPassObject::~CEventPassObject() {
    CEntity::SafeCleanUpRef(m_giver);
}

// 0x4B1700
bool CEventPassObject::IsValid(CPed* ped) {
    if (ped)
        return ped->IsAlive();

    if (CEvent::IsValid(ped))
        return true;

    return false;
}