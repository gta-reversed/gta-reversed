#include "StdInc.h"

#include "InteriorManager_c.h"
#include "FurnitureManager_c.h"

#include <TaskTypes/TaskSimpleCarDrive.h>

void InteriorManager_c::InjectHooks() {
    RH_ScopedClass(InteriorManager_c);
    RH_ScopedCategory("Interior");

    RH_ScopedGlobalInstall(AddSameGroupEffectInfos, 0x598430);
    RH_ScopedGlobalInstall(AreAnimsLoaded, 0x5980F0);
    RH_ScopedInstall(PruneVisibleEffects, 0x598A60);
    RH_ScopedInstall(Exit, 0x598010);
    RH_ScopedInstall(GetVisibleEffects, 0x598D80);
    RH_ScopedInstall(IsInteriorEffectVisible, 0x598690);
    RH_ScopedInstall(GetPedsInterior, 0x598620);
    RH_ScopedInstall(ReturnInteriorToPool, 0x5984B0);
    RH_ScopedInstall(GetInteriorFromPool, 0x5984A0);
    RH_ScopedInstall(GetVectorsInterior, 0x5983D0);
    RH_ScopedInstall(SetStealableObjectStolen, 0x598390);
    RH_ScopedOverloadedInstall(FindStealableObjectId, "1", 0x598360,  int32(InteriorManager_c::*)(CEntity *) const);
    RH_ScopedOverloadedInstall(FindStealableObjectId, "2", 0x5982F0,  int32(InteriorManager_c::*)(int32, int32, CVector) const);
    RH_ScopedInstall(HasInteriorHadStealDataSetup, 0x5982B0);
    RH_ScopedInstall(IsGroupActive, 0x598280);
    RH_ScopedInstall(GetPedsInteriorGroup, 0x598240);
    RH_ScopedInstall(SetEntryExitPtr, 0x598180);
    RH_ScopedInstall(GetBoundingBox, 0x598090);
    RH_ScopedInstall(ActivatePeds, 0x598080);
    RH_ScopedInstall(Update, 0x598F50);
    RH_ScopedInstall(Init, 0x5C0500);
}

// 0x5C0500
void InteriorManager_c::Init() {
    m_IsFrozen       = false;
    m_IsActive       = true;
    m_ArePedsEnabled = true;
    for (auto& i : m_Interiors) {
        m_InteriorPool.AddItem(&i);
    }
    for (auto&& [i, g] : rngv::enumerate(m_InteriorGroups)) {
        g.m_id = (uint8)i;
        m_InteriorGroupPool.AddItem(&g);
    }
    g_furnitureMan.Init();
    m_EnEx          = nullptr;
    m_InteriorCount = 0;
    m_ObjectCount   = 0;
    rng::fill(m_InteriorPedsAliveState, true);
    m_TimeLastPickupsGenerated = UINT32_MAX;
}

// 0x598F50
bool InteriorManager_c::Update() {
    ZoneScoped;

    if (m_IsFrozen) {
        return false;
    }

    const auto plyr = FindPlayerPed();

    InteriorEffectInfo_t visibleIntFxBuf[32];
    const auto numVisibleIntFx = plyr->m_nAreaCode != eAreaCodes::AREA_CODE_NORMAL_WORLD && m_IsActive && !plyr->GetTaskManager().GetActiveTaskAs<CTaskSimpleCarDrive>()
        ? GetVisibleEffects(visibleIntFxBuf, std::size(visibleIntFxBuf))
        : 0;
    PruneVisibleEffects(visibleIntFxBuf, numVisibleIntFx, 8, 20.f);
    const auto visibleIntFx = visibleIntFxBuf | rng::views::take(numVisibleIntFx);

    const auto IsFxAssociatedWithGroup = [](InteriorEffectInfo_t& fx, InteriorGroup_c& g) {
        return fx.Entity == g.GetEntity() && fx.Effects[0]->m_groupId == g.GetId();
    };

    // Remove interiors that are associated with effects that aren't visible anymore
    for (auto it = m_InteriorGroupList.begin(); it != m_InteriorGroupList.end();) {
        auto& g = *it;
        it++; // `RemoveItem` below invalidates the iterator, so increment here
        if (rng::none_of(visibleIntFx, [&](InteriorEffectInfo_t& fx) {
            return IsFxAssociatedWithGroup(fx, g) && !fx.IsCulled;
        })) {
            g.Exit();
            m_InteriorGroupList.RemoveItem(&g);
            m_InteriorGroupPool.AddItem(&g);
        }
    }

    bool hasAddedAnyInteriors{};
    for (auto& ifx : visibleIntFx) {
        //> 0x599071 - Not visible?
        if (ifx.IsCulled) {
            continue;
        }

        //> 0x59909F - Check if there's an existing interior group for this effect
        if (rng::any_of(m_InteriorGroupList, [&](InteriorGroup_c& g) {
            return IsFxAssociatedWithGroup(ifx, g);
        })) {
            continue;
        }

        //> 0x5990B8 - Make an interior group for this effect
        const auto grp = m_InteriorGroupPool.RemoveHead();
        assert(grp);
        grp->Init(ifx.Entity, ifx.Effects[0]->m_groupId);
        grp->m_EnEx = m_EnEx;
        m_InteriorGroupList.AddItem(grp);

        //> 0x59910C - Create interiors for it
        for (auto k = 0u; k < ifx.NumFx; k++) {
            const auto i = m_InteriorPool.RemoveHead();
            if (!i) { // No more interiors to allocate
                break;
            }
            const auto& fxPos = ifx.Entity->GetPosition();

            i->m_Props    = ifx.Effects[k];
            i->m_ID       = (uint32)(fxPos.x * fxPos.y * fxPos.z) + ifx.FxIds[k];
            i->m_AreaCode = ifx.Entity->m_nAreaCode;
            i->m_Group    = grp;

            i->Init(ifx.Effects[k]->m_Pos);
            grp->AddInterior(i);

            hasAddedAnyInteriors = true;
        }

        grp->Setup();

        m_TimeLastPickupsGenerated = CTimer::GetTimeInMS();
    }

    // Update all created groups
    for (auto& g : m_InteriorGroupList) {
        g.Update();
    }

    return hasAddedAnyInteriors;
}

// 0x598A60
void InteriorManager_c::PruneVisibleEffects(InteriorEffectInfo_t* pIntFxInfos, size_t numInfos, size_t maxVisibleFxCount, float maxDist) {
    const std::span intFxInfos{pIntFxInfos, (size_t)numInfos};

    // Check if there's anything to cull at all (More fxs visible than allowed)
    if ((size_t)rng::count(
        intFxInfos,
        0u,
        [](const InteriorEffectInfo_t& intFxInfo) { return intFxInfo.NumFx; }
    ) <= maxVisibleFxCount) {
        return;
    }

    //> 0x598AA3 - Calculate distances to camera
    for (auto& intFxInfo : intFxInfos) {
        intFxInfo.DistSq = sq(10'000.f);
        for (size_t k = intFxInfo.NumFx; k-->0;) {
            const auto fx = intFxInfo.Effects[k];
            intFxInfo.DistSq = std::min(
                intFxInfo.DistSq,
                CVector2D::DistSqr(intFxInfo.Entity->GetMatrix().TransformPoint(fx->m_Pos), TheCamera.GetPosition2D())
            );
        }
    }

    //> 0x598B6E - Sort by distance to camera
    rng::sort(
        intFxInfos,
        {},
        [](const InteriorEffectInfo_t& intFxInfo) { return intFxInfo.DistSq; }
    );

    //> 0x598D4E - Cull excess effects
    size_t n{};
    for (auto& ifxi : intFxInfos) {
        if (ifxi.NumFx + n > maxVisibleFxCount || ifxi.DistSq > sq(maxDist)) {
            ifxi.IsCulled = true;
        } else {
            n += ifxi.NumFx;
        }
    }
}

// 0x598430
void InteriorManager_c::AddSameGroupEffectInfos(InteriorEffectInfo_t* ifxi, int32 a2) {
    if (ifxi->NumFx + 1 >= ifxi->Effects.size()) { // NOTSA
        return;
    }

    const auto mi = ifxi->Entity->GetModelInfo();
    for (size_t i = 0; i < mi->m_n2dfxCount; i++) {
        if (i == ifxi->FxIds[0]) { // Already stored
            continue;
        }

        const auto fx = notsa::dyn_cast_if_present<C2dEffectInterior>(mi->Get2dEffect(i));
        if (!fx || ifxi->Effects[0]->m_groupId != fx->m_groupId) {
            continue;
        }

        const auto j     = ifxi->NumFx++;
        ifxi->Effects[j] = fx;
        ifxi->FxIds[j]   = i;

        if (ifxi->NumFx >= ifxi->Effects.size()) { // Moved down here
            return;
        }
    }
}

// 0x5980F0
bool InteriorManager_c::AreAnimsLoaded(int32 animBlock) {
    animBlock = [&] {
        switch (animBlock) {
        case 0:  return CAnimManager::GetAnimationBlockIndex("int_house");
        case 1:  return CAnimManager::GetAnimationBlockIndex("int_shop");
        case 2:  return CAnimManager::GetAnimationBlockIndex("int_office");
        default: return animBlock;
        }
    }();
    return CAnimManager::GetAnimationBlock((AssocGroupId)animBlock)->IsLoaded;
}

// 0x598010
void InteriorManager_c::Exit() {
    for (auto& g : m_InteriorGroupList) {
        g.Exit();
    }
    m_InteriorGroupList.RemoveAll();
    m_InteriorPool.RemoveAll();
    m_InteriorGroupPool.RemoveAll();
    g_furnitureMan.Exit();
}

// 0x598D80
size_t InteriorManager_c::GetVisibleEffects(InteriorEffectInfo_t* intFxInfos, uint32 maxNumIntFxInfo) {
    if (!maxNumIntFxInfo) {
        return 0;
    }

    int16 objCount{};
    std::array<CEntity*, 2048> entitiesInRange; // `CBuilding*`
    CWorld::FindObjectsInRange(TheCamera.GetPosition(), 100.f, false, &objCount, (int16)entitiesInRange.size(), entitiesInRange.data(), true, false, false, false, false);

    size_t numIntFxInfo{};
    for (auto& e : entitiesInRange | rng::views::take(objCount)) {
        if (!e->m_pRwObject || !e->IsInCurrentAreaOrBarberShopInterior()) {
            continue;
        }

        const auto mi = e->GetModelInfo();
        for (size_t i = 0; i < mi->m_n2dfxCount; i++) {
            const auto ifx = notsa::dyn_cast_if_present<C2dEffectInterior>(mi->Get2dEffect((int32)i));

            // We only care about visible interior effects
            if (!ifx || !IsInteriorEffectVisible(ifx, e)) {
                continue;
            }

            // Check if effect was already stored
            if (numIntFxInfo != 0) {
                if (rng::none_of(intFxInfos, intFxInfos + numIntFxInfo, [&](const InteriorEffectInfo_t& ifxi) {
                    return ifxi.Entity == e && ifxi.Effects[0]->m_groupId == ifx->m_groupId;
                })) {
                    continue;
                }
            }

            // Store the effect
            intFxInfos[numIntFxInfo++] = {
                .Entity   = e,
                .NumFx    = 1,
                .Effects  = { ifx },
                .FxIds    = { i },
                .IsCulled = false,
            };

            if (numIntFxInfo >= maxNumIntFxInfo) {
                goto end; // bugfix: Stop looping if can't store more
            }
        }
    }
end:

    for (size_t i = numIntFxInfo; i --> 0;) {
        AddSameGroupEffectInfos(&intFxInfos[i], maxNumIntFxInfo);
    }

    return numIntFxInfo;
}

// 0x598690
bool InteriorManager_c::IsInteriorEffectVisible(C2dEffectInterior* effect, CEntity* entity) {
    const auto CheckPoint = [
        ebb = &entity->GetColModel()->GetBoundingBox(),
        mat = entity->GetMatrix()
    ](CVector pt) {
        return ebb->IsPointInside(mat.InverseTransformPoint(pt));
    };
    return CheckPoint(TheCamera.GetPosition())
        || CheckPoint(FindPlayerPed(0)->GetPosition())
        || FindPlayerPed(1) && CheckPoint(FindPlayerPed(1)->GetPosition());

    /**
     * Original horrifically complicated code:
    \**
    const auto& ebb = entity->GetColModel()->GetBoundingBox();
    const auto& ebbMin = ebb.m_vecMin,
                ebbMax = ebb.m_vecMax;
    CBoundingBox bb{ CVector{FLT_MAX}, CVector{FLT_MIN} };
    const auto StretchTo = [&bb, m = entity->GetMatrix()](CVector pt) {
        bb.StretchToPoint(m.TransformPoint(pt));
    };
    StretchTo({ebbMin.x, ebbMin.y, ebbMin.z});
    StretchTo({ebbMin.x, ebbMax.y, ebbMin.z});
    StretchTo({ebbMax.x, ebbMax.y, ebbMin.z});
    StretchTo({ebbMax.x, ebbMin.y, ebbMin.z});
    StretchTo({ebbMin.x, ebbMin.y, ebbMax.z});
    StretchTo({ebbMin.x, ebbMax.y, ebbMax.z});
    StretchTo({ebbMax.x, ebbMax.y, ebbMax.z});
    StretchTo({ebbMax.x, ebbMin.y, ebbMax.z});
    return bb.IsPointWithin(TheCamera.GetPosition())
        || bb.IsPointInside(FindPlayerPed(0)->GetPosition())
        || bb.IsPointInside(FindPlayerPed(1)->GetPosition());
    */
}

// 0x598620
Interior_c* InteriorManager_c::GetPedsInterior(const CPed* ped) {
    return GetVectorsInterior(ped->GetPosition());
}

// 0x5984B0
void InteriorManager_c::ReturnInteriorToPool(Interior_c* interior) {
    m_InteriorPool.AddItem(interior);
}

// 0x5984A0
Interior_c* InteriorManager_c::GetInteriorFromPool() {
    return m_InteriorPool.RemoveHead();
}

// 0x5983D0
Interior_c* InteriorManager_c::GetVectorsInterior(const CVector& pt) { // TODO: Name is shit, should be `GetInteriorOfPoint` or something similar
    for (auto& g : m_InteriorGroupList) {
        for (auto& i : g.GetInteriors()) {
            if (i && i->IsPtInside(pt)) {
                return i;
            }
        }
    }
    return nullptr;
}

// 0x598390
void InteriorManager_c::SetStealableObjectStolen(CEntity* entity, uint8 isStolen) {
    if (const auto idx = FindStealableObjectId(entity); idx != -1) {
        m_Objects[idx].wasStolen = isStolen;
    }
}

// 0x598360
int32 InteriorManager_c::FindStealableObjectId(CEntity* entity) const {
    for (auto&& [i, v] : rngv::enumerate(GetObjects())) {
        if (v.entity == entity) {
            return i;
        }
    }
    return -1;
}

// 0x5982F0
int32 InteriorManager_c::FindStealableObjectId(int32 interiorId, int32 modelId, CVector point) const {
    for (auto&& [i, v] : rngv::enumerate(GetObjects())) {
        if (v.interiorId == interiorId && v.modelId == modelId && v.pos == point) {
            return i;
        }
    }
    return -1;
}

// 0x5982B0
bool InteriorManager_c::HasInteriorHadStealDataSetup(Interior_c* interior) const {
    return m_InteriorCount && notsa::contains(GetInteriorIds(), interior->m_ID);
}

// notsa
void InteriorManager_c::SetupInteriorStealData(Interior_c* interior) {
    if (!HasInteriorHadStealDataSetup(interior)) {
        if (m_InteriorCount < std::size(m_InteriorIds)) {
            m_InteriorIds[m_InteriorCount++] = interior->m_ID;
        } else {
            NOTSA_LOG_WARN("Can't set up steal data for interior: out of memory");
        }
    }
}

// 0x598280
int8 InteriorManager_c::IsGroupActive(int32 groupType) const {
    return rng::any_of(m_InteriorGroupList, [&](const InteriorGroup_c& g) {
        return g.m_groupType == groupType;
    });
}

// 0x598240
InteriorGroup_c* InteriorManager_c::GetPedsInteriorGroup(const CPed* ped) {
    for (auto& grp : m_InteriorGroupList) {
        if (notsa::contains(grp.GetPeds(), ped)) {
            return &grp;
        }
    }
    return nullptr;
}

// 0x598180
void InteriorManager_c::SetEntryExitPtr(CEntryExit* enex) {
    if (enex->GetLinkedOrThis()->m_nArea == eAreaCodes::AREA_CODE_NORMAL_WORLD) {
        return;
    }
    if (enex->m_recEntrance == m_EnExRect) {
        return;
    }

    m_ObjectCount   = 0;
    m_InteriorCount = 0;

    rng::fill(m_InteriorPedsAliveState, true);

    m_EnEx     = enex;
    m_EnExRect = enex->m_recEntrance;
}

// 0x598090
bool InteriorManager_c::GetBoundingBox(FurnitureEntity_c* entity, CVector(&corners)[4]) {
    for (auto& grp : m_InteriorGroupList) {
        for (const auto i : grp.GetInteriors()) {
            if (i) {
                return i->GetBoundingBox(entity, corners);
            }
        }
    }
    return false;
}

// 0x598080
void InteriorManager_c::ActivatePeds(bool enable) {
    m_ArePedsEnabled = enable;
}
