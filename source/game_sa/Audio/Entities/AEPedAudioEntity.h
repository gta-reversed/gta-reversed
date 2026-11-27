/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AEAudioEntity.h"
#include "AESound.h"
#include "AETwinLoopSoundEntity.h"
#include <Enums/eSurfaceType.h>

class CPed;

class NOTSA_EXPORT_VTABLE CAEPedAudioEntity : public CAEAudioEntity {
public:
    CAEPedAudioEntity();
    ~CAEPedAudioEntity() = default; // 0x5DE920

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;

    void Initialise(CPed* ped);
    static void StaticInitialise();
    void Terminate();

    void AddAudioEvent(eAudioEvents event, float volume = 0.0f, float speed = 1.0f, CPhysical* entity = nullptr, eSurfaceType surfaceId = SURFACE_DEFAULT, int32 hitEvent = 0, uint32 startDelayMs = 0);

    void TurnOnJetPack();
    void TurnOffJetPack();
    void StopJetPackSound();
    void UpdateJetPack(float thrustFwd, float thrustAngle);
    void PlayWindRush(float, float);

    void HandleFootstepEvent(eAudioEvents event, float volume, float speed, eSurfaceType surfaceId);
    void HandleSkateEvent(eAudioEvents event, float volume, float speed);
    void HandleLandingEvent(eAudioEvents event);
    void HandlePedSwing(eAudioEvents event, int32 hitEvent, uint32 startDelayMs);
    void HandlePedHit(eAudioEvents event, CPhysical* hitEntity, eSurfaceType hitSurface, float volume, uint32 startDelayMs);
    void HandlePedJacked(eAudioEvents event);
    void HandleSwimSplash(eAudioEvents event);
    void HandleSwimWake(eAudioEvents event);

    void PlayShirtFlap(float volume, float speed);
    void Service();

    static void InjectHooks();

private:
    void PlayFightSound(eSoundBankSlot slot, eSoundID soundID, eAudioEvents event, float volume, float speed, float speedVariance, uint32 startDelayMs);

    static float GetHitSurfaceVolume(eSurfaceType surface);

private:
    bool     m_CanAddEvent{};
    eSoundID m_SwimSplashSoundID{}; //!< Last played sound of the swimming bank, cycles through 0-4
    uint32   m_LastSwimWakeTriggerTimeMs{};

    float m_JetPackSound0Volume{};
    float m_JetPackSound1Volume{};
    float m_JetPackSound2Volume{};
    float m_JetPackSoundSpeedMult{};
    CPed* m_Ped{};

    bool      m_IsJetPackPlaying{};
    CAESound* m_JetPackSound0{};
    CAESound* m_JetPackSound1{};
    CAESound* m_JetPackSound2{};

    CAETwinLoopSoundEntity m_ShirtFlapSound{};
    CAESound*              m_UnkSound{};        //!< Only ever reset (`Initialise`, `UpdateParameters`)
    float                  m_UnkVolume{};       //!< Only ever set in `Initialise`
    float                  m_ShirtFlapVolume{}; //!< Current volume of the shirt flap (wind) sound, see `Service`
};

VALIDATE_SIZE(CAEPedAudioEntity, 0x15C);
