#pragma once

#include "CompressedBox.h"
#include <span>

struct tAudioZoneData {
    char  m_szName[8];
    int16 m_nAudioZone;
    bool  m_bIsActive : 1;
};
VALIDATE_SIZE(tAudioZoneData, 0xC);

struct tAudioZoneSphere : tAudioZoneData {
    CSphere m_Sphere;
};
VALIDATE_SIZE(tAudioZoneSphere, 0x1C);

struct tAudioZoneBox : tAudioZoneData {
    CompressedBox m_Box;

    void DrawWireFrame(CRGBA color, const CMatrix& transform) const {
        m_Box.DrawWireFrame(color, transform);
    }
};
VALIDATE_SIZE(tAudioZoneBox, 0x18);

class CAudioZones {
public:
    static inline auto& m_aActiveBoxes = *reinterpret_cast<std::array<int32, 10>*>(0xB6DC6C);
    static inline auto& m_aActiveSpheres = *reinterpret_cast<std::array<int32, 10>*>(0xB6DC94);

    static inline auto& m_NumActiveBoxes = *reinterpret_cast<uint32*>(0xB6DCBC);
    static inline auto& m_NumActiveSpheres = *reinterpret_cast<uint32*>(0xB6DCC0);
    static inline auto& m_NumBoxes = *reinterpret_cast<uint32*>(0xB6DCC4);
    static inline auto& m_NumSpheres = *reinterpret_cast<uint32*>(0xB6DCC8);

    static constexpr int32 NUM_AUDIO_BOXES = 158;
    static inline auto& m_aBoxes = *reinterpret_cast<std::array<tAudioZoneBox, NUM_AUDIO_BOXES>*>(0xB6DCD0);

    static constexpr int32 NUM_AUDIO_SPHERES = 3;
    static inline auto& m_aSpheres = *reinterpret_cast<std::array<tAudioZoneSphere, NUM_AUDIO_SPHERES>*>(0xB6EBA8);

public:
    static void InjectHooks();

    static void Init();

    static void RegisterAudioBox(char name[8], int32 id, bool isActive, CVector min, CVector max);
    static void RegisterAudioSphere(char name[8], int32 id, bool isActive, CVector position, float radius);

    static void SwitchAudioZone(const char* zoneName, bool enable);
    static void Update(bool forceUpdate, CVector posn);

    static auto GetActiveAuZoBoxes() { // TODO/NOTE: This isn't how it works! See `m_aActiveBoxes`
        return m_aBoxes | rng::views::take((size_t)m_NumBoxes);
    }

    // TODO: idc about func at top rn.
    static auto GetAvailableBoxes() {
        return m_aBoxes | rng::views::take((size_t)m_NumBoxes);
    }

    static auto GetAvailableSpheres() {
        return m_aSpheres | rng::views::take((size_t)m_NumSpheres);
    }
};
