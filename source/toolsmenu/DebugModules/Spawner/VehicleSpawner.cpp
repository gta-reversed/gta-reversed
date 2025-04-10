#include "StdInc.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include "VehicleSpawner.h"

#include "CDebugMenuToolInput.h"

namespace VehicleDebugModule {

static CDebugMenuToolInput m_vehicleToolInput;

CDebugMenuToolInput::ToolMap m_vehiclesMap{
    { 400, "Landstalker" },
    { 401, "Bravura" },
    { 402, "Buffalo" },
    { 403, "Linerunner" },
    { 404, "Perennial" },
    { 405, "Sentinel" },
    { 406, "Dumper" },
    { 407, "Fire Truck" },
    { 408, "Trashmaster" },
    { 409, "Stretch" },
    { 410, "Manana" },
    { 411, "Infernus" },
    { 412, "Voodoo" },
    { 413, "Pony" },
    { 414, "Mule" },
    { 415, "Cheetah" },
    { 416, "Ambulance" },
    { 417, "Leviathan" },
    { 418, "Moonbeam" },
    { 419, "Esperanto" },
    { 420, "Taxi" },
    { 421, "Washington" },
    { 422, "Bobcat" },
    { 423, "Mr. Whoopee" },
    { 424, "BF Injection" },
    { 425, "Hunter" },
    { 426, "Premier" },
    { 427, "Enforcer" },
    { 428, "Securicar" },
    { 429, "Banshee" },
    { 430, "Predator" },
    { 431, "Bus" },
    { 432, "Rhino" },
    { 433, "Barracks" },
    { 434, "Hotknife" },
    { 435, "Trailer 1" },
    { 436, "Previon" },
    { 437, "Coach" },
    { 438, "Cabbie" },
    { 439, "Stallion" },
    { 440, "Rumpo" },
    { 441, "RC Bandit" },
    { 442, "Romero" },
    { 443, "Packer" },
    { 444, "Monster" },
    { 445, "Admiral" },
    { 446, "Squalo" },
    { 447, "Seasparrow" },
    { 448, "Pizzaboy" },
    { 449, "Tram" },
    { 450, "Trailer 2" },
    { 451, "Turismo" },
    { 452, "Speeder" },
    { 453, "Reefer" },
    { 454, "Tropic" },
    { 455, "Flatbed" },
    { 456, "Yankee" },
    { 457, "Caddy" },
    { 458, "Solair" },
    { 459, "Berkley's RC Van" },
    { 460, "Skimmer" },
    { 461, "PCJ-600" },
    { 462, "Faggio" },
    { 463, "Freeway" },
    { 464, "RC Baron" },
    { 465, "RC Raider" },
    { 466, "Glendale" },
    { 467, "Oceanic" },
    { 468, "Sanchez" },
    { 469, "Sparrow" },
    { 470, "Patriot" },
    { 471, "Quadbike" },
    { 472, "Coastguard" },
    { 473, "Dinghy" },
    { 474, "Hermes" },
    { 475, "Sabre" },
    { 476, "Rustler" },
    { 477, "ZR-350" },
    { 478, "Walton" },
    { 479, "Regina" },
    { 480, "Comet" },
    { 481, "BMX" },
    { 482, "Burrito" },
    { 483, "Camper" },
    { 484, "Marquis" },
    { 485, "Baggage" },
    { 486, "Dozer" },
    { 487, "Maverick" },
    { 488, "News Chopper" },
    { 489, "Rancher" },
    { 490, "FBI Rancher" },
    { 491, "Virgo" },
    { 492, "Greenwood" },
    { 493, "Jetmax" },
    { 494, "Hotring Racer" },
    { 495, "Sandking" },
    { 496, "Blista Compact" },
    { 497, "Police Maverick" },
    { 498, "Boxville" },
    { 499, "Benson" },
    { 500, "Mesa" },
    { 501, "RC Goblin" },
    { 502, "Hotring Racer 3" },
    { 503, "Hotring Racer 2" },
    { 504, "Bloodring Banger" },
    { 505, "Rancher Lure" },
    { 506, "Super GT" },
    { 507, "Elegant" },
    { 508, "Journey" },
    { 509, "Bike" },
    { 510, "Mountain Bike" },
    { 511, "Beagle" },
    { 512, "Cropduster" },
    { 513, "Stuntplane" },
    { 514, "Tanker" },
    { 515,"Roadtrain" },
    { 516, "Nebula" },
    { 517, "Majestic" },
    { 518, "Buccaneer" },
    { 519, "Shamal" },
    { 520, "Hydra" },
    { 521, "FCR-900" },
    { 522, "NRG-500" },
    { 523, "HPV1000" },
    { 524, "Cement Truck" },
    { 525, "Towtruck" },
    { 526, "Fortune" },
    { 527, "Cadrona" },
    { 528, "FBI Truck" },
    { 529, "Willard" },
    { 530, "Forklift" },
    { 531, "Tractor" },
    { 532, "Combine Harvester" },
    { 533, "Feltzer" },
    { 534, "Remington" },
    { 535, "Slamvan" },
    { 536, "Blade" },
    { 537, "Freight" },
    { 538, "Streak" },
    { 539, "Vortex" },
    { 540, "Vincent" },
    { 541, "Bullet" },
    { 542, "Clover" },
    { 543, "Sadler" },
    { 544, "Fire Truck Ladder" },
    { 545, "Hustler" },
    { 546, "Intruder" },
    { 547, "Primo" },
    { 548, "Cargobob" },
    { 549, "Tampa" },
    { 550, "Sunrise" },
    { 551, "Merit" },
    { 552, "Utility Van" },
    { 553, "Nevada" },
    { 554, "Yosemite" },
    { 555, "Windsor" },
    { 556, "Monster 2" },
    { 557, "Monster 3" },
    { 558, "Uranus" },
    { 559, "Jester" },
    { 560, "Sultan" },
    { 561, "Stratum" },
    { 562, "Elegy" },
    { 563, "Raindance" },
    { 564, "RC Tiger" },
    { 565, "Flash" },
    { 566, "Tahoma" },
    { 567, "Savanna" },
    { 568, "Bandito", },
    { 569, "Freight Train Flatbed" },
    { 570, "Streak Train Trailer" },
    { 571, "Kart" },
    { 572, "Mower" },
    { 573, "Dune" },
    { 574, "Sweeper" },
    { 575, "Broadway" },
    { 576, "Tornado" },
    { 577, "AT-400" },
    { 578,  "DFT-30" },
    { 579, "Huntley" },
    { 580,"Stafford" },
    { 581, "BF-400" },
    { 582, "Newsvan" },
    { 583,"Tug" },
    { 584,"Trailer (Tanker Commando)" },
    { 585, "Emperor" },
    { 586,"Wayfarer" },
    { 587,"Euros" },
    { 588,"Hotdog" },
    { 589,"Club" },
    { 590,"Box Freight" },
    { 591,"Trailer 3" },
    { 592,"Andromada" },
    { 593,"Dodo" },
    { 594,"RC Cam" },
    { 595,"Launch" },
    { 596,"Police LS" },
    { 597,"Police SF" },
    { 598,"Police LV" },
    { 599,"Police Ranger" },
    { 600, "Picador" },
    { 601,"S.W.A.T." },
    { 602,"Alpha" },
    { 603, "Phoenix" },
    { 604,"Glendale Damaged" },
    { 605,"Sadler Damaged" },
    { 606,"Baggage Trailer (covered)" },
    { 607, "Baggage Trailer (Uncovered)" },
    { 608, "Trailer (Stairs)" },
    { 609, "Boxville Mission" },
    { 610, "Farm Trailer" },
    { 611, "Street Clean Trailer" }
};

void Initialise() {
    m_vehicleToolInput.Initialise(256, &m_vehiclesMap);
}

static void Spawn_RC_Cars() {
    auto player = FindPlayerPed();
    if (!player)
        return;

    CPad* pad = CPad::GetPad(0);
    int32 vehicleId = MODEL_INVALID;
    if (pad->IsStandardKeyJustDown('1'))
        vehicleId = MODEL_RCBANDIT;
    else if (pad->IsStandardKeyJustDown('2'))
        vehicleId = MODEL_RCTIGER;
    else if (pad->IsStandardKeyJustDown('3'))
        vehicleId = MODEL_RCBARON;

    if (vehicleId != MODEL_INVALID) {
        CStreaming::RequestModel(vehicleId, STREAMING_GAME_REQUIRED);
        CStreaming::LoadAllRequestedModels(false);
        CVector pos = player->GetPosition() + CVector(2.0f, 1.0f, 1.0f);
        CRemote::GivePlayerRemoteControlledCar(pos, player->GetHeading(), vehicleId);
        CVehicle::bDisableRemoteDetonation = true;
        CVehicle::bDisableRemoteDetonationOnContact = true;
        NOTSA_LOG_DEBUG("spawned rc car\n");
    }
}

void ProcessImGui() {
    ImGui::PushItemWidth(465.0f);
    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
    if (ImGui::InputText(" ", &m_vehicleToolInput.GetInputBuffer(), input_text_flags)) {
        reclaim_focus = true;
    }
    ImGui::PopItemWidth();

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    m_vehicleToolInput.Process();

    ImGui::BeginChild("##vehiclestool", ImVec2(0, 280));
    const auto w = ImGui::GetWindowWidth();
    static bool widthSet = false;
    ImGui::Columns(2);
    if (!widthSet) {
        widthSet = true;
        ImGui::SetColumnWidth(0, w * 0.2f);
        ImGui::SetColumnWidth(1, w * 0.8f);
    }
    ImGui::TextUnformatted("ID");
    ImGui::NextColumn();
    ImGui::TextUnformatted("Name");
    ImGui::NextColumn();
    ImGui::Separator();
    static int32 selectedId = MODEL_INVALID;
    for (const auto& [id, name] : m_vehicleToolInput.GetGridListMap()) {
        ImGui::PushID(id);

        ImGui::Text("%i", id);
        ImGui::NextColumn();
        if (ImGui::Selectable(name.c_str(), selectedId == id, ImGuiSelectableFlags_SpanAllColumns)) {
            selectedId = id;
        }
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && selectedId != MODEL_INVALID)
            CCheat::VehicleCheat(static_cast<eModelID>(selectedId));
        ImGui::NextColumn();
        ImGui::PopID();
    }
    ImGui::EndColumns();
    ImGui::EndChild();
    ImGui::Separator();

    ImGui::SetCursorPosX(117);
    if (ImGui::Button("SPAWN VEHICLE", ImVec2(250, 0)) && selectedId != MODEL_INVALID) {
        CCheat::VehicleCheat(static_cast<eModelID>(selectedId));
    }
}

void ProcessRender() {}

} // namespace VehicleDebugModule
