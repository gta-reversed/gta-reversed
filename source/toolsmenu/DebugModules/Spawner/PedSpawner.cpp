#include "StdInc.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>
#include <extensions/random.hpp>

#include "PedSpawner.h"

#include "CDebugMenuToolInput.h"

namespace PedSpawnerModule {

CDebugMenuToolInput m_pedToolInput;

CDebugMenuToolInput::ToolMap m_pedsMap{
    { 0, "CJ"},
    // wrong id or smth { 1,   "Truth" },
    // wrong id or smth { 2,   "Maccer" },
    { 7,   "Male 01 (Special" },
    { 9,   "Black Female Old Rich" },
    { 10,  "Black Female Old Street" },
    { 11,  "(Las Vegas) Black Female Young Croupier" },
    { 12,  "Black Female Young Rich" },
    { 13,  "Black Female Young Street" },
    { 14,  "Black Male Old Rich" },
    { 15,  "Black Male Old Street" },
    { 16,  "Black Male Young Airport" },
    { 17,  "Black Male Young Business" },
    { 18,  "Black Male Young Beach" },
    { 19,  "Black Male Young Disk Jockey" },
    { 20,  "Black Male Young Rich" },
    { 21,  "Black Male Young Criminal" },
    { 22,  "Black Male Young Street" },
    { 23,  "White Male Young BMX Rider" },
    { 24,  "Madd Doggs Bodyguard 1" },
    { 25,  "Madd Doggs Bodyguard 2" },
    { 26,  "White Male Young Backpacker" },
    { 27,  "White Male Young Construction Worker" },
    { 28,  "Black Male Young Drug Dealer" },
    { 29,  "White Male Young Drug Dealer" },
    { 30,  "Hispanic Male Young Drug Dealer" },
    { 31,  "Desert White Female Old Low Class?" },
    { 32,  "Desert Male Female Old Low Class 1?" },
    { 33,  "Desert Male Female Old Low Class 2?" },
    { 34,  "Desert Male Female Young Low Class 1?" },
    { 35,  "Hispanic Male Old Gardener" },
    { 36,  "White Male Young Golfer 1" },
    { 37,  "White Male Young Golfer 2" },
    { 38,  "Hispanic Female Old Rich" },
    { 39,  "Hispanic Female Old Street" },
    { 40,  "Hispanic Female Young Rich" },
    { 41,  "Hispanic Female Young Street" },
    { 43,  "Hispanic Male Old Rich" },
    { 44,  "Hispanic Male Old Street" },
    { 45,  "Hispanic Male Young Beach" },
    { 46,  "Hispanic Male Young Rich" },
    { 47,  "Hispanic Male Young Criminal" },
    { 48,  "Hispanic Male Young Street" },
    { 49,  "Oriental Male Old Kung Fu" },
    { 50,  "White Male Young Mechanic" },
    { 51,  "Black Male Young Mountain Biker" },
    { 52,  "White Male Young Mountain Biker" },
    { 53,  "Oriental Female Old Rich" },
    { 54,  "Oriental Female Old Street" },
    { 55,  "Oriental Female Young Rich" },
    { 56,  "Oriental Female Young Street" },
    { 57,  "Oriental Male Old Rich" },
    { 58,  "Oriental Male Old Street" },
    { 59,  "Oriental Male Young Rich" },
    { 60,  "Oriental Male Young Street" },
    { 61,  "White Male Young Pilot" },
    { 62,  "White Male Old Pyjamas" },
    { 63,  "Black Female Young Prostitute" },
    { 64,  "Hispanic Female Young Prostitute" },
    { 66,  "Black Male Young Pool 1" },
    { 67,  "Black Male Young Pool 2" },
    { 68,  "White Male Old Preacher" },
    { 69,  "(San Fransisco) Black Female Young Street" },
    { 70,  "White Male Old Scientist" },
    { 71,  "White Male Young Security Guard" },
    { 72,  "(San Fransisco) White Male Young Hippie 1" },
    { 73,  "(San Fransisco) White Male Young Hippie 2" },
    { 75,  "(San Fransisco) White Female Old Prostitute" },
    { 76,  "White Female Young Stewardess" },
    { 77,  "(San Fransisco) White Male Old Tramp 1" },
    { 78,  "White Male Old Tramp 1" },
    { 79,  "Black Male Old Tramp 1" },
    { 80,  "(Las Vegas) Black Male Young Boxer" },
    { 81,  "(Las Vegas) White Male Young Boxer" },
    { 82,  "(Las Vegas) Hispanic Male Young Elvis" },
    { 83,  "(Las Vegas) Black Male Young Elvis" },
    { 84,  "(Las Vegas) Indian Male Young Elvis" },
    { 85,  "(Las Vegas) White Female Young Prostitute" },
    { 87,  "(Las Vegas) White, Female, Young, Stripper 1" },
    { 88,  "White Female Old Rich" },
    { 89,  "White Female Old Street" },
    { 90,  "White Female Young Jogger" },
    { 91,  "White Female Young Rich" },
    { 92,  "White Female Young Rollerblader" },
    { 93,  "White Female Young Street" },
    { 94,  "White Male Old Rich" },
    { 95,  "White Male Old Street" },
    { 96,  "White Male Young Jogger" },
    { 97,  "White Male Young Lifeguard" },
    { 98,  "White Male Young Rich" },
    { 99,  "White Male Young Rollerblader" },
    { 100, "White Male Young Criminal" },
    { 101, "White Male Young Street" },
    { 102, "Ballas Gang Member 1" },
    { 103, "Ballas Gang Member 2" },
    { 104, "Ballas Gang Member 3" },
    { 105, "Grove Street Famalies Gang Member 1" },
    { 106, "Grove Street Famalies Gang Member 2" },
    { 107, "Grove Street Famalies Gang Member 3" },
    { 108, "Los Santos Vagos Gang Member 1" },
    { 109, "Los Santos Vagos Gang Member 2" },
    { 110, "Los Santos Vagos Gang Member 3" },
    { 111, "Mafia Gang Member A" },
    { 112, "Mafia Gang Member B" },
    { 113, "Mafia Boss" },
    { 114, "Vagos Los Aztecas Gang Member 1" },
    { 115, "Vagos Los Aztecas Gang Member 2" },
    { 116, "Vagos Los Aztecas Gang Member 3" },
    { 117, "Triad Gang Member A" },
    { 118, "Triad Gang Member B" },
    { 120, "Triad Boss" },
    { 121, "Da Nang Boys Gang Member 1" },
    { 122, "Da Nang Boys Gang Member 2" },
    { 123, "Da Nang Boys Gang Member 3" },
    { 124, "(Las Vegas) Mafia Gang Member 1" },
    { 125, "(Las Vegas) Mafia Gang Member 2" },
    { 126, "(Las Vegas) Mafia Gang Member 3" },
    { 127, "(Las Vegas) Mafia Gang Member 4" },
    { 128, "(Desert) Native Male, Young, Low Class?" },
    { 129, "(Desert) Native Female, Old Low Class 1?" },
    { 130, "(Desert) Native Female, Old Low Class 2?" },
    { 131, "(Desert) Native Female Young, Low Class?" },
    { 132, "(Desert) Native Male Old Low Class 1?" },
    { 133, "(Desert) Native Male Old Low Class 2?" },
    { 134, "(San Fransisco) Black Male Old Tramp 2" },
    { 135, "(San Fransisco) White Male Old Tramp 2" },
    { 136, "(San Fransisco) White Male Young Tramp 3" },
    { 137, "(San Fransisco) White Male Old Tramp 3" },
    { 138, "(San Fransisco) White Female Young Beach" },
    { 139, "(San Fransisco) Black Female Young Beach" },
    { 140, "(San Fransisco) Hispanic Female Young Beach" },
    { 141, "(San Fransisco) Oriental Female Young Business" },
    { 142, "(San Fransisco) Black Male Young Street" },
    { 143, "(San Fransisco) Black Male Young Criminal" },
    { 144, "Black Male Young Crack Guard?" },
    { 145, "White Female Young Crack" },
    { 146, "Hispanic Male Young Crack Meth?" },
    { 147, "White Male Young Business" },
    { 148, "Black Female Young Business" },
    { 150, "White Female Young Business" },
    { 151, "(Desert) White Female Young Low Class 1?" },
    { 152, "White Female Young Prostitute" },
    { 153, "White Male Young Construction Worker B" },
    { 154, "White Male Young Beach" },
    { 155, "White Male Young Pizza Stack Employee" },
    { 156, "Black Male Old Barber (Old Reece)" },
    { 157, "(Country) White Female Young Hillbilly" },
    { 158, "(Country) White Male Old FR?" },
    { 159, "(Country) White Male Old Hilbilly 1" },
    { 160, "(Country) White Male Old Hilbilly 2" },
    { 161, "(Country) White Male Young FR" },
    { 162, "(Country) White Male Young, Hillbilly 1" },
    { 163, "Black Male Young Bouncer" },
    { 164, "White Male Young Bouncer" },
    { 165, "White Male Old Men In Black" },
    { 166, "Black Male Young Men In Black" },
    { 167, "White Male Young Clucking Bell Employee" },
    { 168, "Black Male Old Chilli Stall Owner" },
    { 169, "(San Fransisco) Oriental Female Young Rich" },
    { 170, "(San Fransisco) Oriental Male Young Street" },
    { 171, "(Las Vegas) White Male Young Blackjack Dealer" },
    { 172, "(Las Vegas) White Female Young Croupier" },
    { 173, "San Fransisco Rifa Gang Member 1" },
    { 174, "San Fransisco Rifa Gang Member 2" },
    { 175, "San Fransisco Rifa Gang Member 3" },
    { 176, "Black Male Young Barber" },
    { 177, "White Male Young Barber" },
    { 178, "White Female Young Sexy" },
    { 179, "White Male Young Ammunation Guy" },
    { 180, "Black Male Young Tattoo" },
    { 181, "(Vegas) White Male Young Criminal" },
    { 182, "(Vegas) Black Male Old Cab Driver" },
    { 183, "(Vegas) Black Male Young Criminal" },
    { 184, "(Vegas) Hispanic Male Young Criminal" },
    { 185, "(San Fransisco) Black Male Young Rich" },
    { 186, "(San Fransisco) Oriental Male Young Rich" },
    { 187, "(San Fransisco) Oriental Male Young Business" },
    { 188, "(San Fransisco) White Male Young Street" },
    { 189, "White Male Young Valet" },
    { 190, "Barbara Schternvart" },
    { 191, "Helena Wankstein" },
    { 192, "Michelle Cannes" },
    { 193, "Katie Zhan" },
    { 194, "Millie Perkins" },
    { 195, "Denise Robinson" },
    { 196, "(Country) White Femal, Old, FR?" },
    { 197, "(Country) White Female Old Hillbilly" },
    { 198, "(Country) White Female Young FR1?" },
    { 199, "(Country) White Female Young FR2?" },
    { 200, "(Country) White Male Young Hillbilly 2" },
    { 201, "(Desert) White Female Young LC2?" },
    { 202, "(Desert) White Male Young LC2?" },
    { 203, "Oriental Male Young Karate Trainer" },
    { 204, "White Male Young Karate Trainer" },
    { 205, "White Female Young Burger Shot Employee" },
    { 206, "(Las Vegas) White Male Young Cab Driver" },
    { 207, "(Las Vegas) Hispanic Female Young Prostitute" },
    { 209, "Oriental Male Old Noodle Stall Owner" },
    { 210, "Oriental Male Old Boat School Instructor" },
    { 211, "White Female Old CLOT? (Bar Steward)" },
    { 212, "(Las Vegas) White Male Old Tramp 1" },
    { 213, "(Las Vegas) White Male Old Tramp 2" },
    { 214, "(Las Vegas) White Female Young Waiter (Maria)" },
    { 215, "(San Fransisco) Black Female Old Rich" },
    { 216, "(San Fransisco) White Female Young Rich" },
    { 217, "White Male Young CLOT? (Bar Stuard)" },
    { 218, "(San Fransisco) Black Female Old Street" },
    { 219, "(San Fransisco) Black Female Young Rich" },
    { 220, "(San Fransisco) Black Male Old Cab Driver" },
    { 221, "(San Fransisco) Black Male Old Rich" },
    { 222, "(San Fransisco) Black Male Old Street" },
    { 223, "(San Fransisco) Hispanic Male Young Criminal" },
    { 224, "(San Fransisco) Oriental Female Old Rich" },
    { 225, "(San Fransisco) Oriental Female Old Street" },
    { 226, "(San Fransisco) Oriental Female Young Street" },
    { 227, "(San Fransisco) Oriental Male Old Business" },
    { 228, "(San Fransisco) Oriental Male Old Rich" },
    { 229, "(San Fransisco) Oriental Male Old Street" },
    { 230, "(San Fransisco) White Male Old Tramp 5" },
    { 231, "(San Fransisco) White Female Old Rich" },
    { 232, "(San Fransisco) White Female Old Street" },
    { 233, "(San Fransisco) White Female Young Street" },
    { 234, "(San Fransisco) White Male Old Cab Driver" },
    { 235, "(San Fransisco) White Male Old Rich" },
    { 236, "(San Fransisco) White Male Old Street" },
    { 237, "(San Fransisco) Hispanic Female Young Prostitute" },
    { 238, "(San Fransisco) Black Female Young Prostitute" },
    { 239, "(San Fransisco) White Male Old Tramp 4" },
    { 240, "(San Fransisco) White Male Young Rich" },
    { 241, "(San Fransisco) Male Young Street" },
    { 242, "(San Fransisco) Male Young Street 2" },
    { 243, "(San Fransisco) Female Young Prostitute" },
    { 244, "(Las Vegas) Black Female Young Street 2" },
    { 245, "(Las Vegas) Black Female Young Prostitute" },
    { 246, "(Las Vegas) Hispanic Female Young Street 3" },
    { 247, "Biker A" },
    { 248, "Biker B" },
    { 249, "Black, Male, Young, Pimp" },
    { 250, "(San Fransisco) White Male Young Criminal" },
    { 251, "White Female Young Lifeguard" },
    { 252, "White Male Young Valet 2" },
    { 253, "Black Male Old Security Guard" },
    { 254, "Biker / Drug Dealer" },
    { 255, "White Male Young Chauffeur" },
    { 256, "(San Fransisco) Black Female Young Stripper" },
    { 257, "(San Fransisco) White Female Young Stripper" },
    { 258, "Heckler 1" },
    { 259, "Heckler 2" },
    { 260, "Black Male Young Construction Worker" },
    { 261, "White Male Young Cab Driver 1" },
    { 262, "Black Male Old Cab Driver" },
    { 263, "(Vegas) White Female Young Waiter 2" },
    { 264, "White Male Old Ice Cream Man" },
    { 274, "Los Angeles Emergency Medical Technician 1" },
    { 275, "Las Vegas Emergency Medical Technician 1" },
    { 276, "San Fransisco Emergency Medical Technician 1" },
    { 277, "Los Angeles Fire Department Firefighter 1" },
    { 278, "Las Vegas Fire Department Firefighter 1" },
    { 279, "San Fransisco Fire Department Firefighter 1" },
    { 280, "Los Angeles Police Department Cop 1" },
    { 281, "San Fransisco Police Department Cop 1" },
    { 282, "Las Vegas Police Department Cop 1" },
    { 283, "Country Sheriff" },
    { 284, "Los Angeles Police Department Motorbike Cop 1" },
    { 285, "S.W.A.T Officer" },
    { 286, "FBI Agent" },
    { 287, "Army Officer" },
    { 288, "Desert Sheriff" },
    // These are cutscene specific
    /* crash
    { 290, "Special Actor 1" },
    { 291, "Special Actor 2" },
    { 292, "Special Actor 3" },
    { 293, "Special Actor 4" },
    { 294, "Special Actor 5" },
    { 295, "Special Actor 6" },
    { 296, "Special Actor 7" },
    { 297, "Special Actor 8" },
    { 298, "Special Actor 9" },
    { 299, "Special Actor 10" }
    */
};

void Initialise() {
    m_pedToolInput.Initialise(256, &m_pedsMap);
}

void SpawnPed(int32 modelId, CVector position) {
    CStreaming::RequestModel(modelId, STREAMING_MISSION_REQUIRED | STREAMING_KEEP_IN_MEMORY);
    CStreaming::LoadAllRequestedModels(false);
    CPed* ped = new CCivilianPed(CPopulation::IsFemale((eModelID)modelId) ? PED_TYPE_CIVFEMALE : PED_TYPE_CIVMALE, modelId);
    ped->SetOrientation(0.0f, 0.0f, 0.0f);
    ped->SetPosn(position);
    CWorld::Add(ped);
    ped->PositionAnyPedOutOfCollision();
}

void SpawnRandomPed() {
    SpawnPed(
        notsa::random_iter(m_pedsMap)->first,
        FindPlayerPed()->TransformFromObjectSpace(CVector(2.0f, 2.0f, 0.0f))
    );
}

void ProcessImGui() {
    ImGui::PushItemWidth(465.0f);
    bool reclaim_focus = false;
    if (ImGui::InputText(" ", &m_pedToolInput.GetInputBuffer(), ImGuiInputTextFlags_EnterReturnsTrue)) {
        reclaim_focus = true;
    }
    ImGui::PopItemWidth();

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    m_pedToolInput.Process();

    ImGui::BeginChild("##pedstool", ImVec2(0, 280));
    const auto  w = ImGui::GetWindowWidth();
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
    static int32 selectedId = -1;
    for (const auto& [id, name] : m_pedToolInput.GetGridListMap()) {
        ImGui::PushID(id);

        ImGui::Text("%i", id);
        ImGui::NextColumn();
        if (ImGui::Selectable(name.c_str(), selectedId == id, ImGuiSelectableFlags_SpanAllColumns)) {
            selectedId = id;
        }
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && selectedId != -1)
            SpawnPed(selectedId, FindPlayerPed()->TransformFromObjectSpace(CVector(2.0f, 2.0f, 0.0f)));
        ImGui::NextColumn();
        ImGui::PopID();
    }
    ImGui::EndColumns();
    ImGui::EndChild();
    ImGui::Separator();

    ImGui::SetCursorPosX(117);
    if (ImGui::Button("SPAWN PED", ImVec2(250, 0)) && selectedId != -1) {
        SpawnPed(selectedId, FindPlayerPed()->TransformFromObjectSpace(CVector(2.0f, 2.0f, 0.0f)));
    }
}

void ProcessRender() {}

} // namespace PedSpawnerModule
