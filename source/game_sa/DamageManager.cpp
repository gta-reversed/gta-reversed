#include "StdInc.h"

#include "DamageManager.h"

void CDamageManager::InjectHooks() {
    RH_ScopedClass(CDamageManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x6A0520);
    RH_ScopedInstall(ResetDamageStatusAndWheelDamage, 0x6A04E0);
    RH_ScopedInstall(FuckCarCompletely, 0x6C25D0); 
    RH_ScopedInstall(ApplyDamage, 0x6C24B0); 
    RH_ScopedInstall(ProgressAeroplaneDamage, 0x6C2460); 
    RH_ScopedInstall(ProgressWheelDamage, 0x6C2440); 
    RH_ScopedInstall(ProgressPanelDamage, 0x6C23C0); 
    RH_ScopedInstall(ProgressEngineDamage, 0x6C23B0); 
    RH_ScopedInstall(ProgressDoorDamage, 0x6C2320); 
    RH_ScopedInstall(GetAeroplaneCompStatus, 0x6C2300); 
    RH_ScopedInstall(SetAeroplaneCompStatus, 0x6C22D0); 
    RH_ScopedInstall(GetEngineStatus, 0x6C22C0); 
    RH_ScopedInstall(SetEngineStatus, 0x6C22A0); 
    RH_ScopedInstall(GetCarNodeIndexFromPanel, 0x6C26A0); 
    RH_ScopedInstall(GetDoorStatus_Component, 0x6C2250); 
    RH_ScopedInstall(GetDoorStatus, 0x6C2230); 
    RH_ScopedInstall(SetDoorStatus_Component, 0x6C21E0); 
    RH_ScopedOverloadedInstall(SetDoorStatus, "", 0x6C21C0, void(CDamageManager::*)(eDoors, eDoorStatus));
    RH_ScopedInstall(SetWheelStatus, 0x6C21A0); 
    RH_ScopedInstall(GetPanelStatus, 0x6C2180); 
    RH_ScopedInstall(SetPanelStatus, 0x6C2150); 
    RH_ScopedInstall(GetLightStatus, 0x6C2130); 
    RH_ScopedInstall(SetLightStatus, 0x6C2100); 
    RH_ScopedInstall(ResetDamageStatus, 0x6C20E0); 
    RH_ScopedInstall(GetComponentGroup, 0x6C2040); 
    RH_ScopedInstall(GetCarNodeIndexFromDoor, 0x6C26F0);
    RH_ScopedNamedInstall(GetWheelStatus_Hooked, "GetWheelStatus", 0x6C21B0);
}

CDamageManager::CDamageManager(float wheelDamageEffect) :
    m_fWheelDamageEffect{ wheelDamageEffect }
{
    ResetDamageStatus();
}

// 0x6A0520
void CDamageManager::Init() {
    m_fWheelDamageEffect = 0.0f;
    ResetDamageStatus(); /* R* used memset here, but let's be a more elegant than that. */
}

// 0x6A04E0
void CDamageManager::ResetDamageStatusAndWheelDamage() {
    ResetDamageStatus();
    m_fWheelDamageEffect = 0.5f;
}

// 0x6C25D0
void CDamageManager::FuckCarCompletely(bool bDontDetachWheel) {
    if (!bDontDetachWheel)
        SetWheelStatus((eCarWheel)CGeneral::GetRandomNumberInRange((size_t)eCarWheel::MAX_CARWHEELS), eCarWheelStatus::WHEEL_STATUS_MISSING);

    for (size_t i = 0; i < eDoors::MAX_DOORS; i++)
        SetDoorStatus((eDoors)i, eDoorStatus::DAMSTATE_NOTPRESENT);

    for (size_t i = 0; i < 3; i++) {
        ProgressPanelDamage(ePanels::FRONT_BUMPER);
        ProgressPanelDamage(ePanels::REAR_BUMPER);
    }

    for (size_t i = 0; i < eLights::MAX_LIGHTS; i++)
        SetLightStatus((eLights)i, eLightsState::VEHICLE_LIGHT_OK);

    for (size_t i = 0; i < ePanels::MAX_PANELS; i++)
        SetPanelStatus((ePanels)i, ePanelDamageState::DAMSTATE_OK);

    SetEngineStatus(250u);
}

// 0x6C24B0
bool CDamageManager::ApplyDamage(CAutomobile* vehicle, tComponent compId, float fIntensity, float fColDmgMult) {
    if (!vehicle->autoFlags.bCanBeVisiblyDamaged)
        return false;

    tComponentGroup group{};
    uint8 relCompIdx{};
    GetComponentGroup(compId, group, relCompIdx);

    static constexpr float afPanelDamageMultByGroup[] = { 2.5f, 1.25f, 3.2f, 1.4f, 2.5f, 2.8f, 0.5f, 1.2f, 0.87f, 0.2f };
    fIntensity *= afPanelDamageMultByGroup[(size_t)group];
    if (compId == tComponent::COMPONENT_WINDSCREEN)
        fIntensity *= 0.6f;

    if (fIntensity <= 150.0f)
        return false;

    switch (group) {
    case tComponentGroup::COMPGROUP_PANEL: {
        if (ProgressPanelDamage((ePanels)relCompIdx))
            vehicle->SetBumperDamage((ePanels)relCompIdx, false);
        return true;
    }
    case tComponentGroup::COMPGROUP_WHEEL: {
        ProgressWheelDamage((eCarWheel)relCompIdx);
        return true;
    }
    case tComponentGroup::COMPGROUP_DOOR:
    case tComponentGroup::COMPGROUP_BONNET:
    case tComponentGroup::COMPGROUP_BOOT: {
        if (ProgressDoorDamage((eDoors)relCompIdx, vehicle))
            vehicle->SetDoorDamage((eDoors)relCompIdx, false);
        return true;
    }
    case tComponentGroup::COMPGROUP_LIGHT: {
        SetLightStatus((eLights)relCompIdx, eLightsState::VEHICLE_LIGHT_SMASHED);
        if (ProgressPanelDamage((ePanels)relCompIdx))
            vehicle->SetPanelDamage((ePanels)relCompIdx, false);
        return true;
    }
    default:
        return true;
    }
}

// 0x6C2460
bool CDamageManager::ProgressAeroplaneDamage(uint8 nFrameId) {
    const auto current = (ePanelDamageState)GetAeroplaneCompStatus(nFrameId);
    if (current == ePanelDamageState::DAMSTATE_DAMAGED)
        return false;

    SetAeroplaneCompStatus(nFrameId, (ePanelDamageState)((unsigned)current + 1));
    return true;
}

// 0x6C2440
bool CDamageManager::ProgressWheelDamage(eCarWheel wheel) {
    const eCarWheelStatus current = GetWheelStatus(wheel);
    if (current == eCarWheelStatus::WHEEL_STATUS_MISSING)
        return false;

    SetWheelStatus(wheel, (eCarWheelStatus)((unsigned)current + 1));
    return true;
}

// 0x6C23C0
bool CDamageManager::ProgressPanelDamage(ePanels panel) {
    switch (GetPanelStatus(panel)) {
    case ePanelDamageState::DAMSTATE_DAMAGED:
    {
        switch (panel) {
        case ePanels::WINDSCREEN_PANEL: {
            if (CGeneral::GetRandomNumberInRange(0, 2))
                return false;
            break;
        }
        default: {
            if (CGeneral::GetRandomNumberInRange(0, 8))
                return false;
            break;
        }
        }
        SetPanelStatus(panel, ePanelDamageState::DAMSTATE_OPENED_DAMAGED);
        return true;
    }
    case ePanelDamageState::DAMSTATE_OPENED_DAMAGED: {
        return false;
    }
    }
    SetPanelStatus(panel, (ePanelDamageState)((int)GetPanelStatus(panel) + 1));
    return true;
}

// 0x6C23B0
void CDamageManager::ProgressEngineDamage() {
    /* empty */
}

// 0x6C2320
bool CDamageManager::ProgressDoorDamage(eDoors door, CAutomobile* pAuto) {
    if ((uint32)door >= (uint32)eDoors::MAX_DOORS) {
        return false;
    }

    switch (GetDoorStatus(door)) {
    case eDoorStatus::DAMSTATE_OK:
    case eDoorStatus::DAMSTATE_OPENED: {
        pAuto->GetDoor(door).Open(0.0f);
        SetDoorStatus(door, eDoorStatus::DAMSTATE_DAMAGED);
        break;
    }
    case eDoorStatus::DAMSTATE_DAMAGED: {
        SetDoorStatus(door, eDoorStatus::DAMSTATE_OPENED_DAMAGED);
        break;
    }
    case eDoorStatus::DAMSTATE_OPENED_DAMAGED: {
        if (CGeneral::GetRandomNumberInRange(0, 8))
            return false;
        SetDoorStatus(door, eDoorStatus::DAMSTATE_NOTPRESENT);
        break;
    }
    case eDoorStatus::DAMSTATE_NOTPRESENT:
        return false;
    }

    /* this original code replaced by SetDoorStatus in upper switch
    if (door < 6u)
        m_aDoorsStatus[door] = status;
    */

    return true;
}

// 0x6C2300
uint8 CDamageManager::GetAeroplaneCompStatus(uint8 frame) {
    return m_nPanelsStatus >> (2 * (m_nPanelsStatus - 12)) & 3;
}

// 0x6C22D0
void CDamageManager::SetAeroplaneCompStatus(uint8 frame, ePanelDamageState status) {
    const auto component = frame - 12;
    m_nPanelsStatus = (status << (2 * component)) | m_nPanelsStatus & ~(3 << (2 * component));
}

// 0x6C22C0
uint32 CDamageManager::GetEngineStatus() {
    return m_nEngineStatus;
}

// 0x6C22A0
void CDamageManager::SetEngineStatus(uint32 status) {
    m_nEngineStatus = std::min<uint8>(status, 250u);
}

// 0x6C26A0
eCarNodes CDamageManager::GetCarNodeIndexFromPanel(ePanels panel) {
    switch (panel) {
    case ePanels::FRONT_LEFT_PANEL:
        return eCarNodes::CAR_WING_LF;
    case ePanels::FRONT_RIGHT_PANEL:
        return eCarNodes::CAR_WING_RF;
    case ePanels::WINDSCREEN_PANEL:
        return eCarNodes::CAR_WINDSCREEN;
    case ePanels::FRONT_BUMPER:
        return eCarNodes::CAR_BUMP_FRONT;
    case ePanels::REAR_BUMPER:
        return eCarNodes::CAR_BUMP_REAR;
    default:
        return (eCarNodes)-1;
    }
}

// 0x6C2250
eDoorStatus CDamageManager::GetDoorStatus_Component(tComponent doorComp) const {
    /* Enums don't seem to match up... */
    switch (doorComp) {
    case tComponent::COMPONENT_DOOR_RF:
        return GetDoorStatus(eDoors::DOOR_RIGHT_FRONT);
    case tComponent::COMPONENT_DOOR_LR:
        return GetDoorStatus(eDoors::DOOR_RIGHT_REAR);
    case tComponent::COMPONENT_DOOR_RR:
        return GetDoorStatus(eDoors::DOOR_LEFT_FRONT);
    case tComponent::COMPONENT_WING_LF:
        return GetDoorStatus(eDoors::DOOR_LEFT_REAR);
    default:
        return eDoorStatus::DAMSTATE_NOTPRESENT;
    }
}

// 0x6C21E0
void CDamageManager::SetDoorStatus_Component(tComponent doorComp, eDoorStatus status) {
    switch (doorComp) {
    case tComponent::COMPONENT_DOOR_RF:
        SetDoorStatus(eDoors::DOOR_RIGHT_FRONT, status);
        break;
    case tComponent::COMPONENT_DOOR_LR:
        SetDoorStatus(eDoors::DOOR_RIGHT_REAR, status);
        break;
    case tComponent::COMPONENT_DOOR_RR:
        SetDoorStatus(eDoors::DOOR_LEFT_FRONT, status);
        break;
    case tComponent::COMPONENT_WING_LF:
        SetDoorStatus(eDoors::DOOR_LEFT_REAR, status);
        break;
    }
}

// 0x6C21C0
void CDamageManager::SetDoorStatus(eDoors door, eDoorStatus status) {
    switch (door) {
    case eDoors::DOOR_BONNET:
    case eDoors::DOOR_BOOT:
    case eDoors::DOOR_LEFT_FRONT:
    case eDoors::DOOR_RIGHT_FRONT:
    case eDoors::DOOR_LEFT_REAR:
    case eDoors::DOOR_RIGHT_REAR:
        m_aDoorsStatus[(unsigned)door] = status;
        break;
    }
}

// 0x6C21B0
eCarWheelStatus CDamageManager::GetWheelStatus(eCarWheel wheel) const {
    return m_anWheelsStatus[(unsigned)wheel];
}

// 0x6C21A0
void CDamageManager::SetWheelStatus(eCarWheel wheel, eCarWheelStatus status) {
    m_anWheelsStatus[(unsigned)wheel] = status;
}

// 0x6C2180
ePanelDamageState CDamageManager::GetPanelStatus(ePanels panel) const {
    return (ePanelDamageState)((m_nPanelsStatus >> (4 * (unsigned)panel)) & 0xF);
}

// 0x6C2150
void CDamageManager::SetPanelStatus(ePanels panel, ePanelDamageState status) {
    m_nPanelsStatus = (status << (4 * (unsigned)panel)) | m_nPanelsStatus & ~(15 << (4 * (unsigned)panel));
}

// 0x6C2130
eLightsState CDamageManager::GetLightStatus(eLights light) const {
    return (eLightsState)((m_nLightsStatus >> (2 * (unsigned)light)) & 3);
}

// 0x6C2100
void CDamageManager::SetLightStatus(eLights light, eLightsState status) {
    m_nLightsStatus = ((unsigned)status << (2 * (unsigned)light)) | m_nLightsStatus & ~(3 << (2 * (unsigned)light));
}

// 0x6C20E0
void CDamageManager::ResetDamageStatus() {
    SetEngineStatus(0);

    for (size_t i = 0; i < eCarWheel::MAX_CARWHEELS; i++)
        SetWheelStatus((eCarWheel)i, eCarWheelStatus::WHEEL_STATUS_OK);

    for (size_t i = 0; i < eDoors::MAX_DOORS; i++)
        SetDoorStatus((eDoors)i, eDoorStatus::DAMSTATE_OK);

    for (size_t i = 0; i < eLights::MAX_LIGHTS; i++)
        SetLightStatus((eLights)i, eLightsState::VEHICLE_LIGHT_OK);

    for (size_t i = 0; i < ePanels::MAX_PANELS; i++)
        SetPanelStatus((ePanels)i, ePanelDamageState::DAMSTATE_OK);
}

// 0x6C2040
bool CDamageManager::GetComponentGroup(tComponent nComp, tComponentGroup& outCompGroup, uint8& outComponentRelativeIdx) {
    outComponentRelativeIdx = (uint8)-2;
    switch (nComp) {
    case tComponent::COMPONENT_WHEEL_LF:
    case tComponent::COMPONENT_WHEEL_RF:
    case tComponent::COMPONENT_WHEEL_LR:
    case tComponent::COMPONENT_WHEEL_RR: {
        outCompGroup = tComponentGroup::COMPGROUP_WHEEL;
        outComponentRelativeIdx = (uint8)nComp - 1;
        return true;
    }
    case tComponent::COMPONENT_BONNET: {
        outCompGroup = tComponentGroup::COMPGROUP_BONNET;
        outComponentRelativeIdx = (uint8)eDoors::DOOR_BONNET;
        return true;
    }
    case tComponent::COMPONENT_BOOT: {
        outCompGroup = tComponentGroup::COMPGROUP_BOOT;
        outComponentRelativeIdx = (uint8)eDoors::DOOR_BOOT;
        return true;
    }
    case tComponent::COMPONENT_DOOR_LF:
    case tComponent::COMPONENT_DOOR_RF:
    case tComponent::COMPONENT_DOOR_LR:
    case tComponent::COMPONENT_DOOR_RR: {
        outCompGroup = tComponentGroup::COMPGROUP_DOOR;
        outComponentRelativeIdx = (uint8)nComp - (uint8)tComponent::COMPONENT_BONNET;
        return true;
    }
    case tComponent::COMPONENT_WING_LF:
    case tComponent::COMPONENT_WING_RF:
    case tComponent::COMPONENT_WING_LR:
    case tComponent::COMPONENT_WING_RR: 
    case tComponent::COMPONENT_WINDSCREEN: {
        outCompGroup = tComponentGroup::COMPGROUP_LIGHT;
        outComponentRelativeIdx = (uint8)nComp - (uint8)tComponent::COMPONENT_WING_LF;
        return true;
    }
    case tComponent::COMPONENT_BUMP_FRONT:
    case tComponent::COMPONENT_BUMP_REAR: {
        outCompGroup = tComponentGroup::COMPGROUP_PANEL;
        outComponentRelativeIdx = (uint8)nComp - (uint8)tComponent::COMPONENT_WING_LF;
        return true;
    }
    case tComponent::COMPONENT_NA: {
        outCompGroup = tComponentGroup::COMPGROUP_NA;
        outComponentRelativeIdx = 0;
        return true;
    }
    }
    return false;
}

// NOTSA
void CDamageManager::SetAllWheelsState(eCarWheelStatus state) {
    constexpr eCarWheel wheels[]{CAR_WHEEL_FRONT_LEFT, CAR_WHEEL_REAR_LEFT, CAR_WHEEL_FRONT_RIGHT, CAR_WHEEL_REAR_RIGHT};
    for (auto&& wheel : wheels) {
        SetWheelStatus(wheel, state);
    }
}

// NOTSA
void CDamageManager::SetDoorStatus(std::initializer_list<eDoors> doors, eDoorStatus status) {
    for (auto&& door : doors) {
        SetDoorStatus(door, status);
    }
}

auto CDamageManager::GetAllLightsState() const->std::array<eLightsState, 4> {
    return {
        GetLightStatus(eLights::LIGHT_FRONT_LEFT),
        GetLightStatus(eLights::LIGHT_FRONT_RIGHT),
        GetLightStatus(eLights::LIGHT_REAR_LEFT),
        GetLightStatus(eLights::LIGHT_REAR_RIGHT)
    };
}

auto CDamageManager::GetAllDoorsStatus() const -> std::array<eDoorStatus, MAX_DOORS> {
    return {
        GetDoorStatus(eDoors::DOOR_BONNET),
        GetDoorStatus(eDoors::DOOR_BOOT),
        GetDoorStatus(eDoors::DOOR_LEFT_FRONT),
        GetDoorStatus(eDoors::DOOR_RIGHT_FRONT),
        GetDoorStatus(eDoors::DOOR_LEFT_REAR),
        GetDoorStatus(eDoors::DOOR_RIGHT_REAR)
    };
}

/*!
* @notsa
* @brief Should only be called if the door is present (asserts in debug)
* @returns If door's state is either \r DAMSTATE_OPENED or \r DAMSTATE_OPENED_DAMAGED
*/
bool CDamageManager::IsDoorOpen(eDoors door) const {
    switch (GetDoorStatus(door)) {
    case eDoorStatus::DAMSTATE_OPENED:
    case eDoorStatus::DAMSTATE_OPENED_DAMAGED:
        return true;
    case eDoorStatus::DAMSTATE_NOTPRESENT:
        assert(0 && "Door not present @ IsDoorOpen"); // Otherwise `!IsDoorOpen() == IsDoorClosed()` may not always be true which may cause bugs :D
        return false;
    }
    return false;
}

/*!
* @notsa
* @brief Should only be called if the door is present (asserts in debug)
* @returns If door's state is either \r DAMSTATE_OK` or \r DAMSTATE_DAMAGED
*/
bool CDamageManager::IsDoorClosed(eDoors door) const {
    switch (GetDoorStatus(door)) {
    case eDoorStatus::DAMSTATE_OK:
    case eDoorStatus::DAMSTATE_DAMAGED:
        return true;
    case eDoorStatus::DAMSTATE_NOTPRESENT:
        assert(0 && "Door not present @ IsDoorClosed"); // Otherwise `!IsDoorOpen() == IsDoorClosed()` may not always be true which may cause bugs :D
        return false;
    }
    return false;
}

bool CDamageManager::IsDoorPresent(eDoors door) const {
    return GetDoorStatus(door) != DAMSTATE_NOTPRESENT;
}

/*!
* @notsa
* @brief   Checks if door is damaged.
* @returns Returns if door's state is neither \r DAMSTATE_OK or \r DAMSTATE_OPENED
*/
bool CDamageManager::IsDoorDamaged(eDoors door) const {
    switch (GetDoorStatus(door)) {
    case eDoorStatus::DAMSTATE_OK:
    case eDoorStatus::DAMSTATE_OPENED:
        return false;
    }
    return true;
}

/*!
* @notsa
* @brief Sets door open. Shouldn't be called if door isn't present (will assert in debug).
*/
void CDamageManager::SetDoorOpen(eDoors door) {
    switch (GetDoorStatus(door)) {
    case eDoorStatus::DAMSTATE_OK:
        SetDoorStatus(door, eDoorStatus::DAMSTATE_OPENED);
        break;
    case eDoorStatus::DAMSTATE_DAMAGED:
        SetDoorStatus(door, eDoorStatus::DAMSTATE_OPENED_DAMAGED);
        break;
    case eDoorStatus::DAMSTATE_NOTPRESENT:
        //assert(0 && "Door should be present @ SetDoorOpen");
        break;
    }
}

/*!
* @notsa
* @brief Sets door open. Shouldn't be called if door isn't present (will assert in debug).
*/
void CDamageManager::SetDoorOpen_Component(tComponent door) {
    switch (GetDoorStatus_Component(door)) {
    case eDoorStatus::DAMSTATE_OK:
        SetDoorStatus_Component(door, eDoorStatus::DAMSTATE_OPENED);
        break;
    case eDoorStatus::DAMSTATE_DAMAGED:
        SetDoorStatus_Component(door, eDoorStatus::DAMSTATE_OPENED_DAMAGED);
        break;
    case eDoorStatus::DAMSTATE_NOTPRESENT:
        //assert(0 && "Door should be present @ SetDoorOpen_Component");
        break;
    }
}

/*!
* @notsa
* @brief Sets door closed. Shouldn't be called if door isn't present (will assert in debug).
*/
void CDamageManager::SetDoorClosed(eDoors door) {
    switch (GetDoorStatus(door)) {
    case eDoorStatus::DAMSTATE_OPENED:
        SetDoorStatus(door, eDoorStatus::DAMSTATE_OK);
        break;
    case eDoorStatus::DAMSTATE_OPENED_DAMAGED:
        SetDoorStatus(door, eDoorStatus::DAMSTATE_DAMAGED);
        break;
    case eDoorStatus::DAMSTATE_NOTPRESENT:
        //assert(0 && "Door should be present @ SetDoorClosed");
        break;
    }
}

/*!
* @notsa
* @brief Sets door closed. Shouldn't be called if door isn't present (will assert in debug).
*/
void CDamageManager::SetDoorClosed_Component(tComponent door) {
    switch (GetDoorStatus_Component(door)) {
    case eDoorStatus::DAMSTATE_OPENED:
        SetDoorStatus_Component(door, eDoorStatus::DAMSTATE_OK);
        break;
    case eDoorStatus::DAMSTATE_OPENED_DAMAGED:
        SetDoorStatus_Component(door, eDoorStatus::DAMSTATE_DAMAGED);
        break;
    case eDoorStatus::DAMSTATE_NOTPRESENT:
        //assert(0 && "Door should be present @ SetDoorClosed");
        break;
    }
}

// 0x6C2230
eDoorStatus CDamageManager::GetDoorStatus(eDoors nDoorIdx) const {
    switch (nDoorIdx) {
    case eDoors::DOOR_BONNET:
    case eDoors::DOOR_BOOT:
    case eDoors::DOOR_LEFT_FRONT:
    case eDoors::DOOR_RIGHT_FRONT:
    case eDoors::DOOR_LEFT_REAR:
    case eDoors::DOOR_RIGHT_REAR:
        return m_aDoorsStatus[(unsigned)nDoorIdx];
    default:
        return eDoorStatus::DAMSTATE_NOTPRESENT;
    }
}

// 0x6C26F0
eCarNodes CDamageManager::GetCarNodeIndexFromDoor(eDoors door) {
    switch (door) {
    case eDoors::DOOR_BONNET:
        return eCarNodes::CAR_BONNET;
    case eDoors::DOOR_BOOT:
        return eCarNodes::CAR_BOOT;
    case eDoors::DOOR_LEFT_FRONT:
        return eCarNodes::CAR_DOOR_LF;
    case eDoors::DOOR_RIGHT_FRONT:
        return eCarNodes::CAR_DOOR_RF;
    case eDoors::DOOR_LEFT_REAR:
        return eCarNodes::CAR_DOOR_LR;
    case eDoors::DOOR_RIGHT_REAR:
        return eCarNodes::CAR_DOOR_RR;
    default:
        return (eCarNodes)-1;
    }
}
