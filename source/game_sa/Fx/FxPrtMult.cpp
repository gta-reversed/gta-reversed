#include "StdInc.h"

#include "FxPrtMult.h"

// 0x4AB270
FxPrtMult_c::FxPrtMult_c() {
    SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_fSize = 1.0f;
    m_Rot = 1.0f;
    m_fLife = 1.0f;
}

// 0x4AB290
FxPrtMult_c::FxPrtMult_c(float red, float green, float blue, float alpha, float size, float rot, float life) {
    SetUp(red, green, blue, alpha, size, rot, life);
}

// 0x4AB2D0
void FxPrtMult_c::SetUp(float red, float green, float blue, float alpha, float size, float rot, float life) {
    SetColor(red, green, blue, alpha);
    m_fSize = size;
    m_Rot = rot;
    m_fLife = life;
}
