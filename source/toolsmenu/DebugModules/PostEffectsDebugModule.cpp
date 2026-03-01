#include "StdInc.h"

#include "PostEffectsDebugModule.h"
#include "PostEffects.h"

using namespace ImGui;

void PostEffectsDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Settings", "Post / Special FX" }, [&] {
        Checkbox("In Cutscene",            &CPostEffects::m_bInCutscene          );
        Checkbox("Skip Post Process",      &CPostEffects::m_bDisableAllPostEffect);
        Checkbox("Save Photo From Script", &CPostEffects::m_bSavePhotoFromScript );
        Checkbox("Radiosity",              &CPostEffects::m_bRadiosity           );
        Checkbox("Night Vision",           &CPostEffects::m_bNightVision         );
        Checkbox("Grain",                  &CPostEffects::m_bGrainEnable         );
        Checkbox("Darkness Filter",        &CPostEffects::m_bDarknessFilter      );
        Checkbox("CCTV",                   &CPostEffects::m_bCCTV                );
        Checkbox("SpeedFX Test Mode",      &CPostEffects::m_bSpeedFXTestMode     );
        Checkbox("Fog",                    &CPostEffects::m_bFog                 );
        Checkbox("Water Depth Darkness",   &CPostEffects::m_bWaterDepthDarkness  );
        Checkbox("Color Correction",       &CPostEffects::m_bColorEnable         );

        Separator();

        Checkbox("SpecialFX - Video Cam", &CSpecialFX::bVideoCam);
        Checkbox("SpecialFX - Lift Cam",  &CSpecialFX::bLiftCam);

        if (Button("SpecialFX - Activate Snapshot")) {
            CSpecialFX::bSnapShotActive = true;
            CSpecialFX::SnapShotFrames  = 0;
        }

        const auto u8_zero = 0, u8_max = 255;

        Checkbox("##Enable Infrared", &CPostEffects::m_bInfraredVision);
        SameLine();
        if (TreeNodeEx("Infrared Vision", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding)) {
            const notsa::ui::ScopedDisable disabledScope{ !CPostEffects::m_bInfraredVision };

            DragScalar("Grain Strength",    ImGuiDataType_S32, &CPostEffects::m_InfraredVisionGrainStrength, 0.5f, &u8_zero, &u8_max);
            DragFloat("Filter Radius",      &CPostEffects::m_fInfraredVisionFilterRadius, 0.001f);
            DragScalarN("Color",            ImGuiDataType_U8, &CPostEffects::m_InfraredVisionCol, 4);
            DragScalarN("Main Color",       ImGuiDataType_U8, &CPostEffects::m_InfraredVisionMainCol, 4);
            DragFloat4("Heat Object Color", *reinterpret_cast<float(*)[4]>(&CPostEffects::m_fInfraredVisionHeatObjectCol), 0.01f, 0.0, 1.0f);

            TreePop();
        }

        Checkbox("##Enable Heat Haze", &CPostEffects::m_bHeatHazeFX);
        SameLine();
        if (TreeNodeEx("Heat Haze FX", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding)) {
            const notsa::ui::ScopedDisable disabledScope{ !CPostEffects::m_bHeatHazeFX };

            const auto min = HEAT_HAZE_0;
            const auto max = (MAX_HEAT_HAZE_TYPES - 1);
        
            DragScalar("Type",          ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXType, 0.05f, &min, &max);
            DragScalar("Intensity",     ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXIntensity, 0.5f, &u8_zero, &u8_max);
            DragScalar("Random Shift",  ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXRandomShift);
            DragScalar("Speed Min",     ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXSpeedMin);
            DragScalar("Speed Max",     ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXSpeedMax);
            DragScalar("Scan Size X",   ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXScanSizeX);
            DragScalar("Scan Size Y",   ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXScanSizeY);
            DragScalar("Render Size X", ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXRenderSizeX);
            DragScalar("Render Size Y", ImGuiDataType_U32, &CPostEffects::m_HeatHazeFXRenderSizeY);

            TreePop();
        };
    });
}
