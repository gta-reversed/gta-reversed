#include <StdInc.h>
#include <Plugins/TwoDEffectPlugin/2dEffect.h>

#include "PedPlacement.h"
#include "PedAttractorPedPlacer.h"


void CPedAttractorPedPlacer::InjectHooks() {
    RH_ScopedClass(CPedAttractorPedPlacer);
    RH_ScopedCategory("Attractors");

    RH_ScopedGlobalInstall(PlacePedAtEffect, 0x5EA390);
}

// 0x5EA390
void CPedAttractorPedPlacer::PlacePedAtEffect(C2dEffectPedAttractor const& fx, CEntity* entity, CPed* ped, float forwardOffsetMultiplier) {
    const auto mat = entity
        ? entity->GetMatrix()
        : CMatrix::Unity();

    const auto fxPos      = CPedAttractorManager::ComputeEffectPos(&fx, mat);
    const auto fxUseDir   = CPedAttractorManager::ComputeEffectUseDir(&fx, mat);
    const auto fxFwdDir   = CPedAttractorManager::ComputeEffectForwardDir(&fx, mat);
    //const auto fxQueueDir = CPedAttractorManager::ComputeEffectQueueDir(&fx, mat); // NOTSA: Actually unused

    ped->SetPosn(std::get<CVector>(CPedPlacement::FindZCoorForPed(fxPos + fxFwdDir * forwardOffsetMultiplier)));

    const auto angle = CVector2D{ fxUseDir }.Heading();
    ped->GetMatrix().SetRotateZOnly(angle);
    ped->m_fAimingRotation = angle;
    ped->m_fCurrentRotation = angle;
}
