#include "StdInc.h"

#include "AECollisionAudioEntity.h"

#include "AEAudioHardware.h"
#include "AEAudioUtility.h"

#include "eAudioBank.h"
#include "eAudioSlot.h"

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
    RH_ScopedInstall(PlayLoopingCollisionSound, 0x4DB450, { .reversed = false });
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
    AEAudioHardware.LoadSoundBank(39, BANK_SLOT_COLLISIONS);
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
    for (auto&& [i, v] : notsa::enumerate(m_Entries)) {
        if (v.m_Entity1 != entity1 && v.m_Entity2 != entity2 && v.m_Entity1 != entity2 && v.m_Entity2 != entity1) {
            continue;
        }
        outIndex = (int32)i;
        return v.m_nStatus;
    }
    NOTSA_UNREACHABLE();
}

// 0x4DB150
void CAECollisionAudioEntity::PlayOneShotCollisionSound(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, float a5, CVector& posn) {
    plugin::CallMethod<0x4DB150, CAECollisionAudioEntity*, CEntity*, CEntity*, uint8, uint8, float, CVector&>(this, entity1, entity2, surf1, surf2, a5, posn);
}

// 0x4DB450
void CAECollisionAudioEntity::PlayLoopingCollisionSound(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, float a5, CVector& posn, uint8 a7) {
    plugin::CallMethod<0x4DB450, CAECollisionAudioEntity*, CEntity*, CEntity*, uint8, uint8, float, CVector&, uint8>(this, entity1, entity2, surf1, surf2, a5, posn, a7);
}

// 0x4DA540
void CAECollisionAudioEntity::UpdateLoopingCollisionSound(CAESound *pSound, CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, float impulseForce, CVector& position, bool bForceLooping) {
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
        snd.m_fMaxVolume = (float)(time + CTimer::GetTimeInMS());
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
    m_tempSound.m_fMaxVolume = static_cast<float>(CTimer::GetTimeInMS() + 166);
}

// 0x4DAE40
void CAECollisionAudioEntity::ReportWaterSplash(CPhysical* physical, float height, bool splashMoreThanOnce) {
    return plugin::CallMethod<0x4DAE40, CAECollisionAudioEntity*, CPhysical*, float, bool>(this, physical, height, splashMoreThanOnce);
}

// 0x4DAB60
void CAECollisionAudioEntity::ReportObjectDestruction(CEntity* entity) {
    if (entity) {
        const auto surface = [&]{
            if (entity->GetModelID() == MODEL_MOLOTOV) { // 0x4DAB7B
                return SURFACE_GLASS;
            }
            if (entity->GetModelID() == ModelIndices::MI_GRASSHOUSE) { // 0x4DAB89
                const auto cd = entity->GetColData();
                if (!cd) {
                    return SURFACE_NONE;
                }
                if (const auto spheres = cd->GetSpheres(); !spheres.empty()) {
                    return spheres[0].m_Surface.m_nMaterial;
                }
                if (const auto boxes = cd->GetBoxes(); !boxes.empty()) {
                    return boxes[0].m_Surface.m_nMaterial;
                }
                if (const auto tris = cd->GetTris(); !tris.empty()) {
                    return tris[0].GetSurfaceType();
                }
            }
        }();
    }
}

// 0x4DBA10
void CAECollisionAudioEntity::ReportCollision(
    CEntity* entity1,
    CEntity* entity2,
    eSurfaceType surf1,
    eSurfaceType surf2,
    CVector& position,
    CVector* normal,
    float impulseForce,
    float relVelSq,
    bool bForceOneShot,
    bool bForceLooping
) {
    if (!AEAudioHardware.IsSoundBankLoaded(BANK_GENRL_COLLISIONS, BANK_SLOT_COLLISIONS)) {
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

    surf1 = GetSurfaceToUse(entity1, surf1, entity2, surf2);
    surf2 = GetSurfaceToUse(entity2, surf2, entity1, surf1);

    if (bForceOneShot) { // 0x4DBC68
        PlayOneShotCollisionSound(entity1, entity2, surf1, surf2, impulseForce, position);
    } else  { // 0x4DBC83
        int32 soundIdx;
        switch (const auto soundStatus = CAECollisionAudioEntity::GetCollisionSoundStatus(entity1, entity2, surf1, surf2, soundIdx)) {
        case COLLISION_SOUND_INACTIVE: {
            if (!bForceLooping) {
                const auto e = &m_Entries[soundIdx];
                return PlayOneShotCollisionSound(e->m_Entity1, e->m_Entity2, e->m_nSurface1, e->m_nSurface2, impulseForce, position);
            }
            [[fallthrough]];
        }
        case COLLISION_SOUND_ONE_SHOT:
            return PlayLoopingCollisionSound(entity1, entity2, surf1, surf2, impulseForce, position, bForceLooping);
        case COLLISION_SOUND_LOOPING: {
            const auto e = &m_Entries[soundIdx];
            e->m_nTime = CTimer::GetTimeInMS() + 100;
            if (e->m_Sound) {
                UpdateLoopingCollisionSound(e->m_Sound, e->m_Entity1, e->m_Entity2, e->m_nSurface1, e->m_nSurface2, impulseForce, position, bForceLooping);
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
