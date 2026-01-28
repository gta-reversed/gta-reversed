#pragma once

#include "Base.h"

#include "rwplcore.h" // RwMatrix

#include "Vector.h"
#include "NodeAddress.h"
#include "InteriorInfo_t.h"
#include "List_c.h"
#include "ListItem_c.h"
#include "FurnitureEntity_c.h"

class CEntity;
class CObject;
class Furniture_c;
class InteriorGroup_c;

typedef struct GotoPt_t GotoPt_t;
struct GotoPt_t {
    int8    tileX;
    int8    tileY;
    int8    link1;
    int8    link2;
    CVector pos;
};
VALIDATE_SIZE(GotoPt_t, 0x10);

class Interior_c : public ListItem_c<Interior_c> {
public:
    int32             m_interiorId;
    InteriorGroup_c*  m_pGroup;
    int32             m_areaCode;
    tEffectInterior*  m_box;
    RwMatrix          m_matrix;
    float             m_distSq;
    TList_c<void>     m_list;               // 0x5C - TODO: Figure out type
    uint8             m_tiles[30][30];
    CNodeAddress      m_exitAddr;
    CNodeAddress      m_doorAddr;
    CVector           m_exitPos;
    CVector           m_doorPos;
    int8              m_numGotoPts;
    int8              m_numInteriorInfos;
    GotoPt_t          m_gotoPts[16];
    GotoPt_t          m_exitPts[8];
    InteriorInfo_t    m_interiorInfos[16];
    int8              m_shopSubType;
    int8              m_style;
    int8              m_style2;

public:
    static void InjectHooks();

    Interior_c() = default;
    ~Interior_c() = default; // 0x591360

    int32 Init(const CVector& pos);
    void Exit();

    void Bedroom_AddTableItem(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6, int32 a7);
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

    auto GetNodeExitAddress() const { return m_exitAddr; }
    auto GetNodeDoorAddress() const { return m_doorAddr; }
    auto GetNodeAddress() const { return m_doorAddr; } // OLD
};
VALIDATE_SIZE(Interior_c, 0x794);
