#include "StdInc.h"
#include "FurnitureSubGroup_c.h"
#include "Furniture_c.h"
#include "FurnitureManager_c.h"

void FurnitureSubGroup_c::InjectHooks() {
    RH_ScopedClass(FurnitureSubGroup_c);
    RH_ScopedCategory("Interior");

    RH_ScopedInstall(GetFurniture, 0x590EE0);
    RH_ScopedInstall(GetRandomId, 0x590FD0);
    RH_ScopedInstall(AddFurniture, 0x5C00C0);
}

// 0x5C00C0
bool FurnitureSubGroup_c::AddFurniture(uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng) {
    if (FurnitureManager_c::g_currFurnitureId >= 256) {
        return false;
    }
    auto* const f = &FurnitureManager_c::g_furnitureStore[FurnitureManager_c::g_currFurnitureId++];
    f->m_nModelId = modelId;
    f->m_nId = id;
    const auto& box = CModelInfo::GetModelInfo(modelId)->GetColModel()->GetBoundingBox();
    // Widths are the bounding-box size plus 1.0 (0.5 if the subgroup can be placed in front of a window), truncated
    const auto extra = m_bCanPlaceInFrontOfWindow ? 0.5f : 1.0f;
    f->m_nWidthX = (int8)(box.GetWidth() + extra);
    f->m_nWidthY = (int8)(box.GetLength() + extra);
    f->m_nWealthMax = wealthMax;
    f->m_nWealthMin = wealthMin;
    f->m_nMaxAng = maxAng;
    f->m_bCanPlaceInFrontOfWindow = m_bCanPlaceInFrontOfWindow;
    f->m_bIsTall = m_bIsTall;
    f->m_bCanSteal = m_bCanSteal;
    m_Furnitures.AddItem(f);
    return true;
}

// 0x590EE0
Furniture_c* FurnitureSubGroup_c::GetFurniture(int16 id, uint8 wealth) {
    if (id != -1) {
        for (auto* f = m_Furnitures.GetHead(); f; f = m_Furnitures.GetNext(f)) {
            if (f->m_nId == id) {
                return f;
            }
        }
        return nullptr;
    }
    if ((int8)wealth == -1) {
        const auto n = m_Furnitures.GetNumItems();
        const auto r = (int32)((float)(CGeneral::GetRandomNumber() & 0xFFFF) * (1.0f / 32768.0f) * (float)n);
        return m_Furnitures.GetItemOffset(false, r);
    }
    int32 count = 0;
    for (auto* f = m_Furnitures.GetHead(); f; f = m_Furnitures.GetNext(f)) {
        if (wealth >= f->m_nWealthMin && wealth <= f->m_nWealthMax) {
            count++;
        }
    }
    const auto r = (int32)((float)(CGeneral::GetRandomNumber() & 0xFFFF) * (1.0f / 32768.0f) * (float)count);
    int32 idx = 0;
    for (auto* f = m_Furnitures.GetHead(); f; f = m_Furnitures.GetNext(f)) {
        if (wealth >= f->m_nWealthMin && wealth <= f->m_nWealthMax) {
            if (idx == r) {
                return f;
            }
            idx++;
        }
    }
    return nullptr;
}

// 0x590FD0
int32 FurnitureSubGroup_c::GetRandomId(uint8 a2) {
    if ((int8)a2 == -1) {
        const auto n = m_Furnitures.GetNumItems();
        const auto r = (int32)((float)(CGeneral::GetRandomNumber() & 0xFFFF) * (1.0f / 32768.0f) * (float)n);
        return m_Furnitures.GetItemOffset(false, r)->m_nId;
    }
    int32 count = 0;
    for (auto* f = m_Furnitures.GetHead(); f; f = m_Furnitures.GetNext(f)) {
        if (a2 >= f->m_nWealthMin && a2 <= f->m_nWealthMax) {
            count++;
        }
    }
    const auto r = (int32)((float)(CGeneral::GetRandomNumber() & 0xFFFF) * (1.0f / 32768.0f) * (float)count);
    int32 idx = 0;
    for (auto* f = m_Furnitures.GetHead(); f; f = m_Furnitures.GetNext(f)) {
        if (a2 >= f->m_nWealthMin && a2 <= f->m_nWealthMax) {
            if (idx == r) {
                return f->m_nId;
            }
            idx++;
        }
    }
    return -1;
}

void FurnitureSubGroup_c::Exit() {
    m_Furnitures.RemoveAll();
}
