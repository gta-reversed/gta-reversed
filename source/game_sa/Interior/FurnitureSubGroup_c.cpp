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
bool FurnitureSubGroup_c::AddFurniture(uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng) {
    return plugin::CallMethodAndReturn<bool, 0x5C00C0, FurnitureSubGroup_c*, uint16, int16, uint8, uint8, uint8>(this, modelId, id, wealthMin, wealthMax, maxAng);
}

// 0x590EE0
Furniture_c* FurnitureSubGroup_c::GetFurniture(int16 id, uint8 wealth) {
    return plugin::CallMethodAndReturn<Furniture_c*, 0x590EE0, FurnitureSubGroup_c*, int16, uint8>(this, id, wealth);
}

// 0x590FD0
int32 FurnitureSubGroup_c::GetRandomId(uint8 a2) {
    return plugin::CallMethodAndReturn<int32, 0x590FD0, FurnitureSubGroup_c*, uint8>(this, a2);
}

void FurnitureSubGroup_c::Exit() {
    m_Furnitures.RemoveAll();
}
