#pragma once

#include "AnimationEnums.h"

// based on https://projectcerbera.com/gta/sa/tutorials/handling#animation

// If the flags are true, second anim group is used, otherwise first
union sVehicleAnimDataGroupFlags {
    uint32 intValue;
    struct {
        uint32 bAlign : 1; // Unknown stuff
        uint32 bOpenFrontDoorsOnExit : 1;
        uint32 bOpenRearDoorsOnExit : 1;
        uint32 bCanEnterFrontDoors : 1;
        uint32 bCanEnterRearDoors : 1;
        uint32 bCanPulloutPed : 1;
        uint32 bCloseFrontDoorWhenInside : 1;
        uint32 bCloseRearDoorWhenInside : 1;

        uint32 bShuffle : 1; // Unkn purpose
        uint32 bCanExitFrontDoors : 1;
        uint32 bCanExitRearDoors : 1;
        uint32 bPlayerCanBeJacked : 1;
        uint32 bCloseFrontDoorWhenOutside : 1;
        uint32 bCloseRearDoorWhenOutside : 1;
        uint32 bCanJumpOutOfVehicle : 1;
        uint32 bRollDownWindowOnDoorClose : 1;

        uint32 bPlayerCanFallOut : 1;
        uint32 bDoorLocked : 1;
    };

public:
    sVehicleAnimDataGroupFlags() {
        intValue = 0;
    }
};
VALIDATE_SIZE(sVehicleAnimDataGroupFlags, 0x4);

union sVehicleAnimDataSpecialFlags {
    uint32 intValue;
    struct {
        uint32 bDontCloseDoorAfterGettingOut : 1;
        uint32 bDontCloseDoorAfterGettingIn : 1;
        uint32 bUseKartDriveAnims : 1;
        uint32 bUseTruckDriveAnims : 1;
        uint32 bUseHovercraftDriveAnims : 1;
        uint32 bRunSpecialLockedDoor : 1; // Unknown
        uint32 bDontOpenDoorWhenGettingIn : 1;
    };

public:
    sVehicleAnimDataSpecialFlags() {
        intValue = 0;
    }
};
VALIDATE_SIZE(sVehicleAnimDataSpecialFlags, 0x4);

struct sVehAnimGroupGeneralTiming {
    float m_fGetInTime;
    float m_fJumpOutTime;
    float m_fGetOutTime;
    float m_fCarJackTime;
    float m_fFallOutTime;

public:
    sVehAnimGroupGeneralTiming() : m_fGetInTime(0.0F), m_fJumpOutTime(0.0F), m_fGetOutTime(0.0F), m_fCarJackTime(0.0F), m_fFallOutTime(0.0F) {}
};

enum eInOutTimingMode : uint8 {
    // TODO: Enum names are incorrect, but don't bother, this enum shit should be removed anyways.
    OPEN_START = 0,
    OPEN_STOP = 1,
    CLOST_START = 2,
    CLOSE_STOP = 3,
};

struct sVehAnimGroupInOutTiming { // TODO: Rename to `sVehAnimGroupOpenCloseTiming`
    float OpenOut{};
    float CloseIn{};
    float OpenIn{};
    float CloseOut{};
public:
    float operator[](eInOutTimingMode mode) { return reinterpret_cast<float*>(this)[(size_t)mode]; } // TODO: Get rid of this

    sVehAnimGroupInOutTiming() = default;
};

enum eInOutTiming : uint8 {
    TIMING_START = 0,
    TIMING_END = 1,
};

enum eVehAnimDoorOffset : uint8 {
    ENTER_FRONT = 0,
    ENTER_REAR = 1,
    ENTER_BIKE_FRONT = 2,
    EXIT_FRONT = 3,
    EXIT_REAR = 4,
    JACK_PED_LEFT = 5,
    JACK_PED_RIGHT = 6,
};

// Most of the code was inlined in final exe, in android IDB 99% of the code is inlined,
// on PC version some methods are left, but without symbols the names aren't matching original source most likely
class CVehicleAnimGroup {
public:
    CVehicleAnimGroup();

public:
    uint8 m_ucFirstGroup;  // see AssocGroupId
    uint8 m_ucSecondGroup; // see AssocGroupId
    char  _pad[2];

    AssocGroupId GetFirstGroup()  const { return (AssocGroupId)m_ucFirstGroup; }
    AssocGroupId GetSecondGroup() const { return (AssocGroupId)m_ucSecondGroup; }

public:
    sVehicleAnimDataGroupFlags   m_animFlags;
    sVehicleAnimDataSpecialFlags m_specialFlags;
    sVehAnimGroupGeneralTiming   m_generalTiming;
    sVehAnimGroupInOutTiming     m_aInOutTiming[2];    // Access using GetInOutTiming()
    CVector                      m_aVecDoorOffsets[7]; // Access using GetDoorOffset()

public:
    static void InjectHooks();

    void         InitAnimGroup(uint8 firstGroup, uint8 secondGroup, int32 animFlags, int32 animSpecialFlags, sVehAnimGroupGeneralTiming* generalTiming, sVehAnimGroupInOutTiming* startTiming, sVehAnimGroupInOutTiming* endTiming);
    void         CopyAnimGroup(CVehicleAnimGroup* src);
    AssocGroupId GetGroup(AnimationId animId) const;
    float        ComputeCriticalBlendTime(AnimationId animId);
    CVector      ComputeAnimDoorOffsets(eVehAnimDoorOffset doorId);

    /// NOTSA: Helper of `CVehicleAnimGroupData::InitAGroupFromData`
    int32 InitFromData(const char* line);
public:
    // Helpers
    sVehAnimGroupInOutTiming& GetInOutTiming(const eInOutTiming timing) { return m_aInOutTiming[timing]; }
    CVector& GetDoorOffset(const eVehAnimDoorOffset door) { return m_aVecDoorOffsets[door]; }
};
VALIDATE_SIZE(CVehicleAnimGroup, 0x94);

class CVehicleAnimGroupData {
public:
    static constexpr int32 NUM_VEH_ANIM_GROUPS = 30;
    static CVehicleAnimGroup (&m_vehicleAnimGroups)[NUM_VEH_ANIM_GROUPS]; // Access using GetVehicleAnimGroup()

public:
    static void InjectHooks();

    static void         GetInOutTimings(AssocGroupId groupId, eInOutTimingMode mode, float* pfAnimStart, float* pfAnimEnd);
    static AssocGroupId GetGroupForAnim(AssocGroupId groupId, AnimationId animId);
    static CVector      GetAnimDoorOffset(AssocGroupId groupId, eVehAnimDoorOffset doorId);
    static float        ComputeCriticalBlendTime(AssocGroupId groupId, AnimationId animId);

    static bool UsesTruckDrivingAnims(AssocGroupId groupId);
    static bool UsesKartDrivingAnims(AssocGroupId groupId);
    static bool UsesHovercraftDrivingAnims(AssocGroupId groupId);

    /// Helper function for `cHandlingDataMgr::LoadHandlingData`
    static int32 LoadAGroupFromData(const char* line);
public:
    // Helpers
    inline static CVehicleAnimGroup& GetVehicleAnimGroup(int32 iGroup) {
        return m_vehicleAnimGroups[iGroup];
    }
};
