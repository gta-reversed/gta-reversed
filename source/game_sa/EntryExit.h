/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Rect.h"
#include "Vector.h"

class CObject;
class CPed;
class CPlayerPed;
class CEntryExit;

class CEntryExit {
public:
    enum eFlags {
        UNKNOWN_INTERIOR       = 1 << 0,
        UNKNOWN_PAIRING        = 1 << 1,
        CREATE_LINKED_PAIR     = 1 << 2,
        REWARD_INTERIOR        = 1 << 3,
        USED_REWARD_ENTRANCE   = 1 << 4,
        CARS_AND_AIRCRAFT      = 1 << 5,
        BIKES_AND_MOTORCYCLES  = 1 << 6,
        DISABLE_ON_FOOT        = 1 << 7,

        ACCEPT_NPC_GROUP       = 1 << 8,
        FOOD_DATE_FLAG         = 1 << 9,
        UNKNOWN_BURGLARY       = 1 << 10,
        DISABLE_EXIT           = 1 << 11,
        BURGLARY_ACCESS        = 1 << 12,
        ENTERED_WITHOUT_EXIT   = 1 << 13,
        ENABLE_ACCESS          = 1 << 14,
        DELETE_ENEX            = 1 << 15,
    };

    char    m_Name[8]{};
    CRect   m_EntranceRect{};
    float   m_EntranceZ{};
    float   m_EntranceAngleRad{};
    CVector m_ExitPos{};
    float   m_ExitAngle{}; //!< In degrees
    union {
        struct {
            uint16 bUnknownInterior : 1;     // 0x1
            uint16 bUnknownPairing : 1;      // 0x2
            uint16 bCreateLinkedPair : 1;    // 0x4
            uint16 bRewardInterior : 1;      // 0x8
            uint16 bUsedRewardEntrance : 1;  // 0x10
            uint16 bCarsAndAircraft : 1;     // 0x20
            uint16 bBikesAndMotorcycles : 1; // 0x40
            uint16 bDisableOnFoot : 1;       // 0x80

            uint16 bAcceptNpcGroup : 1;      // 0x100
            uint16 bFoodDateFlag : 1;        // 0x200
            uint16 bUnknownBurglary : 1;     // 0x400
            uint16 bDisableExit : 1;         // 0x800
            uint16 bBurglaryAccess : 1;      // 0x1000
            uint16 bEnteredWithoutExit : 1;  // 0x2000
            uint16 bEnableAccess : 1;        // 0x4000
            uint16 bDeleteEnex : 1;          // 0x8000
        };
        uint16 m_Flags{};
    };
    eAreaCodesS8 m_Area{};
    uint8        m_SkyColor{};
    uint8        m_TimeOn{};
    uint8        m_TimeOff{};
    uint8        m_NumberOfPeds{};
    CEntryExit*  m_Link{};

    static inline auto& ms_IsWarping = StaticRef<bool>(0x96A7B8);
    static inline auto& ms_Door = StaticRef<CObject*>(0x96A7BC);
    static inline auto& ms_SpawnPoint = StaticRef<CEntryExit*>(0x96A7C0);

public:
    static void InjectHooks();
    
    void WarpGangWithPlayer(CPlayerPed* plyr);

    CEntryExit(
        CVector center,
        CVector entranceRange,
        float entranceAngle,
        CVector exit,
        float exitAngle,
        eAreaCodes area,
        CEntryExit::eFlags flags,
        int32 skyColor,
        int32 timeOn, int32 timeOff,
        int32 numberOfPeds,
        const char* name
    );

    void GenerateAmbientPeds(const CVector& posn);
    CEntryExit* GetEntryExitToDisplayNameOf();
    void GetPositionRelativeToOutsideWorld(CVector& outPos);
    bool IsInArea(const CVector& position);
    void RequestAmbientPeds();
    void RequestObjectsInFrustum() const;
    bool TransitionFinished(CPed* ped);
    bool TransitionStarted(CPed* ped);
    void ProcessStealableObjects(CPed* ped);
    void FindValidTeleportPoint(CVector& outTeleportPoint);
    bool HasNameSet() const;

    // NOTSA

    // These 5 were probably inlined
    auto GetLinkedOrThis() { return m_Link ? m_Link : this; }
    auto GetLinkedOrThis() const { return m_Link ? m_Link : this; }
    bool IsVisibleByTime() const;

    // Most likely didn't exist in this form.
    CMatrix GetRectEntranceMatrix() const;

    // This possibly existed in some form.
    // `IsInArea` has some transformation code Which is very similar to the one in `CEnExManager::AddOne`
    CVector TransformEntrancePoint(const CVector& point) const;
    [[nodiscard]] const CRect& GetEntranceRect() const;
    [[nodiscard]] CVector GetPosition() const;
    [[nodiscard]] CVector2D GetPosition2D() const;
    [[nodiscard]] uint8 GetMyOrLinkedArea() const;
    [[nodiscard]] auto GetArea() const { return m_Area; }
};
VALIDATE_SIZE(CEntryExit, 0x3C);
