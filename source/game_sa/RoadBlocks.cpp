#include "StdInc.h"

#include "RoadBlocks.h"

void CRoadBlocks::InjectHooks() {
    RH_ScopedClass(CRoadBlocks);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x461100, { .reversed = false });
    RH_ScopedInstall(ClearScriptRoadBlocks, 0x460EC0, { .reversed = false });
    RH_ScopedInstall(ClearSpaceForRoadBlockObject, 0x461020, { .reversed = false });
    RH_ScopedInstall(CreateRoadBlockBetween2Points, 0x4619C0, { .reversed = false });
    RH_ScopedInstall(GenerateRoadBlockCopsForCar, 0x461170, { .reversed = false });
    RH_ScopedInstall(GenerateRoadBlocks, 0x4629E0, { .reversed = false });
    RH_ScopedInstall(GetRoadBlockNodeInfo, 0x460EE0, { .reversed = false });
    RH_ScopedInstall(RegisterScriptRoadBlock, 0x460DF0, { .reversed = false });
}

// 0x461100
void CRoadBlocks::Init() {
    plugin::Call<0x461100>();
}

// 0x460EC0
void CRoadBlocks::ClearScriptRoadBlocks() {
    plugin::Call<0x460EC0>();
}

// 0x461020
void CRoadBlocks::ClearSpaceForRoadBlockObject(CVector a1, CVector a2) {
    plugin::Call<0x461020, CVector, CVector>(a1, a2);
}

// 0x4619C0
void CRoadBlocks::CreateRoadBlockBetween2Points(CVector a1, CVector a2, uint32 a3) {
    plugin::Call<0x4619C0, CVector, CVector, uint32>(a1, a2, a3);
}

// 0x461170
void CRoadBlocks::GenerateRoadBlockCopsForCar(CVehicle* vehicle, int32 pedsPositionsType, ePedType type) {
    plugin::Call<0x461170, CVehicle*, int32, ePedType>(vehicle, pedsPositionsType, type);
}

// 0x4629E0
void CRoadBlocks::GenerateRoadBlocks() {
    ZoneScoped;

    plugin::Call<0x4629E0>();
}

// 0x460EE0
bool CRoadBlocks::GetRoadBlockNodeInfo(CNodeAddress a1, float& a2, CVector& a3) {
    return plugin::CallAndReturn<bool, 0x460EE0, CNodeAddress, float, CVector>(a1, a2, a3);
}

// 0x460DF0
void CRoadBlocks::RegisterScriptRoadBlock(CVector a1, CVector a2, bool a3) {
    plugin::Call<0x460DF0, CVector, CVector, bool>(a1, a2, a3);
}
