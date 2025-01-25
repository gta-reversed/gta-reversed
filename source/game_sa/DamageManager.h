/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "eCarWheel.h"
#include "eCarNodes.h"

class CAutomobile;

// Seems like there is some kind of generic damage state enum. look at gComponentDamageState
// and CShopping::StoreVehicleMods.
enum eDamageState : int8 {
    DAMAGE_STATE_OK      = 0,
    DAMAGE_STATE_NOT_OK1 = 1,
    DAMAGE_STATE_NOT_OK2 = 2,
    DAMAGE_STATE_NOT_OK3 = 3,
    DAMAGE_STATE_NOT_OK4 = 4
};

// TODO: Seems like these are the damage states for doors only.
//       Panel's seems to be different, more like: DAMSTATE_OK, DAMSTATE_DAMAGED, DAMSTATE_MISSING
// TODO(yukani): change prefix to something else?
enum ePanelDamageState : uint8 {
    DAMSTATE_OK             = 0, // Closed
    DAMSTATE_OPENED         = 1, // Open
    DAMSTATE_DAMAGED        = 2, // Closed
    DAMSTATE_OPENED_DAMAGED = 3, // Open
    DAMSTATE_NOTPRESENT     = 4  // Missing
};

// original name
enum tComponent : uint8 {
    COMPONENT_NA         = 0,
    COMPONENT_WHEEL_LF   = 1,
    COMPONENT_WHEEL_RF   = 2,
    COMPONENT_WHEEL_LR   = 3,
    COMPONENT_WHEEL_RR   = 4,
    COMPONENT_BONNET     = 5,
    COMPONENT_BOOT       = 6,
    COMPONENT_DOOR_LF    = 7,
    COMPONENT_DOOR_RF    = 8,
    COMPONENT_DOOR_LR    = 9,
    COMPONENT_DOOR_RR    = 10,
    COMPONENT_WING_LF    = 11,
    COMPONENT_WING_RF    = 12,
    COMPONENT_WING_LR    = 13,
    COMPONENT_WING_RR    = 14,
    COMPONENT_WINDSCREEN = 15,
    COMPONENT_BUMP_FRONT = 16,
    COMPONENT_BUMP_REAR  = 17,

    MAX_COMPONENTS = COMPONENT_BUMP_REAR /* cause it starts at 1, not 0 */
};

// original name
enum tComponentGroup : uint8 {
    COMPGROUP_PANEL  = 0,
    COMPGROUP_WHEEL  = 1,
    COMPGROUP_DOOR   = 2,
    COMPGROUP_BONNET = 3,
    COMPGROUP_BOOT   = 4,
    COMPGROUP_LIGHT  = 5,
    COMPGROUP_NA     = 6,

    MAX_COMGROUPS
};

enum eCarWheelStatus : uint8  {
    WHEEL_STATUS_OK = 0,
    WHEEL_STATUS_BURST,
    WHEEL_STATUS_MISSING,
    WHEEL_STATUS_3,
    WHEEL_STATUS_4
};

enum ePanels : uint8 {
    FRONT_LEFT_PANEL = 0,
    FRONT_RIGHT_PANEL,
    REAR_LEFT_PANEL,
    REAR_RIGHT_PANEL,
    WINDSCREEN_PANEL,
    FRONT_BUMPER,
    REAR_BUMPER,

    MAX_PANELS
};

enum eDoors : uint8  {
    DOOR_BONNET = 0,
    DOOR_BOOT,
    DOOR_LEFT_FRONT,
    DOOR_RIGHT_FRONT,
    DOOR_LEFT_REAR,
    DOOR_RIGHT_REAR,

    MAX_DOORS
};
using eDoorStatus = ePanelDamageState;

enum eLights : uint8 {
    LIGHT_FRONT_LEFT  = 0,
    LIGHT_FRONT_RIGHT = 1,
    LIGHT_REAR_RIGHT  = 2,
    LIGHT_REAR_LEFT   = 3,

    MAX_LIGHTS
};

enum eLightsState : uint8 {
    VEHICLE_LIGHT_OK      = 0x0,
    VEHICLE_LIGHT_SMASHED = 0x1,
};

class CDamageManager {
public:
    float           m_fWheelDamageEffect;
    uint8           m_nEngineStatus; // 0 - 250
    eCarWheelStatus m_anWheelsStatus[eCarWheel::MAX_CARWHEELS];
    eDoorStatus     m_aDoorsStatus[eDoors::MAX_DOORS];
    union {
        /* Great job R*, really could've just used an array here... Ends up to be the same size... */

        uint32 m_nLightsStatus;
        struct {
            uint32 leftFront : 2;
            uint32 rightFront : 2;
            uint32 rightRear : 2;
            uint32 leftRear : 2;
        } m_lightStates;
    };
    union {
        uint32 m_nPanelsStatus; // Index with ePanels
        struct {
            uint32 frontLeft : 4;
            uint32 frontRight : 4;
            uint32 rearLeft : 4;
            uint32 rearRight : 4;
            uint32 windscreen : 4;
            uint32 frontBumper : 4;
            uint32 rearBumper : 4;
        } m_panelStates;
        struct {
            /* unknown, bitfields of 2 */
        } m_aeroplanePanelStates;
    };

public:
    static void InjectHooks();

    CDamageManager(float wheelDamageEffect);

    void Init();
    void ResetDamageStatus();
    void ResetDamageStatusAndWheelDamage();
    void FuckCarCompletely(bool bDetachWheel);
    bool ApplyDamage(CAutomobile* vehicle, tComponent compId, float fIntensity, float fColDmgMult);

    // Set next level of damage to aero-plane component
    bool ProgressAeroplaneDamage(uint8 nFrameId);
    bool ProgressWheelDamage(eCarWheel wheel);
    bool ProgressPanelDamage(ePanels panel);
    void ProgressEngineDamage();
    bool ProgressDoorDamage(eDoors door, CAutomobile* pAuto);

    uint8 GetAeroplaneCompStatus(uint8 frame);
    void SetAeroplaneCompStatus(uint8 frame, ePanelDamageState status);

    uint32 GetEngineStatus();
    void SetEngineStatus(uint32 status);

    // There are 2 door functions, one takes `tComponent` the other `eDoors`.
    // To select the correct version to call look at the called function's address when you are REing code
    // And chose accordingly.
    [[nodiscard]] eDoorStatus GetDoorStatus_Component(tComponent nDoorIdx) const;
    void SetDoorStatus_Component(tComponent door, eDoorStatus status);

    [[nodiscard]] eDoorStatus GetDoorStatus(eDoors door) const;
    void SetDoorStatus(eDoors door, eDoorStatus status);

    [[nodiscard]] eCarWheelStatus GetWheelStatus(eCarWheel wheel) const;
    void SetWheelStatus(eCarWheel wheel, eCarWheelStatus status);

    [[nodiscard]] ePanelDamageState GetPanelStatus(ePanels panel) const;
    void SetPanelStatus(ePanels panel, ePanelDamageState status);

    [[nodiscard]] eLightsState GetLightStatus(eLights light) const;
    void SetLightStatus(eLights light, eLightsState status);

    // returns -1 if no node for this panel
    static eCarNodes GetCarNodeIndexFromPanel(ePanels panel);
    static eCarNodes GetCarNodeIndexFromDoor(eDoors door);
    static bool GetComponentGroup(tComponent nComp, tComponentGroup& outCompGroup, uint8& outComponentRelativeIdx);

    // NOTSA
    void SetAllWheelsState(eCarWheelStatus state);
    void SetDoorStatus(std::initializer_list<eDoors> doors, eDoorStatus status); 
    void SetDoorOpen(eDoors door);
    void SetDoorOpen_Component(tComponent door);
    void SetDoorClosed(eDoors door);
    void SetDoorClosed_Component(tComponent door);
    [[nodiscard]] auto GetAllLightsState() const -> std::array<eLightsState, 4>;
    [[nodiscard]] auto GetAllDoorsStatus() const -> std::array<eDoorStatus, MAX_DOORS>;
    [[nodiscard]] bool IsDoorOpen(eDoors door) const;
    [[nodiscard]] bool IsDoorClosed(eDoors door) const;
    [[nodiscard]] bool IsDoorPresent(eDoors door) const;
    [[nodiscard]] bool IsDoorDamaged(eDoors door) const;

private: 
    // Wrapper functions

    // Return type has to be uint32, but we want to return `eCarWheelStatus` in our code.
    uint32 GetWheelStatus_Hooked(eCarWheel wheel) { return (uint32)GetWheelStatus(wheel); }
};

VALIDATE_SIZE(CDamageManager, 0x18);
