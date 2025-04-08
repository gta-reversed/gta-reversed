#pragma once

#include "Base.h"

#include "rwplcore.h" // RwMatrix

#include "Vector.h"
#include "NodeAddress.h"
#include "InteriorInfo_t.h"
#include "List_c.h"
#include "ListItem_c.h"
#include "FurnitureEntity_c.h"
#include "InteriorGroup_c.h"

class CEntity;
class CObject;
class Furniture_c;
class InteriorGroup_c;
struct tEffectInterior;

struct GoToPt_t {
    int8    TileX, TileY;
    int8    Prev, Next;
    CVector Pos;
};
VALIDATE_SIZE(GoToPt_t, 0x10);

enum class eInteriorType {
    SHOP       = 0,
    OFFICE     = 1,
    LOUNGE     = 2,
    BEDROOM    = 3,
    KITCHEN    = 4,
    BATHROOM   = 5,
    OFFLICENSE = 6,
    HOTELROOM  = 7,
    MISC       = 8,
    TESTROOM   = 99, // NOTE: This isn't included in `NUM`

    //
    // Add new above this line
    //
    NUM = 9,
};
using eInteriorTypeS32 = notsa::WEnumS32<eInteriorType>;
using eInteriorTypeS8 = notsa::WEnumS8<eInteriorType>;

class Interior_c : public ListItem_c<Interior_c> {
    enum class eTileStatus : uint8 {
        STATE_0 = 0,
        STATE_1 = 1,
        STATE_2 = 2,
        STATE_3 = 3,
        STATE_4 = 4,
        STATE_5 = 5,
        STATE_6 = 6,
        STATE_7 = 7,
        STATE_8 = 8,
        STATE_9 = 9,
    };
    NOTSA_WENUM_DEFS_FOR(eTileStatus);

    static constexpr size_t NUM_TILES_PER_AXIS = 30;
    static constexpr size_t NUM_TILES = sq(NUM_TILES_PER_AXIS);

    template<typename T>
    using Tiles = T[NUM_TILES_PER_AXIS][NUM_TILES_PER_AXIS]; //notsa::mdarray<uint8, NUM_TILES_PER_AXIS, NUM_TILES_PER_AXIS>;

    enum class eWall {
        NA  = -1,
        X_A = 0,
        Y_A = 1,
        X_B = 2,
        Y_B = 3,
    };
    NOTSA_WENUM_DEFS_FOR(eWall);

public:
    static void InjectHooks();

    Interior_c() = default;
    ~Interior_c() = default; // 0x591360

    bool Init(const CVector& pos); // always true
    void Exit();
    void Furnish();
    void UnFurnish();

    /*!
     * @brief Get corners of a furniture's bounding box
     * @param fe The furniture to get the bb of
     * @param corners The 4 corners of the the bounding box (counter-clockwise, from top left): top left, bottom left, bottom right, top right
     * @return 
    */
    bool GetBoundingBox(FurnitureEntity_c* fe, CVector(&corners)[4]);
    void FindBoundingBox(
        int32 x,     int32 y,
        int32& minX, int32& maxX,
        int32& minY, int32& maxY,
        Tiles<int32>& tileInfo
    );
    bool IsPtInside(const CVector& pt, CVector bias = {});
    void CalcMatrix(const CVector& pos);
    void AddGotoPt(int32 tileX, int32 tileY, float offsetX, float offsetY);
    void CalcExitPts();
    bool AddInteriorInfo(eInteriorInfoTypeS32 actionType, float offsetX, float offsetY, eWall wall, CEntity* entityIgnoredCollision);
    void AddPickups();
    bool IsVisible();
    auto GetDoorNodeAddress() const { return m_DoorAddr; }

    //
    // Bedroom
    //
    void FurnishBedroom();
    void Bedroom_AddTableItem(eInteriorGroupIdS32 groupId, eInteriorSubGroupIdS32 subGroupId, eWall wall, int32 x, int32 y, int32 d);

    //
    // Kitchen
    //
    void FurnishKitchen();
    CObject* Kitchen_FurnishEdges();

    //
    // Lounge
    //
    void FurnishLounge();
    void Lounge_AddTV(eWall wallId, int32 x, int32 y, int32 depth);
    void Lounge_AddHifi(eWall wallId, int32 x, int32 y, int32 depth);
    void Lounge_AddChairInfo(eWall wallId, int32 pos, CEntity* entityIgnoredCollision);
    void Lounge_AddSofaInfo(int32 sitType, int32 offsetX, CEntity* entityIgnoredCollision);

    //
    // Office
    //
    void FurnishOffice();
    bool Office_PlaceEdgeFillers(int32 type, int32 x, int32 y, int32 dir, int32 wall);
    int32 Office_PlaceDesk(int32 tileX, int32 tileY, int32 dir, int32 pedChance, bool lShaped, int32 type);
    int32 Office_PlaceEdgeDesks(int32 type, int32 x, int32 y, int32 dir, int32 wall);
    void Office_FurnishEdges();
    int32 Office_PlaceDeskQuad(int32 formation, int32 tileX, int32 tileY, int32 type);
    int32 Office_FurnishCenter();

    //
    // Shop
    //
    void FurnishShop(eInteriorTypeS32 intType);
    void Shop_FurnishEdges();
    void Shop_FurnishAisles();
    int8  Shop_Place3PieceUnit(int32 type, int32 x, int32 y, int32 dir, int32 len);
    int32 Shop_PlaceEdgeUnits(int32 type, int32 x, int32 y, int32 dir);
    int32 Shop_PlaceCounter(bool left);
    void Shop_PlaceFixedUnits();
    void Shop_FurnishCeiling();
    void  Shop_AddShelfInfo(int32 tileX, int32 tileY, int32 direction);

    //
    // Furnitures
    //
    void PlaceFurniture(Furniture_c* furniture, int32 x, int32 y, float z, int32 height, int32 dir, int32* w, int32* d, bool force);
    CObject* PlaceFurnitureOnWall(
        eInteriorGroupIdS32 interior,
        eInteriorSubGroupId subGroup,
        int32               type = -1,
        float               z = 0.f,
        int32               heightInfo = 0,
        eWallS32            wallId = eWall::NA,
        int32               pos = -1,
        int32               offset = 0,
        eWallS32*           outWall = nullptr,
        int32*              outPos = nullptr,
        int32*              x = nullptr,
        int32*              y = nullptr,
        int32*              w = nullptr,
        int32*              d = nullptr
    );
    void PlaceFurnitureInCorner(int32 furnitureGroupId, int32 furnitureSubgroupId, int32 type, float z, int32 heightInfo, int32 wallId, uint32 offset, int32 *retWallId, int32 *x, int32 *y, int32 *w, int32 *d);
    CObject* PlaceObject(uint8 isStealable, Furniture_c* furniture, float offsetX, float offsetY, float offsetZ, float rotationZ);
    FurnitureEntity_c* GetFurnitureEntity(CEntity*);

    //
    // Tiles
    //
    void ResetTiles();
    int8 CheckTilesEmpty(int32 x, int32 y, int32 w, int32 d, bool canPlaceOnWindow);
    void SetTilesStatus(int32 x, int32 y, int32 w, int32 d, eTileStatusS32 status, bool force = false);
    void SetTileStatus(int32 x, int32 y, eTileStatus status); // notsa
    void SetCornerTiles(int32 wallId, int32 size, int32 status, bool force);
    eTileStatusS32 GetTileStatus(int32 x, int32 y) const;
    int32 GetNumEmptyTiles(int32 x, int32 y, int32 dir, int32 size);
    int32 GetRandomTile(int32 status, int32* x, int32* y);
    void GetTileCentre_OG(float offsetX, float offsetY, CVector* outPos) const;
    CVector GetTileCentre(float offsetX, float offsetY) const;
    bool FindEmptyTiles(int32 w, int32 h, int32* x, int32* y);

    inline float GetRotationOfWall(eWall wallId) {
        const auto wall = std::to_underlying(wallId);

        // Euclidean modulus 4 with `x & 3`
        return static_cast<float>(wall & 3) * 90.0f;
    }

    // Inlined in code `(wallId - 2) & 3`
    inline eWall GetOpposingWall(eWall wallId) {
        switch (wallId) {
        case eWall::X_A: return eWall::X_B;
        case eWall::Y_A: return eWall::Y_B;
        case eWall::X_B: return eWall::X_A;
        case eWall::Y_B: return eWall::Y_A;
        default: NOTSA_UNREACHABLE();
        }
    }

public:
    int32                      m_ID{};                  // 0x8
    InteriorGroup_c*           m_Group{};               // 0xC
    int32                      m_AreaCode{};            // 0x10
    tEffectInterior*           m_Props{};               // 0x14
    RwMatrix                   m_Mat{};                 // 0x18
    float                      m_DistSq{};              // 0x58
    TList_c<FurnitureEntity_c> m_FurnitureEntityList{}; // 0x5C
    Tiles<eTileStatusS8>       m_Tiles{};               // 0x68
    CNodeAddress               m_ExitAddr{};            // 0x3EC
    CNodeAddress               m_DoorAddr{};            // 0x3F0
    CVector                    m_ExitPos{};             // 0x3F4
    CVector                    m_DoorPos{};             // 0x400
    uint8                      m_NumGoToPts{};          // 0x40C
    uint8                      m_NumIntInfo{};          // 0x40D
    GoToPt_t                   m_GoToPts[16]{};         // 0x410
    GoToPt_t                   m_ExitPts[4][2]{};       // 0x510 - 1 for each wall, entry exit + door positions
    InteriorInfo_t             m_IntInfos[16]{};        // 0x590
    int8                       m_ShopSubType{};         // 0x790
    int8                       m_StyleA{};              // 0x791
    int8                       m_StyleB{};              // 0x792
};
VALIDATE_SIZE(Interior_c, 0x794);
