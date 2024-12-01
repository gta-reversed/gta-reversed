#include "StdInc.h"

#include "Garages.h"

static uint32& LastUpdatedGarageId = *(uint32*)0x96EA78;

void CGarages::InjectHooks() {
    RH_ScopedClass(CGarages);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x447120);
    RH_ScopedInstall(Init_AfterRestart, 0x448B60);
    RH_ScopedInstall(Shutdown, 0x4471B0);
    // RH_ScopedInstall(AddOne, 0x4471E0);
    RH_ScopedInstall(CloseHideOutGaragesBeforeSave, 0x44A170);
    RH_ScopedInstall(PlayerArrestedOrDied, 0x449E60);
    RH_ScopedInstall(AllRespraysCloseOrOpen, 0x448B30);
    RH_ScopedInstall(IsModelIndexADoor, 0x448AF0);
    // RH_ScopedInstall(FindSafeHouseIndexForGarageType, 0x4489F0);
    // RH_ScopedInstall(FindGarageForObject, 0x44A240);
    RH_ScopedInstall(IsPointWithinHideOutGarage, 0x448900);
    RH_ScopedInstall(IsGarageOpen, 0x447D00);
    RH_ScopedInstall(IsGarageClosed, 0x447D30);
    RH_ScopedInstall(IsCarSprayable, 0x4479A0);
    RH_ScopedInstall(Update, 0x44C8C0);
    RH_ScopedInstall(ActivateGarage, 0x447CD0);
    RH_ScopedInstall(DeActivateGarage, 0x447CB0);
    RH_ScopedInstall(SetTargetCarForMissionGarage, 0x447C40);
    // RH_ScopedInstall(StoreCarInNearestImpoundingGarage, 0x44A3C0);
    RH_ScopedInstall(TriggerMessage, 0x447B80);
    RH_ScopedInstall(PrintMessages, 0x447790);
    RH_ScopedInstall(ChangeGarageType, 0x4476D0);
    RH_ScopedInstall(GetGarageNumberByName, 0x447680);
    // RH_ScopedInstall(CountCarsInHideoutGarage, 0x44A210);
    RH_ScopedInstall(Load, 0x5D3270, { .enabled = LOAD_HOOKS_ENABLED }); // bad
    RH_ScopedInstall(Save, 0x5D3160, { .enabled = SAVE_HOOKS_ENABLED }); // possible bad
}

// 0x447120
void CGarages::Init() {
    ZoneScoped;

    NumGarages = 0;
    MessageEndTime = 0;
    MessageStartTime = 0;
    PlayerInGarage = 0;
    BombsAreFree = 0;
    CarsCollected = 0;
    BankVansCollected = 0;
    PoliceCarsCollected = 0;
    std::ranges::fill(CarTypesCollected, 0);
    CrushedCarId = -1;
    LastTimeHelpMessage = 0;
    LastGaragePlayerWasIn = -1;
    NoResprays = false;

    for (auto& safeHouseCars : aCarsInSafeHouse) {
        for (auto& car : safeHouseCars) {
            car.m_wModelIndex = 0;
        }
    }
}

// 0x448B60
void CGarages::Init_AfterRestart() {
    if (NumGarages) {
        for (auto i = 0; i < NumGarages; i++) {
            auto& garage = GetGarage(i);
            garage.m_nType = garage.m_nOriginalType;
            garage.InitDoorsAtStart();
            garage.m_GarageAudio.Reset();
        }
    }

    NoResprays = false;

    for (auto& safeHouseCars : aCarsInSafeHouse) { // TODO: Seems like inlined?
        for (auto& car : safeHouseCars) {
            car.Init();
        }
    }
}

// 0x4471B0
void CGarages::Shutdown() {
    for (auto& garage : aGarages) {
        garage.m_GarageAudio.Reset();
    }
}

// 0x44C8C0
void CGarages::Update() {
    ZoneScoped;

    if (CReplay::Mode == eReplayMode::MODE_PLAYBACK || CGameLogic::IsCoopGameGoingOn())
        return;

    bCamShouldBeOutside = false;
    pOldToGarageWeAreIn = TheCamera.m_pToGarageWeAreIn;
    TheCamera.m_pToGarageWeAreIn = nullptr;
    TheCamera.m_pToGarageWeAreInForHackAvoidFirstPerson = nullptr;

    for (int32 i = 0; auto& garage : aGarages) {
        garage.Update(i++);
    }

    if (CTimer::GetFrameCounter() % 16 != 12) // todo: magic number
        return;

    auto& garageToCheck = GetGarage(LastUpdatedGarageId++);
    if (LastUpdatedGarageId >= std::size(aGarages))
        LastUpdatedGarageId = 0;

    if (garageToCheck.m_nType == eGarageType::INVALID)
        return;

    // Originally they've clearly used an some kind of `abs` macro (probably to "optimize" it)... And failed miserably lol
    const auto& camPos = TheCamera.GetPosition();
    if (   std::abs(garageToCheck.m_fLeftCoord - camPos.x) < 40.f
        && std::abs(garageToCheck.m_fFrontCoord - camPos.y) < 40.f
    ) {
        garageToCheck.TidyUpGarageClose();
    } else {
        garageToCheck.TidyUpGarage();
    }
}

// 0x448660
void CGarages::GivePlayerDetonator() {
    auto player = FindPlayerPed();
    auto slot = CWeaponInfo::GetWeaponInfo(WEAPON_DETONATOR, eWeaponSkill::STD)->m_nSlot;
    player->GiveWeapon(WEAPON_DETONATOR, 1, true);
    player->m_aWeapons[slot].m_State = WEAPONSTATE_READY;
    player->m_pPlayerData->m_nChosenWeapon = slot;
    if (player->m_nSavedWeapon != WEAPON_UNIDENTIFIED)
        player->m_nSavedWeapon = WEAPON_DETONATOR;
}

// 0x4471E0
// Garage flags
// 0x1	door opens up and rotate
// 0x2	door goes in
// 0x4	camera follow players
// TODO...
void CGarages::AddOne(float x1, float y1, float z1, float frontX, float frontY, float x2, float y2, float z2, uint8 type, uint32 a10, char* name, uint32 argFlags) {
    return plugin::Call<0x4471E0, float, float, float, float, float, float, float, float, uint8, uint32, char*, uint32>(x1, y1, z1, frontX, frontY, x2, y2, z2, type, a10, name, argFlags);
}

// 0x44A170
void CGarages::CloseHideOutGaragesBeforeSave() {
    for (auto& garage : aGarages) {
        if (garage.IsHideOut() && garage.m_nDoorState == GARAGE_DOOR_OPEN) {
            garage.SetClosed();
            garage.StoreAndRemoveCarsForThisHideOut(GetStoredCarsInSafehouse(FindSafeHouseIndexForGarageType(garage.m_nType)), 4); // todo: 4 is NUM_GARAGE_STORED_CARS?
            garage.RemoveCarsBlockingDoorNotInside();
            garage.ResetDoorPosition();
        }
    }
}

// 0x449E60
void CGarages::PlayerArrestedOrDied() {
    for (auto& garage : aGarages) {
        if (garage.m_nType != eGarageType::INVALID) {
            garage.PlayerArrestedOrDied();
        }
    }

    MessageEndTime = 0;
    MessageStartTime = 0;
}

// 0x448B30
void CGarages::AllRespraysCloseOrOpen(bool bOpen) {
    if (NumGarages) {
        for (auto i = 0; i < NumGarages; i++) {
            auto& garage = GetGarage(i);
            if (garage.m_nType == eGarageType::PAYNSPRAY) {
                garage.m_nDoorState = bOpen ? GARAGE_DOOR_OPEN : GARAGE_DOOR_CLOSED;
            }
        }
    }
}

// 0x448AF0
bool CGarages::IsModelIndexADoor(int32 model) {
    if (model >= 0) {
        if (const auto mi = CModelInfo::GetModelInfo(model)) {
            if (const auto ami = mi->AsAtomicModelInfoPtr()) {
                return ami->IsGarageDoor();
            }
        }
    }
    return false;
}

// wrong
// 0x4489F0
int32 CGarages::FindSafeHouseIndexForGarageType(eGarageType type) {
    return plugin::CallAndReturn<int32, 0x4489F0, eGarageType>(type);

    switch (type) {
    case SAFEHOUSE_SANTAMARIA:     return 1;
    case SAGEHOUSE_ROCKSHORE:      return 2;
    case SAFEHOUSE_FORTCARSON:     return 3;
    case SAFEHOUSE_VERDANTMEADOWS: return 4;
    case SAFEHOUSE_DILLIMORE:      return 5;
    case SAFEHOUSE_PRICKLEPINE:    return 6;
    case SAFEHOUSE_WHITEWOOD:      return 7;
    case SAFEHOUSE_PALOMINOCREEK:  return 8;
    case SAFEHOUSE_REDSANDSWEST:   return 9;
    case SAFEHOUSE_ELCORONA:       return 10;
    case SAFEHOUSE_MULHOLLAND:     return 11;
    case IMPOUND_LS:               return 12;
    case IMPOUND_SF:               return 13;
    case IMPOUND_LV:               return 14;
    case SAFEHOUSE_CALTONHEIGHTS:  return 15;
    case SAFEHOUSE_PARADISO:       return 16;
    case SAFEHOUSE_DOHERTY:        return 17;
    case SAFEHOUSE_HASHBURY:       return 18;
    case HANGAR_ABANDONED_AIRPORT: return 19;
    default:                       return 0;
    }
}

// 0x44A240
int16 CGarages::FindGarageForObject(CObject* obj) {
    return plugin::CallAndReturn<int16, 0x44A240, CObject*>(obj);
}

// 0x447680
int16 CGarages::FindGarageIndex(char* name) {
    for (auto i = 0; i < NumGarages; i++) {
        if (_stricmp(name, GetGarage(i).m_anName) == 0)
            return i;
    }
    return -1;
}

// 0x449C30
float CGarages::FindDoorHeightForMI(uint32 modelIndex) {
    const auto* mi = CModelInfo::GetModelInfo(modelIndex);
    return mi->GetColModel()->GetBoundingBox().GetHeight() - 0.1f;
}

// 0x448900
bool CGarages::IsPointWithinHideOutGarage(Const CVector& point) {
    for (auto& garage : aGarages) {
        if (garage.IsHideOut()) {
            if (garage.IsPointInsideGarage(point))
                return true;
            break;
        }
    }
    return false;
}

// 0x447D00
bool CGarages::IsGarageOpen(int16 garageId) {
    return GetGarage(garageId).IsOpen();
}

// 0x447D30
bool CGarages::IsGarageClosed(int16 garageId) {
    return GetGarage(garageId).IsClosed();
}

// 0x4479A0
bool CGarages::IsCarSprayable(CVehicle* vehicle) {
    if (vehicle->IsLawEnforcementVehicle() || vehicle->IsSubBMX())
        return false;

    switch (vehicle->m_nModelIndex) {
    case eModelID::MODEL_BUS:
    case eModelID::MODEL_COACH:
    /* TODO:
    * In the source it compares against -2, but the in the ASM comment it says `artict1`
    * artict1 is Articulated Trailer which by logic cannot to sprayed due big size, but who know..
    * original code also disallow artict1 to be sprayed (tested)
    */
    case eModelID::MODEL_ARTICT1:
    case eModelID::MODEL_FIRETRUK:
    case eModelID::MODEL_AMBULAN:
        return false;
    }
    return true;
}

// 0x449740
bool CGarages::IsThisCarWithinGarageArea(int16 garageId, CEntity* entity) {
    return GetGarage(garageId).IsEntityEntirelyInside3D(entity, 0.0f);
}

// 0x448990
bool CGarages::IsPointWithinAnyGarage(CVector& point) {
    for (auto& garage : aGarages) {
        if (garage.m_nType != eGarageType::INVALID || garage.IsPointInsideGarage(point)) {
            return true;
        }
    }
    return false;
}

// 0x449BA0
bool CGarages::IsPointInAGarageCameraZone(CVector point) {
    for (auto& garage : aGarages) {
        if (garage.m_nType && garage.IsPointInsideGarage(point, 0.5f)) // todo: m_nType == ONLY_TARGET_VEH or != INVALID
            return true;
    }
    return false;
}

// 0x447CD0
void CGarages::ActivateGarage(int16 garageId) {
    auto& garage = GetGarage(garageId);
    garage.m_bInactive = false;
    if (   garage.m_nType == eGarageType::UNKN_CLOSESONTOUCH
        && garage.m_nDoorState != GARAGE_DOOR_CLOSED
    ) {
        garage.m_nDoorState = GARAGE_DOOR_OPENING;
    }
}

// 0x447CB0
void CGarages::DeActivateGarage(int16 garageId) {
    GetGarage(garageId).m_bInactive = true;
}

// 0x447C40
void CGarages::SetTargetCarForMissionGarage(int16 garageId, CVehicle* vehicle) {
    auto& garage = GetGarage(garageId);
    if (vehicle) {
        garage.m_pTargetCar = vehicle;
        vehicle->RegisterReference(reinterpret_cast<CEntity**>(&garage.m_pTargetCar));
        if (garage.m_nDoorState == GARAGE_DOOR_CLOSED_DROPPED_CAR)
            garage.SetClosed();
    } else {
#if FIX_BUGS
        // NOTE/TODO: They forgot to call `CleanUpOldReference` | test it!
        if (garage.m_pTargetCar)
            garage.m_pTargetCar->CleanUpOldReference(reinterpret_cast<CEntity**>(&garage.m_pTargetCar));
#endif
        garage.m_pTargetCar = nullptr;
    }
}

// 0x447B80
void CGarages::TriggerMessage(const char* tagMsg, int16 numInStr1, uint16 time, int16 numInStr2) {
    if (   strcmp(tagMsg, MessageIDString) != 0 // Different strings
        || CTimer::GetTimeInMS() < MessageStartTime
        || CTimer::GetTimeInMS() >= MessageEndTime
    ) {
        strcpy_s(MessageIDString, tagMsg);
        MessageStartTime = CTimer::GetTimeInMS();
    } else {
        if (CTimer::GetTimeInMS() - MessageStartTime <= 500)
            return;
        MessageStartTime = CTimer::GetTimeInMS() - 500;
    }
    MessageEndTime = MessageStartTime + time;
    MessageNumberInString = numInStr1;
    MessageNumberInString2 = numInStr2;
}

// 0x447790
void CGarages::PrintMessages() {
    if (   CTimer::GetTimeInMS() < MessageStartTime
        || CTimer::GetTimeInMS() >= MessageEndTime
    ) {
        // Time over
        MessageIDString[0] = 0;
        return;
    }

    // Draw it
    CFont::SetScale(SCREEN_STRETCH_X(0.5f), SCREEN_STRETCH_Y(1.4f));
    CFont::SetProportional(true);
    CFont::SetBackground(false, false);
    CFont::SetCentreSize(SCREEN_STRETCH_FROM_RIGHT(230.f));
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetFontStyle(eFontStyle::FONT_MENU);
    CFont::SetColor(HudColour.GetRGB(eHudColours::HUD_COLOUR_LIGHT_BLUE));
    CFont::SetEdge(2);
    CFont::SetDropColor({ 0, 0, 0, 255 });

    const auto DrawFormattedString = [&](auto... formatArgs) {
        CMessages::InsertNumberInString(TheText.Get(MessageIDString), formatArgs..., gGxtString);
        CFont::PrintString(SCREEN_WIDTH / 2.f, SCREEN_STRETCH_Y(155.f), gGxtString);
    };

    if (MessageNumberInString < 0) {
        if (MessageNumberInString2 < 0) {
            CFont::PrintString(SCREEN_WIDTH / 2.f, SCREEN_STRETCH_Y(155.f), TheText.Get(MessageIDString));
        } else {
            DrawFormattedString(MessageNumberInString2, -1, -1, -1, -1, -1);
        }
    } else {
        DrawFormattedString(MessageNumberInString, MessageNumberInString2, -1, -1, -1, -1);
    }
}

// 0x4476D0
void CGarages::ChangeGarageType(int16 garageId, eGarageType type, uint32 unused) {
    auto garage = GetGarage(garageId);
    garage.m_nType = type;

    if (type < BOMBSHOP_TIMED) {
        garage.SetClosed();
        garage.ResetDoorPosition();
        return;
    }

    if (type > PAYNSPRAY) {
        if (type == BURGLARY)
            return;

        garage.SetClosed();
        garage.ResetDoorPosition();
        return;
    }

    if (garage.IsClosed()) {
        garage.SetOpened();
        garage.m_fDoorPosition = 1.0f;
    }
}

// 0x447680
int16 CGarages::GetGarageNumberByName(const char* name) {
    for (auto i = 0; i < NumGarages; i++) {
        const auto& garage = aGarages[i];
        if (_stricmp(garage.m_anName, name) == 0)
            return i;
    }
    return -1;
}

// 0x5D3270
bool CGarages::Load() {
    CloseHideOutGaragesBeforeSave();

    CGenericGameStorage::LoadDataFromWorkBuffer(NumGarages);
    CGenericGameStorage::LoadDataFromWorkBuffer(BombsAreFree);
    CGenericGameStorage::LoadDataFromWorkBuffer(RespraysAreFree);
    CGenericGameStorage::LoadDataFromWorkBuffer(NoResprays);
    CGenericGameStorage::LoadDataFromWorkBuffer(CarsCollected);
    CGenericGameStorage::LoadDataFromWorkBuffer(BankVansCollected);
    CGenericGameStorage::LoadDataFromWorkBuffer(PoliceCarsCollected);

    for (auto& v : CarTypesCollected) {
        CGenericGameStorage::LoadDataFromWorkBuffer(v);
    }

    CGenericGameStorage::LoadDataFromWorkBuffer(LastTimeHelpMessage);

    // NOTE: Here they messed up the order of loops
    //       C/C++ is row-major, so, the row loop should've been the outer one..
    //       This is important, because the array isn't accessed contiguously
    //       Which means the data isn't saved contiguously either, so watch out for that.
    for (auto c = 0; c < 4; c++) {
        for (auto r = 0; r < 20; r++) {
            CGenericGameStorage::LoadDataFromWorkBuffer(aCarsInSafeHouse[r][c]);
        }
    }

    for (auto i = 0; i < NumGarages; i++) {
        auto sg = CGenericGameStorage::LoadDataFromWorkBuffer<CSaveGarage>();
        sg.CopyGarageOutOfSaveGarage(aGarages[i]);
    }

    MessageEndTime = 0;
    MessageStartTime = 0;
    bCamShouldBeOutside = false;

    return true;
}

// todo: fix Update()
// 0x5D3160
bool CGarages::Save() {
    CGenericGameStorage::SaveDataToWorkBuffer(NumGarages);
    CGenericGameStorage::SaveDataToWorkBuffer(BombsAreFree);
    CGenericGameStorage::SaveDataToWorkBuffer(RespraysAreFree);
    CGenericGameStorage::SaveDataToWorkBuffer(NoResprays);
    CGenericGameStorage::SaveDataToWorkBuffer(CarsCollected);
    CGenericGameStorage::SaveDataToWorkBuffer(BankVansCollected);
    CGenericGameStorage::SaveDataToWorkBuffer(PoliceCarsCollected);

    for (auto carType : CarTypesCollected) {
        CGenericGameStorage::SaveDataToWorkBuffer(carType);
    }

    CGenericGameStorage::SaveDataToWorkBuffer(LastTimeHelpMessage);

    // NOTE: Here they messed up the order of loops
    //       C/C++ is row-major, so, the row loop should've been the outer one..
    //       This is important, because the array isn't accessed contiguously
    //       Which means the data isn't saved contiguously either, so watch out for that.
    for (auto c = 0; c < 4; c++) {
        for (auto r = 0; r < 20; r++) {
            CGenericGameStorage::SaveDataToWorkBuffer(aCarsInSafeHouse[r][c]);
        }
    }

    for (auto i = 0; i < NumGarages; i++) {
        CSaveGarage sg{};
        sg.CopyGarageIntoSaveGarage(aGarages[i]);
        CGenericGameStorage::SaveDataToWorkBuffer(sg);
    }

    return true;
}

// 0x44A3C0
void CGarages::StoreCarInNearestImpoundingGarage(CVehicle* vehicle) {
    plugin::Call<0x44A3C0, CVehicle*>(vehicle);
}

// unused
// 0x448890
void CGarages::StopCarFromBlowingUp(CAutomobile* vehicle) {
    return plugin::Call<0x448890, CVehicle*>(vehicle);

    // untested
    vehicle->m_fBurnTimer = 0.0f;
    vehicle->m_fHealth = vehicle->m_fHealth <= 300.0f ? 300.0f : vehicle->m_fHealth;

    auto& manager = vehicle->m_damageManager;
    if (manager.GetEngineStatus() >= 275) {
        manager.SetEngineStatus(manager.GetEngineStatus());
    } else {
        manager.SetEngineStatus(275u);
    }
}

// untested, unused (inlined?)
// 0x447E10
bool CGarages::HasResprayHappened(int16 garageId) {
    auto& garage = GetGarage(garageId);
    const auto resprayed = garage.m_bUsedRespray;
    garage.m_bUsedRespray = false;
    return resprayed;
}

// 0x44A210
int32 CGarages::CountCarsInHideoutGarage(eGarageType type) {
    return plugin::CallAndReturn<int32, 0x44A210, eGarageType>(type);
}
