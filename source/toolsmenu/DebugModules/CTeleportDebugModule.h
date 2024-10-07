#pragma once

#include "DebugModule.h"
#include "Utility.h"

struct ImGuiSettingsHandler;

class TeleportDebugModule final : public DebugModule {
public:
    void OnImGuiInitialised(ImGuiContext* ctx) override;
    void RenderWindow() override;
    void RenderMenuEntry() override;
    void Update() override;

    json Serialize() const final override { return *this; }
    void Deserialize(const json& j) final override;
    std::string_view GetID() const final { return "TeleportDebugModule"; }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(TeleportDebugModule, m_IsOpen);

    static void TeleportTo(const CVector& pos, eAreaCodes areaCode = eAreaCodes::AREA_CODE_NORMAL_WORLD);

private:
    void DoTeleportTo(CVector pos, eAreaCodes areaCode);
    void RenderSavedPositions();
    void RenderTeleporterWindow();
    auto GetVisibleItems() {
        return m_SavedLocations | rng::views::filter(
            [
                filter = std::string_view{ m_Input.Search }
            ](auto& l) {
                return filter.empty() || StringContainsString(l.Name, filter, false);
            }
        );
    }

    void ProcessShortcuts();

    //! Legacy ImGUI backed data storage. Replaced by the Serialization/Deserialization DebugModule API.
    void AddSettingsHandler(ImGuiContext* ctx);

private:
    struct SavedLocation {
        std::string Name{};
        CVector     Pos{};
        eAreaCodes  AreaCode{ eAreaCodes::AREA_CODE_NORMAL_WORLD };
        bool        FindGroundZ{ true };
        bool        IsSelected{ false };

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(SavedLocation, Name, Pos, AreaCode, FindGroundZ, IsSelected);
    };

private:
    bool                       m_IsOpen{ false };
    bool                       m_IsStateFromDeserialization{ false };
    bool                       m_IsFirstUseEver{ true };
    std::vector<SavedLocation> m_SavedLocations{};
    bool                       m_FindGroundZ{ true };
    SavedLocation              m_PrevLocation{};

    struct InputData {
        char    Name[256]{};
        char    Search[256]{};
        CVector Pos{};
        int     AreaCode{};

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(InputData, Name, Search, Pos, AreaCode);
    } m_Input;
};
