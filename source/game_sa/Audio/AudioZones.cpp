#include "StdInc.h"

#include "AudioZones.h"

int32 (&CAudioZones::m_aActiveBoxes)[10] = *(int32(*)[10])0xB6DC6C;
int32 (&CAudioZones::m_aActiveSpheres)[10] = *(int32(*)[10])0xB6DC94;

uint32& CAudioZones::m_NumActiveBoxes = *(uint32*)0xB6DCBC;
uint32& CAudioZones::m_NumActiveSpheres = *(uint32*)0xB6DCC0;
uint32& CAudioZones::m_NumBoxes = *(uint32*)0xB6DCC4;
uint32& CAudioZones::m_NumSpheres = *(uint32*)0xB6DCC8;

tAudioZoneBox (&CAudioZones::m_aBoxes)[NUM_AUDIO_BOXES] = *(tAudioZoneBox(*)[NUM_AUDIO_BOXES])0xB6DCD0;
tAudioZoneSphere (&CAudioZones::m_aSpheres)[NUM_AUDIO_SPHERES] = *(tAudioZoneSphere(*)[NUM_AUDIO_SPHERES])0xB6EBA8;

void CAudioZones::InjectHooks() {
    RH_ScopedClass(CAudioZones);
    RH_ScopedCategory("Audio");

    // RH_ScopedInstall(Init, 0x5081A0);
    // RH_ScopedInstall(RegisterAudioSphere, 0x5081C0);
    // RH_ScopedInstall(RegisterAudioBox, 0x508240);
    // RH_ScopedInstall(SwitchAudioZone, 0x508320);
    // RH_ScopedInstall(Update, 0x5083C0);
}

// 0x5081A0
void CAudioZones::Init() {
    m_NumSpheres = 0;
    m_NumBoxes = 0;
    m_NumActiveSpheres = 0;
    m_NumActiveBoxes = 0;
}

// 0x508240
int32 CAudioZones::RegisterAudioBox(char* name, int32 id, bool b, float x1, float y1, float z1, float x2, float y2, float z2) {
    return plugin::CallAndReturn<int32, 0x508240, char*, int32, bool, float, float, float, float, float, float>(name, id, b, x1, y1, z1, x2, y2, z2);
}

// 0x5081C0
int32 CAudioZones::RegisterAudioSphere(char* name, int32 id, bool b, float x1, float y1, float z1, float radius) {
    return plugin::CallAndReturn<int32, 0x5081C0, char*, int32, bool, float, float, float, float>(name, id, b, x1, y1, z1, radius);
}

// 0x508320
void CAudioZones::SwitchAudioZone(char* zoneName, bool enable) {
    plugin::Call<0x508320, const char*, bool>(zoneName, enable);
}

// 0x5083C0
void CAudioZones::Update(bool a1, CVector posn) {
    plugin::Call<0x5083C0, bool, CVector>(a1, posn);
}
