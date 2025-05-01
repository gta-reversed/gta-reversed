#include "StdInc.h"

#include "FxInterpInfoFloat.h"
#include "FxManager.h"

// 0x4A8440
FxInterpInfoFloat_c::FxInterpInfoFloat_c() : FxInterpInfo_c() {
    m_Keys = nullptr;
}

// 0x5C16F0
void FxInterpInfoFloat_c::Load(FILESTREAM file) {
    for (auto i = 0; i < m_nCount; i++) {
        ReadField<void>(file);
        ReadField<void>(file, "FX_INTERP_DATA:");

        m_bLooped  = ReadField<bool>(file);
        m_nNumKeys = ReadField<int8>(file);

        if (i == 0) {
            m_pTimes = g_fxMan.Allocate<uint16>(m_nNumKeys);
        }

        m_Keys[i] = g_fxMan.Allocate<float>(m_nNumKeys);
        for (auto j = 0; j < m_nNumKeys; j++) {
            ReadField<void>(file, "FX_KEYFLOAT_DATA:");
            m_pTimes[j] = uint16(ReadField<float>(file) * 256.f);
            m_Keys[i][j] = ReadField<float>(file);
        }
    }
}

// NOTSA
void FxInterpInfoFloat_c::Allocate(int32 count) {
    m_nCount = count;
    m_Keys = g_fxMan.Allocate<float*>(count);
}

// 0x4A85C0
float FxInterpInfoFloat_c::GetVal(int32 attrib, float time, float deltaTime) {
    return plugin::CallMethodAndReturn<float, 0x4A85C0, FxInterpInfoFloat_c*, int32, float, float>(this, attrib, time, deltaTime);
}

// 0x4A8470
void FxInterpInfoFloat_c::GetVal(float* outValues, float delta) {
    return plugin::CallMethod<0x4A8470, FxInterpInfoFloat_c*, float*, float>(this, outValues, delta);

    if (m_nNumKeys == 1) {
        for (auto i = 0; i < m_nCount; i++) {
            outValues[i] = *m_Keys[i];
        }
        return;
    }

    if (m_bLooped) {
        auto totalTime = (float)m_pTimes[m_nNumKeys - 1] / 256.0f;
        delta = delta - (delta / totalTime) * totalTime;
    }

    if (m_nNumKeys <= 1) {
        for (auto j = 0; j < m_nCount; j++) {
            outValues[j] = m_Keys[j][m_nNumKeys - 1];
        }
        return;
    }

    int v8 = 1;
    float v11;
    uint16* v10 = m_pTimes + 1;
    while (true) {
        v11 = (float)*v10 / 256.0f;
        if (delta < v11)
            break;
        ++v8;
        ++v10;
        if (v8 >= m_nNumKeys) {
            for (auto j = 0; j < m_nCount; j++) {
                outValues[j] = m_Keys[j][m_nNumKeys - 1];
            }
            return;
        }
    }

    int v13 = 0;
    float v14 = (float)m_pTimes[v8 - 1] / 256.0f;
    float a3a;
    float* v15;
    for (a3a = (delta - v14) / (v11 - v14); v13 < m_nCount; outValues[v13 - 1] = (v15[v8] - v15[v8 - 1]) * a3a + v15[v8 - 1]) {
        v15 = m_Keys[v13++];
    }
}
