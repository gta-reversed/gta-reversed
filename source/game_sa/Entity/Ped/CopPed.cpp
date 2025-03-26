#include "StdInc.h"

#include "CopPed.h"

void CCopPed::InjectHooks() {
    RH_ScopedVirtualClass(CCopPed, 0x86C120, 26);
    RH_ScopedCategory("Entity/Ped");

    RH_ScopedInstall(Constructor, 0x5DDC60);
    RH_ScopedInstall(Destructor, 0x5DE0D0);
    RH_ScopedInstall(SetPartner, 0x5DDE80);
    RH_ScopedInstall(AddCriminalToKill, 0x5DDEB0);
    RH_ScopedInstall(RemoveCriminalToKill, 0x5DE040);
    RH_ScopedInstall(ClearCriminalsToKill, 0x5DE070);
    RH_ScopedVMTInstall(ProcessControl, 0x5DE160);
}

// NOTSA
eModelID CCopPed::GetPedModelForCopType(eCopType ctype) {
    switch (ctype) {
    case COP_TYPE_CITYCOP:
        return CStreaming::GetDefaultCopModel();
    case COP_TYPE_LAPDM1:
        return MODEL_LAPDM1;
    case COP_TYPE_CSHER:
        return MODEL_CSHER;
    case COP_TYPE_ARMY:
        return MODEL_ARMY;
    case COP_TYPE_FBI:
        return MODEL_FBI;
    case COP_TYPE_SWAT1:
    case COP_TYPE_SWAT2:
        return MODEL_SWAT;
    default:
        NOTSA_UNREACHABLE();
    }
}

/* Horrible design, but R* also allowed to pass in a ModelID */
eModelID ResolveModelForCopType(uint32_t typeOrModelID) {
    switch (typeOrModelID) {
    case COP_TYPE_CITYCOP:
    case COP_TYPE_LAPDM1:
    case COP_TYPE_CSHER:
    case COP_TYPE_ARMY:
    case COP_TYPE_FBI:
    case COP_TYPE_SWAT1:
    case COP_TYPE_SWAT2:
        return CCopPed::GetPedModelForCopType((eCopType)(typeOrModelID));
    default:
        return (eModelID)(typeOrModelID); /* A modelID was passed in */
    }
}

// 0x5DDC60
CCopPed::CCopPed(uint32_t copTypeOrModelID) :
    CPed{ PED_TYPE_COP },
    m_nCopTypeOrModelID{ copTypeOrModelID }
{
    // V1053 Calling the 'SetModelIndex' virtual function in the constructor may lead to unexpected result at runtime
    SetModelIndex(ResolveModelForCopType(copTypeOrModelID)); /* R* originally seem to have used this switch to set the model as well, but this is nicer */

    switch (copTypeOrModelID) {
    /* Done in ResolveModelForCopType
    case COP_TYPE_CITYCOP:
        ...;
    case COP_TYPE_LAPDM1:
        ...;
    */
    case COP_TYPE_SWAT1:
    case COP_TYPE_SWAT2: {
        GiveDelayedWeapon(eWeaponType::WEAPON_MICRO_UZI, 1000);
        SetCurrentWeapon(eWeaponType::WEAPON_MICRO_UZI);
        SetArmour(50.0f);
        SetWeaponShootingRange(70);
        SetWeaponAccuracy(68);
        break;
    }
    case COP_TYPE_FBI: {
        GiveDelayedWeapon(eWeaponType::WEAPON_MP5, 1000);
        SetCurrentWeapon(eWeaponType::WEAPON_MP5);
        SetArmour(100.0f);
        SetWeaponShootingRange(60);
        SetWeaponAccuracy(76);
        break;
    }
    case COP_TYPE_ARMY: {
        GiveDelayedWeapon(eWeaponType::WEAPON_M4, 1000);
        SetCurrentWeapon(eWeaponType::WEAPON_M4);
        SetArmour(100.0f);
        SetWeaponShootingRange(80);
        SetWeaponAccuracy(84);
        break;
    }
    default: {
        GiveWeapon(eWeaponType::WEAPON_NIGHTSTICK, 1000, true);
        GiveDelayedWeapon(eWeaponType::WEAPON_PISTOL, 1000);
        m_nActiveWeaponSlot = 0;
        SetArmour(0.0f);
        SetWeaponShootingRange(30);
        SetWeaponAccuracy(60);
        break;
    }
    }
    m_bDontPursuit = false;
    field_74C = 0;
    field_79D = 0;
    field_7A4 = 0;

    CEntity::ClearReference(m_pTargetedObject); // Oookay?

    m_pIntelligence->SetDmRadius(60.0f);
    m_pIntelligence->SetNumPedsToScan(8);
    m_pedSpeech.Initialise(this);
    m_pCopPartner = nullptr;
    std::ranges::fill(m_apCriminalsToKill, nullptr);
    m_isTheDriver = 0;
}

// 0x5DDC60
CCopPed* CCopPed::Constructor(uint32_t copTypeOrModelID) {
    this->CCopPed::CCopPed(copTypeOrModelID);
    return this;
}

// 0x5DE0D0
CCopPed::~CCopPed() {
    if (FindPlayerPed())
        FindPlayerWanted()->RemovePursuitCop(this);
    SetPartner(nullptr);
    ClearCriminalsToKill();
}

// 0x5DE0D0
CCopPed* CCopPed::Destructor() {
    this->CCopPed::~CCopPed();
    return this;
}

// 0x5DDE80
void CCopPed::SetPartner(CCopPed* partner) {
    CEntity::SafeCleanUpRef(m_pCopPartner);
    m_pCopPartner = partner;
    CEntity::SafeRegisterRef(m_pCopPartner);
}

// NOTSA
void CCopPed::ClearCriminalListFromDeadPeds() {
    for (CPed*& ped : m_apCriminalsToKill) {
        if (ped && ped->m_fHealth <= 0.0f) {
            CEntity::ClearReference(ped);
        }
    }
}

// NOTSA
bool CCopPed::IsCriminalInList(CPed* criminal) {
    assert(criminal);

    for (CPed* ped : m_apCriminalsToKill) {
        if (ped == criminal)
            return true;
    }
    return false;
}

// NOTSA
size_t CCopPed::GetEmptySlotForCriminalToKill() {
    for (size_t i = 0; i < std::size(m_apCriminalsToKill); i++) {
        if (!m_apCriminalsToKill[i])
            return i;
    }
    return -1;
}

// 0x5DDEB0
int32 CCopPed::AddCriminalToKill(CPed* criminal) {
    if (!criminal)
        return -1;

    if (!criminal->CanBeCriminal()) {
        return -1;
    }

    ClearCriminalListFromDeadPeds();

    if (criminal->m_fHealth <= 0.0f)
        return -1;

    if (IsCriminalInList(criminal))
        return -1;

    /* Find slot, put criminal in there */
    size_t slot = GetEmptySlotForCriminalToKill();
    if (slot == -1)
        return -1; /* no free slot, return */

    criminal->RegisterReference(reinterpret_cast<CEntity**>(&m_apCriminalsToKill[slot]));
    m_apCriminalsToKill[slot] = criminal;

    criminal->bWantedByPolice = true;
    criminal->bCullExtraFarAway = true;
    criminal->m_fRemovalDistMultiplier = 0.3f;
    criminal->m_nTimeTillWeNeedThisPed = CTimer::GetTimeInMS() + 300'000;

    if (CVehicle* crimVeh = criminal->m_pVehicle) {
        crimVeh->m_nExtendedRemovalRange = 255;
        crimVeh->vehicleFlags.bNeverUseSmallerRemovalRange = true;
    }
    if (m_pCopPartner)
        m_pCopPartner->AddCriminalToKill(criminal);

    return slot;
}

// 0x5DE040
void CCopPed::RemoveCriminalToKill(int32 unk, int32 nCriminalLocalIdx) {
    CPed*& criminal = m_apCriminalsToKill[nCriminalLocalIdx];
    assert(criminal);
    criminal->RegisterReference(reinterpret_cast<CEntity**>(&criminal));
    m_apCriminalsToKill[nCriminalLocalIdx] = nullptr;
}

// 0x5DE070
void CCopPed::ClearCriminalsToKill() {
    for (CPed*& ped : m_apCriminalsToKill) {
        if (ped) {
            ped->m_nTimeTillWeNeedThisPed = CTimer::GetTimeInMS();
            ped->bCullExtraFarAway = false;
            ped->m_fRemovalDistMultiplier = 1.0f;
            CEntity::ClearReference(ped);
        }
    }
}

// 0x5DE160

// 0x5DE160
void CCopPed::ProcessControl() {
    if (FindPlayerWanted()->GetWantedLevel() != 0) {
        if (GetIntelligence()->GetPedDecisionMakerType() == eDecisionMakerEvents::DM_EVENT_PED_ENTERED_MY_VEHICLE)
        {
            GetIntelligence()->SetPedDecisionMakerType(eDecisionMakerEvents::DM_EVENT_KNOCK_OFF_BIKE);
        }
    }

    CPed::ProcessControl();

    if (m_bWasPostponed)
        return;

    if (m_nPedState == PEDSTATE_DEAD)
        return;

    GetActiveWeapon().Update(this);

    if (m_nPedState == PEDSTATE_DIE)
        return;

    if (m_pTargetedObject)
        Say(CTX_GLOBAL_TARGET);

    if (!field_79D)
        return;

    // Never gets here because field_79D is initialized with false
    // This code will break the police, if you comment checking field_79D.
    if (GetIsOnScreen()) {
        if ((CTimer::GetFrameCounter() + m_nRandomSeed) % 32 != 17) // todo: magic
            return;

        CColPoint colPoint{};
        CEntity* hitEntity{};
        CWorld::ProcessLineOfSight(
            *TheCamera.GetGameCamPosition(),
            GetPosition() + CVector{ 0.0f, 0.0f, 0.7f },
            colPoint,
            hitEntity,
            true, false, false, false, false, false, false, false
        );
        if (hitEntity) {
            FlagToDestroyWhenNextProcessed();
        }
    } else {
        FlagToDestroyWhenNextProcessed();
    }
}