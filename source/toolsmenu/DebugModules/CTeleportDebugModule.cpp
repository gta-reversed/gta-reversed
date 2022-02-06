#include "StdInc.h"

#ifdef EXTRA_DEBUG_FEATURES
#include "CTeleportDebugModule.h"
#include "Utility.h"
#include "CDebugMenu.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <iterator>
#include <string>
#include <string_view>
#include <chrono>
#include "Radar.h"

namespace rng = std::ranges;
using namespace ImGui;

struct SavedLocation {
    std::string name{};
    CVector     pos{};
    eAreaCodes  areaCode{};
    bool        selected{};
};

static std::vector<SavedLocation> s_SavedLocations{};
static bool                       s_windowOpen{ true };

namespace TeleportDebugModule {

CVector GetPositionWithGroundHeight(CVector2D pos) {
    return { pos.x, pos.y, CWorld::FindGroundZForCoord(pos.x, pos.y) + 2.f };
}

// Teleport to exact coordinates, and set player to be in the normal world
void DoTeleportTo(CVector pos, eAreaCodes areaCode = eAreaCodes::AREA_CODE_NORMAL_WORLD) {
    CStreaming::LoadSceneCollision(pos);
    CStreaming::LoadScene(pos);

    FindPlayerPed()->m_nAreaCode = areaCode;
    CGame::currArea = areaCode;
    CStreaming::RemoveBuildingsNotInArea(areaCode);
    CTimeCycle::StopExtraColour(false);

    // For now just teleport to ground height
    if (auto vehicle = FindPlayerVehicle()) {
        vehicle->SetPosn(pos);
        vehicle->ResetTurnSpeed();
        vehicle->ResetMoveSpeed();
        vehicle->ResetFrictionMoveSpeed();
        vehicle->ResetFrictionTurnSpeed();
    }
    else
        FindPlayerPed()->Teleport(pos, true);
}

void ProcessImGui() {
    ImGui::SetNextWindowSize({ 400.f, 350.f }, ImGuiCond_FirstUseEver);
    ImGui::Begin("Teleporter Window", &s_windowOpen);

    static CVector pos{};
    PushItemWidth(140.f);
    InputFloat3("", reinterpret_cast<float(&)[3]>(pos), "%.3f"); // Kinda hacky, but it's okay, if this was to break, we'd have bigger problems anyways.
    PopItemWidth();

    static int areaCode{};
    PushItemWidth(40.f);
    SameLine(); InputInt("", &areaCode, 0);
    PopItemWidth();

    // Name input
    static char name[1024]{};
    PushItemWidth(140.f);
    SameLine(); InputText("Name", name, std::size(name));
    PopItemWidth();

    static bool findZGround = true;
    Checkbox("Ground for Z", &findZGround);

    // Helper to draw tooltip over hovered item
    const auto HoveredItemTooltip = [](auto text) {
        if (IsItemHovered()) {
            BeginTooltip();
            TextUnformatted(text);
            EndTooltip();
        }
    };

    // Teleport button
    if (SameLine(); Button("Teleport")) {
        // Not finding ground for Z doesn't make sense with the two. So do it anyways.
        if (GetIO().KeyAlt) { // Random position
            findZGround = true;
            areaCode = eAreaCodes::AREA_CODE_NORMAL_WORLD;
            pos = CVector{
               CGeneral::GetRandomNumberInRange(-3072.0f, 3072.0f),
               CGeneral::GetRandomNumberInRange(-3072.0f, 3072.0f),
               0.0f
            };
        }
        else if (GetIO().KeyShift) { // Teleport to marker set on the map
            // Code from CLEO4 library
            auto hMarker = FrontEndMenuManager.m_nTargetBlipIndex;
            auto pMarker = &CRadar::ms_RadarTrace[LOWORD(hMarker)];

            if (hMarker && pMarker) {
                findZGround = true;
                areaCode = eAreaCodes::AREA_CODE_NORMAL_WORLD;
                pos = CVector{
                    pMarker->m_vPosition.x,
                    pMarker->m_vPosition.y,
                    0.0f
                };
            }
        }
        DoTeleportTo(findZGround ? GetPositionWithGroundHeight(pos) : pos, static_cast<eAreaCodes>(areaCode));
    }
    HoveredItemTooltip("Hold `ALT` to teleport to a random position\nHold `SHIFT` to teleport to marker marked on the map");

    // Save position button
    if (SameLine(); Button("Save")) {
        const auto posToSave{ GetIO().KeyCtrl ? FindPlayerPed()->GetPosition() : GetPositionWithGroundHeight(pos) };
        const auto areaToSave{ GetIO().KeyCtrl ? FindPlayerPed()->m_nAreaCode : static_cast<eAreaCodes>(areaCode) };

        // Either use given name or current zone name
        s_SavedLocations.emplace(s_SavedLocations.begin(), name[0] ? name : CTheZones::GetZoneName(posToSave), posToSave, areaToSave);

        if (!GetIO().KeyAlt) {
            name[0] = 0; // Clear input
        }
    }
    HoveredItemTooltip("Hold `ALT` to prevent name input from getting cleared\nHold `CTRL` to save current position");

    if (!s_SavedLocations.empty()) {

        // Search tool input 
        static char searchInput[1024]{};
        InputText("Search", searchInput, std::size(searchInput), ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AutoSelectAll);
        const std::string_view searchInputSV{ searchInput };

        // Filter views stuff
        const auto IsItemVisible = [&](auto& l) { return searchInputSV.empty() || findStringCaseInsensitive(l.name, searchInputSV); };
        const auto visibleFilter = std::views::filter(IsItemVisible);
        auto visibleItems = s_SavedLocations | visibleFilter;

        // Saved positions table
        if (BeginTable("Saved Positions", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY)) {
            TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
            TableSetupColumn("Position", ImGuiTableColumnFlags_WidthFixed);
            TableSetupColumn("Area code", ImGuiTableColumnFlags_WidthFixed);
            TableHeadersRow();

            // Draw visible items
            for (auto it = visibleItems.begin(); it != visibleItems.end(); it++) {
                auto& v = *it;

                PushID(&v);
                BeginGroup();

                TableNextRow();

                if (TableNextColumn(); Selectable(v.name.c_str(), v.selected, ImGuiSelectableFlags_SpanAllColumns)) {
                    // Here we implement the common Windows like select stuff
                    // Shift held  - Select everything in-between previous selected and this
                    // CTRL  held  - Mark this as selected, but don't unselect others
                    // no modifier - Usual behaviour, mark this as selected, but unselect others

                    if (GetIO().KeyShift) { // Select all visible items in-between
                        const auto other = rng::find_if(visibleItems, [](auto& l) { return l.selected; }); // Find other selected
                        if (other != visibleItems.end()) {
                            const auto [b, e] = std::minmax(other, it, [](auto a, auto b) {return a.base() < b.base(); });
                            rng::for_each(b, e, [](auto& l) { l.selected = true; }); // Select all in-between
                        }
                    } else if (!GetIO().KeyCtrl) { // Unselect all others
                        rng::for_each(s_SavedLocations, [](auto& l) { l.selected = false; });
                    }
                    v.selected = true;
                }

                // Teleport on double click
                if (IsItemHovered() && IsMouseDoubleClicked(0)) {
                    DoTeleportTo(findZGround ? GetPositionWithGroundHeight(v.pos) : v.pos, v.areaCode);
                }

                // Position text
                TableNextColumn(); Text("%.2f %.2f %.2f", v.pos.x, v.pos.y, v.pos.z);
                
                // Area code ID text
                TableNextColumn(); Text("%d %s", v.areaCode, (v.areaCode == AREA_CODE_NORMAL_WORLD) ? "(outside)" : "");

                EndGroup();
                PopID();
            }

            EndTable();

            // Selected item deletion
            if (GetIO().KeysDown[VK_DELETE]) {
                const auto [b, e] = rng::remove_if(visibleItems, [](auto& l) { return l.selected; });
                s_SavedLocations.erase(b.base(), e.base());
            }
        }
    }

    End();
}

void ProcessRender() {}

namespace SettingsHandler {
    // Clear all settings
    void ClearAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler) {
        (void)ctx;
        (void)handler;

        //s_SavedLocations.clear();
    }

    // Called after every read entry line
    // `entry` is a pointer returned from `ReadOpenFn` (we dont use it in this case)
    void ReadLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler, void* entry, const char* line) {
        (void)entry;
        (void)handler;

        char name[1024]{};
        CVector pos{};
        int area{};
        if (sscanf(line, "%f, %f, %f, %d, %[^\t\n]", &pos.x, &pos.y, &pos.z, &area, name) == 5) { // [^\t\n] -> https://stackoverflow.com/questions/2854488
            s_SavedLocations.emplace_back(name, pos, static_cast<eAreaCodes>(area));
        } else if (line[0] && line[0] != '\n') { // Report failed reads on non-empty lines only
            std::cerr << "Failed to load saved location from ini: `" << line << "`\n";
        }
    }

    // Write all settings to buffer (ini file)
    void WriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* out_buf) {
        (void)ctx;

        out_buf->reserve(s_SavedLocations.size() * (3 * 10 + 30)); // Estimate. 3 * 10 for position + 30 for name

        out_buf->append("[SavedLocations][Version 1]\n");
        for (const auto& l : s_SavedLocations) {
            out_buf->appendf("%.3f, %.3f, %.3f, %d, %s\n", l.pos.x, l.pos.y, l.pos.z, l.areaCode, l.name.c_str());
        }
    }

    // Required to have, but for us it's a NOP
    void* ReadOpen(ImGuiContext* ctx, ImGuiSettingsHandler* handler, const char* name) {
        (void)ctx;
        (void)handler;
        (void)name;
        return (void*)1; // Has to return a non-null value, otherwise `ReadLine` wont be called
    }

    void ApplyAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler) {
        (void)ctx;
        (void)handler;

        if (s_SavedLocations.empty()) {
            s_SavedLocations = {
                {"Map centre", {0.f, 0.f, 0.f}},
                {"CJ's House", {2495.2f, -1686.0f, 13.6f}},
                {"SF Airport", {2495.1f, 1658.2f, 13.5f}}
            };
        }
    }
}; // namespace SettingsHandler

void Initialise(ImGuiContext& ctx) {
    ImGuiSettingsHandler ini_handler;
    ini_handler.TypeName = "SavedLocations";
    ini_handler.TypeHash = ImHashStr("SavedLocations");
    ini_handler.ClearAllFn = SettingsHandler::ClearAll;
    ini_handler.ReadOpenFn = SettingsHandler::ReadOpen;
    ini_handler.ReadLineFn = SettingsHandler::ReadLine;
    ini_handler.ApplyAllFn = SettingsHandler::ApplyAll;
    ini_handler.WriteAllFn = SettingsHandler::WriteAll;
    ctx.SettingsHandlers.push_back(ini_handler);
}
}; // namespace TeleportDebugModule
#endif
