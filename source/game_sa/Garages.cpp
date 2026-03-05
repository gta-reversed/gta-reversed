#include "StdInc.h"
#include <extensions/ci_string.hpp>

#include "Camera.h"
#include "Garages.h"

void CGarages::InjectHooks() {
    RH_ScopedClass(CGarages);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x447120);
    RH_ScopedInstall(Init_AfterRestart, 0x448B60);
    RH_ScopedInstall(Shutdown, 0x4471B0);
    RH_ScopedInstall(AddOne, 0x4471E0);
    RH_ScopedInstall(CloseHideOutGaragesBeforeSave, 0x44A170);
    RH_ScopedInstall(PlayerArrestedOrDied, 0x449E60);
    RH_ScopedInstall(AllRespraysCloseOrOpen, 0x448B30);
    RH_ScopedInstall(IsModelIndexADoor, 0x448AF0);
    //RH_ScopedInstall(FindSafeHouseIndexForGarageType, 0x4489F0); // Not hooked - fucks up the stack and crashes
    RH_ScopedInstall(FindGarageForObject, 0x44A240);
    RH_ScopedInstall(IsPointWithinHideOutGarage, 0x448900);
    RH_ScopedInstall(IsGarageOpen, 0x447D00);
    RH_ScopedInstall(IsGarageClosed, 0x447D30);
    RH_ScopedInstall(IsCarSprayable, 0x4479A0);
    RH_ScopedInstall(Update, 0x44C8C0);
    RH_ScopedInstall(ActivateGarage, 0x447CD0);
    RH_ScopedInstall(DeActivateGarage, 0x447CB0);
    RH_ScopedInstall(SetTargetCarForMissionGarage, 0x447C40);
    RH_ScopedInstall(StoreCarInNearestImpoundingGarage, 0x44A3C0);
    RH_ScopedInstall(TriggerMessage, 0x447B80);
    RH_ScopedInstall(PrintMessages, 0x447790);
    RH_ScopedInstall(ChangeGarageType, 0x4476D0);
    RH_ScopedInstall(GetGarageNumberByName, 0x447680);
    RH_ScopedInstall(CountCarsInHideoutGarage, 0x44A210);
    RH_ScopedInstall(Load, 0x5D3270);
    RH_ScopedInstall(Save, 0x5D3160);
}

// 0x447120
void CGarages::Init() {
    ZoneScoped;

    NumGarages            = 0;
    MessageEndTime        = 0;
    MessageStartTime      = 0;
    PlayerInGarage        = 0;
    BombsAreFree          = 0;
    CarsCollected         = 0;
    BankVansCollected     = 0;
    PoliceCarsCollected   = 0;
    CrushedCarId          = -1;
    LastTimeHelpMessage   = 0;
    LastGaragePlayerWasIn = -1;
    NoResprays            = false;

    std::ranges::fill(CarTypesCollected, 0);

    for (auto& safeHouseCars : aCarsInSafeHouse) {
        rng::fill(safeHouseCars, CStoredCar{});
    }
}

// 0x448B60
void CGarages::Init_AfterRestart() {
    for (auto& grg : GetAll()) {
        grg.m_Type = grg.m_OriginalType;
        grg.InitDoorsAtStart();
        grg.m_GarageAudio.Reset();
    }
    
    NoResprays = false;

    for (auto& safeHouseCars : aCarsInSafeHouse) {
        rng::fill(safeHouseCars, CStoredCar{});
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

    if (CReplay::Mode == eReplayMode::MODE_PLAYBACK || CGameLogic::IsCoopGameGoingOn()) {
        return;
    }

    bCamShouldBeOutside = false;
    pOldToGarageWeAreIn = TheCamera.m_pToGarageWeAreIn;
    TheCamera.m_pToGarageWeAreIn = nullptr;
    TheCamera.m_pToGarageWeAreInForHackAvoidFirstPerson = nullptr;

    for (int32 i = 0; auto& garage : aGarages) {
        garage.Update(i++);
    }

    if (CTimer::GetFrameCounter() % 16 != 12) // todo: magic number
        return;

    auto& LastUpdatedGarageId = StaticRef<uint32, 0x96EA78>();

    auto& garageToCheck = GetGarage(LastUpdatedGarageId++);
    if (LastUpdatedGarageId >= std::size(aGarages)) {
        LastUpdatedGarageId = 0;
    }

    if (garageToCheck.m_Type == eGarageType::INVALID) {
        return;
    }

    // Originally they've clearly used an some kind of `abs` macro (probably to "optimize" it)... And failed miserably lol
    const auto& camPos = TheCamera.GetPosition();
    if (   std::abs(garageToCheck.m_MinX - camPos.x) < 40.f
        && std::abs(garageToCheck.m_MinY - camPos.y) < 40.f
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
    player->GetPlayerData()->m_nChosenWeapon = slot;
    if (player->m_nSavedWeapon != WEAPON_UNIDENTIFIED)
        player->m_nSavedWeapon = WEAPON_DETONATOR;
}

// 0x4471E0
// Garage flags
// 0x1	door opens up and rotate
// 0x2	door goes in
// 0x4	camera follow players
// TODO...
void CGarages::AddOne(
    CVector base,
    CVector2D p1,
    CVector2D p2,
    float ceilingZ,
    eGarageType type,
    uint32,
    const char* name,
    uint32 flagsIPL
) {
    new (&aGarages[NumGarages++]) CGarage{
        base,
        p1,
        p2,
        ceilingZ,
        type,
        {},
        name,
        flagsIPL,
    };
}

// 0x44A170
void CGarages::CloseHideOutGaragesBeforeSave() {
    for (auto& garage : aGarages) {
        if (!garage.IsHideOut() || garage.IsClosed()) {
            continue;
        }
        garage.SetClosed();
        garage.StoreAndRemoveCarsForThisHideOut(GetStoredCarsInSafehouse(FindSafeHouseIndexForGarageType(garage.m_Type)));
        garage.RemoveCarsBlockingDoorNotInside();
        garage.ResetDoorPosition();
    }
}

// 0x449E60
void CGarages::PlayerArrestedOrDied() {
    for (auto& garage : aGarages) {
        if (garage.m_Type != eGarageType::INVALID) {
            garage.PlayerArrestedOrDied();
        }
    }

    MessageEndTime = 0;
    MessageStartTime = 0;
}

// 0x448B30
void CGarages::AllRespraysCloseOrOpen(bool bOpen) {
    for (auto& grg : GetAll()) {
        if (grg.m_Type == PAYNSPRAY) {
            grg.SetOpened(bOpen);
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

// 0x4489F0 - Not hooked
int32 CGarages::FindSafeHouseIndexForGarageType(eGarageType type) {
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
    const auto& objPos = obj->GetPosition();

    auto  closestDistSqToCenter = std::numeric_limits<float>::max();
    int32 closest              = -1;
    for (auto&& [i, grg] : rngv::enumerate(GetAll())) {
        if (!grg.IsPointInsideGarage(objPos, 7.f)) { // TODO: Maybe use bounding sphere size of the object instead?
            continue;
        }
        const auto distSq = (grg.GetCenter2D() - objPos).SquaredMagnitude();
        if (distSq >= closestDistSqToCenter) {
            continue;
        }
        closestDistSqToCenter = distSq;
        closest               = (int32)i;
    }
    return closest;
}

// 0x449C30
float CGarages::FindDoorHeightForMI(uint32 modelIndex) {
    const auto* mi = CModelInfo::GetModelInfo(modelIndex);
    return mi->GetColModel()->GetBoundingBox().GetHeight() - 0.1f;
}

// 0x448900
bool CGarages::IsPointWithinHideOutGarage(Const CVector& point) {
    for (auto& garage : aGarages) {
        if (!garage.IsHideOut()) {
            continue;
        }
        if (!garage.IsPointInsideGarage(point)) {
            continue;
        }
        return true;
    }
    return false;
}

// 0x447D00
bool CGarages::IsGarageOpen(int16 garageId) {
    const auto& grg = GetGarage(garageId);
    return GetGarage(garageId).IsOpen(true);
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
    * artict1 is Articulated Trailer which by logic cannot to sprayed due big size, but who knows..
    * original code also disallows artict1 to be sprayed (tested)
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
        if (garage.m_Type != eGarageType::INVALID && garage.IsPointInsideGarage(point)) {
            return true;
        }
    }
    return false;
}

// 0x449BA0
bool CGarages::IsPointInAGarageCameraZone(CVector point) {
    for (auto& garage : aGarages) {
        if (garage.m_Type != INVALID && garage.IsPointInsideGarage(point, 0.5f)) {
            return true;
        }
    }
    return false;
}

// 0x447CD0 - TODO: Remove, useless stub
void CGarages::ActivateGarage(int16 garageId) {
    GetGarage(garageId).Activate();
}

// 0x447CB0 - TODO: Remove, useless stub
void CGarages::DeActivateGarage(int16 garageId) {
    GetGarage(garageId).DeActivate();
}

// 0x447C40
void CGarages::SetTargetCarForMissionGarage(int16 garageId, CVehicle* vehicle) {
    auto& garage = GetGarage(garageId);
    if (vehicle) {
        garage.m_TargetCar = vehicle;
        vehicle->RegisterReference(reinterpret_cast<CEntity**>(&garage.m_TargetCar));
        if (garage.m_DoorState == GARAGE_DOOR_CLOSED_DROPPED_CAR)
            garage.SetClosed();
    } else {
#if FIX_BUGS
        // NOTE/TODO: They forgot to call `CleanUpOldReference` | test it!
        if (garage.m_TargetCar)
            garage.m_TargetCar->CleanUpOldReference(reinterpret_cast<CEntity**>(&garage.m_TargetCar));
#endif
        garage.m_TargetCar = nullptr;
    }
}

// 0x447B80
void CGarages::TriggerMessage(const char* tagMsg, int16 numInStr1, uint16 time, int16 numInStr2) {
    if (   strcmp(tagMsg, MessageIDString) != 0
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
    MessageEndTime         = MessageStartTime + time;
    MessageNumberInString  = numInStr1;
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
        if constexpr (sizeof...(formatArgs)) {
            CMessages::InsertNumberInString(TheText.Get(MessageIDString), formatArgs..., gGxtString);
        }
        CFont::PrintString(SCREEN_WIDTH / 2.f, SCREEN_STRETCH_Y(155.f), gGxtString);
    };

    if (MessageNumberInString < 0) {
        if (MessageNumberInString2 < 0) {
            DrawFormattedString();
        } else {
            DrawFormattedString(MessageNumberInString2, -1, -1, -1, -1, -1);
        }
    } else {
        DrawFormattedString(MessageNumberInString, MessageNumberInString2, -1, -1, -1, -1);
    }
}

// 0x4476D0 - TODO: Useless stub, remove
void CGarages::ChangeGarageType(int16 garageId, eGarageType type, uint32 unused) {
    GetGarage(garageId).ChangeType(type);
}

// 0x447680
int16 CGarages::GetGarageNumberByName(const char* name) {
    const auto ciName = notsa::ci_string_view{ name };
    for (auto&& [i, grg] : rngv::enumerate(GetAll())) {
        if (grg.m_Name == ciName) {
            return (int16)i;
        }
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
    for (auto c = 0; c < MAX_CARS_IN_SAFEHOUSE; c++) {
        for (auto r = 0; r < MAX_NUM_SAFEHOUSES; r++) {
            CGenericGameStorage::LoadDataFromWorkBuffer(aCarsInSafeHouse[r][c]);
        }
    }

    for (auto i = 0u; i < NumGarages; i++) {
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

    for (auto i = 0u; i < NumGarages; i++) {
        CSaveGarage sg{};
        sg.CopyGarageIntoSaveGarage(aGarages[i]);
        CGenericGameStorage::SaveDataToWorkBuffer(sg);
    }

    return true;
}

// NOTSA [Code based on 0x44A3C0]
CGarage* CGarages::FindNearestImpountGarage(CVector pos) {
    auto     closestDistSq = std::numeric_limits<float>::max();
    CGarage* closest       = nullptr;
    for (auto& grg : GetAll()) {
        if (!grg.IsImpound()) {
            continue;
        }
        const auto distSq = (grg.m_Base - pos).SquaredMagnitude();
        if (distSq >= closestDistSq) {
            continue;
        }
        closestDistSq = distSq;
        closest       = &grg;
    }
    return closest;
}

// 0x44A3C0
void CGarages::StoreCarInNearestImpoundingGarage(CVehicle* vehicle) {
    const auto impoundGrg = FindNearestImpountGarage(vehicle->GetPosition());
    if (!impoundGrg) {
        return;
    }
    // Now find a slot to store it.
    // If all slots in use, drop first one,
    // so the vehicle is put in the last slot
    auto impoundedCars = aCarsInSafeHouse[FindSafeHouseIndexForGarageType(impoundGrg->m_Type)] | rng::views::take(MAX_CARS_IN_IMPOUND);
    auto freeIdx = rng::count_if(impoundedCars, &CStoredCar::HasCar); // Since the array is [presumeably] sorted by usage, by counting the number of used slots we get the index of the unused slot
    if (freeIdx == MAX_CARS_IN_IMPOUND) { // All in use
        notsa::shift(impoundedCars, -1); 
        freeIdx -= 1;
        std::destroy_at(&impoundedCars[freeIdx]);
    }
    std::construct_at(&impoundedCars[freeIdx], vehicle);
}

// unused
// 0x448890
void CGarages::StopCarFromBlowingUp(CAutomobile* vehicle) {
    return plugin::Call<0x448890, CVehicle*>(vehicle);

    // untested
    vehicle->m_fBurnTimer = 0.0f;
    vehicle->m_fHealth = vehicle->m_fHealth <= 300.0f ? 300.0f : vehicle->m_fHealth;

    auto& dmgmgr = vehicle->m_damageManager;
    if (dmgmgr.GetEngineStatus() >= 275) {
        dmgmgr.SetEngineStatus(dmgmgr.GetEngineStatus());
    } else {
        dmgmgr.SetEngineStatus(275u);
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
size_t CGarages::CountCarsInHideoutGarage(eGarageType type) {
    return rng::count_if(aCarsInSafeHouse[FindSafeHouseIndexForGarageType(type)], &CStoredCar::HasCar);
}
