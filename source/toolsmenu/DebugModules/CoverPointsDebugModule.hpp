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

protected:
    void UpdateCoverPointsInRange();
    void RenderCoverPointsTable();
    void RenderSelectedCoverPointDetails();
    void RenderSelectedCoverPointDetails3D();
    void RenderCoverPointBB(const CCoverPoint& cpt);
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
