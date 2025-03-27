#pragma once

#include "Base.h"

#include "rwplcore.h" // RwMatrix

#include "Vector.h"
#include "NodeAddress.h"
#include "InteriorInfo_t.h"
#include "List_c.h"
#include "ListItem_c.h"
#include "./FurnitureEntity_c.h"

class CEntity;
class CObject;
class Furniture_c;
class InteriorGroup_c;

class Interior_c : public ListItem_c<Interior_c> {
public:
    int32             m_interiorId;         // 0x8
    InteriorGroup_c*  m_pGroup;             // 0xC
    int32             m_areaCode;           // 0x10
    tEffectInterior*  m_box;                // 0x14
    RwMatrix          m_matrix;             // 0x18
    int32             field_58;             // 0x58
    TList_c<void>     m_list;               // 0x5C - TODO: Figure out type
    char              field_68[900];        // 0x68
    int16             field_3EC;            // 0x3EC
    int16             field_3EE;            // 0x3EE
    CNodeAddress      m_nodeAddress;        // 0x3F0
    int16             field_3F4;            // 0x3F4
    int16             field_3F6;            // 0x3F6
    int32             field_3F8;            // 0x3F8
    int32             field_3FC;            // 0x3FC
    CVector           m_position;           // 0x400
    int8              field_40C;            // 0x40C
    int8              m_interiorInfosCount; // 0x40D
    int32             field_412;            // 0x412
    int32             field_416;            // 0x416
    int32             field_41A;            // 0x41A
    int32             field_41E;            // 0x41E
    int32             field_422;            // 0x422
    int32             field_426;            // 0x426
    int32             field_42A;            // 0x42A
    int32             field_42E;            // 0x42E
    int32             field_432;            // 0x432
    char              gap436[346];          // 0x436
    InteriorInfo_t    m_interiorInfos[16];  // 0x590
    int8              m_furnitureGroupId;   // 0x790
    int8              m_furnitureId;        // 0x791
    int8              field_792;            // 0x792
    int8              field_793;            // 0x793

public:
    static void InjectHooks();

    Interior_c() = default;
    ~Interior_c() = default; // 0x591360

    int32 Init(const CVector& pos);
    void Exit();

    CObject* Bedroom_AddTableItem(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6, int32 a7);
    void FurnishBedroom();
    CObject* Kitchen_FurnishEdges();
    void FurnishKitchen();
    CObject* Lounge_AddTV(int32 a2, int32 a3, int32 a4, int32 a5);
    CObject* Lounge_AddHifi(int32 a2, int32 a3, int32 a4, int32 a5);
    void Lounge_AddChairInfo(int32 a2, int32 a3, CEntity* entityIgnoredCollision);
    void Lounge_AddSofaInfo(int32 sitType, int32 offsetX, CEntity* entityIgnoredCollision);
    void FurnishLounge();
    bool Office_PlaceEdgeFillers(int32 arg0, int32 a2, int32 a3, int32 a6, int32);
    int32 Office_PlaceDesk(int32 a3, int32 arg4, int32 offsetY, int32 a5, uint8 a6, int32 b);
    int32 Office_PlaceEdgeDesks(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6);
    void Office_FurnishEdges();
    int32 Office_PlaceDeskQuad(int32 a2, int32 a3, int32 a4, int32 a5);
    int32 Office_FurnishCenter();
    void FurnishOffice();
    int8 Shop_Place3PieceUnit(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6);
    int32 Shop_PlaceEdgeUnits(int32 a2, int32 a3, int32 a4, int32 a5);
    int32 Shop_PlaceCounter(uint8 a2);
    void Shop_PlaceFixedUnits();
    void Shop_FurnishCeiling();
    void Shop_AddShelfInfo(int32 a2, int32 a3, int32 a5);
    void Shop_FurnishEdges();
    bool GetBoundingBox(FurnitureEntity_c* entity, CVector* a3);
    void ResetTiles();
    CObject* PlaceObject(uint8 isStealable, Furniture_c* furniture, float offsetX, float offsetY, float offsetZ, float rotationZ);
    FurnitureEntity_c* GetFurnitureEntity(CEntity*);
    bool IsPtInside(const CVector& pt, CVector bias = {});
    void CalcMatrix(CVector* translation);
    void Furnish();
    void Unfurnish();
    int8 CheckTilesEmpty(int32 a1, int32 a2, int32 a3, int32 a4, uint8 a5);
    void SetTilesStatus(int32 a, int32 b, int32 a3, int32 a4, int32 a5, int8 a6);
    void SetCornerTiles(int32 a4, int32 a3, int32 a5, uint8 a6);
    int32 GetTileStatus(int32 x, int32 y);
    int32 GetNumEmptyTiles(int32 a2, int32 a3, int32 a4, int32 a5);
    int32 GetRandomTile(int32 a2, int32* a3, int32* a4);
    void Shop_FurnishAisles();
    CVector* GetTileCentre(float offsetX, float offsetY, CVector* pointsIn);
    void AddGotoPt(int32 a, int32 b, float a3, float a4);
    bool AddInteriorInfo(int32 actionType, float offsetX, float offsetY, int32 direction, CEntity* entityIgnoredCollision);
    void AddPickups();
    void FindBoundingBox(int32, int32, int32*, int32*, int32*, int32*, int32*);
    void CalcExitPts();
    bool IsVisible();
    void PlaceFurniture(Furniture_c* a1, int32 a2, int32 a3, float a4, int32 a5, int32 a6, int32* a7, int32* a8, uint8 a9);
    void PlaceFurnitureOnWall(int32 furnitureGroupId, int32 furnitureSubgroupId, int32 furnitureId, float a5, int32 a6, int32 a7, int32 a8, int32 a9, int32* a10, int32* a11,
                              int32* a12, int32* a13, int32* a14, int32* a15);
    void PlaceFurnitureInCorner(int32 furnitureGroupId, int32 furnitureSubgroupId, int32 id, float a4, int32 a5, int32 a6, int32 a2, int32* a9, int32* a10, int32* a11, int32* a12,
                                int32* a13);
    bool FindEmptyTiles(int32 a3, int32 a4, int32* arg8, int32* a5);
    void FurnishShop(int32 a2);

    auto GetNodeAddress() const { return m_nodeAddress; }
};
VALIDATE_SIZE(Interior_c, 0x794);
