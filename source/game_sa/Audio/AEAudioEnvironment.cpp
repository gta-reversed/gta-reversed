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
    RH_ScopedOverloadedInstall(GetPositionRelativeToCamera, "vec", 0x4D80B0, CVector(*)(const CVector&));
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
    static constexpr float CUT_OFF_FREQ = 0.70710678118F; // sqrt(0.5F);
    static constexpr float ATTENUATION_FACTOR = -6.0F;

    // BUG? Seems weird that it uses just single axis,
    // seems like it should be normalized dot product
    // with for example Camera direction,
    // to work the same way regardless of direction
    const auto freq = soundDir.Dot({ 0.f, 1.f, 0.f });

    if (CUT_OFF_FREQ == -1.0F) {
        return 0.0F;
    }

    if (freq >= CUT_OFF_FREQ) {
        return 0.f;
    }

    if (freq <= -CUT_OFF_FREQ) {
        return ATTENUATION_FACTOR;
    }

    return (1.0f - invLerp(-CUT_OFF_FREQ, CUT_OFF_FREQ, freq)) * ATTENUATION_FACTOR;
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
    const auto Calculate = [&](CVector offset) {
        const auto posOS = pt - TheCamera.GetPosition() + offset;
        return camMat.InverseTransformVector(CVector{-posOS.x, posOS.y, posOS.z});
    };
    switch (CCamera::GetActiveCamera().m_nMode) {
    case eCamMode::MODE_SNIPER:
    case eCamMode::MODE_ROCKETLAUNCHER:
    case eCamMode::MODE_1STPERSON: {
        return Calculate(-camMat.GetForward() * 2.f);
    }
    default: {
        const auto* player = FindPlayerPed();
        const auto camDist = player
            ? std::clamp(CVector::Dist(camMat.GetPosition(), player->GetPosition()), 0.0F, 0.5F)
            : 0.f;
        return Calculate(camMat.GetForward() * camDist);
    }
    }
}

// 0x4D8340
CVector CAEAudioEnvironment::GetPositionRelativeToCamera(CPlaceable* placeable) {
    return CAEAudioEnvironment::GetPositionRelativeToCamera(placeable->GetPosition());
}
