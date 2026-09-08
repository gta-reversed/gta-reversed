#pragma once

#include "DebugModule.h"
#include "Garages.h"

struct ImGuiTableColumnSortSpecs;
class CGarage;

namespace notsa { 
namespace debugmodules {
class Garage final : public DebugModule {
    struct GarageInfo {
        enum TableColumn {
            CNAME,
            CTYPE,
            CDISTANCE
        };
    public:
        static std::partial_ordering ImGuiTableOrdering(const ImGuiTableColumnSortSpecs& tss, const GarageInfo& a, const GarageInfo& b);

        GarageInfo(CGarage& g) :
            g{&g},
            name{g.m_Name},
            distSq{}, dist{}
        {
        }

    public:
        CGarage* g{};
        std::string_view name{};           // sv of g->m_Name
        float            distSq{}, dist{}; // Distance from player [updated each frame]
    };
public:
    void RenderWindow() override;
    void RenderMenuEntry() override;

    void HandleEvent(const notsa::events::OnGameInit&) override;
private:
    bool                           m_IsOpen{};
    bool                           m_SortByDist{ true };
    CGarage*                       m_Selected{};
    std::vector<GarageInfo> m_GarageInfos{};
};
}; // namespace debugmodules
}; // namespace notsa
