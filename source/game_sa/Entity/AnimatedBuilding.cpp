#include "StdInc.h"

#include "AnimatedBuilding.h"

void CAnimatedBuilding::InjectHooks()
{
    RH_ScopedVirtualClass(CAnimatedBuilding, 0x8639B0, 22);
    RH_ScopedCategory("Entity");

    RH_ScopedVMTInstall(ProcessControl, 0x537A30);
    RH_ScopedVMTInstall(ProcessCollision, 0x537A40);
    RH_ScopedVMTInstall(PreRender, 0x403F00);
}

// 0x537A30
void CAnimatedBuilding::ProcessControl()
{}

// 0x537A40
void CAnimatedBuilding::ProcessCollision()
{
    m_bIsInSafePosition = true;
}

// 0x403F00
void CAnimatedBuilding::PreRender()
{
    if (!m_pRwObject)
        return;

    CEntity::PreRender();
    CEntity::UpdateRwFrame();
    CEntity::UpdateRpHAnim();
}