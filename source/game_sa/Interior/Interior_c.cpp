#include "StdInc.h"
#include "Interior_c.h"

void Interior_c::InjectHooks() {
    RH_ScopedClass(Interior_c);
    RH_ScopedCategory("Interior");

    //RH_ScopedInstall(Constructor, 0x5921D0, { .reversed = false });
    //RH_ScopedInstall(Destructor, 0x591360, { .reversed = false });

    RH_ScopedInstall(Bedroom_AddTableItem, 0x593F10, { .reversed = false });
    RH_ScopedInstall(FurnishBedroom, 0x593FC0, { .reversed = false });
    RH_ScopedInstall(Kitchen_FurnishEdges, 0x596930, { .reversed = false });
    RH_ScopedInstall(FurnishKitchen, 0x5970B0, { .reversed = false });
    RH_ScopedInstall(Lounge_AddTV, 0x597240, { .reversed = false });
    RH_ScopedInstall(Lounge_AddHifi, 0x597430, { .reversed = false });
    RH_ScopedInstall(Lounge_AddChairInfo, 0x5974E0, { .reversed = false });
    RH_ScopedInstall(Lounge_AddSofaInfo, 0x5975C0, { .reversed = false });
    RH_ScopedInstall(FurnishLounge, 0x597740, { .reversed = false });
    RH_ScopedInstall(Office_PlaceEdgeFillers, 0x599210, { .reversed = false });
    RH_ScopedInstall(Office_PlaceDesk, 0x5993E0, { .reversed = false });
    RH_ScopedInstall(Office_PlaceEdgeDesks, 0x5995B0, { .reversed = false });
    RH_ScopedInstall(Office_FurnishEdges, 0x599770, { .reversed = false });
    RH_ScopedInstall(Office_PlaceDeskQuad, 0x599960, { .reversed = false });
    RH_ScopedInstall(Office_FurnishCenter, 0x599A30, { .reversed = false });
    RH_ScopedInstall(FurnishOffice, 0x599AF0, { .reversed = false });
    RH_ScopedInstall(Shop_Place3PieceUnit, 0x599BB0, { .reversed = false });
    RH_ScopedInstall(Shop_PlaceEdgeUnits, 0x599DC0, { .reversed = false });
    RH_ScopedInstall(Shop_PlaceCounter, 0x599EF0, { .reversed = false });
    RH_ScopedInstall(Shop_PlaceFixedUnits, 0x59A030, { .reversed = false });
    RH_ScopedInstall(Shop_FurnishCeiling, 0x59A130, { .reversed = false });
    RH_ScopedInstall(Shop_AddShelfInfo, 0x59A140, { .reversed = false });
    RH_ScopedInstall(Shop_FurnishEdges, 0x59A1B0, { .reversed = false });
    RH_ScopedInstall(GetBoundingBox, 0x593DB0, { .reversed = false });
    RH_ScopedInstall(Init, 0x593BF0, { .reversed = false });
    RH_ScopedInstall(ResetTiles, 0x593910, { .reversed = false });
    RH_ScopedInstall(PlaceObject, 0x5934E0, { .reversed = false });
    RH_ScopedInstall(GetFurnitureEntity, 0x5913B0, { .reversed = false });
    RH_ScopedInstall(IsPtInside, 0x5913E0, { .reversed = false });
    RH_ScopedInstall(CalcMatrix, 0x5914D0, { .reversed = false });
    RH_ScopedInstall(Furnish, 0x591590, { .reversed = false });
    RH_ScopedInstall(Unfurnish, 0x5915D0, { .reversed = false });
    RH_ScopedInstall(CheckTilesEmpty, 0x591680, { .reversed = false });
    RH_ScopedInstall(SetTilesStatus, 0x591700, { .reversed = false });
    RH_ScopedInstall(SetCornerTiles, 0x5917C0, { .reversed = false });
    RH_ScopedInstall(GetTileStatus, 0x5918E0, { .reversed = false });
    RH_ScopedInstall(GetNumEmptyTiles, 0x591920, { .reversed = false });
    RH_ScopedInstall(GetRandomTile, 0x591B20, { .reversed = false });
    RH_ScopedInstall(Shop_FurnishAisles, 0x59A590, { .reversed = false });
    RH_ScopedInstall(GetTileCentre, 0x591BD0, { .reversed = false });
    RH_ScopedInstall(AddGotoPt, 0x591D20, { .reversed = false });
    RH_ScopedInstall(AddInteriorInfo, 0x591E40, { .reversed = false });
    RH_ScopedInstall(AddPickups, 0x591F90, { .reversed = false });
    RH_ScopedInstall(Exit, 0x592230, { .reversed = false });
    RH_ScopedInstall(FindBoundingBox, 0x5922C0, { .reversed = false });
    RH_ScopedInstall(CalcExitPts, 0x5924A0, { .reversed = false });
    RH_ScopedInstall(IsVisible, 0x5929F0, { .reversed = false });
    RH_ScopedInstall(PlaceFurniture, 0x592AA0, { .reversed = false });
    RH_ScopedInstall(PlaceFurnitureOnWall, 0x593120, { .reversed = false });
    RH_ScopedInstall(PlaceFurnitureInCorner, 0x593340, { .reversed = false });
    RH_ScopedInstall(FindEmptyTiles, 0x591C50, { .reversed = false });
    RH_ScopedInstall(FurnishShop, 0x59A790, { .reversed = false });
}

// 0x593BF0
int32 Interior_c::Init(const CVector& pos) {
    return plugin::CallMethodAndReturn<int32, 0x593BF0>(this, &pos);
}

// 0x592230
void Interior_c::Exit() {
    plugin::CallMethod<0x592230, Interior_c*>(this);
}

// 0x593F10
CObject* Interior_c::Bedroom_AddTableItem(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6, int32 a7) {
    return plugin::CallMethodAndReturn<CObject*, 0x593F10, Interior_c*, int32, int32, int32, int32, int32, int32>(this, a2, a3, a4, a5, a6, a7);
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
    plugin::CallMethod<0x5970B0, Interior_c*>(this);
}

// 0x597240
CObject* Interior_c::Lounge_AddTV(int32 a2, int32 a3, int32 a4, int32 a5) {
    return plugin::CallMethodAndReturn<CObject*, 0x597240, Interior_c*, int32, int32, int32, int32>(this, a2, a3, a4, a5);
}

// 0x597430
CObject* Interior_c::Lounge_AddHifi(int32 a2, int32 a3, int32 a4, int32 a5) {
    return plugin::CallMethodAndReturn<CObject*, 0x597430, Interior_c*, int32, int32, int32, int32>(this, a2, a3, a4, a5);
}

// 0x5974E0
void Interior_c::Lounge_AddChairInfo(int32 a2, int32 a3, CEntity* entityIgnoredCollision) {
    plugin::CallMethod<0x5974E0, Interior_c*, int32, int32, CEntity*>(this, a2, a3, entityIgnoredCollision);
}

// 0x5975C0
void Interior_c::Lounge_AddSofaInfo(int32 sitType, int32 offsetX, CEntity* entityIgnoredCollision) {
    plugin::CallMethod<0x5975C0, Interior_c*, int32, int32, CEntity*>(this, sitType, offsetX, entityIgnoredCollision);
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
    plugin::CallMethod<0x599770, Interior_c*>(this);
}

// 0x599960
int32 Interior_c::Office_PlaceDeskQuad(int32 a2, int32 a3, int32 a4, int32 a5) {
    return plugin::CallMethodAndReturn<int32, 0x599960, Interior_c*, int32, int32, int32, int32>(this, a2, a3, a4, a5);
}

// 0x599A30
int32 Interior_c::Office_FurnishCenter() {
    return plugin::CallMethodAndReturn<int32, 0x599A30, Interior_c*>(this);
}

// 0x599AF0
void Interior_c::FurnishOffice() {
    plugin::CallMethod<0x599AF0, Interior_c*>(this);
}

// 0x599BB0
int8 Interior_c::Shop_Place3PieceUnit(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6) {
    return plugin::CallMethodAndReturn<int8, 0x599BB0, Interior_c*, int32, int32, int32, int32, int32>(this, a2, a3, a4, a5, a6);
}

// 0x599DC0
int32 Interior_c::Shop_PlaceEdgeUnits(int32 a2, int32 a3, int32 a4, int32 a5) {
    return plugin::CallMethodAndReturn<int32, 0x599DC0, Interior_c*, int32, int32, int32, int32>(this, a2, a3, a4, a5);
}

// 0x599EF0
int32 Interior_c::Shop_PlaceCounter(uint8 a2) {
    return plugin::CallMethodAndReturn<int32, 0x599EF0, Interior_c*, uint8>(this, a2);
}

// 0x59A030
void Interior_c::Shop_PlaceFixedUnits() {
    return plugin::CallMethod<0x59A030, Interior_c*>(this);
}

// 0x59A130
void Interior_c::Shop_FurnishCeiling() {
    plugin::CallMethod<0x59A130, Interior_c*>(this);
}

// 0x59A140
void Interior_c::Shop_AddShelfInfo(int32 a2, int32 a3, int32 a5) {
    plugin::CallMethod<0x59A140, Interior_c*, int32, int32, int32>(this, a2, a3, a5);
}

// 0x59A1B0
void Interior_c::Shop_FurnishEdges() {
    plugin::CallMethod<0x59A1B0, Interior_c*>(this);
}

// 0x593DB0
bool Interior_c::GetBoundingBox(FurnitureEntity_c* entity, CVector* a3) {
    return plugin::CallMethodAndReturn<bool, 0x593DB0, Interior_c*, FurnitureEntity_c*, CVector*>(this, entity, a3);
}

// 0x593910
void Interior_c::ResetTiles() {
    plugin::CallMethod<0x593910, Interior_c*>(this);
}

// 0x5934E0
CObject* Interior_c::PlaceObject(uint8 isStealable, Furniture_c* furniture, float offsetX, float offsetY, float offsetZ, float rotationZ) {
    return plugin::CallMethodAndReturn<CObject*, 0x5934E0, Interior_c*, uint8, Furniture_c*, float, float, float, float>(this, isStealable, furniture, offsetX, offsetY, offsetZ,
                                                                                                                         rotationZ);
}

// 0x5913B0
FurnitureEntity_c* Interior_c::GetFurnitureEntity(CEntity* entity) {
    return plugin::CallMethodAndReturn<FurnitureEntity_c*, 0x5913B0, Interior_c*, CEntity*>(this, entity);
}

// 0x5913E0
bool Interior_c::IsPtInside(const CVector& pt, CVector bias) {
    return plugin::CallMethodAndReturn<bool, 0x5913E0, Interior_c*, const CVector&, CVector&>(this, pt, bias);
}

// 0x5914D0
void Interior_c::CalcMatrix(CVector* translation) {
    plugin::CallMethod<0x5914D0, Interior_c*, CVector*>(this, translation);
}

// 0x591590
void Interior_c::Furnish() {
    plugin::CallMethod<0x591590, Interior_c*>(this);
}

// 0x5915D0
void Interior_c::Unfurnish() {
    plugin::CallMethod<0x5915D0, Interior_c*>(this);
}

// 0x591680
int8 Interior_c::CheckTilesEmpty(int32 a1, int32 a2, int32 a3, int32 a4, uint8 a5) {
    return plugin::CallMethodAndReturn<int8, 0x591680, Interior_c*, int32, int32, int32, int32, uint8>(this, a1, a2, a3, a4, a5);
}

// 0x591700
void Interior_c::SetTilesStatus(int32 a, int32 b, int32 a3, int32 a4, int32 a5, int8 a6) {
    plugin::CallMethod<0x591700, Interior_c*, int32, int32, int32, int32, int32, int8>(this, a, b, a3, a4, a5, a6);
}

// 0x5917C0
void Interior_c::SetCornerTiles(int32 a4, int32 a3, int32 a5, uint8 a6) {
    plugin::CallMethod<0x5917C0, Interior_c*, int32, int32, int32, uint8>(this, a4, a3, a5, a6);
}

// 0x5918E0
int32 Interior_c::GetTileStatus(int32 x, int32 y) {
    return plugin::CallMethodAndReturn<int32, 0x5918E0, Interior_c*, int32, int32>(this, x, y);
}

// 0x591920
int32 Interior_c::GetNumEmptyTiles(int32 a2, int32 a3, int32 a4, int32 a5) {
    return plugin::CallMethodAndReturn<int32, 0x591920, Interior_c*, int32, int32, int32, int32>(this, a2, a3, a4, a5);
}

// 0x591B20
int32 Interior_c::GetRandomTile(int32 a2, int32* a3, int32* a4) {
    return plugin::CallMethodAndReturn<int32, 0x591B20, Interior_c*, int32, int32*, int32*>(this, a2, a3, a4);
}

// 0x59A590
void Interior_c::Shop_FurnishAisles() {
    plugin::CallMethod<0x59A590, Interior_c*>(this);
}

// 0x591BD0
CVector* Interior_c::GetTileCentre(float offsetX, float offsetY, CVector* pointsIn) {
    return plugin::CallMethodAndReturn<CVector*, 0x591BD0, Interior_c*, float, float, CVector*>(this, offsetX, offsetY, pointsIn);
}

// 0x591D20
void Interior_c::AddGotoPt(int32 a, int32 b, float a3, float a4) {
    plugin::CallMethod<0x591D20, Interior_c*, int32, int32, float, float>(this, a, b, a3, a4);
}

// 0x591E40
bool Interior_c::AddInteriorInfo(int32 actionType, float offsetX, float offsetY, int32 direction, CEntity* entityIgnoredCollision) {
    return plugin::CallMethodAndReturn<bool, 0x591E40, Interior_c*, int32, float, float, int32, CEntity*>(this, actionType, offsetX, offsetY, direction, entityIgnoredCollision);
}

// 0x591F90
void Interior_c::AddPickups() {
    plugin::CallMethod<0x591F90, Interior_c*>(this);
}

// 0x5922C0
void Interior_c::FindBoundingBox(int32 a1, int32 a2, int32* a3, int32* a4, int32* a5, int32* a6, int32* a7) {
    plugin::CallMethod<0x5922C0, Interior_c*, int32, int32, int32*, int32*, int32*, int32*, int32*>(this, a1, a2, a3, a4, a5, a6, a7);
}

// 0x5924A0
void Interior_c::CalcExitPts() {
    plugin::CallMethod<0x5924A0, Interior_c*>(this);
}

// 0x5929F0
bool Interior_c::IsVisible() {
    return plugin::CallMethodAndReturn<bool, 0x5929F0, Interior_c*>(this);
}

// 0x592AA0
void Interior_c::PlaceFurniture(Furniture_c* a1, int32 a2, int32 a3, float a4, int32 a5, int32 a6, int32* a7, int32* a8, uint8 a9) {
    plugin::CallMethod<0x592AA0, Interior_c*, Furniture_c*, int32, int32, float, int32, int32, int32*, int32*, uint8>(this, a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

// 0x593120
void Interior_c::PlaceFurnitureOnWall(int32 furnitureGroupId, int32 furnitureSubgroupId, int32 furnitureId, float a5, int32 a6, int32 a7, int32 a8, int32 a9, int32* a10,
                                      int32* a11, int32* a12, int32* a13, int32* a14, int32* a15) {
    plugin::CallMethod<0x593120, Interior_c*, int32, int32, int32, float, int32, int32, int32, int32, int32*, int32*, int32*, int32*, int32*, int32*>(
        this, furnitureGroupId, furnitureSubgroupId, furnitureId, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15);
}

// 0x593340
void Interior_c::PlaceFurnitureInCorner(int32 furnitureGroupId, int32 furnitureSubgroupId, int32 id, float a4, int32 a5, int32 a6, int32 a2, int32* a9, int32* a10, int32* a11,
                                        int32* a12, int32* a13) {
    plugin::CallMethod<0x593340, Interior_c*, int32, int32, int32, float, int32, int32, int32, int32*, int32*, int32*, int32*, int32*>(this, furnitureGroupId, furnitureSubgroupId,
                                                                                                                                       id, a4, a5, a6, a2, a9, a10, a11, a12, a13);
}

// 0x591C50
bool Interior_c::FindEmptyTiles(int32 a3, int32 a4, int32* arg8, int32* a5) {
    return plugin::CallMethodAndReturn<bool, 0x591C50, Interior_c*, int32, int32, int32*, int32*>(this, a3, a4, arg8, a5);
}

// 0x59A790
void Interior_c::FurnishShop(int32 a2) {
    plugin::CallMethod<0x59A790, Interior_c*, int32>(this, a2);
}
