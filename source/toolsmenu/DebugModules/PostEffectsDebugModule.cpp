#include "StdInc.h"

#include "PostEffectsDebugModule.h"
#include "PostEffects.h"

void PostEffectsDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Settings", "Post FX" }, [&] {
        ImGui::Checkbox("In Cutscene",            &CPostEffects::m_bInCutscene          );
        ImGui::Checkbox("Skip Post Process",      &CPostEffects::m_bDisableAllPostEffect);
        ImGui::Checkbox("Save Photo From Script", &CPostEffects::m_bSavePhotoFromScript );
        ImGui::Checkbox("Radiosity",              &CPostEffects::m_bRadiosity           );
        ImGui::Checkbox("Night Vision",           &CPostEffects::m_bNightVision         );
        ImGui::Checkbox("Grain",                  &CPostEffects::m_bGrainEnable         );
        ImGui::Checkbox("Darkness Filter",        &CPostEffects::m_bDarknessFilter      );
        ImGui::Checkbox("CCTV",                   &CPostEffects::m_bCCTV                );
        ImGui::Checkbox("SpeedFX Test Mode",      &CPostEffects::m_bSpeedFXTestMode     );
        ImGui::Checkbox("Fog",                    &CPostEffects::m_bFog                 );
        ImGui::Checkbox("Water Depth Darkness",   &CPostEffects::m_bWaterDepthDarkness  );
        ImGui::Checkbox("Color Correction",       &CPostEffects::m_bColorEnable         );

        const auto u8_zero = 0, u8_max = 255;

        ImGui::Checkbox("##Enable Infrared", &CPostEffects::m_bInfraredVision);
        ImGui::SameLine();
        if (ImGui::TreeNodeEx("Infrared Vision", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding)) {
            const notsa::ui::ScopedDisable disabledScope{ !CPostEffects::m_bInfraredVision };

            ImGui::DragScalar("Grain Strength",    ImGuiDataType_S32, &CPostEffects::m_InfraredVisionGrainStrength, 0.5f, &u8_zero, &u8_max);
            ImGui::DragFloat("Filter Radius",      &CPostEffects::m_fInfraredVisionFilterRadius, 0.001f);
            ImGui::DragScalarN("Color",            ImGuiDataType_U8, &CPostEffects::m_InfraredVisionCol, 4);
            ImGui::DragScalarN("Main Color",       ImGuiDataType_U8, &CPostEffects::m_InfraredVisionMainCol, 4);
            ImGui::DragFloat4("Heat Object Color", *reinterpret_cast<float(*)[4]>(&CPostEffects::m_fInfraredVisionHeatObjectCol), 0.01f, 0.0, 1.0f);

            ImGui::TreePop();
        }

        ImGui::Checkbox("##Enable Heat Haze", &CPostEffects::m_bHeatHazeFX);
        ImGui::SameLine();
        if (ImGui::TreeNodeEx("Heat Haze FX", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding)) {
            const notsa::ui::ScopedDisable disabledScope{ !CPostEffects::m_bHeatHazeFX };

            const auto min = HEAT_HAZE_0;
            const auto max = (MAX_HEAT_HAZE_TYPES - 1);
        
            ImGui::DragScalar("Type",          ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXType, 0.05f, &min, &max);
            ImGui::DragScalar("Intensity",     ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXIntensity, 0.5f, &u8_zero, &u8_max);
            ImGui::DragScalar("Random Shift",  ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXRandomShift);
            ImGui::DragScalar("Speed Min",     ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXSpeedMin);
            ImGui::DragScalar("Speed Max",     ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXSpeedMax);
            ImGui::DragScalar("Scan Size X",   ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXScanSizeX);
            ImGui::DragScalar("Scan Size Y",   ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXScanSizeY);
            ImGui::DragScalar("Render Size X", ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXRenderSizeX);
            ImGui::DragScalar("Render Size Y", ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXRenderSizeY);

            ImGui::TreePop();
        };
    });
}
