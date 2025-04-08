#pragma once

#include "Vector.h"
#include "NodeAddress.h"

class CVehicle;
class CPed;

struct CRoadBlock {
public:
    CVector CornerA;
    CVector CornerB;
    bool    IsActive;
    bool    IsCreated;
    uint8   Type; // TODO
private:
    uint8   __field1B;
};
VALIDATE_SIZE(CRoadBlock, 0x1C);

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
    static void RegisterScriptRoadBlock(CVector a1, CVector a2, bool a3);

public:
    static inline std::array<bool, 325>&         InOrOut                   = StaticRef<std::array<bool, 325>>(0xA43438);
    static inline std::array<CNodeAddress, 325>& RoadBlockNodes            = StaticRef<std::array<CNodeAddress, 325>>(0xA435A0);
    static inline std::array<CRoadBlock, 16>&    aScriptRoadBlocks         = StaticRef<std::array<CRoadBlock, 16>>(0xA43AB8);
    static inline bool&                          GenerateDynamicRoadBlocks = StaticRef<bool>(0xA43584);
    static inline int32&                         NumRoadBlocks             = StaticRef<int32>(0xA43580);
};
