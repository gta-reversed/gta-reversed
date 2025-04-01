#include "StdInc.h"

#include "EntryExitManager.h"

void CEntryExitManager::InjectHooks() {
    RH_ScopedClass(CEntryExitManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x43F880);
    RH_ScopedInstall(Shutdown, 0x440B90);
    RH_ScopedInstall(AddEntryExitToStack, 0x43E410);
    RH_ScopedInstall(Update, 0x440D10);
    RH_ScopedInstall(ShutdownForRestart, 0x440C40);
    RH_ScopedInstall(DeleteOne, 0x43FD50);
    RH_ScopedInstall(AddOne, 0x43FA00);
    RH_ScopedInstall(FindNearestDoor, 0x43F630);
    RH_ScopedInstall(FindNearestEntryExit, 0x43F4B0);
    RH_ScopedInstall(EnableBurglaryHouses, 0x43F180);
    RH_ScopedInstall(GetPositionRelativeToOutsideWorld, 0x43F150, {.locked = true});
    RH_ScopedInstall(PostEntryExitsCreation, 0x43F0A0);
    RH_ScopedInstall(LinkEntryExit, 0x43F050);
    RH_ScopedInstall(GetEntryExitIndex, 0x43EFD0);
    RH_ScopedInstall(ResetAreaCodeForVisibleObjects, 0x43ED80);
    RH_ScopedInstall(SetAreaCodeForVisibleObjects, 0x43ECF0);
    RH_ScopedInstall(Load, 0x5D55C0);
    RH_ScopedInstall(Save, 0x5D5970);
}

// 0x43F880
void CEntryExitManager::Init() {
    ZoneScoped;

    mp_QuadTree = new CQuadTreeNode(WORLD_BOUNDS, 4);

    ms_exitEnterState = 0;
    ms_bDisabled = false;
    ms_entryExitStackPosn = 0;
    ms_bBurglaryHousesEnabled = false;

    mp_poolEntryExits = new CEntryExitsPool(400u, "Entry exits");
    mp_poolEntryExits->SetDealWithNoMemory(true);
}

// 0x440B90
void CEntryExitManager::Shutdown() {
    for (auto& enex : mp_poolEntryExits->GetAllValid()) {
        mp_QuadTree->DeleteItem(&enex);
    }

    delete mp_poolEntryExits; // Flush() called in destructor
    mp_poolEntryExits = nullptr;

    delete mp_QuadTree;
    mp_QuadTree = nullptr;

    ms_entryExitStackPosn = 0;
}

// 0x440C40
void CEntryExitManager::ShutdownForRestart() {
    for (auto&& [i, enex] : mp_poolEntryExits->GetAllValidWithIndex()) {
        enex.bEnableAccess = true;
        if (enex.bDeleteEnex) {
            DeleteOne(i);
        }
    }

    EnableBurglaryHouses(false);
    if (mp_Active) {
        ResetAreaCodeForVisibleObjects();
        ms_exitEnterState = 0;
        mp_Active = nullptr;
    }
    ms_entryExitStackPosn = 0;
    ms_bDisabled = false;

}

// 0x440D10
void CEntryExitManager::Update() {
    ZoneScoped;

    const bool bDontShowMarkers =
         CCutsceneMgr::ms_cutsceneProcessing
      || CPad::GetPad()->DisablePlayerControls
      || CGameLogic::IsCoopGameGoingOn()
      || CReplay::Mode == eReplayMode::MODE_PLAYBACK
      || CEntryExitManager::ms_bDisabled;

    if (!bDontShowMarkers && ms_exitEnterState == 0) { // Moved `bDontShowMarkers` here from inner loop
        CPtrListSingleLink matches{};

        auto x = TheCamera.m_mCameraMatrix.GetForward().x * 30.0f + TheCamera.GetPosition().x;
        auto y = TheCamera.m_mCameraMatrix.GetForward().y * 30.0f + TheCamera.GetPosition().y;
        CRect rect(x - 30.0f, y - 30.0f, x + 30.0f, y + 30.0f);
        mp_QuadTree->GetAllMatching(rect, matches);

        for (CPtrNode* it = matches.m_node, *next{}; it; it = next) {
            next = it->GetNext();

            auto* enex = it->ItemAs<CEntryExit>();
            if (enex->bEnableAccess) {
                if (   enex->m_pLink && enex->m_nArea == CGame::currArea  // Has link and link is in current area
                    || !enex->m_pLink && enex->m_nArea != CGame::currArea // Has no link, and is not in current area
                ) {
                    // !bDontShowMarkers check moved before the loop, as doing all this would be useless if bDontShowMarkers was false
                    if (const auto enexCenter = enex->GetPosition();  TheCamera.IsSphereVisible(enexCenter, 1.f)) {
                        if (!CClock::GetIsTimeInRange(enex->m_nTimeOn, enex->m_nTimeOff)) {
                            continue;
                        }

                        if ((enexCenter - TheCamera.GetPosition()).SquaredMagnitude() >= 40.f * 40.f) {
                            continue;
                        }

                        CVector markerPos{ enexCenter.x, enexCenter.y, enexCenter.z + 1.f };
                        C3dMarkers::PlaceMarkerCone((uint32)enex, markerPos, 2.0, 255, 255, 0, 255, 1024, 1.0, 5, 0);
                    }
                }
            }
        }
    }

    const auto& player = FindPlayerPed();
    if (mp_Active) {
        if (mp_Active->TransitionFinished(player)) {
            CEntryExitManager::mp_Active = nullptr;
        }
    } else {
        const auto& playerPos = FindPlayerEntity()->GetPosition();
        const auto pos = CVector2D{ playerPos.x, playerPos.y }; // todo: refactor
        CPtrListSingleLink matches{};
        mp_QuadTree->GetAllMatching(pos, matches);

        bool wasAnyMarkerInArea{};
        for (CPtrNode* it = matches.m_node, *next{}; it; it = next) {
            next = it->GetNext();

            auto* enex = it->ItemAs<CEntryExit>();
            if (enex->bEnableAccess && enex->IsInArea(playerPos)) {
                wasAnyMarkerInArea = true;
                if (!bDontShowMarkers && enex->TransitionStarted(player)) {
                    mp_Active = enex;
                    return;
                }
            }
        }

        if (!wasAnyMarkerInArea) {
            ms_exitEnterState = ms_exitEnterState != 3 ? 0 : 4; // TODO: Missing enum
        }

        // matches.Flush(); - Called in it's destructor
    }
}

// 0x43E410
void CEntryExitManager::AddEntryExitToStack(CEntryExit* enex) {
    enex = enex->GetLinkedOrThis();
    
    if (ms_entryExitStackPosn > 0 && ms_entryExitStack[ms_entryExitStackPosn - 1] == enex) {
        ms_entryExitStackPosn--;
    } else if (enex->m_nArea != AREA_CODE_NORMAL_WORLD) {
        ms_entryExitStack[ms_entryExitStackPosn++] = enex;
    } else {
        ms_entryExitStackPosn = 0;
    }
}

/*!
* @address 0x43FA00
* @brief Add a new entry exit
*
* @param timeOn If flag `BURGLARY_ACCESS` is not set it's going to define the value of enex->timeOn
* @param timeOff Unused
* @param name Name of the entry exit, may be nullptr. If flag `CREATE_LINKED_PAIR is set it is also used to identify the linked pair. (In this case it can't be nullptr)
*
* @returns The pool index of the created entry exit, 0 if failed.
*/
int32 CEntryExitManager::AddOne(
    float centerX, float centerY, float centerZ,
    float entranceAngle,
    float entranceRangeX, float entranceRangeY, float entranceRangeZ /*unused*/,
    float exitX, float exitY, float exitZ,
    float exitAngle,
    int32 area,
    CEntryExit::eFlags flags,
    int32 skyColor,
    int32 timeOn, int32 timeOff,
    int32 numberOfPeds,
    const char* name
) {
    const auto ptr = mp_poolEntryExits->New();
    if (!ptr) {
#ifdef FIX_BUGS
        return -1;
#else
        return 0; // Returns an actually valid index, not good
#endif
    }

    const auto enex = new (ptr) CEntryExit{
        { centerX, centerY, centerZ },
        { entranceRangeX, entranceRangeY, entranceRangeZ },
        entranceAngle,
        { exitX, exitY, exitZ },
        exitAngle,
        area,
        flags,
        skyColor,
        timeOn, timeOff,
        numberOfPeds,
        name
    };

    AddEnExToWorld(enex);
    LinkEntryExit(enex);

    return mp_poolEntryExits->GetIndex(enex);
}

// 0x43FD50
void CEntryExitManager::DeleteOne(int32 index) {
    if (const auto enex = mp_poolEntryExits->GetAt(index)) {
        mp_QuadTree->DeleteItem(enex);
#ifdef FIX_BUGS // Call destructor
        std::destroy_at(enex);
#endif
        mp_poolEntryExits->Delete(enex);
    }
}

// 0x43F630
CObject* CEntryExitManager::FindNearestDoor(CEntryExit const& exit, float radius) {
    const auto entranceCenter = exit.GetPosition();

    CEntity* objsInRange[100]{};
    int16 numObjsInRange{};
    CWorld::FindObjectsInRange(entranceCenter, radius, false, &numObjsInRange, (int16)std::size(objsInRange), objsInRange, false, false, false, true, false);

    if (!numObjsInRange) {
        return nullptr;
    }
    
    float closestDistSq{ FLT_MAX };
    CObject* closest{};
    for (auto&& entity : std::span{ objsInRange, (size_t)numObjsInRange }) {
        const auto obj = entity->AsObject();
        if (!obj->physicalFlags.bDisableMoveForce) {
            continue;
        }

        const auto distSq = (obj->GetPosition() - entranceCenter).SquaredMagnitude(); // Using `SqMag` instead of `Mag`
        if (distSq < closestDistSq) {
            closest       = obj;
            closestDistSq = distSq;
        }
    }
    return closest;
}

// 0x43F4B0
int32 CEntryExitManager::FindNearestEntryExit(const CVector2D& position, float range, int32 ignoreArea) {
    CPtrListSingleLink enexInRange{};
    mp_QuadTree->GetAllMatching(CRect{ position, range }, enexInRange);

    float closestDist2D{ 2.f * range };
    CEntryExit* closest{};
    for (CPtrNode* it = enexInRange.m_node, *next{}; it; it = next) {
        next = it->GetNext();

        auto* enex = it->ItemAs<CEntryExit>();
        if (enex->GetLinkedOrThis()->GetArea() == ignoreArea) {
            continue;
        }

        const auto dist = (enex->GetPosition2D() - position).Magnitude(); // TODO: Use SqMag
        if (dist < closestDist2D) {
            closest = enex;
            closestDist2D = dist;
        }
    }

    return closest
        ? mp_poolEntryExits->GetIndex(closest)
        : -1;
}

// 0x43F180
void CEntryExitManager::EnableBurglaryHouses(bool enable) {
    ms_bBurglaryHousesEnabled = enable;
    for (auto& enex : mp_poolEntryExits->GetAllValid()) {
        enex.bBurglaryAccess = enable;
    }
}

// 0x43F150
void CEntryExitManager::GetPositionRelativeToOutsideWorld(CVector& pos) {
    for (auto i = ms_entryExitStackPosn; i; i--) {
        ms_entryExitStack[i - 1]->GetPositionRelativeToOutsideWorld(pos);
    }
}

// 0x43F0A0
void CEntryExitManager::PostEntryExitsCreation() {
    for (auto& enex : mp_poolEntryExits->GetAllValid()) {
        if (enex.bCreateLinkedPair && !enex.m_pLink) {
            LinkEntryExit(&enex);
        }
    }
}

// 0x43F050
void CEntryExitManager::LinkEntryExit(CEntryExit* enex) {
    if (!enex->bCreateLinkedPair) {
        return;
    }

    const auto linkedEnExIdx{ GetEntryExitIndex(enex->m_szName, 0, CEntryExit::eFlags::CREATE_LINKED_PAIR) };
    if (linkedEnExIdx == -1) {
        return;
    }

    if (const auto linkedEnEx{ mp_poolEntryExits->GetAt(linkedEnExIdx) }) {
        enex->m_pLink = linkedEnEx;
        if (linkedEnEx->m_pLink) {
            linkedEnEx->m_pLink = enex;
        }
        // ?????
        linkedEnEx->m_nTimeOn = 0;
        linkedEnEx->m_nTimeOff = 24;
    }
}

// 0x43EFD0
int32 CEntryExitManager::GetEntryExitIndex(const char* name, uint16 enabledFlags, uint16 disabledFlags) {
    for (auto&& [i, enex] : mp_poolEntryExits->GetAllValidWithIndex()) {
        // Remember: cast to `uint8` == mask by 0xFF
        if ((uint8)(enex.m_nFlags & enabledFlags) == (uint8)enabledFlags
            && (uint8)(enex.m_nFlags & disabledFlags) == 0) {
            if (!_strnicmp(enex.m_szName, name, std::size(enex.m_szName))) {
                return i;
            }
        }
    }
    return -1;
}

// 0x43ED80
void CEntryExitManager::ResetAreaCodeForVisibleObjects() {
    while (ms_numVisibleEntities) {
        ms_visibleEntityList[--ms_numVisibleEntities]->m_nAreaCode = (eAreaCodes)ms_oldAreaCode;
    }
}

// 0x43ECF0
void CEntryExitManager::SetAreaCodeForVisibleObjects() {
    ms_numVisibleEntities = 0;

    auto objsInFrustum = (CEntity**)&PC_Scratch[256];
    const auto numObjsInFrustum = CRenderer::GetObjectsInFrustum(objsInFrustum, 6.f, nullptr);

    const auto playerPed{ FindPlayerPed() };

    for (auto&& entity : std::span{ objsInFrustum, (size_t)numObjsInFrustum }) {
        if (entity->m_nAreaCode == CGame::currArea && entity != playerPed) {
            ms_visibleEntityList[ms_numVisibleEntities++] = entity;
            entity->m_nAreaCode = AREA_CODE_13;

            if (ms_numVisibleEntities >= 32) {
                break;
            }
        }
    }

    ms_oldAreaCode = CGame::currArea;
}

// NOTSA (Code somewhat based on 0x43FC00)
void CEntryExitManager::AddEnExToWorld(CEntryExit* enex) {
    // Calculate rotated corner positions (We ain't gonna use a matrix for this like they did, too complicated)
    const auto& r  = enex->m_recEntrance;
    const auto  rc = r.GetCenter();
    CVector2D corners[]{
        { r.right, r.top },
        { r.left, r.bottom }
    };

    // Rotate it around the center
    for (auto& corner : corners) {
        corner = rc + (corner - rc).RotatedBy(enex->m_fEntranceAngleRad); // NOTE: If doesn't work properly, negate (-angle) the angle
    }

    const auto GetMinMaxAxis = [&](size_t axis) {
        return rng::minmax(
            corners | rng::views::transform([axis](auto&& c) { return c[axis]; })
        );
    };

    // Calculate min-max coordinates
    const auto [minX, maxX] = GetMinMaxAxis(0);
    const auto [minY, maxY] = GetMinMaxAxis(1);
    
    // Add it to the QuadTree using the calculated bounding rect
    mp_QuadTree->AddItem(enex, {minX, minY, maxX, maxY});
}

bool CEntryExitManager::WeAreInInteriorTransition() {
    return ms_exitEnterState != 0;
}

// 0x5D55C0
bool CEntryExitManager::Load() {
    // Load entry exit stack
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_entryExitStackPosn);
    for (auto i = 0u; i < ms_entryExitStackPosn; i++) {
        uint16 enexIdx{};
        CGenericGameStorage::LoadDataFromWorkBuffer(enexIdx);
        ms_entryExitStack[i] = mp_poolEntryExits->GetAt(enexIdx);
    }

    // Load entry exits
    auto enexIdx = CGenericGameStorage::LoadDataFromWorkBuffer<int16>();
    while (enexIdx != -1) {
        const auto flags     = CGenericGameStorage::LoadDataFromWorkBuffer<uint16>();
        const auto linkedIdx = CGenericGameStorage::LoadDataFromWorkBuffer<int16>();

        if (auto enex = mp_poolEntryExits->GetAt(enexIdx)) {
            enex->m_nFlags = flags;
            if (linkedIdx == -1) {
                enex->m_pLink = nullptr;
            } else if (const auto linked = mp_poolEntryExits->GetAt(linkedIdx)) {
                enex->m_pLink = linked;
            } else {
                enex->m_pLink = nullptr;
            }
        } else {
            NOTSA_UNREACHABLE(); // NOTSA - Probably corrupted save file or something.
        }

        CGenericGameStorage::LoadDataFromWorkBuffer(enexIdx);
    }

    return true;
}

// 0x5D5970
bool CEntryExitManager::Save() {
    // Save entry exit stack
    CGenericGameStorage::SaveDataToWorkBuffer(ms_entryExitStackPosn);
    for (auto&& enex : std::span{ ms_entryExitStack, ms_entryExitStackPosn}) {
        CGenericGameStorage::SaveDataToWorkBuffer((uint16)mp_poolEntryExits->GetIndex(enex));
    }

    // Save entry exits
    for (auto&& [i, enex] : mp_poolEntryExits->GetAllValidWithIndex()) {
        int16 data = -1;
        if (enex.m_pLink) {
            // Make sure the link reference is valid
            auto linkIndex = mp_poolEntryExits->GetIndex(enex.m_pLink);
            if (mp_poolEntryExits->IsIndexInBounds(linkIndex)) {
                data = linkIndex;
            }
        }
        CGenericGameStorage::SaveDataToWorkBuffer(i); // Enex idx in pool
        CGenericGameStorage::SaveDataToWorkBuffer(enex.m_nFlags);
        CGenericGameStorage::SaveDataToWorkBuffer(data); // Linked enex idx in pool
    }

    // Mark the end of ENEX table
    CGenericGameStorage::SaveDataToWorkBuffer((int16)-1);
    return true;
}
