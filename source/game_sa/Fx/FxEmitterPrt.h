#pragma once

#include "ListItem_c.h"
#include "Vector.h"
#include "Particle.h"
#include "extensions/FixedFloat.hpp"

struct RwRGBA;
class FxSystem_c;

class FxEmitterPrt_c : public Particle_c, ListItem_c<FxEmitterPrt_c> {
public:
    CRGBA m_MultColor;
    FixedFloat<uint8, 255.0f> m_MultSize;
    FixedFloat<uint8, 255.0f> m_MultRot;
    uint8 m_RandR;
    uint8 m_RandG;
    uint8 m_RandB;
    FixedFloat<uint8, 100.0f> m_Brightness;
    int8  m_RotZ;
    bool  m_bLocalToSystem;
    float m_CurrentRotation;

public:
    FxEmitterPrt_c() = default;  // 0x4A94E0
    ~FxEmitterPrt_c() = default; // 0x4A94F0

    static void InjectHooks();

    static void* operator new[](size_t size);
};
VALIDATE_SIZE(FxEmitterPrt_c, 0x44);
