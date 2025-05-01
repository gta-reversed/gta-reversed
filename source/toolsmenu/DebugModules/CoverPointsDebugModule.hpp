#pragma once

#include "DebugModule.h"

class CCoverPoint;

namespace notsa { 
namespace debugmodules {
class CoverPointsDebugModule final : public DebugModule {
protected:
    struct InRangeCoverPoint {
        float        DistToPlayer;
        CCoverPoint* CoverPoint;
        size_t       TblIdx;
    };

public:
    void RenderWindow() override;
    void RenderMenuEntry() override;
    void Render3D() override;
    void Update() override;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(CoverPointsDebugModule, m_IsOpen, m_IsShowPointsEnabled, m_AllBBsEnabled, m_Range);

protected:
    void UpdateCoverPointsInRange();
    void RenderCoverPointsTable();
    void RenderSelectedCoverPointDetails();
    void RenderCoverPointDetails3D(const CCoverPoint& cpt);
    bool IsCoverPointValid(const CCoverPoint& cpt);

private:
    bool                           m_IsOpen{};
    bool                           m_IsShowPointsEnabled{};
    bool                           m_AllBBsEnabled{};
    float                          m_Range{ 100.f };
    CCoverPoint*                   m_SelectedCpt{};
    std::vector<InRangeCoverPoint> m_CptsInRange{};
};
}; // namespace debugmodules
}; // namespace notsa
