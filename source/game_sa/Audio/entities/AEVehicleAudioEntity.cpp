#include "StdInc.h"

#include "AEVehicleAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"
#include "AESoundManager.h"
#include "AEAudioUtility.h"

#include <Enums/eSoundBankSlot.h>
#include <DamageManager.h> // tComponent
#include <reversiblebugfixes/Bugs.hpp>

constexpr size_t HORN_NUM_PATTERNS = 8;
constexpr size_t HORN_PATTERN_SIZE = 44;
constexpr notsa::mdarray<bool, HORN_NUM_PATTERNS, HORN_PATTERN_SIZE> HornPattern = {{ // 0x862B9C
    { false, false, true, true, true, false, false, false, false, true, true, true, true, false, false, false, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, false, false },
    { false, false, true, true, true, true, true, true, true, true, true, true, false, false, false, false, true, true, true, true, true, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false },
    { false, false, true, true, true, true, true, false, false, false, false, true, true, true, true, false, false, false, false, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, true, true, true, true, true, false, false },
    { false, false, true, true, true, true, false, false, false, false, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false },
    { false, false, false, false, false, false, false, false, false, false, true, true, false, false, false, false, true, true, false, false, false, false, true, true, true, false, false, false, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false },
    { false, false, true, true, true, true, false, false, false, false, true, true, true, true, true, false, false, false, false, true, true, true, false, false, false, false, true, true, true, false, false, false, false, false, true, true, true, true, true, true, true, true, false, false },
    { false, false, true, true, true, true, false, false, false, false, true, true, true, true, true, false, false, false, false, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, false, false },
    { true, true, true, true, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, true, true, true, true, true, false, true, true, true, true, true, true, true, true, false, false, false, false }
}};

bool IsSurfaceAudioGrass(eSurfaceType surface) {
    return g_surfaceInfos.IsAudioGrass(surface) || g_surfaceInfos.IsAudioLongGrass(surface);
}

bool IsSurfaceWet(eSurfaceType surface) {
    return g_surfaceInfos.IsAudioGravel(surface) || g_surfaceInfos.IsAudioSand(surface) || g_surfaceInfos.IsAudioWater(surface);
}

void GracefullyStopSound(CAESound*& sound) {
    if (auto* const s = std::exchange(sound, nullptr)) {
        s->SetFlags(SOUND_REQUEST_UPDATES, false);
        s->StopSound();
    }
}

void StopAndForgetSound(CAESound*& sound) {
    if (auto* const s = std::exchange(sound, nullptr)) {
        s->StopSoundAndForget();
    }
}

std::optional<uint32> GetNumOfContactWheels(CAEVehicleAudioEntity::tVehicleParams& vp) {
    switch (vp.BaseVehicleType) {
    case VEHICLE_TYPE_AUTOMOBILE: return vp.Vehicle->AsAutomobile()->GetNumContactWheels();
    case VEHICLE_TYPE_BIKE:       return vp.Vehicle->AsBike()->GetNumContactWheels();
    default:                      return std::nullopt;
    }
}

// 0x4F63E0
CAEVehicleAudioEntity::CAEVehicleAudioEntity() :
    CAEAudioEntity(),
    m_SkidSound() {
    m_IsInitialized = false;
}

// 0x6D0A10
CAEVehicleAudioEntity::~CAEVehicleAudioEntity() {
    if (m_IsInitialized) {
        Terminate();
    }
}

// 0x4F7670
void CAEVehicleAudioEntity::Initialise(CEntity* entity) {
    assert(entity && entity->GetIsTypeVehicle());

    m_TimeLastServiced            = 0;
    m_Entity                      = entity;
    m_IsPlayerDriver              = false;
    m_IsPlayerPassenger           = false;
    m_IsPlayerDriverAboutToExit   = false;
    m_IsWreckedVehicle            = false;
    m_State                       = eAEState::CAR_OFF;
    m_AuGear                      = 0;
    m_CrzCount                    = 0;
    m_DummySlot                   = SND_BANK_SLOT_NONE;
    m_RainDropCounter             = 0;
    m_DoCountStalls               = 0;
    m_StalledCount                = 0;
    m_SwapStalledTime             = 0;
    m_IsSilentStalled             = false;
    m_TimeSplashLastTriggered     = 0;
    m_TimeBeforeAllowAccelerate   = 0;
    m_TimeBeforeAllowCruise       = 0;
    m_bNitroOnLastFrame           = false;
    m_IsHelicopterDisabled        = false;
    m_ACPlayPositionThisFrame     = -1;
    m_ACPlayPositionLastFrame     = -1;
    m_ACPlayPositionWhenStopped   = 0;
    m_ACPlayPercentWhenStopped    = 0;
    m_FramesAgoACLooped           = 0;
    m_HornVolume                  = -100.0f;
    m_CurrentDummyEngineVolume    = -100.0f;
    m_SurfaceSoundType            = -1;
    m_RoadNoiseSoundType          = -1;
    m_FlatTireSoundType           = -1;
    m_ReverseGearSoundType        = -1;
    m_CurrentDummyEngineFrequency = -1.0f;
    m_CurrentRotorFrequency       = -1.0f;
    m_CurrentNitroRatio           = -1.0f;
    m_SurfaceSound                = nullptr;
    m_RoadNoiseSound              = nullptr;
    m_FlatTireSound               = nullptr;
    m_ReverseGearSound            = nullptr;
    m_HornSound                   = nullptr;
    m_SirenSound                  = nullptr;
    m_FastSirenSound              = nullptr;
    m_MovingPartSmoothedSpeed     = 0.0f;
    m_FadeIn                      = 1.0f;
    m_FadeOut                     = 0.0f;
    m_AuSettings                  = GetVehicleAudioSettings(entity->GetModelIndex() - MODEL_VEHICLE_FIRST);
    m_HasSiren                    = entity->AsVehicle()->UsesSiren();

    for (uint32 i = 0; auto& es : m_EngineSounds) {
        es = { .EngineSoundType = i++ };
    }

    if (m_AuSettings.RadioType == eAERadioType::AE_RT_UNKNOWN) {
        m_AuSettings.RadioStation = eRadioID::RADIO_OFF;
    }

    m_EventVolume = GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND);
    m_IsSingleGear = notsa::contains({ // TODO: Add handling flag for this
        MODEL_PIZZABOY,
        MODEL_CADDY,
        MODEL_FAGGIO,
        MODEL_BAGGAGE,
        MODEL_FORKLIFT,
        MODEL_VORTEX,
        MODEL_KART,
        MODEL_MOWER,
        MODEL_SWEEPER,
        MODEL_TUG,
    }, entity->GetModelId());

    switch (m_AuSettings.VehicleAudioType) {
    case AE_CAR: {
        m_EventVolume -= s_Config.VolOffsetOnGround;
        m_PlayerEngineBank = m_AuSettings.PlayerBank;
        m_DummyEngineBank  = m_AuSettings.DummyBank;
        if (m_IsInitialized) {
            return;
        }

        if (!notsa::contains<eSoundBank>({ SND_BANK_UNK, SND_BANK_GENRL_TEMP_D }, m_AuSettings.DummyBank)) {
            m_DummySlot = RequestBankSlot(m_AuSettings.DummyBank);
        }

        m_IsInitialized = true;
        return;
    }
    case AE_BIKE:
    case AE_BMX: {
        m_PlayerEngineBank = m_AuSettings.PlayerBank;

        if (m_AuSettings.IsBike()) {
            m_EventVolume = m_EventVolume - s_Config.VolOffsetOnGround;
        }

        if (m_IsInitialized) {
            return;
        }

        m_DummyEngineBank = m_AuSettings.DummyBank;
        if (m_DummyEngineBank != SND_BANK_UNK) {
            m_DummySlot = RequestBankSlot(m_AuSettings.DummyBank);
        }

        m_IsInitialized = true;
        return;
    }
    case AE_BOAT:
    case AE_TRAIN: {
        m_PlayerEngineBank = m_AuSettings.PlayerBank;
        m_DummyEngineBank  = m_AuSettings.DummyBank;
        if (m_IsInitialized) {
            return;
        }

        if (!notsa::contains<eSoundBank>({ SND_BANK_UNK, SND_BANK_GENRL_TEMP_D }, m_AuSettings.DummyBank)) {
            m_DummySlot = RequestBankSlot(m_AuSettings.DummyBank);
        }

        m_IsInitialized = true;
        return;
    }
    case AE_AIRCRAFT_HELICOPTER:
    case AE_AIRCRAFT_SEAPLANE: {
        m_DummyEngineBank  = m_AuSettings.DummyBank;
        m_PlayerEngineBank = m_AuSettings.PlayerBank;

        m_IsInitialized    = true;
        return;
    }
    case AE_AIRCRAFT_PLANE: {
        m_DummyEngineBank  = m_AuSettings.DummyBank;
        m_PlayerEngineBank = m_AuSettings.PlayerBank;
        if (m_IsInitialized) {
            return;
        }

        if (m_AuSettings.DummyBank != SND_BANK_UNK) {
            m_DummySlot = RequestBankSlot(m_AuSettings.DummyBank);
        }

        m_IsInitialized = true;
        return;
    }
    case AE_SPECIAL: {
        m_PlayerEngineBank = m_AuSettings.PlayerBank;
        m_EventVolume      = m_EventVolume - 1.5F;
        if (m_IsInitialized) {
            return;
        }

        m_DummyEngineBank = m_AuSettings.DummyBank;
        if (m_DummyEngineBank != SND_BANK_UNK) {
            m_DummySlot = RequestBankSlot(m_AuSettings.DummyBank);
        }

        m_IsInitialized = true;
        return;
    }
    }
}

// 0x5B99F0
void CAEVehicleAudioEntity::StaticInitialise() {
    rng::fill(s_DummyEngineSlots, tDummyEngineSlot{});

    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_APACHE_D, SND_BANK_SLOT_COP_HELI);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_HORN, SND_BANK_SLOT_HORN_AND_SIREN);

    s_pVehicleAudioSettingsForRadio = nullptr;
    s_pPlayerAttachedForRadio       = nullptr;
    s_pPlayerDriver                 = nullptr;
    s_NextDummyEngineSlot           = 0;
    s_HelicoptorsDisabled           = false;
}

// 0x4FB8C0
void CAEVehicleAudioEntity::Terminate() {
    if (!m_IsInitialized) {
        return;
    }

    // Stop sounds
    PlayTrainBrakeSound(-1);
    rng::for_each(m_EngineSounds, GracefullyStopSound, &tEngineSound::Sound);
    StopNonEngineSounds();

    if (m_IsPlayerDriver) {
        TurnOffRadioForVehicle();
    }

    // 0x4FBA43
    if (const auto s = std::exchange(m_DummySlot, SND_BANK_SLOT_NONE); s != SND_BANK_SLOT_NONE) {
        StoppedUsingBankSlot(s);
    }

    m_DummyEngineBank  = SND_BANK_UNK;
    m_PlayerEngineBank = SND_BANK_UNK;
    m_Entity           = nullptr;
    m_State            = eAEState::CAR_OFF;

    if (m_IsPlayerDriver) {
        m_IsPlayerDriver = false;
        s_pPlayerDriver  = nullptr;
    } else if (m_IsPlayerPassenger) {
        m_IsPlayerPassenger = false;
    }

    // OG | Replaced by TurnOffRadioForVehicle
    // s_pPlayerAttachedForRadio = nullptr;
    // s_pVehicleAudioSettingsForRadio = nullptr;

    m_IsPlayerDriverAboutToExit = false;
    m_IsInitialized            = false;
}

// 0x4F4ED0 - May return nullptr
tVehicleAudioSettings* CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio() {
    return s_pVehicleAudioSettingsForRadio;
}

// 0x4F4EE0
void CAEVehicleAudioEntity::EnableHelicoptors() {
    s_HelicoptorsDisabled = true;
}

// 0x4F4EF0
void CAEVehicleAudioEntity::DisableHelicoptors() {
    s_HelicoptorsDisabled = false;
}

// 0x4F5C00
void CAEVehicleAudioEntity::EnableHelicoptor() {
    m_IsHelicopterDisabled = false;
}

// 0x4F5BF0
void CAEVehicleAudioEntity::DisableHelicoptor() {
    m_IsHelicopterDisabled = true;
}

#pragma region Dummy Bank Slot Management
// 0x4F4E60
eSoundBankSlot CAEVehicleAudioEntity::DemandBankSlot(eSoundBank bankId) {
    // Try asking nicely first, maybe there's a slot we can use
    if (const auto s = RequestBankSlot(bankId); s != SND_BANK_SLOT_NONE) {
        return s;
    }

    // Otherwise free up the least used slot
    const auto slot = rng::min_element(s_DummyEngineSlots, std::less<>{}, &tDummyEngineSlot::RefCnt);
    slot->RefCnt = 0;

    // Now this should always return a valid slot
    return RequestBankSlot(bankId);
}

// 0x4F4D10
eSoundBankSlot CAEVehicleAudioEntity::RequestBankSlot(eSoundBank bankId) {
    const auto SlotToBankSlot = [&](auto& slot) {
        return (eSoundBankSlot)(SND_BANK_SLOT_DUMMY_FIRST + std::distance(std::begin(s_DummyEngineSlots), slot));
    };

    // Check if there's a slot with this bank already
    const auto usedSlot = rng::find_if(s_DummyEngineSlots, [&](auto&& s) {
        return s.BankID == bankId;
    });
    if (usedSlot != std::end(s_DummyEngineSlots)) {
        usedSlot->RefCnt++;
        return SlotToBankSlot(usedSlot);
    }

    // If not, find a free slot (if any) and initialize it
    // (NB: I left out the logic for `s_NextDummyEngineSlot`, it's really not necessary...)
    const auto slot = rng::find_if(s_DummyEngineSlots, [&](auto&& s) {
        return s.RefCnt < 1;
    });
    if (slot == std::end(s_DummyEngineSlots)) {
        return SND_BANK_SLOT_NONE; // No free slots
    }

    // Initialize the slot
    AESoundManager.CancelSoundsInBankSlot(SlotToBankSlot(slot), true);
    AEAudioHardware.LoadSoundBank(bankId, SlotToBankSlot(slot));
    slot->BankID = bankId;
    slot->RefCnt = 1;

    // Return the bank slot id
    return SlotToBankSlot(slot);
}

// 0x4F4DF0
void CAEVehicleAudioEntity::StoppedUsingBankSlot(eSoundBankSlot bankSlot) {
    if (auto* const s = GetDummyEngineSlot(bankSlot)) {
        s->RefCnt = std::max(s->RefCnt - 1, 0);
    }
}

// notsa
auto CAEVehicleAudioEntity::GetDummyEngineSlot(eSoundBankSlot bankSlot) -> tDummyEngineSlot* {
    const auto s = bankSlot - SND_BANK_SLOT_DUMMY_FIRST;
    return s >= 0 && s < (int32)(s_DummyEngineSlots.size())
        ? &s_DummyEngineSlots[s]
        : nullptr;
}

// 0x4F4E30
bool CAEVehicleAudioEntity::DoesBankSlotContainThisBank(eSoundBankSlot bankSlot, eSoundBank bankId) {
    auto* const s = GetDummyEngineSlot(bankSlot);
    return s && s->BankID == bankId;
}
#pragma endregion

#pragma region Helpers
// 0x4F5C40
bool CAEVehicleAudioEntity::CopHeli() const noexcept {
    const auto modelIndex = m_Entity->GetModelIndex();
    return modelIndex == MODEL_MAVERICK || modelIndex == MODEL_VCNMAV;
}

// 0x4F5EB0
void CAEVehicleAudioEntity::UpdateGasPedalAudio(CVehicle* vehicle, int32 vehicleType) {
    auto* const cfg = &s_Config.GasPedal;

    const auto current = std::fabs(vehicle->m_GasPedal);
    float& value = vehicleType == VEHICLE_TYPE_BIKE
        ? vehicle->AsBike()->m_GasPedalAudioRevs
        : vehicle->AsAutomobile()->m_GasPedalAudioRevs;
    value = notsa::step_to(value, current, cfg->StepUp, cfg->StepDown, notsa::bugfixes::GenericFrameRate);
}

// 0x4F5080
void CAEVehicleAudioEntity::GetAccelAndBrake(tVehicleParams& vp) const noexcept {
    if (CReplay::Mode == MODE_PLAYBACK) {
        vp.ThisAccel = std::clamp(vp.Vehicle->m_GasPedal, 0.0f, 1.0f);
        vp.ThisBrake = std::clamp(vp.Vehicle->m_BrakePedal, 0.0f, 1.0f);
    } else if (
           s_pPlayerDriver
        && !m_IsPlayerDriverAboutToExit
        && s_pPlayerDriver->m_nPedState != PEDSTATE_ARRESTED
        && s_pPlayerDriver->IsAlive()
    ) {
        CPad* pad    = s_pPlayerDriver->AsPlayer()->GetPadFromPlayer();
        vp.ThisAccel = pad->GetAccelerate() / 255.f;
        vp.ThisBrake = pad->GetBrake() / 255.f;
    } else {
        vp.ThisAccel = 0.f;
        vp.ThisBrake = 0.f;
    }
}


// 0x4F5C10
tVehicleAudioSettings CAEVehicleAudioEntity::GetVehicleAudioSettings(int16 vehId) {
    return ::GetVehicleAudioSettings((eModelID)(+MODEL_VEHICLE_FIRST + vehId));
}

// 0x4F4F00
eAEVehicleAudioType CAEVehicleAudioEntity::GetVehicleTypeForAudio() const noexcept {
    if (!m_IsInitialized || !m_Entity) {
        return eAEVehicleAudioType::GENERIC;
    }
    switch (m_AuSettings.VehicleAudioTypeForName) {
    case AE_VAT_OFFROAD:
    case AE_VAT_TWO_DOOR:
    case AE_VAT_SPORTS_CAR:
    case AE_VAT_STATION_WAGON:
    case AE_VAT_SEDAN:
    case AE_VAT_STRETCH:
    case AE_VAT_LOWRIDER:
    case AE_VAT_TAXI:
    case AE_VAT_CONVERTIBLE:
    case AE_VAT_HEARSE:
    case AE_VAT_COUPE:
    case AE_VAT_POLICE_CAR:
        return eAEVehicleAudioType::CAR;
    case AE_VAT_MOPED:
    case AE_VAT_BIKE:
    case AE_VAT_QUADBIKE:
        return eAEVehicleAudioType::BIKE;
    default:
        return eAEVehicleAudioType::GENERIC;
    }
}

// 0x4F6150
float CAEVehicleAudioEntity::GetFlyingMetalVolume(CPhysical* physical) const noexcept {
    const auto volume = std::min(physical->m_vecTurnSpeed.SquaredMagnitude(), sq(0.75f)) / sq(0.75f);
    return volume >= 1.0e-10f
        ? CAEAudioUtility::AudioLog10(volume) * 10.f
        : -100.f;
}
#pragma endregion

#pragma region Base Sound Functions
// 0x4FB6C0
void CAEVehicleAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (!sound) {
        return;
    }

    if (sound->m_Event == AE_BONNET_FLUBBER_FLUBBER) {
        if (sound->m_PhysicalEntity) {
            sound->m_Volume = GetDefaultVolume(AE_BONNET_FLUBBER_FLUBBER) + GetFlyingMetalVolume(sound->m_PhysicalEntity->AsPhysical());
        }
        return;
    }

    if (sound->m_Event == AE_BOAT_HIT_WAVE) {
        if (curPlayPos == -1) {
            return;
        }

        sound->SetPosition(m_Entity->GetPosition());
        if (CWeather::UnderWaterness >= 0.5F) {
            sound->StopSound();
        }
    }

    if (curPlayPos == -1) {
        if (sound == m_SurfaceSound) {
            m_SurfaceSound = nullptr;
        } else if (sound == m_RoadNoiseSound) {
            m_RoadNoiseSound = nullptr;
        } else if (sound == m_FlatTireSound) {
            m_FlatTireSound = nullptr;
        } else if (sound == m_ReverseGearSound) {
            m_ReverseGearSound = nullptr;
        } else if (sound == m_HornSound) {
            m_HornSound = nullptr;
        } else if (sound == m_SirenSound) {
            m_SirenSound = nullptr;
        } else if (sound == m_FastSirenSound) {
            m_FastSirenSound = nullptr;
        } else if (const auto it = rng::find(m_EngineSounds, sound, &tEngineSound::Sound); it != m_EngineSounds.end()) {
            it->Sound = nullptr;
        }
    } else if (m_AuSettings.IsAircraftEngineSound() && GetEngineSound(AE_SOUND_AIRCRAFT_NEAR) == sound) { // 0x4FB7F1
        sound->SetPosition(GetAircraftNearPosition());
    } else {
        sound->SetPosition(m_Entity->GetPosition());
    }

    if (m_AuSettings.IsCarEngineSound() && sound == m_EngineSounds[AE_SOUND_PLAYER_AC].Sound) {
        m_ACPlayPositionLastFrame = std::exchange(m_ACPlayPositionThisFrame, curPlayPos);
    }
}

// 0x4F6420
void CAEVehicleAudioEntity::AddAudioEvent(eAudioEvents event, float p1) {
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_BULLET_HITS, SND_BANK_SLOT_BULLET_HITS)) {
        return;
    }

    const auto GetPedEventVolume = [this](eAudioEvents ae) {
        float vol = GetDefaultVolume(ae);
        switch (m_AuSettings.VehicleAudioType) {
        case AE_BIKE: vol -= 3.f; break;
        case AE_BMX:  vol -= 6.f; break;
        }
        return vol;
    };

    switch (event) {
    case AE_CAR_BONNET_OPEN:
    case AE_CAR_BOOT_OPEN:
    case AE_CAR_FRONT_LEFT_DOOR_OPEN:
    case AE_CAR_FRONT_RIGHT_DOOR_OPEN:
    case AE_CAR_REAR_LEFT_DOOR_OPEN:
    case AE_CAR_REAR_RIGHT_DOOR_OPEN: { // 0x4F64B3
        const auto PlaySound = [&](eSoundID sfx) {
            AESoundManager.PlaySound({
                .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
                .SoundID           = sfx,
                .AudioEntity       = this,
                .Pos               = GetVehicle()->GetPosition(),
                .Volume            = GetDefaultVolume(AE_CAR_BONNET_OPEN),
                .FrequencyVariance = 0.02f,
            });
        };
        using enum eAEVehicleDoorType;
        switch (m_AuSettings.DoorType) {
        case LIGHT: PlaySound(SND_GENRL_VEHICLE_GEN_OPLIGHT); break;
        case OLD:   PlaySound(SND_GENRL_VEHICLE_GEN_OPOLD);   break;
        case TRUCK: PlaySound(SND_GENRL_VEHICLE_GEN_OTRUCK);  break;
        case VAN:   PlaySound(SND_GENRL_VEHICLE_GEN_OPVAN);   break;
        case UNSET:
        case NEW:   PlaySound(SND_GENRL_VEHICLE_GEN_OPNEW);   break;
        }
        break;
    }
    case AE_CAR_BONNET_CLOSE:
    case AE_CAR_BOOT_CLOSE:
    case AE_CAR_FRONT_LEFT_DOOR_CLOSE:
    case AE_CAR_FRONT_RIGHT_DOOR_CLOSE:
    case AE_CAR_REAR_LEFT_DOOR_CLOSE:
    case AE_CAR_REAR_RIGHT_DOOR_CLOSE: { // 0x4F654D
        const auto PlaySound = [&](eSoundID sfx) {
            AESoundManager.PlaySound({
                .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
                .SoundID           = sfx,
                .AudioEntity       = this,
                .Pos               = GetVehicle()->GetPosition(),
                .Volume            = GetDefaultVolume(AE_CAR_BONNET_CLOSE),
                .FrequencyVariance = 0.02f,
            });
        };
        using enum eAEVehicleDoorType;
        switch (m_AuSettings.DoorType) {
        case LIGHT: PlaySound(SND_GENRL_VEHICLE_GEN_CLLIGHT); break;
        case OLD:   PlaySound(SND_GENRL_VEHICLE_GEN_CLOLD);   break;
        case TRUCK: PlaySound(SND_GENRL_VEHICLE_GEN_CTRUCK);  break;
        case VAN:   PlaySound(SND_GENRL_VEHICLE_GEN_CLVAN);   break;
        case UNSET: 
        case NEW:   PlaySound(SND_GENRL_VEHICLE_GEN_CLNEW);   break;
        }
        break;
    }
    case AE_TYRE_BURST: { // 0x4F65BD
        AESoundManager.PlaySound({
            .BankSlotID          = SND_BANK_SLOT_BULLET_HITS,
            .SoundID           = SND_GENRL_BULLET_HITS_BLOWOUT,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_TYRE_BURST),
        });
        break;
    }
    case AE_WINDSCREEN_SHATTER: { // 0x4F6608
        AESoundManager.PlaySound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = SND_GENRL_COLLISIONS_WINDSCREEN,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_WINDSCREEN_SHATTER),
        });
        break;
    }
    case AE_LIGHT_SMASH: { // 0x4F64AC
        AESoundManager.PlaySound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = SND_GENRL_COLLISIONS_HEADLIGHT,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_LIGHT_SMASH),
        });
        break;
    }
    case AE_BOAT_HIT_WAVE: { // 0x4F66D8
        const auto* const cfg = &s_Config.Boat.WaveHit;

        if (CTimer::GetTimeInMS() < m_TimeSplashLastTriggered + cfg->WaitTime) {
            break;
        }
        if (p1 < cfg->VolMin) {
            break;
        }
        const auto vol = std::clamp(p1 / cfg->VolMax, 0.f, 1.f);
        if (vol <= 0.00001f) {
            break;
        }
        if (CWeather::UnderWaterness >= 0.5f) {
            break;
        }
        AESoundManager.PlaySound({
            .BankSlotID    = SND_BANK_SLOT_COLLISIONS,
            .SoundID     = SND_GENRL_COLLISIONS_SPLASH_END,
            .AudioEntity = this,
            .Pos         = GetVehicle()->GetPosition(),
            .Volume      = vol,
        });
        break;
    }
    case AE_TRAIN_CLACK: { // 0x4F680B
        if (m_DummySlot != -1) {
            if (!AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
                break;
            }
        }
        AESoundManager.PlaySound({
            .BankSlotID          = m_DummySlot,
            .SoundID           = CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_TRAIN_D_TRACK_JOIN_3, SND_GENRL_TRAIN_D_TRACK_JOIN_5),
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_TRAIN_CLACK),
            .RollOffFactor     = 2.f,
            .FrequencyVariance = 0.1f,
        });
        break;
    }
    case AE_NITRO_IGNITION: { // 0x4F6D8B
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN)) {
            break;
        }
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_NITRO_IGNITION, this)) {
            break;
        }
        AESoundManager.PlaySound({
            .BankSlotID           = SND_BANK_SLOT_WEAPON_GEN,
            .SoundID            = SND_GENRL_WEAPONS_TRIGGER,
            .AudioEntity        = this,
            .Pos                = GetVehicle()->GetPosition(),
            .Volume             = GetDefaultVolume(AE_NITRO_IGNITION),
            .RollOffFactor      = 2.f,
            .Speed              = 0.89f,
            .FrequencyVariance  = 0.03f,
            .RegisterWithEntity = GetVehicle(),
            .EventID            = AE_NITRO_IGNITION
        });
        break;
    }
    case AE_START_ENGINE: { // AE_START_ENGINE
        AESoundManager.PlaySound({
            .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
            .SoundID           = SND_GENRL_VEHICLE_GEN_GENERIC_START,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_START_ENGINE),
            .RollOffFactor     = 2.f,
            .FrequencyVariance = 0.05f,
            .EventID           = AE_START_ENGINE
        });
        break;
    }
    case AE_SUSPENSION_HYDRAULIC: { // 0x4F70CE
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_SUSPENSION_HYDRAULIC, this)) {
            break;
        }
        AESoundManager.PlaySound({
            .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
            .SoundID           = SND_GENRL_VEHICLE_GEN_HYDRAULIC_SUSPENSION,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_SUSPENSION_HYDRAULIC) - 6.f,
            .RollOffFactor     = 2.f,
            .FrequencyVariance = 0.05f,
            .EventID           = AE_SUSPENSION_HYDRAULIC
        });
        break;
    }
    case AE_SUSPENSION_BOUNCE: { // 0x4F6EE7
        AESoundManager.PlaySound({
            .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
            .SoundID           = SND_GENRL_VEHICLE_GEN_BOUNCE_SUSPENSION,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_SUSPENSION_BOUNCE) - 3.f,
            .RollOffFactor     = 2.f,
            .FrequencyVariance = 0.05f,
            .EventID           = AE_SUSPENSION_BOUNCE
        });
        break;
    }
    case AE_SUSPENSION_TRIGGER: { // 0x4F6F88
        AESoundManager.PlaySound({
            .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
            .SoundID           = SND_GENRL_VEHICLE_GEN_TRIGGER_SUSPENSION,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_SUSPENSION_TRIGGER) + 0.f,
            .RollOffFactor     = 2.f,
            .Flags             = SOUND_START_PERCENTAGE,
            .FrequencyVariance = 0.05f,
            .PlayTime          = 0,
            .EventID           = AE_SUSPENSION_TRIGGER,
        });
        break;
    }
    case AE_SUSPENSION_ON: { // 0x4F7035
        AESoundManager.PlaySound({
            .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
            .SoundID           = SND_GENRL_VEHICLE_GEN_TRIGGER_SUSPENSION,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_SUSPENSION_ON) + 0.f,
            .RollOffFactor     = 2.f,
            .Flags             = SOUND_START_PERCENTAGE,
            .FrequencyVariance = 0.05f,
            .PlayTime          = 0,
            .EventID           = AE_SUSPENSION_ON,
        });
        break;
    }
    case AE_SUSPENSION_OFF: { // 0x4F7183
        AESoundManager.PlaySound({
            .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
            .SoundID           = SND_GENRL_VEHICLE_GEN_HYDRAULIC_SUSPENSION,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_SUSPENSION_OFF) + 0.f,
            .RollOffFactor     = 2.f,
            .Flags             = SOUND_START_PERCENTAGE,
            .FrequencyVariance = 0.05f,
            .PlayTime          = 0,
            .EventID           = AE_SUSPENSION_OFF,
        });
        break;
    }
    case AE_TRAILER_ATTACH: { // 0x4F72CF
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_TRAILER_ATTACH, this)) {
            break;
        }
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_TRAILER_DETACH, this)) {
            break;
        }
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN)) {
            break;
        }
        AESoundManager.PlaySound({
            .BankSlotID           = SND_BANK_SLOT_WEAPON_GEN,
            .SoundID            = SND_GENRL_WEAPONS_SAWNOFF_B,
            .AudioEntity        = this,
            .Pos                = GetVehicle()->GetPosition(),
            .Volume             = GetDefaultVolume(AE_TRAILER_ATTACH),
            .RollOffFactor      = 2.f,
            .Speed              = 0.56f,
            .FrequencyVariance  = 0.05f,
            .RegisterWithEntity = GetVehicle(),
            .EventID            = AE_TRAILER_ATTACH,
        });
        break;
    }
    case AE_TRAILER_DETACH: { // 0x4F64AC
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_TRAILER_ATTACH, this)) {
            break;
        }
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_TRAILER_DETACH, this)) {
            break;
        }
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN)) {
            break;
        }
        AESoundManager.PlaySound({
            .BankSlotID           = SND_BANK_SLOT_WEAPON_GEN,
            .SoundID            = SND_GENRL_WEAPONS_SAWNOFF_B,
            .AudioEntity        = this,
            .Pos                = GetVehicle()->GetPosition(),
            .Volume             = GetDefaultVolume(AE_TRAILER_DETACH),
            .RollOffFactor      = 2.f,
            .Speed              = 0.5f,
            .FrequencyVariance  = 0.05f,
            .RegisterWithEntity = GetVehicle(),
            .EventID            = AE_TRAILER_DETACH,
        });
        break;
    }
    case AE_BODY_HARVEST: { // 0x4F7224
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_BODY_HARVEST, this)) {
            break;
        }
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_HARVESTER_P, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
            break;
        }
        AESoundManager.PlaySound({
            .BankSlotID           = SND_BANK_SLOT_PLAYER_ENGINE_P,
            .SoundID            = 4,
            .AudioEntity        = this,
            .Pos                = GetVehicle()->GetPosition(),
            .Volume             = GetDefaultVolume(AE_BODY_HARVEST),
            .RollOffFactor      = 2.f,
            .FrequencyVariance  = 0.05f,
            .RegisterWithEntity = GetVehicle(),
            .EventID            = AE_BODY_HARVEST,
        });
        break;
    }
    case AE_PED_CRUNCH: { // 0x4F68E4
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
            break;
        }
        const auto PlaySound = [&](float frq, int16 playTime) {
            AESoundManager.PlaySound({
                .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
                .SoundID           = SND_GENRL_COLLISIONS_COLCARPED,
                .AudioEntity       = this,
                .Pos               = GetVehicle()->GetPosition(),
                .Volume            = GetPedEventVolume(AE_PED_CRUNCH),
                .RollOffFactor     = 1.5f,
                .Speed             = frq,
                .Flags             = SOUND_START_PERCENTAGE,
                .FrequencyVariance = 0.06f,
                .PlayTime          = playTime,
                .EventID           = AE_PED_CRUNCH
            });
        };
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_PED_CRUNCH, this)) {
            PlaySound(0.8f, 40);
        } else {
            PlaySound(0.94f, 0);
        }
        break;
    }
    case AE_PED_DRIVE_OVER: { // 0x4F6B3A
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
            break;
        }
        AESoundManager.PlaySound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_COLLISIONS_GORE_SPLAT1, SND_GENRL_COLLISIONS_GORE_SPLAT3),
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetPedEventVolume(AE_PED_DRIVE_OVER),
            .RollOffFactor     = 1.5f,
            .FrequencyVariance = 0.06f,
            .EventID           = notsa::IsFixBugs() ? AE_PED_DRIVE_OVER : AE_PED_KNOCK_DOWN // Makes no sense for this to be `AE_PED_KNOCK_DOWN`
        });
        break;
    }
    case AE_PED_KNOCK_DOWN: { // 0x4F69CB
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
            break;
        }
        AESoundManager.PlaySound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = SND_GENRL_COLLISIONS_COLSOLIDWOOD,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetPedEventVolume(AE_PED_KNOCK_DOWN),
            .RollOffFactor     = 1.5f,
            .FrequencyVariance = 0.06f,
            .EventID           = AE_PED_KNOCK_DOWN
        });
        AESoundManager.PlaySound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_COLLISIONS_COLCAR01, SND_GENRL_COLLISIONS_COLCAR12),
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetPedEventVolume(AE_PED_KNOCK_DOWN),
            .RollOffFactor     = 1.5f,
            .Flags             = SOUND_START_PERCENTAGE,
            .FrequencyVariance = 0.06f,
            .PlayTime          = 55,
            .EventID           = AE_PED_KNOCK_DOWN
        });
        break;
    }
    case AE_PED_BOUNCE: { // 0x4F6C29
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
            break;
        }
        AESoundManager.PlaySound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = SND_GENRL_COLLISIONS_COLGRASS,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetPedEventVolume(AE_PED_BOUNCE),
            .RollOffFactor     = 1.5f,
            .FrequencyVariance = 0.06f,
            .EventID           = AE_PED_BOUNCE
        });
        AESoundManager.PlaySound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_COLLISIONS_COLCAR01, SND_GENRL_COLLISIONS_COLCAR12),
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetPedEventVolume(AE_PED_BOUNCE),
            .RollOffFactor     = 1.5f,
            .Flags             = SOUND_START_PERCENTAGE,
            .FrequencyVariance = 0.06f,
            .PlayTime          = 25,
            .EventID           = AE_PED_BOUNCE
        });
        break;
    }
    }
}

// 0x4F7580
void CAEVehicleAudioEntity::AddAudioEvent(eAudioEvents event, CPhysical* physical) {
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
        return;
    }
    if (event != AE_BONNET_FLUBBER_FLUBBER) {
        return;
    }
    if (!physical) {
        return;
    }
    AESoundManager.PlaySound({
        .BankSlotID         = SND_BANK_SLOT_VEHICLE_GEN,
        .SoundID            = SND_GENRL_VEHICLE_GEN_HOOD_FLY,
        .AudioEntity        = this,
        .Pos                = physical->GetPosition(),
        .Volume             = GetDefaultVolume(AE_BONNET_FLUBBER_FLUBBER) + GetFlyingMetalVolume(physical),
        .RollOffFactor      = 0.85f,
        .Flags              = SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
        .RegisterWithEntity = physical,
        .EventID            = AE_BONNET_FLUBBER_FLUBBER
    });
}

// 0x502280
void CAEVehicleAudioEntity::Service() {
    if (!m_IsInitialized) {
        return;
    }

    const auto veh = m_Entity->AsVehicle();

    if (m_IsPlayerDriver && !m_IsPlayerDriverAboutToExit) {
        if (auto* const d = s_pPlayerDriver) {
            if (d->GetPedState() == PEDSTATE_ARRESTED || !d->IsAlive()) {
                PlayerAboutToExitVehicleAsDriver();
            }
        }
    }

    const auto isWrecked = m_Entity->GetStatus() == STATUS_WRECKED;
    if (!std::exchange(m_IsWreckedVehicle, isWrecked) && isWrecked) {
        JustWreckedVehicle();
    }

    const auto isPlayerVehicle    = CAEAudioUtility::FindVehicleOfPlayer() == veh; // This is the player's vehicle (They might not be in it though)
    const auto isPlayerDriver     = veh->m_pDriver && veh->m_pDriver->IsPlayer();  // The player is the driver of this vehicle
    const auto isPlayerPassenger  = isPlayerVehicle && !isPlayerDriver;            // The player is (just) a passenger of this vehicle
    const auto isPlayerInVehicle  = isPlayerDriver || isPlayerPassenger;           // The player is in the vehicle
    const auto wasPlayerInVehicle = m_IsPlayerPassenger || m_IsPlayerDriver;

    if (isPlayerInVehicle && !wasPlayerInVehicle) {
        TurnOnRadioForVehicle();
    } else if (!isPlayerInVehicle && wasPlayerInVehicle) {
        TurnOffRadioForVehicle();
    }

    if (isPlayerDriver && !m_IsPlayerDriver) {
        JustGotInVehicleAsDriver();
    } else if (!isPlayerDriver && m_IsPlayerDriver) {
        JustGotOutOfVehicleAsDriver();
    }

    m_IsPlayerDriver    = isPlayerDriver;
    m_IsPlayerPassenger = isPlayerPassenger;

    ProcessVehicle(veh);
}

void CAEVehicleAudioEntity::StaticService() {
    // NOP
}

#pragma endregion

#pragma region Generic Process Functions
// 0x5002C0
void CAEVehicleAudioEntity::ProcessVehicleSirenAlarmHorn(tVehicleParams& vp) {
    // Figure out sirens state
    bool isFastSirenOn = false,
        isSirenOn     = false;
    GetSirenState(isSirenOn, isFastSirenOn, vp);

    // If no siren, possibly use horn
    bool isHornOn = false;
    if (!isSirenOn) {
        if (!vp.Vehicle->m_nAlarmState || vp.Vehicle->m_nAlarmState == -1 || vp.Vehicle->GetStatus() == STATUS_WRECKED) {
            GetHornState(&isHornOn, vp);
        } else {
            const auto time = CTimer::GetTimeInMS();
            if (time > vp.Vehicle->m_HornCounter) {
                vp.Vehicle->m_HornCounter = time + 750;
            }
            isHornOn = vp.Vehicle->m_HornCounter < time + 750 / 2;
        }
    }

    // Play sounds now
    PlayHornOrSiren(isHornOn, isSirenOn, isFastSirenOn, vp);
    m_IsSirenOn     = isSirenOn;
    m_IsFastSirenOn = isFastSirenOn;
    m_IsHornOn      = isHornOn;
}
#pragma endregion

#pragma region Player Car (AE_CAR)
// 0x4F4F70
bool CAEVehicleAudioEntity::IsAccInhibited(tVehicleParams& vp) const noexcept {
    return !AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)
        || vp.ThisBrake > 0.f
        || vp.IsHandbrakeOn
        || IsAccInhibitedBackwards(vp);
}

// inlined?
// 0x4F4FC0
bool CAEVehicleAudioEntity::IsAccInhibitedBackwards(tVehicleParams& vp) const noexcept {
    return vp.WheelSpin > 5.0f
        || !vp.NumDriveWheelsOnGround;
}

// 0x4F4FF0
bool CAEVehicleAudioEntity::IsAccInhibitedForLowSpeed(tVehicleParams& vp) const noexcept {
    return !m_IsSingleGear && vp.Speed < s_Config.PlayerEngine.AC.InhibitForLowSpeedLimit;
}

// 0x4F5020
bool CAEVehicleAudioEntity::IsAccInhibitedForTime() const noexcept {
    return m_TimeBeforeAllowAccelerate < CTimer::GetTimeInMS();
}

// 0x4F5030
void CAEVehicleAudioEntity::InhibitAccForTime(uint32 time) {
    m_TimeBeforeAllowAccelerate = time + CTimer::GetTimeInMS();
}

// 0x4F5060
void CAEVehicleAudioEntity::InhibitCrzForTime(uint32 time) {
    m_TimeBeforeAllowCruise = time + CTimer::GetTimeInMS();
}

// 0x4F5050
bool CAEVehicleAudioEntity::IsCrzInhibitedForTime() const noexcept {
    return m_TimeBeforeAllowCruise < CTimer::GetTimeInMS();
}

#pragma endregion

#pragma region Dummies
// 0x4FCA10
void CAEVehicleAudioEntity::ProcessDummyStateTransition(eAEState newState, float ratio, tVehicleParams& vp) {
    m_TimeLastServiced = CTimer::GetTimeInMS();

    switch (const auto oldState = std::exchange(m_State, newState)) {
    case eAEState::DUMMY_ID: {
        switch (newState) {
        case eAEState::DUMMY_ID: { // 0x4FCA37
            const auto* const cfg = &s_Config.DummyCar.Transitions.FromIdleToIdle;

            // 0x4FCA49
            m_FadeIn = notsa::step_up_to(m_FadeIn, 1.f, cfg->FadeInStep, notsa::bugfixes::GenericFrameRate);
            UpdateVehicleEngineSound(
                AE_SOUND_CAR_ID,
                GetFrequencyForDummyIdle(ratio, m_FadeIn),
                GetVolumeForDummyIdle(ratio, m_FadeIn)
            );

            // 0x4FCAC0
            m_FadeOut = notsa::step_up_to(m_FadeOut, 1.f, cfg->FadeOutStep, notsa::bugfixes::GenericFrameRate);
            if (m_FadeOut < 0.99f) {
                UpdateVehicleEngineSound(
                    AE_SOUND_CAR_REV,
                    GetFrequencyForDummyRev(ratio, m_FadeOut),
                    GetVolumeForDummyRev(ratio, m_FadeOut)
                );
            } else {
                StopGenericEngineSound(AE_SOUND_CAR_REV);
            }

            break;
        }
        case eAEState::CAR_OFF: { // 0x4FCBDF
            StopGenericEngineSound(AE_SOUND_CAR_ID);
            StopGenericEngineSound(AE_SOUND_CAR_REV);
            break;
        }
        case eAEState::DUMMY_CRZ: { // 0x4FCB56
            m_FadeOut = 0.f;
            UpdateVehicleEngineSound(
                AE_SOUND_CAR_ID,
                GetFrequencyForDummyIdle(ratio, m_FadeOut),
                GetVolumeForDummyIdle(ratio, 0.f)
            );

            m_FadeIn = 0.f;
            UpdateVehicleEngineSound(
                AE_SOUND_CAR_REV,
                GetFrequencyForDummyRev(ratio, m_FadeIn),
                GetVolumeForDummyRev(ratio, 0.f)
            );
            break;
        }
        default:
            NOTSA_UNREACHABLE("Invalid new state ({})", newState);
        }
        break;
    }
    case eAEState::DUMMY_CRZ: { // 0x4FCC46
        switch (newState) {
        case eAEState::DUMMY_ID: { // 0x4FCC56
            m_FadeOut = 0.f;
            UpdateVehicleEngineSound(
                AE_SOUND_CAR_REV,
                GetFrequencyForDummyRev(ratio, m_FadeIn),
                GetVolumeForDummyRev(ratio, 0.f)
            );

            m_FadeIn = 0.f;
            UpdateVehicleEngineSound(
                AE_SOUND_CAR_ID,
                GetFrequencyForDummyIdle(ratio, m_FadeOut),
                GetVolumeForDummyIdle(ratio, 0.f)
            );
            break;
        }
        case eAEState::DUMMY_CRZ: { // 0x4FCCE9
            const auto* const cfg = &s_Config.DummyCar.Transitions.FromCrzToCrz;

            // 0x4FCCE9
            m_FadeIn = notsa::step_up_to(m_FadeIn, 1.f, cfg->FadeInStep, notsa::bugfixes::GenericFrameRate);
            UpdateVehicleEngineSound(
                AE_SOUND_CAR_REV,
                GetFrequencyForDummyRev(ratio, m_FadeIn),
                GetVolumeForDummyRev(ratio, m_FadeIn)
            );
             
            // 0x4FCD7C
            m_FadeOut = notsa::step_up_to(m_FadeOut, 1.f, cfg->FadeOutStep, notsa::bugfixes::GenericFrameRate);
            if (m_FadeOut < 0.99f) {
                if (0.65f < m_FadeOut && m_FadeOut < 0.75f) { // 0x4FCD94
                    vp.Vehicle->vehicleFlags.bAudioChangingGear = true;
                }
                UpdateVehicleEngineSound(
                    AE_SOUND_CAR_ID,
                    GetFrequencyForDummyIdle(ratio, m_FadeOut),
                    GetVolumeForDummyIdle(ratio, m_FadeOut)
                );
            } else {
                StopGenericEngineSound(AE_SOUND_CAR_ID);
            }

            break;
        }
        case eAEState::CAR_OFF: { // 0x4FCDFC
            CancelVehicleEngineSound(AE_SOUND_CAR_REV);
            CancelVehicleEngineSound(AE_SOUND_CAR_ID);
            break;
        }
        default:
            NOTSA_UNREACHABLE("Invalid new state ({})", newState);
        }
        break;
    }
    case eAEState::CAR_OFF:
    case eAEState::NUM_STATES: { // 0x4FCE5A
        switch (newState) {
        case eAEState::DUMMY_ID: { // 0x4FCE6D
            if (oldState == eAEState::CAR_OFF) {
                AddAudioEvent(AE_START_ENGINE, 0.f);
            }
            m_FadeOut = 1.f;
            m_FadeIn = 1.f;
            StartVehicleEngineSound(
                AE_SOUND_CAR_ID,
                GetFrequencyForDummyIdle(ratio, 1.f),
                GetVolumeForDummyIdle(ratio, 1.f)
            );
            break;
        }
        case eAEState::DUMMY_CRZ: { // 0x4FCED0
            if (oldState == eAEState::CAR_OFF) {
                AddAudioEvent(AE_START_ENGINE, 0.f);
            }
            m_FadeOut = 1.f;
            m_FadeIn = 1.f;
            StartVehicleEngineSound(
                AE_SOUND_CAR_REV,
                GetFrequencyForDummyRev(ratio, 1.f),
                GetVolumeForDummyRev(ratio, 1.f)
            );
            break;
        }
        case eAEState::CAR_OFF: { // 0x4FCEC8
            break;
        }
        default:
            NOTSA_UNREACHABLE("Invalid new state ({})", newState);
        }
        break;
    }
    default:
        NOTSA_UNREACHABLE("Invalid old state ({})", oldState);
    }
}

// @notsa
constexpr float CAEVehicleAudioEntity::GetDummyIdleRatioProgress(float ratio) {
    return std::clamp(ratio / s_Config.DummyEngine.ID.Ratio, 0.0f, 1.0f);
}

// 0x4F51F0
float CAEVehicleAudioEntity::GetVolumeForDummyIdle(float ratio, float fadeRatio) const noexcept {
    if (GetVehicle()->GetModelId() == MODEL_CADDY) {
        return s_Config.DummyEngine.ID.VolumeBase - 30.0f;
    }

    float volume = lerp(s_Config.DummyEngine.ID.VolumeBase, s_Config.DummyEngine.ID.VolumeMax, GetDummyIdleRatioProgress(ratio));
    if (m_State == eAEState::DUMMY_CRZ) {
        constexpr float points[5][2] = { // 0x8CC0C4
            { 0.0000f, 1.000f},
            { 0.3000f, 1.000f},
            { 0.5000f, 1.000f},
            { 0.7000f, 0.707f},
            { 1.0001f, 0.000f}
        };
        volume += CAEAudioUtility::AudioLog10(CAEAudioUtility::GetPiecewiseLinearT(fadeRatio, points)) * 20.0f;
    } else if (fadeRatio <= 0.99f) {
        volume += CAEAudioUtility::AudioLog10(fadeRatio) * 10.0f;
    }
    if (GetVehicle()->vehicleFlags.bIsDrowning) {
        volume -= s_Config.DummyEngine.VolumeUnderwaterOffset;
    }
    if (GetVehicle()->m_pVehicleBeingTowed) {
        volume += s_Config.DummyEngine.VolumeTrailerOffset;
    }
    return volume + m_AuSettings.EngineVolumeOffset;
}

// 0x4F5310
float CAEVehicleAudioEntity::GetFrequencyForDummyIdle(float ratio, float fadeRatio) const noexcept {
    if (GetVehicle()->GetModelIndex() == MODEL_CADDY) {
        return 1.0f;
    }

    float freq = lerp(s_Config.DummyEngine.ID.FreqBase, s_Config.DummyEngine.ID.FreqMax, GetDummyIdleRatioProgress(ratio));
    if (m_State == eAEState::DUMMY_CRZ) {
        constexpr float points[5][2] = { // 0x8CC0EC
            { 0.0000f, 1.00f },
            { 0.3000f, 1.00f },
            { 0.5000f, 0.85f },
            { 0.7000f, 0.85f },
            { 1.0001f, 0.85f },
        };
        freq *= CAEAudioUtility::GetPiecewiseLinearT(fadeRatio, points);
    }
    if (GetVehicle()->vehicleFlags.bIsDrowning) {
        freq *= s_Config.FreqUnderwaterFactor;
    }
    return freq;
}

// @notsa
constexpr float CAEVehicleAudioEntity::GetDummyRevRatioProgress(float ratio) {
    return std::clamp(invLerp(s_Config.DummyEngine.Rev.Ratio, 1.f, ratio), 0.f, 1.f);
}

// 0x4F53D0
float CAEVehicleAudioEntity::GetVolumeForDummyRev(float ratio, float fadeRatio) const {
    float volume = lerp(s_Config.DummyEngine.Rev.VolumeBase, s_Config.DummyEngine.Rev.VolumeMax, GetDummyRevRatioProgress(ratio));
    if (m_State != eAEState::DUMMY_CRZ) {
        volume += CAEAudioUtility::AudioLog10(1.0f - fadeRatio) * 10.0f;
    } else if (fadeRatio <= 0.99f) {
        constexpr float points[][2] = { // 0x8CC114
            { 0.0000f, 0.000f},
            { 0.3000f, 0.000f},
            { 0.5000f, 0.707f},
            { 0.7000f, 1.000f},
            { 1.0001f, 1.000f},
        };
        volume += CAEAudioUtility::AudioLog10(CAEAudioUtility::GetPiecewiseLinearT(fadeRatio, points)) * 20.0f;
    } else {
        volume += 0.0f;
    }
    if (GetVehicle()->vehicleFlags.bIsDrowning) {
        volume -= s_Config.DummyEngine.VolumeUnderwaterOffset;
    }
    if (GetVehicle()->m_pVehicleBeingTowed) {
        volume += s_Config.DummyEngine.VolumeTrailerOffset;
    }
    return volume + m_AuSettings.EngineVolumeOffset;
}

// 0x4F54F0
float CAEVehicleAudioEntity::GetFrequencyForDummyRev(float ratio, float fadeRatio) const {
    float freq = lerp(s_Config.DummyEngine.Rev.FreqBase, s_Config.DummyEngine.Rev.FreqMax, GetDummyRevRatioProgress(ratio));
    if (m_State == eAEState::DUMMY_CRZ && fadeRatio < 0.99f) {
        freq *= CGeneral::GetPiecewiseLinear({
            { 0.0000f, 1.0f },
            { 0.5000f, 1.0f },
            { 0.7000f, 1.2f },
            { 0.8500f, 1.0f },
            { 1.0001f, 1.0f },
        }, fadeRatio);
    }
    if (GetVehicle()->vehicleFlags.bIsDrowning) {
        freq *= s_Config.FreqUnderwaterFactor;
    }
    return freq;
}
#pragma endregion

#pragma region Engine Sounds Management

// 0x4F55C0
void CAEVehicleAudioEntity::CancelVehicleEngineSound(eVehicleEngineSoundType st) {
    if (st > AE_SOUND_ENGINE_MAX) {
        return;
    }

    auto&      sound       = m_EngineSounds[st].Sound;
    const auto soundLength = sound ? sound->GetSoundLength() : -1;

    GracefullyStopSound(sound);

    if (st == AE_SOUND_PLAYER_AC) {
        m_ACPlayPositionWhenStopped = m_ACPlayPositionThisFrame;
        m_TimeACStopped             = CTimer::GetTimeInMS();

        m_ACPlayPositionThisFrame   = -1;
        m_ACPlayPositionLastFrame   = -1;

        m_ACPlayPercentWhenStopped = soundLength > 0
            ? (int16)(std::clamp((float)(m_ACPlayPositionWhenStopped) / (float)(soundLength), 0.0f, 1.0f) * 100.f)
            : 0;
    }
}

// notsa
void CAEVehicleAudioEntity::CancelAllVehicleEngineSounds(std::optional<eVehicleEngineSoundType> except) {
    for (int32 i{}; i < (int32)(std::size(m_EngineSounds)); i++) {
        if (!except.has_value() || *except == i) {
            CancelVehicleEngineSound((eVehicleEngineSoundType)(i));
        }
    }
}

// 0x4F56D0
// NB: Our function returns whenever the sound was updated, original doesn't, (!) so this function IS NOT hooked!
bool CAEVehicleAudioEntity::UpdateVehicleEngineSound(eVehicleEngineSoundType st, float speed, float volume) {
    return UpdateVehicleEngineSoundRawVolume(st, speed, m_EventVolume + volume);
}

// notsa
bool CAEVehicleAudioEntity::UpdateVehicleEngineSoundRawVolume(eVehicleEngineSoundType st, float speed, float volume) {
    if (CAESound* const sound = m_EngineSounds[st].Sound) {
        sound->m_Speed  = speed;
        sound->m_Volume = volume;
        return true;
    }
    return false;
}

// 0x4F6320
void CAEVehicleAudioEntity::StopGenericEngineSound(eVehicleEngineSoundType st) {
    GracefullyStopSound(m_EngineSounds[st].Sound);
}

// 0x4F7A50
void CAEVehicleAudioEntity::RequestNewPlayerCarEngineSound(eVehicleEngineSoundType st, float speed, float volume) {
    volume += m_EventVolume;

    if (m_DummySlot == -1) {
        return;
    }

    switch (st) {
    case AE_SOUND_CAR_REV:
    case AE_SOUND_CAR_ID: { // 0x4F7AB8
        if (!AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
            return;
        }
        break;
    }
    case AE_SOUND_PLAYER_CRZ:
    case AE_SOUND_PLAYER_AC:
    case AE_SOUND_PLAYER_OFF: // 0x4F7ACB
        if (!AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
            return;
        }
        break;
    default:
        NOTSA_UNREACHABLE();
    }

    const auto DoPlayEngineSoundSound = [&](eSoundBankSlot slot, eSoundID sfx, float rollOffFactor, uint32 flags = 0, int16 playTime = 0) {
        m_EngineSounds[st].Sound = AESoundManager.PlaySound({
            .BankSlotID      = slot,
            .SoundID       = sfx,
            .AudioEntity   = this,
            .Pos           = m_Entity->GetPosition(),
            .Volume        = volume,
            .RollOffFactor = rollOffFactor,
            .Speed         = speed,
            .Flags         = SOUND_REQUEST_UPDATES | flags,
            .PlayTime      = playTime
        });
    };

    switch (st) {
    case AE_SOUND_CAR_REV: { // 0x4F7B28
        DoPlayEngineSoundSound(
            m_DummySlot,
            0,
            s_Config.RevRollOffFactor
        );
        break;
    }
    case AE_SOUND_CAR_ID: { // 0x4F7B28
        DoPlayEngineSoundSound(
            m_DummySlot,
            1,
            s_Config.IdRollOffFactor
        );
        break;
    }
    case AE_SOUND_PLAYER_CRZ: { // 0x4F7C1F
        DoPlayEngineSoundSound(
            SND_BANK_SLOT_PLAYER_ENGINE_P,
            1,
            s_Config.PlayerEngine.Crz.RollOffFactor
        );
        break;
    }
    case AE_SOUND_PLAYER_AC: { // 0x4F7C77
        const auto* const cfg = &s_Config.PlayerEngine.AC;

        const auto DoPlayerACEngineSound = [&](int16 playTime) {
            DoPlayEngineSoundSound(
                SND_BANK_SLOT_PLAYER_ENGINE_P,
                0,
                cfg->RollOffFactor,
                SOUND_START_PERCENTAGE,
                playTime
            );
        };
        m_FramesAgoACLooped = 0;

        // 0x4F7CFC - Slightly refactored the logic...
        const auto delta = CTimer::GetTimeInMS() - m_TimeACStopped;
        if (delta <= cfg->SoundLength) {
            // 0x4F7D96 - Using that delta we can calculate the new play position (in %)
            const auto pos = std::clamp(
                lerp((float)(m_ACPlayPercentWhenStopped), 0.f, (float)(delta) / (float)(cfg->SoundLength)),
                0.f, cfg->LoopMaxPlayTimePercentage
            );

            // 0x4F7E23 - This should force an acceleration loop the next time (See `JustFinishedAccelerationLoop`)
            if (pos > 10.f) {
                m_FramesAgoACLooped = cfg->LoopFrameCnt + 1;
            }

            // 0x4F7EC1 - Play the sound now
            DoPlayerACEngineSound((int16)(pos));
        } else { // Loop back to beginning (Must, as we're past the end of the sound)
            DoPlayerACEngineSound(0);
        }
        break;
    }
    case AE_SOUND_PLAYER_OFF: { // 0x4F7E39
        DoPlayEngineSoundSound(
            SND_BANK_SLOT_PLAYER_ENGINE_P,
            2,
            s_Config.PlayerEngine.Off.RollOffFactor
        );
        break;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x4F7F20
void CAEVehicleAudioEntity::StartVehicleEngineSound(eVehicleEngineSoundType st, float speed, float volume) {
    volume += m_EventVolume;

    if (auto* const s = GetEngineSound(st)) {
        s->SetVolume(volume);
        s->SetSpeed(speed);
        return;
    }

    const auto PlayEngineSound = [&](eSoundID sfx, float rollOffFactor) {
        m_EngineSounds[st].Sound = AESoundManager.PlaySound({
            .BankSlotID      = m_DummySlot,
            .SoundID       = sfx,
            .AudioEntity   = this,
            .Pos           = m_Entity->GetPosition(),
            .Volume        = volume,
            .RollOffFactor = rollOffFactor,
            .Speed         = speed,
            .Flags         = SOUND_REQUEST_UPDATES,
        });
    };
    const auto* const cfg = &s_Config;
    switch (st) {
    case AE_SOUND_CAR_ID:  PlayEngineSound(1, cfg->IdRollOffFactor);  break;
    case AE_SOUND_CAR_REV: PlayEngineSound(0, cfg->RevRollOffFactor); break;
    }
}

#pragma endregion

#pragma region Player Only
// 0x4FD0B0
void CAEVehicleAudioEntity::JustWreckedVehicle() {
    if (m_IsPlayerDriver) {
        TurnOffRadioForVehicle();
    }
    switch (m_AuSettings.VehicleAudioType) {
    case AE_CAR:
    case AE_BIKE:
    case AE_BMX:
    case AE_BOAT:
    case AE_AIRCRAFT_HELICOPTER:
    case AE_AIRCRAFT_PLANE:
    case AE_AIRCRAFT_SEAPLANE:
    case AE_TRAIN:
    case AE_SPECIAL: {
        CancelAllVehicleEngineSounds();
    }
    }
    StopNonEngineSounds();
}

// 0x4FCF40
void CAEVehicleAudioEntity::JustGotOutOfVehicleAsDriver() {
    m_IsPlayerDriverAboutToExit = false;
    s_pPlayerDriver             = nullptr;

    switch (m_AuSettings.VehicleAudioType) {
    case AE_CAR:
    case AE_BIKE: { // 0x4FD004
        m_EventVolume = GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND) - s_Config.VolOffsetOnGround;
        m_FadeIn      = 1.0f;
        m_FadeOut     = 1.0f;
        if (m_State == eAEState::PLAYER_ID) {
            CancelAllVehicleEngineSounds(AE_SOUND_CAR_ID);
            m_State = eAEState::DUMMY_ID;
        } else {
            CancelAllVehicleEngineSounds();
            if (m_State != eAEState::CAR_OFF) {
                m_State = eAEState::NUM_STATES; // ???
            }
        }
        PlayReverseSound(-1);
        m_AuGear        = 0;
        break;
    }
    case AE_TRAIN: { // 0x4FCF78
        PlayTrainBrakeSound(-1);
        break;
    }
    case AE_SPECIAL: { // 0x4FCF98
        m_EventVolume = GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND) - s_Config.VolOffsetOnGround;
        break;
    }
    case AE_AIRCRAFT_PLANE:
    case AE_AIRCRAFT_HELICOPTER:
    case AE_AIRCRAFT_SEAPLANE: { // 0x4FCFC8
        CancelAllVehicleEngineSounds();
        m_CurrentRotorFrequency    = -1.0f;
        m_CurrentDummyEngineVolume = -100.0f;
        break;
    }
    case AE_BMX:
    case AE_BOAT:
    case AE_ONE_GEAR: {
        break;
    }    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x4F5700
void CAEVehicleAudioEntity::JustGotInVehicleAsDriver() {
    s_pPlayerDriver             = m_Entity->AsVehicle()->GetDriver();
    m_IsPlayerDriverAboutToExit = false;

    const auto MaybeCancelPlayerEngineSounds = [this] {
        if (AESoundManager.AreSoundsPlayingInBankSlot(SND_BANK_SLOT_PLAYER_ENGINE_P)) {
            AESoundManager.CancelSoundsInBankSlot(SND_BANK_SLOT_PLAYER_ENGINE_P, false);
        }
    };
    const auto EnsurePlayerEngineSoundBankIsLoaded = [this] {
        AEAudioHardware.EnsureSoundBankIsLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P);
    };

    switch (m_AuSettings.VehicleAudioType) {
    case AE_CAR:
    case AE_BIKE: { // 0x4F5A08
        m_EventVolume = CAEAudioEntity::GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND);

        m_AuGear                  = 0;
        m_CrzCount                = 0.f;
        m_ACPlayPositionThisFrame = -1;
        m_ACPlayPositionLastFrame = -1;

        MaybeCancelPlayerEngineSounds();

        if (m_DummyEngineBank == SND_BANK_UNK || m_PlayerEngineBank == SND_BANK_UNK) {
            return;
        }

        if (m_DummySlot == SND_BANK_SLOT_NONE) {
            m_DummySlot = DemandBankSlot(m_DummyEngineBank);
        }

        EnsurePlayerEngineSoundBankIsLoaded();

        if (m_State != eAEState::DUMMY_ID || m_DummySlot == SND_BANK_SLOT_NONE) {
            CancelAllVehicleEngineSounds();
            m_State = eAEState::CAR_OFF;
        } else {
            CancelAllVehicleEngineSounds(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;
        }

        break;
    }
    case AE_AIRCRAFT_PLANE:
    case AE_AIRCRAFT_HELICOPTER:
    case AE_AIRCRAFT_SEAPLANE: { // 0x4F5957
        CancelAllVehicleEngineSounds();

        if (m_PlayerEngineBank != SND_BANK_UNK) {
            if (!AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
                MaybeCancelPlayerEngineSounds();
                AEAudioHardware.LoadSoundBank(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P);
            }
        }

        if (m_DummyEngineBank != SND_BANK_UNK && m_DummySlot == SND_BANK_SLOT_NONE) {
            m_DummySlot = DemandBankSlot(m_DummyEngineBank);
        }

        m_CurrentRotorFrequency    = -1.0f;
        m_CurrentDummyEngineVolume = -100.0f;

        break;
    }
    case AE_BMX: { // 0x4F576C
        m_EventVolume = CAEAudioEntity::GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND);

        MaybeCancelPlayerEngineSounds();
        CancelAllVehicleEngineSounds();

        if (m_DummySlot == SND_BANK_SLOT_NONE) {
            m_DummySlot = DemandBankSlot(m_DummyEngineBank);
        }

        EnsurePlayerEngineSoundBankIsLoaded();

        break;
    }
    case AE_BOAT: { // 0x4F57FF
        m_EventVolume = CAEAudioEntity::GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND);

        if (m_DummySlot == SND_BANK_SLOT_NONE) {
            m_DummySlot = DemandBankSlot(m_DummyEngineBank);
        }
        break;
    }
    case AE_TRAIN: { // 0x4F5864
        m_EventVolume = CAEAudioEntity::GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND);

        MaybeCancelPlayerEngineSounds();

        if (m_DummySlot == SND_BANK_SLOT_NONE) {
            m_DummySlot = DemandBankSlot(m_DummyEngineBank);
        }

        EnsurePlayerEngineSoundBankIsLoaded();

        break;
    }
    case AE_SPECIAL: { // 0x4F58FD
        m_EventVolume = CAEAudioEntity::GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND);

        MaybeCancelPlayerEngineSounds();
        CancelAllVehicleEngineSounds();

        if (m_DummyEngineBank != SND_BANK_UNK && m_DummySlot == SND_BANK_SLOT_NONE) {
            m_DummySlot = DemandBankSlot(m_DummyEngineBank);

            EnsurePlayerEngineSoundBankIsLoaded();
        }

        break;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x4F5BA0
void CAEVehicleAudioEntity::PlayerAboutToExitVehicleAsDriver() {
    m_IsPlayerDriverAboutToExit = true;

    if (m_IsPlayerDriver) {
        TurnOffRadioForVehicle();
    }
}

// 0x4F5B20
void CAEVehicleAudioEntity::TurnOnRadioForVehicle() {
    s_pPlayerAttachedForRadio       = m_Entity->AsVehicle()->m_pDriver;
    s_pVehicleAudioSettingsForRadio = &m_AuSettings;
    switch (m_AuSettings.RadioType) {
    case AE_RT_CIVILIAN:
    case AE_RT_UNKNOWN:
    case AE_RT_EMERGENCY:
        AudioEngine.StartRadio(m_AuSettings);
    }
}

// 0x4F5B60
void CAEVehicleAudioEntity::TurnOffRadioForVehicle() {
    switch (m_AuSettings.RadioType) {
    case AE_RT_CIVILIAN:
    case AE_RT_UNKNOWN:
    case AE_RT_EMERGENCY:
        AudioEngine.StopRadio(&m_AuSettings, false);
    }
    s_pVehicleAudioSettingsForRadio = nullptr;
    s_pPlayerAttachedForRadio       = nullptr;
}

// 0x4F5E50
bool CAEVehicleAudioEntity::JustFinishedAccelerationLoop() {
    const auto* const cfg = &s_Config.PlayerEngine.AC;

    if (m_FramesAgoACLooped < cfg->LoopFrameCnt) {
        m_FramesAgoACLooped++;
        return false;
    }
    if (m_ACPlayPositionThisFrame < 0 || m_ACPlayPositionLastFrame < 0) {
        return false;
    }
    if (const auto* const sound = GetEngineSound(AE_SOUND_PLAYER_AC)) {
        if (m_ACPlayPositionThisFrame + cfg->LoopInterval > sound->m_Length) {
            m_FramesAgoACLooped = 0;
            return true;
        }
    }
    return false;
}
#pragma endregion

#pragma region Play*Sound Functions
// Code for `0x4F8360` and `0x4FA630`
void CAEVehicleAudioEntity::GenericPlaySurfaceSound(eSoundBankSlot slot, eSoundID newSoundType, float speed, float volume, float rollOff) noexcept {
    volume += m_EventVolume;

    if (m_SurfaceSoundType == newSoundType) {
        if (m_SurfaceSoundType != -1) {
            m_SkidSound.UpdateTwinLoopSound(m_Entity->GetPosition(), volume, speed);
        }
    } else {
        if (m_SkidSound.IsActive()) {
            m_SkidSound.StopSoundAndForget();
        }
        if ((m_SurfaceSoundType = newSoundType) != -1) {
            m_SkidSound.Initialise(
                slot,
                newSoundType,
                newSoundType + 1,
                this,
                200,
                1'000
            );
            m_SkidSound.PlayTwinLoopSound(
                m_Entity->GetPosition(),
                volume,
                speed,
                rollOff
            );
        }
    }
}

// 0x4F8360
void CAEVehicleAudioEntity::PlaySkidSound(eSoundID newSkidSoundType, float speed, float volume) {
    GenericPlaySurfaceSound(SND_BANK_SLOT_VEHICLE_GEN, newSkidSoundType, speed, volume, 2.5f);
}

// 0x4FA630
void CAEVehicleAudioEntity::PlayTrainBrakeSound(eSoundID soundType, float speed, float volume) {
    if (m_PlayerEngineBank != SND_BANK_UNK) {
        if (!AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
            return;
        }
    }
    GenericPlaySurfaceSound(SND_BANK_SLOT_PLAYER_ENGINE_P, soundType, speed, volume, 3.5f);
}

// @notsa
void PlayGenericSound(
    CAEVehicleAudioEntity* self,
    CAESound*&             s,
    eSoundBankSlot         slot,
    eSoundID&              currSfx,
    eSoundID               newSfx,
    float                  speed,
    float                  volume,
    float                  rollOff
) {
    if (currSfx != newSfx) { // Sound type changing...
        GracefullyStopSound(s);
        if ((currSfx = newSfx) != -1) {
            s = AESoundManager.PlaySound({
                .BankSlotID      = slot,
                .SoundID       = newSfx,
                .AudioEntity   = self,
                .Pos           = self->GetVehicle()->GetPosition(),
                .Volume        = volume,
                .RollOffFactor = rollOff,
                .Speed         = speed,
                .Flags         = SOUND_REQUEST_UPDATES,
            });
        }
    } else if (s && currSfx != -1) { // Same sound type, update existing...
        s->SetSpeed(speed);
        s->SetVolume(volume);
    }
}

// 0x4F84D0
void CAEVehicleAudioEntity::PlayRoadNoiseSound(eSoundID sfx, float speed, float volume) {
    PlayGenericSound(
        this,
        m_RoadNoiseSound,
        SND_BANK_SLOT_VEHICLE_GEN,
        m_RoadNoiseSoundType,
        sfx,
        speed,
        m_EventVolume + volume,
        s_Config.RoadNoiseSoundRollOffFactor
    );
}

// 0x4F8650
void CAEVehicleAudioEntity::PlayFlatTyreSound(eSoundID sfx, float speed, float volume) {
    PlayGenericSound(
        this,
        m_FlatTireSound,
        SND_BANK_SLOT_VEHICLE_GEN,
        m_FlatTireSoundType,
        sfx,
        speed,
        m_EventVolume + volume,
        s_Config.FlatTireSoundRollOffFactor
    );
}

// 0x4F87D0
void CAEVehicleAudioEntity::PlayReverseSound(eSoundID sfx, float speed, float volume) {
    PlayGenericSound(
        this,
        m_ReverseGearSound,
        SND_BANK_SLOT_VEHICLE_GEN,
        m_ReverseGearSoundType,
        sfx,
        speed,
        m_EventVolume + volume,
        s_Config.ReverseGearSoundRollOffFactor
    );
}
#pragma endregion

#pragma region Horn/Siren
// 0x4F62A0
void CAEVehicleAudioEntity::GetSirenState(bool& isSirenOn, bool& isFastSirenOn, tVehicleParams& vp) const noexcept {
#ifdef NOTSA_DEBUG
    if (CPad::GetPad()->IsCtrlPressed()) {
        if (CPad::GetPad()->IsStandardKeyJustDown('J')) {
            isSirenOn = true;
        }
        if (CPad::GetPad()->IsStandardKeyJustDown('K')) {
            isFastSirenOn = true;
        }
        return;
    }
#endif
    if (m_IsWreckedVehicle || !m_HasSiren || !vp.Vehicle->vehicleFlags.bSirenOrAlarm) {
        isSirenOn = false;
    } else if (isSirenOn = (vp.Vehicle->GetStatus() != STATUS_ABANDONED)) {
        isFastSirenOn = vp.Vehicle->m_HornCounter && vp.Vehicle->GetModelIndex() != MODEL_MRWHOOP;
    }
}

// 0x4F61E0
void CAEVehicleAudioEntity::GetHornState(bool* out, tVehicleParams& params) const noexcept {
    *out = GetHornState(params);
}

// Code for 0x4F61E0 (But return value directly instead of taking it as an arg...Ö
bool CAEVehicleAudioEntity::GetHornState(tVehicleParams& vp) const noexcept {
    if (m_IsWreckedVehicle) {
        return false;
    }
    if (vp.Vehicle->m_nAlarmState && vp.Vehicle->m_nAlarmState != -1 && vp.Vehicle->GetStatus() != STATUS_WRECKED) {
        return false;
    }
    if (!vp.Vehicle->m_HornCounter) {
        return false;
    }
    if (vp.Vehicle->GetStatus() == STATUS_PLAYER) {
        return true;
    }
    vp.Vehicle->m_HornCounter = std::min(HORN_PATTERN_SIZE, vp.Vehicle->m_HornCounter);
    if (vp.Vehicle->m_HornCounter == HORN_PATTERN_SIZE) {
        vp.Vehicle->m_HornPattern = (int8)(CGeneral::GetRandomNumberInRange(0u, HORN_NUM_PATTERNS));
    }
    return HornPattern[vp.Vehicle->m_HornPattern][HORN_PATTERN_SIZE - vp.Vehicle->m_HornCounter];
}

// 0x4F99D0
void CAEVehicleAudioEntity::PlayHornOrSiren(bool isHornOn, bool isSirenOn, bool isFastSirenOn, tVehicleParams& vp) {
    //
    // Play/stop horn sound
    //
    if (isHornOn && !m_IsHornOn) {
        const auto PlayHornSound = [&](float speed) {
            m_HornSound = AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_HORN_AND_SIREN,
                .SoundID       = (eSoundID)(m_AuSettings.HornType),
                .AudioEntity   = this,
                .Pos           = m_Entity->GetPosition(),
                .Volume        = m_HornVolume,
                .RollOffFactor = 2.5f,
                .Speed         = speed,
                .Doppler       = s_Config.Horn.Doppler,
                .Flags         = SOUND_REQUEST_UPDATES,
            });
        };
        const auto areSoundsLoaded = AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_HORN, SND_BANK_SLOT_HORN_AND_SIREN)
                                  || AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN);
        if (m_AuSettings.HornType >= eAEVehicleHornType::CAR_FIRST && m_AuSettings.HornType <= eAEVehicleHornType::CAR_LAST) { // 0x4F9B7C
            m_HornVolume = m_AuSettings.EngineVolumeOffset + GetDefaultVolume(AE_CAR_HORN);
            if (!areSoundsLoaded) { // 0x4F9BA3
                return;
            }
            if (m_HornSound) { // 0x4F9BC5
                return;
            }
            PlayHornSound(m_AuSettings.HornPitch); // 0x4F9C23
        } else if (m_AuSettings.HornType == eAEVehicleHornType::BIKE) { // 0x4F9A22
            StopAndForgetSound(m_HornSound);
            m_HornVolume = GetDefaultVolume(AE_BIKE_BELL);
            if (!areSoundsLoaded) { // 0x4F9A71
                return;
            }
            PlayHornSound(1.f); // 0x4F9AD0
        } else {
            return;
        }
    } else if (m_AuSettings.HornType == eAEVehicleHornType::BIKE) { 
        m_HornVolume = std::max(m_HornVolume - std::pow(s_Config.Horn.BikeBellFadeOut, CTimer::GetTimeStep()), -100.0f); // NB: Framerate bugfix with `std::pow`
        if (m_HornVolume <= -100.0f) {
            StopAndForgetSound(m_HornSound);
        } else if (m_HornSound) {
            m_HornSound->SetVolume(m_HornVolume);
        }
    } else if (!isHornOn && m_IsHornOn) { // 0x4F9B08
        StopAndForgetSound(m_HornSound); // 0x4F9C46
    }

    //
    // Play/stop siren sound (Refactored completely, logic is the same)
    //
    const auto PlaySirenSound = [&](eSoundBankSlot slot, eSoundID sid) {
        return AESoundManager.PlaySound({
            .BankSlotID    = slot,
            .SoundID       = sid,
            .AudioEntity   = this,
            .Pos           = m_Entity->GetPosition(),
            .Volume        = GetDefaultVolume(AE_SIREN),
            .RollOffFactor = 3.f,
            .Doppler       = s_Config.Siren.Doppler,
            .Flags         = SOUND_REQUEST_UPDATES,
        });
    };

    const auto areHornSoundsLoaded = AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_HORN, SND_BANK_SLOT_HORN_AND_SIREN);

    // Check if fast siren needs playing now
    if (isFastSirenOn) {
        if (!m_IsFastSirenOn && !m_FastSirenSound) {
            if (areHornSoundsLoaded) {
                m_FastSirenSound = PlaySirenSound(SND_BANK_SLOT_HORN_AND_SIREN, SND_GENRL_HORN_SIREN_SHORT); // 0x4F9E2E
            }
        }
    } else {
        StopAndForgetSound(m_FastSirenSound);
    }

    // Check if standard siren needs to be played - Fast siren has precedence!
    if (isSirenOn && !isFastSirenOn) {
        if ((!m_IsSirenOn || m_IsFastSirenOn) && !m_SirenSound) {
            if (areHornSoundsLoaded) {
                const auto isMrWhoop = vp.Vehicle->GetModelId() == MODEL_MRWHOOP;
                if (isMrWhoop && !AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_ICEVAN_P, SND_BANK_SLOT_PLAYER_ENGINE_P)) { // 0x4F9CCD
                    return;
                }
                m_SirenSound = PlaySirenSound(
                    isMrWhoop ? SND_BANK_SLOT_PLAYER_ENGINE_P : SND_BANK_SLOT_HORN_AND_SIREN,
                    isMrWhoop ? SND_GENRL_HORN_HORNBMW328 : SND_GENRL_HORN_SIREN_LONG
                );
            }
        }
    } else {
        StopAndForgetSound(m_SirenSound);
    }
}

#pragma endregion

#pragma region Vehicle Skidding
// 0x4F8F10
void CAEVehicleAudioEntity::ProcessVehicleSkidding(tVehicleParams& vp) {
    float  sumWheelSkid{ 0.f };
    uint32 numWheels{ 0 };

    const auto CalculateWheelSkid = [&](
        float  gasPedalAudioRevs,
        bool   areRearWheelsNotSkidding,
        auto&& wheelStates,
        auto&& wheelCounts
    ) {
        numWheels = std::size(wheelStates);
        
        if (!*GetNumOfContactWheels(vp)) {
            return;
        }
        for (auto&& [i, wstate] : rngv::enumerate(wheelStates)) {
            const auto isFrontWheel = i == CAR_WHEEL_FRONT_LEFT
                                   || i == CAR_WHEEL_FRONT_RIGHT;

            if (wstate == WHEEL_STATE_NORMAL) {
                continue;
            }
            if (wheelCounts[i] <= 0.0f) {
                continue;
            }
            if (areRearWheelsNotSkidding && isFrontWheel && wstate == WHEEL_STATE_SKIDDING) {
                continue;
            }

            const auto dt = vp.Transmission->m_nDriveType;
            sumWheelSkid += dt == '4' || dt == 'F' && isFrontWheel || dt == 'R' && !isFrontWheel
                ? GetVehicleDriveWheelSkidValue(vp.Vehicle, wstate, gasPedalAudioRevs, *vp.Transmission, vp.Speed)
                : GetVehicleNonDriveWheelSkidValue(vp.Vehicle, wstate, *vp.Transmission, vp.Speed);
        }
    };
    switch (vp.BaseVehicleType) {
    case VEHICLE_TYPE_AUTOMOBILE: {
        auto* const a = vp.Vehicle->AsAutomobile();
        CalculateWheelSkid(
            a->m_GasPedalAudioRevs,
            a->m_WheelStates[CAR_WHEEL_REAR_LEFT] != WHEEL_STATE_SKIDDING && a->m_WheelStates[CAR_WHEEL_REAR_RIGHT] != WHEEL_STATE_SKIDDING,
            a->m_WheelStates,
            a->m_WheelCounts
        );
        break;
    }
    case VEHICLE_TYPE_BIKE: {
        auto* const b = vp.Vehicle->AsBike();
        CalculateWheelSkid(
            b->m_GasPedalAudioRevs,
            b->m_WheelStates[1] != WHEEL_STATE_SKIDDING,
            b->m_WheelStates,
            b->m_WheelCounts
        );
        break;
    }
    default:
        NOTSA_UNREACHABLE();
    }

    const auto StopSkidSound = [&] {
        PlaySkidSound(-1, 1.0f, -100.f);
    };
    if (CTimer::GetTimeStep() == 0.f || sumWheelSkid <= 0.f) {
        StopSkidSound();
        return;
    }

    const auto DoPlaySkidSound = [&](eSoundID sfx, float speed, float volume) {
        const float skidPerWheel = sumWheelSkid / (float)(numWheels);
        if (skidPerWheel > 0.00001f) {
            volume += CAEAudioUtility::AudioLog10(skidPerWheel) * 20.0f + s_Config.Skid.VolBase;
            if (volume >= -100.0f) {
                return PlaySkidSound(sfx, speed, volume);
            }
        }
        StopSkidSound();
    };
    if (m_AuSettings.VehicleAudioType == AE_BMX) { // 0x4F90D3
        auto* const cfg = &s_Config.Skid.BMX;
        DoPlaySkidSound(SND_GENRL_VEHICLE_GEN_BIKE_SKID_A, cfg->VolBase, cfg->FrqBase + cfg->FrqWheelSkidFactor * sumWheelSkid);
    } else {
        float vol = m_AuSettings.IsPlaneOrHeli()
            ? s_Config.Skid.VolOffsetForPlaneOrHeli
            : 0.f;
        if (IsSurfaceAudioGrass(vp.Vehicle->m_nContactSurface)) { // 0x4F9114
            auto* const cfg = &s_Config.Skid.Generic.GrassSurface;
            DoPlaySkidSound(SND_GENRL_VEHICLE_GEN_GRASS_SKID_A, cfg->FrqBase + cfg->FrqWheelSkidFactor * sumWheelSkid, cfg->VolBase + vol);
        } else if (IsSurfaceWet(vp.Vehicle->m_nContactSurface)) { // 0x4F914E
            auto* const cfg = &s_Config.Skid.Generic.WetSurface;
            DoPlaySkidSound(SND_GENRL_VEHICLE_GEN_GRAVEL_SKID_A, cfg->FrqBase + cfg->FrqWheelSkidFactor * sumWheelSkid, cfg->VolBase + vol);
        } else { // 0x4F919B
            auto* const cfg = &s_Config.Skid.Generic.StdSurface;
            vol += cfg->VolBase;
            if (m_AuSettings.IsBike()) {
                vol *= cfg->FrqFactorForBikes;
            }
            DoPlaySkidSound(SND_GENRL_VEHICLE_GEN_TARSKIDTWIN1, cfg->FrqBase + cfg->FrqWheelSkidFactor * sumWheelSkid, vol);
        }
    }
}

// 0x4F5F30
float CAEVehicleAudioEntity::GetVehicleDriveWheelSkidValue(CVehicle* veh, tWheelState wheelState, float gasPedalAudioRevs, cTransmission& tr, float speed) const noexcept {
    const auto* const cfg = &s_Config.Skid.DriveWheelSkid;

    if (!cfg->Enabled) {
        return 0.0f;
    }

    switch (wheelState) {
    case WHEEL_STATE_NORMAL:
        return 0.f;
    case WHEEL_STATE_SPINNING:
        return gasPedalAudioRevs > 0.4f
            ? invLerp(0.4f, 1.f, gasPedalAudioRevs) * 0.75f * cfg->SpinningFactor
            : 0.f;
    case WHEEL_STATE_SKIDDING:
        return std::min(1.0f, std::fabs(speed)) * 0.75f * cfg->SkiddingFactor;
    case WHEEL_STATE_FIXED: {
        const auto s = std::fabs(speed);
        return s >= 0.04f
            ? std::min(s * 5.0f, 1.0f) * cfg->StationaryFactor
            : 0.f;
    }
    }
    NOTSA_UNREACHABLE();
}

// 0x4F6000
float CAEVehicleAudioEntity::GetVehicleNonDriveWheelSkidValue(CVehicle* vehicle, tWheelState wheelState, cTransmission&, float velocity) const noexcept {
    const auto* const cfg = &s_Config.Skid.NonDriveWheelSkid;

    if (!cfg->Enabled) {
        return 0.0f;
    }

    switch (wheelState) {
    case WHEEL_STATE_NORMAL:
    case WHEEL_STATE_SPINNING:
        return 0.0f;
    case WHEEL_STATE_SKIDDING: {
        auto s = std::min(1.0f, std::fabs(velocity)) * 0.75f;
        if (m_AuSettings.IsBMX()) {
            s *= 0.2f;
        }
        return s * cfg->SkiddingFactor;
    }
    case WHEEL_STATE_FIXED: {
        auto s = std::abs(velocity);
        if (s >= 0.04f) {
            s = std::min(s * 5.0f, 1.0f);
        }
        return s * cfg->StationaryFactor;
    }
    }
    NOTSA_UNREACHABLE();
}
#pragma endregion

// 0x4FAD40
void CAEVehicleAudioEntity::UpdateGenericVehicleSound(eVehicleEngineSoundType st, eSoundBankSlot bankSlot, eSoundBank bank, eSoundID sfx, float speed, float volume, float rollOff) {
    volume += m_EventVolume;

    if (auto& s = m_EngineSounds[st].Sound) {
        s->SetVolume(volume);
        s->SetSpeed(speed);
    } else if (AEAudioHardware.IsSoundBankLoaded(bank, bankSlot)) {
        s = AESoundManager.PlaySound({
            .BankSlotID      = bankSlot,
            .SoundID       = sfx,
            .AudioEntity   = this,
            .Pos           = m_Entity->GetPosition(),
            .Volume        = volume,
            .RollOffFactor = rollOff,
            .Speed         = speed,
            .Flags         = SOUND_REQUEST_UPDATES,
        });
    } else {
        NOTSA_LOG_WARN("[UpdateGenericVehicleSound({})] Sound bank not loaded [Bank = {}, Slot = {}]", (int32)(st), (int32)(bank), (int32)(bankSlot));
    }
}

// @notsa
void CAEVehicleAudioEntity::UpdateOrRequestVehicleEngineSound(eVehicleEngineSoundType st, float freq, float volume) {
    if (GetEngineSound(st)) {
        UpdateVehicleEngineSound(st, freq, volume);
    } else {
        RequestNewPlayerCarEngineSound(st, freq, volume);
    }
}

// 0x4F8940
void CAEVehicleAudioEntity::ProcessVehicleFlatTyre(tVehicleParams& vp) {
    const auto* const cfg = &s_Config.FlatTyre;

    const auto spinning = [&]{
        switch (m_AuSettings.VehicleAudioType) {
        case AE_SPECIAL: {
            if (vp.Vehicle->GetModelIndex() != MODEL_CADDY) {
                return false;
            }
            [[fallthrough]];
        }
        case AE_CAR: {
            for (int32 i = 0; i < MAX_CARWHEELS; i++) {
                if (vp.Vehicle->AsAutomobile()->m_WheelCounts[i] <= 0.f) {
                    continue;
                }
                if (vp.Vehicle->AsAutomobile()->GetDamageManager().GetWheelStatus((eCarWheel)(i)) != WHEEL_STATE_SPINNING) {
                    continue;
                }
                return true;
            }
            return false;
        }
        case AE_BIKE:
        case AE_BMX: {
            for (int32 i = 0; i < 2; i++) {
                if (vp.Vehicle->AsBike()->m_WheelCounts[i] <= 0.f) {
                    continue;
                }
                if (vp.Vehicle->AsBike()->m_nWheelStatus[i] != WHEEL_STATE_SPINNING) {
                    continue;
                }
                return true;
            }
            return false;
        }
        default:
            return false;
        }
    }();
    if (spinning) {
        const auto vf = std::min(1.f, std::abs(vp.Speed) / (vp.Transmission->m_MaxVelocity * 0.3f)); // velocity factor
        PlayFlatTyreSound(
            4,
            1.f + cfg->FrqGearVelocityFactor * vf,
            vf > 0.f
                ? cfg->VolOffset + CAEAudioUtility::AudioLog10(vf) * 20.f 
                : -100.f
        );
    } else {
        PlayFlatTyreSound(-1);
    }
}

// 0x4F8B00
void CAEVehicleAudioEntity::ProcessVehicleRoadNoise(tVehicleParams& vp) {
    const auto* const cfg = &s_Config.RoadNoise;

    // 0x4F8B47
    bool cancel = false;
    switch (vp.Vehicle->GetModelIndex()) {
    case MODEL_ARTICT1:
    case MODEL_ARTICT2:
    case MODEL_PETROTR:
    case MODEL_ARTICT3: cancel |= !GetNumOfContactWheels(vp).value_or(4); break;
    default:            cancel |= !vp.Transmission;                       break;
    }

    // 0x4F8B88
    const float absSpeed = std::fabs(vp.Speed);
    if (cancel || absSpeed <= 0.0f) {
        PlayRoadNoiseSound(-1);
        return;
    }

    // Play sound now
    const auto PlaySound = [&](eSoundID sfx, float speed, float volume) {
        // 0x4F8BFA (Notice the multiplication!)
        speed *= 0.75f + std::max(1.0f, (TheCamera.GetPosition() - m_Entity->GetPosition()).Magnitude() / cfg->FadeOutDistance) / 2.0f;

        // 0x4F8CF8
        if (const auto v = std::min(1.0f, 2.0f * absSpeed); v > 0.0008f) {
            volume += cfg->VolBase + CAEAudioUtility::AudioLog10(v) * 20.f;
        } else {
            volume = -100.f;
        }

        // 0x4F8D35
        PlayRoadNoiseSound(sfx, speed, volume);
    };
    if (IsSurfaceAudioGrass(vp.Vehicle->m_nContactSurface)) { // 0x4F8C34
        const auto* const c = &cfg->GrassSurface;
        PlaySound(21, c->FrqFactor, c->VolOffset);
    } else if (IsSurfaceWet(vp.Vehicle->m_nContactSurface)) { // 0x4F8C6A
        const auto* const c = &cfg->WetSurface;
        PlaySound(22, c->FrqFactor, c->VolOffset);
    } else {
        const auto* const c = &cfg->StdSurface;
        PlaySound(21, c->FrqFactor, c->VolOffset);
    }
}

// 0x4F8DF0 - Called only if `AE_CAR` and the player is the driver
void CAEVehicleAudioEntity::ProcessReverseGear(tVehicleParams& vp) {
    auto* const a = vp.Vehicle->AsAutomobile();
    const auto* cfg = &s_Config.ReverseGear;

    if (a->vehicleFlags.bEngineOn && (a->m_GasPedal < 0.0f || a->m_nCurrentGear == 0)) { // Are we reversing?
        if (!a->m_NumDriveWheelsOnGround && a->m_NumDriveWheelsOnGroundLastFrame) {
            a->m_GasPedalAudioRevs *= 0.4f;
        }
        const auto revs = a->m_NumDriveWheelsOnGround
            ? vp.Speed / vp.Transmission->m_MaxReverseVelocity
            : a->m_GasPedalAudioRevs;
        PlayReverseSound(
            a->m_GasPedal >= 0.0f ? 20 : 19,
            cfg->FrqBase + std::abs(revs) * cfg->FrqRevsFactor,
            std::abs(revs) > 0.0f
                ? cfg->VolBase + CAEAudioUtility::AudioLog10(std::abs(revs)) * 20.0f
                : -100.0f
        );
    } else { // Cancel sound otherwise
        PlayReverseSound(-1, 1.0f, -100.0f);
    }
}

// 0x4F92C0
void CAEVehicleAudioEntity::ProcessRainOnVehicle(tVehicleParams& params) {
    const auto* const cfg = &s_Config.RainOnVehicle;

    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_RAIN, SND_BANK_SLOT_WEATHER)) {
        return;
    }
    if (CAEWeatherAudioEntity::m_sfRainVolume <= -100.0f) {
        return;
    }
    if (++m_RainDropCounter < 3) {
        return;
    }
    m_RainDropCounter = 0;
    AESoundManager.PlaySound({
        .BankSlotID          = SND_BANK_SLOT_WEATHER,
        .SoundID           = (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(12, 15)),
        .AudioEntity       = this,
        .Pos               = m_Entity->GetPosition(),
        .Volume            = cfg->VolBase + CAEWeatherAudioEntity::m_sfRainVolume,
        .FrequencyVariance = 0.1f,
        .EventID           = AE_RAIN_COLLISION
    });
}

// Android
void CAEVehicleAudioEntity::ProcessPlayerGolfCart(tVehicleParams& params) {
    // NOP
}

// 0x4FAE20
void CAEVehicleAudioEntity::ProcessEngineDamage(tVehicleParams& vp) {
    const auto* const cfg = &s_Config.EngineDamage;

    // 0x4FAE2A - These have been combined from all the various conditions in the function...
    const auto fucked = !m_IsWreckedVehicle && (vp.Vehicle->GetHealth() < 390.f || vp.Vehicle->vehicleFlags.bIsDrowning) && m_AuSettings.IsCar() && vp.Vehicle->vehicleFlags.bEngineOn;
    const auto steam  = !m_IsWreckedVehicle && (vp.Vehicle->GetHealth() < 650.f && !vp.Vehicle->vehicleFlags.bIsDrowning);

    // 0x4FAEAD - Velocity factor
    const auto vf = vp.Transmission
        ? std::clamp(std::abs(vp.Speed / vp.Transmission->m_MaxVelocity), 0.f, 1.f)
        : 0.f;

    // 0x4FAFE5
    if (steam) {
        UpdateGenericVehicleSound(
            AE_SOUND_STEAM,
            SND_BANK_SLOT_WEAPON_GEN,
            SND_BANK_GENRL_WEAPONS,
            SND_GENRL_WEAPONS_SPRAY_PAINT,
            lerp(cfg->SteamSound.FrqMin, cfg->SteamSound.FrqMax, vf),
            lerp(cfg->SteamSound.VolMin, cfg->SteamSound.VolMax, vf),
            cfg->SteamSound.RollOff
        );
    } else {
        StopGenericEngineSound(AE_SOUND_STEAM);
    }

    // 0x4FAF30
    if (fucked) {
        UpdateGenericVehicleSound(
            AE_SOUND_FUCKED,
            SND_BANK_SLOT_VEHICLE_GEN,
            SND_BANK_GENRL_VEHICLE_GEN,
            SND_GENRL_WEAPONS_SPRAY_PAINT,
            lerp(cfg->FuckedSound.FrqMin, cfg->FuckedSound.FrqMax, vf),
            lerp(cfg->FuckedSound.VolMin, cfg->FuckedSound.VolMax, vf),
            cfg->FuckedSound.RollOff
        );
    } else {
        StopGenericEngineSound(AE_SOUND_FUCKED);
    }
}

// 0x4FB070
void CAEVehicleAudioEntity::ProcessNitro(tVehicleParams& vp) {
    const auto* const cfg = &s_Config.Nitro;
    const auto* const a   = vp.Vehicle->AsAutomobile();

    if (a->handlingFlags.bNosInst && a->m_fTireTemperature < 0.f && a->vehicleFlags.bEngineOn && !a->vehicleFlags.bIsDrowning) {
        if (!m_bNitroOnLastFrame) {
            AddAudioEvent(AE_NITRO_IGNITION, 0.f);
        }

        const auto nitro = (float)(vp.ThisAccel);
        m_CurrentNitroRatio = m_CurrentNitroRatio >= 0.f
            ? notsa::step_to(m_CurrentNitroRatio, nitro, cfg->StepUp, cfg->StepDown, notsa::bugfixes::GenericFrameRate)
            : nitro;

        const auto* const s1 = &cfg->Sounds[0];
        UpdateGenericVehicleSound(
            AE_SOUND_NITRO1,
            SND_BANK_SLOT_WEAPON_GEN,
            SND_BANK_GENRL_WEAPONS,
            SND_GENRL_WEAPONS_GAS_LOOP,
            lerp(s1->FrqMin, s1->FrqMax, nitro),
            lerp(s1->VolMin, s1->VolMax, nitro),
            3.f
        );
        
        const auto* const s2 = &cfg->Sounds[1];
        UpdateGenericVehicleSound(
            AE_SOUND_NITRO2,
            SND_BANK_SLOT_VEHICLE_GEN,
            SND_BANK_GENRL_VEHICLE_GEN,
            26,
            lerp(s2->FrqMin, s2->FrqMax, nitro),
            lerp(s2->VolMin, s2->VolMax, nitro),
            3.f
        );

        m_bNitroOnLastFrame = true;
    } else if (m_bNitroOnLastFrame) { // 0x4FB1E5
        StopGenericEngineSound(AE_SOUND_NITRO1);
        StopGenericEngineSound(AE_SOUND_NITRO2);

        m_bNitroOnLastFrame = false;
        m_CurrentNitroRatio = -1.f;
    }
}

// 0x4FB260
void CAEVehicleAudioEntity::ProcessMovingParts(tVehicleParams& vp) {
    auto* const a = vp.Vehicle->AsAutomobile();
    const auto* const cfg = &s_Config.MovingParts;

    switch (vp.Vehicle->GetModelIndex()) {
    case MODEL_PACKER:
    case MODEL_DOZER:
    case MODEL_DUMPER:
    //case MODEL_CEMENT: // This was originally here, but below there's no data defined for it - Perhaps R* wanted it to have sounds like other vehicles...
    case MODEL_ANDROM:
    case MODEL_FORKLIFT:
        break;
    default:
        return;
    }
    
    const auto delta = (float)(a->m_wMiscComponentAngle - a->m_wMiscComponentAnglePrev) / cfg->AngleDeltaFactor;
    m_MovingPartSmoothedSpeed = notsa::step_to(
        m_MovingPartSmoothedSpeed,
        std::clamp<float>(std::fabs(delta), 0.0f, 1.0f),
        cfg->StepUp, cfg->StepDown,
        true
    );

    const auto* const props   = &cfg->PropsByModel.at(vp.Vehicle->GetModelId());
    const auto        slot    = props->Slot.value_or(m_DummySlot);
    const auto* const params = &props->SoundParamsByPartDir[delta <= 0.f ? 0 : 1];

    const auto volume = params->Vol + CAEAudioUtility::AudioLog10(m_MovingPartSmoothedSpeed) * 20.0f;
    if (volume <= -100.0f) {
        StopGenericEngineSound(AE_SOUND_MOVING_PARTS);
    } else {
        UpdateGenericVehicleSound(
            AE_SOUND_MOVING_PARTS,
            slot,
            props->Bank,
            props->Sfx,
            params->Frq,
            volume,
            1.5f
        );
    }
}

// 0x4F5D00
float CAEVehicleAudioEntity::GetVolForPlayerEngineSound(tVehicleParams& vp, eVehicleEngineSoundType st) const noexcept {
    const auto& cfg = s_Config.PlayerEngine;

    float volume = [&]{
        switch (st) {
        case AE_SOUND_CAR_REV:
            return lerp(cfg.Rev.VolMin, cfg.Rev.VolMax, vp.RealRevsRatio);
        case AE_SOUND_CAR_ID:
            return vp.Vehicle->GetModelIndex() == MODEL_CADDY
                ? cfg.ID.VolMin - 30.f
                : lerp(cfg.ID.VolMin, cfg.ID.VolMax, vp.RealRevsRatio);
        case AE_SOUND_PLAYER_CRZ:
            return m_IsSingleGear
                ? cfg.SingleGearVolume
                : lerp(cfg.Crz.VolMin, cfg.Crz.VolMax, m_CrzCount / (float)(cfg.CrzMaxCnt)); // NOTE: May be wrong
        case AE_SOUND_PLAYER_AC:
            return m_IsSingleGear
                ? cfg.SingleGearVolume
                : lerp(cfg.AC.VolMin, cfg.AC.VolMax, m_AuGear / (float)(cfg.MaxAuGear)); // NOTE: May be wrong
        case AE_SOUND_PLAYER_OFF:
            return GetVolForPlayerEngineSound(vp, AE_SOUND_CAR_ID);
        default:
            return 0.f;
        }
    }();
    if (GetVehicle()->vehicleFlags.bIsDrowning) {
        volume -= s_Config.DummyEngine.VolumeUnderwaterOffset;
    }
    if (GetVehicle()->m_pVehicleBeingTowed) {
        volume += s_Config.DummyEngine.VolumeTrailerOffset;
    }
    if (m_bNitroOnLastFrame && m_CurrentNitroRatio <= 1.0f && m_CurrentNitroRatio >= 0.0f) {
        volume += cfg.VolNitroFactor * m_CurrentNitroRatio;
    }
    return volume + m_AuSettings.EngineVolumeOffset;
}

// 0x4F5C60
float CAEVehicleAudioEntity::GetFreqForIdle(float ratio) const noexcept {
    return CGeneral::GetPiecewiseLinear({ // 0x8CC164:
        { 0.000f, 0.000f },
        { 0.075f, 0.700f },
        { 0.150f, 1.100f },
        { 0.250f, 1.250f },
        { 1.000f, 1.700f },
    }, ratio);
}

// 0x4F8070
float CAEVehicleAudioEntity::GetFreqForPlayerEngineSound(tVehicleParams& vp, eVehicleEngineSoundType st) const noexcept {
    auto* const cfg = &s_Config.PlayerEngine;

    float f;
    if (st == AE_SOUND_CAR_ID && vp.Vehicle->GetModelId() == MODEL_CADDY) { // Moved out here to simplify logic...
        f = 0.f;
    } else {
        // 0x4F80A6 - Calculate z move speed factor
        auto zf = vp.ZOverSpeed <= 0.f
            ? +(std::max(cfg->FrqZMoveSpeedLimitMin, vp.ZOverSpeed) / cfg->FrqZMoveSpeedLimitMin)
            : -(std::min(cfg->FrqZMoveSpeedLimitMax, vp.ZOverSpeed) / cfg->FrqZMoveSpeedLimitMax);
        zf *= cfg->FrqZMoveSpeedFactor;
        if (vp.Speed < -0.001f) {
            zf = -zf;
        }

        // 0x4F8111 - Calculate gear factor
        float gf = std::min(1.f, vp.AbsSpeed / vp.Transmission->m_aGears[1].ChangeUpVelocity);

        // 0x4F8131 - Calculate bike factor
        float bf = 0.f;
        if (m_AuSettings.VehicleAudioType == AE_BIKE) {
            auto* const bike = vp.Vehicle->AsBike();

            bf = std::sin(bike->GetRideAnimData()->LeanAngle) * cfg->FrqBikeLeanFactor;
            if (bike->bikeFlags.bPlayerBoost) {
                bf += cfg->FrqPlayerBikeBoostOffset;
            }
            bf *= gf;
        }

        // 0x4F81AB - Calculate frequency with some additional factors for the sound type
        switch (st) {
        case AE_SOUND_CAR_REV: { // 0x4F81BB
            f = vp.WheelSpin <= 5.f
                ? cfg->Rev.FrqOffset + vp.WheelSpin / 5.f * cfg->Rev.FrqWheelSpinFactor
                : cfg->Rev.FrqOffset + lerp(cfg->Rev.FrqMin, cfg->Rev.FrqMax, invLerp(5.f, 10.f, vp.WheelSpin));
            break;
        }
        case AE_SOUND_CAR_ID: { // 0x4F8235
            f = lerp(cfg->ID.FrqMin, cfg->ID.FrqMax, GetFreqForIdle(vp.SpeedRatio));
            break;
        }
        case AE_SOUND_PLAYER_CRZ: { // 0x4F826D
            f  = vp.WheelSpin / 5.f * cfg->FrqWheelSpinFactor; 
            f += (cfg->Crz.FrqMax - cfg->Crz.FrqMin) * m_CrzCount / (float)(cfg->CrzMaxCnt); // NB: Not sure if this was intended to be a lerp or not...
            f += m_IsSingleGear
                ? cfg->Crz.FrqSingleGear
                : cfg->Crz.FrqMin;
            break;
        }
        case AE_SOUND_PLAYER_AC: { // 0x4F8281
            f  = vp.WheelSpin / 5.f * cfg->FrqWheelSpinFactor;
            f += m_IsSingleGear
                ? cfg->AC.FrqSingleGear
                : cfg->AC.FrqPerGearFactor[m_AuGear] + cfg->AC.FrqMultiGearOffset;
            break;
        }
        case AE_SOUND_PLAYER_OFF: { // 0x4F82C1
            f = lerp(cfg->Off.FrqMin, cfg->Off.FrqMax, vp.SpeedRatio);
            break;
        }
        default:
            NOTSA_UNREACHABLE();
        }
        f *= ((gf * zf) + 1.f) * (bf + 1.f);
    }

    if (vp.Vehicle->m_bUnderwater) {
        f *= s_Config.FreqUnderwaterFactor;
    }

    if (m_bNitroOnLastFrame && 0.f <= m_CurrentNitroRatio && m_CurrentNitroRatio <= 1.f) {
        f *= cfg->FrqNitroFactor * m_CurrentNitroRatio + 1.f;
    }

    return f;
}

// 0x4FBB10
void CAEVehicleAudioEntity::ProcessPlayerVehicleEngine(tVehicleParams& vp) {
    auto* const cfg = &s_Config.PlayerEngine;

    if (!AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
        return;
    }

    GetAccelAndBrake(vp);

    vp.AbsSpeed      = std::fabsf(vp.Speed);
    vp.SpeedRatio    = std::clamp(vp.AbsSpeed / vp.Transmission->m_MaxVelocity, 0.f, 1.f);
    vp.RealGear      = vp.Vehicle->m_nCurrentGear;
    vp.IsHandbrakeOn = vp.Vehicle->vehicleFlags.bIsHandbrakeOn;

    auto* const gear = &vp.Transmission->m_aGears[vp.RealGear];
    vp.RealRevsRatio = vp.RealGear
        ? invLerp(gear->ChangeDownVelocity, gear->MaxVelocity, vp.AbsSpeed)
        : 0.f;

    vp.WheelSpin     = vp.Vehicle->m_fWheelSpinForAudio;
    vp.NumGears      = vp.Transmission->m_nNumberOfGears;
    vp.ZOverSpeed    = vp.Speed != 0.f
           ? vp.Vehicle->GetMoveSpeed().z / vp.Speed
           : 0.f;

    if (vp.Vehicle->IsBike()) { // 0x4FBC86
        auto* const b = vp.Vehicle->AsBike();

        vp.NumDriveWheelsOnGround          = b->m_NumDriveWheelsOnGround;
        vp.NumDriveWheelsOnGroundLastFrame = b->m_NumDriveWheelsOnGroundLastFrame;
        vp.GasPedalAudioRevs               = &b->m_GasPedalAudioRevs;
        vp.WheelState                      = b->m_WheelStates;
        vp.PrevSpeed                       = b->m_PrevSpeed;
    } else if (vp.Vehicle->IsAutomobile()) {
        auto* const a = vp.Vehicle->AsAutomobile();

        vp.NumDriveWheelsOnGround          = a->m_NumDriveWheelsOnGround;
        vp.NumDriveWheelsOnGroundLastFrame = a->m_NumDriveWheelsOnGroundLastFrame;
        vp.GasPedalAudioRevs               = &a->m_GasPedalAudioRevs;
        vp.WheelState                      = a->m_WheelStates.data();
        vp.PrevSpeed                       = a->m_PrevSpeed;
    } else {
        NOTSA_UNREACHABLE();
    }

    if (m_IsSingleGear) { // 0x4FBCEA
        vp.RealGear = vp.RealGear ? 1 : 0;
        vp.NumGears = 1;
    }

    const auto RequestEngineSound = [&](int16 st) {
        RequestNewPlayerCarEngineSound(st, GetFreqForPlayerEngineSound(vp, st), GetVolForPlayerEngineSound(vp, st));
    };

    const auto UpdateEngineSound = [&](int16 st) {
        UpdateVehicleEngineSound(st, GetFreqForPlayerEngineSound(vp, st), GetVolForPlayerEngineSound(vp, st));
    };

    const auto UpdateOrRequestEngineSound = [&](int16 st) {
        if (GetEngineSound(st)) {
            UpdateEngineSound(st);
        } else {
            RequestEngineSound(st);
        }
    };

    const auto DoFullAcceleration = [&] {
        if (m_IsSingleGear) {
            m_AuGear = 1;
        } else {
            assert(vp.NumGears == vp.Transmission->m_nNumberOfGears);
            assert(vp.NumGears > 0);
            m_AuGear = std::clamp<uint8>(std::min(vp.RealGear, m_AuGear), 1, vp.NumGears - 1);
        }
        RequestEngineSound(AE_SOUND_PLAYER_AC);
        m_State = eAEState::PLAYER_AC_FULL;
    };

    const bool isReversing = vp.Speed < -0.001f;

    switch (m_State) {
    case eAEState::CAR_OFF: { // 0x4FBD24
        if (!vp.Vehicle->vehicleFlags.bEngineOn) {
            CancelAllVehicleEngineSounds(AE_SOUND_STEAM);
            m_State = eAEState::CAR_OFF;

            break;
        }

        AddAudioEvent(AE_START_ENGINE, 0.f);

        if (!isReversing) { // 0x4FBD66 (inverted!)
            if (vp.ThisAccel <= 0) { // 0x4FBDD0
                RequestNewPlayerCarEngineSound(AE_SOUND_CAR_ID, 1.f, -100.f);
                m_State = eAEState::PLAYER_ID;

                break;
            }

            if (!IsAccInhibited(vp) && !IsAccInhibitedForLowSpeed(vp)) { // 0x4FBDE9
                if (IsAccInhibitedForLowSpeed(vp)) { // 0x4FBDF7 - NB: Condition always false due to the cond. above...
                    RequestEngineSound(AE_SOUND_CAR_ID);
                    m_State = eAEState::PLAYER_ID;
                } else if (vp.ThisAccel <= s_Config.PlayerEngine.AC.WheelSpinThreshold) { // 0x4FBE0F
                    RequestNewPlayerCarEngineSound(AE_SOUND_CAR_ID);
                    m_State = eAEState::PLAYER_ID;
                } else if (vp.RealGear < vp.NumGears && s_Config.PlayerEngine.CrzSpeedOffset + vp.PrevSpeed > vp.Speed && vp.ZOverSpeed > s_Config.PlayerEngine.ZMoveSpeedThreshold) { // 0x4FBE15
                    vp.Vehicle->vehicleFlags.bAudioChangingGear = true;
                    RequestEngineSound(AE_SOUND_PLAYER_CRZ);
                    m_State = eAEState::PLAYER_CRZ;
                } else {
                    CancelVehicleEngineSound(AE_SOUND_PLAYER_OFF);
                    RequestEngineSound(AE_SOUND_PLAYER_AC);
                    m_State = eAEState::PLAYER_AC_FULL;
                }

                break;
            }
        }

        if (vp.ThisAccel <= s_Config.PlayerEngine.AC.WheelSpinThreshold) {  // 0x4FBB10
            RequestEngineSound(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;
        } else {
            CancelVehicleEngineSound(AE_SOUND_PLAYER_OFF);
            RequestEngineSound(AE_SOUND_CAR_REV);
            m_State = eAEState::PLAYER_WHEEL_SPIN;
        }

        break;
    }
    case eAEState::PLAYER_AC_FULL: {
        if (!vp.Vehicle->vehicleFlags.bEngineOn) { // 0x4FC4DA
            CancelAllVehicleEngineSounds();
            m_State = eAEState::CAR_OFF;

            break;
        }

        if (isReversing) { // 0x4FC502
            CancelVehicleEngineSound(AE_SOUND_PLAYER_AC);
            if (vp.ThisAccel > s_Config.PlayerEngine.AC.WheelSpinThreshold) { // 0x4FC504
                RequestEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_WHEEL_SPIN;
            } else {
                RequestEngineSound(AE_SOUND_PLAYER_OFF);
                RequestEngineSound(AE_SOUND_CAR_ID);
                m_State = eAEState::PLAYER_ID;
            }
            break;
        }

        if (vp.ThisAccel <= 0) { // 0x4FC521
            CancelVehicleEngineSound(AE_SOUND_PLAYER_AC);
            RequestEngineSound(AE_SOUND_PLAYER_OFF);
            RequestEngineSound(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;

            break;
        }

        if (IsAccInhibited(vp)) { // 0x4FC52D
            CancelVehicleEngineSound(AE_SOUND_PLAYER_AC);
            if (vp.ThisAccel <= s_Config.PlayerEngine.AC.WheelSpinThreshold) { // 0x4FC543
                RequestEngineSound(AE_SOUND_PLAYER_OFF);
                RequestEngineSound(AE_SOUND_CAR_ID);
                m_State = eAEState::PLAYER_ID;
            } else {
                RequestEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_WHEEL_SPIN;
            }
            break;
        }

        if (   IsAccInhibitedForLowSpeed(vp)
            || IsAccInhibitedForLowSpeed(vp)
            || vp.ThisAccel <= s_Config.PlayerEngine.AC.WheelSpinThreshold
        ) { // 0x4FC558 - NB: Same function (in the cond) called twice?
            CancelVehicleEngineSound(AE_SOUND_PLAYER_AC);
            RequestEngineSound(AE_SOUND_PLAYER_OFF);
            RequestEngineSound(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;

            break;
        }

        if (   vp.RealGear < vp.NumGears
            && s_Config.PlayerEngine.CrzSpeedOffset + vp.PrevSpeed > vp.Speed
            && (vp.ZOverSpeed > s_Config.PlayerEngine.ZMoveSpeedThreshold || CCullZones::DoExtraAirResistanceForPlayer())
            && !IsCrzInhibitedForTime()
        ) { // 0x4FC5E2
            vp.Vehicle->vehicleFlags.bAudioChangingGear = true;
            CancelVehicleEngineSound(AE_SOUND_PLAYER_AC);
            RequestEngineSound(AE_SOUND_PLAYER_CRZ);
            InhibitCrzForTime(2'000);
            m_State = eAEState::PLAYER_CRZ;

            break;
        }

        if (JustFinishedAccelerationLoop()) { // 0x4FC631 - Try change gears
            const auto gear = m_AuGear;
            m_AuGear = std::min(s_Config.PlayerEngine.MaxAuGear, m_AuGear + 1);
            if (gear + 2 <= vp.NumGears || vp.RealGear < vp.NumGears && !CCullZones::DoExtraAirResistanceForPlayer()) { // 0x4FC640 - Got more gears?
                vp.Vehicle->vehicleFlags.bAudioChangingGear = true;
                UpdateEngineSound(AE_SOUND_PLAYER_AC);
                m_State = eAEState::PLAYER_AC_FULL;
            } else { // 0x4FC67B - Last gear, switch to crz
                if (!m_IsSingleGear) {
                    vp.Vehicle->vehicleFlags.bAudioChangingGear = true;
                }
                CancelVehicleEngineSound(AE_SOUND_PLAYER_AC);
                RequestEngineSound(AE_SOUND_PLAYER_CRZ);
                m_State = eAEState::PLAYER_CRZ;
            }
        } else {
            UpdateOrRequestEngineSound(AE_SOUND_PLAYER_AC);
        }

        break;
    }
    case eAEState::PLAYER_WHEEL_SPIN: { // 0x4FC76A
        if (!vp.Vehicle->vehicleFlags.bEngineOn) { // 0x4FC76A
            CancelAllVehicleEngineSounds();
            m_State = eAEState::CAR_OFF;

            break;
        }

        if (isReversing) { // 0x4FC795
            if (vp.ThisAccel > s_Config.PlayerEngine.AC.WheelSpinThreshold) { // 0x4FC7A2
                UpdateEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_WHEEL_SPIN;
            } else {
                CancelVehicleEngineSound(AE_SOUND_CAR_REV);
                RequestEngineSound(AE_SOUND_PLAYER_OFF);
                RequestEngineSound(AE_SOUND_CAR_ID);
                m_State = eAEState::PLAYER_ID;
            }

            break;
        }

        if (vp.ThisAccel <= 0) { // 0x4FC7DA
            CancelVehicleEngineSound(AE_SOUND_CAR_REV);
            RequestEngineSound(AE_SOUND_PLAYER_OFF);
            RequestEngineSound(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;

            break;
        }

        if (IsAccInhibited(vp) || IsAccInhibitedForLowSpeed(vp)) { // 0x4FC7F6
            if (vp.ThisAccel > s_Config.PlayerEngine.AC.WheelSpinThreshold) { // 0x4FC93B
                UpdateEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_WHEEL_SPIN;

                break;
            }
        } else if (!IsAccInhibitedForLowSpeed(vp) && vp.ThisAccel > s_Config.PlayerEngine.AC.WheelSpinThreshold) { // 0x4FC828
            CancelVehicleEngineSound(AE_SOUND_CAR_REV);
            if (   vp.RealGear < vp.NumGears
                && s_Config.PlayerEngine.CrzSpeedOffset + vp.PrevSpeed > vp.Speed
                && vp.ZOverSpeed > s_Config.PlayerEngine.ZMoveSpeedThreshold
            ) { // 0x4FC841
                vp.Vehicle->vehicleFlags.bAudioChangingGear = true;
                RequestEngineSound(AE_SOUND_PLAYER_CRZ);
                m_State = eAEState::PLAYER_CRZ;
            } else {
                DoFullAcceleration(); // 0x4FC3A1
            }
            break;
        }

        // 0x4FC96F
        CancelVehicleEngineSound(AE_SOUND_CAR_REV);
        RequestEngineSound(AE_SOUND_PLAYER_OFF);
        RequestEngineSound(AE_SOUND_CAR_ID);
        m_State = eAEState::PLAYER_ID;

        break;
    }
    case eAEState::PLAYER_CRZ: { // 0x4FBEC8
        if (!vp.Vehicle->vehicleFlags.bEngineOn) {
            CancelAllVehicleEngineSounds();
            m_State = eAEState::CAR_OFF;

            break;
        }

        const auto EndCrz = [&] {
            m_CrzCount = 0.f;
            CancelVehicleEngineSound(AE_SOUND_PLAYER_CRZ);
        };
        const auto CrzToIdle = [&] {
            EndCrz();
            RequestEngineSound(AE_SOUND_PLAYER_OFF);
            RequestEngineSound(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;
        };
        const auto EndCrzWhenInhibited = [&] {
            if (vp.ThisAccel <= cfg->AC.WheelSpinThreshold) {
                CrzToIdle();
            } else {
                EndCrz();
                RequestEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_WHEEL_SPIN;
            }
        };

        if (isReversing) { // 0x4FBEF3
            EndCrzWhenInhibited();
            break;
        }

        if (vp.ThisAccel <= 0) {
            CrzToIdle();
            break;
        }

        if (IsAccInhibited(vp) || IsAccInhibitedForLowSpeed(vp)) { // 0x4FBF55
            EndCrzWhenInhibited();
            break;
        }

        if (!IsAccInhibitedForLowSpeed(vp) && vp.ThisAccel > cfg->AC.WheelSpinThreshold) {
            if (   vp.RealGear < vp.NumGears
                && s_Config.PlayerEngine.AC.SpeedOffset + vp.PrevSpeed < vp.Speed
                && !IsAccInhibitedForTime()
                && !CCullZones::DoExtraAirResistanceForPlayer()
            ) { // 0x4FBFC7 (Inverted!)
                EndCrz();
                InhibitAccForTime(2'000);
                DoFullAcceleration(); // 0x4FC8C2
            } else { // 0x4FC0B7 - Still doing crz, update sound
                m_CrzCount = vp.Speed <= vp.PrevSpeed - 0.05f
                    ? std::max(0.f, m_CrzCount - 0.25f)
                    : std::min(m_CrzCount + 1.f, (float)(cfg->CrzMaxCnt));
                UpdateEngineSound(AE_SOUND_PLAYER_CRZ);
                m_State = eAEState::PLAYER_CRZ;
            }
        } else {
            CrzToIdle();
        }
        break;
    }
    case eAEState::PLAYER_ID: {
        if (!vp.Vehicle->vehicleFlags.bEngineOn) {
            CancelAllVehicleEngineSounds();
            m_State = eAEState::CAR_OFF;

            break;
        }

        if (isReversing) { // 0x4FC211
            if (vp.ThisAccel <= s_Config.PlayerEngine.AC.WheelSpinThreshold) { // (Inverted!)
                UpdateEngineSound(AE_SOUND_CAR_ID);
                m_State = eAEState::PLAYER_ID;
            } else { // 0x4FC224
                CancelVehicleEngineSound(AE_SOUND_CAR_ID);
                CancelVehicleEngineSound(AE_SOUND_PLAYER_OFF);
                RequestEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_WHEEL_SPIN;
            }
            break;
        }

        if (vp.ThisAccel <= 0) { // 0x4FC27D
            UpdateEngineSound(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;

            break;
        }

        if (IsAccInhibited(vp) || IsAccInhibitedForLowSpeed(vp)) { // 0x4FC28B
            if (vp.ThisAccel > cfg->AC.WheelSpinThreshold) { // 0x4FC41E
                CancelVehicleEngineSound(AE_SOUND_CAR_ID);
                CancelVehicleEngineSound(AE_SOUND_PLAYER_OFF);
                RequestEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_WHEEL_SPIN;

                break;
            }
        } else if (!IsAccInhibitedForLowSpeed(vp) && vp.ThisAccel > cfg->AC.WheelSpinThreshold) {
            CancelVehicleEngineSound(AE_SOUND_CAR_ID);
            CancelVehicleEngineSound(AE_SOUND_PLAYER_OFF);

            if (   vp.RealGear < vp.NumGears
                && vp.PrevSpeed + cfg->CrzSpeedOffset > vp.Speed
                && vp.ZOverSpeed > cfg->ZMoveSpeedThreshold
            ) {
                vp.Vehicle->vehicleFlags.bAudioChangingGear = true;
                RequestEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_CRZ;
            } else {
                DoFullAcceleration();
            }
        } else {
            UpdateEngineSound(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;
        }
        break;
    }
    default:
        NOTSA_UNREACHABLE("Invalid state: {}", (int)(m_State));
    }
}

#pragma region RC Utility functions
// notsa
void CAEVehicleAudioEntity::UpdateDummyRCAcAndBrake(tVehicleParams& vp) {
    if (s_pPlayerDriver && m_IsPlayerDriver) {
        GetAccelAndBrake(vp);
    } else {
        vp.ThisAccel = 0.f;
        vp.ThisBrake = 0.f;
    }
}
#pragma endregion

#pragma region Aircraft
// notsa - Update rotor frequency smoothly
void CAEVehicleAudioEntity::UpdateRotorFreq(float freq, float stepUp, float stepDown) {
    m_CurrentRotorFrequency = m_CurrentRotorFrequency < 0.f
        ? freq
        : notsa::step_to(m_CurrentRotorFrequency, freq, stepUp, stepDown, notsa::bugfixes::GenericFrameRate);
}

// notsa
auto GetAircraftDistantSoundVolume(const CPhysical* v, float baseVol) {
    const auto distSq = CVector::DistSqr(TheCamera.GetPosition(), v->GetPosition());
    if (distSq <= sq(48.f)) {
        return -100.f;
    }
    auto vol = baseVol;
    if (distSq <= 128.f) {
        vol += invLerp(48.f, 128.f, std::sqrtf(distSq)); // NB: I think this should be substracted (The closer the plane is, the less volume the distant sound should have)
    }
    return CAEAudioUtility::AudioLog10(vol) * 20.f + 8.f;
}

// notsa - See 0x4FE5E5
auto GetAircraftCameraPoVFactor(const CPhysical* p) {
    return ((p->GetPosition() - TheCamera.GetPosition()).Normalized().Dot(p->GetForward()) + 1.f) * 0.5f;
}

// notsa - Get acceleration factor for aircrafts
auto GetAircraftAcceleration(CAEVehicleAudioEntity::tVehicleParams& vp) {
    auto s = vp.ThisAccel > 0
        ? 0.1f
        : 0.f;
    if (vp.ThisBrake > 0) {
        s -= 0.05f;
    }
    return s;
}

// notsa
auto GetDummyRCRotorSpeedFactor(float s) {
    s = std::min(1.f, s);
    return s > 0.2f
        ? std::max(0.2f, lerp(0.f, 1.25f, s - 0.2f))
        : 0.2f;
}


// 0x4F96A0
CVector CAEVehicleAudioEntity::GetAircraftNearPosition() const noexcept {
    if (m_AuSettings.IsHeli()) {
        CVector pos;
        m_Entity->AsVehicle()->GetComponentWorldPosition(s_Config.HeliAudioComponent, pos);
        return pos;
    }
    return m_Entity->GetPosition();
}

// 0x501C50
void CAEVehicleAudioEntity::ProcessAircraft(tVehicleParams& params) {
    if (m_IsWreckedVehicle) { // location optimized
        return;
    }

    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
        return;
    }

    auto* vehicle = params.Vehicle;
    switch (m_AuSettings.VehicleAudioType) {
    case AE_AIRCRAFT_HELICOPTER: {
        if (s_HelicoptorsDisabled || m_IsHelicopterDisabled) {
            JustWreckedVehicle();
        } else if (m_IsPlayerDriver) {
            ProcessPlayerHeli(params);
        } else if (vehicle->GetStatus() == STATUS_PHYSICS) {
            ProcessAIHeli(params);
        } else {
            ProcessDummyHeli(params);
        }
        break;
    }
    case AE_AIRCRAFT_PLANE: {
        switch (vehicle->GetModelIndex()) {
        case MODEL_SHAMAL:
        case MODEL_HYDRA:
        case MODEL_AT400:
        case MODEL_ANDROM: { // Originally there was an `isPlane` variable. I just rearranged stuff, to look nicer
            if (m_IsPlayerDriver) {
                ProcessPlayerJet(params);
            } else {
                ProcessDummyJet(params);
            }
            break;
        }
        default: {
            if (m_IsPlayerDriver) {
                ProcessPlayerProp(params);
            } else if (vehicle->GetStatus() == STATUS_PHYSICS || vehicle->m_autoPilot.m_vehicleRecordingId >= 0) {
                ProcessAIProp(params);
            } else {
                ProcessDummyProp(params);
            }
        }
        }
        break;
    }
    case AE_AIRCRAFT_SEAPLANE: {
        if (m_IsPlayerDriver) {
            ProcessPlayerSeaPlane(params);
        } else {
            ProcessDummySeaPlane(params);
        }
        break;
    }
    }
}

// 0x4F93C0
void CAEVehicleAudioEntity::PlayAircraftSound(eAircraftSoundType st, eSoundBankSlot slot, eSoundID sfx, float volume, float speed) {
    if (notsa::bugfixes::CAEVehicleAudioEntity_PlayAircraftSound_VolumeFix) { // Most likely they left this out, because, for example, vehicle sounds (`ProcessPlayerVehicleEngine`) use `UpdateVehicleEngineSound` that does this too
        volume += m_EventVolume;
    }
    if (UpdateVehicleEngineSoundRawVolume(st, speed, volume)) {
        return;
    }
    const auto DoPlaySound = [&](float rollOff, float doppler = 1.0f, std::optional<CVector> pos = std::nullopt) {
        m_EngineSounds[st].Sound = AESoundManager.PlaySound({
            .BankSlotID      = slot,
            .SoundID       = sfx,
            .AudioEntity   = this,
            .Pos           = pos.value_or(m_Entity->GetPosition()),
            .Volume        = m_EventVolume + volume,
            .RollOffFactor = rollOff,
            .Speed         = speed,
            .Doppler       = doppler,
            .Flags         = SOUND_REQUEST_UPDATES
        });
    };
    switch (st) {
    case AE_SOUND_AIRCRAFT_DISTANT:     DoPlaySound(6.0f);                                 break;
    case AE_SOUND_AIRCRAFT_FRONT:                                                          
    case AE_SOUND_AIRCRAFT_REAR:        DoPlaySound(4.0f);                                 break;
    case AE_SOUND_AIRCRAFT_NEAR:        DoPlaySound(1.0f, 0.4f);                           break;
    case AE_SOUND_AIRCRAFT_THRUST:      DoPlaySound(4.5f);                                 break;
    case AE_SOUND_AIRCRAFT_JET_DISTANT: DoPlaySound(50.0f, 1.f, CVector{ 0.f, 1.f, 0.f }); break;
    default:                            NOTSA_UNREACHABLE();
    }
}

#pragma region Propellers

// notsa
auto GetPropSpeedFactor(float propSpeed) {
    return CGeneral::GetPiecewiseLinear({ // 0x8CBE00
        { 0.00f, 0.0f },
        { 0.15f, 0.3f },
        { 1.00f, 1.0f }
    }, std::clamp(propSpeed, 0.f, 1.f));
}

// notsa
float CalculatePropSpeed(const CPlane* plane, float gas, float brake) {
    return plane->m_autoPilot.m_vehicleRecordingId != -1
        ? 0.7f + 0.3f * std::clamp(std::max(gas, brake), 0.f, 1.f)
        : plane->m_fPropSpeed / 0.34f;
}

// notsa
float CalculatePlanePropFreq(CAEVehicleAudioEntity::tVehicleParams& vp, float ac) {
    const auto plane = vp.Vehicle->AsPlane();
    return (
        + std::abs(plane->GetRight().Dot(CVector::ZAxisVector())) * 0.1f // Right tilt
        - plane->GetForward().Dot(CVector::ZAxisVector()) * 0.15f // Backward tilt
        + ac
        + 1.f
    );
}

// 0x4FDFD0 - Process AI propeller sound
void CAEVehicleAudioEntity::ProcessAIProp(tVehicleParams& vp) {
    auto* const plane = vp.Vehicle->AsPlane();

    if (m_DummyEngineBank == SND_BANK_UNK) { // 0x4FE008
        return;
    }
    if (!EnsureHasDummySlot() || !EnsureSoundBankIsLoaded(true)) {
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) { // 0x4FE0CA
        return;
    }
    if (!vp.Vehicle->vehicleFlags.bEngineOn && vp.Vehicle->m_autoPilot.m_vehicleRecordingId == -1) { // 0x4FE0F2
        CancelAllVehicleEngineSounds();
        return;
    }
     
    // 0x4FE129
    const auto propSpeed       = CalculatePropSpeed(plane, vp.ThisAccel, vp.ThisBrake);
    const auto propSpeedFactor = GetPropSpeedFactor(propSpeed);

    // 0x4FE25A
    UpdateRotorFreq(CalculatePlanePropFreq(vp, 0.f), 1.f / 187.5f, 1.f / 187.5f);
    
    // 0x4FE3CF
    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_FRONT,
        m_DummySlot,
        1,
        CAEAudioUtility::AudioLog10(propSpeedFactor) * 20.f + 8.f,
        m_CurrentRotorFrequency
    );
    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_DISTANT,
        m_DummySlot,
        0,
        GetAircraftDistantSoundVolume(plane, propSpeedFactor) // <- 0x4FE2EF
    );
}

// Based on `ProcessDummyProp`. Implementation for both `ProcessDummyProp` and `ProcessPlayerProp`
// Process plane propeller sounds
void CAEVehicleAudioEntity::ProcessDummyOrPlayerProp(tVehicleParams& vp, bool isDummy) {
    const auto* const plane = vp.Vehicle->AsPlane();

    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN) || !AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
        return;
    }

    if (!plane->vehicleFlags.bEngineOn && (isDummy || plane->m_autoPilot.m_vehicleRecordingId < 0)) {
        CancelAllVehicleEngineSounds();
        return;
    }

    if (isDummy) {
        vp.ThisAccel = plane->m_GasPedal;
        vp.ThisBrake = plane->m_BrakePedal;
    } else {
        GetAccelAndBrake(vp);
    }

    // 0x4FDA9B
    const auto propSpeedFactor = GetPropSpeedFactor(std::clamp(CalculatePropSpeed(plane, vp.ThisAccel, vp.ThisBrake), 0.f, 1.f));

    // 0x4FDDCA
    float propStalledVol, propStalledFreq;
    ProcessPropOrJetStall(plane, propStalledVol, propStalledFreq);

    // 0x4FDEB7 - Calculate and update rotor frequency
    UpdateRotorFreq(CalculatePlanePropFreq(vp, GetAircraftAcceleration(vp)) * propStalledFreq, 1 / 187.5f, 1 / 187.5f);
    
    const auto camPoVFactor = GetAircraftCameraPoVFactor(plane);

    // 0x4FDF72
    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_FRONT,
        SND_BANK_SLOT_PLAYER_ENGINE_P,
        0,
        propStalledVol + CAEAudioUtility::AudioLog10((1.f - s_Config.RotorVolTiltFactor * camPoVFactor) * propSpeedFactor) * 20.f + 8.f, // 0x4FDBC9
        m_CurrentRotorFrequency
    );
    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_REAR,
        SND_BANK_SLOT_PLAYER_ENGINE_P,
        1,
        propStalledVol + CAEAudioUtility::AudioLog10((camPoVFactor * 0.5f + 0.5f) * propSpeedFactor) * 20.f + 8.f, // 0x4FDC1A
        m_CurrentRotorFrequency
    );
    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_NEAR,
        SND_BANK_SLOT_VEHICLE_GEN,
        17,
        propStalledVol + CAEAudioUtility::AudioLog10(propSpeedFactor) * 20.f + 8.f, // 0x4FDC3C
        1.f
    );
    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_DISTANT,
        SND_BANK_SLOT_VEHICLE_GEN,
        16,
        GetAircraftDistantSoundVolume(plane, propStalledVol), // <- 0x4FDC90
        1.f
    );
}

// 0x4FD290
void CAEVehicleAudioEntity::ProcessPlayerProp(tVehicleParams& vp) {
    ProcessDummyOrPlayerProp(vp, false);
}

// 0x4FD8F0
void CAEVehicleAudioEntity::ProcessDummyProp(tVehicleParams& vp) {
    ProcessDummyOrPlayerProp(vp, true);
}

// notsa (Used in ProcessPlayerProp, ProcessDummyProp, ProcessPlayerJet)
void CAEVehicleAudioEntity::ProcessPropOrJetStall(const CPlane* plane, float& outVol, float& outFreq) {
    outVol        = -100.f;
    outFreq       = 1.f;

    const auto hp = plane->m_fHealth;

    // NOTE: Why not use piecewise?
    std::tie(outVol, outFreq) = [&]() -> std::pair<float, float> {
        if (hp >= 650.f) {
            return { 0.f, 1.f };
        }
        if (hp >= 460.f) { // 0x50180B 
            return { -6.f, 0.95f };
        }
        if (hp >= 390.f) { // 0x5017EA
            return { -9.f, 0.90f };
        }
        if (hp >= 250.f) { // 0x5017C2
            return { -12.f, 0.85f };
        }
        return { -18.f, 0.80f }; // 0x50177C
    }();

    if (m_DoCountStalls) {
        m_StalledCount = 15;
    }

    if (hp >= 650.f) {
        return;
    }

    if (CTimer::GetTimeInMS() >= m_SwapStalledTime) {
        constexpr static struct {
            float FastStallProbability;
            struct {
                uint32 Min, Max;
            } Intervals[2]; // 0 = slow stall, 1 = fast stall 

            auto GetRandomStallTime() const {
                const auto& i = Intervals[CAEAudioUtility::ResolveProbability(FastStallProbability)];
                return CAEAudioUtility::GetRandomNumberInRange(i.Min, i.Max);
            }
        } s_StallTimes[2]{
            { 0.95f, {{ 400, 420 }, { 75, 175 }} }, // Silent
            { 0.85f, {{ 175, 250 }, { 25, 75  }} }, // Non-Silent
        };
        m_SwapStalledTime = CTimer::GetTimeInMS() + s_StallTimes[m_IsSilentStalled].GetRandomStallTime();
        m_IsSilentStalled = !m_IsSilentStalled;
    }
    m_StalledCount = std::max(0, m_StalledCount - 1);

    if (m_IsSilentStalled) {
        outVol = 0.f;
    }
}

#pragma endregion

#pragma region Heli
auto GetHeliRotorSpeed(const CHeli* h) {
    return h->m_fHeliRotorSpeed / 0.22f;
}

// notsa
auto GetVolumeFactorForHeliRotor(float rotorSpeed) {
    return CGeneral::GetPiecewiseLinear({ // 0x8CBE40
        { 0.00f, 0.00f },
        { 0.15f, 0.00f },
        { 0.38f, 0.40f },
        { 0.63f, 0.70f },
        { 1.00f, 1.00f },
    }, rotorSpeed);
}

// notsa
auto GetHeliRotorFreqFactor(float rotorSpeed) {
    return CGeneral::GetPiecewiseLinear({ // 0x8CBE18
        { 0.00f, 0.00f },
        { 0.15f, 0.00f },
        { 0.38f, 0.60f },
        { 0.63f, 1.00f },
        { 1.00f, 1.00f },
    }, rotorSpeed);
}

// notsa
auto GetHeliForwardTilt(const CHeli* h) {
    return (1.f - h->GetUp().Dot(CVector::ZAxisVector())) * 0.5f;
}

// notsa
void CAEVehicleAudioEntity::UpdateHeliRotorFrequency(const CHeli* heli, float rotorSpeed, float acceleration, bool isDummy) {
    const auto* const cfg = &s_Config.Heli;

    auto rotorFreq = (GetHeliForwardTilt(heli) * 0.2f + 1.f) // Forward tilt
                   * GetHeliRotorFreqFactor(rotorSpeed); // Factor for rotor rotation speed
    if (!isDummy) {
        rotorFreq *= acceleration + 1.f;
    }
    if (heli->GetHealth() < 390.f) {
        const auto* const var = &cfg->RotorFreqVariance;
        rotorFreq *= CAEAudioUtility::GetRandomNumberInRange(var->Min, var->Max) + var->Offset;
    }
    const auto* const stepping = rotorSpeed <= 0.63f
        ? &cfg->RotorFreqStepLoSpeed
        : &cfg->RotorFreqStepHiSpeed;
    UpdateRotorFreq(rotorFreq, stepping->StepUp, stepping->StepDown);
}

// 0x4FEE20
void CAEVehicleAudioEntity::ProcessAIHeli(tVehicleParams& vp) {
    const auto* const heli = vp.Vehicle->AsHeli();
    const auto* const cfg = &s_Config.Heli;

    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_APACHE_D, SND_BANK_SLOT_COP_HELI)) {
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
        return;
    }

    // 0x4FEEB8
    const auto rotorSpeed = std::clamp(GetHeliRotorSpeed(heli), 0.f, 1.f);

    // 0x4FF067 - Calculate and update rotor frequency
    UpdateHeliRotorFrequency(heli, rotorSpeed, 1.f, true);

    // 0x4FEFD1 (Moved down here)
    const auto camPoVFactor = GetAircraftCameraPoVFactor(heli);

    // 0x4FE50D (Moved here)
    const auto rotorVolFactor = GetVolumeFactorForHeliRotor(rotorSpeed);

    if (CWeather::UnderWaterness >= 0.5f) {
        CancelAllVehicleEngineSounds(AE_SOUND_AIRCRAFT_DISTANT);
        PlayAircraftSound(
            AE_SOUND_AIRCRAFT_DISTANT,
            SND_BANK_SLOT_VEHICLE_GEN,
            3,
            CAEAudioUtility::AudioLog10(rotorVolFactor) * 20.f + 8.f,
            1.f
        );
    } else if (m_PlayerEngineBank != SND_BANK_GENRL_APACHE) {
        PlayAircraftSound(
            AE_SOUND_AIRCRAFT_FRONT,
            SND_BANK_SLOT_VEHICLE_GEN,
            5,
            CAEAudioUtility::AudioLog10((1.f - camPoVFactor) * rotorVolFactor) * 20.f + 8.f, // <- 0x4FEFFD
            m_CurrentRotorFrequency
        );
        PlayAircraftSound(
            AE_SOUND_AIRCRAFT_REAR,
            SND_BANK_SLOT_VEHICLE_GEN,
            18,
            CAEAudioUtility::AudioLog10((camPoVFactor * 0.5f + 0.5f) * rotorVolFactor) * 20.f + 8.f, // <- 0x4FF034
            m_CurrentRotorFrequency
        );
        PlayAircraftSound(
            AE_SOUND_AIRCRAFT_NEAR,
            SND_BANK_SLOT_VEHICLE_GEN,
            23,
            CAEAudioUtility::AudioLog10(rotorVolFactor) * 20.f + 2.f, // <- 0x4FF04C
            1.f
        );
        PlayAircraftSound(
            AE_SOUND_AIRCRAFT_DISTANT,
            SND_BANK_SLOT_VEHICLE_GEN,
            3,
            GetAircraftDistantSoundVolume(heli, rotorVolFactor),
            1.f
        );
    } else {
        PlayAircraftSound(
            AE_SOUND_AIRCRAFT_FRONT,
            SND_BANK_SLOT_COP_HELI,
            1,
            cfg->CopHeliVolOffset + CAEAudioUtility::AudioLog10(rotorVolFactor) * 20.f + 8.f,
            m_CurrentRotorFrequency
        );
        PlayAircraftSound(
            AE_SOUND_AIRCRAFT_DISTANT,
            SND_BANK_SLOT_COP_HELI,
            0,
            cfg->CopHeliVolOffset + GetAircraftDistantSoundVolume(heli, rotorVolFactor),
            1.f
        );
    }
}

// notsa
// Used for `ProcessPlayerHeli` and `ProcessDummyHeli`
// References code from `ProcessPlayerHeli`
void CAEVehicleAudioEntity::ProcessDummyOrPlayerHeli(tVehicleParams& vp, bool isDummy) {
    const auto* const heli = vp.Vehicle->AsHeli();
    const auto* const cfg = &s_Config.Heli;

    if (!AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
        return;
    }

    // 0x4FE4B4
    const auto rotorSpeed = std::clamp(GetHeliRotorSpeed(heli), 0.f, 1.f); // <- Maybe piecewise is wrong?

    // 0x4FE528
    GetAccelAndBrake(vp);

    // 0x4FE6CA - Calculate and update rotor frequency
    UpdateHeliRotorFrequency(heli, rotorSpeed, isDummy ? 1.f : GetAircraftAcceleration(vp), isDummy);

    // 0x4FE5E5 (Moved down here)
    const auto camPoVFactor = GetAircraftCameraPoVFactor(heli);

    // 0x4FE50D (Moved here)
    const auto rotorVolFactor = GetVolumeFactorForHeliRotor(rotorSpeed);

    // 0x4FE8C5
    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_FRONT,
        SND_BANK_SLOT_PLAYER_ENGINE_P,
        0,
        CAEAudioUtility::AudioLog10((1.f - s_Config.RotorVolTiltFactor * camPoVFactor) * rotorVolFactor) * 20.f + 8.f, // <- 0x4FE643
        m_CurrentRotorFrequency
    );

    // 0x4FE8D8
    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_REAR,
        SND_BANK_SLOT_PLAYER_ENGINE_P,
        1,
        CAEAudioUtility::AudioLog10((camPoVFactor * 0.5f + 0.5f) * rotorVolFactor) * 20.f + 8.f, // <- 0x4FE68B
        m_CurrentRotorFrequency
    );

    // 0x4FE8EF
    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_NEAR,
        SND_BANK_SLOT_PLAYER_ENGINE_P,
        3,
        CAEAudioUtility::AudioLog10(rotorVolFactor) * 20.f + 2.f, // <- 0x4FE6A6
        1.f
    );

    // 0x4FE906
    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_DISTANT,
        SND_BANK_SLOT_VEHICLE_GEN,
        SND_GENRL_VEHICLE_GEN_DISTANT,
        GetAircraftDistantSoundVolume(heli, rotorVolFactor), // <- [0x4FE840 - 0x4FE840]
        1.f
    );
}

// 0x4FE420
void CAEVehicleAudioEntity::ProcessPlayerHeli(tVehicleParams& vp) {
    ProcessDummyOrPlayerHeli(vp, false);
}

// 0x4FE940
void CAEVehicleAudioEntity::ProcessDummyHeli(tVehicleParams& vp) {
    //const auto* const heli = vp.Vehicle->AsHeli();
    //const auto* const cfg = &s_Config.Heli;
    //
    //if (!AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
    //    return;
    //}
    //if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
    //    return;
    //}
    //
    //// 0x4FE9C6
    //const auto rotorSpeed = std::clamp(GetHeliRotorSpeed(heli), 0.f, 1.f);
    //
    //// 0x4FF067 - Calculate and update rotor frequency
    //UpdateHeliRotorFrequency(heli, rotorSpeed, true);
    ProcessDummyOrPlayerHeli(vp, true);
}
#pragma endregion

#pragma region Seaplane

// 0x4FF320
void CAEVehicleAudioEntity::ProcessPlayerSeaPlane(tVehicleParams& vp) {
    const auto* const veh = vp.Vehicle->AsBoat(); // Yes,it's actually a boat

    if (!AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
        return;
    }

    // 0x4FF3CA
    GetAccelAndBrake(vp);

    // 0x4FF5F1
    CalculatePlanePropFreq(vp, GetAircraftAcceleration(vp));
    
    const auto propSpeed    = veh->m_fPropSpeed / 0.22f;
    const auto camPoVFactor = GetAircraftCameraPoVFactor(veh);
    const auto sf           = GetDummyRCRotorSpeedFactor(propSpeed);
    PlayAircraftSound( // 0x4FF74A
        AE_SOUND_AIRCRAFT_FRONT,
        SND_BANK_SLOT_PLAYER_ENGINE_P,
        0,
        CAEAudioUtility::AudioLog10((1.f - camPoVFactor) * sf) * 20.f + 8.f, // <- 0x4FF569
        m_CurrentRotorFrequency
    );
    PlayAircraftSound( // 0x4FF75D
        AE_SOUND_AIRCRAFT_REAR,
        SND_BANK_SLOT_PLAYER_ENGINE_P,
        1,
        CAEAudioUtility::AudioLog10((camPoVFactor * 0.5f + 0.5f) * sf) * 20.f + 8.f, // <- 0x4FF320
        m_CurrentRotorFrequency
    );
    PlayAircraftSound( // 0x4FF774
        AE_SOUND_AIRCRAFT_NEAR,
        SND_BANK_SLOT_VEHICLE_GEN,
        17,
        CAEAudioUtility::AudioLog10(sf) * 20.f + 8.f, // <- 0x4FF5B5
        1.f
    );
    PlayAircraftSound( // 0x4FF78B
        AE_SOUND_AIRCRAFT_DISTANT,
        SND_BANK_SLOT_VEHICLE_GEN,
        16,
        GetAircraftDistantSoundVolume(veh, std::clamp(propSpeed, 0.2f, 1.f)),
        1.f
    );
}

// 0x4FF7C0
void CAEVehicleAudioEntity::ProcessDummySeaPlane(tVehicleParams& vp) {
    const auto* const plane = vp.Vehicle->AsBoat();

    if (!plane->vehicleFlags.bEngineOn)  {
        CancelAllVehicleEngineSounds();
        return;
    }

    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
        return;
    }

    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_NEAR,
        SND_BANK_SLOT_VEHICLE_GEN,
        17,
        GetAircraftDistantSoundVolume(plane, 0.f),
        1.f
    );
}
#pragma endregion

#pragma region Jet

// 0x4FF900
void CAEVehicleAudioEntity::ProcessGenericJet(bool bEngineOn, tVehicleParams& params, float fEngineSpeed, float fAccelRatio, float fBrakeRatio, float fStalledVolume, float fStalledFrequency) {
    plugin::CallMethod<0x4FF900, CAEVehicleAudioEntity*, uint8, tVehicleParams&, float, float, float, float, float>(this, bEngineOn, params, fEngineSpeed, fAccelRatio, fBrakeRatio, fStalledVolume, fStalledFrequency);
}

// 0x501960
void CAEVehicleAudioEntity::ProcessDummyJet(tVehicleParams& vp) {
    auto* const plane = vp.Vehicle->AsPlane();
    ProcessGenericJet(
        plane->vehicleFlags.bEngineOn || plane->m_autoPilot.m_vehicleRecordingId >= 0,
        vp,
        (std::clamp(CalculatePropSpeed(plane, plane->m_GasPedal, plane->m_BrakePedal), 0.2f, 1.f) - 0.2f) * 1.25f,
        plane->m_GasPedal / 255.f,
        plane->m_BrakePedal / 255.f,
        0.f,
        1.f
    );
}

// 0x501650
void CAEVehicleAudioEntity::ProcessPlayerJet(tVehicleParams& vp) {
    auto* const plane = vp.Vehicle->AsPlane();
    const auto isRecordingVehicle = plane->m_autoPilot.m_vehicleRecordingId >= 0;

    if (isRecordingVehicle) {
        vp.ThisAccel = 0;
        vp.ThisBrake = 0;
    } else {
        GetAccelAndBrake(vp);
    }

    // 0x5016A1
    const auto engineSpeed = std::max((std::clamp(CalculatePropSpeed(plane, vp.ThisAccel, vp.ThisBrake), 0.2f, 1.f) - 0.2f) * 1.25f, 0.2f);

    // 0x501746
    float stalledVolume, stalledFreq;
    ProcessPropOrJetStall(plane, stalledVolume, stalledFreq);

    // 0x501946
    ProcessGenericJet(
        plane->vehicleFlags.bEngineOn,
        vp,
        engineSpeed,
        vp.ThisAccel,
        vp.ThisBrake,
        stalledVolume,
        stalledFreq
    );
}

#pragma endregion

#pragma region Hovercraft (Vortex)
// 0x500F50
void CAEVehicleAudioEntity::ProcessDummyHovercraft(tVehicleParams& params) {
    plugin::CallMethod<0x500F50, CAEVehicleAudioEntity*, tVehicleParams&>(this, params);
}

// Android
void CAEVehicleAudioEntity::ProcessPlayerHovercraft(tVehicleParams& params) {
    // NOP
}
#pragma endregion

#pragma region RC

// notsa
auto GetDummyRCAcceleration(CAEVehicleAudioEntity::tVehicleParams& vp) {
    return vp.ThisBrake > 0
        ? -0.05f
        : vp.ThisAccel > 0
            ? 0.1f
            : 0.f;
}

#pragma region RC Plane

// 0x4FA7C0
void CAEVehicleAudioEntity::ProcessDummyRCPlane(tVehicleParams& vp) {
    const auto* const cfg = &s_Config.DummyRCPlane;
    const auto* const plane = vp.Vehicle->AsPlane();

    if (!EnsureHasDummySlot() || !EnsureSoundBankIsLoaded(true)) {
        return;
    }

    if (m_IsWreckedVehicle) { // 0x4FA877
        CancelAllVehicleEngineSounds();
        return;
    }

    // 0x4FA8A2 - Propeller speed factor
    const auto sf = GetDummyRCRotorSpeedFactor(plane->m_fPropSpeed / 0.34f);

    // 0x4FA8B9 - Calculate accel/brake
    UpdateDummyRCAcAndBrake(vp);

    // 0x4FA9EE - Calculate and update rotor frequency
    UpdateRotorFreq(CalculatePlanePropFreq(vp, GetDummyRCAcceleration(vp)), cfg->RotorFreqStepUp, cfg->RotorFreqStepDown);

    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_FRONT,
        m_DummySlot,
        0,
        cfg->RotorVolBase + CAEAudioUtility::AudioLog10(sf) * 20.f,
        m_CurrentRotorFrequency
    );
}

// Android
void CAEVehicleAudioEntity::ProcessPlayerRCPlane(tVehicleParams& params) {
    // NOP
}
#pragma endregion

#pragma region RC Heli

// 0x4FAA80
void CAEVehicleAudioEntity::ProcessDummyRCHeli(tVehicleParams& vp) {
    const auto* const cfg = &s_Config.DummyRCHeli;
    const auto* const heli = vp.Vehicle->AsHeli();

    if (!EnsureHasDummySlot() || !EnsureSoundBankIsLoaded(true)) {
        return;
    }

    if (m_IsWreckedVehicle) { // 0x4FAB37
        CancelAllVehicleEngineSounds();
        return;
    }

    // 0x4FAB79 - Calculate accel/brake
    UpdateDummyRCAcAndBrake(vp);

    // 0x4FABA0 - Calculate speed factor
    const auto sf = GetDummyRCRotorSpeedFactor(GetHeliRotorSpeed(heli));

    // 0x4FABFF - Update rotor frequency
    {
        const auto rotorFrq = cfg->FrqBase
            + sf * cfg->FrqSpeedFactor
            + std::min(1.f, (1.f - heli->GetUp().Dot(CVector::ZAxisVector())) * 0.5f / cfg->TiltDownStep) * cfg->FrqTiltDownFactor
            + GetDummyRCAcceleration(vp);
        UpdateRotorFreq(rotorFrq, cfg->RotorFreqStepUp, cfg->RotorFreqStepDown);
    }

    // 0x4FAD2A - Play sound
    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_FRONT,
        m_DummySlot,
        0,
        cfg->VolBase + CAEAudioUtility::AudioLog10(sf) * 20.f,
        m_CurrentRotorFrequency
    );
}

// Android
void CAEVehicleAudioEntity::ProcessPlayerRCHeli(tVehicleParams& params) {
    // NOP
}
#pragma endregion
#pragma endregion
#pragma endregion

#pragma region Train

// notsa
auto GetTrainSpeed(const CTrain* train) {
    auto trspeed = train->trainFlags.bNotOnARailRoad
        ? 0.f
        : train->m_fTrainSpeed;
    if (train->trainFlags.bClockwiseDirection) {
        trspeed = -trspeed;
    }
    return trspeed;
}

// notsa
auto GetTrainSpeedFactor(const CTrain* train, float trspeed) {
    return train->trainFlags.bNotOnARailRoad
        ? 0.f
        : std::clamp(std::abs(trspeed), 0.f, 1.f);
}

// notsa
auto GetTrainPlayerAcFactor(const CTrain* train, float trspeed) {
    const auto* const p = CPad::GetPad();
    const auto ac = std::abs(trspeed) >= 0.001f
        ? trspeed < 0.f
            ? -p->GetBrake()
            : +p->GetAccelerate()
        : p->GetAccelerate() - p->GetBrake();
    return (float)(ac) / 255.f;
}

// 0x500710
void CAEVehicleAudioEntity::ProcessDummyTrainEngine(tVehicleParams& vp) {
    const auto* const train = vp.Vehicle->AsTrain();
    const auto isTram = m_DummyEngineBank == SND_BANK_GENRL_TRAM_D;
    const auto* const cfg = isTram
        ? &s_Config.Train.Tram.EngineSound
        : &s_Config.Train.Generic.EngineSound;

    if (!EnsureHasDummySlot() || !EnsureSoundBankIsLoaded(true, false)) {
        return;
    }

    if (!train->trainFlags.bIsFrontCarriage) {
        StopGenericEngineSound(AE_SOUND_TRAIN_ENGINE);
        return;
    }

    float frq{}, vol{};

    // 0x500876 - Calculate frequency for tilt
    frq = std::clamp(train->GetForward().Dot(CVector::ZAxisVector()), -0.3f, 0.3f) * cfg->FrqSlopeFactor;

    // 0x5008B2 - Calculate linear speed of train to use
    const auto trspeed = GetTrainSpeed(train);

    // 0x5008B2 - Calculate speed factor
    const auto sf = GetTrainSpeedFactor(train, trspeed);

    // 0x5009A9
    if (isTram) {
        vol += CAEAudioUtility::AudioLog10(sf) * 20.f;
    }

    // 0x5009D0
    if (train->GetStatus() == STATUS_PLAYER) {
        vol += GetTrainPlayerAcFactor(train, trspeed) * cfg->VolPlayerAcFactor;
    }

    // 0x500A98
    UpdateTrainSound(
        AE_SOUND_TRAIN_ENGINE,
        m_DummySlot,
        0,
        lerp(cfg->FrqMin, cfg->FrqMax, sf) * (1.f + frq),
        vol
    );
}

// 0x500AB0
void CAEVehicleAudioEntity::ProcessPlayerTrainBrakes(tVehicleParams& vp) {
    const auto* const train = vp.Vehicle->AsTrain();
    const auto* const cfg = m_DummyEngineBank == SND_BANK_GENRL_TRAM_D
        ? &s_Config.Train.Tram.EngineSound
        : &s_Config.Train.Generic.EngineSound;

    const auto trspeed = GetTrainSpeed(train);
    const auto sf      = GetTrainSpeedFactor(train, trspeed);

    const auto vol = GetTrainPlayerAcFactor(train, trspeed) * CGeneral::GetPiecewiseLinear({
        { 0.0f, 0.f },
        { 0.001f, 0.f },
        { 0.1f, 1.f },
        { 1.f, 1.f }
    }, sf);
    if (vol >= -100.f && !train->trainFlags.bNotOnARailRoad && train->trainFlags.bIsFrontCarriage) {
        PlayTrainBrakeSound(
            0,
            lerp(cfg->FrqMin, cfg->FrqMax, sf),
            cfg->VolBase + CAEAudioUtility::AudioLog10(vol) * 20.f
        );
    } else {
        PlayTrainBrakeSound(-1, 1.f, -100.f); // Stop sound
    }
}

// Android
void CAEVehicleAudioEntity::ProcessPlayerTrainEngine(tVehicleParams& params) {
    // NOP
}

// 0x4FA3F0
constexpr float TrainCarriagesNoise[]{ 1.08f, 0.96f, 1.04f, 0.92f, 1.f };
void CAEVehicleAudioEntity::ProcessTrainTrackSound(tVehicleParams& vp) {
    const auto* const train = vp.Vehicle->AsTrain();

    if (!EnsureHasDummySlot() || !EnsureSoundBankIsLoaded(true, false)) {
        return;
    }

    const auto sf = std::abs(train->m_fTrainSpeed); // Speed factor
    if (train->trainFlags.bNotOnARailRoad || sf < 0.00001f) {
        UpdateTrainSound(AE_SOUND_TRAIN_TRACK, m_DummySlot, 2, 0.f, -100.f); // Stop sound
        return;
    }

    // Get cfg for this type
    const auto* const cfg = m_DummyEngineBank == SND_BANK_GENRL_TRAM_D
        ? &s_Config.Train.Tram.TrackSound
        : &s_Config.Train.Generic.TrackSound;

    // Count number of carriages
    size_t n{};
    for (auto it = train; it; it = it->m_pPrevCarriage) {
        n++;
    }

    // Update track sound now
    UpdateTrainSound(
        AE_SOUND_TRAIN_TRACK,
        m_DummySlot,
        SND_GENRL_TRAIN_D_TRAIN_TRACK,
        lerp(cfg->FrqMin, cfg->FrqMax, sf) * TrainCarriagesNoise[n % std::size(TrainCarriagesNoise)],
        cfg->VolBase + CAEAudioUtility::AudioLog10(sf) * 20.f
    ); 
}

// 0x4FA1C0
void CAEVehicleAudioEntity::UpdateTrainSound(eTrainEngineSoundType st, eSoundBankSlot slot, eSoundID sfx, float speed, float volume) {
    volume += m_EventVolume;

    if (m_DummySlot == SND_BANK_SLOT_NONE) {
        return;
    }

    if (UpdateVehicleEngineSound(st, speed, volume)) {
        return;
    }

    const auto PlaySound = [&](float rollOff, bool checkIsBankLoaded) {
        if (checkIsBankLoaded && !AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
            return;
        }
        m_EngineSounds[st].Sound = AESoundManager.PlaySound({
            .BankSlotID      = slot,
            .SoundID       = sfx,
            .AudioEntity   = this,
            .Pos           = m_Entity->GetPosition(),
            .Volume        = volume,
            .RollOffFactor = rollOff,
            .Speed         = speed,
            .Flags         = SOUND_REQUEST_UPDATES
            });
        };
    const auto* const cfg = m_DummyEngineBank == SND_BANK_GENRL_TRAM_D
        ? &s_Config.Train.Tram
        : &s_Config.Train.Generic;
    switch (st) {
    case AE_SOUND_TRAIN_ENGINE:  PlaySound(cfg->EngineSoundRollOff, true);   break; // 0x4FA31F
    case AE_SOUND_TRAIN_TRACK:   PlaySound(cfg->TrackSoundRollOff, true);    break; // 0x4FA280
    case AE_SOUND_TRAIN_DISTANT: PlaySound(cfg->DistantSoundRollOff, false); break; // 0x4FA20B 
    default:
        NOTSA_UNREACHABLE();
    }
}
#pragma endregion

#pragma region Boat
// 0x5003A0
void CAEVehicleAudioEntity::ProcessBoatEngine(tVehicleParams& vp) {
    const auto* const boat = vp.Vehicle->AsBoat();
    const auto* const cfg  = &s_Config.Boat;


    if (!EnsureHasDummySlot()) {
        return;
    }
    if (!EnsureSoundBankIsLoaded(true)) {
        return;
    }

    // 0x500451 - Engine off/got wrecked?
    if (!boat->vehicleFlags.bEngineOn || m_IsWreckedVehicle) {
        StopGenericEngineSound(AE_SOUND_BOAT_ENGINE);
        StopGenericEngineSound(AE_SOUND_BOAT_DISTANT);
        return;
    }

    if (s_pPlayerDriver && m_IsPlayerDriver) {
        GetAccelAndBrake(vp);
    }

    // Engine speed factor
    const auto es = std::clamp(boat->m_fPropSpeed * 3.f, 0.f, 1.f);

    // 0x5004C7 - Calculate speed
    {
        const auto roll  = std::min(1.f, 1.f - boat->GetUp().Dot(CVector::ZAxisVector()));
        const auto speed = cfg->FrqEngineBase
            + roll * cfg->FrqEngineRollFactor
            + es * (boat->m_nBoatFlags.bOnWater ? cfg->FrqEngineOnWaterFactor : cfg->FrqEngineInAirFactor);
        m_CurrentDummyEngineFrequency = m_CurrentDummyEngineFrequency >= 0.f
            ? notsa::step_to(m_CurrentDummyEngineFrequency, speed, 0.02f, notsa::bugfixes::GenericFrameRate)
            : speed;
    }

    // 0x5005E0 - Calculate volume
    {
        const auto volume = cfg->VolBase
            + es * cfg->VolEngineSpeedFactor;
        m_CurrentDummyEngineVolume = m_CurrentDummyEngineVolume >= 0.f
            ? notsa::step_to(m_CurrentDummyEngineVolume, volume, 3.f, notsa::bugfixes::GenericFrameRate)
            : volume;
    }

    // 0x500632 - Update sound
    if (CWeather::UnderWaterness < 0.5f) {
        UpdateBoatSound(AE_SOUND_BOAT_ENGINE, m_DummySlot, 0, m_CurrentDummyEngineFrequency, m_CurrentDummyEngineVolume);
        UpdateBoatSound(AE_SOUND_BOAT_DISTANT, SND_BANK_SLOT_VEHICLE_GEN, 28, 1.f, -100.f);
    } else {
        UpdateBoatSound(AE_SOUND_BOAT_ENGINE, m_DummySlot, 0, 1.f, -100.f);
        UpdateBoatSound(AE_SOUND_BOAT_DISTANT, SND_BANK_SLOT_VEHICLE_GEN, 28, m_CurrentDummyEngineFrequency, m_CurrentDummyEngineVolume + 6.f);
    }
}

// 0x4FA0C0
void CAEVehicleAudioEntity::ProcessBoatMovingOverWater(tVehicleParams& vp) {
    const auto* const cfg = &s_Config.Boat.MovingOverWaterSound;
    const auto* const boat = vp.Vehicle->AsBoat();

    const auto sf = std::min(0.75f, std::abs(vp.Speed)) / 0.75f;

    auto volume = boat->m_nBoatFlags.bOnWater && sf >= 0.00001f
        ? (m_AuSettings.IsSeaplane() ? cfg->VolBaseOfSeaplane : cfg->VolBase) + CAEAudioUtility::AudioLog10(sf) * 20.f
        : -100.f;
    auto speed = cfg->FrqBase + cfg->FrqSpeedFactor * sf;

    if (CWeather::UnderWaterness >= 0.5f) {
        volume += cfg->VolUnderwaterOffset;
        speed  *= cfg->FrqUnderwaterFactor;
    }

    UpdateBoatSound(AE_SOUND_BOAT_WATER_SKIM, SND_BANK_SLOT_COLLISIONS, 3, speed, volume);
}

// 0x4F9E90
void CAEVehicleAudioEntity::UpdateBoatSound(eBoatEngineSoundType st, eSoundBankSlot slot, eSoundID sfx, float speed, float volume) {
    const auto* const cfg = &s_Config.Boat;

    volume += m_EventVolume;

    if (m_AuSettings.VehicleAudioType != AE_AIRCRAFT_SEAPLANE && st != AE_SOUND_BOAT_WATER_SKIM && m_DummySlot == SND_BANK_SLOT_NONE) {
        return;
    }

    if (UpdateVehicleEngineSoundRawVolume(st, speed, volume)) { // Try updating existing sound
        return;
    }

    const auto DoPlaySound = [&](float rollOff) {
        m_EngineSounds[st].Sound = AESoundManager.PlaySound({
            .BankSlotID      = slot,
            .SoundID       = sfx,
            .AudioEntity   = this,
            .Pos           = m_Entity->GetPosition(),
            .Volume        = volume,
            .RollOffFactor = rollOff,
            .Speed         = speed,
            .Flags         = SOUND_REQUEST_UPDATES
        });
    };
    switch (st) {
    case AE_SOUND_BOAT_ENGINE: {
        if (AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
            DoPlaySound(cfg->EngineSoundRollOff);
        }
        break;
    }
    case AE_SOUND_BOAT_DISTANT: {
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
            DoPlaySound(cfg->DistantSoundRollOff);
        }
        break;
    }
    case AE_SOUND_BOAT_WATER_SKIM: {
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
            DoPlaySound(cfg->WaterSkimSoundRollOff);
        }
        break;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}
#pragma endregion

#pragma region Bicycle
// 0x4F9710
void CAEVehicleAudioEntity::PlayBicycleSound(eBicycleSoundType st, eSoundBankSlot slot, eSoundID sfx, float volume, float speed) {
    if (notsa::bugfixes::CAEVehicleAudioEntity_PlayBicycleSound_VolumeFix) { // Most likely they left this out, because, for example, vehicle sounds (`ProcessPlayerVehicleEngine`) use `UpdateVehicleEngineSound` that does this too
        volume += m_EventVolume;
    }
    if (m_DummySlot == SND_BANK_SLOT_NONE) {
        return;
    }

    const auto DoPlaySound = [&](eSoundID sfx, float rollOff, uint32 flags = 0, int16 playTime = 0) {
        m_EngineSounds[st].Sound = AESoundManager.PlaySound({
            .BankSlotID      = slot,
            .SoundID       = sfx,
            .AudioEntity   = this,
            .Pos           = m_Entity->GetPosition(),
            .Volume        = volume,
            .RollOffFactor = rollOff,
            .Speed         = speed,
            .Flags         = flags | SOUND_REQUEST_UPDATES,
            .PlayTime      = playTime,
        });
    };

    if (st != AE_SOUND_BICYCLE_CHAIN_CLANG) {
        if (UpdateVehicleEngineSoundRawVolume(st, speed, volume)) {
            return;
        }
    }

    switch (st) {
    case AE_SOUND_BICYCLE_CHAIN_CLANG: {
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
            return;
        }
        if (GetEngineSound(AE_SOUND_BICYCLE_CHAIN_CLANG)) {
            return;
        }
        static constexpr struct { // 0x8CC1D8
            eSoundID SfxID;
            int16    PlayPercentage;
        } RandomArray[] = {
            {5, 38},
            {7, 25},
            {8, 25},
            {9, 50}
        };
        const auto* const choice = &CGeneral::RandomChoice(RandomArray);
        DoPlaySound(choice->SfxID, 1.f, SOUND_START_PERCENTAGE, choice->PlayPercentage);
        break;
    }
    case AE_SOUND_BICYCLE_TYRE: {
        if (AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
            DoPlaySound(sfx, 2.5f);
        }
        break;
    }
    case AE_SOUND_BICYCLE_SPROCKET_1: {
        if (AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
            DoPlaySound(sfx, 1.5f);
        }
        break;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x4F60B0
float CAEVehicleAudioEntity::GetBaseVolumeForBicycleTyre(float ratio) const noexcept {
    return CGeneral::GetPiecewiseLinear({ // 0x8CC18C:
        { 0.00f, 0.00f },
        { 0.10f, 0.30f },
        { 0.20f, 0.45f },
        { 0.50f, 0.85f },
        { 1.00f, 1.00f },
        }, ratio);
}

// 0x4FFDC0
void CAEVehicleAudioEntity::ProcessDummyBicycle(tVehicleParams& params) {
    plugin::CallMethod<0x4FFDC0, CAEVehicleAudioEntity*, tVehicleParams&>(this, params);
}

// 0x500040
void CAEVehicleAudioEntity::ProcessPlayerBicycle(tVehicleParams& params) {
    plugin::CallMethod<0x500040, CAEVehicleAudioEntity*, tVehicleParams&>(this, params);
}
#pragma endregion

#pragma region RC Car
// 0x500DC0
void CAEVehicleAudioEntity::ProcessDummyRCCar(tVehicleParams& vp) {
    if (!EnsureHasDummySlot() || !EnsureSoundBankIsLoaded(true)) {
        return;
    }
    if (m_IsWreckedVehicle) {
        CancelAllVehicleEngineSounds();
        return;
    }
    const auto sf = std::clamp(vp.Speed / vp.Transmission->m_MaxVelocity, 0.f, 1.f);
    UpdateGenericVehicleSound(
        AE_SOUND_CAR_ID,
        m_DummySlot,
        m_DummyEngineBank,
        0,
        CGeneral::GetPiecewiseLinear({ // 0x8CC0A4
            { 0.00f, -100.f },
            { 0.05f, -24.0f },
            { 0.20f, 0.000f },
            { 1.00f, 0.000f },
            { 0.00f, 1.000f }
        }, sf),
        CGeneral::GetPiecewiseLinear({ // 0x8CBF8C
            { 0.00f, 0.70f },
            { 0.05f, 0.70f },
            { 0.20f, 0.90f },
            { 1.00f, 1.15f },
            { -12.f, -6.0f }
        }, sf),
        1.f
    );
}

// Android
void CAEVehicleAudioEntity::ProcessPlayerRCCar(tVehicleParams& params) {
    // NOP
}
#pragma endregion

#pragma region Special Vehicle (Golf Cart, Combine, etc)
// 0x501AB0
void CAEVehicleAudioEntity::ProcessSpecialVehicle(tVehicleParams& params) {
    switch (params.Vehicle->GetModelId()) {
    case MODEL_ARTICT1:
    case MODEL_ARTICT2:
    case MODEL_PETROTR:
    case MODEL_ARTICT3: {
        ProcessVehicleRoadNoise(params);
        ProcessRainOnVehicle(params);
        break;
    }
    case MODEL_RCBANDIT:
    case MODEL_RCTIGER: {
        ProcessDummyRCCar(params);
        break;
    }
    case MODEL_CADDY: {
        ProcessVehicleRoadNoise(params);
        ProcessVehicleSkidding(params);
        ProcessVehicleFlatTyre(params);
        ProcessVehicleSirenAlarmHorn(params);
        ProcessDummyGolfCart(params);
        ProcessRainOnVehicle(params);
        if (params.Vehicle->GetStatus() != STATUS_SIMPLE) {
            ProcessEngineDamage(params);
        }
        break;
    }
    case MODEL_RCBARON: {
        ProcessDummyRCPlane(params);
        break;
    }
    case MODEL_RCRAIDER:
    case MODEL_RCGOBLIN: {
        ProcessDummyRCHeli(params);
        break;
    }
    case MODEL_VORTEX: {
        ProcessDummyHovercraft(params);
        ProcessRainOnVehicle(params);
        ProcessVehicleSirenAlarmHorn(params);
        break;
    }
    }
}

// 0x500CE0
void CAEVehicleAudioEntity::ProcessPlayerCombine(tVehicleParams& vp) {
    const auto* const cfg     = &s_Config.Combine;
    const auto* const combine = vp.Vehicle;
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_HARVESTER_P, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
        return;
    }
    if (combine->vehicleFlags.bEngineOn) {
        UpdateGenericVehicleSound(
            AE_SOUND_MOVING_PARTS,
            SND_BANK_SLOT_PLAYER_ENGINE_P,
            SND_BANK_GENRL_HARVESTER_P,
            SND_GENRL_HARVESTER_P_ZZHARVEST_LOOP,
            1.f,
            CAEAudioUtility::AudioLog10(std::clamp(vp.Speed / cfg->VolSpeedFactor, 0.f, 1.f)) * 20.f + cfg->VolBase,
            2.f
        );
    } else {
        CancelVehicleEngineSound(AE_SOUND_MOVING_PARTS);
    }
}

// 0x501270
void CAEVehicleAudioEntity::ProcessDummyGolfCart(tVehicleParams& vp) {
    auto* const cfg = &s_Config.DummyEngine.GolfCart;

    if (!EnsureHasDummySlot()) {
        return;
    }
    if (!EnsureSoundBankIsLoaded(true)) {
        return;
    }
    if (!vp.Vehicle->vehicleFlags.bEngineOn || m_IsWreckedVehicle) {
        CancelAllVehicleEngineSounds();
        return;
    }
    const auto t = vp.Transmission && vp.Vehicle->GetStatus() != STATUS_ABANDONED
        ? std::clamp(std::abs(vp.Speed / vp.Transmission->m_MaxVelocity), 0.f, 1.f)
        : 0.f;
    if (t >= 0.05f) {
        const auto volume = CGeneral::GetPiecewiseLinear({
            { 0.00f, 0.00f },
            { 0.05f, 0.00f },
            { 0.10f, 0.50f },
            { 1.00f, 1.00f }
        }, t);
        UpdateGenericVehicleSound(
            AE_DUMMY_ID,
            m_DummySlot,
            m_DummyEngineBank,
            0,
            lerp(cfg->FrqMin, cfg->FrqMax, t),
            CAEAudioUtility::AudioLog10(volume) * 20.f + cfg->VolOffset,
            2.f
        );
    } else {
        StopGenericEngineSound(AE_DUMMY_ID);
    }
}
#pragma endregion

// 0x501480
void CAEVehicleAudioEntity::ProcessDummyVehicleEngine(tVehicleParams& vp) {
    if (!EnsureHasDummySlot() || !EnsureSoundBankIsLoaded(true)) {
        return;
    }
    if (vp.Vehicle->vehicleFlags.bEngineOn) {
        const auto ratio = vp.Vehicle->GetStatus() != STATUS_ABANDONED && vp.Transmission
            ? std::clamp(std::abs(vp.Speed / vp.Transmission->m_MaxVelocity), 0.f, 1.f)
            : 0.f;
        bool isIdling;
        switch (m_State) {
        case eAEState::CAR_OFF:
        case eAEState::DUMMY_ID:   isIdling = ratio < s_Config.DummyEngine.ID.Ratio;          break;
        case eAEState::NUM_STATES: NOTSA_UNREACHABLE(); // This one was originally handled as part of the above, but it doesn't make any sense....
        case eAEState::DUMMY_CRZ:  isIdling = ratio < s_Config.DummyEngine.Rev.Ratio;         break;
        default:                   ProcessDummyStateTransition(eAEState::CAR_OFF, ratio, vp); return;
        }
        ProcessDummyStateTransition(isIdling ? eAEState::DUMMY_ID : eAEState::DUMMY_CRZ, ratio, vp);
    } else {
        ProcessDummyStateTransition(eAEState::CAR_OFF, 0.f, vp);
    }
}

bool CAEVehicleAudioEntity::EnsureHasDummySlot() noexcept {
    if (m_DummySlot == -1) {
        m_DummySlot = RequestBankSlot(m_DummyEngineBank);
    }
    return m_DummySlot != -1;
}

// notsa
bool CAEVehicleAudioEntity::EnsureSoundBankIsLoaded(bool isDummy, bool turnOffIfNotLoaded) {
    const auto bank = isDummy ? m_DummyEngineBank : m_PlayerEngineBank;
    const auto slot = isDummy ? m_DummySlot.get() : SND_BANK_SLOT_PLAYER_ENGINE_P;
    if (AEAudioHardware.IsSoundBankLoaded(bank, slot)) {
        return true;
    }
    if (!DoesBankSlotContainThisBank(slot, bank)) {
        m_DummySlot = RequestBankSlot(bank);
        if (turnOffIfNotLoaded) {
            m_State = eAEState::CAR_OFF;
        }
    }
    return false;
}

// notsa
void CAEVehicleAudioEntity::StopNonEngineSounds() noexcept {
    PlaySkidSound(-1);

    GracefullyStopSound(m_SurfaceSound);
    m_SurfaceSoundType = -1;

    GracefullyStopSound(m_RoadNoiseSound);
    m_RoadNoiseSoundType = -1;

    GracefullyStopSound(m_FlatTireSound);
    m_FlatTireSoundType = -1;

    GracefullyStopSound(m_ReverseGearSound);
    m_ReverseGearSoundType = -1;

    StopAndForgetSound(m_HornSound);
    StopAndForgetSound(m_SirenSound);
    StopAndForgetSound(m_FastSirenSound);
}

// 0x501E10
void CAEVehicleAudioEntity::ProcessVehicle(CPhysical* physical) {
    auto* const vehicle        = physical->AsVehicle();
    const auto  isStatusSimple = vehicle->GetStatus() == STATUS_SIMPLE;

    tVehicleParams vp{};
    vp.Vehicle               = vehicle;
    vp.ModelIndexMinusOffset = physical->GetModelIndex() - MODEL_VEHICLE_FIRST;
    vp.BaseVehicleType       = vehicle->m_nVehicleType;
    vp.SpecificVehicleType   = vehicle->m_nVehicleSubType;
    vp.Transmission          = vehicle->m_pHandlingData
        ? &vehicle->m_pHandlingData->m_transmissionData
        : nullptr;
    vp.Speed                 = isStatusSimple
        ? vehicle->m_autoPilot.m_speed / 50.0f
        : DotProduct(physical->m_vecMoveSpeed, physical->m_matrix->GetForward());

    switch (m_AuSettings.VehicleAudioType) {
    case AE_CAR: {
        UpdateGasPedalAudio(vehicle, vehicle->m_nVehicleType);

        if (m_IsWreckedVehicle) {
            break;
        }

        ProcessVehicleRoadNoise(vp);

        if (m_IsPlayerDriver) {
            ProcessReverseGear(vp);
        }

        if (!isStatusSimple) {
            ProcessVehicleSkidding(vp);
            ProcessVehicleFlatTyre(vp);
        }

        ProcessVehicleSirenAlarmHorn(vp);

        if (m_IsPlayerDriver) {
            ProcessPlayerVehicleEngine(vp);
        } else {
            ProcessDummyVehicleEngine(vp);
        }

        if (!isStatusSimple) {
            ProcessEngineDamage(vp);
        }

        ProcessRainOnVehicle(vp);

        if (vp.Vehicle->IsAutomobile()) {
            ProcessNitro(vp);
        }

        ProcessMovingParts(vp);

        if (vp.Vehicle->GetModelIndex() == MODEL_COMBINE) {
            ProcessPlayerCombine(vp);
        }

        vehicle->AsAutomobile()->m_PrevSpeed = vp.Speed;
        break;
    }
    case AE_BIKE: {
        if (m_IsWreckedVehicle) {
            break;
        }

        UpdateGasPedalAudio(vehicle, vehicle->m_nVehicleType);
        ProcessVehicleRoadNoise(vp);

        if (!isStatusSimple) {
            ProcessVehicleSkidding(vp);
        }

        ProcessVehicleSirenAlarmHorn(vp);

        if (m_IsPlayerDriver) {
            ProcessPlayerVehicleEngine(vp);
        } else {
            ProcessDummyVehicleEngine(vp);
        }

        if (!isStatusSimple) {
            ProcessEngineDamage(vp);
            ProcessVehicleFlatTyre(vp);
        }

        ProcessRainOnVehicle(vp);

        vehicle->AsBike()->m_PrevSpeed = vp.Speed;
        break;
    }
    case AE_BMX: {
        if (m_IsWreckedVehicle) {
            break;
        }

        UpdateGasPedalAudio(vehicle, vehicle->m_nVehicleType);
        ProcessVehicleSkidding(vp);
        ProcessVehicleSirenAlarmHorn(vp);

        if (m_IsPlayerDriver) {
            ProcessPlayerBicycle(vp);
        } else {
            ProcessDummyBicycle(vp);
        }

        vehicle->AsBike()->m_PrevSpeed = vp.Speed;
        break;
    }
    case AE_BOAT: {
        ProcessBoatEngine(vp);
        ProcessRainOnVehicle(vp);
        ProcessBoatMovingOverWater(vp);
        break;
    }
    case AE_AIRCRAFT_HELICOPTER: {
        UpdateGasPedalAudio(vehicle, vehicle->m_nVehicleType);
        ProcessAircraft(vp);
        ProcessRainOnVehicle(vp);
        break;
    }
    case AE_AIRCRAFT_PLANE: {
        UpdateGasPedalAudio(vehicle, vehicle->m_nVehicleType);
        ProcessAircraft(vp);
        ProcessRainOnVehicle(vp);
        ProcessMovingParts(vp);
        break;
    }
    case AE_AIRCRAFT_SEAPLANE: {
        ProcessAircraft(vp);
        ProcessRainOnVehicle(vp);
        ProcessBoatMovingOverWater(vp);
        break;
    }
    case AE_ONE_GEAR:
        break;
    case AE_TRAIN: {
        ProcessTrainTrackSound(vp);
        ProcessDummyTrainEngine(vp);
        if (m_IsPlayerDriver) {
            ProcessPlayerTrainBrakes(vp);
        }
        break;
    }
    case AE_SPECIAL: {
        ProcessSpecialVehicle(vp);
        break;
    }
    case AE_NO_VEHICLE:
        break;
    default:
        NOTSA_UNREACHABLE();
    }
}

void CAEVehicleAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEVehicleAudioEntity, 0x862CEC, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x4F63E0);
    RH_ScopedInstall(Destructor, 0x6D0A10);

    // Static
    RH_ScopedInstall(StaticGetPlayerVehicleAudioSettingsForRadio, 0x4F4ED0);
    RH_ScopedInstall(StaticService, 0x4F4EC0);
    RH_ScopedInstall(StaticInitialise, 0x5B99F0);

    RH_ScopedVMTInstall(UpdateParameters, 0x4FB6C0);
    RH_ScopedInstall(Initialise, 0x4F7670);
    RH_ScopedInstall(Terminate, 0x4FB8C0);

    RH_ScopedInstall(GetFlyingMetalVolume, 0x4F6150);
    RH_ScopedInstall(GetVehicleAudioSettings, 0x4F5C10);

    // Heli
    RH_ScopedInstall(EnableHelicoptors, 0x4F4EF0);
    RH_ScopedInstall(DisableHelicoptors, 0x4F4EE0);
    RH_ScopedInstall(EnableHelicoptor, 0x4F5C00);
    RH_ScopedInstall(DisableHelicoptor, 0x4F5BF0);
    RH_ScopedInstall(CopHeli, 0x4F5C40);

    RH_ScopedInstall(GetVehicleTypeForAudio, 0x4F4F00);

    RH_ScopedInstall(GetVolumeForDummyIdle, 0x4F51F0);
    RH_ScopedInstall(GetFrequencyForDummyIdle, 0x4F5310);

    RH_ScopedInstall(GetVolumeForDummyRev, 0x4F53D0);
    RH_ScopedInstall(GetFrequencyForDummyRev, 0x4F54F0);

    RH_ScopedInstall(JustGotInVehicleAsDriver, 0x4F5700);
    RH_ScopedInstall(TurnOnRadioForVehicle, 0x4F5B20);
    RH_ScopedInstall(TurnOffRadioForVehicle, 0x4F5B60);
    RH_ScopedInstall(PlayerAboutToExitVehicleAsDriver, 0x4F5BA0);
    RH_ScopedInstall(GetFreqForIdle, 0x4F5C60);
    RH_ScopedInstall(GetAccelAndBrake, 0x4F5080);
    RH_ScopedInstall(UpdateGasPedalAudio, 0x4F5EB0);

    RH_ScopedInstall(JustGotOutOfVehicleAsDriver, 0x4FCF40);
    RH_ScopedInstall(JustWreckedVehicle, 0x4FD0B0);

    // Bank slots
    RH_ScopedInstall(RequestBankSlot, 0x4F4D10);
    RH_ScopedInstall(DemandBankSlot, 0x4F4E60);
    RH_ScopedInstall(DoesBankSlotContainThisBank, 0x4F4E30);
    RH_ScopedInstall(StoppedUsingBankSlot, 0x4F4DF0);

    // Acceleration
    RH_ScopedInstall(JustFinishedAccelerationLoop, 0x4F5E50);
    RH_ScopedInstall(IsAccInhibited, 0x4F4F70);
    RH_ScopedInstall(IsAccInhibitedBackwards, 0x4F4FC0);
    RH_ScopedInstall(IsAccInhibitedForLowSpeed, 0x4F4FF0);
    RH_ScopedInstall(IsAccInhibitedForTime, 0x4F5020);
    RH_ScopedInstall(InhibitAccForTime, 0x4F5030);

    // Crz
    RH_ScopedInstall(InhibitCrzForTime, 0x4F5060);
    RH_ScopedInstall(IsCrzInhibitedForTime, 0x4F5050);

    // Skidding
    RH_ScopedInstall(GetVehicleDriveWheelSkidValue, 0x4F5F30);
    RH_ScopedInstall(GetVehicleNonDriveWheelSkidValue, 0x4F6000);
    RH_ScopedInstall(PlaySkidSound, 0x4F8360);
    RH_ScopedInstall(ProcessVehicleSkidding, 0x4F8F10);

    // Horn
    RH_ScopedOverloadedInstall(GetHornState, "", 0x4F61E0, void(CAEVehicleAudioEntity::*)(bool*, tVehicleParams&) const); // Done
    RH_ScopedInstall(GetSirenState, 0x4F62A0);
    RH_ScopedInstall(PlayHornOrSiren, 0x4F99D0);
    RH_ScopedInstall(ProcessVehicleSirenAlarmHorn, 0x5002C0);

    RH_ScopedInstall(UpdateGenericVehicleSound, 0x4FAD40);

    // Engine Sound
    RH_ScopedInstall(StartVehicleEngineSound, 0x4F7F20);
    RH_ScopedInstall(CancelVehicleEngineSound, 0x4F55C0);
    RH_ScopedInstall(RequestNewPlayerCarEngineSound, 0x4F7A50);
    RH_ScopedInstall(GetFreqForPlayerEngineSound, 0x4F8070);
    RH_ScopedInstall(GetVolForPlayerEngineSound, 0x4F5D00);
    //RH_ScopedInstall(UpdateVehicleEngineSound, 0x4F56D0); // Don't hook because original function doesn't return a bool, ours does
    RH_ScopedInstall(StopGenericEngineSound, 0x4F6320);

    // Car + bike
    RH_ScopedInstall(PlayFlatTyreSound, 0x4F8650);
    RH_ScopedInstall(PlayReverseSound, 0x4F87D0);
    RH_ScopedInstall(PlayRoadNoiseSound, 0x4F84D0);

    // Boat
    RH_ScopedInstall(ProcessBoatMovingOverWater, 0x4FA0C0);
    RH_ScopedInstall(ProcessBoatEngine, 0x5003A0);
    RH_ScopedInstall(UpdateBoatSound, 0x4F9E90);

    // Train
    RH_ScopedInstall(UpdateTrainSound, 0x4FA1C0);
    RH_ScopedInstall(ProcessTrainTrackSound, 0x4FA3F0);
    RH_ScopedInstall(ProcessDummyTrainEngine, 0x500710);
    RH_ScopedInstall(ProcessPlayerTrainBrakes, 0x500AB0);
    RH_ScopedInstall(PlayTrainBrakeSound, 0x4FA630);

    // Aircrafts
    RH_ScopedInstall(PlayAircraftSound, 0x4F93C0);
    RH_ScopedInstall(GetAircraftNearPosition, 0x4F96A0);
    RH_ScopedInstall(ProcessAircraft, 0x501C50);

    // Other..
    RH_ScopedInstall(ProcessVehicleFlatTyre, 0x4F8940);
    RH_ScopedInstall(ProcessVehicleRoadNoise, 0x4F8B00);
    RH_ScopedInstall(ProcessReverseGear, 0x4F8DF0);
    RH_ScopedInstall(ProcessRainOnVehicle, 0x4F92C0);
    RH_ScopedInstall(ProcessDummyRCPlane, 0x4FA7C0);
    RH_ScopedInstall(ProcessDummyRCHeli, 0x4FAA80);
    RH_ScopedInstall(ProcessEngineDamage, 0x4FAE20);
    RH_ScopedInstall(ProcessNitro, 0x4FB070); 
    RH_ScopedInstall(ProcessMovingParts, 0x4FB260);
    RH_ScopedInstall(ProcessDummyStateTransition, 0x4FCA10);

    RH_ScopedInstall(ProcessPlayerProp, 0x4FD290);
    RH_ScopedInstall(ProcessDummyProp, 0x4FD8F0);
    RH_ScopedInstall(ProcessAIProp, 0x4FDFD0);

    RH_ScopedInstall(ProcessPlayerHeli, 0x4FE420);
    RH_ScopedInstall(ProcessAIHeli, 0x4FEE20);
    RH_ScopedInstall(ProcessDummyHeli, 0x4FE940);

    RH_ScopedInstall(ProcessPlayerSeaPlane, 0x4FF320);
    RH_ScopedInstall(ProcessDummySeaPlane, 0x4FF7C0);

    // Jet
    RH_ScopedInstall(ProcessPlayerJet, 0x501650);
    RH_ScopedInstall(ProcessDummyJet, 0x501960);
    RH_ScopedInstall(ProcessGenericJet, 0x4FF900, { .reversed = false });

    // Bicycle
    RH_ScopedInstall(PlayBicycleSound, 0x4F9710);
    RH_ScopedInstall(GetBaseVolumeForBicycleTyre, 0x4F60B0);
    RH_ScopedInstall(ProcessDummyBicycle, 0x4FFDC0, { .reversed = false });
    RH_ScopedInstall(ProcessPlayerBicycle, 0x500040, { .reversed = false });

    RH_ScopedInstall(ProcessPlayerCombine, 0x500CE0);
    RH_ScopedInstall(ProcessDummyRCCar, 0x500DC0);
    RH_ScopedInstall(ProcessDummyHovercraft, 0x500F50, { .reversed = false });
    RH_ScopedInstall(ProcessDummyGolfCart, 0x501270);
    RH_ScopedInstall(ProcessDummyVehicleEngine, 0x501480);
    RH_ScopedInstall(ProcessSpecialVehicle, 0x501AB0);

    // Keep these hooked at all times:
    RH_ScopedInstall(ProcessPlayerVehicleEngine, 0x4FBB10, { .locked = true }); // Un-hooking this causes random crashes...
    RH_ScopedInstall(ProcessVehicle, 0x501E10, { .locked = true }); // -||-
    RH_ScopedInstall(Service, 0x502280, { .locked = true }); // -||-

    RH_ScopedOverloadedInstall(AddAudioEvent, "0", 0x4F6420, void(CAEVehicleAudioEntity::*)(eAudioEvents, float));
    RH_ScopedOverloadedInstall(AddAudioEvent, "1", 0x4F7580, void(CAEVehicleAudioEntity::*)(eAudioEvents, CPhysical*));
}
