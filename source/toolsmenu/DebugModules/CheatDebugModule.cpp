#include "StdInc.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "CheatDebugModule.h"

void CheatDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Settings" }, [&] {
        ImGui::MenuItem("Cheats", nullptr, &m_IsOpen);
    });
}

void CheatDebugModule::Update() {
    if (!m_GodMode) {
        return;
    }
    if (const auto plyr = FindPlayerPed()) {
        plyr->m_fHealth = plyr->m_fMaxHealth;
        plyr->m_fArmour = 100.f;
    }
}

void CheatDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "Cheats", { 500.f, 700.f }, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    // TODO: Use this instead of the below copy pasted code (So the radio buttons actually have a state)..
    //const auto DoCheat = [](eCheats cheat, const char* name) {
    //    if (ImGui::RadioButton(name, CCheat::IsActive(cheat))) {
    //        CCheat::ApplyCheat(cheat);
    //    }
    //};

    if (ImGui::CollapsingHeader("Player")) {
        ImGui::Checkbox("God Mode", &m_GodMode);
        if (ImGui::RadioButton("Adrenaline", false)) {
            CCheat::AdrenalineCheat();
        }
        if (ImGui::RadioButton("Health Money Armour", false)) {
            CCheat::MoneyArmourHealthCheat();
        }
        if (ImGui::RadioButton("Weapon 1", false)) {
            CCheat::WeaponCheat1();
        }
        if (ImGui::RadioButton("Weapon 2", false)) {
            CCheat::WeaponCheat2();
        }
        if (ImGui::RadioButton("Weapon 3", false)) {
            CCheat::WeaponCheat3();
        }
        if (ImGui::RadioButton("Weapon 4", false)) {
            CCheat::WeaponCheat4();
        }
        if (ImGui::RadioButton("Weapon Skills", false)) {
            CCheat::WeaponSkillsCheat();
        }
        if (ImGui::RadioButton("Never wanted", false)) {
            CCheat::NotWantedCheat();
        }
        if (ImGui::RadioButton("Wanted", false)) {
            CCheat::WantedCheat();
        }
        if (ImGui::RadioButton("Wanted Up", false)) {
            CCheat::WantedLevelUpCheat();
        }
        if (ImGui::RadioButton("Wanted Down", false)) {
            CCheat::WantedLevelDownCheat();
        }
        if (ImGui::RadioButton("Health", false)) {
            CCheat::HealthCheat();
        }
        if (ImGui::RadioButton("Muscle", false)) {
            CCheat::MuscleCheat();
        }
        if (ImGui::RadioButton("Skinny", false)) {
            CCheat::SkinnyCheat();
        }
        if (ImGui::RadioButton("Fat", false)) {
            CCheat::FatCheat();
        }
        if (ImGui::RadioButton("Stamina", false)) {
            CCheat::StaminaCheat();
        }
        if (ImGui::RadioButton("Vehicle Skills", false)) {
            CCheat::VehicleSkillsCheat();
        }
        if (ImGui::RadioButton("Big Head aka Jimmy Neutron", false)) {
            CCheat::BigHeadCheat();
        }
        if (ImGui::RadioButton("Thin Body", false)) {
            CCheat::ThinBodyCheat();
        }
    }

    if (ImGui::CollapsingHeader("Vehicle")) {
        if (ImGui::RadioButton("Apache", false)) {
            CCheat::ApacheCheat();
        }
        if (ImGui::RadioButton("Vortex", false)) {
            CCheat::VortexCheat();
        }
        if (ImGui::RadioButton("Dozer", false)) {
            CCheat::DozerCheat();
        }
        if (ImGui::RadioButton("Flyboy", false)) {
            CCheat::FlyboyCheat();
        }
        if (ImGui::RadioButton("Golf Cart", false)) {
            CCheat::GolfcartCheat();
        }
        if (ImGui::RadioButton("Hearse", false)) {
            CCheat::HearseCheat();
        }
        if (ImGui::RadioButton("Love fist", false)) {
            CCheat::LovefistCheat();
        }
        if (ImGui::RadioButton("Monster Truck", false)) {
            CCheat::MonsterTruckCheat();
        }
        if (ImGui::RadioButton("Quad", false)) {
            CCheat::QuadCheat();
        }
        if (ImGui::RadioButton("Stock Car 1", false)) {
            CCheat::StockCarCheat();
        }
        if (ImGui::RadioButton("Stock Car 2", false)) {
            CCheat::StockCar2Cheat();
        }
        if (ImGui::RadioButton("Stock Car 3", false)) {
            CCheat::StockCar3Cheat();
        }
        if (ImGui::RadioButton("Stock Car 4", false)) {
            CCheat::StockCar4Cheat();
        }
        if (ImGui::RadioButton("Stunt Plane", false)) {
            CCheat::StuntPlaneCheat();
        }
        if (ImGui::RadioButton("Tank", false)) {
            CCheat::TankCheat();
        }
        if (ImGui::RadioButton("Tanker", false)) {
            CCheat::TankerCheat();
        }
        if (ImGui::RadioButton("Trashmaster", false)) {
            CCheat::TrashmasterCheat();
        }
    }

    if (ImGui::CollapsingHeader("Weather")) {
        if (ImGui::RadioButton("Cloudy Weather", false)) {
            CCheat::CloudyWeatherCheat();
        }
        if (ImGui::RadioButton("Extra Sunny Weather", false)) {
            CCheat::ExtraSunnyWeatherCheat();
        }
        if (ImGui::RadioButton("Sunny Weather", false)) {
            CCheat::SunnyWeatherCheat();
        }
        if (ImGui::RadioButton("Fast time", false)) {
            CCheat::FastTimeCheat();
        }
        if (ImGui::RadioButton("Foggy Weather", false)) {
            CCheat::FoggyWeatherCheat();
        }
        if (ImGui::RadioButton("Rainy Weather", false)) {
            CCheat::RainyWeatherCheat();
        }
        if (ImGui::RadioButton("Sandstorm", false)) {
            CCheat::SandstormCheat();
        }
        if (ImGui::RadioButton("Slow Time", false)) {
            CCheat::SlowTimeCheat();
        }
        if (ImGui::RadioButton("Storm", false)) {
            CCheat::StormCheat();
        }
        if (ImGui::RadioButton("Dusk", false)) {
            CCheat::DuskCheat();
        }
    }

    if (ImGui::CollapsingHeader("Misc")) {
        if (ImGui::RadioButton("Blow Up Cars", false)) {
            CCheat::BlowUpCarsCheat();
        }
        if (ImGui::RadioButton("Jetpack", false)) {
            CCheat::JetpackCheat();
        }
        if (ImGui::RadioButton("DriveBy", false)) {
            CCheat::DrivebyCheat();
        }
        if (ImGui::RadioButton("All Cars Are Great", false)) {
            CCheat::AllCarsAreGreatCheat();
        }
        if (ImGui::RadioButton("All Cars Are Shit", false)) {
            CCheat::AllCarsAreShitCheat();
        }
        if (ImGui::RadioButton("Black Cars", false)) {
            CCheat::BlackCarsCheat();
        }
        if (ImGui::RadioButton("Beach Party", false)) {
            CCheat::BeachPartyCheat();
        }
        if (ImGui::RadioButton("Countryside Invasion", false)) {
            CCheat::CountrysideInvasionCheat();
        }
        if (ImGui::RadioButton("Elvis Lives Matter", false)) {
            CCheat::ElvisLivesCheat();
        }
        if (ImGui::RadioButton("Everyone Attacks Player", false)) {
            CCheat::EverybodyAttacksPlayerCheat();
        }
        if (ImGui::RadioButton("Funhouse", false)) {
            CCheat::FunhouseCheat();
        }
        if (ImGui::RadioButton("Gang Land", false)) {
            CCheat::GangLandCheat();
        }
        if (ImGui::RadioButton("Gangs", false)) {
            CCheat::GangsCheat();
        }
        if (ImGui::RadioButton("Love Conquers All", false)) {
            CCheat::LoveConquersAllCheat();
        }
        if (ImGui::RadioButton("Love fist", false)) {
            CCheat::LovefistCheat();
        }
        if (ImGui::RadioButton("Mayhem", false)) {
            CCheat::MayhemCheat();
        }
        if (ImGui::RadioButton("Midnight", false)) {
            CCheat::MidnightCheat();
        }
        if (ImGui::RadioButton("Ninja", false)) {
            CCheat::NinjaCheat();
        }
        if (ImGui::RadioButton("Parachute", false)) {
            CCheat::ParachuteCheat();
        }
        if (ImGui::RadioButton("Pink Cars", false)) {
            CCheat::PinkCarsCheat();
        }
        if (ImGui::RadioButton("Predator", false)) {
            CCheat::PredatorCheat();
        }
        if (ImGui::RadioButton("Riot", false)) {
            CCheat::RiotCheat();
        }
        if (ImGui::RadioButton("Village People", false)) {
            CCheat::VillagePeopleCheat();
        }
        if (ImGui::RadioButton("Time Travel", false)) {
            CCheat::TimeTravelCheat();
        }
        if (ImGui::RadioButton("The Gambler", false)) {
            CCheat::TheGamblerCheat();
        }
    }

    if (ImGui::RadioButton("Reset Cheats", false)) {
        CCheat::ResetCheats();
    }
}
