#include "StdInc.h"
#include "AEAudioEnvironment.h"

sReverbEnvironment (&gAudioZoneToReverbEnvironmentMap)[NUM_AUDIO_ENVIRONMENTS] = *(sReverbEnvironment(*)[NUM_AUDIO_ENVIRONMENTS])0x8AD670;

#include "data/SoundAttenuationTable.h"

void CAEAudioEnvironment::InjectHooks() {
    RH_ScopedClass(CAEAudioEnvironment);
    RH_ScopedCategory("Audio");

    RH_ScopedInstall(GetDopplerRelativeFrequency, 0x4D7E40);
    RH_ScopedInstall(GetDistanceAttenuation, 0x4D7F20);
    RH_ScopedInstall(GetDirectionalMikeAttenuation, 0x4D7F60);
    RH_ScopedInstall(GetReverbEnvironmentAndDepth, 0x4D8010);
    RH_ScopedOverloadedInstall(GetPositionRelativeToCamera, "vec", 0x4D80B0, CVector(*)(const CVector&), { .locked = true }); // Unook -> Crash due to different signature
    RH_ScopedOverloadedInstall(GetPositionRelativeToCamera, "placeable", 0x4D8340, CVector(*)(CPlaceable*));
}

// 0x4D7E40
float CAEAudioEnvironment::GetDopplerRelativeFrequency(float prevDist, float curDist, uint32 prevTime, uint32 curTime, float dopplerScale) {
    if (TheCamera.Get_Just_Switched_Status()) {
        return 1.0F;
    }

    const auto deltaDist = curDist - prevDist;
    if (dopplerScale == 0.0F || deltaDist == 0.0F || curTime <= prevTime) {
        return 1.0F;
    }

    const auto doppler = deltaDist * 1000.0F / (float)(curTime - prevTime) * dopplerScale;
    if (std::fabs(doppler) >= 340.0F) {
        return 1.0F;
    }

    return 340.0F / (std::clamp(doppler, -35.0F, 35.0F) + 340.0F);
}

// 0x4D7F20
float CAEAudioEnvironment::GetDistanceAttenuation(float dist) {
    assert(dist >= 0.f);
    return dist < ATTENUATION_TABLE_MAX_DIST
        ? gSoundDistAttenuationTable[(uint32)std::floor(dist / ATTENUATION_TABLE_RESOLUTION)]
        : -100.f;
}

// 0x4D7F60
float CAEAudioEnvironment::GetDirectionalMikeAttenuation(const CVector& soundDir) {
    // https://en.wikipedia.org/wiki/Cutoff_frequency
    static constexpr float fCutoffFrequency = 0.70710678118F; // sqrt(0.5F);
    static constexpr float fAttenuationMult = -6.0F;

    CVector vecDir = soundDir;
    vecDir.Normalise();

    // BUG? Seems weird that it uses just single axis, seems like it should be normalized Dot product with for example Camera direction, to work the same way regardless of
    // direction
    const auto freq = vecDir.y; // (vecDir.x + vecDir.z) * 0.0F + vecDir.y
    if (fCutoffFrequency == -1.0F || freq >= fCutoffFrequency)
        return 0.0F;

    if (freq <= -fCutoffFrequency)
        return fAttenuationMult;

    return (1.0F - invLerp(-fCutoffFrequency, fCutoffFrequency, freq)) * fAttenuationMult;
}

// 0x4D8010
void CAEAudioEnvironment::GetReverbEnvironmentAndDepth(int8* reverbEnv, int32* depth) {
    if (CWeather::UnderWaterness >= 0.5F) {
        *reverbEnv = 22; // todo: EAX_ENVIRONMENT_UNDERWATER
        *depth = -12;
        return;
    }

    int32 nZone;
    if (CAudioZones::m_NumActiveSpheres > 0)
        nZone = CAudioZones::m_aSpheres[CAudioZones::m_aActiveSpheres[0]].m_nAudioZone;
    else if (CAudioZones::m_NumActiveBoxes > 0)
        nZone = CAudioZones::m_aBoxes[CAudioZones::m_aActiveBoxes[0]].m_nAudioZone;
    else if (CGame::currArea)
        nZone = 14;
    else {
        *reverbEnv = 23; // todo: EAX_ENVIRONMENT_DRUGGED
        *depth = -100;
        return;
    }

    if (nZone != -1) {
        *reverbEnv = gAudioZoneToReverbEnvironmentMap[nZone].m_nEnvironment;
        *depth     = gAudioZoneToReverbEnvironmentMap[nZone].m_nDepth;
        return;
    }

    *reverbEnv = 23; // todo: EAX_ENVIRONMENT_DRUGGED
    *depth = -100;
}

// 0x4D80B0
CVector CAEAudioEnvironment::GetPositionRelativeToCamera(const CVector& pt) {
    const auto& camMat = TheCamera.m_mCameraMatrix;
    const auto Calculate = [&](CVector camOffset) -> CVector { // @ 0x4D82DF
        const auto p = camMat.InverseTransformVector(pt - (TheCamera.GetPosition() - camOffset));
        return { -p.x, p.y, p.z };
    };

    switch (CCamera::GetActiveCamera().m_nMode) {
    case eCamMode::MODE_SNIPER:
    case eCamMode::MODE_ROCKETLAUNCHER:
    case eCamMode::MODE_1STPERSON:
        return Calculate(camMat.GetForward() * 2.f);
    }

    const auto camDist = FindPlayerPed()
        ? CVector::Dist(camMat.GetPosition(), FindPlayerPed()->GetPosition())
        : 0.5f;
    return Calculate(camMat.GetBackward() * std::clamp(camDist - 0.5f, 0.f, 0.5f));
}

// 0x4D8340
CVector CAEAudioEnvironment::GetPositionRelativeToCamera(CPlaceable* placeable) {
    return CAEAudioEnvironment::GetPositionRelativeToCamera(placeable->GetPosition());
}
