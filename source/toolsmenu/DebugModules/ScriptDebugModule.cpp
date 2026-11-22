#include <StdInc.h>
#include "ScriptDebugModule.hpp"
#include "TheScripts.h"

namespace notsa { 
namespace debugmodules {
void ScriptDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "Scripts", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }
    using namespace ImGui;
    Checkbox("DbgFlag", &CTheScripts::DbgFlag);
}

void ScriptDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Scripts", nullptr, &m_IsOpen);
    });
}

void ScriptDebugModule::Render3D() {
    /*
    constexpr struct {
        CVector2D a, b;
        float rot;
    } areas[]{
        //{{0.f, 0.f}, {5.f, 10.f}, 5.f}

        {{ 2198.686 / 10.f,     -2180.832 / 10.f, }, {  2116.212 / 10.f,   -2263.317 / 10.f,  },  82.0 / 10.f},
        //{{ 2172.36,      -2207.176, }, {  2116.212,   2263.317,  },  82.0},
        //{{ 2198.686,     -2180.832, }, {  2116.212,   2263.317,  },  82.0},
        //{{ 2198.686,     -2180.832, }, {  2116.212,   2263.317,  },  82.0},
        //{{ 2167.955,     -2253.054, }, {  2175.023,   2245.327,  }, -11.5},
        //{{ 2161.927,     -2243.2519,}, {  2168.71,    2237.79,   },  10.0},
        //{{ 2160.6587,    -2244.4089,}, {  2168.71,    2237.79,   },  12.0},
        //{{ 2157.47,      -2246.72,  }, {  2152.737,   2251.8567, }, -10.0},
        //{{ 2160.728,     -2254.586, }, {  2152.4,     2263.3,    },  10.0},
        //{{ 1222.799,     -735.563,  }, {  1348.407,   759.322,   }, -100.0},
        //{{-1559.2874,     1337.1991,}, { -1562.5834,  1327.2573, }, -62.9096},
        //{{-1590.3267,     1336.8811,}, { -1607.2059,  1307.5653, }, -37.0915},
        //{{-1616.5742,    1319.3361, }, { -1632.8600,  1303.4518, }, -23.9075},
        //{{-1595.1842,    1316.6805, }, { -1603.4827,  1308.8937, }, -8.3514},
        //{{ 1048.4285,    2093.5213, }, {  1048.8224,  2081.8444, },  15.0},
        //{{ 1055.9634,    2093.8372, }, {  1055.8059,  2081.0337, },  10.0},
        //{{ 1055.1740,    2064.7097, }, {  1054.8353,  21099.8091,},  15.0},
        //{{ 1018.6445,    2100.5955, }, {  1020.0024,  2145.0366, }, -40.0},
        //{{ 1030.1047,    2095.7146, }, {  1054.8175,  2098.7776, },  30.0},
    };

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));

    const CRGBA color[4]{ // from top left, clockwise
        {255, 0, 0, 255},
        {0, 255, 0, 255},
        {0, 0, 255, 255},
        {255, 255, 0, 255}
    };
    for (auto& area : areas) {
        const auto angle = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(
            area.a.x, area.a.y,
            area.b.x, area.b.y
        ) + HALF_PI);
        const CVector2D offset{
            +std::sin(angle) * area.rot,
            -std::cos(angle) * area.rot,
        };
        const auto isInArea = IsPositionInAngledArea(FindPlayerCoors(), area.a, area.b, area.rot);
        CVector2D pos[]{
            //area.a - CVector2D{std::sin(angle + PI), -std::cos(angle + PI)} * area.rot,
            area.a,
            area.b,
            area.b + CVector2D{std::sin(angle),       -std::cos(angle)} * area.rot,
        };
        RwIm3DVertex vertices[std::size(pos)];
        for (auto i = 0u; i < std::size(pos); i++) {
            RwIm3DVertexSetPos(&vertices[i], pos[i].x, pos[i].y, CWorld::FindGroundZForCoord(pos[i].x, pos[i].y) + 2.f);
            RwIm3DVertexSetRGBA(&vertices[i], color[i + isInArea].r, color[i + isInArea].g, color[i + isInArea].b, color[i].a);
        }
        RwImVertexIndex idxs[]{ 0, 1, 2 }; // , 0, 1, 3 
        if (RwIm3DTransform(vertices, std::size(vertices), nullptr, rwIM3D_VERTEXRGBA)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, idxs, std::size(idxs));
            RwIm3DEnd();
        }
    }
    */
}
}; // namespace debugmodules
}; // namespace notsa
