#pragma once

#include "ListItem_c.h"
#include "Vector.h"

class FxSystem_c;
class FxEmitterPrt_c;

struct Particle_c : public ListItem_c<Particle_c> {
    float       m_fTotalLife;
    float       m_fCurrentLife;
    CVector     m_Pos;
    CVector     m_Velocity;
    FxSystem_c* m_System;

    // NOTSA
    FxEmitterPrt_c* AsFxEmitterPrt() { return reinterpret_cast<FxEmitterPrt_c*>(this); }
};
VALIDATE_SIZE(Particle_c, 0x2C);
