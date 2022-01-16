#include "StdInc.h"
#include "AEAudioEnvironment.h"

sReverbEnvironment (&gAudioZoneToReverbEnvironmentMap)[NUM_AUDIO_ENVIRONMENTS] = *(sReverbEnvironment(*)[NUM_AUDIO_ENVIRONMENTS])0x8AD670;
float (&gSoundDistAttenuationTable)[NUM_SOUND_DIST_ATTENUATION_ENTRIES] = *(float(*)[NUM_SOUND_DIST_ATTENUATION_ENTRIES])0x8AC270;

void CAEAudioEnvironment::InjectHooks() {
    ReversibleHooks::Install("CAEAudioEnvironment", "GetDopplerRelativeFrequency", 0x4D7E40, &CAEAudioEnvironment::GetDopplerRelativeFrequency);
    ReversibleHooks::Install("CAEAudioEnvironment", "GetDistanceAttenuation", 0x4D7F20, &CAEAudioEnvironment::GetDistanceAttenuation);
    ReversibleHooks::Install("CAEAudioEnvironment", "GetDirectionalMikeAttenuation", 0x4D7F60, &CAEAudioEnvironment::GetDirectionalMikeAttenuation);
    ReversibleHooks::Install("CAEAudioEnvironment", "GetReverbEnvironmentAndDepth", 0x4D8010, &CAEAudioEnvironment::GetReverbEnvironmentAndDepth);
    ReversibleHooks::Install("CAEAudioEnvironment", "GetPositionRelativeToCamera_vec", 0x4D80B0, (void(*)(CVector*, CVector*)) & CAEAudioEnvironment::GetPositionRelativeToCamera);
    ReversibleHooks::Install("CAEAudioEnvironment", "GetPositionRelativeToCamera_placeable", 0x4D8340, (void(*)(CVector*, CPlaceable*)) & CAEAudioEnvironment::GetPositionRelativeToCamera);
}

float CAEAudioEnvironment::GetDopplerRelativeFrequency(float prevDist, float curDist, uint32 prevTime, uint32 curTime, float timeScale) {
    const auto fDistDiff = curDist - prevDist;
    if (TheCamera.Get_Just_Switched_Status())
        return 1.0F;

    if (timeScale == 0.0F || fDistDiff == 0.0F || curTime <= prevTime)
        return 1.0F;

    const auto fDoppler = fDistDiff * 1000.0F / static_cast<float>(curTime - prevTime) * timeScale;
    if (fabs(fDoppler) >= 340.0F)
        return 1.0F;

    const auto fClamped = clamp(fDoppler, -35.0F, 35.0F);
    return 340.0F / (fClamped + 340.0F);
}

float CAEAudioEnvironment::GetDistanceAttenuation(float dist) {
    if (dist >= 128.0F)
        return -100.0F;

    auto iArrIndex = static_cast<uint32>(floor(dist * 10.0F));
    return gSoundDistAttenuationTable[iArrIndex];
}

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

void CAEAudioEnvironment::GetReverbEnvironmentAndDepth(int8* reverbEnv, int32* depth) {
    if (CWeather::UnderWaterness >= 0.5F) {
        *reverbEnv = 22;
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
        *reverbEnv = 23;
        *depth = -100;
        return;
    }

    if (nZone != -1) {
        *reverbEnv = gAudioZoneToReverbEnvironmentMap[nZone].m_nEnvironment;
        *depth = gAudioZoneToReverbEnvironmentMap[nZone].m_nDepth;
        return;
    }

    *reverbEnv = 23;
    *depth = -100;
}

void CAEAudioEnvironment::GetPositionRelativeToCamera(CVector* vecOut, CVector* vecPos) {
    static const float fFirstPersonMult = 2.0F;
    if (!vecPos)
        return;

    const auto camMode = CCamera::GetActiveCamera().m_nMode;
    if (camMode == eCamMode::MODE_SNIPER || camMode == eCamMode::MODE_ROCKETLAUNCHER || camMode == eCamMode::MODE_1STPERSON) {
        CMatrix    tempMat = TheCamera.m_mCameraMatrix;
        auto&      vecCamPos = TheCamera.GetPosition();
        const auto vecOffset = *vecPos - (vecCamPos - tempMat.GetForward() * fFirstPersonMult);

        vecOut->x = -DotProduct(vecOffset, tempMat.GetRight());
        vecOut->y = DotProduct(vecOffset, tempMat.GetForward());
        vecOut->z = DotProduct(vecOffset, tempMat.GetUp());
        return;
    }

    auto  fMult = 0.0F;
    auto* player = FindPlayerPed(-1);
    if (player)
        fMult = clamp(DistanceBetweenPoints(player->GetPosition(), TheCamera.GetPosition()), 0.0F, 0.5F);

    CMatrix    tempMat = TheCamera.m_mCameraMatrix;
    auto&      vecCamPos = TheCamera.GetPosition();
    const auto vecOffset = *vecPos - (vecCamPos + tempMat.GetForward() * fMult);

    vecOut->x = -DotProduct(vecOffset, tempMat.GetRight());
    vecOut->y = DotProduct(vecOffset, tempMat.GetForward());
    vecOut->z = DotProduct(vecOffset, tempMat.GetUp());
}

void CAEAudioEnvironment::GetPositionRelativeToCamera(CVector* vecOut, CPlaceable* placeable) {
    return CAEAudioEnvironment::GetPositionRelativeToCamera(vecOut, &placeable->GetPosition());
}
