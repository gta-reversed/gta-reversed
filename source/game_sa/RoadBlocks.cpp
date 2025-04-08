#include "StdInc.h"

#include "RoadBlocks.h"
#include <extensions/File.hpp>

void CRoadBlocks::InjectHooks() {
    RH_ScopedClass(CRoadBlocks);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x461100);
    RH_ScopedInstall(ClearScriptRoadBlocks, 0x460EC0);
    RH_ScopedInstall(ClearSpaceForRoadBlockObject, 0x461020);
    RH_ScopedInstall(CreateRoadBlockBetween2Points, 0x4619C0, { .reversed = false });
    RH_ScopedInstall(GenerateRoadBlockCopsForCar, 0x461170, { .reversed = false });
    RH_ScopedInstall(GenerateRoadBlocks, 0x4629E0, { .reversed = false });
    RH_ScopedInstall(GetRoadBlockNodeInfo, 0x460EE0, { .reversed = false });
    RH_ScopedInstall(RegisterScriptRoadBlock, 0x460DF0);
}

// 0x461100
void CRoadBlocks::Init() {
    rng::fill(InOrOut, true);
    GenerateDynamicRoadBlocks = false;

    if (notsa::File rbx("data\\paths\\roadblox.dat", "rb"); rbx) {
        rbx.Read(&NumRoadBlocks, sizeof(int32));
        rbx.Read(RoadBlockNodes.data(), RoadBlockNodes.size() * sizeof(CNodeAddress));
    } else {
        NOTSA_UNREACHABLE("roadblox.dat couldn't be opened!");
    }
    ClearScriptRoadBlocks();
}

// 0x460EC0
void CRoadBlocks::ClearScriptRoadBlocks() {
    for (auto& srb : aScriptRoadBlocks) {
        srb.IsActive = false;
    }
}

// 0x461020
// Returns true if cleared successfully.
bool CRoadBlocks::ClearSpaceForRoadBlockObject(CVector cornerA, CVector cornerB){
    int16 numObjects{};
    CObject* objects[2]{};
    CWorld::FindObjectsIntersectingCube(
        cornerA,
        cornerB,
        &numObjects,
        2,
        reinterpret_cast<CEntity**>(objects),
        false,
        true,
        true,
        true,
        false
    );

    if (numObjects > 2 || numObjects <= 0) {
        return numObjects <= 0;
    }

    const auto Remove = [](CEntity* e) {
        CWorld::Remove(e);
        delete e;
    };

    for (auto* obj : objects | rngv::take(numObjects)) {
        switch (obj->GetType()) {
        case ENTITY_TYPE_VEHICLE:
            if (auto* v = obj->AsVehicle(); !v->CanBeDeleted()) {
                return false;
            } else if (!v->vehicleFlags.bCreateRoadBlockPeds) {
                Remove(v);
            }
            break;
        case ENTITY_TYPE_PED:
            if (auto* p = obj->AsPed(); p->CanBeDeleted()) {
                Remove(p);
            } else {
                return false;
            }
            break;
        case ENTITY_TYPE_OBJECT:
            if (obj->CanBeDeleted() && obj->m_nObjectType != OBJECT_GAME) {
                Remove(obj);
            } else {
                return false;
            }
            break;
        default:
            NOTSA_UNREACHABLE();
        }
    }

    return true;
}

// 0x4619C0
void CRoadBlocks::CreateRoadBlockBetween2Points(CVector a1, CVector a2, uint32 a3) {
    plugin::Call<0x4619C0, CVector, CVector, uint32>(a1, a2, a3);
}

// 0x461170
void CRoadBlocks::GenerateRoadBlockCopsForCar(CVehicle* vehicle, int32 pedsPositionsType, ePedType type) {
    eCopType copType{ COP_TYPE_CITYCOP };
    eModelID pedModel{ MODEL_INVALID };
    bool     isSpecialCop{};

    if (type == PED_TYPE_COP) {
        switch (vehicle->GetModelID()) {
        case MODEL_ENFORCER:
            copType      = COP_TYPE_SWAT1;
            pedModel     = MODEL_SWAT;
            isSpecialCop = true;
            break;
        case MODEL_BARRACKS:
            copType      = COP_TYPE_ARMY;
            pedModel     = MODEL_ARMY;
            isSpecialCop = true;
            break;
        case MODEL_FBIRANCH:
            copType      = COP_TYPE_FBI;
            pedModel     = MODEL_FBI;
            isSpecialCop = true;
            break;
        case MODEL_COPCARRU:
            isSpecialCop = true;
            [[fallthrough]];
        default:
            copType = COP_TYPE_CITYCOP;
            break;
        }
    } else {
        if (type >= PED_TYPE_GANG1 && type <= PED_TYPE_GANG10) {
            bool found{};
            for (auto i = 0; i < TOTAL_GANGS; i++) {
                if (CPopCycle::m_pCurrZoneInfo->GangStrength[i]) {
                    pedModel = CGangs::ChooseGangPedModel((eGangID)i);
                    if (pedModel != MODEL_INVALID) {
                        found = true;
                        break;
                    }
                }
            }

            if (!found) {
                return;
            }
        }
    }

    static constexpr CVector PLACEMENTS[] = {
        { -1.5, +1.9, 0.0 },
        { -1.5, -2.6, 0.0 },
        { +1.5, +1.9, 0.0 },
        { +1.5, -2.6, 0.0 },
        { -1.5,  0.0, 0.0 },
        { +1.5,  0.0, 0.0 },
        {  0.0, +3.2, 0.0 },
        { +1.5, -1.8, 0.0 },
        {  0.0, +3.2, 0.0 },
        { -1.5, -1.8, 0.0 },
        { -1.5,  0.0, 0.0 },
        { +1.5,  0.0, 0.0 } 
    };

    for (auto i = 0u; i < vehicle->m_nNumPedsForRoadBlock; i++) {
        // ...
    }
}

// 0x4629E0
void CRoadBlocks::GenerateRoadBlocks() {
    ZoneScoped;

    plugin::Call<0x4629E0>();
}

// 0x460EE0
bool CRoadBlocks::GetRoadBlockNodeInfo(CNodeAddress address, float& width, CVector& vec) {
    return false;
}

// 0x460DF0
void CRoadBlocks::RegisterScriptRoadBlock(CVector cornerA, CVector cornerB, bool isGangRoute) {
    auto free = rng::find_if(aScriptRoadBlocks, [](const auto& srb) { return !srb.IsActive; });
    if (free == aScriptRoadBlocks.end()) {
        // No free script roadblock found
        return;
    }

    free->CornerA     = cornerA;
    free->CornerB     = cornerB;
    free->IsActive    = true;
    free->IsCreated   = true;
    free->IsGangRoute = isGangRoute;
}
