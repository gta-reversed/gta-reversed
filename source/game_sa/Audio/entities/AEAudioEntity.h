/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AESound.h"

#include "eAudioEvents.h"

class CEntity;

class NOTSA_EXPORT_VTABLE CAEAudioEntity {
public:
    CEntity* m_Entity;
    CAESound m_tempSound;

private:
    static int8*& m_pAudioEventVolumes;

protected:
    CAEAudioEntity();
    ~CAEAudioEntity() = default;

public:
    virtual void UpdateParameters(CAESound* sound, int16 curPlayPos) { /* Empty on purpose */ };

    static bool StaticInitialise();
    static void Shutdown();
    static float GetDefaultVolume(eAudioEvents event) { return static_cast<float>(m_pAudioEventVolumes[event]); }
};

VALIDATE_SIZE(CAEAudioEntity, 0x7C);
