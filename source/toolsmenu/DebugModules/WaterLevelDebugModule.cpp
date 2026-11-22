#include "StdInc.h"
#include "WaterLevelDebugModule.hpp"
#include "WaterLevel.h"

#include <imgui.h>

namespace WaterLevelDebugModule {
void ProcessImGui() {
    using namespace ImGui;

    Checkbox("Skip Camera Range Update", &CWaterLevel::DontUpdateCameraRange);
    Checkbox("Don't render Y split tri", &CWaterLevel::DontRenderYSplitTri);

    const auto TogglableColorEditRGBA = [](bool& toggleState, const char* label, CRGBA& inOutColor) {
        PushID(label);
        Checkbox("Enable", &toggleState);
        SameLine();
        BeginDisabled(!toggleState);
        RwRGBAReal colorToEdit = inOutColor;
        if (ColorEdit4(label, (float*)&colorToEdit)) {
            inOutColor = CRGBA{ colorToEdit };
        }
        EndDisabled();
        PopID();
    };

    const auto DoWaterColor = [&](auto idx, const char* label) {
        auto& debugColor = CWaterLevel::DebugWaterColors[idx];
        TogglableColorEditRGBA(debugColor.active, label, debugColor.color);
    };
    DoWaterColor(CWaterLevel::DebugWaterColor::TRI, "Triangle Water Color");
    DoWaterColor(CWaterLevel::DebugWaterColor::RECT, "Rectangle Water Color");

    if (TreeNode("Water Wake Segment Part Colors")) {
        for (auto i = 0; i < 4; i++) {
            PushID(i);
            Text("Part %d", i);
            SameLine();
            ColorEdit3("Part", (float*)(&CWaterLevel::WakeSegmentPartColors[i]));
            PopID();
        }
        TreePop();
    }

    Checkbox("Enable", &CWaterLevel::bSplitBigPolys);
    SameLine();
    BeginDisabled(!CWaterLevel::bSplitBigPolys);
    SliderInt("Split Poly At Size", &CWaterLevel::BigPolySize, 1, 2048, "%d", ImGuiSliderFlags_Logarithmic); // 0 crashes the game (stack overflow)
    EndDisabled();
    
    SliderInt("m_MaxNumBlocksOutsideWorldToBeRendered", &CWaterLevel::m_MaxNumBlocksOutsideWorldToBeRendered, 0, (int)CWaterLevel::m_BlocksToBeRenderedOutsideWorldX.size());

    // Doesn't work (Color is always set to be the same as TimeCyc's water color -> Edit that instead)
    //DoWaterColorEdit("Triangle Water Color", CWaterLevel::WaterColorTriangle);
    //DoWaterColorEdit("Quad Water Color", CWaterLevel::WaterColor);

    // Doesn't work (Alpha is set each frame in the code, so this doesn't work)
    //for (auto& alpha : CWaterLevel::WaterLayerAlpha) {
    //    SliderInt("Water Layer Alpha", (int*)&alpha, 0, 255);
    //}
}
}; // namespace WaterLevelDebugModule
