#pragma once
#include <array>
#include <concepts>

#include "CompressedMatrixNotAligned.h"
#include "Vector.h"
#include "Matrix.h"
#include "eReplay.h"

class CVehicle;
class CBike;
class CPlane;
class CHeli;
class CBmx;
class CTrain;

#pragma pack(push, 1)
struct AnimationState {
    int16 m_nAnimId;
    uint8 m_nTime;
    uint8 m_nSpeed;
    uint8 m_nGroupId1;
    uint8 m_nGroupId2;

    static AnimationState Make(int16 animId, float time, float speed, uint8 groupId) {
        AnimationState ret{};
        ret.m_nAnimId   = animId;
        ret.m_nTime     = static_cast<uint8>(std::clamp(time, 0.0f, 4.0f)  * 63.75f);
        ret.m_nSpeed    = static_cast<uint8>(std::clamp(speed, 0.0f, 3.0f) * 85.0f);
        ret.m_nGroupId1 = groupId;

        return ret;
    }

    static AnimationState MakeBlend(int16 animId, float time, float speed, uint8 groupId, float blend) {
        AnimationState ret{};
        ret.m_nAnimId   = animId;
        ret.m_nTime     = static_cast<uint8>(std::clamp(time, 0.0f, 4.0f)  * 63.75f);
        ret.m_nSpeed    = static_cast<uint8>(std::clamp(speed, 0.0f, 3.0f) * 85.0f);
        ret.m_nGroupId1 = static_cast<uint8>(std::clamp(blend, 0.0f, 2.0f) * 127.5f);
        ret.m_nGroupId2 = groupId;

        return ret;
    }
};
#pragma pack(pop)
VALIDATE_SIZE(AnimationState, 0x6);

/*
struct CStoredAnimationState {
    AnimationState first;
    AnimationState second;
    AnimationState third;
};
*/
using CStoredAnimationState = std::array<AnimationState, 3>;
VALIDATE_SIZE(CStoredAnimationState, 0x12);

#pragma pack(push, 1)
struct tReplayBlockBase {
    eReplayPacket type;

    template <class T>
        requires std::derived_from<T, tReplayBlockBase>
    T* As() {
        return reinterpret_cast<T*>(this);
    }
};
VALIDATE_SIZE(tReplayBlockBase, 0x1);

struct tReplayEndBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_END;
    /* nil */
};

struct tReplayVehicleBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_VEHICLE;

    uint8 poolRef;
    uint8 health;
    uint8 gasPedal;
    CCompressedMatrixNotAligned matrix;
    uint8 angleDoorLF;
    uint8 angleDoorRF;
    uint16 modelId;
    union {
        uint32 panels; // Index with ePanels
        struct {
            uint32 frontLeft : 4;
            uint32 frontRight : 4;
            uint32 rearLeft : 4;
            uint32 rearRight : 4;
            uint32 windscreen : 4;
            uint32 frontBumper : 4;
            uint32 rearBumper : 4;
        } panelStates;
        struct {
            /* unknown, bitfields of 2 */
        } aeroplanePanelStates;
    };
    struct {
        int8 x, y, z;
    } vecMoveSpeed;
    uint8 steerAngle_or_doomVerticalRot; // m_fDoomVerticalRotation if modelId == RHINO, steerAngle if not.
    uint8 wheelsSuspensionCompression[4];
    uint8 wheelRotation[4];
    uint8 doorStatus;
    uint8 primaryColor;
    uint8 secondaryColor;
    uint8 physicalFlags;
    uint8 vehicleSubType;
    uint8 vehicleType;
    uint8 __pad[2];

    static tReplayVehicleBlock MakeVehicleUpdateData(CVehicle& veh, int32 poolIdx);
    void ExtractVehicleUpdateData(CVehicle& veh, float interpolation);
};

struct tReplayBikeBlock : tReplayVehicleBlock {
    static constexpr auto Type = REPLAY_PACKET_BIKE;

    uint8 animLean;
    uint8 steerAngle;
    uint8 __pad[2];

    static tReplayBikeBlock MakeBikeUpdateData(CBike& bike, int32 poolIdx);
    void ExtractBikeUpdateData(CBike& bike, float interpolation);
};

struct tReplayPedHeaderBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_PED_HEADER;

    uint8 poolRef;
    int16 modelId;
    uint8 pedType;
    uint8 __pad[3];
};

struct tReplayPedUpdateBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_PED_UPDATE;

    uint8 poolRef;
    int8 heading;
    uint8 vehicleIndex; // 0: not in a vehicle
    CStoredAnimationState animState;
    uint8 __pad1[2];
    CCompressedMatrixNotAligned matrix;
    int16 weaponModel;
    uint16 animGroup;
    uint8 contactSurfaceBrightness;
    struct {
        bool isTalking : 1;
        bool stillOnValidPoly : 1;
        bool usesCollision : 1;
    } flags;
    uint8 __pad2[2];
};

struct tReplayCameraBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_GENERAL;

    bool isUsingRemoteVehicle;
    uint8 __pad[2];
    uint8 matrix[sizeof(CMatrix)]; // Use GetMatrix() to access
    CVector firstFocusPosn;

    // TODO/FIXME: Remove this func and properly handle CMatrix element
    CMatrix& GetMatrix() {
        return *(CMatrix*)&matrix;
    }
};

struct tReplayClockBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_CLOCK;

    uint8 currentHour;
    uint8 currentMinute;
    uint8 __pad;
};

struct tReplayWeatherBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_WEATHER;

    uint8 oldWeather;
    uint8 newWeather;
    uint8 __pad;
    float interpValue;
};

// End of frame
struct tReplayEOFBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_END_OF_FRAME;

private:
    uint8 do_not_access[3];
};

struct tReplayTimerBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_TIMER;

    uint8 __pad[3];
    uint32 timeInMS;
};

struct tReplayBulletTraceBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_BULLET_TRACES;

    uint8 __pad[2];
    uint8 index;
    CVector start;
    CVector end;
};

// Unused
struct tReplayParticleBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_PARTICLE;

    uint8 unk[19];
};

struct tReplayMiscBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_MISC;

    uint8 __pad1[3];
    uint32 camShakeStart;
    float camShakeForce;
    uint8 currArea;
    struct {
        bool videoCam : 1;
        bool liftCam : 1;
    } camConfig;
    uint8 __pad2[2];
};

struct tReplayDeletedVehicleBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_DELETED_VEH;

    uint8 __pad;
    int16 poolRef;
};

struct tReplayDeletedPedBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_DELETED_PED;

    uint8 __pad;
    int16 poolRef;
};

// same as tReplayBikeBlock
struct tReplayBmxBlock : tReplayVehicleBlock {
    static constexpr auto Type = REPLAY_PACKET_BMX;

    uint8 animLean;
    uint8 steerAngle;
    uint8 __pad[2];

    static tReplayBmxBlock MakeBmxUpdateData(CBmx& bmx, int32 poolIdx);
};

struct tReplayHeliBlock : tReplayVehicleBlock {
    static constexpr auto Type = REPLAY_PACKET_HELI;

    float rotorSpeed;
};

struct tReplayPlaneBlock : tReplayVehicleBlock {
    static constexpr auto Type = REPLAY_PACKET_PLANE;

    float propSpeed;
    float field_9C8;
};

struct tReplayTrainBlock : tReplayVehicleBlock {
    static constexpr auto Type = REPLAY_PACKET_TRAIN;

    float trainSpeed;
    float currentRailDistance;
    float length;
    uint32 prevCarriageRef;
    uint32 nextCarriageRef;
    uint8 trackId;
    uint8 __pad[3];

    static tReplayTrainBlock MakeTrainUpdateData(CTrain& train, int32 poolIdx);
    void ExtractTrainUpdateData(CTrain& train, float interpolation);
};

struct tReplayClothesBlock : tReplayBlockBase {
    static constexpr auto Type = REPLAY_PACKET_CLOTHES;

    uint8 __pad[3];
    std::array<uint32, 10> modelKeys;
    std::array<uint32, 18> textureKeys;
    uint16 fatStat;    // compressed float
    uint16 muscleStat; // compressed float
};
#pragma pack(pop)
