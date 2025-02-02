#include <StdInc.h>
#include "CheckpointsDebugModule.hpp"
#include "Checkpoints.h"

namespace notsa { 
namespace debugmodules {
void CheckpointsDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "Checkpoints", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    // Type
    constexpr const char* s_TypeNames[]{
        "TUBE",
        "ENDTUBE",
        "EMPTYTUBE",
        "TORUS",
        "TORUS_NOFADE",
        "TORUSROT",
        "TORUSTHROUGH",
        "TORUS_UPDOWN",
        "TORUS_DOWN",
    };
    constexpr eCheckpointType s_TypeValues[]{
        eCheckpointType::TUBE,
        eCheckpointType::ENDTUBE,
        eCheckpointType::EMPTYTUBE,
        eCheckpointType::TORUS,
        eCheckpointType::TORUS_NOFADE,
        eCheckpointType::TORUSROT,
        eCheckpointType::TORUSTHROUGH,
        eCheckpointType::TORUS_UPDOWN,
        eCheckpointType::TORUS_DOWN,
    };

    const auto PlaceOne = [&, this](eCheckpointType type, CVector pos) {
        const CRGBA color{m_Color};
        CCheckpoints::PlaceMarker(
            (uint32)m_CurrentID++,
            type,
            pos,
            m_Dir,
            m_Size,
            color,
            (uint16)m_PulsePeriod,
            m_PulseFraction,
            (int16)m_RotateRate
        );
    };

    // ID
    {
        notsa::ui::ScopedID id{"ID"};
        ImGui::InputInt("ID", &m_CurrentID, 1, 10);
    }

    // Type
    {
        notsa::ui::ScopedID id{"Type"};
        ImGui::ListBox("Type", &m_SelectedTypeIdx, s_TypeNames, IM_ARRAYSIZE(s_TypeNames));
        if (ImGui::IsItemFocused() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            PlaceOne(s_TypeValues[m_SelectedTypeIdx], m_Pos);
        }
        if (ImGui::SameLine(); ImGui::Button("Random") || m_PlaceOneRandom) {
            m_SelectedTypeIdx = CGeneral::GetRandomNumberInRange<int>(0, IM_ARRAYSIZE(s_TypeNames));
        }
    }

    // Position
    {
        notsa::ui::ScopedID id{"Position"};
        ImGui::InputFloat3("Position", reinterpret_cast<float*>(&m_Pos));
        ImGui::SameLine();
        ImGui::Checkbox("Use player's", &m_UsePlayersPosition);
        if (m_UsePlayersPosition) {
            m_Pos = FindPlayerCoors();
        }
    }

    // Direction
    {
        notsa::ui::ScopedID id{"Direction"};
        ImGui::InputFloat3("Direction", reinterpret_cast<float*>(&m_Dir));
        if (ImGui::SameLine(); ImGui::Button("Random") || m_PlaceOneRandom) {
            m_Dir = CVector::Random(0.f, 1.f).Normalized();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Use player's", &m_UsePlayersDir);
        if (m_UsePlayersDir) {
            m_Dir = FindPlayerPed()->GetForward();
        }
    }

    // Size
    {
        notsa::ui::ScopedID id{"Size"};
        ImGui::DragFloat("Size", &m_Size);
        if (ImGui::SameLine(); ImGui::Button("Random") || m_PlaceOneRandom) {
            m_Size = CGeneral::GetRandomNumberInRange(0.f, 20.f);
        }
    }

    // Color
    {
        notsa::ui::ScopedID id{"Color"};
        ImGui::ColorPicker4("Color", reinterpret_cast<float(&)[4]>(m_Color));
    }

    // Pulse Period
    {
        notsa::ui::ScopedID id{"Pulse Period"};
        ImGui::DragInt("Pulse Period", &m_PulsePeriod, 1.f, 0, UINT16_MAX);
        if (ImGui::SameLine(); ImGui::Button("Random") || m_PlaceOneRandom) {
            m_PulsePeriod = CGeneral::GetRandomNumberInRange<uint16>(0, UINT16_MAX);
        }
    }

    // Pulse Fraction
    {
        notsa::ui::ScopedID id{"Pulse Fraction"};
        ImGui::DragFloat("Pulse Fraction", &m_PulseFraction, 1.f, 0.f, 1.f);
        if (ImGui::SameLine(); ImGui::Button("Random") || m_PlaceOneRandom) {
            m_PulseFraction = CGeneral::GetRandomNumberInRange(0.f, 1.f);
        }
    }

    // Rotate Rate
    {
        notsa::ui::ScopedID id{"Rotate Rate"};
        ImGui::DragInt("Rotate Rate", &m_RotateRate, 1.f, INT16_MIN, INT16_MAX);
        if (ImGui::SameLine(); ImGui::Button("Random") || m_PlaceOneRandom) {
            m_RotateRate = CGeneral::GetRandomNumberInRange<int16>(INT16_MIN, INT16_MAX);
        }
    }

    // Place marker
    {
        if (ImGui::Button("Place") || m_PlaceOneRandom) {
            PlaceOne(s_TypeValues[m_SelectedTypeIdx], m_Pos);
        }

        ImGui::SameLine();
        m_PlaceOneRandom = ImGui::Button("Place Random");

        ImGui::SameLine();
        if (ImGui::Button("Place one of each type")) {
            for (int i = 0; i < IM_ARRAYSIZE(s_TypeValues); i++) {
                PlaceOne(s_TypeValues[i], m_Pos + m_Dir * ((m_Size + 1.f) * (float)i));
            }
        }
    }

    // Delete all
    {
        if (ImGui::Button("Clear All")) {
            rng::for_each(CCheckpoints::m_aCheckPtArray, &CCheckpoint::MarkAsDeleted);
        }
    }
}

void CheckpointsDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Checkpoints", nullptr, &m_IsOpen);
    });
}
}; // namespace debugmodules
}; // namespace notsa
