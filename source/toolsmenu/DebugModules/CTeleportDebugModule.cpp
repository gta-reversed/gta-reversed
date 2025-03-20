#include "StdInc.h"

#include "CTeleportDebugModule.h"
#include "Utility.h"
#include "UIRenderer.h"
#include "DebugModule.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <iterator>
#include <string>
#include <string_view>
#include <chrono>
#include "Radar.h"
#include "extensions/enumerate.hpp"

using namespace ImGui;

void TeleportDebugModule::RenderMenuEntry() {
    if (BeginMenu("Tools")) {
        if (MenuItem("Teleporter")) {
            m_IsOpen = true;
        }
        ImGui::EndMenu();
    }
}

void TeleportDebugModule::RenderWindow() {
    if (m_IsOpen) {
        if (Begin("Teleporter", &m_IsOpen)) {
            SetNextWindowSize({ 415.f, 290.f }, ImGuiCond_FirstUseEver);
            RenderTeleporterWindow();
        }
        End();
    }
}

void TeleportDebugModule::Update() {
    ProcessShortcuts();
}

void TeleportDebugModule::Deserialize(const json& j) {
    from_json(j, *this);
    m_IsStateFromDeserialization = true;
}

void TeleportDebugModule::OnImGuiInitialised(ImGuiContext* ctx) {
    AddSettingsHandler(ctx);
}

static CVector GetPositionWithGroundHeight(const CVector2D& pos) {
    return { pos.x, pos.y, CWorld::FindGroundZForCoord(pos.x, pos.y) + 2.f };
}

// Teleport to exact coordinates
void TeleportDebugModule::TeleportTo(const CVector& pos, eAreaCodes areaCode) {
    CStreaming::LoadSceneCollision(pos);
    CStreaming::LoadScene(pos);

    auto player = FindPlayerPed();
    player->m_nAreaCode = areaCode;
    CGame::currArea = areaCode;
    CStreaming::RemoveBuildingsNotInArea(areaCode);
    CTimeCycle::StopExtraColour(false);

    if (auto vehicle = FindPlayerVehicle()) {
        vehicle->Teleport(pos, true);
    } else {
        player->Teleport(pos, true);
    }

    // Teleport player's group too
    if (auto group = CPedGroups::GetPedsGroup(player)) {
        if (group->GetMembership().CountMembersExcludingLeader()) {
            group->Teleport(pos);
        }
    }
}

void TeleportDebugModule::DoTeleportTo(CVector pos, eAreaCodes areaCode) {
    // Save previous location for teleporting back.
    m_PrevLocation.Pos      = FindPlayerPed()->GetPosition();
    m_PrevLocation.AreaCode = FindPlayerPed()->m_nAreaCode;
    m_PrevLocation.IsSelected = true; // mark the position is saved.

    TeleportDebugModule::TeleportTo(pos, areaCode);
}

void TeleportDebugModule::RenderSavedPositions() {
    auto visibleItems = GetVisibleItems();

    if (!BeginTable("Saved Positions", 5, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY)) {
        return;
    }

    TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed);
    TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
    TableSetupColumn("Position", ImGuiTableColumnFlags_WidthFixed);
    TableSetupColumn("Area code", ImGuiTableColumnFlags_WidthFixed);
    TableSetupColumn("Ground", ImGuiTableColumnFlags_WidthFixed);

    TableHeadersRow();

    // Draw visible items
    for (auto it = visibleItems.begin(); it != visibleItems.end(); it++) {
        auto& item = *it;

        PushID(&item);
        BeginGroup();

        TableNextRow();
        TableNextColumn();

        Text("%d", std::distance(visibleItems.begin(), it) + 1); // Index

        if (TableNextColumn(); Selectable(item.Name.c_str(), item.IsSelected, ImGuiSelectableFlags_SpanAllColumns)) {
            // Here we implement the common Windows like select stuff
            // Shift held  - Select everything in-between previous selected and this
            // CTRL  held  - Mark this as selected, but don't unselect others
            // no modifier - Usual behaviour, mark this as selected, but unselect others

            if (GetIO().KeyShift) { // Select all visible items in-between
                const auto other = rng::find_if(visibleItems, [](auto& l) { return l.IsSelected; }); // Find other selected
                if (other != visibleItems.end()) {
                    const auto [b, e] = std::minmax(other, it, [](auto ai, auto bi) {return ai.base() < bi.base(); });
                    rng::for_each(b, e, [](auto& l) { l.IsSelected = true; }); // Select all in-between
                }
            } else if (!GetIO().KeyCtrl) { // Unselect all others
                rng::for_each(m_SavedLocations, [](auto& l) { l.IsSelected = false; });
            }
            item.IsSelected = true;
        }

        // Teleport on double click
        if (IsItemHovered() && IsMouseDoubleClicked(0)) {
            DoTeleportTo(item.FindGroundZ ? GetPositionWithGroundHeight(item.Pos) : item.Pos, item.AreaCode);
        }

        // Position text
        TableNextColumn();
        if (item.FindGroundZ) {
            Text("%.2f %.2f", item.Pos.x, item.Pos.y);
        } else {
            Text("%.2f %.2f %.2f", item.Pos.x, item.Pos.y, item.Pos.z);
        }

        // Area code ID text
        TableNextColumn();
        Text("%d %s", item.AreaCode, (item.AreaCode == AREA_CODE_NORMAL_WORLD) ? "(outside)" : "");

        // Ground text
        TableNextColumn();
        Text("%s", item.FindGroundZ ? "Yes" : "No");

        EndGroup();
        PopID();
    }

    EndTable();

    // Selected item deletion
    if (ImGui::IsKeyChordPressed(ImGuiKey_Delete)) {
        const auto [b, e] = rng::remove_if(visibleItems, [](auto& l) { return l.IsSelected; });
        m_SavedLocations.erase(b.base(), e.base());
    }
}

void TeleportDebugModule::RenderTeleporterWindow() {
    const auto& AlignRight = [&](float pad) {
        return GetWindowContentRegionMax().x - pad;
    };

    PushItemWidth(140.f);
    InputFloat3("Coords", reinterpret_cast<float(&)[3]>(m_Input.Pos), "%.3f");
    PopItemWidth();

    PushItemWidth(30.f);
    SameLine();
    InputInt("Area", &m_Input.AreaCode, 0);
    PopItemWidth();

    PushItemWidth(AlignRight(CalcTextSize("Name").x + 275.0f));
    SameLine();
    InputText("Name", m_Input.Name, std::size(m_Input.Name));
    PopItemWidth();

    Checkbox("Z = Ground Z", &m_FindGroundZ);

    // Helper to draw tooltip over hovered item
    const auto HoveredItemTooltip = [](auto text) {
        if (IsItemHovered()) {
            BeginTooltip();
            TextUnformatted(text);
            EndTooltip();
        }
    };

    // Teleport button
    SameLine(AlignRight(64.0f + 9.0f + 36.0f)); // [teleport] + space + [save]
    if (Button("Teleport", { 64.0f, 19.0f })) {
        // Not finding ground for Z doesn't make sense with the two. So do it anyways.
        bool forceGround = false;

        if (GetIO().KeyAlt) { // Random position
            forceGround = true;
            m_Input.AreaCode = AREA_CODE_NORMAL_WORLD;
            m_Input.Pos = CVector{ // todo: see CWorld::IsInWorldBounds
               CGeneral::GetRandomNumberInRange(-3072.0f, 3072.0f),
               CGeneral::GetRandomNumberInRange(-3072.0f, 3072.0f),
               0.0f
            };
        } else if (GetIO().KeyShift) { // Teleport to marker set on the map
            // Code from CLEO4 library
            auto hMarker = FrontEndMenuManager.m_nTargetBlipIndex;
            auto pMarker = &CRadar::ms_RadarTrace[LOWORD(hMarker)];

            if (hMarker && pMarker) {
                forceGround = true;
                m_Input.AreaCode = AREA_CODE_NORMAL_WORLD;
                m_Input.Pos = CVector{
                    pMarker->m_vPosition.x,
                    pMarker->m_vPosition.y,
                    0.0f
                };
            }
        }
        DoTeleportTo(
            (m_FindGroundZ || forceGround)
                ? GetPositionWithGroundHeight(m_Input.Pos)
                : m_Input.Pos,
            static_cast<eAreaCodes>(m_Input.AreaCode)
        );
    }
    HoveredItemTooltip("Hold `ALT` to teleport to a random position\nHold `SHIFT` to teleport to marker marked on the map");

    // Save position button
    SameLine(AlignRight(36.0f));
    if (Button("Save", { 36.0f, 19.0f })) {
        const auto posToSave{ GetIO().KeyCtrl ? FindPlayerPed()->GetPosition() : GetPositionWithGroundHeight(m_Input.Pos) };
        const auto areaToSave{ GetIO().KeyCtrl ? FindPlayerPed()->m_nAreaCode : static_cast<eAreaCodes>(m_Input.AreaCode) };
        const auto nameToSave{ m_Input.Name[0] ? m_Input.Name : ((areaToSave == AREA_CODE_NORMAL_WORLD) ? GxtCharToUTF8(CTheZones::GetZoneName(posToSave)) : "<Unnamed>")};

        // Either use given name or current zone name
        m_SavedLocations.emplace_back(nameToSave, posToSave, areaToSave, m_FindGroundZ);

        if (!GetIO().KeyAlt) {
            m_Input.Name[0] = 0; // Clear input
        }
    }
    HoveredItemTooltip("Hold `ALT` to prevent name input from getting cleared\nHold `CTRL` to save current position");

    // Current position display
    {
        const auto pos = FindPlayerCoors();
        Text("Current Pos: %.3f, %.3f, %.3f", pos.x, pos.y, pos.z);
        if (SameLine(); Button("Copy")) {
            char buf[256];
            sprintf_s(buf, "%.3f, %.3f, %.3f", pos.x, pos.y, pos.z);
            SetClipboardText(buf);
        }
    }


    if (!m_SavedLocations.empty()) {
        // Search tool input
        PushItemWidth(AlignRight(CalcTextSize("Search").x + 11.0f));
        InputText("Search", m_Input.Search, std::size(m_Input.Search), ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AutoSelectAll);

        // Finally, render saved positions
        RenderSavedPositions();
    }
}

void TeleportDebugModule::ProcessShortcuts() { 
    if (GetIO().MouseDrawCursor) { // Cursor visible => Ignore
        return;
    }

    const auto pad = CPad::GetPad(0);

    // Key `0` is previous location, so teleport to it (if set)
    if (pad->IsStandardKeyJustDown('0') && m_PrevLocation.IsSelected) {
        DoTeleportTo(m_PrevLocation.Pos, m_PrevLocation.AreaCode);
    }

    // Keys `1` - `9` are saved locations (Corresponding to the nth (currently) visible location (in the table))
    for (auto k = 0; k < 9; k++) {
        if (!pad->IsStandardKeyJustDown('1' + k)) {
            continue;
        }

        // Find `i`th visible element and teleport to it
        for (auto&& [itemIdx, loc] : notsa::enumerate(GetVisibleItems())) {
            if (k == itemIdx) {
                DoTeleportTo(loc.FindGroundZ ? GetPositionWithGroundHeight(loc.Pos) : loc.Pos, loc.AreaCode);
                break;
            }
        }

        return;
    }
}

void TeleportDebugModule::AddSettingsHandler(ImGuiContext* ctx) {
    ImGuiSettingsHandler ih;
    ih.TypeName   = "SavedLocations";
    ih.TypeHash   = ImHashStr("SavedLocations");
    ih.UserData   = this;
    ih.ClearAllFn = [](ImGuiContext* ctx, ImGuiSettingsHandler* handler) {};
    ih.WriteAllFn = [](ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* out_buf) {}; // We don't use imgui to save this data anymore
    ih.ReadLineFn = [](ImGuiContext* ctx, ImGuiSettingsHandler* handler, void* entry, const char* line) {
        UNUSED(ctx);

        const auto self    = (TeleportDebugModule*)handler->UserData;
        const auto version = reinterpret_cast<int>(entry);

        // If already read from the JSON just skip this part
        if (self->m_IsStateFromDeserialization) {
            return;
        }

        char name[1024]{};
        CVector pos{};

        switch (version) {
        case 2: {
            int area{};
            int findGround{1};

            // [^\t\n] -> https://stackoverflow.com/questions/2854488
            if (sscanf_s(line, "%f, %f, %f, %d, %d, %[^\t\n]", &pos.x, &pos.y, &pos.z, &area, &findGround, SCANF_S_STR(name)) == 6) {
                self->m_SavedLocations.emplace_back(name, pos, static_cast<eAreaCodes>(area), static_cast<bool>(findGround));
                return;
            }
        } break;
        case 1: {
            if (sscanf_s(line, "%f, %f, %f, %[^\t\n]", &pos.x, &pos.y, &pos.z, SCANF_S_STR(name)) == 4) {
                self->m_SavedLocations.emplace_back(name, pos);
                return;
            }
            break;
        }
        default: {
            NOTSA_LOG_ERR("Invalid teleport tool version: {}", version);
            return;
        }
        }

        if (line[0] && line[0] != '\n') { // Report failed reads on non-empty lines only
            NOTSA_LOG_ERR("Failed to load saved location from ini: `{}`", line);
        }
    };
    ih.ApplyAllFn = [](ImGuiContext* ctx, ImGuiSettingsHandler* handler) {
        UNUSED(ctx);

        const auto self = (TeleportDebugModule*)handler->UserData;

        // Add some default locations if first ever use
        if (self->m_SavedLocations.empty() && self->m_IsFirstUseEver) {
            self->m_SavedLocations = {
                { "Map Center", { 0.f,     0.f,      0.f   }, AREA_CODE_NORMAL_WORLD, true },
                { "CJ's House", { 2495.2f, -1686.0f, 13.6f }, AREA_CODE_NORMAL_WORLD, true },
                { "SF Airport", { 2495.1f, 1658.2f,  13.5f }, AREA_CODE_NORMAL_WORLD, true }
            };
        }

        self->m_IsFirstUseEver = false;
    };
    ih.ReadOpenFn = [](ImGuiContext* ctx, ImGuiSettingsHandler* handler, const char* name) {
        UNUSED(ctx);
        UNUSED(handler);

        int version{2};
        VERIFY(sscanf_s(name, "Version %d", &version) == 1);
        return (void*)version; // Has to return a non-null value, otherwise `ReadLine` won't be called
    };
    ctx->SettingsHandlers.push_back(ih);
}
