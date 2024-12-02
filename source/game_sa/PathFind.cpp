/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"
#include <extensions/enumerate.hpp>
#include "PathFind.h"

// TODO: Move into the class itself
CVector& s_pathsNeededPosn = *(CVector*)0x977B70;
bool&    s_bLoadPathsNeeded  = *(bool*)0x96F030;

// TODO: Remove this and use a stack array or smth..
std::array<bool, NUM_PATH_MAP_AREAS>& ToBeStreamed = *(std::array<bool, NUM_PATH_MAP_AREAS>*)0x96EFD0;

std::array<float, 64>& XCoorGiven = *(std::array<float, 64>*)0x96EE80;
std::array<float, 64>& YCoorGiven = *(std::array<float, 64>*)0x96ED80;
std::array<float, 64>& ZCoorGiven = *(std::array<float, 64>*)0x96EC80;
std::array<std::array<int8, 8>, 64>& ConnectsToGiven = *(std::array<std::array<int8, 8>, 64>*)0x96EAC0;

std::array<int32, NUM_PATH_INTERIOR_AREAS>& aInteriorNodeLinkedToExterior = *(std::array<int32, NUM_PATH_INTERIOR_AREAS>*)0x96EA98;
std::array<CNodeAddress, NUM_PATH_INTERIOR_AREAS>& aExteriorNodeLinkedTo = *(std::array<CNodeAddress, NUM_PATH_INTERIOR_AREAS>*)0x977B7C;

std::array<CNodeAddress, 5000>& aNodesToBeCleared = *(std::array<CNodeAddress, 5000>*)0x972CD0;

void CPathFind::InjectHooks() {
    RH_ScopedClass(CPathFind);
    RH_ScopedCategoryGlobal();

    // Hooks commented out because most of the functions have no definitions
    // thus it's not possible to take their address
    // And I'm lazy to add stubs

    RH_ScopedInstall(AddNodeToNewInterior, 0x450E90);
    RH_ScopedInstall(FindNearestExteriorNodeToInteriorNode, 0x450F30);
    RH_ScopedInstall(ThisNodeHasToBeSwitchedOff, 0x44D3E0);
    RH_ScopedInstall(These2NodesAreAdjacent, 0x44D230);
    RH_ScopedInstall(FindRegionForCoors, 0x44D830);
    RH_ScopedInstall(FindYRegionForCoors, 0x44D8C0);
    RH_ScopedInstall(FindXRegionForCoors, 0x44D890);
    RH_ScopedInstall(AddInteriorLinkToExternalNode, 0x44DF30);
    RH_ScopedInstall(AddInteriorLink, 0x44DED0);
    RH_ScopedInstall(MarkRegionsForCoors, 0x44DB60);
    RH_ScopedOverloadedInstall(FindStartPointOfRegion, "", 0x44D930, void(CPathFind::*)(size_t, size_t, float&, float&));
    RH_ScopedInstall(FindYCoorsForRegion, 0x44D910);
    RH_ScopedInstall(FindXCoorsForRegion, 0x44D8F0);
    RH_ScopedInstall(IsAreaNodesAvailable, 0x420AA0);
    RH_ScopedInstall(HaveRequestedNodesBeenLoaded, 0x450DB0);
    RH_ScopedInstall(MakeRequestForNodesToBeLoaded, 0x450D70);
    RH_ScopedInstall(UpdateStreaming, 0x450A60);
    RH_ScopedInstall(TakeWidthIntoAccountForWandering, 0x4509A0);
    RH_ScopedInstall(Shutdown, 0x450950);
    RH_ScopedOverloadedInstall(FindNodeCoorsForScript, "TwoNodes", 0x450780, CVector(CPathFind::*)(CNodeAddress, CNodeAddress, float&, bool*));
    RH_ScopedOverloadedInstall(FindNodeCoorsForScript, "LinkedNode", 0x4505E0, CVector(CPathFind::*)(CNodeAddress, bool*));
    RH_ScopedInstall(IsWaterNodeNearby, 0x450DE0);
    RH_ScopedInstall(CountNeighboursToBeSwitchedOff, 0x4504F0);
    //RH_ScopedInstall(FindNodeOrientationForCarPlacement, 0x450320);
    //RH_ScopedInstall(FindNodePairClosestToCoors, 0x44FEE0);
    RH_ScopedInstall(FindNodeClosestToCoorsFavourDirection, 0x44FCE0);
    //RH_ScopedInstall(FindNodeClosestToCoors, 0x44FA30);
    RH_ScopedInstall(MarkRoadNodeAsDontWander, 0x450560);
    //RH_ScopedInstall(AddDynamicLinkBetween2Nodes, 0x4512D0);
    RH_ScopedOverloadedInstall(LoadPathFindData, "Area", 0x452F40, void(CPathFind::*)(int32));
    RH_ScopedOverloadedInstall(LoadPathFindData, "FromStream", 0x4529F0, void(CPathFind::*)(RwStream*, int32));
    //RH_ScopedInstall(SwitchPedRoadsOffInArea, 0x452F00);
    //RH_ScopedInstall(SwitchRoadsOffInArea, 0x452C80);
    //RH_ScopedInstall(SwitchRoadsOffInAreaForOneRegion, 0x452820);
    RH_ScopedInstall(ComputeRoute, 0x452760, { .reversed = false });
    //RH_ScopedInstall(CompleteNewInterior, 0x452270);
    RH_ScopedInstall(SwitchOffNodeAndNeighbours, 0x452160);
    //RH_ScopedInstall(Find2NodesForCarCreation, 0x452090);
    //RH_ScopedInstall(TestCoorsCloseness, 0x452000);
    //RH_ScopedInstall(FindNextNodeWandering, 0x451B70);
    RH_ScopedInstall(DoPathSearch, 0x4515D0, {.reversed = false}); // Sometimes breaks `CTaskComplexFollowNodeRoute::ComputePathNodes` - To repro just walk around in groove st. 
    //RH_ScopedInstall(FindParkingNodeInArea, 0x4513F0);
    RH_ScopedInstall(FindLinkBetweenNodes, 0x451350);
    RH_ScopedInstall(ReturnInteriorNodeIndex, 0x451300);
    //RH_ScopedInstall(FindNthNodeClosestToCoors, 0x44F8C0);
    //RH_ScopedInstall(FindNodeClosestInRegion, 0x44F2C0);
    //RH_ScopedInstall(CalcDistToAnyConnectingLinks, 0x44F190);
    //RH_ScopedInstall(CalcRoadDensity, 0x44EFC0);
    RH_ScopedInstall(TestForPedTrafficLight, 0x44D480);
    RH_ScopedInstall(UnMarkAllRoadNodesAsDontWander, 0x44D400);
    RH_ScopedInstall(TidyUpNodeSwitchesAfterMission, 0x44D3B0);
    RH_ScopedInstall(ThisNodeWillLeadIntoADeadEnd, 0x44D310);
    RH_ScopedInstall(AddNodeToList, 0x44D1E0);
    RH_ScopedInstall(RemoveNodeFromList, 0x44D1B0);
    RH_ScopedInstall(UnLoadPathFindData, 0x44D0F0);
    RH_ScopedInstall(Init, 0x44D080);
    RH_ScopedInstall(GetPathNode, 0x420AC0);
    RH_ScopedInstall(TestCrossesRoad, 0x44D790);
    RH_ScopedInstall(ReInit, 0x44E4E0);
    RH_ScopedInstall(RemoveInterior, 0x44E1A0);
    RH_ScopedInstall(AddDynamicLinkBetween2Nodes_For1Node, 0x44E000);
    RH_ScopedInstall(StartNewInterior, 0x44DE80);
    RH_ScopedInstall(LoadSceneForPathNodes, 0x44DE00);
    RH_ScopedInstall(AreNodesLoadedForArea, 0x44DD10);
    RH_ScopedInstall(ReleaseRequestedNodes, 0x44DD00);
    RH_ScopedInstall(SetPathsNeededAtPosition, 0x44DCD0);
    RH_ScopedInstall(SetLinksBridgeLights, 0x44D960);
    RH_ScopedInstall(Save, 0x5D34C0);
    RH_ScopedInstall(Load, 0x5D3500);
}

void CPathNode::InjectHooks() {
    RH_ScopedClass(CPathNode);
    RH_ScopedCategoryGlobal();
    RH_ScopedInstall(GetPosition, 0x420A10);
}

// 0x44D080
void CPathFind::Init() {
    ZoneScoped;

    static int32 NumTempExternalNodes = 0; // Unused
    m_nNumForbiddenAreas = 0;
    m_loadAreaRequestPending = false;

    for (auto i = 0u; i < NUM_TOTAL_PATH_NODE_AREAS; ++i) {
        m_pPathNodes[i] = nullptr;
        m_pNaviNodes[i] = nullptr;
        m_pNodeLinks[i] = nullptr;
        m_pLinkLengths[i] = nullptr;
        m_pPathIntersections[i] = nullptr;
        m_pNaviLinks[i] = nullptr; // BUG: Out of array bounds write, same as in original code
        m_aUnused[i] = nullptr;    // BUG: Out of array bounds write, same as in original code
    }

    rng::fill(m_interiorIDs, (uint32)-1);
}

// 0x44E4E0
void CPathFind::ReInit() {
    m_nNumForbiddenAreas = 0;
    m_loadAreaRequestPending = false;
}

// 0x44DD00
void CPathFind::MakeRequestForNodesToBeLoaded(float minX, float maxX, float minY, float maxY) {
    m_loadAreaRequestPending = true;
    m_loadAreaRequestMinX = minX;
    m_loadAreaRequestMaxX = maxX;
    m_loadAreaRequestMinY = minY;
    m_loadAreaRequestMaxY = maxY;
    UpdateStreaming(true);
}

// 0x44DD10
bool CPathFind::AreNodesLoadedForArea(float minX, float maxX, float minY, float maxY) {
    return IterAreasTouchingRect({ minX, minY, maxX, maxY }, [this](auto areaId) -> bool { return IsAreaLoaded(areaId); });
}

// 0x450DB0
bool CPathFind::HaveRequestedNodesBeenLoaded() {
    return AreNodesLoadedForArea(
        m_loadAreaRequestMinX,
        m_loadAreaRequestMaxX,
        m_loadAreaRequestMinY,
        m_loadAreaRequestMaxY
    );
}

// 0x450950
void CPathFind::Shutdown() {
    for (auto x = 0u; x < NUM_PATH_MAP_AREA_X; ++x) {
        for (auto y = 0u; y < NUM_PATH_MAP_AREA_Y; ++y) {
            auto relativeId = x + y * NUM_PATH_MAP_AREA_X;
            if (m_pPathNodes[relativeId]) {
                CStreaming::RemoveModel(DATToModelId(relativeId));
            }
        }
    }
}

bool CPathFind::ThisNodeWillLeadIntoADeadEnd(CPathNode* startNode, CPathNode* endNode) {
    auto curr = startNode, prev = endNode;
    while (true) {
        CPathNode* next{}; // If node has no links (or neither links area is loaded) this will be nullptr
        for (auto& linked : GetNodeLinkedNodes(*curr)) {
            if (&linked == prev) { // Obviously don't count the previous node
                continue;
            }
            if (linked.m_nBehaviourType == 4u || linked.m_nBehaviourType > 10u) { // TODO: Enum?
                // I'm unsure what's happening here
                // I think, since this function isn't recursive, they just
                // consider having 2 appropriate links as a non-deadend
                if (next) {
                    return false;
                }
                next = &linked;
            }
        }
        if (!next) { 
            return true;
        }
        prev = curr;
        curr = next;
    }
}

// 0x44D3B0
void CPathFind::TidyUpNodeSwitchesAfterMission() {
    m_nNumForbiddenAreas = std::min(54u, m_nNumForbiddenAreas); // todo: magic number
}

// 0x44D400
void CPathFind::UnMarkAllRoadNodesAsDontWander() {
    for (auto i = 0u; i < NUM_PATH_MAP_AREAS; ++i) {
        if (!m_pPathNodes[i])
            continue;

        for (auto nodeInd = 0u; nodeInd < m_anNumVehicleNodes[i]; ++nodeInd) {
            m_pPathNodes[i][nodeInd].m_bDontWander = false;
        }
    }
}

// 0x44DD00
void CPathFind::ReleaseRequestedNodes() {
    m_loadAreaRequestPending = false;
}

/*!
* @brief Find intersection info between 2 nodes
* @addr notsa 100% inlined
*/
auto CPathFind::FindIntersection(const CNodeAddress& startNodeAddress, const CNodeAddress& targetNodeAddress) -> CPathIntersectionInfo* {
    // Make sure both nodes areas are loaded
    if (!AreNodeAreasLoaded({ targetNodeAddress, startNodeAddress })) {
        return nullptr;
    }

    const auto& startNode = *GetPathNode(startNodeAddress);
    const auto& nodeLinks = m_pNodeLinks[startNodeAddress.m_wAreaId];
    for (auto i = 0u; i < startNode.m_nNumLinks; i++) {
        const auto linkedNodeIdx = startNode.m_wBaseLinkId + i;
        if (nodeLinks[linkedNodeIdx] == targetNodeAddress) {
            return &m_pPathIntersections[startNodeAddress.m_wAreaId][linkedNodeIdx];
        }
    }

    return nullptr;
}

// 0x44D790
bool CPathFind::TestCrossesRoad(CNodeAddress startNodeAddress, CNodeAddress targetNodeAddress) {
    const auto intersect = FindIntersection(startNodeAddress, targetNodeAddress);
    return intersect && intersect->m_bRoadCross;
}

// 0x44ECA0
bool CPathFind::GeneratePedCreationCoors_Interior(float x, float y, CVector* outCoords, CNodeAddress* unused1, CNodeAddress* unused2, float* outOrientation) {
    return plugin::CallMethodAndReturn<bool, 0x44ECA0>(this, x, y, outCoords, unused1, unused2, outOrientation);
}

// 0x44D480
bool CPathFind::TestForPedTrafficLight(CNodeAddress startNodeAddress, CNodeAddress targetNodeAddress) {
    const auto intersect = FindIntersection(startNodeAddress, targetNodeAddress);
    return intersect && intersect->m_bPedTrafficLight;
}

// 0x4509A0
CVector CPathFind::TakeWidthIntoAccountForWandering(CNodeAddress nodeAddress, uint16 randomSeed) {
    // Invalid area, or area not loaded
    if (nodeAddress.IsValid() && IsAreaNodesAvailable(nodeAddress)) {
        const auto nbits = 4u;
        const auto Random = [](uint16 seed) { return (float)(seed % (1 << nbits) - 7); };
        return GetPathNode(nodeAddress)->GetPosition() + CVector{ Random(randomSeed), Random(randomSeed >> nbits), 0.f };
    }

    return {};
}

//  0x44F8C0
CNodeAddress CPathFind::FindNthNodeClosestToCoors(CVector pos, uint8 nodeType, float maxDistance, bool bLowTraffic, bool bUnkn, int32 nthNode, bool bBoatsOnly,
                                                  bool bIgnoreInterior, CNodeAddress* outNode) {
    CNodeAddress outAddress;
    plugin::CallMethod<0x44F8C0, CPathFind*, CNodeAddress*, CVector, uint8, float, bool, bool, int32, bool, bool, CNodeAddress*>(
        this, &outAddress, pos, nodeType, maxDistance, bLowTraffic, bUnkn, nthNode, bBoatsOnly, bIgnoreInterior, outNode);
    return outAddress;
}

// 0x451B70
void CPathFind::FindNextNodeWandering(uint8 nodeType, CVector vecPos, CNodeAddress* originAddress, CNodeAddress* targetAddress, uint8 dir, uint8* outDir) {
    plugin::CallMethod<0x451B70, CPathFind*, uint8, CVector, CNodeAddress*, CNodeAddress*, uint8, uint8*>(this, nodeType, vecPos, originAddress, targetAddress, dir, outDir);
}

// 0x4515D0
void CPathFind::DoPathSearch(
    ePathType pathType,
    CVector originPos,
    CNodeAddress originAddrAddrHint, // If invalid/area not loaded the closest node to `originPos` is used.
    CVector targetPos,
    CNodeAddress* outResultNodes,
    int16& outNodesCount,
    int32 maxNodesToFind,
    float* outDistance,
    float maxSearchDistance,
    CNodeAddress* targetNodeAddrHint, // If null/invalid/area not loaded the closest node to `targetPos` is used.
    float maxSearchDepth,
    bool sameLaneOnly,
    CNodeAddress forbiddenNodeAddr,
    bool bAllowWaterNodeTransitions,
    bool forBoats
) {
    // Moved this up here, as it's set in every return path
    outNodesCount = 0;

    const auto ResolveNode = [&, this](CVector nodePosn, CNodeAddress* addr) {
        if (addr && addr->IsValid()) {
            // In case area is not loaded we still fall-back to
            // finding the closest node, as that will yield the
            // node closest in a loaded area
            if (IsAreaNodesAvailable(*addr)) {
                return GetPathNode(*addr);
            }
        }
        const auto foundAddr = FindNodeClosestToCoors(
            nodePosn,
            pathType,
            maxSearchDistance,
            false,
            false,
            false,
            forBoats,
            false
        );
        return foundAddr.IsValid()
            ? GetPathNode(foundAddr)
            : nullptr;
    };

    // Resolve addresses to use. Dont use `originAddrAddr` or `targetNodeAddr` after this point
    CPathNode *origin, *target{};
    if (   !(origin = ResolveNode(originPos, &originAddrAddrHint))
        || !(target = ResolveNode(targetPos, targetNodeAddrHint))
    ) {
    fail:
        outNodesCount = 0;
        if (outDistance) {
            *outDistance = 100'000.f;
        }
        return;
    }

    // Check if the 2 nodes ended up being the same
    if (*origin == *target) {
        outNodesCount = 0;
        if (outDistance) {
            *outDistance = 0.f;
        }
        return;
    }

    // Check if flood fill values match, if not, fail
    if (origin->m_nFloodFill != target->m_nFloodFill) {
        goto fail;
    }

    rng::fill(m_pathFindHashTable, nullptr);
    m_totalNumNodesInPathFindHashTable = 0u;

    AddNodeToList(target, 0);

    size_t numNodesToBeCleared{};
    const auto AddNodeToBeCleared = [&](const CPathNode& node) {
        if (numNodesToBeCleared < std::size(aNodesToBeCleared)) {
            aNodesToBeCleared[numNodesToBeCleared++] = node.GetAddress();
        }
    };
    AddNodeToBeCleared(*target);

    size_t iterDepth{};
    bool finished{};
    while (true) {
        // Dijkstra's algorithm (probably)

        // Find distances
        for (auto node = m_pathFindHashTable[iterDepth % std::size(m_pathFindHashTable)]; node; node = node->m_next) {
            if (*node == *origin) {
                finished = true;
            }

            for (auto linkNum = 0u; linkNum < node->m_nNumLinks; linkNum++) {
                const auto linkIdx    = node->m_wBaseLinkId + linkNum;
                const auto linkedAddr = m_pNodeLinks[node->m_wAreaId][linkIdx];

                if (!IsAreaNodesAvailable(linkedAddr)) {
                    continue;
                }

                auto& linked = *GetPathNode(linkedAddr);

                // Omitted the bool variable and instead used `continue`s

                if (sameLaneOnly) { // 0x451814
                    const auto& naviLinkAddr = m_pNaviLinks[node->m_wAreaId][linkIdx];
                    if (IsAreaLoaded(naviLinkAddr.m_wAreaId)) {
                        const auto& naviLink = GetCarPathLink(naviLinkAddr);
                        if (naviLink.m_attachedTo == linked.GetAddress()) {
                            if (!naviLink.m_numOppositeDirLanes) {
                                continue;
                            }
                        } else if (!naviLink.m_numSameDirLanes) {
                            continue;
                        }
                    }
                }

                if (forbiddenNodeAddr == linked.GetAddress()) {
                    continue;
                }

                // 0x451885
                if (node->m_bWaterNode != linked.m_bWaterNode && !bAllowWaterNodeTransitions) {
                    continue;
                }

                // 0x4518BD
                const auto distToOriginFromLinked = node->m_totalDistFromOrigin + m_pLinkLengths[node->m_wAreaId][linkIdx];

                // If this new route we found is better than the previous re-insert node into hashtable
                if (distToOriginFromLinked < linked.m_totalDistFromOrigin) {
                    if (linked.m_totalDistFromOrigin != SHRT_MAX - 1) { // Why the fuck they used this instead of `SHRT_MAX`?
                        RemoveNodeFromList(&linked);
                    } else {
                        AddNodeToBeCleared(linked);
                    }
                    AddNodeToList(&linked, distToOriginFromLinked);
                }
            }

            // We've visited this node, so remove it
            RemoveNodeFromList(node);
        }

        // No more nodes? Well, too sad.
        if (!m_totalNumNodesInPathFindHashTable) {
            break;
        }

        // Hit limit?
        if (++iterDepth > maxSearchDepth) {
            break;
        }

        if (numNodesToBeCleared >= std::size(aNodesToBeCleared) - 50) {
            break;
        }

        if (!finished) { // Inverted 0x4519C0 
            continue;
        }

        if (outDistance) {
            *outDistance = origin->m_totalDistFromOrigin;
        }
        if (outResultNodes) { // Weird check really, because below it isn't checked :D
            outResultNodes[outNodesCount++] = origin->GetAddress();
        }

        for (auto node = origin; node == target || outNodesCount < maxNodesToFind; outNodesCount++) {
            for (auto linkNum = 0u; linkNum < node->m_nNumLinks; linkNum++) {
                const auto linkedAddr = m_pNodeLinks[node->m_wAreaId][linkNum];
                const auto linkIdx    = node->m_wBaseLinkId + linkNum;
                if (!IsAreaNodesAvailable(linkedAddr)) {
                    continue;
                }
                const auto linked = GetPathNode(linkedAddr);
                if (const auto dist = node->m_totalDistFromOrigin - m_pLinkLengths[node->m_wAreaId][linkIdx]; dist == linked->m_totalDistFromOrigin) {
                    outResultNodes[outNodesCount++] = linkedAddr;
                    node = linked;
                    break;
                }
            }
        }
        break;
    }
    for (auto& addr : aNodesToBeCleared | rng::views::take(numNodesToBeCleared)) {
        GetPathNode(addr)->m_totalDistFromOrigin = SHRT_MAX - 1;
    }
}

// 0x452760
void CPathFind::ComputeRoute(uint8 nodeType, const CVector& vecStart, const CVector& vecEnd, const CNodeAddress& address, CNodeRoute& nodeRoute) {
    plugin::CallMethod<0x452760>(this, nodeType, &vecStart, &vecEnd, &address, &nodeRoute);
}

// 0x44D960
void CPathFind::SetLinksBridgeLights(float fXMin, float fXMax, float fYMin, float fYMax, bool value) {
    const auto areaRect = CRect{ {fXMin, fYMin}, {fXMax, fYMax} };

    for (auto areaId = 0u; areaId < NUM_PATH_MAP_AREAS; areaId++) {
        if (!IsAreaLoaded(areaId)) {
            continue;
        }

        for (auto n = 0u; n < m_anNumCarPathLinks[areaId]; n++) {
            auto& node = GetCarPathLink({ areaId, n });
            if (areaRect.IsPointInside(node.GetNodeCoors())) {
                node.m_bridgeLights = value;
            }
        }
    }
}

namespace detail {
// NOTSA
CVector GetPosnBetweenNodesForScript(CPathNode* nodeA, CVector2D dir) {
    return nodeA->GetPosition() + CVector{dir.GetPerpLeft() * ((float)nodeA->m_nPathWidth / 16.f + 2.7f)};
}
};

// 0x4505E0
CVector CPathFind::FindNodeCoorsForScript(CNodeAddress address, bool* bFound) {
    const auto SetFound = [&](bool found) {
        if (bFound) {
            *bFound = found;
        }
    };
    if (!address.IsValid() || !IsAreaNodesAvailable(address)) {
        SetFound(false);
        return {};
    } else {
        SetFound(true);

        const auto node = GetPathNode(address);
        const auto nodePos = node->GetPosition();
 
        // If this node has a link return some kind of position between this and the first link
        if (node->m_nPathWidth && node->m_nNumLinks) {
            if (const auto firstLink = m_pNodeLinks[node->m_wBaseLinkId]) {
                if (const auto firstLinkedNode = GetPathNode(*firstLink)) {
                    auto dir = CVector2D{ firstLinkedNode->GetPosition() - nodePos }.Normalized();

                    // By negating here we invert the direction
                    dir = dir.x >= 0 ? dir : -dir;

                    return detail::GetPosnBetweenNodesForScript(node, dir);
                }
            }
        }

        // Otherwise just return this node's position
        return nodePos;
    }
}

// 0x450780
CVector CPathFind::FindNodeCoorsForScript(CNodeAddress nodeAddrA, CNodeAddress nodeAddrB, float& outHeadingDeg, bool* outFound) {
    const auto SetFound = [&](bool found) {
        if (outFound) {
            *outFound = found;
        }
    };
    if (nodeAddrA.IsValid() && nodeAddrB.IsValid() && AreNodeAreasLoaded({ nodeAddrA, nodeAddrB })) { // Inverted
        SetFound(true);

        const auto nodeA = GetPathNode(nodeAddrA);
        const auto posA  = nodeA->GetPosition();
        const auto dir   = CVector2D{ GetPathNode(nodeAddrB)->GetPosition() - posA }.Normalized();

        outHeadingDeg = RWRAD2DEG(dir.Heading());

        return nodeA->m_nPathWidth
            ? detail::GetPosnBetweenNodesForScript(nodeA, dir)
            : posA;
    } else {
        SetFound(false);
        return {};
    }
}

// 0x450560
void CPathFind::MarkRoadNodeAsDontWander(float x, float y, float z) {
    CVector pos = {x, y, z};
    auto node = FindNodeClosestToCoors(pos, PATH_TYPE_VEH, 999999.88f, 0, 0, 0, 0, 0);
    if (node.IsValid()) {
        m_pPathNodes[node.m_wAreaId][node.m_wNodeId].m_bDontWander = true;
    }
}

// 0x452820
void CPathFind::SwitchRoadsOffInAreaForOneRegion(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, bool bLowTraffic, uint8 nodeType, int32 areaId,
                                                 uint8 bUnused) {
    return plugin::CallMethod<0x452820, CPathFind*, float, float, float, float, float, float, bool, char, int32, bool>(this, xMin, xMax, yMin, yMax, zMin, zMax, bLowTraffic,
                                                                                                                       nodeType, areaId, bUnused);
}

// NOTSA
CPathNode* CPathFind::GetPathNode(CNodeAddress address) {
    assert(address.IsValid());
    assert(IsAreaNodesAvailable(address));
    return &m_pPathNodes[address.m_wAreaId][address.m_wNodeId];
}

// NOTSA
bool CPathFind::FindNodeCoorsForScript(CVector& outPos, CNodeAddress addr) {
    bool valid{};
    FindNodeCoorsForScript(addr, &valid);
    return valid;
}

// 0x452F40
void CPathFind::LoadPathFindData(int32 areaId) {
    CTimer::Suspend();
    sprintf_s(gString, "data\\paths\\nodes%d.dat", areaId);
    auto* stream = RwStreamOpen(RwStreamType::rwSTREAMFILENAME, RwStreamAccessType::rwSTREAMREAD, gString);
    LoadPathFindData(stream, areaId);
    CTimer::Resume();
}

// 0x4529F0
void CPathFind::LoadPathFindData(RwStream* stream, int32 areaId) {
    RwStreamRead(stream, &m_anNumNodes[areaId],        sizeof(m_anNumNodes[areaId]));
    RwStreamRead(stream, &m_anNumVehicleNodes[areaId], sizeof(m_anNumVehicleNodes[areaId]));
    RwStreamRead(stream, &m_anNumPedNodes[areaId],     sizeof(m_anNumPedNodes[areaId]));
    RwStreamRead(stream, &m_anNumCarPathLinks[areaId], sizeof(m_anNumCarPathLinks[areaId]));
    RwStreamRead(stream, &m_anNumAddresses[areaId],    sizeof(m_anNumAddresses[areaId]));

    assert(m_anNumNodes[areaId] == m_anNumVehicleNodes[areaId] + m_anNumPedNodes[areaId]);

    auto numNodes = m_anNumNodes[areaId];
    if (numNodes) {
        m_pPathNodes[areaId] = new CPathNode[numNodes];
        RwStreamRead(stream, m_pPathNodes[areaId], sizeof(CPathNode) * numNodes);
    } else {
        m_pPathNodes[areaId] = new CPathNode[1];
    }

    auto numCarPathLinks = m_anNumCarPathLinks[areaId];
    if (numCarPathLinks) {
        m_pNaviNodes[areaId] = new CCarPathLink[numCarPathLinks];
        RwStreamRead(stream, m_pNaviNodes[areaId], sizeof(CCarPathLink) * numCarPathLinks);
    } else {
        m_pNaviNodes[areaId] = nullptr;
    }

    auto numAddresses = m_anNumAddresses[areaId];
    if (numAddresses) {
        auto numToAdd = numAddresses + NUM_DYNAMIC_LINKS_PER_AREA * 12;
        m_pNodeLinks[areaId] = new CNodeAddress[numToAdd];
        m_pNaviLinks[areaId] = new CCarPathLinkAddress[numAddresses];
        m_pLinkLengths[areaId] = new uint8[numToAdd];
        m_pPathIntersections[areaId] = new CPathIntersectionInfo[numToAdd];
        RwStreamRead(stream, m_pNodeLinks[areaId], sizeof(CNodeAddress) * numToAdd);
        RwStreamRead(stream, m_pNaviLinks[areaId], sizeof(CCarPathLinkAddress) * numAddresses);
        RwStreamRead(stream, m_pLinkLengths[areaId], sizeof(uint8) * numToAdd);
        RwStreamRead(stream, m_pPathIntersections[areaId], sizeof(CPathIntersectionInfo) * numToAdd);
    } else {
        m_pNodeLinks[areaId] = nullptr;
        m_pNaviLinks[areaId] = nullptr;
        m_pLinkLengths[areaId] = nullptr;
        m_pPathIntersections[areaId] = nullptr;
    }

    for (auto i = 0u; i < numNodes; ++i) {
        auto& node = m_pPathNodes[areaId][i];
        node.m_isOriginallyOnDeadEnd = node.m_onDeadEnd;
    }

    for (auto i = 0u; i < m_nNumForbiddenAreas; ++i) {
        auto& area = m_aForbiddenAreas[i];
        SwitchRoadsOffInAreaForOneRegion(area.m_fXMin, area.m_fXMax, area.m_fYMin, area.m_fYMax, area.m_fZMin, area.m_fZMax, area.m_bEnable, area.m_nType, areaId, false);
    }
    for (auto i = 0u; i < NUM_DYNAMIC_LINKS_PER_AREA; ++i) {
        rng::fill(m_aDynamicLinksBaseIds[i], -1);
        rng::fill(m_aDynamicLinksIds[i], -1);
    }
}

// 0x44D0F0
void CPathFind::UnLoadPathFindData(int32 index) {
    delete[] m_pPathNodes[index];
    delete[] m_pNaviNodes[index];
    delete[] m_pNodeLinks[index];
    delete[] m_pNaviLinks[index];
    delete[] m_pLinkLengths[index];
    delete[] m_pPathIntersections[index];

    m_pPathNodes[index] = nullptr;
    m_pNaviNodes[index] = nullptr;
    m_pNodeLinks[index] = nullptr;
    m_pNaviLinks[index] = nullptr;
    m_pLinkLengths[index] = nullptr;
    m_pPathIntersections[index] = nullptr;
}

// 0x44DE00
void CPathFind::LoadSceneForPathNodes(CVector point) {
    rng::fill(ToBeStreamed, false);
    MarkRegionsForCoors(point, 350.f);
    for (const auto [areaId, load] : notsa::enumerate(ToBeStreamed)) {
        if (load) {
            CStreaming::RequestModel(DATToModelId((int32)areaId), STREAMING_DEFAULT);
        }
    }
}

// 0x450DE0
bool CPathFind::IsWaterNodeNearby(CVector position, float radius) {
    for (auto areaId = 0u; areaId < NUM_PATH_MAP_AREAS; areaId++) {
        for (const auto& node : GetPathNodesInArea(areaId, PATH_TYPE_VEH)) {
            if (node.m_bWaterNode) {
                if ((node.GetPosition() - position).SquaredMagnitude() <= sq(radius)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// 0x44F460
CNodeAddress CPathFind::FindNodeClosestToCoors(
    CVector pos,
    ePathType nodeType,
    float maxDistance,
    uint16 unk2,
    int32 unk3,
    uint16 unk4,
    uint16 bBoatsOnly,
    int32 unk6
) {
    CNodeAddress tempAddress;
    plugin::CallMethodAndReturn<CNodeAddress*, 0x44F460, CPathFind*, CNodeAddress*, CVector, ePathType, float, uint16, int32, uint16, uint16, int32>(
        this, &tempAddress, pos, nodeType, maxDistance, unk2, unk3, unk4, bBoatsOnly, unk6);
    return tempAddress;
}

// 0x450A60
void CPathFind::UpdateStreaming(bool bForceStreaming) {
    ZoneScoped;

    // The time thingy I think is some kind of `% 512`, not sure yet, will have to figure it out.
    if (!s_bLoadPathsNeeded && !bForceStreaming && (CTimer::m_snTimeInMilliseconds ^ CTimer::m_snPreviousTimeInMilliseconds) < 512) {
        return;
    }

    rng::fill(ToBeStreamed, false);
    std::array<bool, NUM_PATH_MAP_AREAS> ToBeStreamedForScript{};

    // Mark areas around the player
    if (FindPlayerPed()) {
        MarkRegionsForCoors(FindPlayerCoors(), 350.f);
    }

    // Mark areas requested by `SetPathsNeededAtPosition`
    if (s_bLoadPathsNeeded) {
        MarkRegionsForCoors(s_pathsNeededPosn, 300.f);
        s_bLoadPathsNeeded = false;
    }

    // Mark paths around some specific mission vehicles
    for (const auto& veh : GetVehiclePool()->GetAllValid()) {
        if (!veh.IsMissionVehicle()) {
            continue;
        }

        switch (veh.m_nVehicleSubType) {
        case VEHICLE_TYPE_HELI:
        case VEHICLE_TYPE_PLANE:
        case VEHICLE_TYPE_BOAT:
        case VEHICLE_TYPE_TRAIN:
        case VEHICLE_TYPE_FPLANE:
            break;
        default:
            MarkRegionsForCoors(veh.GetPosition(), 300.f);
        }
    }

    // Mark areas inside load request rect
    if (m_loadAreaRequestPending) {
        IterAreasTouchingRect(
            { m_loadAreaRequestMinX, m_loadAreaRequestMinY, m_loadAreaRequestMaxX, m_loadAreaRequestMaxY },
            [&, this](auto areaId) {
                ToBeStreamed[areaId] = ToBeStreamedForScript[areaId] = true;
                return true;
            }
        );
    }

    // Load/unload areas as per `ToBeStreamed`
    for (const auto [areaId, shouldBeLoaded] : notsa::enumerate(ToBeStreamed)) {
        if (shouldBeLoaded) {
            if (!IsAreaLoaded(areaId)) {
                CStreaming::RequestModel(
                    DATToModelId(areaId),
                    ToBeStreamedForScript[areaId]
                        ? STREAMING_MISSION_REQUIRED
                        : STREAMING_KEEP_IN_MEMORY
                );
                DEV_LOG("Requested area: %i", (int)areaId);
            }
        } else if (IsAreaLoaded(areaId)) {
            CStreaming::RemoveModel(DATToModelId(areaId));
            DEV_LOG("Removed area: %i", (int)areaId);
        }
    }
}

// 0x44DE80
void CPathFind::StartNewInterior(int32 interiorNum) {
    InteriorIDBeingBuilt = interiorNum;
    bInteriorBeingBuilt = true;
    NumNodesGiven = 0;
    NumLinksToExteriorNodes = 0;

    // BUG: Possible endless loop if 8 interiors are loaded i think
    NewInteriorSlot = 0;
    while (m_interiorIDs[NewInteriorSlot] != (uint32)-1) {
        NewInteriorSlot++;
        assert(NewInteriorSlot < 8);
    }
}

// 0x450E90
CNodeAddress CPathFind::AddNodeToNewInterior(
    float x,
    float y,
    float z,
    bool bDontWander,
    int8 con0,
    int8 con1,
    int8 con2,
    int8 con3,
    int8 con4,
    int8 con5
) {
    const auto idx = NumNodesGiven++;
    XCoorGiven[idx] = x;
    YCoorGiven[idx] = y;
    ZCoorGiven[idx] = z;
    rng::copy(std::array{ con0, con1, con2, con3, con4, con5 }, ConnectsToGiven[idx].begin());
    return { (uint16)(NUM_PATH_MAP_AREAS + NewInteriorSlot), (uint16)idx };
}

// 0x451300 unused
CNodeAddress CPathFind::ReturnInteriorNodeIndex(int32 unkn, uint32 intId, int16 nodeId) {
    for (auto i = 0; i < NUM_PATH_INTERIOR_AREAS; ++i) {
        if (m_interiorIDs[i] == intId) {
            return CNodeAddress(NUM_PATH_MAP_AREAS + i, nodeId);
        }
    }
    return {};
}

// 0x451350
CCarPathLinkAddress CPathFind::FindLinkBetweenNodes(CNodeAddress nodeAddrA, CNodeAddress nodeAddrB) {
    if (AreNodeAreasLoaded({ nodeAddrA, nodeAddrB })) {
        const auto nodeA = GetPathNode(nodeAddrA);
        for (auto i = 0u; i < nodeA->m_nNumLinks; i++) {
            const auto linkIdx = nodeA->m_wBaseLinkId + i;
            if (m_pNodeLinks[nodeA->m_wAreaId][linkIdx] == nodeAddrB) {
                return m_pNaviLinks[nodeA->m_wAreaId][linkIdx];
            }
        }
    }
    return {};
}

// 0x4513F0
CVector CPathFind::FindParkingNodeInArea(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    CVector vecOut;
    plugin::CallMethod<0x4513F0, CPathFind*, CVector*, float, float, float, float, float, float>(this, &vecOut, minX, maxX, minY, maxY, minZ, maxZ);
    return vecOut;
}

// 0x450F30
CNodeAddress CPathFind::FindNearestExteriorNodeToInteriorNode(int32 interiorId) {
    return FindNodeClosestToCoors(
        { XCoorGiven[interiorId], YCoorGiven[interiorId], ZCoorGiven[interiorId] },
        ePathType::PATH_TYPE_PED,
        3.f,
        0,
        0,
        0,
        0,
        true
    );
}

// 0x44E000
void CPathFind::AddDynamicLinkBetween2Nodes_For1Node(CNodeAddress first, CNodeAddress second) {
    assert(IsAreaNodesAvailable(first));

    auto& firstPathInfo = m_pPathNodes[first.m_wAreaId][first.m_wNodeId];
    auto numAddresses = m_anNumAddresses[first.m_wAreaId];

    uint32 firstLinkId;
    if (static_cast<uint32>(firstPathInfo.m_wBaseLinkId) >= numAddresses)
        firstLinkId = firstPathInfo.m_wBaseLinkId;
    else {
        auto* nodeLink = &m_pNodeLinks[first.m_wAreaId][numAddresses];
        auto linkCounter = 0u;
        while (!nodeLink->IsValid()) {
            nodeLink += 12; // No clue why we jump 12 objects each time (Search: MAGIC_NUM_12)
            ++linkCounter;
        }

        firstLinkId = numAddresses + 12 * linkCounter;
        for (auto i = 0u; i < firstPathInfo.m_nNumLinks; ++i) {
            m_pNodeLinks[first.m_wAreaId][firstLinkId + i] = m_pNodeLinks[first.m_wAreaId][firstPathInfo.m_wBaseLinkId + i];
            m_pLinkLengths[first.m_wAreaId][firstLinkId + i] = m_pLinkLengths[first.m_wAreaId][firstPathInfo.m_wBaseLinkId + i];
            m_pPathIntersections[first.m_wAreaId][firstLinkId + i] = m_pPathIntersections[first.m_wAreaId][firstPathInfo.m_wBaseLinkId + i];
        }

        if (first.m_wAreaId < NUM_PATH_MAP_AREAS) {
            auto& linkInfo = m_aDynamicLinksBaseIds[first.m_wAreaId];
            for (auto i = 0u; i < NUM_DYNAMIC_LINKS_PER_AREA; ++i) {
                if (linkInfo[i] == -1) {
                    m_aDynamicLinksBaseIds[first.m_wAreaId][i] = firstPathInfo.m_wBaseLinkId;
                    m_aDynamicLinksIds[first.m_wAreaId][i] = firstLinkId;
                    break;
                }
            }
        }
    }

    m_pNodeLinks[first.m_wAreaId][firstLinkId + firstPathInfo.m_nNumLinks] = second;
    m_pLinkLengths[first.m_wAreaId][firstLinkId + firstPathInfo.m_nNumLinks] = 5;
    m_pPathIntersections[first.m_wAreaId][firstLinkId + firstPathInfo.m_nNumLinks].Clear();
    firstPathInfo.m_nNumLinks++;
    firstPathInfo.m_wBaseLinkId = firstLinkId;
}

// 0x44D230
bool CPathFind::These2NodesAreAdjacent(CNodeAddress nodeAddress1, CNodeAddress nodeAddress2) {
    const auto node1 = GetPathNode(nodeAddress1);
    for (auto i = 0u; i < node1->m_nNumLinks; i++) {
        if (m_pNodeLinks[node1->m_wAreaId][node1->m_wBaseLinkId + i] == nodeAddress2) {
            return true;
        }
    }
    return false;
}

// 0x44FCE0
CNodeAddress CPathFind::FindNodeClosestToCoorsFavourDirection(CVector pos, ePathType nodeType, CVector2D dir) {
    dir = dir.Normalized(); // In-place normalize
    
    CNodeAddress closest{};
    float        scoreOfClosest{std::numeric_limits<float>::max()};
    for (auto areaId{ 0u }; areaId < NUM_TOTAL_PATH_NODE_AREAS; areaId++) {
        for (const auto& node : GetPathNodesInArea(areaId, nodeType)) { // NOTE: Function takes care of checking whenever the area is loaded
            const auto dirToNodeUN = pos - node.GetPosition();

            const auto dotScore = (abs(dirToNodeUN) * CVector { 1.f, 1.f, 3.f }).ComponentwiseSum();
            if (dotScore >= scoreOfClosest) {
                continue;
            }

            const auto score = dotScore - (dir.Dot(CVector2D{ dirToNodeUN }.Normalized()) - 1.f) * 20.f;
            if (score <= scoreOfClosest) {
                scoreOfClosest = score;
                closest = node.GetAddress();
            }
        }
    }

    return closest;
}

// 0x5D34C0
bool CPathFind::Save() {
    CGenericGameStorage::SaveDataToWorkBuffer(m_nNumForbiddenAreas);
    for (auto& area : std::span{ m_aForbiddenAreas, m_nNumForbiddenAreas }) {
        CGenericGameStorage::SaveDataToWorkBuffer(area);
    }
    return true;
}

// 0x5D3500
bool CPathFind::Load() {
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nNumForbiddenAreas);
    for (auto& area : std::span{ m_aForbiddenAreas, m_nNumForbiddenAreas }) {
        CGenericGameStorage::LoadDataFromWorkBuffer(area);
    }
    return true;
}

bool CPathFind::AreNodeAreasLoaded(const std::initializer_list<CNodeAddress>& addrs) const {
    return rng::all_of(addrs, [this](auto&& addr) { return IsAreaNodesAvailable(addr); });
}

// 0x44DCD0
void CPathFind::SetPathsNeededAtPosition(const CVector& posn) {
    s_pathsNeededPosn = posn;
    s_bLoadPathsNeeded = true;
}

namespace detail {
constexpr size_t RegionValueOf(float p, size_t nareas) {
    return std::clamp((uint32)((p + 3000.f) / (6000.f / (float)nareas)), 0u, (uint32)nareas - 1);
}
}; // namespace detail

size_t CPathFind::FindXRegionForCoors(float x) const {
    return detail::RegionValueOf(x, NUM_PATH_MAP_AREA_X);
}

size_t CPathFind::FindYRegionForCoors(float y) const {
    return detail::RegionValueOf(y, NUM_PATH_MAP_AREA_Y);
}

// 0x44DB60
void CPathFind::MarkRegionsForCoors(CVector pos, float radius) {
    // HACK: Since the below function isnt `static` (TODO...) we gotta use the class instance here...
    ThePaths.IterAreasTouchingRect(
        { pos, radius },
        [](auto areaId) {
            ToBeStreamed[areaId] = true;
            return true;
        }
    );
}

// 0x44D3E0 - Moved to CPathNode
bool CPathFind::ThisNodeHasToBeSwitchedOff(CPathNode* node) {
    return node->HasToBeSwitchedOff();
}

// 0x4504F0
// This function is only called from `SwitchOffNodeAndNeighbours` but when unhooked
// it doesn't spoil `eax` which makes the former crash
// so hopefully the `__asm mov eax, this` fixes it
// If not just lock both :D
size_t CPathFind::CountNeighboursToBeSwitchedOff(const CPathNode& node) {
    const auto ret = (size_t)rng::count_if(GetNodeLinkedNodes(node), &CPathNode::HasToBeSwitchedOff);
    __asm mov eax, this // It has to be `this`
    return ret;
}

// 0x452160
void CPathFind::SwitchOffNodeAndNeighbours(CPathNode* node, CPathNode*& outNext1, CPathNode** outNext2, bool isOnDeadEnd, bool setIsDeadEndToOriginal) {
    const auto isNodeOnDeadEnd = setIsDeadEndToOriginal
        ? node->m_isOriginallyOnDeadEnd
        : isOnDeadEnd;
    node->m_onDeadEnd = isNodeOnDeadEnd;
   
    outNext1 = nullptr;
    if (outNext2) {
        *outNext2 = nullptr;
    }

    if (CountNeighboursToBeSwitchedOff(*node) > 2) {
        return;
    }

    for (auto& linked : GetNodeLinkedNodes(*node)) {
        if (!linked.HasToBeSwitchedOff()) {
            continue;
        }
        if (linked.m_onDeadEnd == isNodeOnDeadEnd) {
            continue;
        }
        if (CountNeighboursToBeSwitchedOff(*node) > 2) {
            continue;
        }
        if (!outNext1) {
            outNext1 = &linked;
        }
#ifdef FIX_BUGS // Above it was checked whenever it's set so I assume this was a bug
        else if (outNext2) // Don't get confused, `outNext2` is a ptr to a ptr
#else
        else
#endif
        {
            *outNext2 = &linked;
        }
    }
}

// 0x44DF30
void CPathFind::AddInteriorLinkToExternalNode(int32 interiorNodeIdx, CNodeAddress externalNodeAddr) {
    const auto idx = NumLinksToExteriorNodes++;
    aInteriorNodeLinkedToExterior[idx] = interiorNodeIdx;
    aExteriorNodeLinkedTo[idx] = externalNodeAddr;
}

// 0x44E1A0
void CPathFind::RemoveInterior(uint32 intId) {
    for (auto intSlot = 0u; intSlot < NUM_PATH_INTERIOR_AREAS; intSlot++) {
        const auto intSlotAreaId = NUM_PATH_MAP_AREAS + intSlot;

        if (m_interiorIDs[intSlot] != intId) {
            continue;
        }

        for (auto areaId = 0u; areaId < NUM_TOTAL_PATH_NODE_AREAS; areaId++) {
            for (auto& node : GetPathNodesInArea(areaId, PATH_TYPE_PED)) {
                // I assume this checks if the link is an interior link?
                if ((int32)node.m_wBaseLinkId < (int32)m_anNumAddresses[areaId]) {
                    continue;
                }

                // Remove all link of this node that point to a node int the current interior
                bool foundNodeFromOtherInt{}, foundNodeFromThisInt{};
                (void)rng::remove_if(GetNodeLinkedNodes(node, false), [&](CPathNode& linkedNode) {
                    if (linkedNode.m_wAreaId == intSlotAreaId) {
                        node.m_nNumLinks--;
                        foundNodeFromOtherInt = true;
                        return true;
                    } else if (linkedNode.m_wAreaId >= NUM_PATH_MAP_AREAS) {
                        foundNodeFromThisInt = true;
                    }
                    return false;
                });

                // If we found a linked node and there was no other node from another interior
                if (foundNodeFromOtherInt || !foundNodeFromThisInt) {
                    continue;
                }

                // TODO: Magic number `12` (Search: MAGIC_NUM_12)
                // Null out all links of this node
                rng::fill(std::span{ &m_pNodeLinks[node.m_wAreaId][node.m_wBaseLinkId], 12 }, CNodeAddress{});

                // Delete dynamic link of this area
                // Honestly, this doesn't make much sense... As in, I don't think these are dynamic areas? We'll see.. TODO
                const auto& dynLinks  = m_aDynamicLinksIds[intSlot];
                const auto  dynLinkIt = rng::find(dynLinks, (int32)node.m_wBaseLinkId);
                if (dynLinkIt != rng::end(dynLinks)) {
                    const auto dynLinkIdx = rng::distance(rng::begin(dynLinks), dynLinkIt);
                    node.m_wBaseLinkId = m_aDynamicLinksBaseIds[intSlot][dynLinkIdx];
                    m_aDynamicLinksBaseIds[intSlot][dynLinkIdx] = -1;
                    m_aDynamicLinksIds[intSlot][dynLinkIdx] = -1;
                }
            }
        }

        // Finally, unload area and related data
        const auto FreeAndNull = [](auto& ptr) {
            CMemoryMgr::Free(ptr);
            ptr = nullptr;
        };
        FreeAndNull(m_pPathIntersections[intSlotAreaId]);
        FreeAndNull(m_pLinkLengths[intSlotAreaId]);
        FreeAndNull(m_pPathNodes[intSlotAreaId]);
        FreeAndNull(m_pLinkLengths[intSlotAreaId]);

        m_anNumAddresses[intSlotAreaId] = 0;
        m_anNumCarPathLinks[intSlotAreaId] = 0;
        m_anNumPedNodes[intSlotAreaId] = 0;
        m_anNumVehicleNodes[intSlotAreaId] = 0;
        m_anNumNodes[intSlotAreaId] = 0;
    }
}

// 0x44D930
void CPathFind::FindStartPointOfRegion(size_t x, size_t y, float& outX, float& outY) {
    const auto pos = FindStartPointOfRegion(x, y);
    outX = pos.x;
    outY = pos.y;
}

// notsa
CVector2D CPathFind::FindStartPointOfRegion(size_t x, size_t y) {
    return { FindXCoorsForRegion(x), FindYCoorsForRegion(y) };
}

namespace detail {
constexpr auto GetCoorsOfRegion(size_t p, size_t nareas) {
    return (6000.f / (float)nareas) * (float)p - 3000.f;
}
};

// 0x44D8F0
float CPathFind::FindXCoorsForRegion(size_t x) {
    return detail::GetCoorsOfRegion(x, NUM_PATH_MAP_AREA_X);
}

// 0x44D910
float CPathFind::FindYCoorsForRegion(size_t y) {
    return detail::GetCoorsOfRegion(y, NUM_PATH_MAP_AREA_Y);
}

// 0x44DED0
void CPathFind::AddInteriorLink(int32 intNodeA, int32 intNodeB) {
    const auto AddLink = [](int32 intIdx, int32 linkTo) {
        const auto it = rng::find(ConnectsToGiven[intIdx], -1); 
        assert(!(*it >= 0)); // NOTE: Original code did a `while (*it >= 0), if anything goes bad use `>= 0` for `rng::find`
        *it = linkTo;
    };
    AddLink(intNodeA, intNodeB);
    AddLink(intNodeB, intNodeA);
}

// notsa
std::span<CPathNode> CPathFind::GetPathNodesInArea(size_t areaId, ePathType ptype) const {
    if (const auto allNodes = m_pPathNodes[areaId]) {
        const auto numVehNodes = m_anNumVehicleNodes[areaId];
        switch (ptype) {
        case ePathType::PATH_TYPE_VEH: // Vehicles, boats, race tracks
            return std::span{ allNodes, m_anNumVehicleNodes[areaId] };
        case ePathType::PATH_TYPE_PED: // Peds only
            assert(m_anNumPedNodes[areaId] == m_anNumNodes[areaId] - numVehNodes); // Pirulax: I'm assuming this is true, so if this doesnt assert for a long time remove it
            return std::span{ allNodes + numVehNodes, m_anNumPedNodes[areaId] };
        case ePathType::PATH_TYPE_ALL: // All of the above
            return std::span{ allNodes, m_anNumNodes[areaId] };
        default:
            NOTSA_UNREACHABLE("Invalid pathType: {}", (int)ptype);
        }
    }
    return {}; // Area not loaded, return nothing.. Perhaps assert here instead?
}

// 0x44D1B0
void CPathFind::RemoveNodeFromList(CPathNode* node) {
    node->m_prev->m_next = node->m_next;
    if (node->m_next) {
        node->m_next->m_prev = node->m_prev;
    }

    m_totalNumNodesInPathFindHashTable--;
}

void CPathFind::AddNodeToList(CPathNode* node, int32 distFromOrigin) {
    // Insert the node as the head into it's bucket

    auto& head = m_pathFindHashTable[distFromOrigin % std::size(m_pathFindHashTable)];

    node->m_next = head;

    // Make this node's `next` point to the head in the hash table
    // I guess this works as long as you only access the `m_prev` variable
    // as that's at offset 0
    // This is a really bad hack to avoid having to do special handling
    // for the head in `RemoveNodeFromList`...
    node->m_prev = reinterpret_cast<CPathNode*>(&head);

    if (head) {
        head->m_prev = node;
    }

    head = node;

    assert(distFromOrigin <= std::numeric_limits<decltype(node->m_totalDistFromOrigin)>::max()); // Prevent bugs from overflow
    node->m_totalDistFromOrigin = (int16)distFromOrigin;

    m_totalNumNodesInPathFindHashTable++;
}
