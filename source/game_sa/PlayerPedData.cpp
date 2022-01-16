#include "StdInc.h"

void CPlayerPedData::InjectHooks() {
    ReversibleHooks::Install("CPlayerPedData", "Constructor", 0x56F810, &CPlayerPedData::Constructor);
    ReversibleHooks::Install("CPlayerPedData", "Destructor", 0x571840, &CPlayerPedData::Destructor);
    ReversibleHooks::Install("CPlayerPedData", "AllocateData", 0x571880, &CPlayerPedData::AllocateData);
    ReversibleHooks::Install("CPlayerPedData", "DeAllocateData", 0x56F890, &CPlayerPedData::DeAllocateData);
    ReversibleHooks::Install("CPlayerPedData", "SetInitialState", 0x56F210, &CPlayerPedData::SetInitialState);
}

// 0x56F810
CPlayerPedData::CPlayerPedData() {
    m_pWanted = nullptr;
    m_pPedClothesDesc = nullptr;
    m_nMeleeWeaponAnimReferenced = 0;
    m_nMeleeWeaponAnimReferencedExtra = 0;
    SetInitialState();
    m_pSpecialAtomic = nullptr;
    m_nFireHSMissilePressedTime = 0;
    m_LastHSMissileTarget = nullptr;
    m_bLastHSMissileLOS = 0;
}

CPlayerPedData *CPlayerPedData::Constructor() {
    this->CPlayerPedData::CPlayerPedData();
    return this;
}

// 0x571840
CPlayerPedData::~CPlayerPedData() {
    DeAllocateData();
}

CPlayerPedData *CPlayerPedData::Destructor() {
    this->CPlayerPedData::~CPlayerPedData();
    return this;
}

// 0x571880
void CPlayerPedData::AllocateData() {
    if (!m_pWanted) {
      m_pWanted = new CWanted();
    }
    m_pWanted->Initialise();

    if (!m_pPedClothesDesc) {
      m_pPedClothesDesc = new CPedClothesDesc;
    }
    m_pPedClothesDesc->Initialise();
}

//  0x56F890
void CPlayerPedData::DeAllocateData() {
    delete m_pWanted;
    m_pWanted = nullptr;

    delete m_pPedClothesDesc;
    m_pPedClothesDesc = nullptr;
}

// 0x56F210
void CPlayerPedData::SetInitialState() {
    m_pArrestingCop = nullptr;
    m_vecFightMovement = CVector2D(0.0f, 0.0f);
    m_vecTargetBoneOffset = CVector(0.0f, 0.0f, 0.0f);
    m_fMoveBlendRatio = 0.0f;
    m_nChosenWeapon = 0;
    m_nStandStillTimer = 0;
    m_nCarDangerCounter = 0;
    m_pDangerCar = nullptr;
    m_nHitAnimDelayTimer = 0;
    m_fAttackButtonCounter = 0;
    m_nAdrenalineEndTime = 0;
    m_nDrunkenness = 0;
    m_nFadeDrunkenness = 0;
    m_nDrugLevel = 0;
    m_nScriptLimitToGangSize = 99;
    m_fFPSMoveHeading = 0;
    m_fLookPitch = 0;
    m_fSkateBoardSpeed = 0;
    m_fGunSpinSpeed = 0.0f;
    m_fGunSpinAngle = 0;
    m_nLastTimeFiring = 0;
    m_nTargetBone = 0;
    m_nBusFaresCollected = 0;
    m_bPlayerSprintDisabled = false;
    m_bDontAllowWeaponChange = false;
    m_bForceInteriorLighting = false;
    m_nPadDownPressedInMilliseconds = 0;
    m_nPadUpPressedInMilliseconds = 0;
    m_bPlayersGangActive = true;
    m_nWetness = 0;
    m_fTimeCanRun = CStats::GetFatAndMuscleModifier(STAT_MOD_TIME_CAN_RUN);
    m_fMoveSpeed = 0.0f;
    m_fBreath = CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG);
    m_nFireHSMissilePressedTime = 0;
    m_LastHSMissileTarget = nullptr;
    m_pCurrentProstitutePed = nullptr;
    m_pLastProstituteShagged = nullptr;
    m_nPlayerFlags |= 0x1010;
    m_bLastHSMissileLOS = 0;
    m_nModelIndexOfLastBuildingShot = -1;
}
