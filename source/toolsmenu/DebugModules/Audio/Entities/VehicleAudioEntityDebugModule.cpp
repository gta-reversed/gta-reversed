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
    m_Veh = FindTargetVehicle();
}

void VehicleAudioEntityDebugModule::RenderMemberVars() {
    if (!m_Veh) {
        ImGui::Text("No vehicle!");
        return;
    }

    auto* ae = &m_Veh->m_vehicleAudio;

    const auto FormattedText = []<typename... Args>(std::string_view fmt, Args&&... args) {
        char buf[1024];
        *std::vformat_to(buf, fmt, std::make_format_args(args...)) = 0;
        ImGui::Text(buf);
    };

    FormattedText("State: {}", (AE::eAEState)(ae->m_State));
    FormattedText("DoCountStalls: {}", ae->m_DoCountStalls);
    FormattedText("IsInitialized: {}", ae->m_IsInitialized);
    FormattedText("IsPlayerDriver: {}", ae->m_IsPlayerDriver);
    FormattedText("IsPlayerPassenger: {}", ae->m_IsPlayerPassenger);
    FormattedText("IsPlayerDriverAboutToExit: {}", ae->m_IsPlayerDriverAboutToExit);
    FormattedText("IsWreckedVehicle: {}", ae->m_IsWreckedVehicle);
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
    if (auto* const as = &ae->m_AuSettings; ImGui::TreeNode("AuSettings")) {
        FormattedText("VehicleAudioType: {}", as->VehicleAudioType);
        FormattedText("PlayerBank: {}", as->PlayerBank);
        FormattedText("DummyBank: {}", as->DummyBank);
        FormattedText("BassSetting: {}", as->BassSetting);
        FormattedText("BassFactor: {}", as->BassFactor);
        FormattedText("EnginePitch: {}", as->EnginePitch);
        FormattedText("HornType: {}", as->HornType);
        FormattedText("HornPitch: {}", as->HornPitch);
        FormattedText("DoorType: {}", as->DoorType);
        FormattedText("EngineUpgrade: {}", as->EngineUpgrade);
        FormattedText("RadioStation: {}", as->RadioStation);
        FormattedText("RadioType: {}", as->RadioType);
        FormattedText("VehicleAudioTypeForName: {}", (int32)(as->VehicleAudioTypeForName));
        FormattedText("EngineVolumeOffset: {}", as->EngineVolumeOffset);

        ImGui::TreePop();
    }
    if (ImGui::TreeNode("EngineSounds")) {
        const auto SoundTypeToString = [ae](AE::eVehicleEngineSoundType st) -> std::optional<const char*> {
            switch (ae->m_AuSettings.VehicleAudioType) {
            case AE_CAR:
                return EnumToString((AE::eCarEngineSoundType)(st));
            case AE_AIRCRAFT_HELICOPTER:
            case AE_AIRCRAFT_PLANE:
            case AE_AIRCRAFT_SEAPLANE:
                return EnumToString((AE::eAircraftSoundType)(st));
            }
            return std::nullopt;
        };
        for (auto&& [i, es] : notsa::enumerate(ae->m_EngineSounds)) {
            const auto st = SoundTypeToString((AE::eVehicleEngineSoundType)(i)).value_or("<unknown>");
            if (const auto* const s = es.Sound) {
                FormattedText(
                    "[{:<30}]: V: {:<5.2f}, F: {:<5.2f}",
                    st,
                    s->GetVolume(),
                    s->GetSpeed()
                );
            } else {
                FormattedText("[{:<30}]: -", st);
            }
        }
        ImGui::TreePop();
    }
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
}

void VehicleAudioEntityDebugModule::RenderGlobalVars() {
    //auto* const cfg = &AE::s_Config;
    //
    //if (ImGui::Button("Reset")) {
    //    *cfg = AE::s_DefaultConfig;
    //}
    //
    //ImGui::SliderFloat("FreqUnderwaterFactor", &cfg->FreqUnderwaterFactor, -100, 100.f);
    //
    //if (auto* const dws = &cfg->DriveWheelSkid; ImGui::TreeNode("DriveWheelSkid")) {
    //    ImGui::Checkbox("Enabled", &dws->Enabled);
    //    ImGui::DragFloat("SpinningFactor", &dws->SpinningFactor, 0.005f, 0.f, 1.f);
    //    ImGui::DragFloat("SkiddingFactor", &dws->SkiddingFactor, 0.005f, 0.f, 1.f);
    //    ImGui::DragFloat("StationaryFactor", &dws->StationaryFactor, 0.005f, 0.f, 1.f);
    //
    //    ImGui::TreePop();
    //}
    //
    //if (auto* const de = &cfg->DummyEngine; ImGui::TreeNode("Dummy Engine")) {
    //    if (ImGui::Button("Reset")) {
    //        cfg->DummyEngine = AE::s_DefaultConfig.DummyEngine;
    //    }
    //
    //    ImGui::SliderFloat("VolumeUnderwaterOffset", &cfg->DummyEngine.VolumeUnderwaterOffset, -100.f, 100.f);
    //    ImGui::SliderFloat("VolumeTrailerOffset", &cfg->DummyEngine.VolumeTrailerOffset, -100.f, 100.f);
    //
    //    if (ImGui::TreeNode("Idle")) {
    //        if (ImGui::Button("Reset")) {
    //            de->ID = AE::s_DefaultConfig.DummyEngine.ID;
    //        }
    //
    //        ImGui::SliderFloat("Ratio", &de->ID.Ratio, -1.f, 1.f);
    //
    //        ImGui::SliderFloat("VolumeBase", &de->ID.VolumeBase, -100.f, 100.f);
    //        ImGui::SliderFloat("VolumeMax", &de->ID.VolumeMax, -100.f, 100.f);
    //
    //        ImGui::SliderFloat("FreqBase", &de->ID.FreqBase, -100, 100.f);
    //        ImGui::SliderFloat("FreqMax", &de->ID.FreqMax, -100, 100.f);
    //
    //        ImGui::TreePop();
    //    }
    //
    //    if (ImGui::TreeNode("Rev")) {
    //        if (ImGui::Button("Reset")) {
    //            de->Rev = AE::s_DefaultConfig.DummyEngine.Rev;
    //        }
    //
    //        ImGui::SliderFloat("Ratio", &de->Rev.Ratio, -1.f, 1.f);
    //
    //        ImGui::SliderFloat("VolumeBase", &de->Rev.VolumeBase, -100.f, 100.f);
    //        ImGui::SliderFloat("VolumeMax", &de->Rev.VolumeMax, -100.f, 100.f);
    //
    //        ImGui::SliderFloat("FreqBase", &de->Rev.FreqBase, -100, 100.f);
    //        ImGui::SliderFloat("FreqMax", &de->Rev.FreqMax, -100, 100.f);
    //
    //        ImGui::TreePop();
    //    }
    //
    //    if (ImGui::TreeNode("GolfCart")) {
    //        ImGui::DragFloat("FrqMin", &de->GolfCart.FrqMin, 0.005f, 0.f, 1.f);
    //        ImGui::DragFloat("FrqMax", &de->GolfCart.FrqMax, 0.005f, 0.f, 1.f);
    //        ImGui::DragFloat("VolOffset", &de->GolfCart.VolOffset, 0.005f, 0.f, 1.f);
    //
    //        ImGui::TreePop();
    //    }
    //
    //    ImGui::TreePop();
    //}
    //
    //if (auto* const pe = &cfg->PlayerEngine; ImGui::TreeNode("Player Engine")) {
    //    ImGui::DragFloat("CrzSpeedOffset", &pe->CrzSpeedOffset, 1.f, -100.f, 100.f);
    //    ImGui::DragInt("CrzMaxCnt", &pe->CrzMaxCnt, 1.f, -100, 100);
    //    ImGui::DragFloat("ZMoveSpeedThreshold", &pe->ZMoveSpeedThreshold, 1.f, -100.f, 100.f);
    //    ImGui::DragInt("MaxAuGear", &pe->MaxAuGear, 1.f, -100, 100);
    //    ImGui::DragFloat("NitroFactor", &pe->VolNitroFactor, 1.f, -100.f, 100.f);
    //    ImGui::DragFloat("SingleGearVolume", &pe->SingleGearVolume, 1.f, -100.f, 100.f);
    //    ImGui::DragFloat("VolNitroFactor", &pe->VolNitroFactor, 1.f, -100.f, 100.f);
    //
    //    ImGui::DragFloat("FrqNitroFactor", &pe->FrqNitroFactor, 1.f, -100.f, 100.f);
    //    ImGui::DragFloat("FrqWheelSpinFactor", &pe->FrqWheelSpinFactor, 1.f, -100.f, 100.f);
    //    ImGui::DragFloat("FrqBikeLeanFactor", &pe->FrqBikeLeanFactor, 1.f, -100.f, 100.f);
    //    ImGui::DragFloat("FrqPlayerBikeBoostOffset", &pe->FrqPlayerBikeBoostOffset, 1.f, -100.f, 100.f);
    //    ImGui::DragFloat("FrqZMoveSpeedLimitMin", &pe->FrqZMoveSpeedLimitMin, 1.f, -100.f, 100.f);
    //    ImGui::DragFloat("FrqZMoveSpeedLimitMax", &pe->FrqZMoveSpeedLimitMax, 1.f, -100.f, 100.f);
    //    ImGui::DragFloat("FrqZMoveSpeedFactor", &pe->FrqZMoveSpeedFactor, 1.f, -100.f, 100.f);
    //
    //    if (ImGui::TreeNode("Rev")) {
    //        ImGui::DragFloat("FrqWheelSpinFactor", &pe->Rev.FrqWheelSpinFactor, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("FrqOffset", &pe->Rev.FrqOffset, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("FrqMin", &pe->Rev.FrqMin, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("FrqMax", &pe->Rev.FrqMax, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("VolMin", &pe->Rev.VolMin, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("VolMax", &pe->Rev.VolMax, 1.f, -100.f, 100.f);
    //        ImGui::TreePop();
    //    }
    //    if (ImGui::TreeNode("ID")) {
    //        ImGui::DragFloat("FrqMin", &pe->ID.FrqMin, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("FrqMax", &pe->ID.FrqMax, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("VolMin", &pe->ID.VolMin, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("VolMax", &pe->ID.VolMax, 1.f, -100.f, 100.f);
    //        ImGui::TreePop();
    //    }
    //    if (ImGui::TreeNode("Crz")) {
    //        ImGui::DragFloat("FrqSingleGear", &pe->Crz.FrqSingleGear, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("FrqMin", &pe->Crz.FrqMin, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("FrqMax", &pe->Crz.FrqMax, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("VolMin", &pe->Crz.VolMin, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("VolMax", &pe->Crz.VolMax, 1.f, -100.f, 100.f);
    //        ImGui::TreePop();
    //    }
    //    if (ImGui::TreeNode("AC")) {
    //        if (ImGui::TreeNode("FrqPerGearFactor")) {
    //            for (auto&& [i, gf] : notsa::enumerate(pe->AC.FrqPerGearFactor)) {
    //                char buf[1024];
    //                *std::format_to(buf, "Gear {}", i) = 0;
    //                ImGui::DragFloat(buf, &gf, 1.f, -100.f, 100.f);
    //            }
    //            ImGui::TreePop();
    //        }
    //        ImGui::DragFloat("FrqSingleGear", &pe->AC.FrqSingleGear, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("FrqMultiGearOffset", &pe->AC.FrqMultiGearOffset, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("VolMin", &pe->AC.VolMin, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("VolMax", &pe->AC.VolMax, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("WheelSpinThreshold", &pe->AC.WheelSpinThreshold, 0.005f, 0.f, 1.f);
    //        ImGui::DragFloat("InhibitForLowSpeedLimit", &pe->AC.InhibitForLowSpeedLimit, 1.f, -100.f, 100.f);
    //        ImGui::DragFloat("SpeedOffset", &pe->AC.SpeedOffset, 1.f, -100.f, 100.f);
    //        ImGui::TreePop();
    //    }
    //    ImGui::TreePop();
    //}
}
CVehicle* VehicleAudioEntityDebugModule::FindTargetVehicle() const noexcept {
    if (auto* const veh = FindPlayerVehicle()) {
        return veh;
    }
    if (auto* const plyr = FindPlayerPed()) {
        if (auto* const on = plyr->m_pContactEntity; on && on->IsVehicle()) {
            return on->AsVehicle();
        }
        if (auto* const closest = plyr->GetIntelligence()->GetVehicleScanner().GetClosestVehicleInRange()) {
            return closest;
        }
    }
    return m_Veh; // Unchanged
}
}; // namespace debugmodules
}; // namespace notsa
