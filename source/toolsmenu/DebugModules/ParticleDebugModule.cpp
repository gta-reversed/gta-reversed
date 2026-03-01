#include "StdInc.h"

#include "ParticleDebugModule.h"
#include "imgui.h"

using namespace ImGui;

constexpr static const char* FX_PARTICLES[] = {
    "blood_heli", "boat_prop", "camflash", "carwashspray", "cement", "cloudfast", "coke_puff", "coke_trail", "cigarette_smoke", "explosion_barrel", "explosion_crate",
    "explosion_door", "exhale", "explosion_fuel_car", "explosion_large", "explosion_medium", "explosion_molotov", "explosion_small", "explosion_tiny", "extinguisher", "flame",
    "fire", "fire_med", "fire_large", "flamethrower", "fire_bike", "fire_car", "gunflash", "gunsmoke", "insects", "heli_dust", "jetpack", "jetthrust", "nitro", "molotov_flame",
    "overheat_car", "overheat_car_electric", "prt_boatsplash", "prt_cardebris", "prt_collisionsmoke", "prt_glass", "prt_gunshell", "prt_sand", "prt_sand2", "prt_smokeII_3_expand",
    "prt_smoke_huge", "prt_spark", "prt_spark_2", "prt_splash", "prt_wake", "prt_watersplash", "prt_wheeldirt", "petrolcan", "puke", "riot_smoke", "spraycan", "smoke30lit",
    "smoke30m", "smoke50lit", "shootlight", "smoke_flare", "tank_fire", "teargas", "teargasAD", "tree_hit_fir", "tree_hit_palm", "vent", "vent2", "water_hydrant", "water_ripples",
    "water_speed", "water_splash", "water_splash_big", "water_splsh_sml", "water_swim", "waterfall_end", "water_fnt_tme", "water_fountain", "wallbust", "WS_factorysmoke"
};

void ParticleDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "Particles", {860.f, 290.f}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    BeginGroup();

    static bool ignoreBC{false};
    Checkbox("Ignore bound checks", &ignoreBC);

    if (Button("Random Fx here")) {
        auto* fx = g_fxMan.CreateFxSystem(
            CGeneral::RandomChoice(FX_PARTICLES),
            FindPlayerCoors(PED_TYPE_PLAYER1),
            nullptr,
            ignoreBC
        );

        if (fx)
            fx->PlayAndKill();
    }

    Separator();

    EndGroup();
}

void ParticleDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Particles", nullptr, &m_IsOpen);
    });
}
