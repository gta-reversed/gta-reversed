#include "StdInc.h"
#include "FurnitureSubGroup_c.h"
#include "Furniture_c.h"
#include "ModelInfo.h"
#include "ColModel.h"

void FurnitureSubGroup_c::InjectHooks() {
    RH_ScopedClass(FurnitureSubGroup_c);
    RH_ScopedCategory("Interior");

    RH_ScopedInstall(GetFurniture, 0x590EE0, { .Reversed = false });
    RH_ScopedInstall(GetRandomId, 0x590FD0, { .Reversed = false });
    RH_ScopedInstall(AddFurniture, 0x5C00C0);
}

// 0x5C00C0
bool FurnitureSubGroup_c::AddFurniture(uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng) {
    // The original accesses only the low 16 bits of the manager's counter.
    static auto& s_NumFurnitures = StaticRef<uint16>(0xBAB378);
    static auto& s_Furnitures    = StaticRef<std::array<Furniture_c, 256>>(0xBAE1F8);

    if (s_NumFurnitures >= s_Furnitures.size()) {
        return false;
    }

    auto* const furniture      = &s_Furnitures[s_NumFurnitures++];

    furniture->m_nModelId      = modelId;
    furniture->m_nId           = id;

    const auto* const colModel = CModelInfo::GetModelInfo(modelId)->GetColModel();
    const auto&       bb       = colModel->m_boundBox;
    float             widthX, widthY;
    if (!m_bCanSteal) {
        widthX = bb.m_vecMax.x + 0.5f;
        widthY = bb.m_vecMax.y + 0.5f;
    } else {
        widthX = bb.m_vecMax.x - bb.m_vecMin.x;
        widthY = bb.m_vecMax.y - bb.m_vecMin.y;
    }
    // Preserve the original truncation and round up when the fractional part is at least 0.02f.
    const auto widthXInt                  = static_cast<int32>(widthX);
    furniture->m_nWidthX                  = static_cast<int8>(widthXInt + (widthX - static_cast<float>(widthXInt) >= 0.02f ? 1 : 0));
    const auto widthYInt                  = static_cast<int32>(widthY);
    furniture->m_nWidthY                  = static_cast<int8>(widthYInt + (widthY - static_cast<float>(widthYInt) >= 0.02f ? 1 : 0));

    furniture->m_nWealthMin               = wealthMin;
    furniture->m_nWealthMax               = wealthMax;
    furniture->m_nMaxAng                  = maxAng;

    furniture->m_bCanPlaceInFrontOfWindow = m_bCanPlaceInFrontOfWindow;
    furniture->m_bIsTall                  = m_bIsTall;
    furniture->m_bCanSteal                = m_bCanSteal;

    m_Furnitures.AddItem(furniture);
    return true;
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
