#include "StdInc.h"
#include "FurnitureSubGroup_c.h"
#include "Furniture_c.h"

void FurnitureSubGroup_c::InjectHooks() {
    RH_ScopedClass(FurnitureSubGroup_c);
    RH_ScopedCategory("Interior");

    RH_ScopedInstall(GetFurniture, 0x590EE0);
    RH_ScopedInstall(GetRandomId, 0x590FD0);
    RH_ScopedInstall(AddFurniture, 0x5C00C0, { .Reversed = false });
}

// 0x5C00C0
bool FurnitureSubGroup_c::AddFurniture(uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng) {
    return plugin::CallMethodAndReturn<bool, 0x5C00C0, FurnitureSubGroup_c*, uint16, int16, uint8, uint8, uint8>(this, modelId, id, wealthMin, wealthMax, maxAng);
}

// 0x590EE0
Furniture_c* FurnitureSubGroup_c::GetFurniture(int16 id, uint8 wealth) {
    if (id > -1) {
        for (auto* furniture = m_Furnitures.GetHead(); furniture; furniture = m_Furnitures.GetNext(furniture)) {
            if (furniture->m_nId == id) {
                return furniture;
            }
        }
        return nullptr;
    }


    // Сколько фурнитуры попадает в диапазон богатства
    int32 numInWealthRange = 0;
    for (auto* furniture = m_Furnitures.GetHead(); furniture; furniture = m_Furnitures.GetNext(furniture)) {
        if (uint8(furniture->m_nWealthMin) <= wealth && wealth <= uint8(furniture->m_nWealthMax)) {
            numInWealthRange++;
        }
    }

    const auto rnd = int32((rand() & 0xFFFF) * (1.0f / 32768.0f) /* 0x858B14 */ * (float)numInWealthRange); // __ftol2

    int32 i = 0;
    for (auto* furniture = m_Furnitures.GetHead(); furniture; furniture = m_Furnitures.GetNext(furniture)) {
        if (uint8(furniture->m_nWealthMin) <= wealth && wealth <= uint8(furniture->m_nWealthMax)) {
            if (i == rnd) {
                return furniture;
            }
            i++;
        }
    }
    return nullptr;
}

// 0x590FD0
int32 FurnitureSubGroup_c::GetRandomId(uint8 wealth) {

    // Сколько фурнитуры попадает в диапазон богатства
    int32 numInWealthRange = 0;
    for (auto* furniture = m_Furnitures.GetHead(); furniture; furniture = m_Furnitures.GetNext(furniture)) {
        if (uint8(furniture->m_nWealthMin) <= wealth && wealth <= uint8(furniture->m_nWealthMax)) {
            numInWealthRange++;
        }
    }

    const auto rnd = int32((rand() & 0xFFFF) * (1.0f / 32768.0f) /* 0x858B14 */ * (float)numInWealthRange); // __ftol2

    int32 i = 0;
    for (auto* furniture = m_Furnitures.GetHead(); furniture; furniture = m_Furnitures.GetNext(furniture)) {
        if (uint8(furniture->m_nWealthMin) <= wealth && wealth <= uint8(furniture->m_nWealthMax)) {
            if (i == rnd) {
                return furniture->m_nId;
            }
            i++;
        }
    }
    return -1;
}

void FurnitureSubGroup_c::Exit() {
    m_Furnitures.RemoveAll();
}
