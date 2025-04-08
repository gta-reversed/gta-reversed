#pragma once

#include "Vector.h"
#include "NodeAddress.h"

class CVehicle;
class CPed;

struct CRoadBlock {
    CVector CornerA;
    CVector CornerB;
    bool    IsActive;
    bool    IsCreated;
    bool    IsGangRoute;
};
VALIDATE_SIZE(CRoadBlock, 0x1C);

static constexpr size_t MAX_ROADBLOCKS{ 325 };
static constexpr size_t MAX_SCRIPT_ROADBLOCKS{ 16 };

class CRoadBlocks {
public:
    static void InjectHooks();

    static void Init();
    static void ClearScriptRoadBlocks();
    static void ClearSpaceForRoadBlockObject(CVector a1, CVector a2);
    static void CreateRoadBlockBetween2Points(CVector a1, CVector a2, uint32 a3);
    static void GenerateRoadBlockCopsForCar(CVehicle* vehicle, int32 a2, ePedType pedType);
    static void GenerateRoadBlocks();
    static bool GetRoadBlockNodeInfo(CNodeAddress a1, float& a2, CVector& a3);
    static void RegisterScriptRoadBlock(CVector cornerA, CVector cornerB, bool isGangRoute);

public:
    static inline std::array<bool, MAX_ROADBLOCKS>&              InOrOut                   = StaticRef<std::array<bool, MAX_ROADBLOCKS>>(0xA43438);
    static inline std::array<CNodeAddress, MAX_ROADBLOCKS>&      RoadBlockNodes            = StaticRef<std::array<CNodeAddress, MAX_ROADBLOCKS>>(0xA435A0);
    static inline std::array<CRoadBlock, MAX_SCRIPT_ROADBLOCKS>& aScriptRoadBlocks         = StaticRef<std::array<CRoadBlock, MAX_SCRIPT_ROADBLOCKS>>(0xA43AB8);
    static inline bool&                                          GenerateDynamicRoadBlocks = StaticRef<bool>(0xA43584);
    static inline int32&                                         NumRoadBlocks             = StaticRef<int32>(0xA43580);
};
