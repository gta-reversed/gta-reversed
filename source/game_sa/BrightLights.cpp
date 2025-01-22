#include "StdInc.h"

#include "BrightLights.h"

uint32& CBrightLights::NumBrightLights = *(uint32*)0xC7C6FC;
tBrightLight (&CBrightLights::aBrightLights)[MAX_NUM_BRIGHTLIGHTS] = *(tBrightLight(*)[MAX_NUM_BRIGHTLIGHTS])0xC7CB58;

void CBrightLights::InjectHooks() {
    RH_ScopedClass(CBrightLights);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x722140);
    RH_ScopedInstall(RenderOutGeometryBuffer, 0x722150);
    RH_ScopedInstall(Render, 0x7241C0);
    RH_ScopedInstall(RegisterOne, 0x724770);
}

// 0x722140
void CBrightLights::Init() {
    NumBrightLights = 0;
}

// 0x722150
void CBrightLights::RenderOutGeometryBuffer() {
    if (uiTempBufferIndicesStored) {
        LittleTest();
        if (RwIm3DTransform(TempBufferVertices.m_3d, uiTempBufferVerticesStored, nullptr, rwIM3D_VERTEXUV))
        {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, uiTempBufferIndicesStored);
            RwIm3DEnd();
        }
        uiTempBufferVerticesStored = 0;
        uiTempBufferIndicesStored = 0;
    }
}

// 0x7241C0
void CBrightLights::Render() {
    ZoneScoped;

    if (NumBrightLights == 0)
        return;

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLNONE));

    uiTempBufferVerticesStored = 0;
    uiTempBufferIndicesStored = 0;

    for (unsigned i = 0; i < NumBrightLights; i++) {
        // Maybe render vertex buffer if running low on free items...
        if (uiTempBufferIndicesStored > TOTAL_TEMP_BUFFER_INDICES - 40u ||
            uiTempBufferVerticesStored > TOTAL_TEMP_BUFFER_3DVERTICES - 40u
        ) {
            CBrightLights::RenderOutGeometryBuffer();
        }
        aBrightLights[i].Render();
    }

    RenderOutGeometryBuffer();

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);

    NumBrightLights = 0;
}

// 0x724770
void CBrightLights::RegisterOne(CVector posn, CVector top, CVector right, CVector at, eBrightLightColor color, uint8 arg5, uint8 arg6, uint8 arg7) {
    if (color == eBrightLightColor::BRIGHTLIGHT_NONE)
        return;

    if (NumBrightLights >= MAX_NUM_BRIGHTLIGHTS)
        return;

    const float distToCam = (TheCamera.GetPosition() - posn).Magnitude() / TheCamera.m_fLODDistMultiplier;
    if (distToCam > 60.0f)
        return;

    auto& light = aBrightLights[NumBrightLights];

    light.m_vecPosition       = posn;
    light.m_vecRight          = top;
    light.m_vecTop            = right;
    light.m_vecAt             = at;
    light.m_fDistanceToCamera = distToCam;
    light.m_nColor            = color;
    light.field_35            = arg5;
    light.field_36            = arg6;
    light.field_37            = arg7;

    NumBrightLights++;
}

// NOTSA, inlined
CRGBA tBrightLight::GetColorRGBA() const {
    // Return alpha value.
    // _BIG has at most 127 alpha, while _SMALL has 255
    const auto GetAlpha = [this]() -> uint8 {

        const auto UnadjAlpha = [this] {
            if (m_fDistanceToCamera >= 45.0f)
                return (uint8)((1.0f - invLerp(45.0f, 60.0f, m_fDistanceToCamera)) * 255.0f);
            return (uint8)255;
        };

        switch (m_nColor) {
        case BRIGHTLIGHT_GREEN_BIG:
        case BRIGHTLIGHT_YELLOW_BIG:
        case BRIGHTLIGHT_RED_BIG:
            return UnadjAlpha() / 2;

        case BRIGHTLIGHT_GREEN_SMALL:
        case BRIGHTLIGHT_YELLOW_SMALL:
        case BRIGHTLIGHT_RED_SMALL:
            return UnadjAlpha();

        default: { // NOTSA
            assert(0);
            return 0;
        }
        }
    };

    // Originally R* used:
    //const auto rnd = (uint8)((float)(uint8)CGeneral::GetRandomNumber() / 5.0f);
    const auto rnd = (uint8)CGeneral::GetRandomNumberInRange(0, 255 / 5);
    switch (m_nColor) {
    case BRIGHTLIGHT_GREEN_BIG:
    case BRIGHTLIGHT_GREEN_SMALL:
        return { rnd, 255, rnd, GetAlpha() };

    case BRIGHTLIGHT_YELLOW_BIG:
    case BRIGHTLIGHT_YELLOW_SMALL:
        return { 255, 128, rnd, GetAlpha() };

    case BRIGHTLIGHT_RED_BIG:
    case BRIGHTLIGHT_RED_SMALL:
        return { 255, rnd, rnd, GetAlpha() };

    default: { // NOTSA
        NOTSA_UNREACHABLE();
        return { 255, 255, 255, 255 }; // Shouldn't reach this point.. in theory that is..
    }
    }
}

// NOTSA, inlined
void tBrightLight::Render() const {
    // Get absolute vertex index in buffer (relative to the current number of vertices)
    const auto GetVertexRealIdx = [](unsigned i) {
        return (unsigned)uiTempBufferVerticesStored + i;
    };

    // Get vertex pointer, `i` is passed to `GetVertexRealIdx`
    const auto GetVertex = [=](unsigned i) {
        return &TempBufferVertices.m_3d[GetVertexRealIdx(i)];
    };

    // Set position of vertex, workaround for taking a temporary's address
    const auto SetVertexPosition = [=](unsigned i, CVector posn) {
        RxObjSpace3DVertexSetPos(GetVertex(i), &posn);
    };

    // Push a vertex index. `vertIdx` is passed to `GetVertexRealIdx`
    auto PushVertexIndex = [=, idx = 0](unsigned vertIdx) mutable {
        aTempBufferIndices[(unsigned)uiTempBufferIndicesStored + idx++] = GetVertexRealIdx(vertIdx);
    };

    const RwRGBA color = GetColorRGBA().ToRwRGBA();
    switch (m_nColor) {
    case BRIGHTLIGHT_GREEN_BIG:
    case BRIGHTLIGHT_YELLOW_BIG:
    case BRIGHTLIGHT_RED_BIG: {
        static constexpr float TrafficLightsUp[]   = { 0.162f, 0.162f, 0.0f, -0.162f, -0.162f, 0.0f };  // 0x5C0A7C
        static constexpr float TrafficLightsSide[] = { -0.09f, 0.09f, 0.162f, 0.09f, -0.09f, -0.162f }; // 0x5C0A94

        for (unsigned i = 0; i < 6; i++) {
            RxObjSpace3DVertexSetPreLitColor(GetVertex(i), &color);

            SetVertexPosition(i,
                  m_vecPosition
                + m_vecRight * TrafficLightsUp[i]
                + m_vecTop * TrafficLightsSide[i]
            );
        }

        // 0x8D5DBC
        static constexpr RxVertexIndex TrafficLightIndices[] = { 0, 1, 5, 1, 2, 3, 1, 3, 4, 1, 4, 5 };
        for (auto& index : TrafficLightIndices) {
            PushVertexIndex(index);
        }

        uiTempBufferIndicesStored += (uint16)std::size(TrafficLightIndices);
        uiTempBufferVerticesStored += 6;
        break;
    }
    case BRIGHTLIGHT_GREEN_SMALL:
    case BRIGHTLIGHT_YELLOW_SMALL:
    case BRIGHTLIGHT_RED_SMALL: {
        constexpr unsigned segCount = 8;
        constexpr float    segAngleStep = 2 * PI / (float)(segCount);
        constexpr float    segSize = 0.1f;
        for (unsigned i = 0; i < segCount; i++) {
            RxObjSpace3DVertexSetPreLitColor(GetVertex(i), &color);

            const float angle = segAngleStep * (float)(i);
            SetVertexPosition(i,
                  m_vecPosition
                + m_vecRight * std::cos(angle) * segSize
                + m_vecTop * std::sin(angle) * segSize
            );
        }

        for (unsigned i = 1; i <= 6; i++) {
            PushVertexIndex(0);
            PushVertexIndex(i);
            PushVertexIndex(i + 1);
        }

        uiTempBufferIndicesStored += 18;
        uiTempBufferVerticesStored += segCount;
        break;
    }
    default: {
        assert(0); // NOTSA
        break;
    }
    }
}
