#include "StdInc.h"

#include "AECollisionAudioEntity.h"

#include "AEAudioHardware.h"
#include "AEAudioUtility.h"

notsa::mdarray<int32, TOTAL_NUM_SURFACE_TYPES + 16, 4> gCollisionLookup = {{ // No clue what the +16 is for
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 34, 34, 70, 100 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 34, 34, 70, 100 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { -1, -1, 0, 20 },
    { -1, -1, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 35, 37, 90, 50 },
    { 33, 33, 0, 50 },
    { 40, 40, 60, 40 },
    { -1, -1, 0, 30 },
    { -1, -1, 0, 30 },
    { -1, -1, 0, 30 },
    { -1, -1, 0, 0 },
    { -1, -1, 0, 0 },
    { 46, 46, 90, 80 },
    { 65, 65, 60, 100 },
    { 62, 62, 80, 100 },
    { 32, 32, 40, 100 },
    { 69, 71, 80, 40 },
    { 69, 71, 80, 40 },
    { 65, 65, 60, 100 },
    { 54, 54, 50, 100 },
    { 30, 30, 60, 100 },
    { 38, 38, 70, 60 },
    { 60, 60, 0, 30 },
    { 12, 14, 0, 40 },
    { -1, -1, 0, 20 },
    { 20, 28, 90, 100 },
    { 20, 28, 90, 100 },
    { 20, 28, 90, 100 },
    { -1, -1, 1, 20 },
    { 60, 60, 0, 30 },
    { 4, 4, 60, 30 },
    { 34, 34, 70, 100 },
    { 19, 19, 80, 50 },
    { 31, 31, 0, 20 },
    { 33, 33, 0, 50 },
    { 33, 33, 0, 50 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 31, 31, 0, 20 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 33, 33, 0, 50 },
    { 33, 33, 0, 50 },
    { 33, 33, 0, 50 },
    { 33, 33, 0, 50 },
    { 33, 33, 0, 50 },
    { 33, 33, 0, 50 },
    { -1, -1, 0, 20 },
    { -1, -1, 0, 20 },
    { -1, -1, 0, 20 },
    { -1, -1, 0, 20 },
    { -1, -1, 0, 20 },
    { 34, 34, 70, 100 },
    { 33, 33, 0, 50 },
    { 34, 34, 70, 100 },
    { 33, 33, 0, 50 },
    { 33, 33, 0, 50 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 34, 34, 70, 100 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 34, 34, 70, 100 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 34, 34, 70, 100 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 33, 33, 0, 50 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 34, 34, 70, 100 },
    { 31, 31, 0, 20 },
    { 34, 34, 70, 100 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { 31, 31, 0, 20 },
    { -1, -1, 0, 20 },
    { -1, -1, 0, 20 },
    { -1, -1, 0, 20 },
    { 31, 31, 0, 20 },
    { 33, 33, 0, 70 },
    { 4, 4, 60, 30 },
    { 31, 31, 0, 20 },
    { 34, 34, 70, 100 },
    { 65, 65, 60, 100 },
    { 31, 31, 0, 20 },
    { 65, 65, 60, 100 },
    { 34, 34, 70, 100 },
    { 10, 10, 40, 10 },
    { 63, 64, 50, 80 },
    { 55, 55, 60, 80 },
    { 59, 59, 30, 20 },
    { 4, 4, 60, 30 },
    { 39, 39, 50, 80 },
    { 45, 45, 70, 40 },
    { 40, 40, 70, 50 },
    { 40, 40, 70, 50 },
    { 51, 51, 30, 50 },
    { 52, 52, 50, 20 },
    { 47, 49, 30, 20 },
    { 65, 65, 60, 100 },
    { -1, -1, 0, 0 },
    { -1, -1, 0, 0 },
    { -1, -1, 0, 0 },
    { -1, -1, 0, 0 },
    { -1, -1, 0, 0 },
    { -1, -1, 0, 0 },
    { -1, -1, 0, 0 },
    { -1, -1, 0, 0 },
    { -1, -1, 0, 0 },
    { 5, 9, 90, 60 },
    { 11, 11, 0, 30 },
    { 61, 61, 0, 40 },
    { 50, 50, 0, 30 },
    { 0, 2, 0, 70 },
    { 0, 2, 0, 30 },
    { 4, 6, 0, 40 }
}};

void CAECollisionAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAECollisionAudioEntity, 0x862E64, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Initialise, 0x5B9BD0);
    RH_ScopedInstall(InitialisePostLoading, 0x4DA050);
    RH_ScopedInstall(AddCollisionSoundToList, 0x4DAAC0);
    RH_ScopedInstall(Reset, 0x4DA320);
    RH_ScopedInstall(ReportGlassCollisionEvent, 0x4DA070);
    RH_ScopedInstall(UpdateLoopingCollisionSound, 0x4DA540, { .reversed = false });
    RH_ScopedInstall(GetCollisionSoundStatus, 0x4DA830, { .reversed = true });
    RH_ScopedInstall(ReportObjectDestruction, 0x4DAB60);
    RH_ScopedInstall(PlayOneShotCollisionSound, 0x4DB150, { .reversed = false });
    RH_ScopedInstall(PlayLoopingCollisionSound, 0x4DB450);
    RH_ScopedInstall(PlayBulletHitCollisionSound, 0x4DB7C0, { .reversed = false });
    RH_ScopedInstall(ReportCollision, 0x4DBA10);
    RH_ScopedInstall(ReportBulletHit, 0x4DBDF0);
    RH_ScopedInstall(Service, 0x4DA2C0);

    RH_ScopedOverloadedInstall(ReportWaterSplash, "at-position", 0x4DA190, void(CAECollisionAudioEntity::*)(CVector, float), { .reversed = true });
    RH_ScopedOverloadedInstall(ReportWaterSplash, "for-physical", 0x4DAE40, void(CAECollisionAudioEntity::*)(CPhysical*, float, bool), { .reversed = false });
}

// 0x5B9BD0
void CAECollisionAudioEntity::Initialise() {
    *this = {};
}

// 0x4DA050
void CAECollisionAudioEntity::InitialisePostLoading() {
    AEAudioHardware.LoadSoundBank(39, SND_BANK_SLOT_COLLISIONS);
    AEAudioHardware.LoadSoundBank(27, 3);
}

// 0x4DA320
void CAECollisionAudioEntity::Reset() {
    for (auto& entry : m_Entries) {
        if (entry.m_nStatus != COLLISION_SOUND_LOOPING)
            continue;

        if (entry.m_Sound)
            entry.m_Sound->StopSoundAndForget();

        entry = {};
    }
}

// 0x4DAAC0
void CAECollisionAudioEntity::AddCollisionSoundToList(
    CEntity* entity1,
    CEntity* entity2,
    eSurfaceType surf1,
    eSurfaceType surf2,
    CAESound* sound,
    eCollisionSoundStatus status
)
{
    // Find an entry with no sound.
    const auto e = rng::find_if_not(m_Entries, &tCollisionAudioEntry::m_Sound);
    if (e == m_Entries.end()) {
        return;
    }

    e->m_Entity1    = entity1;
    e->m_Entity2    = entity2;

    e->m_nSurface1  = surf1;
    e->m_nSurface2  = surf2;

    e->m_Sound      = sound;
    e->m_nStatus    = status;
    e->m_nTime      = status == COLLISION_SOUND_LOOPING ? CTimer::GetTimeInMS() + 100 : 0;

    m_nActiveCollisionSounds++;
}

// 0x4DA830
eCollisionSoundStatus CAECollisionAudioEntity::GetCollisionSoundStatus(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, int32& outIndex) {
    for (auto&& [i, v] : rngv::enumerate(m_Entries)) {
        if (v.m_Entity1 != entity1 && v.m_Entity2 != entity2 && v.m_Entity1 != entity2 && v.m_Entity2 != entity1) {
            continue;
        }
        outIndex = (int32)i;
        return v.m_nStatus;
    }
    NOTSA_UNREACHABLE();
}

// 0x4DB150
void CAECollisionAudioEntity::PlayOneShotCollisionSound(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, float a5, const CVector& posn) {
    plugin::CallMethod<0x4DB150, CAECollisionAudioEntity*, CEntity*, CEntity*, uint8, uint8, float, const CVector&>(this, entity1, entity2, surf1, surf2, a5, posn);
}

// 0x4DB450
void CAECollisionAudioEntity::PlayLoopingCollisionSound(CEntity* entityA, CEntity* entityB, eSurfaceType surfA, eSurfaceType surfB, float force, const CVector& pos, uint8 isForceLooping) {
    const auto PlaySound = [&](float volume, float speed) {
        const auto GetSoundID = [&]() -> eSoundID { // 0x4DB6AF
            if (g_surfaceInfos.IsAudioGrass(surfA) || g_surfaceInfos.IsAudioGrass(surfB)) {
                return 0;
            } else if (g_surfaceInfos.IsAudioWater(surfA) || g_surfaceInfos.IsAudioWater(surfB)) {
                return 3;
            } else if (g_surfaceInfos.IsAudioMetal(surfA) || g_surfaceInfos.IsAudioMetal(surfB)) {
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
            AddCollisionSoundToList(entityA, entityB, surfA, surfB, sound, COLLISION_SOUND_LOOPING);
        }
    };
    const auto PlaySoundForCar = [&](CVector velocity, CVector turn) {
        const auto mag = std::sqrt(std::max(turn.SquaredMagnitude(), velocity.SquaredMagnitude()));
        if (mag == 0.f) {
            PlaySound(-100.f, 0.f);
        } else {
            auto speed = std::min(std::sqrt(mag * ((gCollisionLookup[surfA][3] * gCollisionLookup[surfB][3]) / 10000.f)) * 3.f, 0.f);
            if (isForceLooping) {
                speed /= 6.f;
            }
            PlaySound(GetDefaultVolume(AE_GENERAL_COLLISION) + CAEAudioUtility::AudioLog10(std::min(speed / 0.75f, 1.f) * 20.f), speed);
        }
    };
    if (surfA == SURFACE_CAR && surfB == SURFACE_CAR) {
        const auto vehA = entityA->AsVehicle(),
                   vehB = entityB->AsVehicle();
        PlaySoundForCar(vehA->GetMoveSpeed() - vehB->GetMoveSpeed(), vehA->GetTurnSpeed() - vehB->GetTurnSpeed());
    } else if ((surfB == SURFACE_PED || surfA != SURFACE_CAR) && (surfA == SURFACE_PED || surfB != SURFACE_CAR)) {
        PlaySound(-100.f, 0.f);
    } else {
        const auto vehA = entityA->AsVehicle();
        PlaySoundForCar(vehA->GetMoveSpeed(), vehA->GetTurnSpeed());
    }
}

// 0x4DA540
void CAECollisionAudioEntity::UpdateLoopingCollisionSound(CAESound *pSound, CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, float impulseForce, const CVector& position, bool bForceLooping) {
    plugin::CallMethod<0x4DA540>(this, pSound, entity1, entity2, surf1, surf2, impulseForce, &position, bForceLooping);
}

// 0x4DB7C0
void CAECollisionAudioEntity::PlayBulletHitCollisionSound(eSurfaceType surface, const CVector& posn, float angleWithColPointNorm) {
    if (surface >= SURFACE_UNKNOWN_194)
        return;

    int32 iRand;
    float maxDistance = 1.5f;
    float volume = GetDefaultVolume(AE_BULLET_HIT);
    if (surface == SURFACE_PED)
    {
        do
            iRand = CAEAudioUtility::GetRandomNumberInRange(7, 9);
        while (iRand == m_nLastBulletHitSoundID);
    }
    else if (g_surfaceInfos.IsAudioWater(surface))
    {
        do
            iRand = CAEAudioUtility::GetRandomNumberInRange(16, 18);
        while (iRand == m_nLastBulletHitSoundID);
        maxDistance = 2.0f;
        volume = volume + 6.0f;
    }
    else if (g_surfaceInfos.IsAudioWood(surface))
    {
        do
            iRand = CAEAudioUtility::GetRandomNumberInRange(19, 21);
        while (iRand == m_nLastBulletHitSoundID);
    }
    else if (g_surfaceInfos.IsAudioMetal(surface))
    {
        float probability = (90.0f - angleWithColPointNorm) / 180.0f; // see BoneNode_c::EulerToQuat
        if (CAEAudioUtility::ResolveProbability(probability))
        {
            do
                iRand = CAEAudioUtility::GetRandomNumberInRange(10, 12);
            while (iRand == m_nLastBulletHitSoundID);
        }
        else
        {
            do
                iRand = CAEAudioUtility::GetRandomNumberInRange(4, 6);
            while (iRand == m_nLastBulletHitSoundID);
        }
    } else if (g_surfaceInfos.IsAudioGravelConcreteOrTile(surface))
    {
        do
            iRand = CAEAudioUtility::GetRandomNumberInRange(13, 15);
        while (iRand == m_nLastBulletHitSoundID);
    }
    else
    {
        do
            iRand = CAEAudioUtility::GetRandomNumberInRange(1, 3);
        while (iRand == m_nLastBulletHitSoundID);
    }

    if (iRand >= 0) {
        CAESound sound;
        sound.Initialise(3, iRand, this, posn, volume, maxDistance, 1.0f, 1.0f, 0, SOUND_DEFAULT, 0.02f, 0);
        AESoundManager.RequestNewSound(&sound);
        m_nLastBulletHitSoundID = iRand;
    }
}

// 0x4DA070
void CAECollisionAudioEntity::ReportGlassCollisionEvent(eAudioEvents glassSoundType, Const CVector& posn, uint32 time) {
    auto speed = 1.0f;
    const auto sfxId = [glassSoundType, &speed] {
        switch (glassSoundType) {
        case AE_GLASS_HIT:
            return 51;
        case AE_GLASS_CRACK:
            return 68;
        case AE_GLASS_BREAK_SLOW:
            speed = 0.75f;
            return 56;
        case AE_GLASS_BREAK_FAST:
            return 56;
        case AE_GLASS_HIT_GROUND:
            return CAEAudioUtility::GetRandomNumberInRange(15, 18);
        case AE_GLASS_HIT_GROUND_SLOW:
            speed = 0.56f;
            return CAEAudioUtility::GetRandomNumberInRange(15, 18);
        default:
            return -1; // Invalid audio event
        }
    }();

    if (sfxId == -1)
        return;

    m_tempSound.Initialise(
        2,
        sfxId,
        this,
        posn,
        GetDefaultVolume(glassSoundType),
        1.5f,
        speed
    );

    if (time) {
        auto& snd = m_tempSound;
        snd.m_ClientVariable = (float)(time + CTimer::GetTimeInMS());
        snd.m_nEvent = glassSoundType;
        snd.m_bRequestUpdates = true;
    }
}

// 0x4DA190
void CAECollisionAudioEntity::ReportWaterSplash(CVector posn, float volume) {
    if (!AEAudioHardware.IsSoundBankLoaded(39, 2)) {
        if (!AudioEngine.IsLoadingTuneActive())
            AEAudioHardware.LoadSoundBank(39, 2);

        return;
    }

    m_tempSound.Initialise(
        2,
        67,
        this,
        posn,
        GetDefaultVolume(AE_WATER_SPLASH) + volume,
        2.5f,
        1.26f,
        1.0f,
        0u,
        SOUND_REQUEST_UPDATES
    );
    m_tempSound.m_nEvent = AE_FRONTEND_SELECT;
    AESoundManager.RequestNewSound(&m_tempSound);

    m_tempSound.Initialise(
        2,
        66,
        this,
        posn,
        GetDefaultVolume(AE_WATER_SPLASH) + volume,
        2.5f,
        0.0f,
        1.0f,
        0u,
        SOUND_REQUEST_UPDATES
    );
    m_tempSound.m_nEvent = AE_FRONTEND_BACK;
    m_tempSound.m_ClientVariable = static_cast<float>(CTimer::GetTimeInMS() + 166);
}

// 0x4DAE40
void CAECollisionAudioEntity::ReportWaterSplash(CPhysical* physical, float height, bool splashMoreThanOnce) {
    return plugin::CallMethod<0x4DAE40, CAECollisionAudioEntity*, CPhysical*, float, bool>(this, physical, height, splashMoreThanOnce);
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
    eSurfaceType   surfA,
    eSurfaceType   surfB,
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

    surfA = GetSurfaceToUse(entityA, surfA, entityB, surfB);
    surfB = GetSurfaceToUse(entityB, surfB, entityA, surfA);

    if (isForceOneShot) { // 0x4DBC68
        PlayOneShotCollisionSound(entityA, entityB, surfA, surfB, impulseForce, pos);
    } else  { // 0x4DBC83
        int32 soundIdx;
        switch (const auto soundStatus = CAECollisionAudioEntity::GetCollisionSoundStatus(entityA, entityB, surfA, surfB, soundIdx)) {
        case COLLISION_SOUND_INACTIVE: {
            if (!isForceLooping) {
                const auto e = &m_Entries[soundIdx];
                return PlayOneShotCollisionSound(e->m_Entity1, e->m_Entity2, e->m_nSurface1, e->m_nSurface2, impulseForce, pos);
            }
            return PlayLoopingCollisionSound(entityA, entityB, surfA, surfB, impulseForce, pos, isForceLooping);
        }
        case COLLISION_SOUND_ONE_SHOT:
            return PlayLoopingCollisionSound(entityA, entityB, surfA, surfB, impulseForce, pos, isForceLooping);
        case COLLISION_SOUND_LOOPING: {
            const auto e = &m_Entries[soundIdx];
            e->m_nTime = CTimer::GetTimeInMS() + 100;
            if (e->m_Sound) {
                UpdateLoopingCollisionSound(e->m_Sound, e->m_Entity1, e->m_Entity2, e->m_nSurface1, e->m_nSurface2, impulseForce, pos, isForceLooping);
            }
        }
        default:
            NOTSA_UNREACHABLE("Invalid soundStatus: {}", (int)soundStatus);
        }
    }
}

// 0x4DBDF0
void CAECollisionAudioEntity::ReportBulletHit(CEntity* entity, eSurfaceType surface, const CVector& posn, float angleWithColPointNorm) {
    if (AEAudioHardware.IsSoundBankLoaded(27, 3)) {
        if (entity && entity->IsVehicle()) {
            surface = entity->AsVehicle()->IsSubBMX()
                ? eSurfaceType(188) // todo: C* Surface
                : SURFACE_CAR; 
        }
        PlayBulletHitCollisionSound(surface, posn, angleWithColPointNorm);
    }
}

// 0x4DA2C0
void CAECollisionAudioEntity::Service() {
    const auto time = CTimer::GetTimeInMS();
    for (auto& entry : m_Entries) {
        if (entry.m_nStatus != COLLISION_SOUND_LOOPING || time < entry.m_nTime)
            continue;

        entry = {};
        --m_nActiveCollisionSounds;
    }
}
