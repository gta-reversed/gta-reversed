#include "StdInc.h"

#include "AEScriptAudioEntity.h"
#include "AESoundManager.h"
#include "AEAudioUtility.h"
#include "AEAudioHardware.h"
#include "AEAmbienceTrackManager.h"

// NOTSA: Handled by the game (0x4EE627), but has no name (`AE_SCRIPT_LAST` has this value)
constexpr auto AE_SCRIPT_UNNAMED_1190 = static_cast<eAudioEvents>(1'190);

// Tracks of the mini games and the school awards [NOTSA names]
enum eMissionAmbienceTrack : int32 {
    TRACK_BEE           = 141,
    TRACK_BIKE_AWARD    = 142,
    TRACK_AWARD         = 145,
    TRACK_DRIVING_AWARD = 149,
    TRACK_DUAL          = 150,
    TRACK_GOGO          = 153,
    TRACK_OTB           = 160,
    TRACK_PILOT_AWARD   = 161,
    TRACK_TEMPEST       = 172,
};

// 0x5B9B60
void CAEScriptAudioEntity::Initialise() {
    rng::for_each(m_WavLinks, &CAudioLink::Init);
}

// 0x4EC150
void CAEScriptAudioEntity::Reset() {
    for (auto i = 0; i < MISSION_AUDIO_COUNT; i++) {
        ClearMissionAudio(i);
    }
    m_MiniGameOneShotTriggered  = false;
    m_Crane                     = nullptr;
    m_InACrane                  = false;
    m_MissionSoundEnvelopePhase = ENVELOPE_FADE_OUT;
}

// 0x0
void CAEScriptAudioEntity::AddAudioEvent(int32) {
    /* Android NOP */
}

// 0x4EC100
void CAEScriptAudioEntity::AttachMissionAudioToPhysical(uint8 sampleId, CPhysical* physical) {
    m_WavLinks[sampleId].Entity   = physical;
    m_WavLinks[sampleId].Position = CAudioLink::UNSET_POSITION;
}

// NOTSA
static eSoundBankSlot GetMissionBankSlot(uint8 sampleId) {
    return static_cast<eSoundBankSlot>(SND_BANK_SLOT_MISSION1 + sampleId);
}

// 0x4EC040
void CAEScriptAudioEntity::ClearMissionAudio(uint8 sampleId) {
    if (sampleId >= std::size(m_WavLinks)) {
        return;
    }

    AESoundManager.CancelSoundsInBankSlot(GetMissionBankSlot(sampleId), true);

    auto& link    = m_WavLinks[sampleId];
    link.Entity   = nullptr;
    link.Sound    = nullptr;
    link.Position = CAudioLink::UNSET_POSITION;
}

// 0x4EBFE0
bool CAEScriptAudioEntity::IsMissionAudioSampleFinished(uint8 sampleId) {
    if (sampleId >= std::size(m_WavLinks)) {
        return true;
    }

    if (sampleId < BANK_LINK_A) {
        return !m_WavLinks[sampleId].Sound;
    }
    return !AESoundManager.AreSoundsPlayingInBankSlot(GetMissionBankSlot(sampleId));
}

// 0x4EBF60
bool CAEScriptAudioEntity::GetMissionAudioLoadingStatus(uint8 sampleId) {
    if (sampleId >= std::size(m_WavLinks)) {
        return true;
    }

    const auto& link = m_WavLinks[sampleId];
    if (link.BankID < 0) {
        return true;
    }

    if (link.SoundID < 0) {
        return AEAudioHardware.GetSoundBankLoadingStatus(
            link.BankID,
            GetMissionBankSlot(sampleId)
        );
    } else {
        return AEAudioHardware.GetSoundLoadingStatus(
            link.BankID,
            static_cast<eSoundID>(link.SoundID),
            GetMissionBankSlot(sampleId)
        );
    }
}

// 0x4EC020
int32 CAEScriptAudioEntity::GetMissionAudioEvent(uint8 sampleId) {
    return m_WavLinks[sampleId].AudioEvent;
}

// 0x4EC0C0
void CAEScriptAudioEntity::SetMissionAudioPosition(uint8 sampleId, const CVector& posn) {
    m_WavLinks[sampleId].Position = posn;
    m_WavLinks[sampleId].Entity   = nullptr;
}

// 0x4EC4D0
CVector* CAEScriptAudioEntity::GetMissionAudioPosition(uint8 sampleId) {
    auto& link = m_WavLinks[sampleId];
    if (link.Entity) {
        return &link.Entity->GetPosition();
    }
    return link.HasPosition()
        ? &link.Position
        : nullptr;
}

// 0x4EC6D0
void CAEScriptAudioEntity::PlayMissionBankSound(eAudioEvents eventId, const CVector& posn, CPhysical* physical, int16 sfxId, uint8 linkId, bool onlyIfNotPlaying, float volume, float maxDistance, float speed) {
    if (linkId < BANK_LINK_A || linkId >= std::size(m_WavLinks) || onlyIfNotPlaying && AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(eventId, this)) {
        return;
    }

    if (!AEAudioHardware.IsSoundBankLoaded(m_WavLinks[linkId].BankID, GetMissionBankSlot(linkId))) {
        return;
    }

    const auto Play = [&](CVector pos, bool isFrontend) {
        AESoundManager.PlaySound({
            .BankSlotID         = GetMissionBankSlot(linkId),
            .SoundID            = sfxId,
            .AudioEntity        = this,
            .Pos                = pos,
            .Volume             = CAEAudioEntity::GetDefaultVolume(eventId) + volume,
            .RollOffFactor      = maxDistance,
            .Speed              = speed,
            .Flags              = SOUND_IS_CANCELLABLE | SOUND_REQUEST_UPDATES | (isFrontend ? SOUND_FRONT_END : 0u) | (physical ? SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY : 0u),
            .FrequencyVariance  = 0.0f,
            .RegisterWithEntity = physical,
            .EventID            = eventId
        });
    };

    if (physical) {
        Play(physical->GetPosition(), false);
    } else if (posn.IsZero() || posn == CAudioLink::UNSET_POSITION) {
        Play({ 0.0f, 1.0f, 0.0f }, true);
    } else if (posn == CVector{ -1.0f, 0.0f, 0.0f } || posn == CVector{ 1.0f, 0.0f, 0.0f }) {
        Play(posn, true);
    } else {
        Play(posn, false);
    }
}

// 0x4EC550
void CAEScriptAudioEntity::PlayResidentSoundEvent(eSoundBankSlot slot, eSoundBank bank, eSoundID sfx, eAudioEvents event, const CVector& posn, CPhysical* physical, float vol, float speed, int16 playPosn, float maxDistance) {
    if (!AEAudioHardware.IsSoundBankLoaded(bank, slot)) {
        return;
    }

    const auto [pos, isFrontend] = [&] -> std::pair<CVector, bool> {
        if (physical) {
            return { physical->GetPosition(), false };
        } else if (posn == CAudioLink::UNSET_POSITION || posn.IsZero()) {
            return { CVector{0.0f, 1.0f, 0.0f}, true };
        } else {
            return { posn, false };
        }
    }();

    AESoundManager.PlaySound({
        .BankSlotID         = slot,
        .SoundID            = sfx,
        .AudioEntity        = this,
        .Pos                = pos,
        .Volume             = GetDefaultVolume(event) + vol,
        .RollOffFactor      = maxDistance,
        .Speed              = speed,
        .Doppler            = 1.0f,
        .FrameDelay         = 0,
        .Flags              = SOUND_START_PERCENTAGE | SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | (isFrontend ? SOUND_FRONT_END : 0u),
        .FrequencyVariance  = 0.0f,
        .PlayTime           = playPosn,
        .RegisterWithEntity = physical,
        .EventID            = event
    });
}

// 0x4EC270
void CAEScriptAudioEntity::PlayLoadedMissionAudio(uint8 sampleId) {
    if (sampleId >= std::size(m_WavLinks)) {
        return;
    }
    auto& link = m_WavLinks[sampleId];

    if (link.BankID < 0 || link.SoundID < 0 || !GetMissionAudioLoadingStatus(sampleId)) {
        return;
    }

    const auto [volume, duck] = [&link, sampleId] -> std::pair<float, bool> {
        if (link.AudioEvent == AE_SCRIPT_SLOT_USE_CUSTOM) { // 0x4EC2F1
            return { VOLUME_SILENCE, false };
        }

        const auto def = CAEAudioEntity::GetDefaultVolume((eAudioEvents)link.AudioEvent);
        return {
            (sampleId < BANK_LINK_A && def == -128.0f) ? 6.0f : def,
            sampleId < BANK_LINK_A
        };
    }();

    const auto* const pos = GetMissionAudioPosition(sampleId);

    // 0x4EC3EC
    link.Sound = AESoundManager.PlaySound({
        .BankSlotID    = GetMissionBankSlot(sampleId),
        .SoundID       = static_cast<eSoundID>(link.SoundID),
        .AudioEntity   = this,
        .Pos           = pos ? *pos : CVector{ 0.0f, 1.0f, 0.0f },
        .Volume        = volume,
        .RollOffFactor = 2.0f,
        .Flags         = SOUND_PLAY_PHYSICALLY | SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | (pos ? 0u : SOUND_FRONT_END) | (duck ? SOUND_IS_DUCKABLE | SOUND_IS_COMPRESSABLE | SOUND_SMOOTH_DUCKING : 0u),
    });
}

// 0x4EC190
void CAEScriptAudioEntity::PreloadMissionAudio(uint8 sampleId, eAudioEvents scriptId) {
    if (sampleId >= std::size(m_WavLinks) || !IsMissionAudioSampleFinished(sampleId)) {
        return;
    }

    auto& link = m_WavLinks[sampleId];
    if (!CAEAudioUtility::GetBankAndSoundFromScriptSlotAudioEvent(scriptId, link.BankID, link.SoundID, sampleId)) {
        return;
    }

    if (link.SoundID < 0) {
        AEAudioHardware.LoadSoundBank(link.BankID, GetMissionBankSlot(sampleId));
    } else {
        AEAudioHardware.LoadSound(link.BankID, static_cast<eSoundID>(link.SoundID), GetMissionBankSlot(sampleId));
    }
    link.AudioEvent = scriptId;
    link.Entity     = nullptr;
    link.Position   = CAudioLink::UNSET_POSITION;
}

// 0x4ECCF0
void CAEScriptAudioEntity::ProcessMissionAudioEvent(eAudioEvents eventId, const CVector& posn, CPhysical* physical, float volume, float speed) {
    const auto CancelSoundsOfEvent = [&](eAudioEvents startEvent) {
        if (physical) {
            AESoundManager.CancelSoundsOfThisEventPlayingForThisEntityAndPhysical(startEvent, this, physical);
        } else {
            AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(startEvent, this);
        }
    };
    const auto AddDoorAudioEvent = [&](eAudioEvents doorEvent, float doorSpeed) {
        CVector pos = physical ? physical->GetPosition() : posn;
        m_DoorAudioEntity.AddAudioEvent(doorEvent, pos, 0.0f, doorSpeed);
    };
    const auto PlayAmbienceTrack = [](eMissionAmbienceTrack track) {
        AEAmbienceTrackManager.PlaySpecialMissionAmbienceTrack(static_cast<eAudioEvents>(track)); // NOTE: It takes a track, not an event
    };

    // NOTE: The original only looks at the low 16 bits of the event (0x4ECCFB)
    switch (static_cast<eAudioEvents>(static_cast<uint16>(eventId))) {
    case AE_CRANE_WINCH_MOVE: // 0x4ECD8C
        if (!m_InACrane) {
            break;
        }
        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntityAndPhysical(eventId, this, physical)) {
            PlayResidentSoundEvent(SND_BANK_SLOT_PLAYER_ENGINE_P, SND_BANK_GENRL_CRANE_P, SND_GENRL_CRANE_P_LIFT_LOOP, eventId, posn, physical, volume, speed, 0, 2.5f);
        }
        m_CraneFrequencyScaling         = speed;
        m_CraneVolume                   = GetDefaultVolume(AE_CRANE_WINCH_MOVE) + volume;
        m_LastMiniGameLoopTriggerTimeMs = CTimer::GetTimeInMS();
        break;
    case AE_SCRIPT_DISABLE_HELI_AUDIO: // 0x4ECD7D
        CAEVehicleAudioEntity::DisableHelicoptors();
        break;
    case AE_SCRIPT_ENABLE_HELI_AUDIO: // 0x4ECE0C
        CAEVehicleAudioEntity::EnableHelicoptors();
        break;
    case AE_SCRIPT_CEILING_VENT_LAND: // 0x4ECE43
        PlayResidentSoundEvent(SND_BANK_SLOT_COLLISIONS, SND_BANK_GENRL_COLLISIONS, SND_GENRL_COLLISIONS_SIGN_B, eventId, posn, physical, 0.0f, 0.79f, 35);
        break;
    case AE_SCRIPT_CLAXON_START: // 0x4ECE2C
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_A);
        break;
    case AE_SCRIPT_CLAXON_STOP: // 0x4ECE73
        AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_SCRIPT_CLAXON_START, this);
        break;
    case AE_SCRIPT_BLAST_DOOR_SLIDE_START: // 0x4ED390
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_A);
        break;
    case AE_SCRIPT_BLAST_DOOR_SLIDE_STOP: // 0x4ECEDC
        AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_SCRIPT_BLAST_DOOR_SLIDE_START, this);
        break;
    case AE_SCRIPT_BONNET_DENT:
    case AE_SCRIPT_CAR_SMASH_CAR:
    case AE_SCRIPT_MAGNET_VEHICLE_COLLISION: // 0x4EE25A
        PlayResidentSoundEvent(SND_BANK_SLOT_COLLISIONS, SND_BANK_GENRL_COLLISIONS, CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_COLLISIONS_COLCAR01, SND_GENRL_COLLISIONS_COLCAR12), eventId, posn, physical, 0.0f);
        break;
    case AE_SCRIPT_BASKETBALL_BOUNCE: // 0x4ECEA3
        PlayMissionBankSound(eventId, posn, physical, CAEAudioUtility::GetRandomNumberInRange<eSoundID>(0, 2), BANK_LINK_B);
        break;
    case AE_SCRIPT_BASKETBALL_HIT_HOOP:
    case AE_SCRIPT_GOGO_EXPLOSION:
    case AE_SCRIPT_BANDIT_INSERT_COIN:
    case AE_SCRIPT_OTB_NO_CASH: // 0x4ED588
        PlayMissionBankSound(eventId, posn, physical, 3, BANK_LINK_B);
        break;
    case AE_SCRIPT_BASKETBALL_SCORE: // 0x4ECFA1
        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(eventId, this)) {
            PlayMissionBankSound(eventId, posn, physical, 4, BANK_LINK_B);
        }
        break;
    case AE_SCRIPT_POOL_HIT_WHITE: // 0x4ECF0C
        PlayMissionBankSound(eventId, posn, physical, 10, BANK_LINK_B);
        break;
    case AE_SCRIPT_POOL_HIT_CUSHION:
    case AE_SCRIPT_DUAL_SHOOT: // 0x4ED317
        PlayMissionBankSound(eventId, posn, physical, 8, BANK_LINK_B);
        break;
    case AE_SCRIPT_POOL_BALL_POT: // 0x4ECF3A
        PlayMissionBankSound(eventId, posn, physical, CAEAudioUtility::GetRandomNumberInRange<eSoundID>(3, 5), BANK_LINK_B);
        break;
    case AE_SCRIPT_POOL_CHALK_CUE:
    case AE_SCRIPT_TEMPEST_SELECT: // 0x4ECF73
        PlayMissionBankSound(eventId, posn, physical, 7, BANK_LINK_B);
        break;
    case AE_SCRIPT_CRANE_ENTER: // 0x4ECFCD
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_CRANE_P, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
            if (AESoundManager.AreSoundsPlayingInBankSlot(SND_BANK_SLOT_PLAYER_ENGINE_P)) {
                AESoundManager.CancelSoundsInBankSlot(SND_BANK_SLOT_PLAYER_ENGINE_P, false);
            }
            AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_CRANE_P, SND_BANK_SLOT_PLAYER_ENGINE_P);
        }
        m_Crane    = physical;
        m_InACrane = true;
        break;
    case AE_SCRIPT_CRANE_MOVE_START: // 0x4ED07B
        if (m_InACrane && !AESoundManager.AreSoundsOfThisEventPlayingForThisEntityAndPhysical(eventId, this, physical)) {
            PlayResidentSoundEvent(SND_BANK_SLOT_PLAYER_ENGINE_P, SND_BANK_GENRL_CRANE_P, SND_GENRL_CRANE_P_LIFT_LOOP, eventId, posn, physical, 0.0f, 1.0f, 0, 2.5f);
        }
        break;
    case AE_SCRIPT_CRANE_MOVE_STOP: // 0x4ED0CC
        if (!m_InACrane) {
            break;
        }
        CancelSoundsOfEvent(AE_SCRIPT_CRANE_MOVE_START);
        PlayResidentSoundEvent(SND_BANK_SLOT_PLAYER_ENGINE_P, SND_BANK_GENRL_CRANE_P, SND_GENRL_CRANE_P_CRANE_LIMIT, eventId, posn, physical, 0.0f, 1.0f, 0, 2.5f);
        break;
    case AE_SCRIPT_CRANE_EXIT: // 0x4ED024
        if (!m_InACrane) {
            break;
        }
        AESoundManager.CancelSoundsInBankSlot(SND_BANK_SLOT_PLAYER_ENGINE_P, true);
        PlayResidentSoundEvent(SND_BANK_SLOT_PLAYER_ENGINE_P, SND_BANK_GENRL_CRANE_P, SND_GENRL_CRANE_P_CRANE_OFF, eventId, posn, physical, 0.0f, 1.0f, 0, 2.5f);
        m_Crane    = nullptr;
        m_InACrane = false;
        break;
    case AE_SCRIPT_VIDEO_POKER_PAYOUT: // 0x4ED129
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_B);
        break;
    case AE_SCRIPT_VIDEO_POKER_BUTTON: // 0x4ED157
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_B);
        break;
    case AE_SCRIPT_WHEEL_OF_FORTUNE_CLACKER: // 0x4ED34B
        PlayResidentSoundEvent(SND_BANK_SLOT_COLLISIONS, SND_BANK_GENRL_COLLISIONS, SND_GENRL_COLLISIONS_COLBENCH1, eventId, posn, physical, 0.0f, 1.0f, 70);
        break;
    case AE_SCRIPT_KEYPAD_BEEP: // 0x4ED37B
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_SCRIPT_KEYPAD, SND_BANK_SLOT_MISSION3)) {
            PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_A);
        } else if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_SCRIPT_UNCLE_SAM, SND_BANK_SLOT_MISSION3)) { // 0x4ED3BE
            PlayMissionBankSound(eventId, posn, physical, 3, BANK_LINK_A);
        }
        break;
    case AE_SCRIPT_KEYPAD_PASS: // 0x4EE400
        PlayMissionBankSound(eventId, posn, physical, 2, BANK_LINK_A);
        break;
    case AE_SCRIPT_KEYPAD_FAIL:
    case AE_SCRIPT_DA_NANG_HEAVY_DOOR_OPEN:
    case AE_SCRIPT_CAT2_WOODEN_DOOR_BREACH: // 0x4ED405
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_A);
        break;
    case AE_SCRIPT_SHOOTING_RANGE_TARGET_SHATTER: // 0x4ED4AD
        PlayMissionBankSound(eventId, posn, physical, CAEAudioUtility::GetRandomNumberInRange<eSoundID>(2, 6), BANK_LINK_B);
        break;
    case AE_SCRIPT_SHOOTING_RANGE_TARGET_DROP: // 0x4ED433
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_B);
        break;
    case AE_SCRIPT_SHOOTING_RANGE_TARGET_MOVE_START: // 0x4ED447
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_B);
        break;
    case AE_SCRIPT_SHOOTING_RANGE_TARGET_MOVE_STOP: // 0x4ED475
        CancelSoundsOfEvent(AE_SCRIPT_SHOOTING_RANGE_TARGET_MOVE_START);
        break;
    case AE_SCRIPT_SHUTTER_DOOR_START:
    case AE_SCRIPT_GARAGE_DOOR_START: // 0x4EDBEA
        AddDoorAudioEvent(AE_GARAGE_DOOR_OPENING, 1.0f);
        break;
    case AE_SCRIPT_SHUTTER_DOOR_STOP:
    case AE_SCRIPT_GARAGE_DOOR_STOP: // 0x4EDC39
        AddDoorAudioEvent(AE_GARAGE_DOOR_OPENED, 1.0f);
        break;
    case AE_SCRIPT_PARACHUTE_OPEN: // 0x4ED32B
        PlayResidentSoundEvent(SND_BANK_SLOT_WEAPON_GEN, SND_BANK_GENRL_WEAPONS, 65, eventId, posn, physical, 0.0f);
        break;
    case AE_SCRIPT_DUAL_THRUST: // 0x4ED19B
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_B, true);
        m_LastMiniGameLoopTriggerTimeMs = CTimer::GetTimeInMS();
        break;
    case AE_SCRIPT_DUAL_EXPLOSION_SHORT:
    case AE_SCRIPT_OTB_LOSE: // 0x4ED1D5
        PlayMissionBankSound(eventId, posn, physical, 2, BANK_LINK_B, true);
        break;
    case AE_SCRIPT_DUAL_EXPLOSION_LONG: // 0x4ED203
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_B, true);
        break;
    case AE_SCRIPT_DUAL_MENU_SELECT:
    case AE_SCRIPT_BEE_SELECT: // 0x4ED231
        PlayMissionBankSound(eventId, posn, physical, 5, BANK_LINK_B);
        break;
    case AE_SCRIPT_DUAL_MENU_DESELECT:
    case AE_SCRIPT_BEE_PICKUP:
    case AE_SCRIPT_TEMPEST_HIGHLIGHT: // 0x4ED248
        PlayMissionBankSound(eventId, posn, physical, 4, BANK_LINK_B);
        break;
    case AE_SCRIPT_DUAL_GAME_OVER: // 0x4ED276
        PlayMissionBankSound(eventId, posn, physical, 3, BANK_LINK_B, true);
        break;
    case AE_SCRIPT_DUAL_PICKUP_LIGHT: // 0x4ED2A4
        PlayMissionBankSound(eventId, posn, physical, 7, BANK_LINK_B, true);
        break;
    case AE_SCRIPT_DUAL_PICKUP_DARK: // 0x4ED2BB
        PlayMissionBankSound(eventId, posn, physical, 6, BANK_LINK_B, true);
        break;
    case AE_SCRIPT_DUAL_TOUCH_DARK: // 0x4ED2E9
        PlayMissionBankSound(eventId, posn, physical, 9, BANK_LINK_B, true);
        break;
    case AE_SCRIPT_DUAL_TOUCH_LIGHT: // 0x4ED4E6
        PlayMissionBankSound(eventId, posn, physical, 10, BANK_LINK_B, true);
        break;
    case AE_SCRIPT_AMMUNATION_BUY_WEAPON:
    case AE_SCRIPT_SHOP_BUY: // 0x4ED939
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PURCHASE_WEAPON);
        break;
    case AE_SCRIPT_AMMUNATION_BUY_WEAPON_DENIED:
    case AE_SCRIPT_SHOP_BUY_DENIED: // 0x4ED956
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_CAR_NO_CASH);
        break;
    case AE_SCRIPT_RACE_321: // 0x4ED973
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RACE_321);
        break;
    case AE_SCRIPT_RACE_GO: // 0x4ED990
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RACE_GO);
        break;
    case AE_SCRIPT_PART_MISSION_COMPLETE:
    case AE_SCRIPT_CHECKPOINT_GREEN:
    case AE_SCRIPT_PROPERTY_PURCHASED: // 0x4EE5ED
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE);
        break;
    case AE_SCRIPT_GOGO_PLAYER_FIRE: // 0x4ED543
        PlayMissionBankSound(eventId, posn, physical, 5, BANK_LINK_B);
        break;
    case AE_SCRIPT_GOGO_ENEMY_FIRE: // 0x4ED571
        PlayMissionBankSound(eventId, posn, physical, 2, BANK_LINK_B);
        break;
    case AE_SCRIPT_GOGO_TRACK_START: // 0x4ED655
        PlayAmbienceTrack(TRACK_GOGO);
        break;
    case AE_SCRIPT_GOGO_TRACK_STOP:
    case AE_SCRIPT_DUAL_TRACK_STOP:
    case AE_SCRIPT_BEE_TRACK_STOP:
    case AE_SCRIPT_AWARD_TRACK_STOP:
    case AE_SCRIPT_OTB_TRACK_STOP:
    case AE_SCRIPT_TEMPEST_TRACK_STOP:
    case AE_SCRIPT_DRIVING_AWARD_TRACK_STOP:
    case AE_SCRIPT_BIKE_AWARD_TRACK_STOP:
    case AE_SCRIPT_PILOT_AWARD_TRACK_STOP: // 0x4EE565
        AEAmbienceTrackManager.StopSpecialMissionAmbienceTrack();
        break;
    case AE_SCRIPT_GOGO_SELECT:
    case AE_SCRIPT_TEMPEST_PLAYER_SHOOT: // 0x4ED5B6
        PlayMissionBankSound(eventId, posn, physical, 6, BANK_LINK_B);
        break;
    case AE_SCRIPT_GOGO_ACCEPT: // 0x4ED5E4
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_B);
        break;
    case AE_SCRIPT_GOGO_DECLINE:
    case AE_SCRIPT_BEE_ACCEPT: // 0x4ED5F9
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_B);
        break;
    case AE_SCRIPT_GOGO_GAME_OVER: // 0x4ED627
        PlayMissionBankSound(eventId, posn, physical, 4, BANK_LINK_B, true);
        break;
    case AE_SCRIPT_DUAL_TRACK_START: // 0x4ED52A
        PlayAmbienceTrack(TRACK_DUAL);
        break;
    case AE_SCRIPT_BEE_ZAP: // 0x4ED66E
        PlayMissionBankSound(eventId, posn, physical, 6, BANK_LINK_B);
        break;
    case AE_SCRIPT_BEE_DROP: // 0x4ED685
        PlayMissionBankSound(eventId, posn, physical, 2, BANK_LINK_B);
        break;
    case AE_SCRIPT_BEE_DECLINE: // 0x4ED6B3
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_B, false, 0.0f, 2.0f, 0.79f);
        break;
    case AE_SCRIPT_BEE_TRACK_START: // 0x4ED6F8
        PlayAmbienceTrack(TRACK_BEE);
        break;
    case AE_SCRIPT_BEE_GAME_OVER:
    case AE_SCRIPT_TEMPEST_GAME_OVER: // 0x4ED6E1
        PlayMissionBankSound(eventId, posn, physical, 3, BANK_LINK_B, true);
        break;
    case AE_SCRIPT_FREEZER_OPEN: // 0x4ED86A
        PlayMissionBankSound(eventId, posn, physical, 2, BANK_LINK_A, true, 0.0f, 3.0f);
        break;
    case AE_SCRIPT_FREEZER_CLOSE: // 0x4ED853
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_A, true, 0.0f, 3.0f);
        break;
    case AE_SCRIPT_MEAT_TRACK_START: // 0x4ED898
        PlayMissionBankSound(eventId, posn, physical, 3, BANK_LINK_A, true, 0.0f, 3.0f);
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_A, false, 0.0f, 3.0f);
        break;
    case AE_SCRIPT_MEAT_TRACK_STOP: // 0x4ED8E2
        AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_SCRIPT_MEAT_TRACK_START, this);
        PlayMissionBankSound(eventId, posn, physical, 4, BANK_LINK_A, true, 0.0f, 3.0f);
        break;
    case AE_SCRIPT_ROULETTE_ADD_CASH: // 0x4ED78F
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT);
        break;
    case AE_SCRIPT_ROULETTE_REMOVE_CASH: // 0x4ED7AC
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_BACK);
        break;
    case AE_SCRIPT_ROULETTE_NO_CASH: // 0x4ED7C9
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_ERROR);
        break;
    case AE_SCRIPT_ROULETTE_SPIN: // 0x4ED7E6
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_B);
        m_LastMiniGameLoopTriggerTimeMs = CTimer::GetTimeInMS();
        m_MiniGameOneShotTriggered      = false;
        break;
    case AE_SCRIPT_BANDIT_WHEEL_STOP: // 0x4ED728
        CancelSoundsOfEvent(AE_SCRIPT_BANDIT_WHEEL_START);
        PlayMissionBankSound(eventId, posn, physical, 2, BANK_LINK_B);
        break;
    case AE_SCRIPT_BANDIT_WHEEL_START: // 0x4ED711
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_B);
        break;
    case AE_SCRIPT_BANDIT_PAYOUT: // 0x4ED778
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_B);
        break;
    case AE_SCRIPT_BIKE_PACKER_CLUNK: // 0x4ED823
        PlayResidentSoundEvent(SND_BANK_SLOT_COLLISIONS, SND_BANK_GENRL_COLLISIONS, SND_GENRL_COLLISIONS_SOLID_METAL, eventId, posn, physical, 0.0f, 1.0f, 16);
        break;
    case AE_SCRIPT_AWARD_TRACK_START: // 0x4ED920
        PlayAmbienceTrack(TRACK_AWARD);
        break;
    case AE_SCRIPT_MESH_GATE_OPEN_START: // 0x4ED9AD
        if (m_MeshGateTriggerCount) {
            PlayResidentSoundEvent(SND_BANK_SLOT_COLLISIONS, SND_BANK_GENRL_COLLISIONS, SND_GENRL_COLLISIONS_WIRE_FENCE1, eventId, posn, physical, 0.0f, 1.0f, 63);
        } else {
            PlayResidentSoundEvent(SND_BANK_SLOT_COLLISIONS, SND_BANK_GENRL_COLLISIONS, SND_GENRL_COLLISIONS_WIRE_FENCE2, eventId, posn, physical, 0.0f, 1.0f, 62);
        }
        m_MeshGateTriggerCount = (m_MeshGateTriggerCount + 1) % 2;
        break;
    case AE_SCRIPT_MESH_GATE_OPEN_STOP: // 0x4EDD79
        if (m_MeshGateTriggerCount) {
            PlayResidentSoundEvent(SND_BANK_SLOT_COLLISIONS, SND_BANK_GENRL_COLLISIONS, SND_GENRL_COLLISIONS_WIRE_FENCE1, eventId, posn, physical, 0.0f, 0.94f, 33);
        } else {
            PlayResidentSoundEvent(SND_BANK_SLOT_COLLISIONS, SND_BANK_GENRL_COLLISIONS, SND_GENRL_COLLISIONS_WIRE_FENCE2, eventId, posn, physical, 0.0f, 1.0f, 25);
        }
        m_MeshGateTriggerCount = (m_MeshGateTriggerCount + 1) % 2;
        break;
    case AE_SCRIPT_OGLOC_DOORBELL: // 0x4EE2EE
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_A);
        break;
    case AE_SCRIPT_OGLOC_WINDOW_RATTLE_BANG: // 0x4EE31C
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_A);
        break;
    case AE_SCRIPT_STINGER_RELOAD:
    case AE_SCRIPT_MECHANIC_SLIDE_OUT: // 0x4EE361
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_A);
        break;
    case AE_SCRIPT_HEAVY_DOOR_START: // 0x4EDFDA
        PlayMissionBankSound(eventId, posn, physical, 2, BANK_LINK_A);
        break;
    case AE_SCRIPT_SHOOT_CONTROLS: // 0x4EDFF1
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_SCRIPT_BLACK_PROJECT, SND_BANK_SLOT_MISSION3)) {
            PlayMissionBankSound(eventId, posn, physical, 3, BANK_LINK_A);
        } else if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_SCRIPT_UNCLE_SAM, SND_BANK_SLOT_MISSION3)) { // 0x4EE00A
            PlayMissionBankSound(eventId, posn, physical, 4, BANK_LINK_A);
        }
        break;
    case AE_SCRIPT_CARGO_PLANE_DOOR_START: // 0x4EDF9C
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_A);
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_A);
        break;
    case AE_SCRIPT_CARGO_PLANE_DOOR_STOP: // 0x4EDFB3
        AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_SCRIPT_CARGO_PLANE_DOOR_START, this);
        PlayMissionBankSound(eventId, posn, physical, 2, BANK_LINK_A);
        break;
    case AE_SCRIPT_DA_NANG_CONTAINER_OPEN:
    case AE_SCRIPT_MECHANIC_ATTACH_CAR_BOMB:
    case AE_SCRIPT_CAT2_SECURITY_ALARM: // 0x4EE38F
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_A);
        break;
    case AE_SCRIPT_GYM_BIKE_START: // 0x4EE1BC
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_B, true, -18.0f);
        m_MissionSoundEnvelopePhase = ENVELOPE_FADE_IN;
        break;
    case AE_SCRIPT_GYM_BIKE_STOP:
    case AE_SCRIPT_GYM_RUNNING_MACHINE_STOP: // 0x4EE246
        m_MissionSoundEnvelopePhase = ENVELOPE_FADE_OUT;
        break;
    case AE_SCRIPT_GYM_BOXING_BELL:
    case AE_SCRIPT_TEMPEST_EXPLOSION: // 0x4EDB04
        PlayMissionBankSound(eventId, posn, physical, 2, BANK_LINK_B);
        break;
    case AE_SCRIPT_GYM_INCREASE_DIFFICULTY: // 0x4EE1DD
        PlayMissionBankSound(eventId, posn, physical, 3, BANK_LINK_B);
        break;
    case AE_SCRIPT_GYM_REST_WEIGHTS: // 0x4EE1F4
        PlayMissionBankSound(eventId, posn, physical, 7, BANK_LINK_B);
        break;
    case AE_SCRIPT_GYM_RUNNING_MACHINE_START: // 0x4EE20B
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_B, true, -18.0f);
        m_MissionSoundEnvelopePhase = ENVELOPE_FADE_IN;
        break;
    case AE_SCRIPT_OTB_BET_ZERO: // 0x4EDA27
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_B);
        break;
    case AE_SCRIPT_OTB_INCREASE_BET:
    case AE_SCRIPT_TEMPEST_ENEMY_SHOOT: // 0x4EDA55
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_B);
        break;
    case AE_SCRIPT_OTB_PLACE_BET: // 0x4EDA6C
        PlayMissionBankSound(eventId, posn, physical, 4, BANK_LINK_B);
        break;
    case AE_SCRIPT_OTB_WIN: // 0x4EDA9A
        PlayMissionBankSound(eventId, posn, physical, 5, BANK_LINK_B, true);
        break;
    case AE_SCRIPT_STINGER_FIRE: // 0x4EE333
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_A);
        break;
    case AE_SCRIPT_HEAVY_GATE_START: // 0x4EE3A6
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_A);
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_A);
        break;
    case AE_SCRIPT_HEAVY_GATE_STOP: // 0x4EE3F0
        AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_SCRIPT_HEAVY_GATE_START, this);
        PlayMissionBankSound(eventId, posn, physical, 2, BANK_LINK_A);
        break;
    case AE_SCRIPT_VERTICAL_BIRD_LIFT_START: // 0x4EDEAE
        PlayMissionBankSound(eventId, posn, physical, 2, BANK_LINK_A);
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_A);
        break;
    case AE_SCRIPT_VERTICAL_BIRD_LIFT_STOP: // 0x4EDEF8
        CancelSoundsOfEvent(AE_SCRIPT_VERTICAL_BIRD_LIFT_START);
        PlayMissionBankSound(eventId, posn, physical, 3, BANK_LINK_A);
        break;
    case AE_SCRIPT_PUNCH_PED: // 0x4EDDDD
        PlayResidentSoundEvent(SND_BANK_SLOT_WEAPON_GEN, SND_BANK_GENRL_WEAPONS, 58, eventId, posn, physical, 0.0f);
        PlayResidentSoundEvent(SND_BANK_SLOT_WEAPON_GEN, SND_BANK_GENRL_WEAPONS, 40, eventId, posn, physical, 0.0f);
        break;
    case AE_SCRIPT_AMMUNATION_GUN_COLLISION: // 0x4EDCFB
        PlayResidentSoundEvent(SND_BANK_SLOT_COLLISIONS, SND_BANK_GENRL_COLLISIONS, SND_GENRL_COLLISIONS_COLSOLIDWOOD, eventId, posn, physical, 0.0f);
        PlayResidentSoundEvent(SND_BANK_SLOT_COLLISIONS, SND_BANK_GENRL_COLLISIONS, SND_GENRL_COLLISIONS_GRENADE, eventId, posn, physical, 0.0f, 0.79f, 22);
        break;
    case AE_SCRIPT_CAMERA_SHOT: // 0x4EDC7D
        if (physical) {
            AudioEngine.ReportWeaponEvent(AE_WEAPON_FIRE, WEAPON_CAMERA, physical);
        } else { // 0x4EDCA1
            PlayResidentSoundEvent(SND_BANK_SLOT_WEAPON_GEN, SND_BANK_GENRL_WEAPONS, 45, eventId, posn, nullptr, 0.0f);
        }
        break;
    case AE_SCRIPT_BUY_CAR_MOD: // 0x4EE5B3
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_BUY_CAR_MOD);
        break;
    case AE_SCRIPT_BUY_CAR_RESPRAY: // 0x4EE5D0
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_CAR_RESPRAY);
        break;
    case AE_SCRIPT_BASEBALL_BAT_HIT_PED: // 0x4EDDFA
        PlayResidentSoundEvent(SND_BANK_SLOT_WEAPON_GEN, SND_BANK_GENRL_WEAPONS, 34, eventId, posn, physical, 5.0f);
        PlayResidentSoundEvent(SND_BANK_SLOT_WEAPON_GEN, SND_BANK_GENRL_WEAPONS, 40, eventId, posn, physical, 0.0f);
        break;
    case AE_SCRIPT_STAMP_PED: // 0x4EDE3E
        PlayResidentSoundEvent(SND_BANK_SLOT_WEAPON_GEN, SND_BANK_GENRL_WEAPONS, 82, eventId, posn, physical, -3.0f);
        break;
    case AE_SCRIPT_CHECKPOINT_AMBER: // 0x4EE579
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE, 0.0f, 1.12f);
        break;
    case AE_SCRIPT_CHECKPOINT_RED: // 0x4EE596
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE, 0.0f, 1.26f);
        break;
    case AE_SCRIPT_CAR_SMASH_GATE: // 0x4EE295
        PlayResidentSoundEvent(SND_BANK_SLOT_COLLISIONS, SND_BANK_GENRL_COLLISIONS, CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_COLLISIONS_COLCAR01, SND_GENRL_COLLISIONS_COLCAR12), eventId, posn, physical, 0.0f);
        PlayResidentSoundEvent(SND_BANK_SLOT_COLLISIONS, SND_BANK_GENRL_COLLISIONS, SND_GENRL_COLLISIONS_SOLID_METAL, eventId, posn, physical, 0.0f);
        break;
    case AE_SCRIPT_OTB_TRACK_START: // 0x4EDAB1
        PlayAmbienceTrack(TRACK_OTB);
        break;
    case AE_SCRIPT_PED_HIT_WATER_SPLASH: // 0x4EE47F
        if (physical) {
            AudioEngine.ReportWaterSplash(physical, -6.0f, false);
        } else { // 0x4EE4A3
            AudioEngine.ReportWaterSplash(posn, -6.0f);
        }
        break;
    case AE_SCRIPT_RESTAURANT_TRAY_COLLISION: // 0x4EDD49
        PlayResidentSoundEvent(SND_BANK_SLOT_COLLISIONS, SND_BANK_GENRL_COLLISIONS, SND_GENRL_COLLISIONS_COLBENCH1, eventId, posn, physical, 0.0f, 1.0f, 65);
        break;
    case AE_SCRIPT_SWEETS_HORN: // 0x4EE42E
        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(eventId, this)) {
            PlayResidentSoundEvent(SND_BANK_SLOT_HORN_AND_SIREN, SND_BANK_GENRL_HORN, SND_GENRL_HORN_HORNPICKUP, eventId, posn, physical, 0.0f);
            m_LastMiniGameLoopTriggerTimeMs = CTimer::GetTimeInMS();
        }
        break;
    case AE_SCRIPT_PICKUP_STANDARD: // 0x4EE60A
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_MONEY);
        break;
    case AE_SCRIPT_MINITANK_FIRE: // 0x4EDC55
        if (physical) {
            AudioEngine.ReportWeaponEvent(AE_WEAPON_FIRE, WEAPON_M4, physical);
        }
        break;
    case AE_SCRIPT_EXPLOSION: { // 0x4EDCBE
        CVector pos = physical ? physical->GetPosition() : posn;
        m_ExplosionAudioEntity.AddAudioEvent(AE_EXPLOSION, pos, 0.0f);
        break;
    }
    case AE_SCRIPT_VERTICAL_BIRD_ALARM_START: // 0x4EDF36
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_A);
        break;
    case AE_SCRIPT_VERTICAL_BIRD_ALARM_STOP: // 0x4EDF64
        CancelSoundsOfEvent(AE_SCRIPT_VERTICAL_BIRD_ALARM_START);
        break;
    case AE_SCRIPT_PED_COLLAPSE: // 0x4EE18C
        PlayResidentSoundEvent(SND_BANK_SLOT_FOOTSTEPS_GENERIC, SND_BANK_FEET_GENERIC, 0, eventId, posn, physical, 0.0f);
        break;
    case AE_SCRIPT_AIR_HORN: // 0x4EE03A
        PlayMissionBankSound(eventId, CVector{ -1.0f, 0.0f, 0.0f }, nullptr, 0, BANK_LINK_B);
        PlayMissionBankSound(eventId, CVector{ 1.0f, 0.0f, 0.0f }, nullptr, 1, BANK_LINK_B);
        break;
    case AE_SCRIPT_SHUTTER_DOOR_SLOW_START: // 0x4EE0E6
        AddDoorAudioEvent(AE_GARAGE_DOOR_OPENING, 0.79f);
        break;
    case AE_SCRIPT_SHUTTER_DOOR_SLOW_STOP: // 0x4EE145
        AddDoorAudioEvent(AE_GARAGE_DOOR_OPENED, 1.0f);
        break;
    case AE_SCRIPT_BEE_BUZZ: // 0x4EDACA
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_B, true);
        m_LastMiniGameLoopTriggerTimeMs = CTimer::GetTimeInMS();
        break;
    case AE_SCRIPT_RESTAURANT_CJ_EAT: // 0x4EE4D5
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_B);
        break;
    case AE_SCRIPT_RESTAURANT_CJ_PUKE: // 0x4EE4EC
        PlayMissionBankSound(eventId, posn, physical, 1, BANK_LINK_B);
        break;
    case AE_SCRIPT_TEMPEST_PICKUP1: // 0x4EDB32
        PlayMissionBankSound(eventId, posn, physical, 5, BANK_LINK_B, false, 0.0f, 2.0f, 0.67f);
        break;
    case AE_SCRIPT_TEMPEST_PICKUP2: // 0x4EDB3C
        PlayMissionBankSound(eventId, posn, physical, 5, BANK_LINK_B, false, 0.0f, 2.0f, 0.79f);
        break;
    case AE_SCRIPT_TEMPEST_PICKUP3: // 0x4EDB53
        PlayMissionBankSound(eventId, posn, physical, 5, BANK_LINK_B);
        break;
    case AE_SCRIPT_TEMPEST_WARP: // 0x4EDB81
        PlayMissionBankSound(eventId, posn, physical, 8, BANK_LINK_B, true, 0.0f, 2.0f, 0.38f);
        break;
    case AE_SCRIPT_TEMPEST_SHIELD_GLOW: // 0x4EDB98
        PlayMissionBankSound(eventId, posn, physical, 0, BANK_LINK_B, true);
        m_LastMiniGameLoopTriggerTimeMs = CTimer::GetTimeInMS();
        break;
    case AE_SCRIPT_TEMPEST_TRACK_START: // 0x4EDBD1
        PlayAmbienceTrack(TRACK_TEMPEST);
        break;
    case AE_SCRIPT_DRIVING_AWARD_TRACK_START: // 0x4EE51A
        PlayAmbienceTrack(TRACK_DRIVING_AWARD);
        break;
    case AE_SCRIPT_BIKE_AWARD_TRACK_START: // 0x4EE533
        PlayAmbienceTrack(TRACK_BIKE_AWARD);
        break;
    case AE_SCRIPT_PILOT_AWARD_TRACK_START: // 0x4EE54C
        PlayAmbienceTrack(TRACK_PILOT_AWARD);
        break;
    case AE_SCRIPT_PED_DEATH_CRUNCH: // 0x4EDE61
        if (CLocalisation::Blood() && physical && physical->GetIsTypePed()) {
            physical->AsPed()->GetAE().AddAudioEvent(AE_PED_CRUNCH, 0.0f, 1.0f, physical);
        }
        break;
    case AE_SCRIPT_UNNAMED_1190: // 0x4EE627
        PlayResidentSoundEvent(SND_BANK_SLOT_WEAPON_GEN, SND_BANK_GENRL_WEAPONS, CAEAudioUtility::GetRandomNumberInRange<eSoundID>(78, 80), eventId, posn, physical, 0.0f);
        break;
    case AE_SCRIPT_WAREHOUSE_DOOR_SLIDE_START:
    case AE_SCRIPT_WAREHOUSE_DOOR_SLIDE_STOP:
    case AE_SCRIPT_POOL_BREAK:
    case AE_SCRIPT_POOL_BALL_HIT_BALL:
    case AE_SCRIPT_CRANE_SMASH_PORTACABIN:
    case AE_SCRIPT_CONTAINER_COLLISION:
    case AE_SCRIPT_FREEFALL_START:
    case AE_SCRIPT_FREEFALL_STOP:
    case AE_SCRIPT_PARACHUTE_COLLAPSE:
    case AE_SCRIPT_OFFICE_FIRE_ALARM_START:
    case AE_SCRIPT_OFFICE_FIRE_ALARM_STOP:
    case AE_SCRIPT_OFFICE_FIRE_COUGHING_START:
    case AE_SCRIPT_OFFICE_FIRE_COUGHING_STOP:
    case AE_SCRIPT_BIKE_GANG_WHEEL_SPIN:
    case AE_SCRIPT_MOLOTOV:
    case AE_SCRIPT_HEAVY_DOOR_STOP:
    case AE_SCRIPT_DA_NANG_MUFFLED_REFUGEES:
    case AE_SCRIPT_PICKUP_CRATE:
    case AE_SCRIPT_ROULETTE_BALL_BOUNCING: // 0x4EE65B
        break;
    default:
        break;
    }
}

// 0x4EE960
void CAEScriptAudioEntity::ReportMissionAudioEvent(eAudioEvents eventId, CPhysical* physical, float volume, float speed) {
    ProcessMissionAudioEvent(eventId, CAudioLink::UNSET_POSITION, physical, volume, speed);
}

// 0x4EE940
void CAEScriptAudioEntity::ReportMissionAudioEvent(eAudioEvents eventId, const CVector& posn) {
    ProcessMissionAudioEvent(eventId, posn, nullptr);
}

// NOTSA - The part of `UpdateParameters` that looks after the looping sounds of the crane and the mini games
void CAEScriptAudioEntity::UpdateMiniGameSound(CAESound* sound) {
    constexpr auto VOLUME_STEP       = 0.1f; // NOTE: Per update, so framerate dependent

    const auto StopIfNotTriggeredFor = [&](uint32 ms) {
        if (CTimer::GetTimeInMS() > m_LastMiniGameLoopTriggerTimeMs + ms) {
            sound->StopSoundAndForget();
            m_LastMiniGameLoopTriggerTimeMs = 0;
        }
    };

    switch (sound->m_Event) {
    case AE_CRANE_WINCH_MOVE: // 0x4ECA37
        if (CTimer::GetTimeInMS() > m_LastMiniGameLoopTriggerTimeMs + 300) {
            sound->StopSoundAndForget();
            m_LastMiniGameLoopTriggerTimeMs = 0;
            // NOTE: The original passes the sound's physical entity, but `StopSoundAndForget` has just cleared it
            PlayResidentSoundEvent(SND_BANK_SLOT_PLAYER_ENGINE_P, SND_BANK_GENRL_CRANE_P, SND_GENRL_CRANE_P_CRANE_LIMIT, AE_SCRIPT_CRANE_MOVE_STOP, sound->m_CurrPos, nullptr, -12.0f, 1.0f, 0, 2.5f);
        } else { // 0x4ECA83
            sound->SetVolume(m_CraneVolume);
            sound->SetSpeed(m_CraneFrequencyScaling);
        }
        break;
    case AE_SCRIPT_DUAL_THRUST:
    case AE_SCRIPT_BEE_BUZZ:
    case AE_SCRIPT_TEMPEST_SHIELD_GLOW: // 0x4ECC7D
        StopIfNotTriggeredFor(300);
        break;
    case AE_SCRIPT_SWEETS_HORN: // 0x4ECC8D
        StopIfNotTriggeredFor(500);
        break;
    case AE_SCRIPT_ROULETTE_SPIN: // 0x4ECA9A
        if (CTimer::GetTimeInMS() > m_LastMiniGameLoopTriggerTimeMs + 4'500) {
            if (sound->GetVolume() > -40.0f) {
                sound->SetVolume(sound->GetVolume() - VOLUME_STEP);
            } else {
                sound->StopSoundAndForget();
                m_LastMiniGameLoopTriggerTimeMs = 0;
            }
            if (sound->GetSpeed() > 0.0f) {
                sound->SetSpeed(notsa::step_down_to(sound->GetSpeed(), 0.0f, 0.001f));
            }
        }
        if (CTimer::GetTimeInMS() > m_LastMiniGameLoopTriggerTimeMs + 4'800 && !m_MiniGameOneShotTriggered) { // 0x4ECB08
            PlayMissionBankSound(AE_SCRIPT_ROULETTE_BALL_BOUNCING, CAudioLink::UNSET_POSITION, nullptr, CAEAudioUtility::GetRandomNumberInRange<eSoundID>(1, 3), BANK_LINK_B);
            m_MiniGameOneShotTriggered = true;
        }
        break;
    case AE_SCRIPT_GYM_BIKE_START:
    case AE_SCRIPT_GYM_RUNNING_MACHINE_START: { // 0x4ECB66, 0x4ECBC4
        const auto maxVolume = GetDefaultVolume(static_cast<eAudioEvents>(sound->m_Event));
        if (m_MissionSoundEnvelopePhase == ENVELOPE_FADE_IN) {
            if (sound->GetVolume() < maxVolume) {
                sound->SetVolume(notsa::step_up_to(sound->GetVolume(), maxVolume, VOLUME_STEP));
            }
        } else if (m_MissionSoundEnvelopePhase == ENVELOPE_FADE_OUT) { // 0x4ECC19
            const auto minVolume = maxVolume - 18.0f;
            if (sound->GetVolume() > minVolume) {
                sound->SetVolume(notsa::step_down_to(sound->GetVolume(), minVolume, VOLUME_STEP));
            } else {
                sound->StopSoundAndForget();
            }
        }
        break;
    }
    }
}

// 0x4EC970
void CAEScriptAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (!sound) {
        return;
    }

    for (auto& link : m_WavLinks) {
        if (link.Sound != sound) { // 0x4EC9B0
            // NOTE: The original does this inside the loop, so it runs once for every link that doesn't play this sound (4 times for a mini game sound)
            UpdateMiniGameSound(sound);
            continue;
        }

        if (curPlayPos == -1) { // The sound has finished
            link.Sound = nullptr;
            return;
        }

        if (link.Entity) {
            sound->SetPosition(link.Entity->GetPosition());
        }
    }
}

// 0x4EC900
void CAEScriptAudioEntity::Service() {
    if (!m_Crane || AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_SCRIPT_CRANE_ENTER, this)) {
        return;
    }

    PlayResidentSoundEvent(
        SND_BANK_SLOT_PLAYER_ENGINE_P,
        SND_BANK_GENRL_CRANE_P,
        SND_GENRL_CRANE_P_CRANE_ENGINE,
        AE_SCRIPT_CRANE_ENTER,
        CAudioLink::UNSET_POSITION,
        m_Crane,
        0.0f,
        1.0f,
        0,
        2.5f
    );
}

void CAEScriptAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEScriptAudioEntity, 0x862E58, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedConstructorInstall(0x5074D0, "", {});
    RH_ScopedInstall(Initialise, 0x5B9B60);
    RH_ScopedInstall(Service, 0x4EC900);
    RH_ScopedInstall(Reset, 0x4EC150);
    RH_ScopedInstall(GetMissionAudioLoadingStatus, 0x4EBF60);
    RH_ScopedInstall(IsMissionAudioSampleFinished, 0x4EBFE0);
    RH_ScopedInstall(GetMissionAudioEvent, 0x4EC020);
    RH_ScopedInstall(ClearMissionAudio, 0x4EC040);
    RH_ScopedInstall(SetMissionAudioPosition, 0x4EC0C0);
    RH_ScopedInstall(AttachMissionAudioToPhysical, 0x4EC100);
    RH_ScopedInstall(PreloadMissionAudio, 0x4EC190);
    RH_ScopedInstall(PlayLoadedMissionAudio, 0x4EC270);
    RH_ScopedInstall(GetMissionAudioPosition, 0x4EC4D0);
    RH_ScopedInstall(PlayResidentSoundEvent, 0x4EC550);
    RH_ScopedInstall(PlayMissionBankSound, 0x4EC6D0);
    RH_ScopedInstall(ProcessMissionAudioEvent, 0x4ECCF0);
    RH_ScopedOverloadedInstall(ReportMissionAudioEvent, "1", 0x4EE960, void (CAEScriptAudioEntity::*)(eAudioEvents, CPhysical*, float, float));
    RH_ScopedOverloadedInstall(ReportMissionAudioEvent, "2", 0x4EE940, void (CAEScriptAudioEntity::*)(eAudioEvents, const CVector&));
    RH_ScopedVMTInstall(UpdateParameters, 0x4EC970);
}
