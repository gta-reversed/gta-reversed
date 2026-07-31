#pragma once

#include <extensions/Shapes/AngledRect.hpp>
#include <extensions/Shapes/AngledBox.hpp>

#include "AEDoorAudioEntity.h"
#include "StoredCar.h"

enum eGarageType : uint8 {
    INVALID = 0,
    ONLY_TARGET_VEH = 1,
    BOMBSHOP_TIMED = 2,
    BOMBSHOP_ENGINE = 3,
    BOMBSHOP_REMOTE = 4,
    PAYNSPRAY = 5,
    COLLECTORSITEMS = 6,
    COLLECTSPECIFICCARS = 7,
    COLLECTCARS_1 = 8,
    COLLECTCARS_2 = 9,
    COLLECTCARS_3 = 10,
    UNKN_CLOSESONTOUCH = 11,
    SIXTY_SECONDS = 12,
    CRUSHER = 13,
    OPEN_FOR_TARGET_FREEZE_PLAYER = 14,
    SCRIPT_ONLY_OPEN = 15,

    SAFEHOUSE_GANTON = 16,
    SAFEHOUSE_SANTAMARIA = 17,
    SAGEHOUSE_ROCKSHORE = 18,

    SCRIPT_CONTROLLED = 19,
    STAY_OPEN_WITH_CAR_INSIDE = 20,
    CLOSE_WITH_CAR_DONT_OPEN_AGAIN = 21,
    COLLECTCARS_4 = 22,
    SCRIPT_OPEN_FREEZE_WHEN_CLOSING = 23,

    SAFEHOUSE_FORTCARSON = 24,
    SAFEHOUSE_VERDANTMEADOWS = 25,
    SAFEHOUSE_DILLIMORE = 26,
    SAFEHOUSE_PRICKLEPINE = 27,
    SAFEHOUSE_WHITEWOOD = 28,
    SAFEHOUSE_PALOMINOCREEK = 29,
    SAFEHOUSE_REDSANDSWEST = 30,
    SAFEHOUSE_ELCORONA = 31,
    SAFEHOUSE_MULHOLLAND = 32,

    IMPOUND_LS = 33,
    IMPOUND_SF = 34,
    IMPOUND_LV = 35,

    TUNING_LOCO_LOW_CO = 36,
    TUNING_WHEEL_ARCH_ANGELS = 37,
    TUNING_TRANSFENDER = 38,

    SAFEHOUSE_CALTONHEIGHTS = 39,
    SAFEHOUSE_PARADISO = 40,
    SAFEHOUSE_DOHERTY = 41,
    SAFEHOUSE_HASHBURY = 42,

    BURGLARY = 43,

    HANGAR_AT400 = 44,
    HANGAR_ABANDONED_AIRPORT = 45
};

inline const char* EnumToString(eGarageType gt) {
    #define ECASE(e) case eGarageType::e: return #e
    switch (gt) {
    default:
    ECASE(INVALID);
    ECASE(ONLY_TARGET_VEH);
    ECASE(BOMBSHOP_TIMED);
    ECASE(BOMBSHOP_ENGINE);
    ECASE(BOMBSHOP_REMOTE);
    ECASE(PAYNSPRAY);
    ECASE(COLLECTORSITEMS);
    ECASE(COLLECTSPECIFICCARS);
    ECASE(COLLECTCARS_1);
    ECASE(COLLECTCARS_2);
    ECASE(COLLECTCARS_3);
    ECASE(UNKN_CLOSESONTOUCH);
    ECASE(SIXTY_SECONDS);
    ECASE(CRUSHER);
    ECASE(OPEN_FOR_TARGET_FREEZE_PLAYER);
    ECASE(SCRIPT_ONLY_OPEN);

    ECASE(SAFEHOUSE_GANTON);
    ECASE(SAFEHOUSE_SANTAMARIA);
    ECASE(SAGEHOUSE_ROCKSHORE);

    ECASE(SCRIPT_CONTROLLED);
    ECASE(STAY_OPEN_WITH_CAR_INSIDE);
    ECASE(CLOSE_WITH_CAR_DONT_OPEN_AGAIN);
    ECASE(COLLECTCARS_4);
    ECASE(SCRIPT_OPEN_FREEZE_WHEN_CLOSING);

    ECASE(SAFEHOUSE_FORTCARSON);
    ECASE(SAFEHOUSE_VERDANTMEADOWS);
    ECASE(SAFEHOUSE_DILLIMORE);
    ECASE(SAFEHOUSE_PRICKLEPINE);
    ECASE(SAFEHOUSE_WHITEWOOD);
    ECASE(SAFEHOUSE_PALOMINOCREEK);
    ECASE(SAFEHOUSE_REDSANDSWEST);
    ECASE(SAFEHOUSE_ELCORONA);
    ECASE(SAFEHOUSE_MULHOLLAND);

    ECASE(IMPOUND_LS);
    ECASE(IMPOUND_SF);
    ECASE(IMPOUND_LV);

    ECASE(TUNING_LOCO_LOW_CO);
    ECASE(TUNING_WHEEL_ARCH_ANGELS);
    ECASE(TUNING_TRANSFENDER);

    ECASE(SAFEHOUSE_CALTONHEIGHTS);
    ECASE(SAFEHOUSE_PARADISO);
    ECASE(SAFEHOUSE_DOHERTY);
    ECASE(SAFEHOUSE_HASHBURY);

    ECASE(BURGLARY);
    ECASE(HANGAR_AT400);
    ECASE(HANGAR_ABANDONED_AIRPORT);
    }
    #undef ECASE
}

enum eGarageDoorState : uint8 {
    GARAGE_DOOR_CLOSED = 0,
    GARAGE_DOOR_OPEN = 1,
    GARAGE_DOOR_CLOSING = 2,
    GARAGE_DOOR_OPENING = 3,
    GARAGE_DOOR_WAITING_PLAYER_TO_EXIT = 4,
    GARAGE_DOOR_CLOSED_DROPPED_CAR = 5,
    GARAGE_DOOR_CRUSHING = 6,
};

/*!
*  [C]                   [A]
*   +---------------------+ 
*   |                    /|\
*   |                     |
*   |                  [dir A]
*   |                     |
*   |                    /|\
*   |                     |
*   +<-----<[dir B]<------+
* [B]                    [base]
* Side lengths are lenA and lenB [not called height and width, because that would depend on the orientation of it]
*/
class CGarage {
public:
    // https://gtamods.com/wiki/GRGE#Garage_flags
    enum eIPLFlags {
        IPL_FLAG_DOOR_UP_AND_ROTATE = 1 << 0,
        IPL_FLAG_DOOR_GOES_IN       = 1 << 1,
        IPL_FLAG_CAM_FOLLOW_PLAYER  = 1 << 2,
    };

public:
    CVector          m_Base{};
    CVector2D        m_DirA{};
    CVector2D        m_DirB{};
    float            m_CeilingZ{};
    float            m_LenA{};
    float            m_LenB{};
    float            m_MinX{};
    float            m_MaxX{};
    float            m_MinY{};
    float            m_MaxY{};
    float            m_DoorOpenness{};
    uint32           m_TimeToOpen{};
    CVehicle*        m_TargetCar{};
    char             m_Name[8]{};
    eGarageType      m_Type{};
    eGarageDoorState m_DoorState{};
    union {
        uint8 m_nFlags;
        struct {
            uint8 m_bClosingEmpty : 1; // Was anything inside the garage when it's door started closing
            uint8 m_bInactive : 1;
            uint8 m_bUsedRespray : 1;
            uint8 m_bDoorOpensUp : 1;
            uint8 m_bDoorGoesIn : 1;
            uint8 m_bCameraFollowsPlayer : 1;
            uint8 m_bDoorClosed : 1;
            uint8 m_bRespraysAlwaysFree : 1;
        };
    };
    eGarageType        m_OriginalType{};
    CAEDoorAudioEntity m_GarageAudio{};

public:
    static constexpr auto NUM_GARAGE_STORED_CARS{ 4u };

    static void InjectHooks();

    CGarage(
        CVector base,
        CVector2D p1,
        CVector2D p2,
        float ceilingZ,
        eGarageType type,
        uint32,
        const char* name,
        uint32 flagsIPL // See eIPLFlags
    );

    CGarage() = default;  // 0x4470E0
    ~CGarage() = default; // 0x447110

    void TidyUpGarageClose();
    void TidyUpGarage();
    void StoreAndRemoveCarsForThisHideOut(CStoredCar* car, int32 maxSlot = NUM_GARAGE_STORED_CARS);
    void RemoveCarsBlockingDoorNotInside();
    bool IsEntityTouching3D(CEntity* entity);
    bool IsEntityEntirelyOutside(CEntity* entity, float tolerance = 0.f);
    bool IsPlayerOutsideGarage(float tolerance = 0.f);

    bool IsEntityEntirelyInside3D(CEntity* entity, float tolerance = 0.f);
    bool IsStaticPlayerCarEntirelyInside();
    bool IsPlayerEntirelyInsideGarage();

    bool IsPointInsideGarage(CVector point);
    bool IsPointInsideGarage(CVector point, float radius);
    bool IsSphereInsideGarage(const CSphere& sp) { return IsPointInsideGarage(sp.m_vecCenter, sp.m_fRadius); }
    void PlayerArrestedOrDied();
    void OpenThisGarage();
    void CloseThisGarage();
    void UpdatePlayerCameraStuff();
    void InitDoorsAtStart();
    void Update(int32 garageId);

    bool RightModTypeForThisGarage(CVehicle* vehicle);
    float CalcDistToGarageRectangleSquared(float, float);
    void NeatlyLineUpStoredCars(CStoredCar* cars);
    bool RestoreCarsForThisHideOut(CStoredCar* storedCars);

    bool RestoreCarsForThisImpoundingGarage(CStoredCar* car);
    void StoreAndRemoveCarsForThisImpoundingGarage(CStoredCar* storedCars, uint32 numCarsToStore);

    int32 FindMaxNumStoredCarsForGarage();
    bool EntityHasASphereTest(CEntity* entity, bool inside, float tolerance = 0.f);
    bool EntityHasASphereWayOutsideGarage(CEntity* entity, float tolerance = 0.f);
    bool EntityHasSphereInsideGarage(CEntity* entity, float tolerance = 0.f);
    bool EntityHasSphereOutsideGarage(CEntity* entity, float tolerance = 0.f) { return EntityHasASphereWayOutsideGarage(entity, tolerance); }
    bool IsAnyOtherCarTouchingGarage(CVehicle* ignoredVehicle = nullptr);
    void ThrowCarsNearDoorOutOfGarage(CVehicle* ignoredVehicle);
    bool IsAnyOtherPedTouchingGarage(CPed* ignoredVehicle);
    bool IsAnyCarBlockingDoor();
    size_t CountCarsWithCenterPointWithinGarage(CVehicle* ignoredVehicle);
    void CenterCarInGarage(CVehicle* vehicle);
    void FindDoorsWithGarage(CObject*& door1, CObject*& door2);
    bool SlideDoorOpen();
    bool SlideDoorClosed();
    bool IsGarageEmpty();
    float CalcDistToGarageRectangleSquared(CVector2D pos);

    void ChangeType(eGarageType newType);
    void Activate();
    void DeActivate();

    static void BuildRotatedDoorMatrix(CEntity* entity, float fDoorPosition);
    // NOTSA section
    [[nodiscard]] bool IsHideOut() const;
    [[nodiscard]] bool IsOpen(bool checkIsWaitingToExit = false) const { return m_DoorState == GARAGE_DOOR_OPEN || (checkIsWaitingToExit && m_DoorState == GARAGE_DOOR_WAITING_PLAYER_TO_EXIT); }
    [[nodiscard]] bool IsClosed() const { return m_DoorState == GARAGE_DOOR_CLOSED; }
    bool IsImpound() const;
    void SetOpened(bool open = true) { m_DoorState = open ? GARAGE_DOOR_OPEN : GARAGE_DOOR_CLOSED; }
    void SetClosed() { m_DoorState = GARAGE_DOOR_CLOSED; }
    void ResetDoorPosition() { m_DoorOpenness = 0.0f; } // todo: not good name

    //! Get offset to center [from the base]
    CVector2D GetCenterOffset() const;
    CVector2D GetCenter2D() const { return m_Base + GetCenterOffset(); }

    //! Represents the AABB of this garage
    CBoundingBox GetAABB() const;

    //! Represents the base of the garage in 2D
    notsa::shapes::AngledRect GetBaseAngledRect() const;

    //! Represents the base rect, but axis aligned in 2D
    CRect GetAARect() const;

    //! Represents the garage's actual bounding box
    notsa::shapes::AngledBox GetBB() const;

    bool IsVehicleTypeAllowedInside(CVehicle* veh);

    //! Only call this if garage is a safehouse
    CStoredCar* GetStoredCarsInThisSafehouse();
};

VALIDATE_SIZE(CGarage, 0xD8);

struct CSaveGarage {
    eGarageType      m_nType;
    eGarageDoorState m_nDoorState;
    uint8            m_nFlags;
    CVector          m_vPosn;
    CVector2D        m_vDirectionA, m_vDirectionB;
    float            m_fTopZ;
    float            m_fWidth, m_fHeight;
    float            m_fLeftCoord;
    float            m_fRightCoord;
    float            m_fFrontCoord;
    float            m_fBackCoord;
    float            m_fDoorPosition;
    uint32           m_nTimeToOpen;
    char             m_anName[8];
    eGarageType      m_nOriginalType;

    void CopyGarageIntoSaveGarage(Const CGarage& garage);
    void CopyGarageOutOfSaveGarage(CGarage& garage) const;
};

VALIDATE_SIZE(CSaveGarage, 0x50);
