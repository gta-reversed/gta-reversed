#include "StdInc.h"

#include <imgui.h>
#include <libs/imgui/misc/cpp/imgui_stdlib.h>
#include <imgui_internal.h>

#include "MissionDebugModule.h"

#include "CDebugMenuToolInput.h"
#include "EntryExitManager.h"
#include "TheScripts.h"
#include "Hud.h"

CDebugMenuToolInput::ToolMap s_MissionsMap{
    { 0, "Initial 1" },
    { 1, "Initial 2" },
    { 2, "Intro" },
    { 3, "Video Game: They Crawled From Uranus" },
    { 4, "Video Game: Dualuty" },
    { 5, "Video Game: Go Go Space Monkey" },
    { 6, "Video Game: Let's Get Ready To Bumble" },
    { 7, "Video Game: Inside Track Betting" },
    { 8, "Pool" },
    { 9, "Lowrider (Bet And Dance)" },
    { 10, "Beefy Baron" },
    { 11, "Big Smoke" },
    { 12, "Ryder" },
    { 13, "Tagging Up Turf" },
    { 14, "Cleaning The Hood" },
    { 15, "Drive-Thru" },
    { 16, "Nines And AK's" },
    { 17, "Drive-By" },
    { 18, "Sweet's Girl" },
    { 19, "Cesar Vialpando" },
    { 20, "Los Sepulcros" },
    { 21, "Doberman" },
    { 22, "Burning Desire" },
    { 23, "Gray Imports" },
    { 24, "Home Invasion" },
    { 25, "Catalyst" },
    { 26, "Robbing Uncle Sam" },
    { 27, "OG Loc" },
    { 28, "Running Dog" },
    { 29, "Wrong Side of the Tracks" },
    { 30, "Just Business" },
    { 31, "Life's a Beach" },
    { 32, "Madd Dogg's Rhymes" },
    { 33, "Management Issues" },
    { 34, "House Party" },
    { 35, "Race Tournament / 8-track / Dirt Track" },
    { 36, "Lowrider (High Stakes)" },
    { 37, "Reuniting The Families" },
    { 38, "The Green Sabre" },
    { 39, "Badlands" },
    { 40, "First Date" },
    { 41, "Local Liquor Store" },
    { 42, "Small Town Bank" },
    { 43, "Tanker Commander" },
    { 44, "Against All Odds" },
    { 45, "King in Exile" },
    { 46, "Body Harvest" },
    { 47, "Are you going to San Fierro?" },
    { 48, "Wu Zi Mu / Farewell, My Love..." },
    { 49, "Wear Flowers In Your Hai" },
    { 50, "Deconstruction" },
    { 51, "555 WE TIP" },
    { 52, "Snail Trail" },
    { 53, "Mountain Cloud Boys" },
    { 54, "Ran Fa Li" },
    { 55, "Lure" },
    { 56, "Amphibious Assault" },
    { 57, "The Da Nang Thang" },
    { 58, "Photo Opportunity" },
    { 59, "Jizzy" },
    { 60, "Outrider" },
    { 61, "Ice Cold Killa" },
    { 62, "Toreno's Last Flight" },
    { 63, "Yay Ka-Boom-Boom" },
    { 64, "Pier 69" },
    { 65, "T-Bone Mendez" },
    { 66, "Mike Toreno" },
    { 67, "Zeroing In" },
    { 68, "Test Drive" },
    { 69, "Customs Fast Track " },
    { 70, "Puncture Wounds" },
    { 71, "Back to School" },
    { 72, "Air Raid" },
    { 73, "Supply Lines..." },
    { 74, "New Model Army " },
    { 75, "Monster" },
    { 76, "Highjack" },
    { 77, "Interdiction" },
    { 78, "Verdant Meadows" },
    { 79, "N.O.E." },
    { 80, "Stowaway" },
    { 81, "Black Project" },
    { 82, "Green Goo" },
    { 83, "Learning to Fly" },
    { 84, "Fender Ketchup" },
    { 85, "Explosive Situation" },
    { 86, "You've Had Your Chips" },
    { 87, "Fish in a Barrel " },
    { 88, "Don Peyote" },
    { 89, "Intensive Care" },
    { 90, "The Meat Business" },
    { 91, "Freefall" },
    { 92, "Saint Mark's Bistro" },
    { 93, "Misappropriation" },
    { 94, "High Noon" },
    { 95, "Madd Dogg" },
    { 96, "Architectural Espionage " },
    { 97, "Key To Her Heart" },
    { 98, "Dam And Blast" },
    { 99, "Cop Wheels" },
    { 100, "Up, Up and Away!" },
    { 101, "Breaking the Bank at Caligula's" },
    { 102, "A Home In The Hills" },
    { 103, "Vertical Bird" },
    { 104, "Home Coming " },
    { 105, "Cut Throat Business" },
    { 106, "Beat Down on B Dup " },
    { 107, "Grove 4 Life" },
    { 108, "Riot" },
    { 109, "Los Desperados" },
    { 110, "End Of The Line (1)" },
    { 111, "End Of The Line (2)" },
    { 112, "End Of The Line (3)" },
    { 113, "Shooting range" },
    { 114, "Los Santos Gym Fight School" },
    { 115, "San Fierro Gym Fight School" },
    { 116, "Las Venturas Gym Fight School" },
    { 117, "Trucking" },
    { 118, "Quarry" },
    { 119, "Boat School" },
    { 120, "Bike School" },
    { 121, "Taxi-Driver Sub-Mission" },
    { 122, "Paramedic Sub-Mission" },
    { 123, "Firefighter Sub-Mission" },
    { 124, "Vigilante Sub-Mission" },
    { 125, "Burglary Sub-Mission" },
    { 126, "Freight Train Sub-Mission" },
    { 127, "Pimping Sub-Mission" },
    { 128, "Arena Mission: Blood Ring" },
    { 129, "Arena Mission: Kickstart" },
    { 130, "Beat the Cock!" },
    { 131, "Courier" },
    { 132, "The Chiliad Challenge" },
    { 133, "BMX / NRG-500 STUNT Mission" },
    { 134, "Buy Properties Mission" }
};

MissionDebugModule::MissionDebugModule() {
    m_MissionToolInput.Initialise(256, &s_MissionsMap);
}

void InitializeAndStartNewScript() {
    CTheScripts::WipeLocalVariableMemoryForMissionScript();
    CRunningScript* script = CTheScripts::StartNewScript(&CTheScripts::MissionBlock[0]);
    script->m_bUseMissionCleanup = true;
    script->m_bIsMission = true;
    script->m_pBaseIP = &CTheScripts::MissionBlock[0];
    CTheScripts::bAlreadyRunningAMissionScript = true;
    CGameLogic::ClearSkip(false);
}

bool MissionDebugModule::StartMission(int32 missionId, bool bDoMissionCleanUp = true) {
    if (!m_ShouldStartMission && CTheScripts::IsPlayerOnAMission()) {
        if (CCutsceneMgr::HasLoaded()) {
            CCutsceneMgr::DeleteCutsceneData();
        }
        CTheScripts::FailCurrentMission = 2;
    }
    if (CTheScripts::FailCurrentMission > 0) {
        m_ShouldStartMission = true;
        m_MissionIDToStart = missionId;
        return true;
    }
    m_ShouldStartMission = false;
    CTheScripts::bPlayerIsOffTheMap = false;
    CGame::currArea = 0;
    CPlayerPed* player = FindPlayerPed();
    player->m_nAreaCode = AREA_CODE_NORMAL_WORLD;
    if (!CGame::currArea) {
        player->m_pEnex = nullptr;
        CEntryExitManager::ms_entryExitStackPosn = 0;
        CTimeCycle::StopExtraColour(false);
    }

    if (bDoMissionCleanUp)
        CTheScripts::MissionCleanUp.Process();

    if (CTheScripts::NumberOfExclusiveMissionScripts > 0) {
        if (missionId <= 65532)
            return false;
        missionId = 0xFFFF - missionId;
    }
    CTimer::Suspend();
    int32 offsetToMission = CTheScripts::MultiScriptArray[missionId];
    CFileMgr::ChangeDir("\\");
    if (CGame::bMissionPackGame) {
        size_t bytesRead = 0;
        while (FrontEndMenuManager.CheckMissionPackValidMenu()) {
            CFileMgr::SetDirMyDocuments();
            sprintf_s(gString, "MPACK//MPACK%d//SCR.SCM", CGame::bMissionPackGame);
            auto* file = CFileMgr::OpenFile(gString, "rb");
            if (file) {
                CFileMgr::Seek(file, offsetToMission, 0);
                bytesRead = CFileMgr::Read(file, &CTheScripts::MissionBlock[0], MISSION_SCRIPT_SIZE);
                CFileMgr::CloseFile(file);
                if (bytesRead >= 1) {
                    InitializeAndStartNewScript();
                    break;
                }
            } else if (bytesRead >= 1) {
                break;
            }
        }
    }
    CFileMgr::SetDir("");
    if (!CGame::bMissionPackGame) {
        auto* file = CFileMgr::OpenFile("data\\script\\main.scm", "rb");
        CFileMgr::Seek(file, offsetToMission, 0);
        CFileMgr::Read(file, &CTheScripts::MissionBlock[0], MISSION_SCRIPT_SIZE);
        CFileMgr::CloseFile(file);
        InitializeAndStartNewScript();
    }
    CTimer::Resume();
    return true;
}

void MissionDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "Missions Starter", { 485.f, 400.f }, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    if (m_ShouldStartMission) {
        StartMission(m_MissionIDToStart);
    }
    
    ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 10.f);
    ImGui::InputText(" ", &m_MissionToolInput.GetInputBuffer());

    m_MissionToolInput.Process();

    ImGui::BeginChild("##missionstool", ImVec2(0.f, 310.f));
    if (ImGui::BeginTable("Missions", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Borders)) { 
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);

        ImGui::TableHeadersRow();

        for (auto&& [idx, name] : m_MissionToolInput.GetGridListMap()) {
            const notsa::ui::ScopedID tableID{ idx };
            //ImGui::BeginGroup();
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%i", idx);

            ImGui::TableNextColumn();
            if (ImGui::Selectable(name.c_str(), m_SelectedMissionIdx == idx, ImGuiSelectableFlags_SpanAllColumns)) {
                m_SelectedMissionIdx = idx;
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && m_SelectedMissionIdx != -1) {
                CHud::SetHelpMessage("Starting Mission!"_gxt, true, false, false);
                StartMission(m_SelectedMissionIdx);
            }
            //ImGui::EndGroup();
        }

        ImGui::EndTable();
    }
    ImGui::EndChild();
}

void MissionDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Tools" }, [&] {
        ImGui::MenuItem("Mission Starter", nullptr, &m_IsOpen);
    });
}
