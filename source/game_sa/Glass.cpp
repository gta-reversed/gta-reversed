#include "StdInc.h"

#include "Glass.h"
#include "FallingGlassPane.h"
#include "Shadows.h"

CVector2D (&CGlass::PanePolyPositions)[4][3] = *(CVector2D(*)[4][3])0x8D5CD8;
int32& CGlass::ReflectionPolyVertexBaseIdx = *(int32*)0xC71B18;
int32& CGlass::ReflectionPolyIndexBaseIdx = *(int32*)0xC71B1C;
RxObjSpace3DLitVertex (&CGlass::ReflectionPolyVertexBuffer)[1706] = *(RxObjSpace3DLitVertex(*)[1706])0xC5B158;
RxObjSpace3DLitVertex (&CGlass::ShatteredPolyVertexBuffer)[512] = *(RxObjSpace3DLitVertex(*)[512])0xC56958;
int32& CGlass::ShatteredVerticesBaseIdx = *(int32*)0xC71B20;
int32& CGlass::ShatteredIndicesBaseIdx = *(int32*)0xC71B24;
uint32& CGlass::H1iLightPolyVerticesIdx = *(uint32*)0xC71B28;
int32& CGlass::HiLightPolyIndicesIdx = *(int32*)0xC71B2C;
CVector2D (&CGlass::PanePolyCenterPositions)[5] = *(CVector2D(*)[5])0xC71B30;
CEntity* (&CGlass::apEntitiesToBeRendered)[32] = *(CEntity * (*)[32])0xC71B58;
CFallingGlassPane (&CGlass::aGlassPanes)[44] = *(CFallingGlassPane(*)[44])0xC71BF8;
uint32& CGlass::LastColCheckMS = *(uint32*)0xC72FA8;

void CGlass::InjectHooks() {
    RH_ScopedClass(CGlass);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x71A8D0);
    RH_ScopedInstall(HasGlassBeenShatteredAtCoors, 0x71CB70);
    RH_ScopedInstall(CarWindscreenShatters, 0x71C2B0);
    RH_ScopedInstall(WasGlassHitByBullet, 0x71C0D0);
    RH_ScopedInstall(WindowRespondsToCollision, 0x71BC40);
    RH_ScopedInstall(GeneratePanesForWindow, 0x71B620);
    RH_ScopedInstall(Update, 0x71B0D0);
    RH_ScopedInstall(Render, 0x71CE20);
    RH_ScopedInstall(FindWindowSectorList, 0x71AFC0);
    RH_ScopedInstall(RenderReflectionPolys, 0x71AED0);
    RH_ScopedInstall(RenderShatteredPolys, 0x71AE30);
    RH_ScopedInstall(RenderHiLightPolys, 0x71ADA0);
    RH_ScopedInstall(CalcAlphaWithNormal, 0x71ACF0);
    RH_ScopedInstall(AskForObjectToBeRenderedInGlass, 0x71ACD0);
    RH_ScopedInstall(FindFreePane, 0x71ACA0);
    RH_ScopedInstall(WindowRespondsToSoftCollision, 0x71AF70);
    RH_ScopedInstall(BreakGlassPhysically, 0x71CF50);
    RH_ScopedInstall(WindowRespondsToExplosion, 0x71C1A0);
}

// 0x71A8D0
void CGlass::Init() {
    for (auto& pane : aGlassPanes) {
        pane.m_bExist = false;
    }

    for (auto i = 0u; i < std::size(PanePolyPositions); i++) {
        const auto& poly = PanePolyPositions[i];

        // Calculate center position of each pane by taking an average
        PanePolyCenterPositions[i] = std::accumulate(std::begin(poly), std::end(poly), CVector2D{}) / (float)std::size(PanePolyPositions);
    }
}

// (float, float, float ) & unused
// 0x71CB70
bool CGlass::HasGlassBeenShatteredAtCoors(CVector point) {
    const int32 startSectorX = std::max(CWorld::GetSectorX(point.x - 30.f), 0);
    const int32 startSectorY = std::max(CWorld::GetSectorY(point.y - 30.f), 0);
    const int32 endSectorX   = std::min(CWorld::GetSectorX(point.x + 30.f), MAX_SECTORS_X - 1);
    const int32 endSectorY   = std::min(CWorld::GetSectorY(point.y + 30.f), MAX_SECTORS_Y - 1);

    CWorld::IncrementCurrentScanCode();

    float maxDist = 20.f;
    CEntity* entity{};
    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            FindWindowSectorList(GetRepeatSector(sectorX, sectorY)->GetList(REPEATSECTOR_OBJECTS), maxDist, entity, point);
            FindWindowSectorList(GetSector(sectorX, sectorY)->m_dummies, maxDist, entity, point);
        }
    }
    return entity && !entity->IsDummy() && entity->AsObject()->objectFlags.bHasBrokenGlass;
}

// 0x71C2B0
void CGlass::CarWindscreenShatters(CVehicle* vehicle) {
    const auto colModel = vehicle->GetColModel();
    if (!colModel || colModel->GetTriCount() < 2) {
        return;
    }

    const auto& triangles = colModel->m_pColData->m_pTriangles;

    // Find the triangle of the glass rectangle
    // They're after each other, so we just have to find the first one here
    uint32 glassTriIdx{};
    for (; glassTriIdx < colModel->GetTriCount(); glassTriIdx++) {
        if (g_surfaceInfos.IsGlass(triangles[glassTriIdx].m_nMaterial)) {
            break;
        }
    }

    // `glassTriIdx` is the first triangle of the glass rectangle, the second one is the one after it.
    // So, logically, if `glassTriIdx + 1` is valid, `glassTriIdx` is going to be valid as well.
    if (glassTriIdx + 1 >= colModel->GetTriCount())
        return;

    CCollision::CalculateTrianglePlanes(colModel);

    auto& vehMat = (CMatrix&)vehicle->GetMatrix();

    // Grab normal and transform it to world space
    const auto normal = vehMat.TransformVector(colModel->m_pColData->m_pTrianglePlanes[glassTriIdx].GetNormal());

    // Calculate direction vectors
    const auto fwd   = Normalized(CrossProduct(vehMat.GetRight(), normal));
    const auto right = Normalized(CrossProduct(vehMat.GetUp(), normal));

    // Store world space vertex positions of both triangles
    // Should be more like [2][3] but a multdim array would've been way harder to deal with
    // so we're going to use a flattened [6] array
    CVector triVertices[6]{};
    for (auto t = 0; t < 2; t++) { // t - triangle idx
        const auto& tri = triangles[glassTriIdx + t];
        for (auto v = 0; v < 3; v++) { // v - vertex idx of this triangle
            triVertices[t * 3 + v] =vehMat.TransformPoint(UncompressVector(colModel->m_pColData->m_pVertices[tri.m_vertIndices[v]]));
        }
    }

    // Calculate dot products for both directions
    // The dot product of the position of a vertex and `right` will give it's "position" on the `right` axis
    // same goes for `fwd`
    // By this we basically get a 2D position of each vertex on the plane of the glass.

    const auto CalculateDotProducts = [&](CVector direction) {
        std::array<float, 6> out{};
        rng::transform(triVertices, out.begin(), [&](auto&& v) { return DotProduct(v, direction); });
        return out;
    };

    // Calculate dot products for all vertices on both direction vectors
    // In 2D terms right would be `x` and `fwd` would be `y`
    const auto rightDots = CalculateDotProducts(right);
    const auto fwdDots   = CalculateDotProducts(fwd);

    // Find max values in each direction
    // This will give us the top right point basically
    const float maxDotFwd   = *rng::max_element(fwdDots);
    const float maxDotRight = *rng::max_element(rightDots);

    // Calculate bottom left corner
    float minRightFwdDotSum = FLT_MAX; // todo: FLT_MAX is not NOTSA ?
    uint32 minRightFwdDotSumIdx{};
    for (auto i = 0u; i < 6u; i++) { // todo: should be start from 1 ?
        const auto rightDot = rightDots[i], fwdDot = fwdDots[i];
        if (rightDot + fwdDot < minRightFwdDotSum) {
            minRightFwdDotSum = rightDot + fwdDot;
            minRightFwdDotSumIdx = i;
        }
    }

    assert(0 <= minRightFwdDotSumIdx && minRightFwdDotSumIdx < std::size(rightDots));
    // Size of pane in directions
    auto extentRight = maxDotRight - rightDots[minRightFwdDotSumIdx];
    auto extentFwd = maxDotFwd - fwdDots[minRightFwdDotSumIdx];

    const auto blPos = triVertices[minRightFwdDotSumIdx];
    GeneratePanesForWindow(
        ePaneType::CAR,
        blPos,
        fwd * extentFwd,
        right * extentRight, vehicle->m_vecMoveSpeed,
        blPos + fwd * extentFwd / 2.f + right * extentRight / 2.f,
        0.1f,
        false,
        false,
        2,
        true
    );
}

bool IsGlassObjectWithCol(CEntity* entity) {
    if (entity->IsObject() && entity->m_bUsesCollision) {
        if (const auto ami = entity->GetModelInfo()->AsAtomicModelInfoPtr()) {
            return ami->IsGlass();
        }
    }
    return false;
}

// 0x71C0D0
void CGlass::WasGlassHitByBullet(CEntity* entity, CVector hitPos) {
    if (!IsGlassObjectWithCol(entity))
        return;

    const auto object = entity->AsObject();
    if (object->objectFlags.bHasBrokenGlass) {
        if (CGeneral::GetRandomNumber() % 4 == 2) {
            WindowRespondsToCollision(entity, 0.0f, {}, hitPos, false);
        }
    } else {
        object->objectFlags.bHasBrokenGlass = true; // Just mark it as broken
    }
}

template<size_t N>
std::pair<float, float> FindMinMaxZOfVertices(CVector (&vertices)[N]) {
    const auto [min, max] = rng::minmax(vertices | rng::views::transform([](auto&& v) { return v.z; }));
    return { min, max };
}

// 0x71BC40
void CGlass::WindowRespondsToCollision(CEntity* entity, float fDamageIntensity, CVector vecMoveSpeed, CVector vecPoint, bool max1PaneSection) {
    auto object = entity->AsObject();
    if (object->objectFlags.bGlassBrokenAltogether)
        return;

    object->objectFlags.bHasBrokenGlass = true;

    if (const auto cd = object->GetColModel()->m_pColData; cd && cd->m_nNumTriangles == 2) {
        // Object space vertices
        CVector verticesOS[4];
        rng::transform(std::span{ cd->m_pVertices, 4 }, verticesOS, UncompressVector);

        const auto [minZ, maxZ] = FindMinMaxZOfVertices(verticesOS);

        // R* used `Mag2D`, but `SqMag2D` is a better choice
        const auto vertFurthestFromV0 = rng::max_element(verticesOS, {}, [v0 = verticesOS[0]](auto&& v) { return (v0 - v).SquaredMagnitude2D(); });

        // Transform vertices to world space
        const auto vert0Pos          = object->GetMatrix().TransformPoint({ verticesOS[0].x, verticesOS[0].y, minZ });
        const auto furthestFromV0Pos = object->GetMatrix().TransformPoint({ vertFurthestFromV0->x, vertFurthestFromV0->y, minZ });

        AudioEngine.ReportGlassCollisionEvent(AE_GLASS_BREAK_FAST, object->GetPosition());
        GeneratePanesForWindow(
            fDamageIntensity <= 300.f ? ePaneType::DELAYED : ePaneType::DEFAULT,
            vert0Pos,
            { 0.f, 0.f, maxZ - minZ },
            furthestFromV0Pos - vert0Pos,
            vecMoveSpeed,
            vecPoint,
            0.1f,
            //object->objectFlags.bGlassBroken,
            (bool)((object->m_nObjectFlags >> 4) & 0xFFFFFF01),
            max1PaneSection,
            1,
            false
        );
    }

    object->m_bUsesCollision = false;
    object->m_bIsVisible = false;
    object->objectFlags.bGlassBrokenAltogether = true;
}

/*
 * 0x71B620
 *
 * Corners:
 * TL ----- TR
 * |  0 | 1 |
 * | ------ | fwd
 * |  2 | 3 |
 * BL ----- BR
 *  right
 *
 * - Neither 'size' vectors are normalized!
 *
 * type                    - @ePaneType
 * point                   - BL
 * fwd, right              - As illustrated above
 * center                  - The centre of the above rectangle (each pane is a piece of it)
 * velocity                - How fast the panes fly
 * velocityCenterDragCoeff - Modify the velocity's direction to be more towards the center point
 * bShatter                - Render shatter polys
 * numSectionsMax1         - Limit no. of sections to 1 - Unsure what's it use-case as setting `numSections` to 1 achieves the same
 * numSections             - No. of sections of each axis, the total number of sections will be `numSections ^ 2` (squared)
**/
void CGlass::GeneratePanesForWindow(ePaneType type, CVector point, CVector fwd, CVector right, CVector velocity, CVector center, float velocityCenterDragCoeff,
                                    bool bShatter, bool numSectionsMax1, int32 numSections, bool unk) {

    const float totalSizeY = fwd.Magnitude(), totalSizeX = right.Magnitude();

    // Calculate no. of sections, and section size
    const auto CalculateCountOfSectionsAndSizeAxis = [&](auto axisSize) {
        const auto count = numSectionsMax1 ? 1 : std::min(numSections * (uint32)(axisSize + 0.75f/*make it round upwards*/), 3u);
        return std::make_pair(count, axisSize / (float)count);
    };

    const auto [countX, sizeX] = CalculateCountOfSectionsAndSizeAxis(totalSizeX);
    const auto [countY, sizeY] = CalculateCountOfSectionsAndSizeAxis(totalSizeY);

    // DEV_LOG("Panes: {} x {} ({.3f} x {.3f})", countX, countY, sizeX, sizeY);

    bool hitGround{};
    float groundZ = CWorld::FindGroundZFor3DCoord(point, &hitGround, nullptr);
    if (!hitGround)
        groundZ = point.z - 2.f;

    if (!countY)
        return;

    for (auto posY = 0u; posY < countY; posY++) {
        for (auto posX = 0u; posX < countX; posX++) {
            for (auto piece = 0u; piece < std::size(PanePolyPositions); piece++) {
                CFallingGlassPane* pane = FindFreePane();
                if (!pane)
                    continue;

                pane->m_nPieceIndex = piece;

                // Calculate matrix
                auto& mat = pane->m_Matrix;
                mat.GetRight() = Normalized(right) * sizeX;
                mat.GetUp() = Normalized(fwd) * sizeY;
                mat.GetForward() = Normalized(CrossProduct(mat.GetRight(), mat.GetUp()));

                // Matrix's position is the center of the glass pane
                const auto paneCenterPos = (PanePolyCenterPositions[piece] + CVector2D{ (float)posX, (float)posY } )* CVector2D{ sizeX, sizeY };
                mat.GetPosition() = point
                    + Normalized(fwd) * paneCenterPos.y
                    + Normalized(right) * paneCenterPos.x;

                {
                    constexpr auto RandomFactor = [] {return (float)((CGeneral::GetRandomNumber() % 128) - 64) * 0.0015f; };
                    pane->m_Velocity = velocity + CVector{ RandomFactor(), RandomFactor(), 0.f };
                }

                if (velocityCenterDragCoeff != 0.0f) {
                    pane->m_Velocity += Normalized(mat.GetPosition() - center) * velocityCenterDragCoeff;
                }

                {
                    constexpr auto RandomFactor = [] { return (float)((CGeneral::GetRandomNumber() % 128) - 64) / 500.f; }; // Random number in range (-0.128, 0.128)
                    pane->m_RandomNumbers = CVector{ RandomFactor(), RandomFactor(), RandomFactor() };
                }

                switch (type) {
                case ePaneType::DELAYED: {
                    pane->m_nCreatedTime = CTimer::GetTimeInMS() + (uint32)((mat.GetPosition() - center).Magnitude() * 100.f);
                    break;
                }
                case ePaneType::CAR:
                case ePaneType::DEFAULT:{
                    pane->m_nCreatedTime = CTimer::GetTimeInMS();
                    break;
                }
                }

                pane->m_fGroundZ = groundZ;
                pane->m_bRenderShatter = bShatter;
                pane->m_fSize = sizeY;
                pane->m_f6F = unk;
                pane->m_bExist = true;
            }
        }
    }
}

// 0x71B0D0
void CGlass::Update() {
    ZoneScoped;

    for (auto& pane : aGlassPanes) {
        if (pane.m_bExist) {
            pane.Update();
        }
    }
}

// 0x71CE20
void CGlass::Render() {
    ZoneScoped;

    H1iLightPolyVerticesIdx = 0;
    HiLightPolyIndicesIdx = 0;

    ShatteredVerticesBaseIdx = 1024;
    ShatteredIndicesBaseIdx = 2048;
    ReflectionPolyVertexBaseIdx = 1536;
    ReflectionPolyIndexBaseIdx = 3072;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGCOLOR,          RWRSTATE(CTimeCycle::m_CurrentColours.GetSkyBottom(0).ToIntARGB()));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));

    for (auto& pane : aGlassPanes) {
        if (pane.m_bExist) {
            pane.Render();
        }
    }

    RenderHiLightPolys();
    RenderShatteredPolys();
    RenderReflectionPolys();

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,  RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,     RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,    RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,    RWRSTATE(FALSE));
}

// (..., CEntity**, float, float, float)
// 0x71AFC0
void CGlass::FindWindowSectorList(CPtrList& objList, float& outDist, CEntity*& outEntity, CVector point) {
    if (!objList.GetNode())
        return;

    for (CPtrNode* it = objList.GetNode(); it;) {
        const auto entity = static_cast<CEntity*>(it->m_item);
        it = it->GetNext();

        if (entity->IsScanCodeCurrent())
            continue;

        if (!entity->IsObject())
            continue;

        const auto object = entity->AsObject();
        if (const auto ami = CModelInfo::GetModelInfo(entity->m_nModelIndex)->AsAtomicModelInfoPtr()) {
            switch (ami->nSpecialType) {
            case eModelInfoSpecialType::GLASS_TYPE_1:
            case eModelInfoSpecialType::GLASS_TYPE_2: {
                object->m_nScanCode = CWorld::ms_nCurrentScanCode;
                const auto dist = DistanceBetweenPoints(point, object->GetPosition());
                if (dist < outDist) {
                    outEntity = entity;
                    outDist = dist;
                }
                break;
            }
            }
        }
    }
}

// 0x71AED0
void CGlass::RenderReflectionPolys() {
    if (ReflectionPolyVertexBaseIdx != 1536) {

        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpShadowHeadLightsTex)));
        RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDINVSRCALPHA));

        if (RwIm3DTransform(ReflectionPolyVertexBuffer, ReflectionPolyVertexBaseIdx - 1536, nullptr, rwIM3D_VERTEXUV))
        {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, &aTempBufferIndices[3072], ReflectionPolyIndexBaseIdx - 3072);
            RwIm3DEnd();
        }

        ReflectionPolyVertexBaseIdx = 1536;
        ReflectionPolyIndexBaseIdx = 3072;
    }
}

// 0x71AE30
void CGlass::RenderShatteredPolys() {
    if (ShatteredVerticesBaseIdx != 1024)
    {
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCrackedGlassTex)));
        RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDINVSRCALPHA));

        if (RwIm3DTransform(ShatteredPolyVertexBuffer, ShatteredVerticesBaseIdx - 1024, nullptr, rwIM3D_VERTEXUV))
        {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, &aTempBufferIndices[2048], ShatteredIndicesBaseIdx - 2048);
            RwIm3DEnd();
        }

        ShatteredVerticesBaseIdx = 1024;
        ShatteredIndicesBaseIdx = 2048;
    }
}

// 0x71ADA0
void CGlass::RenderHiLightPolys() {
    if (H1iLightPolyVerticesIdx != 0)
    {
        RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDONE));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDONE));
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpShadowExplosionTex)));

        if (RwIm3DTransform(TempBufferVertices.m_3d, H1iLightPolyVerticesIdx, nullptr, rwIM3D_VERTEXUV)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, HiLightPolyIndicesIdx);
            RwIm3DEnd();
        }

        HiLightPolyIndicesIdx = 0;
        H1iLightPolyVerticesIdx = 0;
    }
}

// (CVector*)
// 0x71ACF0
uint8 CGlass::CalcAlphaWithNormal(const CVector& normal) {
    const auto& camFwd = TheCamera.GetForward();
    const auto fwdOnNormalProj2x = ProjectVector(camFwd, normal) * 2.f;
    const auto factor = ( // TODO: What the fuck is going on here???
          camFwd.x - fwdOnNormalProj2x.x
        + camFwd.y - fwdOnNormalProj2x.y
        - camFwd.z + fwdOnNormalProj2x.z
    ) / SQRT_3;
    return (uint8)(std::pow(factor, 6) * 235.f + 20.f);
}

// 0x71ACD0
void CGlass::AskForObjectToBeRenderedInGlass(CEntity* entity) {
    if (NumGlassEntities < std::size(apEntitiesToBeRendered) - 1) {
        apEntitiesToBeRendered[NumGlassEntities++] = entity;
    }
}

// 0x71ACA0
CFallingGlassPane* CGlass::FindFreePane() {
    for (auto& pane : aGlassPanes) {
        if (!pane.m_bExist)
            return &pane;
    }
    return nullptr;
}

// 0x71AF70
void CGlass::WindowRespondsToSoftCollision(CEntity* entity, float fDamageIntensity) {
    if (entity->m_bUsesCollision && fDamageIntensity > 50.f && !entity->AsObject()->objectFlags.bHasBrokenGlass) {
        AudioEngine.ReportGlassCollisionEvent(AE_GLASS_HIT, entity->GetPosition());
        entity->AsObject()->objectFlags.bHasBrokenGlass = true;
    }
}

// 0x71CF50
void CGlass::BreakGlassPhysically(CVector point, float radius) {
    if (CTimer::GetTimeInMS() < LastColCheckMS + 1000 && CTimer::GetTimeInMS() >= LastColCheckMS)
        return;

    for (auto objIdx = 0; objIdx < GetObjectPool()->GetSize(); objIdx++) {
        auto object = GetObjectPool()->GetAt(objIdx);
        if (!object)
            continue;

        if (!IsGlassObjectWithCol(object))
            continue;

        const auto colModel = object->GetColModel();
        if (!colModel)
            continue;

        const auto colData = colModel->m_pColData;
        if (!colData)
            continue;

        if (colModel->GetTriCount() < 2)
            continue;

        const auto& objPos = object->GetPosition();
        // Test if point touches any of the model's triangles
        {
            const CColSphere sphere{
                object->GetMatrix().TransformVector(point - objPos),
                radius
            };
            CCollision::CalculateTrianglePlanes(colModel);

            // TODO / NOTE: Shouldn't the loop stop the first time it hits a triangle? Unsure why they didn't do it like that?
            bool hasHit{};
            for (auto tri = 0u; tri < colModel->GetTriCount(); tri++) {
                if (CCollision::TestSphereTriangle(sphere, colData->m_pVertices, colData->m_pTriangles[tri], colData->m_pTrianglePlanes[tri])) {
                    hasHit = true;
                }
            }
            if (!hasHit)
                continue;
        }

        LastColCheckMS = CTimer::GetTimeInMS();

        if (!object->objectFlags.bHasBrokenGlass) {
            AudioEngine.ReportGlassCollisionEvent(AE_GLASS_HIT, objPos);
            object->objectFlags.bHasBrokenGlass = true;
            return;
        }

        CVector verticesObjSpace[4];
        for (auto i = 0u; i < std::size(verticesObjSpace); ++i) {
            verticesObjSpace[i] = UncompressVector(colData->m_pVertices[i]);
        }

        const auto [minZ, maxZ] = FindMinMaxZOfVertices(verticesObjSpace);

        // Vertex furthest of vertex 0
        const auto furthestOfV0 = rng::max_element(verticesObjSpace, {},
            [&](const auto& v) { return DistanceBetweenPoints2D(verticesObjSpace[0], v); }
        );

        const auto v0Pos = object->GetMatrix().TransformPoint({ verticesObjSpace[0].x, verticesObjSpace[0].y, minZ });
        const auto furthestOfV0Pos = object->GetMatrix().TransformPoint({ furthestOfV0->x, furthestOfV0->y, minZ });

        AudioEngine.ReportGlassCollisionEvent(AE_GLASS_HIT, objPos);

        GeneratePanesForWindow(
            ePaneType::DELAYED,
            v0Pos,
            { 0.f, 0.f, maxZ - minZ },
            furthestOfV0Pos - v0Pos,
            {},
            point,
            0.1f,
            object->objectFlags.bHasBrokenGlass,
            false,
            1,
            false
        );
        object->m_bUsesCollision = false;
        object->m_bIsVisible = false;
        object->objectFlags.bHasBrokenGlass = true;
    }
}

// 0x71C1A0
void CGlass::WindowRespondsToExplosion(CEntity* entity, CVector pos) {
    if (!entity->m_bUsesCollision) {
        return;
    }

    const auto& entityPos = entity->GetPosition();
    const auto entityToPosDir = entityPos - pos;
    const auto dist = entityToPosDir.Magnitude();
    if (dist >= 10.f) {
        if (dist < 30.f) {
            entity->AsObject()->objectFlags.bHasBrokenGlass = true;
        }
    } else {
        WindowRespondsToCollision(entity, 10000.f, entityToPosDir * (0.3f / dist), entityPos, true);
    }
}
