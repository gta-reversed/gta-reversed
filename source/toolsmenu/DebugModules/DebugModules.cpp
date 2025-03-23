#include "StdInc.h"

#include <fstream>

#include "DebugModules.h"
#include "imgui.h"

#include "./CollisionDebugModule.h"
#include "./CheatDebugModule.h"
#include "./PedDebugModule.h"
#include "./Script/MissionDebugModule.h"
#include "./Audio/CutsceneTrackManagerDebugModule.h"
#include "./Audio/AmbienceTrackManagerDebugModule.h"
#include "./Audio/PoliceScannerAudioEntityDebugModule.h"
#include "./Audio/UserRadioTrackDebugModule.h"
#include "./CStreamingDebugModule.h"
#include "./CPickupsDebugModule.h"
#include "./CDarkelDebugModule.h"
#include "./HooksDebugModule.h"
#include "./CTeleportDebugModule.h"
#include "./ParticleDebugModule.h"
#include "./PostEffectsDebugModule.h"
#include "./PoolsDebugModule.h"
#include "./TimeCycleDebugModule.h"
#include "./CullZonesDebugModule.h"
#include "./TextDebugModule.h"
#include "./ProcObjectDebugModule.h"
#include "./Spawner/SpawnerDebugModule.hpp"
#include "./ImGuiDebugModule.hpp"
#include "./ScriptDebugModule.hpp"
#include "./CloudsDebugModule.hpp"
#include "./AudioZonesDebugModule.h"
#include "./WeaponDebugModule.hpp"
#include "./CheckpointsDebugModule.hpp"
#include "./TwoDEffectsDebugModule.hpp"
#include "./VehicleInfoDebugModule.h"
#include "./CoverPointsDebugModule.hpp"

DebugModules::DebugModules(ImGuiContext* ctx) :
    m_ImCtx(ctx)
{
    CreateModules();
}

DebugModules::~DebugModules() {
    DoSerializeModules();
}

void DebugModules::PreRenderUpdate() {
    for (auto& module : m_Modules) {
        module->Update();
    }

    // Handle DebugModule serialization
    const auto now = CTimer::GetTimeInMS();
    if ((now - m_LastSerializationTimeMs) >= MODULE_SERIALIZATION_INTERVAL_MS) {
        DoSerializeModules();
        m_LastSerializationTimeMs = now;
    }
}

void DebugModules::Render2D() {
    ImGui::BeginMainMenuBar();
    for (auto& module : m_Modules) {
        module->RenderMenuEntry();
    }
    RenderMenuBarInfo();
    ImGui::EndMainMenuBar();

    for (auto& module : m_Modules) {
        module->RenderWindow();
    }
}

void DebugModules::Render3D() {
    ZoneScoped;

    for (auto& module : m_Modules) {
        module->Render3D();
    }
}

void DebugModules::CreateModules() {
    // "Tools" menu
    Add<TeleportDebugModule>();
    Add<SpawnerDebugModule>();
    Add<MissionDebugModule>();
    Add<CheatDebugModule>();

    // "Settings" menu
    Add<HooksDebugModule>();
    Add<PostEffectsDebugModule>();

    // "Visualization" menu
    Add<CollisionDebugModule>();
    Add<PedDebugModule>();

    // "Stats" menu
    Add<PoolsDebugModule>();
    Add<CStreamingDebugModule>();

    // "Extra" menu (Put your extra debug modules here, unless they might be useful in general)
    Add<DarkelDebugModule>();
    Add<CPickupsDebugModule>();
    Add<PoliceScannerAudioEntityDebugModule>();
    Add<AmbienceTrackManagerDebugModule>();
    Add<CutsceneTrackManagerDebugModule>();
    Add<UserRadioTrackDebugModule>();
    Add<notsa::debugmodules::ScriptDebugModule>();
    Add<notsa::debugmodules::CloudsDebugModule>();
    Add<notsa::debugmodules::WeaponDebugModule>();
    Add<ParticleDebugModule>();
    Add<TextDebugModule>();
    Add<notsa::debugmodules::CheckpointsDebugModule>();
    Add<ProcObjectDebugModule>();
    Add<VehicleInfoDebugModule>();

    // Stuff that is present in multiple menus
    Add<notsa::debugmodules::TwoDEffectsDebugModule>(); // Visualization + Extra
    Add<notsa::debugmodules::CoverPointsDebugModule>(); // Visualization + Extra
    Add<TimeCycleDebugModule>(); // Visualization + Extra
    Add<CullZonesDebugModule>(); // Visualization + Extra
    Add<COcclusionDebugModule>(); // Visualization + Extra
    Add<AudioZonesDebugModule>(); // Visualization + Extra
    Add<notsa::debugmodules::ImGuiDebugModule>(); // Stats + Extra

    // Restore state of modules
    DoDeserializeModules();
}

void DebugModules::RenderMenuBarInfo() {
    const auto& io = ImGui::GetIO();

    ImGui::SameLine(ImGui::GetWindowWidth() - 280.f);

    // Draw this first, cause FPS fluctuates (and would move this text)
    ImGui::Text("F7 / Ctrl + M |");

    // TODO: V-Sync (Use IsVSyncActive()) - How to get VSync target FPS?
    //       Can't use `RsGlobal.frameLimit`, because there's an active vsync limit (coming from somewhere lol)
    const auto MaxFrameRate = FrontEndMenuManager.m_bPrefsFrameLimiter ? (float)RsGlobal.frameLimit : 60.f;
    const auto FrameRateProg = std::max(invLerp(MaxFrameRate * 0.30f, MaxFrameRate, io.Framerate), 0.f);
    ImGui::PushStyleColor(ImGuiCol_Text, { std::max(0.f, 1.f - FrameRateProg), std::min(1.f, FrameRateProg), 0.f, 1.f });
    ImGui::Text("%.1f FPS [%.2f ms]", io.Framerate, io.Framerate ? 1000.f / io.Framerate : 0.f); // Calculate frametime from framerate (to make the next less wobbly as the io.DeltaTime varies a lot otherwise)
    ImGui::PopStyleColor();
}

void DebugModules::DoSerializeModules() {
    // Serialize all modules
    json states{};
    for (const auto& m : m_Modules) {
        const auto id = m->GetID();
        if (id.empty()) {
            continue;
        }
        states[id] = m->Serialize();
    }

    // Save to file
    std::ofstream outf{"DebugModules.json"};
    outf << states;
}

void DebugModules::DoDeserializeModules() {
    json states;

    // Load from file
    {
        std::ifstream inf{"DebugModules.json"};
        if (inf.fail()) {
            return; // File doesn't exist, etc
        }
        try {
            inf >> states;
        } catch (const json::exception& e) {
            const fs::path bakFileName{std::format("DebugModules-{}.json.bak", time(nullptr))};
            fs::copy("DebugModules.json", bakFileName);
            NOTSA_LOG_ERR("Error while loading `DebugModules.json`: {}", e.what());
            NOTSA_LOG_ERR("Backing up corrupted file to {} and creating a new one.", bakFileName.string());
            return;
        }
    }

    // Deserialize modules
    for (const auto& m : m_Modules) {
        const auto id = m->GetID();

        // If no ID we don't serialize the module
        if (id.empty()) {
            continue;
        }

        // This module might not have been serialized yet, but that's all good
        const auto s = states.find(id);
        if (s == states.end()) {
            continue;
        }

        // Now deserialize... We handle exceptions too, because otherwise we get a weird crash in a dll we don't have pdb's for????
        try {
            m->Deserialize(*s);
        } catch (const json::exception& e) {
            NOTSA_LOG_ERR("JSON exception occurred while deserializing module `{}`: {}", id, e.what());
        }
    }
}
