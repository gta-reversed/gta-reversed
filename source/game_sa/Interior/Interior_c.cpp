#include "StdInc.h"
#include "Interior_c.h"

#include "Furniture_c.h"
#include "FurnitureManager_c.h"
#include "InteriorGroup_c.h"

//! @notsa Interior tiles are stored as `field_68[x * 30 + y]`
static constexpr int32 TILE_ROW_STRIDE = 30;

/*!
 * @notsa
 * @brief The original code's way of generating a random number in range `[0, max)`:
 *        `(int)((float)(rand() & 0xFFFF) * (1.0f / 32768.0f) * max)`
 */
static int32 RandomNumberInRange(float max) {
    return (int32)((float)(rand() & 0xFFFF) * (1.0f / 32768.0f) * max);
}

void Interior_c::InjectHooks() {
    RH_ScopedClass(Interior_c);
    RH_ScopedCategory("Interior");

    //RH_ScopedInstall(Constructor, 0x5921D0, { .reversed = false });
    //RH_ScopedInstall(Destructor, 0x591360, { .reversed = false });

    RH_ScopedInstall(Bedroom_AddTableItem, 0x593F10);
    RH_ScopedInstall(FurnishBedroom, 0x593FC0, { .reversed = false });
    RH_ScopedInstall(Kitchen_FurnishEdges, 0x596930, { .reversed = false });
    RH_ScopedInstall(FurnishKitchen, 0x5970B0);
    RH_ScopedInstall(Lounge_AddTV, 0x597240, { .reversed = false });
    RH_ScopedInstall(Lounge_AddHifi, 0x597430);
    RH_ScopedInstall(Lounge_AddChairInfo, 0x5974E0);
    RH_ScopedInstall(Lounge_AddSofaInfo, 0x5975C0);
    RH_ScopedInstall(FurnishLounge, 0x597740, { .reversed = false });
    RH_ScopedInstall(Office_PlaceEdgeFillers, 0x599210, { .reversed = false });
    RH_ScopedInstall(Office_PlaceDesk, 0x5993E0, { .reversed = false });
    RH_ScopedInstall(Office_PlaceEdgeDesks, 0x5995B0, { .reversed = false });
    RH_ScopedInstall(Office_FurnishEdges, 0x599770);
    RH_ScopedInstall(Office_PlaceDeskQuad, 0x599960);
    RH_ScopedInstall(Office_FurnishCenter, 0x599A30);
    RH_ScopedInstall(FurnishOffice, 0x599AF0);
    RH_ScopedInstall(Shop_Place3PieceUnit, 0x599BB0);
    RH_ScopedInstall(Shop_PlaceEdgeUnits, 0x599DC0);
    RH_ScopedInstall(Shop_PlaceCounter, 0x599EF0);
    RH_ScopedInstall(Shop_PlaceFixedUnits, 0x59A030, { .reversed = false });
    RH_ScopedInstall(Shop_FurnishCeiling, 0x59A130);
    RH_ScopedInstall(Shop_AddShelfInfo, 0x59A140);
    RH_ScopedInstall(Shop_FurnishEdges, 0x59A1B0, { .reversed = false });
    RH_ScopedInstall(GetBoundingBox, 0x593DB0);
    RH_ScopedInstall(Init, 0x593BF0, { .reversed = false });
    RH_ScopedInstall(ResetTiles, 0x593910);
    RH_ScopedInstall(PlaceObject, 0x5934E0, { .reversed = false });
    RH_ScopedInstall(GetFurnitureEntity, 0x5913B0);
    RH_ScopedInstall(IsPtInside, 0x5913E0);
    RH_ScopedInstall(CalcMatrix, 0x5914D0);
    RH_ScopedInstall(Furnish, 0x591590);
    RH_ScopedInstall(Unfurnish, 0x5915D0, { .reversed = false });
    RH_ScopedInstall(CheckTilesEmpty, 0x591680);
    RH_ScopedInstall(SetTilesStatus, 0x591700);
    RH_ScopedInstall(SetCornerTiles, 0x5917C0);
    RH_ScopedInstall(GetTileStatus, 0x5918E0);
    RH_ScopedInstall(GetNumEmptyTiles, 0x591920);
    RH_ScopedInstall(GetRandomTile, 0x591B20);
    RH_ScopedInstall(Shop_FurnishAisles, 0x59A590);
    RH_ScopedInstall(GetTileCentre, 0x591BD0);
    RH_ScopedInstall(AddGotoPt, 0x591D20);
    RH_ScopedInstall(AddInteriorInfo, 0x591E40);
    RH_ScopedInstall(AddPickups, 0x591F90);
    RH_ScopedInstall(Exit, 0x592230);
    RH_ScopedInstall(FindBoundingBox, 0x5922C0);
    RH_ScopedInstall(CalcExitPts, 0x5924A0, { .reversed = false });
    RH_ScopedInstall(PlaceFurniture, 0x592AA0, { .reversed = false });
    RH_ScopedInstall(PlaceFurnitureOnWall, 0x593120, { .reversed = false });
    RH_ScopedInstall(PlaceFurnitureInCorner, 0x593340, { .reversed = false });
    RH_ScopedInstall(FindEmptyTiles, 0x591C50);
    RH_ScopedInstall(FurnishShop, 0x59A790);
}

// 0x593BF0
int32 Interior_c::Init(const CVector& pos) {
    return plugin::CallMethodAndReturn<int32, 0x593BF0>(this, &pos);
}

// 0x592230
void Interior_c::Exit() {
    CPickups::RemovePickUpsInArea(
        m_position.x - 50.0f, m_position.x + 50.0f, m_position.y - 50.0f, m_position.y + 50.0f, m_position.z - 50.0f, m_position.z + 50.0f
    );
    Unfurnish();
}

// 0x593F10
CObject* Interior_c::Bedroom_AddTableItem(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6, int32 a7) {
    float offsetX = (float)a5;
    float offsetY = (float)a6;
    if (a4 == 0 || a4 == 2) {
        offsetX += 0.5f;
    } else if (a4 == 1 || a4 == 3) {
        offsetY += 0.5f;
    }

    return PlaceObject(
        true,
        g_furnitureMan.GetFurniture(a2, a3, -1, m_box->m_status),
        offsetX + 0.5f,
        offsetY + 0.5f,
        0.5f,
        (float)a7 * 90.0f
    );
}

// 0x593FC0
void Interior_c::FurnishBedroom() {
    plugin::CallMethod<0x593FC0, Interior_c*>(this);
}

// 0x596930
CObject* Interior_c::Kitchen_FurnishEdges() {
    return plugin::CallMethodAndReturn<CObject*, 0x596930, Interior_c*>(this);
}

// 0x5970B0
void Interior_c::FurnishKitchen() {
    SetTilesStatus((int32)m_box->m_door - 1, 0, 2, 1, 7, 0);
    const auto maxX = (int32)m_box->m_width - 2;
    const auto maxY = (int32)m_box->m_depth - 2;
    for (auto x = 1; x <= maxX; x++) {
        SetTilesStatus(x, maxY, 1, 1, 3, 0);
        SetTilesStatus(x, 0, 1, 1, 3, 0);
    }
    for (auto y = 0; y <= maxY; y++) {
        SetTilesStatus(1, y, 1, 1, 3, 0);
        SetTilesStatus(maxX, y, 1, 1, 3, 0);
    }
    AddGotoPt(1, 1, 0.0f, 0.0f);
    AddGotoPt(1, maxY, 0.0f, 0.0f);
    AddGotoPt(maxX, 1, 0.0f, 0.0f);
    AddGotoPt(maxX, maxY, 0.0f, 0.0f);
    m_furnitureId = static_cast<int8>(g_furnitureMan.GetRandomId(4, 0, m_box->m_status));
    Kitchen_FurnishEdges();
    auto* unit = g_furnitureMan.GetFurniture(8, 1, -1, m_box->m_status);
    // NB: original rolls the depth offset first, then the width offset (C++ arg order is unspecified, so sequence explicitly)
    const auto offsetY = RandomNumberInRange((float)m_box->m_depth * 0.5f - (float)unit->m_nWidthY * 0.5f);
    const auto offsetX = RandomNumberInRange((float)m_box->m_width * 0.5f - (float)unit->m_nWidthX * 0.5f);
    int32 sizeX{}, sizeY{};
    PlaceFurniture(unit, offsetX, offsetY, 0.0f, 0, 0, &sizeX, &sizeY, false);
}

// 0x597240
CObject* Interior_c::Lounge_AddTV(int32 a2, int32 a3, int32 a4, int32 a5) {
    return plugin::CallMethodAndReturn<CObject*, 0x597240, Interior_c*, int32, int32, int32, int32>(this, a2, a3, a4, a5);
}

// 0x597430
CObject* Interior_c::Lounge_AddHifi(int32 a2, int32 a3, int32 a4, int32 a5) {
    float offsetX = (float)a3;
    float offsetY = (float)a4;

    if (a2 == 0 || a2 == 2) {
        offsetX += 0.5f;
    } else if (a2 == 1 || a2 == 3) {
        offsetY += 0.5f;
    }

    return PlaceObject(
        true,
        g_furnitureMan.GetFurniture(2, 8, -1, m_box->m_status),
        offsetX + 0.5f,
        offsetY + 0.5f,
        0.5f,
        (float)(a2 & 3) * 90.0f
    );
}

// 0x5974E0
void Interior_c::Lounge_AddChairInfo(int32 a2, int32 a3, CEntity* entityIgnoredCollision) {
    switch (a2) {
    case 0:
        AddInteriorInfo(1, (float)a3 + 0.5f, (float)(m_box->m_depth - 1) - 1.0f, 2, entityIgnoredCollision);
        break;
    case 1:
        AddInteriorInfo(1, 1.0f, (float)a3 + 0.5f, 3, entityIgnoredCollision);
        break;
    case 2:
        AddInteriorInfo(1, (float)a3 + 0.5f, 1.0f, 0, entityIgnoredCollision);
        break;
    case 3:
        AddInteriorInfo(1, (float)(m_box->m_width - 1) - 1.0f, (float)a3 + 0.5f, 1, entityIgnoredCollision);
        break;
    }
}

// 0x5975C0
void Interior_c::Lounge_AddSofaInfo(int32 sitType, int32 offsetX, CEntity* entityIgnoredCollision) {
    switch (sitType) {
    case 0: {
        const auto x = (float)offsetX + 0.5f;
        const auto y = (float)(m_box->m_depth - 1) - 1.0f;
        AddInteriorInfo(1, x, y, 2, entityIgnoredCollision);
        AddInteriorInfo(1, x + 1.0f, y, 2, entityIgnoredCollision);
        break;
    }
    case 1: {
        const auto y = (float)offsetX + 0.5f;
        AddInteriorInfo(1, 1.0f, y, 3, entityIgnoredCollision);
        AddInteriorInfo(1, 1.0f, y + 1.0f, 3, entityIgnoredCollision);
        break;
    }
    case 2: {
        const auto x = (float)offsetX + 0.5f;
        AddInteriorInfo(1, x, 1.0f, 0, entityIgnoredCollision);
        AddInteriorInfo(1, x + 1.0f, 1.0f, 0, entityIgnoredCollision);
        break;
    }
    case 3: {
        const auto x = (float)(m_box->m_width - 1) - 1.0f;
        const auto y = (float)offsetX + 0.5f;
        AddInteriorInfo(1, x, y, 1, entityIgnoredCollision);
        AddInteriorInfo(1, x, y + 1.0f, 1, entityIgnoredCollision);
        break;
    }
    }
}

// 0x597740
void Interior_c::FurnishLounge() {
    plugin::CallMethod<0x597740, Interior_c*>(this);
}

// 0x599210
bool Interior_c::Office_PlaceEdgeFillers(int32 arg0, int32 a2, int32 a3, int32 a6, int32 a7) {
    return plugin::CallMethodAndReturn<bool, 0x599210, Interior_c*, int32, int32, int32, int32, int32>(this, arg0, a2, a3, a6, a7);
}

// 0x5993E0
int32 Interior_c::Office_PlaceDesk(int32 a3, int32 arg4, int32 offsetY, int32 a5, uint8 a6, int32 b) {
    return plugin::CallMethodAndReturn<int32, 0x5993E0, Interior_c*, int32, int32, int32, int32, uint8, int32>(this, a3, arg4, offsetY, a5, a6, b);
}
// 0x5995B0
int32 Interior_c::Office_PlaceEdgeDesks(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6) {
    return plugin::CallMethodAndReturn<int32, 0x5995B0, Interior_c*, int32, int32, int32, int32, int32>(this, a2, a3, a4, a5, a6);
}

// 0x599770
void Interior_c::Office_FurnishEdges() {
    const auto maxX = (int32)m_box->m_width - 3;
    const auto maxY = (int32)m_box->m_depth - 3;
    for (auto x = 2; x <= maxX; x++) {
        SetTilesStatus(x, maxY, 1, 1, 3, 0);
        SetTilesStatus(x, 2, 1, 1, 3, 0);
    }
    for (auto y = 2; y <= maxY; y++) {
        SetTilesStatus(2, y, 1, 1, 3, 0);
        SetTilesStatus(maxX, y, 1, 1, 3, 0);
    }
    AddGotoPt(2, 2, 0.5f, 0.5f);
    AddGotoPt(2, maxY, 0.5f, -0.5f);
    AddGotoPt(maxX, 2, -0.5f, 0.5f);
    AddGotoPt(maxX, maxY, -0.5f, -0.5f);
    const auto width = (int32)m_box->m_width - 1;
    const auto depth = (int32)m_box->m_depth;
    SetTilesStatus((int32)m_box->m_door - 2, 0, 4, 2, 7, 0);
    for (auto x = 1; x < width;) {
        x += Office_PlaceEdgeDesks(-1, x, 0, 2, 2);
    }
    for (auto x = 1; x < width;) {
        x += Office_PlaceEdgeDesks(-1, x, depth - 1, 0, 0);
    }
    for (auto y = 1; y <= depth - 2;) {
        y += Office_PlaceEdgeDesks(-1, 0, y, 1, 1);
    }
    for (auto y = 1; y <= depth - 2;) {
        y += Office_PlaceEdgeDesks(-1, width, y, 3, 3);
    }
    for (auto x = 1; x < width;) {
        x += Office_PlaceEdgeFillers(-1, x, 0, 2, 2);
    }
    for (auto x = 1; x < width;) {
        x += Office_PlaceEdgeFillers(-1, x, depth - 1, 0, 0);
    }
    for (auto y = 1; y <= depth - 2;) {
        y += Office_PlaceEdgeFillers(-1, 0, y, 1, 1);
    }
    for (auto y = 1; y <= depth - 2;) {
        y += Office_PlaceEdgeFillers(-1, width, y, 3, 3);
    }
}
// 0x599960
int32 Interior_c::Office_PlaceDeskQuad(int32 a2, int32 a3, int32 a4, int32 a5) {
    const auto y = a3 - 2;
    Office_PlaceDesk(a2, y, 2, 0x46, 0, a5);
    Office_PlaceDesk(a2, a3, 0, 0x46, 0, a5);
    Office_PlaceDesk(a2 - 2, a3, 0, 0x46, 0, a5);
    Office_PlaceDesk(a2 - 2, y, 2, 0x46, 0, a5);

    SetTilesStatus(a2 - 3, a3 - 3, 6, 1, 3, 0);
    SetTilesStatus(a2 - 3, a3 + 2, 6, 1, 3, 0);
    SetTilesStatus(a2 - 3, y, 1, 4, 3, 0);
    SetTilesStatus(a2 + 2, y, 1, 4, 3, 0);
    return 6;
}

// 0x599A30
int32 Interior_c::Office_FurnishCenter() {
    const auto w         = (int32)m_box->m_width - 6;
    auto       numDesksX = w / 6;
    const auto d         = (int32)m_box->m_depth - 6;
    const auto numDesksY = d / 6;
    auto       x         = (w % 6) / 2;
    const auto y         = (d % 6) / 2;

    if (w > 0 && d > 0 && numDesksX > 0) {
        for (; numDesksX != 0; --numDesksX) {
            x += 6;
            for (auto i = numDesksY, deskY = y; i != 0; --i, deskY += 6) {
                Office_PlaceDeskQuad(-1, x, deskY + 6, (int32)m_furnitureId);
            }
        }
    }
    return y;
}

// 0x599AF0
void Interior_c::FurnishOffice() {
    SetTilesStatus(0, 0, 2, 2, 2, 0);
    SetTilesStatus(0, m_box->m_depth - 2, 2, 2, 2, 0);
    SetTilesStatus(m_box->m_width - 2, 0, 2, 2, 2, 0);
    SetTilesStatus(m_box->m_width - 2, m_box->m_depth - 2, 2, 2, 2, 0);

    m_furnitureId = static_cast<int8>(g_furnitureMan.GetRandomId(1, 0, m_box->m_status));
    field_792     = static_cast<int8>(g_furnitureMan.GetRandomId(1, 1, m_box->m_status));

    Office_FurnishEdges();
    Office_FurnishCenter();
    Shop_FurnishCeiling();
}

// 0x599BB0
int8 Interior_c::Shop_Place3PieceUnit(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6) {
    // NOTE: `a2` and `a5` double as the out params of `PlaceFurniture` (Just like in the original code)
    Furniture_c *firstUnit, *middleUnit;
    if (a5 != 2 && a5 != 3) {
        firstUnit  = g_furnitureMan.GetFurniture(m_furnitureGroupId, a2, -1, m_box->m_status);
        middleUnit = g_furnitureMan.GetFurniture(m_furnitureGroupId, a2 + 1, firstUnit->m_nId, m_box->m_status);
    } else {
        firstUnit  = g_furnitureMan.GetFurniture(m_furnitureGroupId, a2 + 1, -1, m_box->m_status);
        middleUnit = g_furnitureMan.GetFurniture(m_furnitureGroupId, a2, firstUnit->m_nId, m_box->m_status);
    }
    auto* lastUnit = g_furnitureMan.GetFurniture(m_furnitureGroupId, a2 + 2, firstUnit->m_nId, m_box->m_status);

    const auto dir = a5;
    if (dir != 2 && dir != 0) { // Along Y
        PlaceFurniture(firstUnit, a3, a4, 0.0f, 1, dir, &a5, &a2, false);
        auto pos = a4 + a2; // `a2` now holds the size of the placed unit
        for (auto num = a6 - 2; num > 0; num--) {
            PlaceFurniture(lastUnit, a3, pos, 0.0f, 1, dir, &a5, &a2, false);
            pos += a2;
        }
        PlaceFurniture(middleUnit, a3, pos, 0.0f, 1, dir, &a5, &a2, false);
    } else { // Along X
        PlaceFurniture(firstUnit, a3, a4, 0.0f, 1, dir, &a5, &a2, false);
        auto pos = a3 + a5; // `a5` now holds the size of the placed unit
        for (auto num = a6 - 2; num > 0; num--) {
            PlaceFurniture(lastUnit, pos, a4, 0.0f, 1, dir, &a5, &a2, false);
            pos += a5;
        }
        PlaceFurniture(middleUnit, pos, a4, 0.0f, 1, dir, &a5, &a2, false);
    }
    return 1;
}

// 0x599DC0
int32 Interior_c::Shop_PlaceEdgeUnits(int32 type, int32 x, int32 y, int32 dir) {
    const auto alongX = (dir == 2 || dir == 0) ? 1 : 2;
    const auto numEmpty = GetNumEmptyTiles(x, y, alongX, 1);
    if (numEmpty <= 1) {
        return 1;
    }
    auto size = 2 - RandomNumberInRange(-3.0f);
    if (numEmpty != 3) {
        if (const auto rest = numEmpty - size; rest >= 0 && rest == 1) {
            size--;
        } else if (rest < 0) {
            size = numEmpty;
        }
    } else {
        size = 3;
    }
    static auto& s_Wealth = StaticRef<int32>(0xBB3DE4);
    if (type != -1) {
        Shop_Place3PieceUnit(type, x, y, dir, size);
    } else if (s_Wealth > 50) {
        Shop_Place3PieceUnit(0, x, y, dir, size);
    } else if (s_Wealth > 25) {
        Shop_Place3PieceUnit(3, x, y, dir, size);
    } else if (s_Wealth > 10) {
        Shop_Place3PieceUnit(6, x, y, dir, size);
    } else {
        Shop_Place3PieceUnit(9, x, y, dir, size);
    }
    return size;
}

// 0x599EF0
int32 Interior_c::Shop_PlaceCounter(uint8 a2) {
    auto* bigUnit       = g_furnitureMan.GetFurniture(0, 0xC, -1, m_box->m_status);
    auto* smallUnit     = g_furnitureMan.GetFurniture(0, 0xD, -1, m_box->m_status);

    const auto rotation = RandomNumberInRange(90.0f);

    int32 sizeX{}, sizeY{};
    int32 x;
    int32 x2;
    if (a2 == 0) {
        x = (int32)m_box->m_door + 2;
        PlaceFurniture(bigUnit, x, 1, 0.0f, 1, 0, &sizeX, &sizeY, false);
        SetTilesStatus(x, 0, sizeX + 1, 1, 2, 0);
        x2 = (int32)m_box->m_door - 2;
    } else {
        const auto door = (int32)m_box->m_door;
        x               = door - 5;
        PlaceFurniture(bigUnit, x, 1, 0.0f, 1, 0, &sizeX, &sizeY, false);
        SetTilesStatus(door - 6, 0, sizeX + 1, 1, 2, 0);
        x2 = door + 1;
    }
    PlaceFurniture(smallUnit, x2, 0, 0.0f, 1, rotation, &sizeX, &sizeY, true);

    return x + 2;
}

// 0x59A030
void Interior_c::Shop_PlaceFixedUnits() {
    plugin::CallMethod<0x59A030, Interior_c*>(this);
}
// 0x593DB0
bool Interior_c::GetBoundingBox(FurnitureEntity_c* entity, CVector* corners) {
    const auto type = m_box->m_type;
    if (type != 0 && type != 1 && type != 6) {
        return false;
    }
    const auto tileX = entity->m_tileX;
    const auto tileY = entity->m_tileY;
    int32 visited[900]{};
    visited[tileX * TILE_ROW_STRIDE + tileY] = 1;
    auto minX = (int32)tileX, maxX = (int32)tileX;
    auto minY = (int32)tileY, maxY = (int32)tileY;
    FindBoundingBox(tileX, tileY, &minX, &maxX, &minY, &maxY, visited);
    constexpr auto kOutset = 0.35f;
    GetTileCentre((float)minX - 0.5f - kOutset, (float)maxY + kOutset + 0.5f, &corners[0]);
    GetTileCentre((float)minX - 0.5f - kOutset, (float)minY - 0.5f - kOutset, &corners[1]);
    GetTileCentre((float)maxX + kOutset + 0.5f, (float)minY - 0.5f - kOutset, &corners[2]);
    GetTileCentre((float)maxX + kOutset + 0.5f, (float)maxY + kOutset + 0.5f, &corners[3]);
    return true;
}

// 0x593910
void Interior_c::ResetTiles() {
    std::fill(std::begin(field_68), std::end(field_68), 0);

    // Left doors
    if (m_box->m_lDoorStart != -1) {
        const auto start = (int32)m_box->m_lDoorStart;
        const auto count = (int32)m_box->m_lDoorEnd - start;
        if (count > 0 && start >= 0 && m_box->m_width != 0 && start + count <= (int32)m_box->m_depth) {
            for (auto i = 0; i < count; i++) {
                auto& tile = field_68[start + i]; // Leftmost column (x = 0)
                if (tile != 3 && tile == 0) {
                    tile = 8;
                }
            }
        }
    }

    // Right doors
    if (m_box->m_rDoorStart != -1) {
        const auto start = (int32)m_box->m_rDoorStart;
        const auto count = (int32)m_box->m_rDoorEnd - start;
        const auto x     = (int32)m_box->m_width - 1;
        if (count > 0 && x >= 0 && start >= 0 && start + count <= (int32)m_box->m_depth) {
            for (auto i = 0; i < count; i++) {
                auto& tile = field_68[x * TILE_ROW_STRIDE + start + i];
                if (tile != 3 && tile == 0) {
                    tile = 8;
                }
            }
        }
    }

    // Top doors
    if (m_box->m_tDoorStart != -1) {
        const auto start = (int32)m_box->m_tDoorStart;
        const auto count = (int32)m_box->m_tDoorEnd - start;
        const auto y     = (int32)m_box->m_depth - 1;
        if (count > 0 && start >= 0 && y >= 0 && start + count <= (int32)m_box->m_width) {
            for (auto i = 0; i < count; i++) {
                auto& tile = field_68[(start + i) * TILE_ROW_STRIDE + y];
                if (tile != 3 && tile == 0) {
                    tile = 8;
                }
            }
        }
    }

    // Left windows
    if (m_box->m_lWindowStart != -1) {
        const auto start = (int32)m_box->m_lWindowStart;
        const auto count = (int32)m_box->m_lWindowEnd - start;
        if (count > 0 && start >= 0 && m_box->m_width != 0 && start + count <= (int32)m_box->m_depth) {
            for (auto i = 0; i < count; i++) {
                auto& tile = field_68[start + i]; // Leftmost column (x = 0)
                if (tile != 3 && tile == 0) {
                    tile = 9;
                }
            }
        }
    }

    // Right windows
    if (m_box->m_rWindowStart != -1) {
        const auto start = (int32)m_box->m_rWindowStart;
        const auto count = (int32)m_box->m_rWindowEnd - start;
        const auto x     = (int32)m_box->m_width - 1;
        if (count > 0 && x >= 0 && start >= 0 && start + count <= (int32)m_box->m_depth) {
            for (auto i = 0; i < count; i++) {
                auto& tile = field_68[x * TILE_ROW_STRIDE + start + i];
                if (tile != 3 && tile == 0) {
                    tile = 9;
                }
            }
        }
    }

    // Top windows
    if (m_box->m_tWindowStart != -1) {
        const auto start = (int32)m_box->m_tWindowStart;
        const auto count = (int32)m_box->m_tWindowEnd - start;
        const auto y     = (int32)m_box->m_depth - 1;
        if (count > 0 && start >= 0 && y >= 0 && start + count <= (int32)m_box->m_width) {
            for (auto i = 0; i < count; i++) {
                auto& tile = field_68[(start + i) * TILE_ROW_STRIDE + y];
                if (tile != 3 && tile == 0) {
                    tile = 9;
                }
            }
        }
    }

    // No-go zones
    for (auto i = 0; i < 3; i++) {
        const auto x = (int32)m_box->m_noGoLeft[i];
        if (x == -1) {
            continue;
        }
        const auto y = (int32)m_box->m_noGoBottom[i];
        if (y == -1) {
            continue;
        }
        const auto w = (int32)m_box->m_noGoWidth[i];
        const auto d = (int32)m_box->m_noGoDepth[i];
        if (x < 0 || y < 0 || x + w > (int32)m_box->m_width || y + d > (int32)m_box->m_depth || w <= 0) {
            continue;
        }
        for (auto ix = 0; ix < w; ix++) {
            for (auto iy = 0; iy < d; iy++) {
                auto& tile = field_68[(x + ix) * TILE_ROW_STRIDE + y + iy];
                if (tile != 3 && tile == 0) {
                    tile = 11;
                }
            }
        }
    }
}

// 0x5934E0
CObject* Interior_c::PlaceObject(uint8 isStealable, Furniture_c* furniture, float offsetX, float offsetY, float offsetZ, float rotationZ) {
    return plugin::CallMethodAndReturn<CObject*, 0x5934E0, Interior_c*, uint8, Furniture_c*, float, float, float, float>(this, isStealable, furniture, offsetX, offsetY, offsetZ, rotationZ);
}

// 0x5913B0
FurnitureEntity_c* Interior_c::GetFurnitureEntity(CEntity* entity) {
    for (auto* item = static_cast<FurnitureEntity_c*>(m_list.GetHead()); item; item = item->m_pNext) {
        if (item->m_entity == entity) {
            return item;
        }
    }
    return nullptr;
}

// 0x5913E0
bool Interior_c::IsPtInside(const CVector& pt, CVector bias) {
    const CVector offset = pt - m_matrix.pos;

    if (std::abs(m_matrix.right.x * offset.x + m_matrix.right.y * offset.y + m_matrix.right.z * offset.z) > bias.x + (float)m_box->m_width * 0.5f) {
        return false;
    }
    if (std::abs(m_matrix.up.x * offset.x + m_matrix.up.y * offset.y + m_matrix.up.z * offset.z) > bias.y + (float)m_box->m_depth * 0.5f) {
        return false;
    }
    return std::abs(m_matrix.at.x * offset.x + m_matrix.at.y * offset.y + m_matrix.at.z * offset.z) <= bias.z + (float)m_box->m_height * 0.5f;
}

// 0x5914D0
void Interior_c::CalcMatrix(CVector* translation) {
    m_matrix.right = { 1.0f, 0.0f, 0.0f };
    m_matrix.up    = { 0.0f, 1.0f, 0.0f };
    m_matrix.at    = { 0.0f, 0.0f, 1.0f };
    m_matrix.pos   = { 0.0f, 0.0f, 0.0f };
    m_matrix.flags |= 0x20003; // rwMATRIXINTERNALIDENTITY | 0x3

    const CVector axis = { 0.0f, 0.0f, 1.0f };
    RwMatrixRotate(&m_matrix, &axis, m_box->m_rot, rwCOMBINEREPLACE);
    RwMatrixTranslate(&m_matrix, translation, rwCOMBINEPOSTCONCAT);

    if (auto* entity = m_pGroup->GetEntity(); entity->GetRwObject()) {
        RwMatrixMultiply(&m_matrix, &m_matrix, entity->GetRwMatrix());
    } else {
        RwMatrixMultiply(&m_matrix, &m_matrix, nullptr);
    }
}

// 0x591590
void Interior_c::Furnish() {
    switch (m_box->m_type) {
    case 0:
        FurnishShop(0);
        break;
    case 1:
        FurnishOffice();
        break;
    case 2:
        FurnishLounge();
        break;
    case 3:
        FurnishBedroom();
        break;
    case 4:
        FurnishKitchen();
        break;
    }
}

// 0x5915D0
void Interior_c::Unfurnish() {
    plugin::CallMethod<0x5915D0, Interior_c*>(this);
}

// 0x591680
int8 Interior_c::CheckTilesEmpty(int32 a1, int32 a2, int32 a3, int32 a4, uint8 a5) {
    if (a1 < 0 || a2 < 0 || (int32)m_box->m_width < a1 + a3 || (int32)m_box->m_depth < a2 + a4) {
        return false;
    }

    for (auto x = 0; x < a3; x++) {
        for (auto y = 0; y < a4; y++) {
            if (const auto tile = field_68[(a1 + x) * TILE_ROW_STRIDE + a2 + y]) {
                if (a5 == 0) {
                    return false;
                }
                if (tile != 9) {
                    return false;
                }
            }
        }
    }
    return true;
}

// 0x591700
void Interior_c::SetTilesStatus(int32 a, int32 b, int32 a3, int32 a4, int32 a5, int8 a6) {
    if (a < 0 || b < 0 || a + a3 > (int32)m_box->m_width || b + a4 > (int32)m_box->m_depth || a3 <= 0) {
        return;
    }

    for (auto x = 0; x < a3; x++) {
        for (auto y = 0; y < a4; y++) {
            auto& tile = field_68[(a + x) * TILE_ROW_STRIDE + b + y];
            if (tile == 9 && a5 == 5) {
                tile = 10;
            } else if (a6 == 0) {
                if (tile == 3) {
                    if (a5 == 3) {
                        return;
                    }
                    if (a5 == 4) {
                        tile = 4;
                    }
                } else if (tile == 0) { // Otherwise some tile types are overwritten
                    tile = (char)a5;
                }
            } else if (tile != 5 && tile != 7 && tile != 8) {
                tile = (char)a5;
            }
        }
    }
}

// 0x5917C0
void Interior_c::SetCornerTiles(int32 a4, int32 a3, int32 a5, uint8 a6) {
    switch (a4) {
    case 0:
        SetTilesStatus(0, m_box->m_depth - 1, a3, 1, a5, a6);
        SetTilesStatus(0, m_box->m_depth - a3, 1, a3, a5, a6);
        break;
    case 1:
        SetTilesStatus(0, 0, a3, 1, a5, a6);
        SetTilesStatus(0, 0, 1, a3, a5, a6);
        break;
    case 2:
        SetTilesStatus(m_box->m_width - a3, 0, a3, 1, a5, a6);
        SetTilesStatus(m_box->m_width - 1, 0, 1, a3, a5, a6);
        break;
    case 3:
        SetTilesStatus(m_box->m_width - a3, m_box->m_depth - 1, a3, 1, a5, a6);
        SetTilesStatus(m_box->m_width - 1, m_box->m_depth - a3, 1, a3, a5, a6);
        break;
    }
}

// 0x5918E0
int32 Interior_c::GetTileStatus(int32 x, int32 y) {
    if (x < (int32)m_box->m_width && y < (int32)m_box->m_depth && x >= 0 && y >= 0) {
        return field_68[x * TILE_ROW_STRIDE + y];
    }
    return 1;
}

// 0x591920
int32 Interior_c::GetNumEmptyTiles(int32 a2, int32 a3, int32 a4, int32 a5) {
    int32      numEmptyTiles = 0;
    const auto step          = a4 == 3 || a4 == 0 ? -1 : 1;

    if (a4 == 3 || a4 == 1) { // Scan along Y
        for (auto x = a2;; x += step) {
            for (auto i = 0, y = a3; i < a5; i++, y++) {
                if (x >= (int32)m_box->m_width || y >= (int32)m_box->m_depth || x < 0 || y < 0 || field_68[x * TILE_ROW_STRIDE + y] != 0) {
                    return numEmptyTiles;
                }
            }
            numEmptyTiles++;
        }
    } else { // Scan along X
        for (auto y = a3;; y += step) {
            for (auto i = 0, x = a2; i < a5; i++, x++) {
                if (x >= (int32)m_box->m_width || y >= (int32)m_box->m_depth || x < 0 || y < 0 || field_68[x * TILE_ROW_STRIDE + y] != 0) {
                    return numEmptyTiles;
                }
            }
            numEmptyTiles++;
        }
    }
}

// 0x591B20
int32 Interior_c::GetRandomTile(int32 a2, int32* a3, int32* a4) {
    int32 x, y;
    do {
        x = RandomNumberInRange((float)m_box->m_width);
        y = RandomNumberInRange((float)m_box->m_depth);
    } while (GetTileStatus(x, y) != a2);

    *a3 = x;
    *a4 = y;
    return y;
}

// 0x59A590
void Interior_c::Shop_FurnishAisles() {
    const auto width = (int32)m_box->m_width;
    const auto depth = (int32)m_box->m_depth;
    const auto numX  = width - 6;
    const auto numY  = depth - 7;
    if (numX <= 0 || numY <= 0) {
        return;
    }

    AddGotoPt(2, 3, -0.5f, -0.5f);
    const auto maxY = depth - 3;
    AddGotoPt(2, maxY, -0.5f, 0.5f);

    int32 x = 0;
    for (; x < numX; x++) {
        auto y           = 4;

        const auto r = RandomNumberInRange(100.0f);
        int32      type;
        if (r > 50) {
            type = 0;
        } else if (r > 25) {
            type = 3;
        } else if (r > 10) {
            type = 6;
        } else {
            type = 9;
        }

        switch (x % 4) {
        case 0:
            if (x != width - 7) {
                for (auto i = numY; i > 0; i--) {
                    y += Shop_PlaceEdgeUnits(type, x + 3, y, 3);
                }
            }
            break;
        case 1:
            for (auto i = numY; i > 0; i--) {
                y += Shop_PlaceEdgeUnits(type, x + 3, y, 1);
            }
            break;
        case 2:
            for (auto i = 0; i < numY; i++) {
                Shop_AddShelfInfo(x + 3, i + 4, 3);
            }
            break;
        case 3:
            SetTilesStatus(x + 3, 4, 1, numY, 3, 0);
            AddGotoPt(x + 3, 3, -0.5f, -0.5f);
            AddGotoPt(x + 3, maxY, -0.5f, 0.5f);
            break;
        }
    }

    AddGotoPt(x + 3, 3, 0.5f, -0.5f);
    AddGotoPt(x + 3, maxY, 0.5f, 0.5f);
}

// 0x591BD0
CVector* Interior_c::GetTileCentre(float offsetX, float offsetY, CVector* pointsIn) {
    pointsIn->x = (float)-m_box->m_width * 0.5f + offsetX + 0.5f;
    pointsIn->y = (float)-m_box->m_depth * 0.5f + offsetY + 0.5f;
    pointsIn->z = (float)-m_box->m_height * 0.5f;
    RwV3dTransformPoints(pointsIn, pointsIn, 1, &m_matrix);
    return pointsIn;
}

// 0x591D20
void Interior_c::AddGotoPt(int32 x, int32 y, float biasX, float biasY) {
    if (field_40C >= 16) {
        return;
    }
    if ((x < (int32)m_box->m_width && y < (int32)m_box->m_depth && x >= 0 && y >= 0 && field_68[x * TILE_ROW_STRIDE + y] == 3)
        || GetTileStatus(x, y) == 7) {
        CVector pos;
        GetTileCentre((float)x + biasX, (float)y + biasY, &pos);

        // Each goto slot is 0x10 bytes at 0x410: int8 tileX at +0, int8 tileY at +1, CVector pos at +4
        auto* slotBase = reinterpret_cast<uint8*>(this) + 0x410 + (int32)field_40C * 0x10;
        slotBase[0x0] = (int8)x;
        slotBase[0x1] = (int8)y;
        *reinterpret_cast<CVector*>(slotBase + 0x4) = pos;

        if (x >= 0 && y >= 0 && x + 1 <= (int32)m_box->m_width && y + 1 <= (int32)m_box->m_depth) {
            auto& tile = field_68[x * TILE_ROW_STRIDE + y];
            if (tile == 3 || tile == 0) {
                tile = 4;
            }
        }
        field_40C++;
    }
}

// 0x591E40
bool Interior_c::AddInteriorInfo(int32 actionType, float offsetX, float offsetY, int32 direction, CEntity* entityIgnoredCollision) {
    if (m_interiorInfosCount >= 16) {
        return false;
    }

    CVector pos;
    GetTileCentre(offsetX, offsetY, &pos);
    pos.z += 0.8f;

    CVector dir{};
    if (direction != -1) {
        switch (direction) {
        case 0:
            dir.y = -1.0f;
            break;
        case 1:
            dir.x = 1.0f;
            break;
        case 2:
            dir.y = 1.0f;
            break;
        case 3:
            dir.x = -1.0f;
            break;
        }
        RwV3dTransformVectors(&dir, &dir, 1, &m_matrix);
    }

    auto& info                  = m_interiorInfos[m_interiorInfosCount];
    info.Type                   = static_cast<eInteriorInfoType>(actionType);
    info.Pos                    = pos;
    info.Dir                    = dir;
    info.IsInUse                = false;
    info.EntityIgnoredCollision = entityIgnoredCollision;
    m_interiorInfosCount++;
    return true;
}

// 0x591F90
void Interior_c::AddPickups() {
    static auto& s_LastPickupTimeMs = StaticRef<uint32>(0xBB3DC4);
    if (CTimer::GetTimeInMS() - s_LastPickupTimeMs <= 179999) {
        return;
    }
    int32 numPlaced = 0;
    for (int32 i = 0; i < 100 && numPlaced <= 0; i++) {
        const auto x = RandomNumberInRange((float)m_box->m_width - 1.0f);
        const auto y = RandomNumberInRange((float)m_box->m_depth - 1.0f);
        if (x >= (int32)m_box->m_width || y >= (int32)m_box->m_depth || x < 0 || y < 0) {
            continue;
        }
        const auto tile = field_68[x * TILE_ROW_STRIDE + y];
        if (tile != 0 && tile != 3 && tile != 4) {
            continue;
        }
        CVector pos;
        GetTileCentre((float)x, (float)y, &pos);
        if (RandomNumberInRange(100.0f) < 75) {
            CPickups::GenerateNewOne(pos, ModelIndices::MI_MONEY, PICKUP_MONEY, 10 - RandomNumberInRange(-40.0f));
        } else {
            pos.z += 0.5f;
            int32 weaponType;
            const auto r = RandomNumberInRange(100.0f);
            if (r < 40) {
                weaponType = WEAPON_BASEBALLBAT;
            } else if (r < 80) {
                weaponType = WEAPON_PISTOL;
            } else {
                weaponType = ((0x59 < r) - 1 & 0xFFFFFFEB) + 0x19;
            }
            CPickups::GenerateNewOne_WeaponType(pos, (eWeaponType)weaponType, PICKUP_ONCE, 3 - RandomNumberInRange(-15.0f), false, nullptr);
        }
        numPlaced++;
    }
}

// 0x5922C0
void Interior_c::FindBoundingBox(int32 x, int32 y, int32* minX, int32* maxX, int32* minY, int32* maxY, int32* visited) {
    int32 nextY = y + 1;
    while (true) {
        // Neighbour (x - 1, y)
        if (x > 0 && x - 1 < (int32)m_box->m_width && y < (int32)m_box->m_depth && x - 1 >= 0 && y >= 0) {
            const auto idx = (x - 1) * TILE_ROW_STRIDE + y;
            if (field_68[idx] == 5 && !visited[idx]) {
                visited[idx] = 1;
                if (x - 1 < *minX) {
                    *minX = x - 1;
                }
                FindBoundingBox(x - 1, y, minX, maxX, minY, maxY, visited);
            }
        }
        // Neighbour (x, y + 1)
        if (y < 29 && x < (int32)m_box->m_width && nextY < (int32)m_box->m_depth && x >= 0 && nextY >= 0) {
            const auto idx = x * TILE_ROW_STRIDE + nextY;
            if (field_68[idx] == 5 && !visited[idx]) {
                visited[idx] = 1;
                if (nextY > *maxY) {
                    *maxY = nextY;
                }
                FindBoundingBox(x, nextY, minX, maxX, minY, maxY, visited);
            }
        }
        // Neighbour (x + 1, y)
        if (x < 29) {
            const auto nextX = x + 1;
            if (nextX < (int32)m_box->m_width && y < (int32)m_box->m_depth && nextX >= 0) {
                if (y < 0) {
                    return;
                }
                const auto idx = nextX * TILE_ROW_STRIDE + y;
                if (field_68[idx] == 5 && !visited[idx]) {
                    visited[idx] = 1;
                    if (nextX > *maxX) {
                        *maxX = nextX;
                    }
                    FindBoundingBox(nextX, y, minX, maxX, minY, maxY, visited);
                }
            }
        }
        // Neighbour (x, y - 1), handled iteratively
        if (y <= 0) {
            return;
        }
        if (x >= (int32)m_box->m_width) {
            return;
        }
        const auto prevY = nextY - 2;
        if (prevY >= (int32)m_box->m_depth) {
            return;
        }
        if (x < 0) {
            return;
        }
        if (prevY < 0) {
            return;
        }
        const auto idx = x * TILE_ROW_STRIDE + prevY;
        if (field_68[idx] != 5) {
            return;
        }
        if (visited[idx]) {
            return;
        }
        visited[idx] = 1;
        if (prevY < *minY) {
            *minY = prevY;
        }
        y--;
        nextY--;
    }
}

// 0x5924A0
void Interior_c::CalcExitPts() {
    plugin::CallMethod<0x5924A0, Interior_c*>(this);
}

// 0x5929F0
bool Interior_c::IsVisible() {
    const auto& camPos = TheCamera.GetPosition();
    if (IsPtInside(camPos, { 5.0f, 5.0f, 0.0f })) {
        return true;
    }
    if (m_box->m_door > 0) {
        const auto dx = camPos.x - m_position.x;
        const auto dy = camPos.y - m_position.y;
        if (dx * dx + dy * dy < 100.0f) {
            return true;
        }
    }
    return false;
}

// 0x592AA0
void Interior_c::PlaceFurniture(Furniture_c* a1, int32 a2, int32 a3, float a4, int32 a5, int32 a6, int32* a7, int32* a8, uint8 a9) {
    plugin::CallMethod<0x592AA0, Interior_c*, Furniture_c*, int32, int32, float, int32, int32, int32*, int32*, uint8>(this, a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

// 0x593120
void Interior_c::PlaceFurnitureOnWall(int32 furnitureGroupId, int32 furnitureSubgroupId, int32 furnitureId, float a5, int32 a6, int32 a7, int32 a8, int32 a9, int32* a10, int32* a11, int32* a12, int32* a13, int32* a14, int32* a15) {
    plugin::CallMethod<0x593120, Interior_c*, int32, int32, int32, float, int32, int32, int32, int32, int32*, int32*, int32*, int32*, int32*, int32*>(
        this, furnitureGroupId, furnitureSubgroupId, furnitureId, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15
    );
}

// 0x593340
void Interior_c::PlaceFurnitureInCorner(int32 furnitureGroupId, int32 furnitureSubgroupId, int32 id, float a4, int32 a5, int32 a6, int32 a2, int32* a9, int32* a10, int32* a11, int32* a12, int32* a13) {
    return plugin::CallMethod<0x593340, Interior_c*, int32, int32, int32, float, int32, int32, int32, int32*, int32*, int32*, int32*, int32*>(this, furnitureGroupId, furnitureSubgroupId, id, a4, a5, a6, a2, a9, a10, a11, a12, a13);
}
// 0x591C50
bool Interior_c::FindEmptyTiles(int32 a3, int32 a4, int32* arg8, int32* a5) {
    for (auto i = 0; i < 100; i++) {
        const auto x = RandomNumberInRange((float)m_box->m_width);
        const auto y = RandomNumberInRange((float)m_box->m_depth);
        if (CheckTilesEmpty(x, y, a3, a4, true)) {
            *arg8 = x;
            *a5   = y;
            return true;
        }
    }
    return false;
}
void Interior_c::FurnishShop(int32 a2) {
    m_furnitureGroupId = static_cast<int8>(a2);

    const auto door    = (int32)m_box->m_door;
    if (door - 1 > 5 || (int32)m_box->m_width - door > 5) {
        SetTilesStatus(0, 0, 1, 1, 2, 0);
        SetTilesStatus(0, m_box->m_depth - 1, 1, 1, 2, 0);
        SetTilesStatus(m_box->m_width - 1, 0, 1, 1, 2, 0);
        SetTilesStatus(m_box->m_width - 1, m_box->m_depth - 1, 1, 1, 2, 0);

        Shop_PlaceFixedUnits();
        plugin::CallMethod<0x59A1B0, Interior_c*>(this); // Shop_FurnishEdges: not yet reversed
        Shop_FurnishAisles();
    }
}

// 0x59A1B0
void Interior_c::Shop_FurnishEdges() {
    plugin::CallMethod<0x59A1B0, Interior_c*>(this);
}

// 0x59A130 - empty in the original binary (RET only)
void Interior_c::Shop_FurnishCeiling() {
}

// 0x59A140
void Interior_c::Shop_AddShelfInfo(int32 a2, int32 a3, int32 a5) {
    static auto& ctr = StaticRef<int32>(0x8D0948);
    if (ctr > 1) {
        // rand() * (1.0f / 32767.0f) * 100.0f, truncated to int: chance roll must exceed 60
        const auto roll = (int32)((float)rand() * (1.0f / 32767.0f) * 100.0f);
        if (roll > 60) {
            AddInteriorInfo(8, (float)a2, (float)a3, a5, nullptr);
            ctr = 1;
            return;
        }
    }
    ctr++;
}
