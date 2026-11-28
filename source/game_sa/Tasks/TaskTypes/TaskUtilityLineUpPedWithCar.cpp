#include "StdInc.h"

#include "TaskUtilityLineUpPedWithCar.h"
#include "VehicleAnimGroupData.h"
#include "PedPlacement.h"
#include "rtslerp.h"

namespace {
//! Critical blend times (of the vehicle anim group) with an absolute value from this on are special
constexpr float SPECIAL_CRITICAL_BLEND_TIME = 10.f;

//! The align anims use a special critical blend time as a normal one after taking this away
constexpr float SPECIAL_CRITICAL_BLEND_TIME_BASE = 11.f;

//! Up to this anim progress the ped takes over the vehicle's whole orientation, after it only its own heading
constexpr float MAX_ANIM_PROGRESS_TO_USE_VEHICLE_ORIENTATION = 0.2f;

//! @return How much of the blend is done at `time`: Before the critical time if it's positive, after it otherwise
float CalculateBlend(float time, float criticalTime, float absCriticalTime) {
    if (criticalTime > 0.f) {
        return time >= absCriticalTime
            ? 1.f
            : time / absCriticalTime;
    }
    return time >= absCriticalTime
        ? (time - absCriticalTime) / (1.f - absCriticalTime)
        : 0.f;
}

//! @return The offset type of the door, `type` if it has none of its own
eVehAnimDoorOffset GetOffsetTypeOfDoor(eTargetDoor door, eVehAnimDoorOffset type) { // 0x64fdf9
    switch (door) {
    case TARGET_DOOR_DRIVER:
    case TARGET_DOOR_FRONT_RIGHT:
        return ENTER_FRONT;
    case TARGET_DOOR_REAR_LEFT:
    case TARGET_DOOR_REAR_RIGHT:
        return ENTER_REAR;
    case TARGET_DOOR_UNK:
        return ENTER_BIKE_FRONT;
    default:
        return type;
    }
}

//! @return The door offset type to use for the anim
eVehAnimDoorOffset GetDoorOffsetType(eTargetDoor door, CAnimBlendAssociation* assoc) {
    if (!assoc) {
        return GetOffsetTypeOfDoor(door, ENTER_FRONT);
    }
    switch (const auto animId = assoc->GetAnimId()) {
    case ANIM_ID_CAR_ALIGN_LHS: // 0x64fc64
    case ANIM_ID_CAR_ALIGN_RHS:
    case ANIM_ID_CAR_ALIGNHI_LHS:
    case ANIM_ID_CAR_ALIGNHI_RHS:
    case ANIM_ID_CAR_OPEN_LHS:
    case ANIM_ID_CAR_OPEN_RHS:
    case ANIM_ID_CAR_OPEN_LHS_1:
    case ANIM_ID_CAR_OPEN_RHS_1:
    case ANIM_ID_CAR_GETIN_LHS_0:
    case ANIM_ID_CAR_GETIN_RHS_0:
    case ANIM_ID_CAR_GETIN_LHS_1:
    case ANIM_ID_CAR_GETIN_RHS_1:
    case ANIM_ID_CAR_GETIN_BIKE_FRONT:
    case ANIM_ID_CAR_PULLOUT_LHS:
    case ANIM_ID_CAR_PULLOUT_RHS:
    case ANIM_ID_UNKNOWN_15:
    case ANIM_ID_CAR_CLOSEDOOR_LHS_0:
    case ANIM_ID_CAR_CLOSEDOOR_RHS_0:
    case ANIM_ID_CAR_CLOSEDOOR_LHS_1:
    case ANIM_ID_CAR_CLOSEDOOR_RHS_1:
    case ANIM_ID_CAR_SHUFFLE_RHS_0:
    case ANIM_ID_CAR_SHUFFLE_RHS_1:
    case ANIM_ID_CAR_DOORLOCKED_LHS:
    case ANIM_ID_CAR_DOORLOCKED_RHS:
        return GetOffsetTypeOfDoor(door, ENTER_FRONT);
    case ANIM_ID_DEFAULT_CAR_CRAWLOUTRHS_0: // 0x64fdef
    case ANIM_ID_DEFAULT_CAR_CRAWLOUTRHS_1:
    case ANIM_ID_CAR_GETOUT_LHS_0:
    case ANIM_ID_CAR_GETOUT_RHS_0:
    case ANIM_ID_CAR_GETOUT_LHS_1:
    case ANIM_ID_CAR_GETOUT_RHS_1:
    case ANIM_ID_CAR_CLOSE_LHS_0:
    case ANIM_ID_CAR_CLOSE_RHS_0:
    case ANIM_ID_CAR_CLOSE_LHS_1:
    case ANIM_ID_CAR_CLOSE_RHS_1:
    case ANIM_ID_CAR_FALLOUT_LHS:
    case ANIM_ID_CAR_FALLOUT_RHS:
        return GetOffsetTypeOfDoor(door, EXIT_FRONT);
    case ANIM_ID_CAR_JACKEDLHS: // 0x64fdc9
    case ANIM_ID_CAR_JACKEDRHS:
        return ENTER_FRONT;
    default: // 0x64fca2
        // NOTE/BUG: The original leaves the offset type uninitialized for every other anim (`ANIM_ID_CAR_ROLLOUT_LHS`, `ANIM_ID_CAR_ROLLDOOR`, ...):
        // The compiled code uses the vehicle's address as the type, so `ComputeAnimDoorOffsets` reads far out of bounds.
        NOTSA_UNREACHABLE_CASE(animId);
    }
}

//! @return The position of the seat and the offset of the door from it
std::pair<CVector, CVector> GetSeatPosAndDoorOffset(eTargetDoor door, const CVector& frontSeat, const CVector& rearSeat, float seatOffset, const CVector& animOffset) {
    switch (door) {
    case TARGET_DOOR_FRONT_RIGHT: // 0x64feba
        return {
            CVector{ frontSeat.x + seatOffset, frontSeat.y,  frontSeat.z  },
            CVector{ -animOffset.x,            animOffset.y, animOffset.z }
        };
    case TARGET_DOOR_REAR_RIGHT: // 0x64ff1a
        return {
            CVector{ rearSeat.x + seatOffset, rearSeat.y,   rearSeat.z   },
            CVector{ -animOffset.x,           animOffset.y, animOffset.z }
        };
    case TARGET_DOOR_DRIVER: // 0x64fe92
        return {
            CVector{ -(frontSeat.x + seatOffset), frontSeat.y, frontSeat.z },
            animOffset
        };
    case TARGET_DOOR_REAR_LEFT: // 0x64fed1
        return {
            CVector{ -(rearSeat.x + seatOffset), rearSeat.y, rearSeat.z },
            animOffset
        };
    case TARGET_DOOR_UNK: // 0x64ff71
        return { frontSeat, animOffset };
    default: // 0x64ffb3
        return { frontSeat, CVector{} };
    }
}

//! @return How far out of the seat the ped is (1 = at the door) and how much its height is blended away from the height at the door
std::pair<float, float> GetAnimProgressAndHeightBlend(CVehicle* vehicle, CAnimBlendAssociation* assoc) {
    if (!assoc) {
        return { 0.f, 0.f };
    }
    const auto  animId       = assoc->GetAnimId();
    const float animTime     = assoc->GetTimeProgress();
    const float criticalTime = vehicle->GetAnimGroup().ComputeCriticalBlendTime(animId);
    const bool  isAT400      = vehicle->GetModelId() == MODEL_AT400;
    switch (animId) {
    case ANIM_ID_DEFAULT_CAR_CRAWLOUTRHS_0: // 0x65164d
    case ANIM_ID_DEFAULT_CAR_CRAWLOUTRHS_1:
        return { animTime, 0.f };
    case ANIM_ID_CAR_ALIGN_LHS: // 0x65151f
    case ANIM_ID_CAR_ALIGN_RHS:
    case ANIM_ID_CAR_ALIGNHI_LHS:
    case ANIM_ID_CAR_ALIGNHI_RHS: {
        if (isAT400) {
            return { 1.f, 1.f };
        }
        if (std::abs(criticalTime) < SPECIAL_CRITICAL_BLEND_TIME) {
            return { 1.f, 0.f };
        }
        return { 1.f, CalculateBlend(animTime, criticalTime - SPECIAL_CRITICAL_BLEND_TIME_BASE, std::abs(criticalTime) - SPECIAL_CRITICAL_BLEND_TIME_BASE) };
    }
    case ANIM_ID_CAR_OPEN_LHS: // 0x6515f6
    case ANIM_ID_CAR_OPEN_RHS:
    case ANIM_ID_CAR_OPEN_LHS_1:
    case ANIM_ID_CAR_OPEN_RHS_1:
        return { 1.f, isAT400 || std::abs(criticalTime) >= SPECIAL_CRITICAL_BLEND_TIME ? 1.f : 0.f };
    case ANIM_ID_CAR_GETIN_LHS_0: // 0x65166d
    case ANIM_ID_CAR_GETIN_RHS_0:
    case ANIM_ID_CAR_GETIN_LHS_1:
    case ANIM_ID_CAR_GETIN_RHS_1:
    case ANIM_ID_CAR_GETIN_BIKE_FRONT: {
        if (isAT400 || std::abs(criticalTime) >= SPECIAL_CRITICAL_BLEND_TIME) {
            return { 1.f - animTime, 1.f };
        }
        return { 1.f - animTime, CalculateBlend(animTime, criticalTime, std::abs(criticalTime)) };
    }
    case ANIM_ID_CAR_PULLOUT_LHS: // 0x651773
    case ANIM_ID_CAR_PULLOUT_RHS:
    case ANIM_ID_UNKNOWN_15:
    case ANIM_ID_CAR_CLOSE_LHS_0:
    case ANIM_ID_CAR_CLOSE_RHS_0:
    case ANIM_ID_CAR_CLOSE_LHS_1:
    case ANIM_ID_CAR_CLOSE_RHS_1:
        return { 1.f, 0.f };
    case ANIM_ID_CAR_CLOSEDOOR_LHS_0: // 0x6517be
    case ANIM_ID_CAR_CLOSEDOOR_RHS_0:
    case ANIM_ID_CAR_CLOSEDOOR_LHS_1:
    case ANIM_ID_CAR_CLOSEDOOR_RHS_1:
    case ANIM_ID_CAR_SHUFFLE_RHS_0:
    case ANIM_ID_CAR_SHUFFLE_RHS_1:
    case ANIM_ID_CAR_ROLLDOOR:
        return { 0.f, 1.f };
    case ANIM_ID_CAR_GETOUT_LHS_0: // 0x6517d3
    case ANIM_ID_CAR_GETOUT_RHS_0:
    case ANIM_ID_CAR_GETOUT_LHS_1:
    case ANIM_ID_CAR_GETOUT_RHS_1:
    case ANIM_ID_CAR_JACKEDLHS:
    case ANIM_ID_CAR_JACKEDRHS:
    case ANIM_ID_CAR_ROLLOUT_LHS:
    case ANIM_ID_CAR_ROLLOUT_RHS:
    case ANIM_ID_CAR_FALLOUT_LHS:
    case ANIM_ID_CAR_FALLOUT_RHS:
        return { animTime, 1.f - CalculateBlend(animTime, criticalTime, std::abs(criticalTime)) };
    case ANIM_ID_CAR_DOORLOCKED_LHS: // 0x651788
    case ANIM_ID_CAR_DOORLOCKED_RHS:
        return { 1.f, vehicle->GetAnimGroup().m_specialFlags.bRunSpecialLockedDoor ? 1.f : 0.f };
    default: // 0x651885
        return { 0.f, 0.f };
    }
}

//! @return How much of the way from the height at the door to the height on the bike's seat the ped is
float GetBikeSeatBlend(CAnimBlendAssociation* assoc, float animProgress) {
    switch (assoc->GetAnimId()) {
    case ANIM_ID_CAR_GETOUT_LHS_0: // 0x651b93
    case ANIM_ID_CAR_GETOUT_RHS_0:
    case ANIM_ID_CAR_GETOUT_LHS_1:
    case ANIM_ID_CAR_GETOUT_RHS_1:
        return 1.f - animProgress;
    case ANIM_ID_CAR_GETIN_LHS_0: // 0x651b57
    case ANIM_ID_CAR_GETIN_RHS_0:
    case ANIM_ID_CAR_GETIN_LHS_1:
    case ANIM_ID_CAR_GETIN_RHS_1:
        return std::min(assoc->GetTimeProgress() * 2.f, 1.f);
    default: // 0x651b4d
        return 0.f;
    }
}

//! Put the ped at `pos`, with a rotation `t` (0 - 1) of the way between the two
void SetPedMatrixBetween(CPed* ped, const CMatrix& from, const CMatrix& to, float t, const CVector& pos) {
    RwMatrix rwFrom, rwTo;
    from.CopyToRwMatrix(&rwFrom);
    to.CopyToRwMatrix(&rwTo);

    RtQuat qFrom, qTo;
    RtQuatConvertFromMatrix(&qFrom, &rwFrom);
    RtQuatConvertFromMatrix(&qTo, &rwTo);

    RtQuatSlerpCache cache;
    RtQuatSetupSlerpCache(&qFrom, &qTo, &cache);

    RtQuat q;
    RtQuatSlerp(&q, &qFrom, &qTo, t, &cache);

    RwMatrix rwMat;
    RtQuatConvertToMatrix(&q, &rwMat);

    CMatrix mat;
    mat.UpdateMatrix(&rwMat);
    mat.GetPosition() = pos;
    ped->SetMatrix(mat);
}
};

// 0x64FBB0
CTaskUtilityLineUpPedWithCar::CTaskUtilityLineUpPedWithCar(const CVector& offset, int32 time, int32 doorOpenPosType, int32 doorIdx) :
    m_Offset{ offset },
    m_DoorOpenPosZ{ -999.99f },
    m_EndTime{ CTimer::GetTimeInMS() + time },
    m_nDoorOpenPosType{ doorOpenPosType },
    m_nDoorIdx{ doorIdx } {
}

// 0x64FC10
CVector CTaskUtilityLineUpPedWithCar::GetLocalPositionToOpenCarDoor(CVehicle* vehicle, float animProgress, CAnimBlendAssociation* assoc) {
    const auto  door                 = GetTargetDoor();
    const bool  isRearDoor           = door == TARGET_DOOR_REAR_LEFT || door == TARGET_DOOR_REAR_RIGHT;
    const float seatOffset           = vehicle->vehicleFlags.bIsVan && isRearDoor
        ? 0.f
        : animProgress * vehicle->m_pHandlingData->m_fSeatOffsetDistance;

    const auto offsetType            = GetDoorOffsetType(door, assoc);

    auto&         animGroup          = vehicle->GetAnimGroup();
    const bool    isLockedDoorAnim   = assoc && (assoc->GetAnimId() == ANIM_ID_CAR_DOORLOCKED_LHS || assoc->GetAnimId() == ANIM_ID_CAR_DOORLOCKED_RHS);
    const bool    isLockedDoorBlend  = isLockedDoorAnim && animGroup.m_specialFlags.bRunSpecialLockedDoor;
    const CVector animOffset         = isLockedDoorBlend // 0x64fd05
        ? animGroup.ComputeAnimDoorOffsets(offsetType) * (1.f - assoc->GetBlendAmount()) + CVehicleAnimGroupData::GetVehicleAnimGroup(0).ComputeAnimDoorOffsets(offsetType) * assoc->GetBlendAmount()
        : animGroup.ComputeAnimDoorOffsets(offsetType);

    auto* const   mi                 = vehicle->GetVehicleModelInfo();
    const CVector frontSeat          = mi->GetFrontSeatPosn();
    const CVector rearSeat           = mi->GetBackSeatPosn();
    const auto [seatPos, doorOffset] = GetSeatPosAndDoorOffset(door, frontSeat, rearSeat, seatOffset, animOffset);
    if (door == TARGET_DOOR_UNK) {
        return seatPos + doorOffset;
    }
    if (vehicle->IsBike()) { // 0x65004b
        CVector worldPos;
        vehicle->AsBike()->GetCorrectedWorldDoorPosition(worldPos, doorOffset, seatPos);
        return vehicle->GetMatrix().InverseTransformVector(worldPos - vehicle->GetPosition());
    }
    return seatPos - doorOffset;
}

// 0x650A80
CVector CTaskUtilityLineUpPedWithCar::GetPositionToOpenCarDoor(CVehicle* vehicle, float animProgress, CAnimBlendAssociation* assoc) {
    return vehicle->GetMatrix().TransformPoint(GetLocalPositionToOpenCarDoor(vehicle, animProgress, assoc));
}

// 0x6513A0
bool CTaskUtilityLineUpPedWithCar::ProcessPed(CPed* ped, CVehicle* vehicle, CAnimBlendAssociation* assoc) {
    if (m_nDoorOpenPosType == LINE_UP_AT_DOOR) {
        ped->ResetMoveSpeed();
    }

    const auto door = GetTargetDoor();

    // NOTE: Not `IsUpsideDown()`, that one uses -0.9
    const bool isUpsideDown = vehicle->GetUp().z <= -0.8f;
    if (isUpsideDown) { // 0x651421
        ped->m_fAimingRotation = door == TARGET_DOOR_FRONT_RIGHT || door == TARGET_DOOR_REAR_RIGHT
            ? vehicle->GetHeading() - PI
            : vehicle->GetHeading();
    } else if (door == TARGET_DOOR_UNK) { // 0x65146e
        ped->m_fAimingRotation = vehicle->GetHeading() + PI;
    } else if (m_nDoorOpenPosType != LINE_UP_FALLING) { // 0x651497
        ped->m_fAimingRotation = vehicle->GetHeading();
    }

    const auto [animProgress, heightBlend] = GetAnimProgressAndHeightBlend(vehicle, assoc);

    const bool    isFalling                = m_nDoorOpenPosType == LINE_UP_FALLING;
    const CVector doorPos                  = isFalling
        ? ped->GetPosition()
        : GetPositionToOpenCarDoor(vehicle, animProgress, assoc);
    const CVector openDoorPos              = vehicle->IsBike() || isFalling // 0x6518cc
        ? doorPos
        : GetPositionToOpenCarDoor(vehicle, 1.f, assoc);

    m_DoorOpenPosZ                         = [&] {
        if (vehicle->physicalFlags.bSubmergedInWater) { // 0x651a1b
            return vehicle->IsBoat() && vehicle->IsUpsideDown()
                ? openDoorPos.z + 1.f
                : openDoorPos.z;
        }
        // Highest the ground can be at the door: The vehicle's bottom at that side + the ped's height
        const float sideDist     = (openDoorPos - vehicle->GetPosition()).Dot(vehicle->GetRight());
        const float maxGroundZ   = vehicle->GetPosition().z - vehicle->GetHeightAboveRoad() + vehicle->GetRight().z * sideDist + 1.f;
        const float foundGroundZ = std::get<CVector>(CPedPlacement::FindZCoorForPed(openDoorPos)).z;
        return foundGroundZ < maxGroundZ - 0.5f // 0x6519e3
            ? openDoorPos.z
            : foundGroundZ;
    }();

    const float fallenZ = [&] {
        if (m_nDoorOpenPosType != LINE_UP_AT_DOOR_FALLING && !isFalling) {
            return doorPos.z;
        }
        const float fallSpeed = ped->GetMoveSpeed().z - CTimer::GetTimeStep() * GAME_GRAVITY; // 0x651a55
        if (ped->GetPosition().z + fallSpeed >= m_DoorOpenPosZ) {                             // 0x651ac7
            ped->GetMoveSpeed().z = fallSpeed;
            return ped->GetPosition().z;
        }
        ped->ResetMoveSpeed();
        return m_DoorOpenPosZ;
    }();

    const float posZ = [&] {
        if (fallenZ < m_DoorOpenPosZ) {
            if (vehicle->IsBike() && assoc && assoc->GetAnimId() != ANIM_ID_CAR_GETIN_BIKE_FRONT) { // 0x651b0f
                const float seatBlend = GetBikeSeatBlend(assoc, animProgress);
                return (vehicle->GetPosition().z - vehicle->GetHeightAboveRoad() + 1.f - m_DoorOpenPosZ) * seatBlend + m_DoorOpenPosZ;
            }
        } else if (m_nDoorOpenPosType != LINE_UP_AT_DOOR) { // 0x651bd1
            return fallenZ;
        }
        return (fallenZ - m_DoorOpenPosZ) * heightBlend + m_DoorOpenPosZ; // 0x651bd8
    }();

    const CVector pos = [&] {
        if (CTimer::GetTimeInMS() >= m_EndTime) { // 0x651d09
            ped->m_fCurrentRotation = ped->m_fAimingRotation;
            return CVector{ doorPos.x, doorPos.y, posZ };
        }

        // Blend from the position and the heading the ped had when the line up began
        const float blend = (float)(m_EndTime - CTimer::GetTimeInMS()) * (1.f / 600.f);
        if (blend <= 0.f) {
            m_Offset.x = 0.f;
            m_Offset.y = 0.f;
        }
        m_Offset.z                = 0.f;

        const float currHeading   = ped->m_fCurrentRotation;
        const float aimHeading    = CGeneral::LimitRadianAngle(ped->m_fAimingRotation);
        const float targetHeading = aimHeading > currHeading + PI
            ? aimHeading - TWO_PI
            : aimHeading < currHeading - PI
            ? aimHeading + TWO_PI
            : aimHeading;
        ped->m_fCurrentRotation   = currHeading - (1.f - blend) * (currHeading - targetHeading);

        return CVector{ doorPos.x, doorPos.y, posZ } - m_Offset * blend;
    }();

    if (assoc) {
        switch (assoc->GetAnimId()) {
        case ANIM_ID_CAR_GETIN_LHS_0: // 0x651d43
        case ANIM_ID_CAR_GETIN_RHS_0:
        case ANIM_ID_CAR_GETIN_LHS_1:
        case ANIM_ID_CAR_GETIN_RHS_1:
        case ANIM_ID_CAR_GETIN_BIKE_FRONT: {
            CMatrix vehMat{ vehicle->GetMatrix() };
            if (assoc->GetAnimId() == ANIM_ID_CAR_GETIN_BIKE_FRONT) {
                CMatrix halfTurn;
                halfTurn.SetRotateZ(PI);
                vehMat *= halfTurn;
            }
            SetPedMatrixBetween(ped, ped->GetMatrix(), vehMat, assoc->GetTimeProgress(), pos);
            return false;
        }
        case ANIM_ID_CAR_GETOUT_LHS_0: // 0x6521f2
        case ANIM_ID_CAR_GETOUT_RHS_0:
        case ANIM_ID_CAR_GETOUT_LHS_1:
        case ANIM_ID_CAR_GETOUT_RHS_1:
        case ANIM_ID_UNKNOWN_26:       {
            CMatrix pedMat{ ped->GetMatrix() };
            pedMat.SetRotateZOnly(ped->m_fCurrentRotation);
            SetPedMatrixBetween(ped, vehicle->GetMatrix(), pedMat, assoc->GetTimeProgress(), pos);
            return false;
        }
        }
    }

    if (animProgress > MAX_ANIM_PROGRESS_TO_USE_VEHICLE_ORIENTATION || isUpsideDown || vehicle->IsBike() || vehicle->IsSubQuad()) { // 0x652746
        ped->SetPosn(pos);
        ped->SetOrientation(0.f, 0.f, ped->m_fCurrentRotation);
    } else { // 0x65267b
        CMatrix mat{ vehicle->GetMatrix() };
        mat.GetPosition() += mat.TransformVector(GetLocalPositionToOpenCarDoor(vehicle, 0.f, assoc));
        ped->SetMatrix(mat);
    }
    return false;
}

void CTaskUtilityLineUpPedWithCar::InjectHooks() {
    RH_ScopedClass(CTaskUtilityLineUpPedWithCar);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x64FBB0, "", {}, const CVector&, int32, int32, int32);
    RH_ScopedDestructorInstall(0x64FC00);

    RH_ScopedInstall(GetLocalPositionToOpenCarDoor, 0x64FC10);
    RH_ScopedInstall(GetPositionToOpenCarDoor, 0x650A80);
    RH_ScopedInstall(ProcessPed, 0x6513A0);
}
