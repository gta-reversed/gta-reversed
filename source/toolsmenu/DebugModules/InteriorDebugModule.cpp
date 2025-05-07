#include <StdInc.h>
#include "InteriorDebugModule.h"
#include "InteriorManager_c.h"
#include "Interior/Interior_c.h"
using namespace ImGui;

static const char* TYPE_TEXT[] = {
    "SHOP", "OFFICE", "LOUNGE", "BEDROOM", "KITCHEN", "BATHROOM", "OFFLICENSE", "HOTELROOM", "MISC", "TESTROOM"
};

const static auto s_IntTypeNames = notsa::make_mapping<eInteriorType, const char*>({
    { eInteriorType::SHOP,       "SHOP"       },
    { eInteriorType::OFFICE,     "OFFICE"     },
    { eInteriorType::LOUNGE,     "LOUNGE"     },
    { eInteriorType::BEDROOM,    "BEDROOM"    },
    { eInteriorType::KITCHEN,    "KITCHEN"    },
    { eInteriorType::BATHROOM,   "BATHROOM"   },
    { eInteriorType::OFFLICENSE, "OFFLICENSE" },
    { eInteriorType::HOTELROOM,  "HOTELROOM"  },
    { eInteriorType::MISC,       "MISC"       },
    { eInteriorType ::TESTROOM,  "TESTROOM"   },
});

namespace notsa {
namespace debugmodules {
void InteriorDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "Interior", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    RenderSelectorTab();
    RenderPropsTab();

    //if (BeginTabBar("InteriorDebug")) {
    //    RenderSelectorTab();
    //    RenderPropsTab();
    //
    //    EndTabBar();
    //}
}

void InteriorDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Interior", nullptr, &m_IsOpen);
    });
}

void InteriorDebugModule::RenderSelectorTab() {
    if (!TreeNode("Selector")) {
        return;
    }

    {
        notsa::ui::ScopedDisable d{m_UsePlayersInt};
        InputInt("Interior ID", &m_SelectedIntIdx, 1, 1);
        m_SelectedIntIdx = std::clamp(m_SelectedIntIdx, 0, (int)std::size(g_interiorMan.m_Interiors));
    }

    SameLine();
    Checkbox("Use Player's", &m_UsePlayersInt);
    
    m_Int = m_UsePlayersInt
        ? g_interiorMan.GetPedsInterior(FindPlayerPed())
        : &g_interiorMan.m_Interiors[m_SelectedIntIdx];

    TreePop();
}

void InteriorDebugModule::RenderPropsTab() {
    notsa::ui::ScopedDisable d{!m_Int};

    if (m_Int && m_Int->m_ID) {
        if (TreeNode("Basic")) {
            Text("Interior ID: %d", m_Int->m_ID);
            Text("Group:       0x%X", m_Int->m_Group);
            Text("Area Code:   %d", m_Int->m_AreaCode);

            TreePop();
        }
        if (TreeNode("Props")) {
            if (auto* const p = m_Int->m_Props) {
                Text("Type:     %s\n",   p->m_IntType);
                Text("Group ID: %d\n",   p->m_groupId);
                Text("Width:    %d\n",   p->m_width);
                Text("Height:   %d\n",   p->m_height);
                Text("Depth:    %d\n",   p->m_depth);
                Text("Door:     %d\n",   p->m_door);
                Text("Seed:     %d\n",   p->m_seed);
                Text("Status:   %d\n",   p->m_status);
                Text("Rot :     %.2f\n", p->m_rot);
            } else {
                Text("<Not Available>");
            }
            TreePop();
        }
    } else {
        Text("<Not Available>");
    }

}
}; // namespace debugmodules
}; // namespace notsa
