#include <StdInc.h>
#include "./VehicleAudioEntityDebugModule.hpp"

using AE = CAEVehicleAudioEntity;

namespace notsa { 
namespace debugmodules {
void VehicleAudioEntityDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "Vehicle Audio Entity Debug Module", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }
    if (ImGui::BeginTabBar("MyTabBar")) {
        if (ImGui::BeginTabItem("Fields")) {
            RenderMemberVars();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Globals")) {
            RenderGlobalVars();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void VehicleAudioEntityDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra", "Audio", "Entities" }, [&] {
        ImGui::MenuItem("Vehicle", nullptr, &m_IsOpen);
    });
}

void VehicleAudioEntityDebugModule::Update() {
    if (auto* const veh = FindPlayerVehicle()) {
        m_Veh = veh;
    } else if (auto* const plyr = FindPlayerPed()) {
        if (auto* const closest = plyr->GetIntelligence()->GetVehicleScanner().GetClosestVehicleInRange()) {
            m_Veh = closest;
        }
    }
}

void VehicleAudioEntityDebugModule::RenderMemberVars() {
    if (!m_Veh) {
        ImGui::Text("No vehicle!");
        return;
    }

    auto* ae = &m_Veh->m_vehicleAudio;

    const auto FormattedText = []<typename... Args>(std::string_view fmt, Args&&... args) {
        char buf[1024];
        *std::vformat_to(buf, fmt, std::make_format_args(args)...) = 0;
        ImGui::Text(buf);
    };

    FormattedText("DoCountStalls: {}", ae->m_DoCountStalls);
    //FormattedText("AuSettings: {}", ae->m_AuSettings);
    FormattedText("IsInitialized: {}", ae->m_IsInitialized);
    FormattedText("IsPlayerDriver: {}", ae->m_IsPlayerDriver);
    FormattedText("IsPlayerPassenger: {}", ae->m_IsPlayerPassenger);
    FormattedText("IsPlayerDriverAboutToExit: {}", ae->m_IsPlayerDriverAboutToExit);
    FormattedText("IsWreckedVehicle: {}", ae->m_IsWreckedVehicle);
    FormattedText("State: {}", (int32)ae->m_State);
    FormattedText("AuGear: {}", ae->m_AuGear);
    FormattedText("CrzCount: {}", ae->m_CrzCount);
    FormattedText("IsSingleGear: {}", ae->m_IsSingleGear);
    FormattedText("RainDropCounter: {}", ae->m_RainDropCounter);
    FormattedText("StalledCount: {}", ae->m_StalledCount);
    FormattedText("SwapStalledTime: {}", ae->m_SwapStalledTime);
    FormattedText("IsSilentStalled: {}", ae->m_IsSilentStalled);
    FormattedText("IsHelicopterDisabled: {}", ae->m_IsHelicopterDisabled);
    FormattedText("IsHornOn: {}", ae->m_IsHornOn);
    FormattedText("IsSirenOn: {}", ae->m_IsSirenOn);
    FormattedText("IsFastSirenOn: {}", ae->m_IsFastSirenOn);
    FormattedText("HornVolume: {}", ae->m_HornVolume);
    FormattedText("HasSiren: {}", ae->m_HasSiren);
    FormattedText("TimeSplashLastTriggered: {}", ae->m_TimeSplashLastTriggered);
    FormattedText("TimeBeforeAllowAccelerate: {}", ae->m_TimeBeforeAllowAccelerate);
    FormattedText("TimeBeforeAllowCruise: {}", ae->m_TimeBeforeAllowCruise);
    FormattedText("EventVolume: {}", ae->m_EventVolume);
    FormattedText("DummyEngineBank: {}", ae->m_DummyEngineBank);
    FormattedText("PlayerEngineBank: {}", ae->m_PlayerEngineBank);
    FormattedText("DummySlot: {}", ae->m_DummySlot);
    //FormattedText("EngineSounds: {}", ae->m_EngineSounds);
    FormattedText("TimeLastServiced: {}", ae->m_TimeLastServiced);
    FormattedText("ACPlayPositionThisFrame: {}", ae->m_ACPlayPositionThisFrame);
    FormattedText("ACPlayPositionLastFrame: {}", ae->m_ACPlayPositionLastFrame);
    FormattedText("FramesAgoACLooped: {}", ae->m_FramesAgoACLooped);
    FormattedText("ACPlayPercentWhenStopped: {}", ae->m_ACPlayPercentWhenStopped);
    FormattedText("TimeACStopped: {}", ae->m_TimeACStopped);
    FormattedText("ACPlayPositionWhenStopped: {}", ae->m_ACPlayPositionWhenStopped);

    FormattedText("SurfaceSoundType: {}", ae->m_SurfaceSoundType);
    FormattedText("SurfaceSound: 0x{:X}", LOG_PTR(ae->m_SurfaceSound));

    FormattedText("RoadNoiseSoundType: {}", ae->m_RoadNoiseSoundType);
    FormattedText("RoadNoiseSound: 0x{:X}", LOG_PTR(ae->m_RoadNoiseSound));

    FormattedText("FlatTireSoundType: {}", ae->m_FlatTireSoundType);
    FormattedText("FlatTireSound: 0x{:X}", LOG_PTR(ae->m_FlatTireSound));

    FormattedText("ReverseGearSoundType: {}", ae->m_ReverseGearSoundType);
    FormattedText("ReverseGearSound: 0x{:X}", LOG_PTR(ae->m_ReverseGearSound));

    FormattedText("HornSoundType: {}", ae->m_HornSoundType);
    FormattedText("HornSound: 0x{:X}", LOG_PTR(ae->m_HornSound));

    FormattedText("SirenSound: 0x{:X}", LOG_PTR(ae->m_SirenSound));
    FormattedText("FastSirenSound: 0x{:X}", LOG_PTR(ae->m_FastSirenSound));

    //FormattedText("SkidSound: {}", ae->m_SkidSound);

    FormattedText("CurrentRotorFrequency: {}", ae->m_CurrentRotorFrequency);
    FormattedText("CurrentDummyEngineVolume: {}", ae->m_CurrentDummyEngineVolume);
    FormattedText("CurrentDummyEngineFrequency: {}", ae->m_CurrentDummyEngineFrequency);
    FormattedText("MovingPartSmoothedSpeed: {}", ae->m_MovingPartSmoothedSpeed);
    FormattedText("FadeIn: {}", ae->m_FadeIn);
    FormattedText("FadeOut: {}", ae->m_FadeOut);
    FormattedText("bNitroOnLastFrame: {}", ae->m_bNitroOnLastFrame);
    FormattedText("CurrentNitroRatio: {}", ae->m_CurrentNitroRatio);

    ImGui::Text("State");
}

void VehicleAudioEntityDebugModule::RenderGlobalVars() {
    auto* const cfg = &AE::s_Config;

    if (ImGui::TreeNode("Dummy Engine")) {
        if (ImGui::Button("Reset")) {
            cfg->DummyEngine = AE::s_DefaultConfig.DummyEngine;
        }

        ImGui::SliderFloat("VolumeUnderwaterOffset", &cfg->DummyEngine.VolumeUnderwaterOffset, -100.f, 100.f);
        ImGui::SliderFloat("VolumeTrailerOffset", &cfg->DummyEngine.VolumeTrailerOffset, -100.f, 100.f);

        ImGui::SliderFloat("FreqUnderwaterFactor", &cfg->DummyEngine.FreqUnderwaterFactor, -100, 100.f);

        if (ImGui::TreeNode("Idle")) {
            if (ImGui::Button("Reset")) {
                cfg->DummyEngine.Idle = AE::s_DefaultConfig.DummyEngine.Idle;
            }

            ImGui::SliderFloat("Ratio", &cfg->DummyEngine.Idle.Ratio, -1.f, 1.f);

            ImGui::SliderFloat("VolumeBase", &cfg->DummyEngine.Idle.VolumeBase, -100.f, 100.f);
            ImGui::SliderFloat("VolumeMax", &cfg->DummyEngine.Idle.VolumeMax, -100.f, 100.f);

            ImGui::SliderFloat("FreqBase", &cfg->DummyEngine.Idle.FreqBase, -100, 100.f);
            ImGui::SliderFloat("FreqMax", &cfg->DummyEngine.Idle.FreqMax, -100, 100.f);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Rev")) {
            if (ImGui::Button("Reset")) {
                cfg->DummyEngine.Rev = AE::s_DefaultConfig.DummyEngine.Rev;
            }

            ImGui::SliderFloat("Ratio", &cfg->DummyEngine.Rev.Ratio, -1.f, 1.f);

            ImGui::SliderFloat("VolumeBase", &cfg->DummyEngine.Rev.VolumeBase, -100.f, 100.f);
            ImGui::SliderFloat("VolumeMax", &cfg->DummyEngine.Rev.VolumeMax, -100.f, 100.f);

            ImGui::SliderFloat("FreqBase", &cfg->DummyEngine.Rev.FreqBase, -100, 100.f);
            ImGui::SliderFloat("FreqMax", &cfg->DummyEngine.Rev.FreqMax, -100, 100.f);

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}
}; // namespace debugmodules
}; // namespace notsa
