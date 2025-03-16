#include "StdInc.h"

#include "AEAudioUtility.h"
#include "AEGlobalWeaponAudioEntity.h"
#include "AEAudioHardware.h"

CAESound* (&pWaterfall)[3] = *(CAESound * (*)[3])0xB612F0;
CAESound*& pFogHorn = *(CAESound**)0xB612FC;

// 0x5075B0
CAEGlobalWeaponAudioEntity::CAEGlobalWeaponAudioEntity() : CAEWeaponAudioEntity() {
    m_Physical = new CPhysical();
}

// 0x506C50
CAEGlobalWeaponAudioEntity::~CAEGlobalWeaponAudioEntity() {
    delete m_Physical;
}

// 0x4DEF90
void CAEGlobalWeaponAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos != -1)
        return;

    if (sound == pFogHorn) {
        pFogHorn = nullptr;
        return;
    }

    for (auto& waterfall : pWaterfall) {
        if (sound == waterfall) {
            waterfall = nullptr;
        }
    }
}

// 0x4DFAA0
void CAEGlobalWeaponAudioEntity::AddAudioEvent(eAudioEvents audioId, eWeaponType weaponType, CPhysical* entity) {
    if (!entity) {
        return;
    }

    switch (audioId) {
    case AE_WEAPON_FIRE:
    case AE_WEAPON_FIRE_PLANE: return WeaponFire(weaponType, entity, audioId);
    case AE_WEAPON_RELOAD_A:
    case AE_WEAPON_RELOAD_B:   return WeaponReload(weaponType, entity, audioId);
    case AE_PROJECTILE_FIRE:   return ProjectileFire(weaponType, entity, audioId);
    default:                   return;
    }
}

// 0x4DF060
void CAEGlobalWeaponAudioEntity::ProjectileFire(eWeaponType weaponType, CPhysical* physical, eAudioEvents event) {
    switch (weaponType) {
    case WEAPON_ROCKET:
    case WEAPON_ROCKET_HS:
        break;
    default:
        return;
    }

    constexpr float gfRocketFrequencyVariations[] = { 1.08f, 1.0f, 0.92f, 3.3f };// 0x8AE5AC
    m_FrequencyVariation = (m_FrequencyVariation + 1) % (std::size(gfRocketFrequencyVariations) - 1);

    const auto PlayRocketSound = [&](int16 bankSlotID, eSoundID soundID, float speedMult, float volumeOffsetdB) {
        AESoundManager.PlaySound({
            .BankSlotID = bankSlotID,
            .SoundID = soundID,
            .AudioEntity = this,
            .Pos = physical->GetPosition(),
            .Volume = GetDefaultVolume(event) + volumeOffsetdB,
            .RollOffFactor = 3.0f,
            .Speed = gfRocketFrequencyVariations[m_FrequencyVariation] * speedMult,
            .Flags = SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY,
            .FrequencyVariance = 0.02f,
            .RegisterWithEntity = physical
            });
        };

    if (!AEAudioHardware.IsSoundBankLoaded(143, 5)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(143, 5);
        }
        return;
    }
    PlayRocketSound(5, 68, 1.f, -8.f);

    if (!AEAudioHardware.IsSoundBankLoaded(138, 19)) {
        AEAudioHardware.LoadSoundBank(138, 19);
        return;
    }
    PlayRocketSound(19, 26, 1.25f, 0.f);
}

// 0x4DF210
void CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire() {
    constexpr CVector s_FogHornPositions[]{
        {-2683.f, 1445.f, 37.f}, // 0x4DF271
        {-2683.f, 1804.f, 69.f}, // 0x4DF29F
    };
    constexpr int32 FOGHORN_FADE_OUT_DELAY = 5000;
    constexpr int32 FOGHORN_STOP_DELAY     = 5000; // time to live

    constexpr CVector s_WaterfallPositions[]{
        {2075.3f, 1925.9f, 13.0f}, // 0x4DF2C9
        {2085.8f, 1905.5f, 11.3f}, // 0x4DF2FD
        {2091.5f, 1888.9f, 11.1f}, // 0x4DF31D
    };

    enum eState : int32 {
        INITIAL,
        GUNSHOTS,
        FOGHORN_0,
        STATE_3,
        FOGHORN_1,
        WATERFALL_LV, // Only in LV
    };
    static auto& s_State = StaticRef<eState>(0xB61324);

    static auto& s_GunShots   = StaticRef<int32>(0xB61318);
    static auto& s_WeaponType = StaticRef<eWeaponType>(0xB6131C);
    static auto& s_Delay      = StaticRef<uint32>(0xB61320);
    static auto& s_LastTime   = ScopedStaticRef<uint32>(0xB61364, 0xB61368, 0x1, CTimer::GetTimeInMS());

    switch (s_State) {
    case eState::INITIAL: {
        switch (CWeather::WeatherRegion) {
        case eWeatherRegion::WEATHER_REGION_LA: { // 0x4DF35A
            if (CTimer::GetFrameCounter() % 1024) { // TODO: Maybe use a timer or something?
                return;
            }

            const auto a = CGeneral::GetRandomNumberInRange(0.f, TWO_PI);
            const auto r = CGeneral::GetRandomNumberInRange(40.f, 80.f);
            CVector pos = TheCamera.GetPosition() + CVector{ std::cos(a) * r, std::sin(a) * r, 0.f };
            pos.z = std::min(pos.z, 20.f);
            m_Physical->SetPosn(pos);

            PickAmbientGunFire(s_GunShots, s_WeaponType, s_Delay);

            s_State = eState::GUNSHOTS;

            break;
        }
        case WEATHER_REGION_LV: { // 0x4DF580
            if (!AEAudioHardware.IsSoundBankLoaded(39u, 2)) {
                return;
            }
            if (!rng::all_of(pWaterfall, notsa::IsNull{})) {
                return;
            }
            if (CVector::DistSqr(s_WaterfallPositions[1], TheCamera.GetPosition()) >= sq(100.f)) {
                return;
            }
            const auto PlayWaterfallSound = [&](int32 i, float speed) {
                pWaterfall[i] = AESoundManager.PlaySound({
                    .BankSlotID    = 2,
                    .SoundID       = 3,
                    .AudioEntity   = this,
                    .Pos           = s_WaterfallPositions[i],
                    .Volume        = -3.f,
                    .RollOffFactor = 2.5f,
                    .Speed         = speed,
                    .Flags         = SOUND_REQUEST_UPDATES
                });
            };
            constexpr auto SPEED_BASE = 1.f;
            constexpr auto SPEED_VAR  = 0.01f;
            PlayWaterfallSound(0, SPEED_BASE);
            PlayWaterfallSound(1, SPEED_BASE + SPEED_VAR);
            PlayWaterfallSound(2, SPEED_BASE - SPEED_VAR);

            s_State = eState::WATERFALL_LV;

            break;
        }
        case WEATHER_CLOUDY_LA: {
            break;
        }
        default: { // 0x4DF490
            if (CWeather::Foggyness_SF < 0.5) {
                return;
            }
            if (CTimer::GetFrameCounter() % 1024) { // TODO: Maybe use a timer or something?
                return;
            }
            if (CVector::DistSqr(TheCamera.GetPosition(), s_FogHornPositions[0]) > sq(2000.f)) {
                return;
            }
            if (pFogHorn) {
                return;
            }

            pFogHorn = AESoundManager.PlaySound({
                .BankSlotID    = 17,
                .SoundID       = 13,
                .AudioEntity   = this,
                .Pos           = s_FogHornPositions[0],
                .RollOffFactor = 60.f,
                .Flags         = SOUND_REQUEST_UPDATES
            });

            s_State = eState::FOGHORN_0;
            s_LastTime = CTimer::GetTimeInMS();

            break;
        }
        }
        break;
    }
    case eState::GUNSHOTS: { // 0x4DF769
        if (s_LastTime + s_Delay >= CTimer::GetTimeInMS()) {
            return;
        }

        if (CVector::DistSqr(FindPlayerCoors(), m_Physical->GetPosition()) <= sq(24.f)) {
            m_Physical->SetType(ENTITY_TYPE_NOTINPOOLS);
            AudioEngine.ReportWeaponEvent(AE_WEAPON_FIRE, s_WeaponType, m_Physical);

            if (--s_GunShots) { // 0x4DF862
                assert(s_WeaponType == WEAPON_PISTOL || s_WeaponType == WEAPON_AK47); // See `PickAmbientGunFire`
                s_Delay = s_WeaponType == WEAPON_PISTOL
                    ? CAEAudioUtility::GetRandomNumberInRange(500, 1200)
                    : CAEAudioUtility::GetRandomNumberInRange(90, 450);
            } else if (CGeneral::RandomBool(60.f)) { // 0x4DF7E9
                s_State = eState::INITIAL;
            } else { // 0x4DF849
                PickAmbientGunFire(s_GunShots, s_WeaponType, s_Delay);
            }
        } else {
            s_State = eState::INITIAL;
        }
        s_LastTime = CTimer::GetTimeInMS();

        break;
    }
    case eState::FOGHORN_0:   // 0x4DF8A4
    case eState::FOGHORN_1: { // 0x4DF98A
        if (CTimer::GetTimeInMS() > s_LastTime + FOGHORN_STOP_DELAY) {
            if (pFogHorn) {
                pFogHorn->StopSound();
            }
            s_State = s_State == eState::FOGHORN_0
                ? eState::STATE_3   // FOGHORN_0 -> S3
                : eState::INITIAL;  // FOGHORN_1 -> S0
        } else if (CTimer::GetTimeInMS() > s_LastTime + FOGHORN_FADE_OUT_DELAY) { // BUG: Since `FOGHORN_TTL` and `FOGHORN_FADE_OUT_DELAY` are both 5000 the `if` here will never be true.
            if (pFogHorn) {
                pFogHorn->m_fVolume -= 3.3f;
            }
        }
        break;
    }
    case eState::STATE_3: { // 0x4DF8D1
        if (s_LastTime + 6500 < CTimer::GetTimeInMS()) {
            return;
        }
        if (!AEAudioHardware.IsSoundBankLoaded(59, 0)) { // BUG: Pretty sure the `bankSlotId` param should be 17 here
            return;
        }
        if (!pFogHorn) {
            pFogHorn = AESoundManager.PlaySound({
                .BankSlotID    = 17,
                .SoundID       = 13,
                .AudioEntity   = this,
                .Pos           = s_FogHornPositions[1],
                .RollOffFactor = 60.f,
                .Speed         = 1.2f,
                .Flags         = SOUND_REQUEST_UPDATES
            });

            s_State    = eState::FOGHORN_1;
            s_LastTime = CTimer::GetTimeInMS();
        }
        break;
    }
    case eState::WATERFALL_LV: { // 0x4DF9D8
        if (CWeather::WeatherRegion != eWeatherRegion::WEATHER_REGION_LV || CVector::DistSqr(TheCamera.GetPosition(), s_WaterfallPositions[1]) > sq(100.f)) {
            for (auto& s : pWaterfall) {
                if (s) {
                    std::exchange(s, nullptr)->StopSound();
                }
            }
            s_State = eState::INITIAL;
        }
        break;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

void CAEGlobalWeaponAudioEntity::PickAmbientGunFire(int32& gunShots, eWeaponType& weaponType, uint32& delay) {
    const auto b = CGeneral::DoCoinFlip();
    gunShots     = b ? CAEAudioUtility::GetRandomNumberInRange(2, 4)       : CAEAudioUtility::GetRandomNumberInRange(4, 14);
    weaponType   = b ? WEAPON_PISTOL                                               : WEAPON_AK47;
    delay        = b ? CAEAudioUtility::GetRandomNumberInRange(500, 1'200) : CAEAudioUtility::GetRandomNumberInRange(90, 350);
    
}

void CAEGlobalWeaponAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEGlobalWeaponAudioEntity, 0x862E5C, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x5075B0);
    RH_ScopedInstall(Destructor, 0x506C50);
    RH_ScopedInstall(UpdateParameters, 0x4DEF90);
    RH_ScopedInstall(AddAudioEvent, 0x4DFAA0);
    RH_ScopedInstall(ProjectileFire, 0x4DF060);
    RH_ScopedInstall(ServiceAmbientGunFire, 0x4DF210);
}

CAEGlobalWeaponAudioEntity* CAEGlobalWeaponAudioEntity::Constructor() {
    this->CAEGlobalWeaponAudioEntity::CAEGlobalWeaponAudioEntity();
    return this;
}
CAEGlobalWeaponAudioEntity* CAEGlobalWeaponAudioEntity::Destructor() { this->CAEGlobalWeaponAudioEntity::~CAEGlobalWeaponAudioEntity(); return this; }
