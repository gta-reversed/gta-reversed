#include "StdInc.h"

#include <extensions/utility.hpp>

#include "AECollisionAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"

#include "AECollisionAudioEntity.CollisionLookup.h"

void CAECollisionAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAECollisionAudioEntity, 0x862E64, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Initialise, 0x5B9BD0);
    RH_ScopedInstall(InitialisePostLoading, 0x4DA050);
    RH_ScopedInstall(AddCollisionSoundToList, 0x4DAAC0);
    RH_ScopedInstall(Reset, 0x4DA320);
    RH_ScopedInstall(ReportGlassCollisionEvent, 0x4DA070);
    RH_ScopedInstall(UpdateLoopingCollisionSound, 0x4DA540);
    RH_ScopedInstall(GetCollisionSoundStatus, 0x4DA830);
    RH_ScopedInstall(ReportObjectDestruction, 0x4DAB60);
    RH_ScopedInstall(PlayOneShotCollisionSound, 0x4DB150);
    RH_ScopedInstall(PlayLoopingCollisionSound, 0x4DB450);
    RH_ScopedInstall(PlayBulletHitCollisionSound, 0x4DB7C0);
    RH_ScopedInstall(ReportCollision, 0x4DBA10);
    RH_ScopedInstall(ReportBulletHit, 0x4DBDF0);
    RH_ScopedInstall(Service, 0x4DA2C0);
    RH_ScopedInstall(ChooseCollisionSoundID, 0x4DAA50);
    RH_ScopedOverloadedInstall(ReportWaterSplash, "at-position", 0x4DA190, void(CAECollisionAudioEntity::*)(CVector, float));
    RH_ScopedOverloadedInstall(ReportWaterSplash, "for-physical", 0x4DAE40, void(CAECollisionAudioEntity::*)(CPhysical*, float, bool));
}

// 0x5B9BD0
void CAECollisionAudioEntity::Initialise() {
    *this = {};
}

// 0x4DA050
void CAECollisionAudioEntity::InitialisePostLoading() {
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_BULLET_HITS, SND_BANK_SLOT_BULLET_HITS);
}

// 0x4DA320
void CAECollisionAudioEntity::Reset() {
    for (auto& entry : m_CollisionSoundList) {
        if (entry.Status != COLLISION_SOUND_LOOPING) {
            continue;
        }

        if (entry.Sound) {
            entry.Sound->StopSoundAndForget();
        }

        entry = {};
    }
}

// 0x4DAAC0
void CAECollisionAudioEntity::AddCollisionSoundToList(
    CEntity* entityA,
    CEntity* entityB,
    eSurfaceType surfaceA,
    eSurfaceType surfaceB,
    CAESound* sound,
    eCollisionSoundStatus status
) {
    // Find an entry with no sound.
    const auto e = rng::find_if_not(m_CollisionSoundList, &tCollisionSound::Sound);
    if (e != m_CollisionSoundList.end()) {
        NOTSA_LOG_WARN("Collision sound list is full");
        return;
    }

    e->EntityA  = entityA;
    e->EntityB  = entityB;

    e->SurfaceA = surfaceA;
    e->SurfaceB = surfaceB;

    e->Sound          = sound;
    e->Status         = status;
    e->LoopStopTimeMs = status == COLLISION_SOUND_LOOPING
        ? CTimer::GetTimeInMS() + 100
        : 0;

    m_NumActiveCollisionSounds++;
}

// 0x4DA830
eCollisionSoundStatus CAECollisionAudioEntity::GetCollisionSoundStatus(CEntity* entityA, CEntity* entityB, eSurfaceType surfaceA, eSurfaceType surfaceB, int32& outIndex) {
    auto status = COLLISION_SOUND_INACTIVE;
    for (auto&& [i, v] : rngv::enumerate(m_CollisionSoundList)) {
        if (v.EntityA == entityA && v.EntityB == entityB || v.EntityA == entityB && v.EntityB == entityA) {
            if ((status = v.Status) == COLLISION_SOUND_LOOPING) {
                outIndex = (int32)(i);
                return v.Status;
            }
        }
    }
    outIndex = -1;
    return status;
}

// 0x4DB150
void CAECollisionAudioEntity::PlayOneShotCollisionSound(CEntity* entityA, CEntity* entityB, eSurfaceType surfaceA, eSurfaceType surfaceB, float impulseMagnitude, const CVector& posn) {
    const auto ProcessSound = [&](CEntity* eA, CEntity* eB, eSurfaceType sA, eSurfaceType sB) {
        if (sB >= SURFACE_NUM_TYPES_FOR_COLLISION) {
            return false;
        }
        if (sB == SURFACE_UNKNOWN_192 && sA != SURFACE_UNKNOWN_192) {
            return false;
        }
        const auto isMissionScriptSurface = notsa::contains({ SURFACE_UNKNOWN_192, SURFACE_UNKNOWN_193, SURFACE_UNKNOWN_194 }, sB); // 0x4DB23E
        const auto slot = isMissionScriptSurface
            ? SND_BANK_SLOT_MISSION4
            : SND_BANK_SLOT_COLLISIONS;
        if (isMissionScriptSurface) {
            const auto bank = notsa::find_value(notsa::make_mapping<eSurfaceType, eSoundBank>({
                { SURFACE_UNKNOWN_192, SND_BANK_SCRIPT_POOL_MINIGAME },
                { SURFACE_UNKNOWN_193, SND_BANK_SCRIPT_BASKETBALL },
                { SURFACE_UNKNOWN_194, SND_BANK_SCRIPT_GYM },
            }), sB);
            if (!AEAudioHardware.IsSoundBankLoaded(bank, slot)) {
                return false;
            }
        }
        const auto soundID = ChooseCollisionSoundID(sB); // 0x4DB280
        if (soundID == -1) {
            return true;
        }
        auto offset = ((float)(gCollisionLookup[sA].ParamD) * impulseMagnitude) / 100.f;
        if (sB == SURFACE_UNKNOWN_188 && sA == SURFACE_PED) { // 0x4DB2A6
            offset *= 10.f;
        }
        offset *= 500.f;
        const auto startAt = 100 - std::min(100, (int32)(std::floor(offset))); // 0x4DB2FB
        if (startAt >= 100) {
            return true;
        }
        const auto PlaySound = [&](float volume, int32 startAt) {
            volume += GetDefaultVolume(AE_GENERAL_COLLISION) - 3.f;
            if (volume <= -100.f) { // 0x4DB37E
                return;
            }
            auto* const sound = AESoundManager.PlaySound({
                .BankSlotID        = slot,
                .SoundID           = soundID,
                .AudioEntity       = this,
                .Pos               = posn,
                .Volume            = volume,
                .RollOffFactor     = 2.f,
                .Flags             = SOUND_ROLLED_OFF | SOUND_START_PERCENTAGE | SOUND_REQUEST_UPDATES,
                .FrequencyVariance = 0.02f
            });
            m_CollisionSoundIDHistory[sB] = soundID;
            if (sound) { // 0x4DB3E7
                AddCollisionSoundToList(entityA, entityB, sA, sB, sound, COLLISION_SOUND_ONE_SHOT);
            }
        };
        const auto maxStartAt = gCollisionLookup[sB].MaxStartAt;
        if (startAt > maxStartAt) { // 0x4DB32D
            PlaySound(CAEAudioUtility::AudioLog10((float)(100 - startAt) / (float)(100 - maxStartAt)) * 20.f, maxStartAt);
        } else {
            PlaySound(0.f, startAt);
        }
        return true;
    };

    if (!ProcessSound(entityA, entityB, surfaceA, surfaceB)) {
        return;
    }
    if (!ProcessSound(entityB, entityA, surfaceB, surfaceA)) {
        return;
    }
}

// based on `PlayLoopingCollisionSound` and `UpdateLoopingCollisionSound`
std::pair<float, float> CAECollisionAudioEntity::GetLoopingCollisionSoundVolumeAndSpeed(CEntity* entityA, CEntity* entityB, eSurfaceType surfaceA, eSurfaceType surfaceB, bool isForceLooping) {
    const auto CalculateVolumeAtSpeed = [&](float speed) {
        return GetDefaultVolume(AE_GENERAL_COLLISION) + CAEAudioUtility::AudioLog10(std::min(speed / 0.75f, 1.f) * 20.f);
    };
    const auto GetVolumeAndSpeedForPhysical = [&](CVector velocity, CVector turn) -> std::pair<float, float> {
        const auto mag = std::sqrt(std::max(turn.SquaredMagnitude(), velocity.SquaredMagnitude()));
        if (mag == 0.f) {
            return { -100.f, 0.f };
        } else {
            auto speed = std::min(std::sqrt(mag * ((gCollisionLookup[surfaceA].ParamD * gCollisionLookup[surfaceB].ParamD) / 10000.f)) * 3.f, 0.3f);
            if (isForceLooping) {
                speed /= 6.f;
            }
            return { CalculateVolumeAtSpeed(speed), speed };
        }
    };
    
    if (surfaceA == SURFACE_CAR && surfaceB == SURFACE_CAR) {
        const auto vehA = entityA->AsVehicle(),
                   vehB = entityB->AsVehicle();
        return GetVolumeAndSpeedForPhysical(vehA->GetMoveSpeed() - vehB->GetMoveSpeed(), vehA->GetTurnSpeed() - vehB->GetTurnSpeed());
    }

    if ((surfaceB != SURFACE_PED && surfaceA == SURFACE_CAR) || (surfaceA != SURFACE_PED && surfaceB == SURFACE_CAR)) {
        assert(entityA->IsPhysical());

        const auto* const physicalA = entityA->AsPhysical();
        return GetVolumeAndSpeedForPhysical(physicalA->GetMoveSpeed(), physicalA->GetTurnSpeed());
    }

    return { -100.f, 0.f };
}

// 0x4DB450
void CAECollisionAudioEntity::PlayLoopingCollisionSound(CEntity* entityA, CEntity* entityB, eSurfaceType surfaceA, eSurfaceType surfaceB, float force, const CVector& pos, bool isForceLooping) {
    const auto [volume, speed] = GetLoopingCollisionSoundVolumeAndSpeed(entityA, entityB, surfaceA, surfaceB, isForceLooping);
    const auto GetSoundID = [&]() -> eSoundID { // 0x4DB6AF
        if (g_surfaceInfos.IsAudioGrass(surfaceA) || g_surfaceInfos.IsAudioGrass(surfaceB)) {
            return 0;
        } else if (g_surfaceInfos.IsAudioWater(surfaceA) || g_surfaceInfos.IsAudioWater(surfaceB)) {
            return 3;
        } else if (g_surfaceInfos.IsAudioMetal(surfaceA) || g_surfaceInfos.IsAudioMetal(surfaceB)) {
            return 2;
        } else {
            return 1;
        }
    };
    if (auto* const sound = AESoundManager.PlaySound({
            .BankSlotID    = SND_BANK_SLOT_COLLISIONS,
            .SoundID       = GetSoundID(),
            .AudioEntity   = this,
            .Pos           = pos,
            .Volume        = volume,
            .RollOffFactor = 2.f,
            .Speed         = std::max(speed, 0.75f) * 0.8f,
            .Flags         = SOUND_REQUEST_UPDATES,
    })) {
        AddCollisionSoundToList(entityA, entityB, surfaceA, surfaceB, sound, COLLISION_SOUND_LOOPING);
    }
}

// 0x4DA540
void CAECollisionAudioEntity::UpdateLoopingCollisionSound(
    CAESound*      sound,
    CEntity*       entityA,
    CEntity*       entityB,
    eSurfaceType   surfaceA,
    eSurfaceType   surfaceB,
    float          impulseForce,
    const CVector& pos,
    bool           isForceLooping
) {
    const auto [volume, speed] = GetLoopingCollisionSoundVolumeAndSpeed(entityA, entityB, surfaceA, surfaceB, isForceLooping);
    sound->SetSpeed(notsa::step_to(std::max(0.75f, speed), sound->GetSpeed(), 0.1f));
    sound->SetVolume(notsa::step_to(volume, sound->GetVolume(), 1.0f));
    sound->SetPosition(pos);
}

// 0x4DB7C0
void CAECollisionAudioEntity::PlayBulletHitCollisionSound(eSurfaceType surface, const CVector& posn, float angleWithColPointNorm) {
    if (surface >= SURFACE_NUM_TYPES_FOR_COLLISION) {
        return;
    }
    const auto PlayRandomSound = [&](int32 minID, int32 maxID, float volumeOffset = 0.f, float rollOff = 1.5f) { // 0x4DB9BF
        //! Find a new random sound ID that is not the same as the last one.
        const auto GetNewRandomSoundID = [&]{
            while (true) {
                const auto id = CAEAudioUtility::GetRandomNumberInRange(minID, maxID);
                if (id != m_LastBulletHitSoundID) {
                    return id;
                }
            }
        };
        AESoundManager.PlaySound({
            .BankSlotID        = SND_BANK_SLOT_BULLET_HITS,
            .SoundID           = m_LastBulletHitSoundID = GetNewRandomSoundID(),
            .AudioEntity       = this,
            .Pos               = posn,
            .Volume            = GetDefaultVolume(AE_BULLET_HIT) + volumeOffset,
            .RollOffFactor     = rollOff,
            .FrequencyVariance = 0.02f
        });
    };
    if (surface == SURFACE_PED) {
        PlayRandomSound(7, 9);
    } else if (g_surfaceInfos.IsAudioWater(surface)) {
        PlayRandomSound(16, 18, 6.f, 2.f);
    } else if (g_surfaceInfos.IsAudioWood(surface)) {
        PlayRandomSound(19, 21);
    } else if (g_surfaceInfos.IsAudioMetal(surface)) {
        if (CAEAudioUtility::ResolveProbability((90.0f - angleWithColPointNorm) / 180.0f)) { // see BoneNode_c::EulerToQuat
            PlayRandomSound(10, 12);
        } else {
            PlayRandomSound(4, 6);
        }
    } else if (g_surfaceInfos.IsAudioGravelConcreteOrTile(surface)) {
        PlayRandomSound(13, 15);
    } else {
        PlayRandomSound(1, 3);
    }
}

// 0x4DA070
void CAECollisionAudioEntity::ReportGlassCollisionEvent(eAudioEvents event, const CVector& posn, uint32 time) {
    const auto PlayCollisionSound = [&](eSoundID soundID, float speed) {
        if (time) {
            AESoundManager.PlaySound({
                .BankSlotID     = SND_BANK_SLOT_COLLISIONS,
                .SoundID        = soundID,
                .AudioEntity    = this,
                .Pos            = posn,
                .Volume         = GetDefaultVolume(event),
                .RollOffFactor  = 1.5f,
                .Speed          = 0.f,
                .Flags          = SOUND_REQUEST_UPDATES,
                .EventID        = event,
                .ClientVariable = (float)(time + CTimer::GetTimeInMS()),
            });
        } else {
            AESoundManager.PlaySound({
                .BankSlotID     = SND_BANK_SLOT_COLLISIONS,
                .SoundID        = soundID,
                .AudioEntity    = this,
                .Pos            = posn,
                .Volume         = GetDefaultVolume(event),
                .RollOffFactor  = 1.5f,
                .Speed          = speed,
            });
        }
    };
    switch (event) {
    case AE_GLASS_HIT:             PlayCollisionSound(51, 1.f);                                                break;
    case AE_GLASS_CRACK:           PlayCollisionSound(68, 1.f);                                                break;
    case AE_GLASS_BREAK_SLOW:      PlayCollisionSound(56, 0.75f);                                              break;
    case AE_GLASS_BREAK_FAST:      PlayCollisionSound(56, 1.f);                                                break;
    case AE_GLASS_HIT_GROUND:      PlayCollisionSound(CAEAudioUtility::GetRandomNumberInRange(15, 18), 1.f);   break;
    case AE_GLASS_HIT_GROUND_SLOW: PlayCollisionSound(CAEAudioUtility::GetRandomNumberInRange(15, 18), 0.56f); break;
    default:                       NOTSA_UNREACHABLE();
    }
}

// 0x4DA190
void CAECollisionAudioEntity::ReportWaterSplash(CVector posn, float volume) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS, true)) {
        return;
    }
    AESoundManager.PlaySound({                        
        .BankSlotID    = SND_BANK_SLOT_COLLISIONS,
        .SoundID       = 67,
        .AudioEntity   = this,
        .Pos           = posn,
        .Volume        = GetDefaultVolume(AE_WATER_SPLASH) + volume,
        .RollOffFactor = 2.5f,
        .Speed         = 1.26f,
        .Flags         = SOUND_REQUEST_UPDATES,
        .EventID       = 1                               
    });
    AESoundManager.PlaySound({
        .BankSlotID     = SND_BANK_SLOT_COLLISIONS,
        .SoundID        = 66,
        .AudioEntity    = this,
        .Pos            = posn,
        .Volume         = GetDefaultVolume(AE_WATER_SPLASH) + volume,
        .RollOffFactor  = 2.5f,
        .Speed          = 0.f,
        .Flags          = SOUND_REQUEST_UPDATES,
        .EventID        = 2,
        .ClientVariable = (float)(CTimer::GetTimeInMS() + 166)
    });
}

// 0x4DAE40
void CAECollisionAudioEntity::ReportWaterSplash(CPhysical* physical, float volume, bool isForceSplash) {
    assert(physical->IsPhysical());

    if (!isForceSplash) {
        if (physical->GetMoveSpeed().z > -0.1f && volume == -100.f) {
            return;
        }
        if (rng::any_of(std::to_array({ 1, 2, 3 }), [&](int32 event) {
            return AESoundManager.AreSoundsOfThisEventPlayingForThisEntityAndPhysical(event, this, physical) != 0;
        })) {
            return;
        }
    }
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS, true)) { // 0x4DAED2
        return;
    }
    const auto PlaySound = [&](int32 eventID, float speed, float volumeOffset, uint32 interval) {
        if (volume <= -100.f) { // 0x4DAF90
            volume = std::max(CAEAudioUtility::AudioLog10(std::min(0.6f, physical->GetMoveSpeed().Magnitude())) * 20.f, -18.f);
        }
        volume += GetDefaultVolume(AE_WATER_SPLASH) + volumeOffset;

        AESoundManager.PlaySound({
            .BankSlotID         = SND_BANK_SLOT_COLLISIONS,
            .SoundID            = 67,
            .AudioEntity        = this,
            .Pos                = physical->GetPosition(),
            .Volume             = volume,
            .RollOffFactor      = 2.5f,
            .Speed              = speed,
            .Flags              = SOUND_REQUEST_UPDATES,
            .EventID            = 1,
        });
        AESoundManager.PlaySound({
            .BankSlotID         = SND_BANK_SLOT_COLLISIONS,
            .SoundID            = 66,
            .AudioEntity        = this,
            .Pos                = physical->GetPosition(),
            .Volume             = volume,
            .RollOffFactor      = 2.5f,
            .Speed              = speed,
            .Flags              = SOUND_REQUEST_UPDATES | SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY,
            .RegisterWithEntity = physical,
            .EventID            = eventID,
            .ClientVariable     = (float)(CTimer::GetTimeInMS() + interval),
        });
    };
    if (physical->IsPed()) { // 0x4DAF2B - Inverted
        if (isForceSplash && !physical->AsPed()->GetIntelligence()->GetTaskSwim()) {
            PlaySound(2, 1.26f, -6.f, 166);
        }
    } else if (physical->IsVehicle()) { // 0x4DAF52
        PlaySound(3, 0.94f, 0.f, 248);
    } else { // 0x4DAF70
        PlaySound(2, 1.26f, -12.f, 166);
    }
}

// 0x4DAB60
void CAECollisionAudioEntity::ReportObjectDestruction(CEntity* entity) {
    if (!entity) {
        return;
    }

    if (entity->GetModelID() == ModelIndices::MI_GRASSHOUSE) { // 0x4DAB94 - Moved up here
        AESoundManager.PlaySound({ // 0x4DABEF
            .BankSlotID    = SND_BANK_SLOT_COLLISIONS,
            .SoundID       = (eSoundID)(56),
            .AudioEntity   = this,
            .Pos           = entity->GetPosition(),
            .Volume        = GetDefaultVolume(AE_GLASS_BREAK_SLOW),
            .RollOffFactor = 1.5f,
            .Speed         = 0.75f,
        });
        AESoundManager.PlaySound({ // 0x4DAC65
            .BankSlotID    = SND_BANK_SLOT_COLLISIONS,
            .SoundID       = (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(15, 18)),
            .AudioEntity   = this,
            .Pos           = entity->GetPosition(),
            .Volume        = GetDefaultVolume(AE_GLASS_HIT_GROUND_SLOW),
            .RollOffFactor = 1.5f,
            .Speed         = 0.0f,
            .Flags         = SOUND_REQUEST_UPDATES,
            .EventID        = AE_GLASS_HIT_GROUND_SLOW,
            .ClientVariable = (float)(CTimer::GetTimeInMS() + 600),
        });
        return;
    }

    const auto PlaySoundForSurface = [&](eSurfaceType surface) {
        const auto PlaySound = [&](eSoundID soundID) {
            AESoundManager.PlaySound({
                .BankSlotID        = SND_BANK_SLOT_COLLISIONS,
                .SoundID           = soundID,
                .AudioEntity       = this,
                .Pos               = entity->GetPosition(),
                .Volume            = GetDefaultVolume(AE_GENERAL_COLLISION),
                .RollOffFactor     = 2.f,
                .FrequencyVariance = 0.0588f,
            });
        };
        switch (surface) {
        case SURFACE_WOOD_CRATES:
        case SURFACE_WOOD_BENCH:
        case SURFACE_WOOD_PICKET_FENCE:
        case SURFACE_WOOD_SLATTED_FENCE:
        case SURFACE_WOOD_RANCH_FENCE:   PlaySound(CAEAudioUtility::GetRandomNumberInRange(41, 44)); break; // 0x4DAD14
        case SURFACE_GLASS:              PlaySound(11); break;                                              // 0x4DAD43
        default:                         break;
        }
    };
    if (entity->GetModelID() == MODEL_MOLOTOV) { // 0x4DAB7B
        PlaySoundForSurface(SURFACE_GLASS);
    } else if (const auto cd = entity->GetColData()) { // 0x4DAB89
        if (const auto spheres = cd->GetSpheres(); !spheres.empty()) {
            PlaySoundForSurface(spheres.front().GetSurfaceType());
        } else if (const auto boxes = cd->GetBoxes(); !boxes.empty()) {
            PlaySoundForSurface(boxes.front().GetSurfaceType());
        } else if (const auto tris = cd->GetTris(); !tris.empty()) {
            PlaySoundForSurface(tris.front().GetSurfaceType());
        }
    }
}

// 0x4DBA10
void CAECollisionAudioEntity::ReportCollision(
    CEntity*       entityA,
    CEntity*       entityB,
    eSurfaceType   surfaceA,
    eSurfaceType   surfaceB,
    const CVector& pos,
    const CVector* normal,
    float          impulseForce,
    float          relVelSq,
    bool           isForceOneShot,
    bool           isForceLooping
) {
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
        return;
    }
    if (!CanAddNewSound()) {
        return;
    }
    if (relVelSq <= 0.f) {
        return;
    }

    const auto GetSurfaceToUse = [&](
        CEntity* eA, eSurfaceType& sA,
        CEntity* eB, eSurfaceType& sB
    ) {
        if (!eA->IsVehicle()) {      
            switch (eA->GetModelID()) {
            case MODEL_MOLOTOV:
                return SURFACE_GLASS;
            case MODEL_SATCHEL:
                return  SURFACE_UNKNOWN_190;
            case MODEL_GRENADE:
            case MODEL_BOMB:
            case MODEL_TEARGAS:
                return SURFACE_UNKNOWN_191;
            }
            const auto eAModelId = eA->GetModelID();
            if (eAModelId == ModelIndices::MI_BASKETBALL) {
                return SURFACE_UNKNOWN_193;
            }
            if (eAModelId == ModelIndices::MI_PUNCHBAG) {
                return SURFACE_UNKNOWN_194;
            }
            if (eAModelId == ModelIndices::MI_GRASSHOUSE) {
                return SURFACE_UNBREAKABLE_GLASS;
            }
            if (eAModelId == ModelIndices::MI_IMY_GRAY_CRATE) {
                return SURFACE_WOOD_SOLID;
            }
            if (eA->IsPhysical() && eA->AsPhysical()->physicalFlags.bMakeMassTwiceAsBig) {
                return SURFACE_UNKNOWN_192;
            }
            return sA;        
        }
        if (eB && eB->IsBuilding() && normal && eA->GetUp().Dot(*normal) > 0.6f) {
            if (eA->AsVehicle()->IsSubBMX()) {
                return SURFACE_UNKNOWN_188;
            }
            if (g_surfaceInfos.GetFrictionEffect(sB) != FRICTION_EFFECT_SPARKS) {
                return SURFACE_RUBBER;
            }
        }
        if (eA->AsVehicle()->IsSubBMX()) {
            return SURFACE_UNKNOWN_188;
        }
        return SURFACE_CAR;
    };

    surfaceA = GetSurfaceToUse(entityA, surfaceA, entityB, surfaceB);
    surfaceB = GetSurfaceToUse(entityB, surfaceB, entityA, surfaceA);

    if (isForceOneShot) { // 0x4DBC68
        PlayOneShotCollisionSound(entityA, entityB, surfaceA, surfaceB, impulseForce, pos);
    } else  { // 0x4DBC83
        int32 entryID;
        switch (const auto soundStatus = CAECollisionAudioEntity::GetCollisionSoundStatus(entityA, entityB, surfaceA, surfaceB, entryID)) {
        case COLLISION_SOUND_INACTIVE: { // 0x4DBD1C, 0x4DBD5A
            if (isForceLooping) {
                return PlayLoopingCollisionSound(entityA, entityB, surfaceA, surfaceB, impulseForce, pos, isForceLooping);
            }
            return PlayOneShotCollisionSound(entityA, entityB, surfaceA, surfaceB, impulseForce, pos);
        }
        case COLLISION_SOUND_ONE_SHOT: // 0x4DBD1C, 0x4DBDE0
            return PlayLoopingCollisionSound(entityA, entityB, surfaceA, surfaceB, impulseForce, pos, isForceLooping);
        case COLLISION_SOUND_LOOPING: { // 0x4DBCA3, 0x4DBD44
            const auto e = &m_CollisionSoundList[entryID];
            e->LoopStopTimeMs = CTimer::GetTimeInMS() + 100;
            if (e->Sound) {
                UpdateLoopingCollisionSound(e->Sound, e->EntityA, e->EntityB, e->SurfaceA, e->SurfaceB, impulseForce, pos, isForceLooping);
            }
            break;
        }
        default:
            NOTSA_UNREACHABLE("Invalid soundStatus: {}", (int)soundStatus);
        }
    }
}

// 0x4DBDF0
void CAECollisionAudioEntity::ReportBulletHit(CEntity* entity, eSurfaceType surface, const CVector& posn, float angleWithColPointNorm) {
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_BULLET_HITS, SND_BANK_SLOT_BULLET_HITS)) {
        return;
    }
    if (entity && entity->GetIsTypeVehicle()) {
        surface = entity->AsVehicle()->IsSubBMX()
            ? (eSurfaceType)(188) // todo: C* Surface
            : SURFACE_CAR;
    }
    PlayBulletHitCollisionSound(surface, posn, angleWithColPointNorm);
}

// 0x4DA2C0
void CAECollisionAudioEntity::Service() {
    const auto time = CTimer::GetTimeInMS();
    for (auto& entry : m_CollisionSoundList) {
        if (entry.Status != COLLISION_SOUND_LOOPING || time < entry.LoopStopTimeMs)
            continue;

        entry = {};
        --m_NumActiveCollisionSounds;
    }
}

// 0x4DAA50
eSoundID CAECollisionAudioEntity::ChooseCollisionSoundID(eSurfaceType surface) {
    const auto* const l = &gCollisionLookup[surface];
    if (l->MinSoundID == l->MaxSoundID) {
        return l->MinSoundID;
    }
    while (true) {
        const auto soundID = CAEAudioUtility::GetRandomNumberInRange(l->MinSoundID, l->MaxSoundID);
        if (soundID != m_CollisionSoundIDHistory[surface]) {
            return soundID;
        }
    }
    NOTSA_UNREACHABLE();
}
