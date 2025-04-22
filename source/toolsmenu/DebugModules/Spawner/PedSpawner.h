#pragma once

#include "Base.h"
#include "Vector.h"

namespace PedSpawnerModule {

CPed* SpawnPed(int32 modelId, CVector position);
void SpawnRandomPed();

void Initialise();
void ProcessImGui();
void ProcessRender();

} // namespace PedSpawnerModule
