#include "StdInc.h"

#include "AudioZones.h"

void CAudioZones::InjectHooks() {
    RH_ScopedClass(CAudioZones);
    RH_ScopedCategory("Audio");

    RH_ScopedInstall(Init, 0x5081A0);
    RH_ScopedInstall(RegisterAudioSphere, 0x5081C0);
    RH_ScopedInstall(RegisterAudioBox, 0x508240);
    RH_ScopedInstall(SwitchAudioZone, 0x508320);
    RH_ScopedInstall(Update, 0x5083C0);
}

// 0x5081A0
void CAudioZones::Init() {
    ZoneScoped;

    m_NumSpheres = 0;
    m_NumBoxes = 0;
    m_NumActiveSpheres = 0;
    m_NumActiveBoxes = 0;
}

// 0x508240
void CAudioZones::RegisterAudioBox(char name[8], int32 id, bool isActive, CVector min, CVector max) {

    tAudioZoneBox audioZoneBox;
    strcpy_s(audioZoneBox.m_szName, name);
    audioZoneBox.m_bIsActive = isActive; // TODO: m_nFlags field has only 1 flag - Active or inactive and takes only 1 bit. Although gta uses 2 bytes for this, but how is the idea to define this single flag so as not to be confused in the future
    audioZoneBox.m_nAudioZone = id;
    audioZoneBox.m_Box = CompressedBox{
        .m_vecMin = CompressLargeVector(min),
        .m_vecMax = CompressLargeVector(max),
    };
    m_aBoxes[m_NumBoxes++] = audioZoneBox;
}

// 0x5081C0
void CAudioZones::RegisterAudioSphere(char name[8], int32 id, bool isActive, CVector position, float radius) {
    tAudioZoneSphere audioZoneSphere;
    strcpy_s(audioZoneSphere.m_szName, name);
    audioZoneSphere.m_nAudioZone = id;
    audioZoneSphere.m_bIsActive = isActive; // TODO: m_nFlags field has only 1 flag - Active or inactive and takes only 1 bit. Although gta uses 2 bytes for this, but how is the idea to define this single flag so as not to be confused in the future
    audioZoneSphere.m_Sphere = {position, radius};

    m_aSpheres[m_NumSpheres++] = audioZoneSphere;
}

// 0x508320
void CAudioZones::SwitchAudioZone(const char* zoneName, bool enable) {
    for (auto& sphere : GetAvailableSpheres()) {
        if (!_stricmp(zoneName, sphere.m_szName))
            sphere.m_bIsActive = enable;
    }

    for (auto& box : GetAvailableBoxes()) {
        if (!_stricmp(zoneName, box.m_szName))
            box.m_bIsActive = enable;
    }
}

// 0x5083C0
void CAudioZones::Update(bool forceUpdate, CVector posn) {
    ZoneScoped;

    static CVector LastUpdateCoors{};

    // TODO: FPS-independent counter
    if (!forceUpdate && (CTimer::m_FrameCounter % 16) != 6 && DistanceBetweenPoints(posn, LastUpdateCoors) < 20.0f)
        return;

    LastUpdateCoors = posn;
    m_NumActiveSpheres = m_NumActiveBoxes = 0;

    for (auto&& [idx, sphere] : notsa::enumerate(GetAvailableSpheres())) {
        // Do not add more to list if it's filled.
        if (m_NumActiveSpheres >= std::size(m_aActiveSpheres))
            break;

        if (!sphere.m_bIsActive)
            continue;

        if (sphere.m_Sphere.IsPointWithin(posn))
            m_aActiveSpheres[m_NumActiveSpheres++] = idx;
    }

    for (auto&& [idx, box] : notsa::enumerate(GetAvailableBoxes())) {
        // Do not add more to list if it's filled.
        if (m_NumActiveBoxes >= std::size(m_aActiveBoxes))
            break;

        if (!box.m_bIsActive)
            continue;

        if (CBox{box.m_Box}.IsPointInside(posn))
            m_aActiveBoxes[m_NumActiveBoxes++] = idx;
    }
}
