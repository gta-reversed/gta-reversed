#include "StdInc.h"

#include "RoadBlocks.h"

void CRoadBlocks::InjectHooks() {
    RH_ScopedClass(CRoadBlocks);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x461100);
    RH_ScopedInstall(ClearScriptRoadBlocks, 0x460EC0);
    RH_ScopedInstall(ClearSpaceForRoadBlockObject, 0x461020, { .reversed = false });
    RH_ScopedInstall(CreateRoadBlockBetween2Points, 0x4619C0, { .reversed = false });
    RH_ScopedInstall(GenerateRoadBlockCopsForCar, 0x461170, { .reversed = false });
    RH_ScopedInstall(GenerateRoadBlocks, 0x4629E0, { .reversed = false });
    RH_ScopedInstall(GetRoadBlockNodeInfo, 0x460EE0, { .reversed = false });
    RH_ScopedInstall(RegisterScriptRoadBlock, 0x460DF0, { .reversed = false });
}

inline uint32* ms_RoadBlockData1    = reinterpret_cast<uint32*>(0xA43438);
inline uint8*  ms_bRoadblockLoaded  = reinterpret_cast<uint8*>(0xA43584);
inline void*   ms_RoadBlockData2    = reinterpret_cast<void*>(0xA43580);
inline void*   ms_RoadBlockData3    = reinterpret_cast<void*>(0xA435A0);
inline void*   ms_RoadBlockSlots    = reinterpret_cast<void*>(0xA43AD0);
inline void*   ms_RoadBlockSlotsEnd = reinterpret_cast<void*>(0xA43C90);

void ReadFromFile(FILE* file, void* buffer, size_t count) {
    fread(buffer, 1, count, file);
}

// 0x461100
void CRoadBlocks::Init() {
    // Initialize 0x51 DWORDs with the pattern 0x01010101 at ms_RoadBlockData1
    std::fill_n(reinterpret_cast<uint32*>(ms_RoadBlockData1), 0x51, 0x01010101);

    // Clear the roadblock loaded flag
    *ms_bRoadblockLoaded = false;

    // Try opening the roadblock data file
    FILE* file = CFileMgr::OpenFile("data\\paths\\roadblox.dat", "rb");
    if (file) {
        // Read 4 bytes into ms_RoadBlockData2
        ReadFromFile(file, ms_RoadBlockData2, 4);

        // Read 0x514 bytes into ms_RoadBlockData3
        ReadFromFile(file, ms_RoadBlockData3, 0x514);

        // Close the file
        fclose(file);
    }

    // Clear all roadblock slots by setting the first byte (active flag) of each slot to 0
    for (uint8* slot = reinterpret_cast<uint8*>(ms_RoadBlockSlots);
         slot < reinterpret_cast<uint8*>(ms_RoadBlockSlotsEnd);
         slot += 0x1C) {
        *slot = 0;
    }
}

// 0x460EC0
void CRoadBlocks::ClearScriptRoadBlocks() {
    for (uint8* slot = reinterpret_cast<uint8*>(ms_RoadBlockSlots);
         slot < reinterpret_cast<uint8*>(ms_RoadBlockSlotsEnd);
         slot += 0x1C)
    {
        *slot = 0; // clear "active" flag
    }
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
