#pragma once

#include "DebugModule.h"
#include <vector>
#include <optional>

namespace notsa { 
namespace debugmodules {
class TwoDEffectsDebugModule final : public DebugModule {
    struct InRange2DFx {
        CEntity*       Entity;       //!< The entity this fx belongs to
        C2dEffectBase* Fx;           //!< The 2D effect
        CVector        FxPos;        //!< The world position of the fx (Calculated using the entity's matrix)
        float          DistToPlayer; //!< Distance of the effect to the player
        size_t         TblIdx;       //!< # in the table
        ImGuiID        Hash;         //!< Hash identifying of this object
    };

public:
    void RenderWindow() override;
    void RenderMenuEntry() override;
    void Render3D() override;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(TwoDEffectsDebugModule, m_IsOpen);

private:
    //! Update m_EntitiesInRange/m_FxInRange
    void UpdateEntitiesAndEffectsInRange();

    //! Render table of nearby effects
    void RenderNearbyEffectsTable();

    //! Render details pane of the selected fx
    void RenderSelectedEffectDetails();

    //! Render bounding boxes of effects
    void RenderEffectBB(const InRange2DFx&);

    //! Render extra details (such as direction lines for attractors, etc)
    void RenderSelectedEffectDetails3D();

private:
    bool                       m_IsOpen{false};
    bool                       m_AllBBsEnabled{false};
    int                        m_MaxEntities{ 512 };
    float                      m_Range{ 100.f };
    std::vector<CEntity*>      m_EntitiesInRange{}; //!< All entities (buildings + objects) in range (`m_Range`)
    std::vector<InRange2DFx>   m_FxInRange{};       //!< All effects of the entitites in range
    std::optional<InRange2DFx> m_SelectedFx{};      //!< Selected fx (Hash == m_SelectedFxHash)
    ImGuiID                    m_SelectedFxHash{};  //!< ID of the selected fx (this is so that even if the fx goes out of range later it will still pop up as selected)
};
}; // namespace debugmodules
}; // namespace notsa
