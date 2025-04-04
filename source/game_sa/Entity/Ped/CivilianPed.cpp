#include "StdInc.h"
#include "CivilianPed.h"

void CCivilianPed::InjectHooks() {
    RH_ScopedVirtualClass(CCivilianPed, 0x86C0A8, 26);
    RH_ScopedCategory("Entity/CivilianPed");

    RH_ScopedInstall(Constructor, 0x5DDB70);
    RH_ScopedInstall(Destructor, 0x5DDC30);
    RH_ScopedVMTInstall(ProcessControl, 0x5DDBF0);
}


// 0x5DDB70
CCivilianPed::CCivilianPed(ePedType pedType, uint32 modelIndex) : CPed(pedType) {
    SetModelIndex(modelIndex); // V1053 Calling the 'SetModelIndex' virtual function in the constructor may lead to unexpected result at runtime
    m_pedSpeech.Initialise(this);
}

// 0x5DDBF0
void CCivilianPed::ProcessControl() {
    if (m_nCreatedBy != (PED_MISSION | PED_GAME)) {
        CPed::ProcessControl();
        if (!m_bWasPostponed && m_nPedState != PEDSTATE_DEAD)
            GetActiveWeapon().Update(this);
    }
}

CCivilianPed* CCivilianPed::Constructor(ePedType pedType, uint32 modelIndex) {
    this->CCivilianPed::CCivilianPed(pedType, modelIndex);
    return this;
}

CCivilianPed* CCivilianPed::Destructor() {
    this->CCivilianPed::~CCivilianPed();
    return this;
}
