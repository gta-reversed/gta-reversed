#pragma once

#include "CompressedBox.h"

struct tAudioZoneSphere {
    char    m_szName[8];
    uint16  m_nAudioZone;
    uint16  m_nFlags;
    CVector m_vPosn;
    float   m_fRadius;
};
VALIDATE_SIZE(tAudioZoneSphere, 0x1C);

struct tAudioZoneBox {
    char          m_Name[8];
    uint16        m_nAudioZone;
    uint16        m_Flags;
    CompressedBox m_Box;
};
VALIDATE_SIZE(tAudioZoneBox, 0x18);

class CAudioZones {
public:
    static int32 (&m_aActiveBoxes)[10];
    static int32 (&m_aActiveSpheres)[10];

    static uint32& m_NumActiveBoxes;
    static uint32& m_NumActiveSpheres;
    static uint32& m_NumBoxes;
    static uint32& m_NumSpheres;

    static constexpr int32 NUM_AUDIO_BOXES = 158;
    static tAudioZoneBox (&m_aBoxes)[NUM_AUDIO_BOXES];

    static constexpr int32 NUM_AUDIO_SPHERES = 3;
    static tAudioZoneSphere (&m_aSpheres)[NUM_AUDIO_SPHERES];

public:
    static void InjectHooks();

    static void Init();

    static int32  RegisterAudioBox(char name[16], int32 id, bool b, float x1, float y1, float z1, float x2, float y2, float z2);
    static int32  RegisterAudioSphere(char name[16], int32 id, bool b, float x1, float y1, float z1, float radius);
};
