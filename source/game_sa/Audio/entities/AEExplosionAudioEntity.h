/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AEAudioEntity.h"

class NOTSA_EXPORT_VTABLE CAEExplosionAudioEntity : public CAEAudioEntity {
public:
    uint32 m_Speed;

public:
    static void InjectHooks();

    CAEExplosionAudioEntity();
    ~CAEExplosionAudioEntity() = default;

    static void StaticInitialise();

    void AddAudioEvent(eAudioEvents audioEvent, CVector& posn, float volume);
    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;
};

VALIDATE_SIZE(CAEExplosionAudioEntity, 0x80);
