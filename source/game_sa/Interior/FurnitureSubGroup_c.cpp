#include "StdInc.h"
#include "FurnitureSubGroup_c.h"

void FurnitureSubGroup_c::InjectHooks() {
    RH_ScopedClass(FurnitureSubGroup_c);
    RH_ScopedCategory("Interior");

    RH_ScopedInstall(GetFurniture, 0x590EE0, { .reversed = false });
    RH_ScopedInstall(GetRandomId, 0x590FD0, { .reversed = false });
    RH_ScopedInstall(AddFurniture, 0x5C00C0, { .reversed = false });
}

// 0x5C00C0
bool FurnitureSubGroup_c::AddFurniture(uint16 modelId, int16 refId, uint8 minRating, uint8 maxRating, uint8 maxAngle) {
    return plugin::CallMethodAndReturn<bool, 0x5C00C0, FurnitureSubGroup_c*, uint16, int16, uint8, uint8, uint8>(this, modelId, refId, minRating, maxRating, maxAngle);
}

// 0x590EE0
Furniture_c* FurnitureSubGroup_c::GetFurniture(int16 refId, uint8 rating) {
    return plugin::CallMethodAndReturn<Furniture_c*, 0x590EE0, FurnitureSubGroup_c*, int16, uint8>(this, refId, rating);
}

// 0x590FD0
int32 FurnitureSubGroup_c::GetRandomId(uint8 rating) {
    return plugin::CallMethodAndReturn<int32, 0x590FD0, FurnitureSubGroup_c*, uint8>(this, rating);
}

void FurnitureSubGroup_c::Exit() {
    m_FurnitureList.RemoveAll();
}
