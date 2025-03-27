#include "StdInc.h"

#include <extensions/enumerate.hpp>

#include "3dMarkers.h"
#include "PostEffects.h"

void C3dMarkers::InjectHooks() {
    RH_ScopedClass(C3dMarkers);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x724E40);
    RH_ScopedInstall(Update, 0x7227B0);
    RH_ScopedInstall(Shutdown, 0x722710);
    RH_ScopedInstall(Render, 0x725040);
    RH_ScopedInstall(DirectionArrowFindFirstFreeSlot, 0x721120);
    RH_ScopedInstall(DirectionArrowSet, 0x721140);
    RH_ScopedInstall(DirectionArrowsDraw, 0x7211F0);
    RH_ScopedInstall(DirectionArrowsInit, 0x721100);
    RH_ScopedInstall(ForceRender, 0x722870);
    RH_ScopedInstall(PlaceMarker, 0x725120);
    RH_ScopedInstall(PlaceMarkerCone, 0x726D40);
    RH_ScopedInstall(PlaceMarkerSet, 0x725BA0);
    RH_ScopedInstall(User3dMarkerAtomicCB, 0x7210D0);
    RH_ScopedInstall(User3dMarkerDelete, 0x721090);
    RH_ScopedInstall(User3dMarkerDeleteAll, 0x7210B0);
    RH_ScopedInstall(User3dMarkerFindFirstFreeSlot, 0x720FB0);
    RH_ScopedInstall(User3dMarkerSet, 0x720FD0);
    RH_ScopedInstall(User3dMarkersDraw, 0x723240);
    RH_ScopedInstall(LoadUser3dMarkers, 0x5D42E0);
    RH_ScopedInstall(SaveUser3dMarkers, 0x5D4300);
}

//> 0x724E40
void C3dMarkers::Init() {
    for (auto& marker : m_aMarkerArray) {
        marker.Init();
    }

    NumActiveMarkers = 0;
    m_angleDiamondDeg = 0.0f;

    //
    // Load clumps + textures + models
    //

    rng::fill(m_pRpClumpArray, nullptr);

    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));
    CFileMgr::ChangeDir("\\");

    m_pRpClumpArray[MARKER3D_CYLINDER] = LoadMarker("cylinder"); // black cylinder
    m_pRpClumpArray[MARKER3D_TORUS]    = LoadMarker("hoop");     // black hoop

    m_pRpClumpArray[MARKER3D_ARROW]
        = m_pRpClumpArray[MARKER3D_CONE_NO_COLLISION]
        = m_pRpClumpArray[MARKER3D_CONE]
        = LoadMarker("diamond_3");

    // MARKER3D_ARROW2 and MARKER3D_TUBE don't seem to have clumps.

    CTxdStore::PopCurrentTxd();
}

//> 0x7227B0
void C3dMarkers::Update() {
    m_angleDiamondDeg += CTimer::GetTimeStep() * 5.0f;

    for (auto& marker : m_aMarkerArray) {
        if (marker.m_bIsUsed) {
            marker.m_mat.UpdateRW();
            RwFrameUpdateObjects(RpAtomicGetFrame(marker.m_pAtomic));
            marker.m_bMustBeRenderedThisFrame = true;
        }
    }
}

//> 0x722710
void C3dMarkers::Shutdown() {
    for (auto& marker : m_aMarkerArray) {
        marker.DeleteIfHasAtomic();
    }

    // Original code is retarded, this does the same, but better.
    for (auto& clump : m_pRpClumpArray) {
        if (clump) {
            RpClumpForAllAtomics(clump, RemoveRefsCB, nullptr);
            RpClumpDestroy(clump);

            clump = nullptr;
        }
    }
}

//> 0x725040
void C3dMarkers::Render() {
    ZoneScoped;

    static RwRGBAReal& ambient = *(RwRGBAReal*)0xC80444; // STATICREF
    static RwRGBAReal& directional = *(RwRGBAReal*)0xC80434; // STATICREF

    // Setup render states
    ActivateDirectional();
    SetAmbientColours(&ambient);
    SetDirectionalColours(&directional);

    // Now draw all
    User3dMarkersDraw();
    Render3dMarkers();
    DirectionArrowsDraw();
}

// Code from `0x725103`
void C3dMarkers::Render3dMarkers() {
    NumActiveMarkers = 0;

    for (auto& marker : m_aMarkerArray) {
        if (marker.m_bMustBeRenderedThisFrame) {
            if (TheCamera.IsSphereVisible(marker.m_mat.GetPosition(), 2.0f, reinterpret_cast<RwMatrix*>(&TheCamera.m_mMatInverse))) {
                if (marker.m_DistToCam2D < 150.0f || IgnoreRenderLimit || marker.m_nType == MARKER3D_TORUS) {
                    marker.Render();
                }
            }
            marker.m_bIsUsed = false;
            marker.m_bMustBeRenderedThisFrame = false;

            NumActiveMarkers += 1;
        } else {
            marker.DeleteIfHasAtomic();
        }
    }
}

// notsa, code from 0x725198
C3dMarker* C3dMarkers::FindById(uint32 id) {
    for (auto& a : m_aMarkerArray) {
        if (!a.m_bIsUsed && a.m_nIdentifier == id) {
            return &a;
        }
    }
    return nullptr;
}

// notsa, code from 0x7251EA
C3dMarker* C3dMarkers::FindFree() {
    for (auto& a : m_aMarkerArray) {
        if (a.m_nType == MARKER3D_NA) {
            return &a;
        }
    }
    return nullptr;
}

// Returns slot index; -1 if not found
//> 0x721120
int32 C3dMarkers::DirectionArrowFindFirstFreeSlot() {
    for (const auto& [index, arrow] : notsa::enumerate(ms_directionArrows)) {
        if (!arrow.m_bIsUsed) {
            return index;
        }
    }
    return -1;
}

//> 0x721140
void C3dMarkers::DirectionArrowSet(CVector posn, float size, int32 red, int32 green, int32 blue, int32 alpha, float dirX, float dirY, float dirZ) {
    auto arrowIndex = DirectionArrowFindFirstFreeSlot();
    if (arrowIndex == -1) {
        return;
    }

    auto& arrow = ms_directionArrows[arrowIndex];
    arrow.m_vecPosition.Set(posn.x, posn.y, posn.z + 3.0f);
    arrow.m_normal.Set(dirX, dirY, dirZ);
    arrow.m_fSize = size;
    arrow.m_nRed = red;
    arrow.m_nGreen = green;
    arrow.m_nBlue = blue;
    arrow.m_nAlpha = alpha;
    arrow.m_bIsUsed = true;
}

//> 0x7211F0
void C3dMarkers::DirectionArrowsDraw() {
    auto bRenderParamsSet = false;

    for (auto& arrow : ms_directionArrows) {
        if (!arrow.m_bIsUsed) {
            continue;
        }

        if (!bRenderParamsSet) {
            RwRenderStateGet(rwRENDERSTATECULLMODE,          &CPostEffects::ms_imf.cullMode);
            RwRenderStateGet(rwRENDERSTATEZTESTENABLE,       &CPostEffects::ms_imf.bZTest);
            RwRenderStateGet(rwRENDERSTATEZWRITEENABLE,      &CPostEffects::ms_imf.bZWrite);
            RwRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, &CPostEffects::ms_imf.bVertexAlpha);

            RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLNONE));
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
            RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));

            bRenderParamsSet = true;
        }

        arrow.Render(m_pRpClumpArray[MARKER3D_CONE]);

        arrow.m_bIsUsed = false;
    }

    if (bRenderParamsSet) {
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(CPostEffects::ms_imf.cullMode));
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(CPostEffects::ms_imf.bZTest));
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(CPostEffects::ms_imf.bZWrite));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(CPostEffects::ms_imf.bVertexAlpha));
    }
}

//> 0x721100
void C3dMarkers::DirectionArrowsInit() {
    for (auto& arrow : ms_directionArrows) {
        arrow.m_bIsUsed = false;
    }
}

//> 0x722870
void C3dMarkers::ForceRender(bool ignore) {
    IgnoreRenderLimit = ignore;
}

//> 0x722810
RpClump* C3dMarkers::LoadMarker(const char* modelName) {
    // Find model from name
    int32 modelId{ MODEL_INVALID };
    const auto mi = CModelInfo::GetModelInfo(modelName, &modelId);

    // Make sure model is loaded
    CStreaming::RequestModel(modelId, STREAMING_GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);

    // Create an instance of the model, reset it's rotation and return.
    const auto atomic = reinterpret_cast<RpAtomic*>(mi->CreateInstance());
    const auto frame  = RpAtomicGetFrame(atomic);
    const auto clump  = RpClumpCreate();

    RwFrameSetIdentity(frame);

    RpClumpAddAtomic(clump, atomic);
    RpClumpSetFrame(clump, frame);

    mi->AddRef();

    return clump;
}

//> 0x725120
C3dMarker* C3dMarkers::PlaceMarker(
    uint32 id,
    e3dMarkerType type,
    CVector& pos,
    float size,
    uint8 red, uint8 green, uint8 blue, uint8 alpha,
    uint16 pulsePeriod,
    float pulseFraction,
    int16 rotateRate,
    CVector normal,
    bool zCheck
) {
    assert(id != 0);

    auto markerToPlayerDist2D = (pos - FindPlayerCentreOfWorld(0)).Magnitude2D();

    switch (type) {
    case MARKER3D_TUBE: {
        markerToPlayerDist2D *= 0.25f;
        break;
    }
    case MARKER3D_ARROW:
    case MARKER3D_ARROW2:
    case MARKER3D_TORUS:
    case MARKER3D_CONE:
    case MARKER3D_CONE_NO_COLLISION:
    case MARKER3D_CYLINDER: {
        break;
    }
    default: {
        NOTSA_LOG_WARN("Invalid marker type ({})", (int)type);
        return nullptr; // OG behaviour
    }
    }


    //> 0x725198 - Try finding by ID
    C3dMarker* m{FindById(id)};

    //> 0x72521B - If not found by ID, try finding an unused one, or find one that can be replaced
    if (!m && !(m = FindFree())) {
        const auto IsReusableType = [](e3dMarkerType t) {
            switch (t) {
            case MARKER3D_ARROW:
            case MARKER3D_ARROW2:
            case MARKER3D_CONE:
            case MARKER3D_CONE_NO_COLLISION:
                return true;
            default:
                return false;
            }
        };

        if (!IsReusableType(type)) {
            return nullptr;
        }

        for (auto& v : m_aMarkerArray) {
            if (IsReusableType(v.m_nType) && (!m || v.m_DistToCam2D > m->m_DistToCam2D)) {
                m = &v;
            }
        }

        if (m) {
            m->m_nType = MARKER3D_NA;
        }
    }

    //> 0x725498 - Init the marker
    if (!m) {
        return nullptr;
    }

    m->m_DistToCam2D = markerToPlayerDist2D;

    if (m->m_nIdentifier == id && m->m_nType == type) {
        switch (type) {
        case MARKER3D_CONE:
        case MARKER3D_ARROW: {
            //> 0x725517
            if (m->m_vecLastPosition.SquaredMagnitude() <= sq(0.01) && CTimer::GetTimeInMS() - m->m_nOnScreenTestTime >= 2'000) {
                m->m_nOnScreenTestTime = CTimer::GetTimeInMS();
                if (m->m_vecLastPosition != pos) {
                    CEntity* hitEntity{};
                    CColPoint hitCP{};
                    const auto hasHit = CWorld::ProcessLineOfSight(
                        pos - CVector{ 0.f, 0.f, 1.5f },
                        pos,
                        hitCP,
                        hitEntity,
                        true,
                        false,
                        false,
                        false,
                        false,
                        false,
                        false,
                        false
                    );
                    m->m_fRoofHeight = hasHit
                        ? hitCP.m_vecPoint.z
                        : 65535.f;
                }
            }

            //> 0x7255F9
            if (m->m_fRoofHeight < 65535.f) {
                size *= 0.5f;
                pos.z = m->m_fRoofHeight - RpAtomicGetBoundingSphere(m->m_pAtomic)->radius * 0.1f; // TODO: Is there a macro for `->radius`?
            }

            break;
        }
        }

        //> 0x725626
        switch (type) {
        case MARKER3D_CONE:
        case MARKER3D_CONE_NO_COLLISION: {
            pos.z += std::sin(DegreesToRadians(m_angleDiamondDeg)) * (m->m_fRoofHeight < 65535.f ? 0.15f : 0.3f);
        }
        }

        //> 0x725660
        switch (type) {
        case MARKER3D_ARROW:
        case MARKER3D_ARROW2:
        case MARKER3D_CONE:
        case MARKER3D_CONE_NO_COLLISION: {
            m->m_fStdSize = size * (1.f - 0.3f * (25.f - std::clamp(markerToPlayerDist2D, 5.f, 25.f)) / 20.f);
        }
        }

        //> 0x7256D1 -> Moved after the `if` branch

        //> 0x72577E
        m->m_fSize = m->m_fStdSize - std::sin(0.f) * m->m_fStdSize * pulseFraction;
        if (m->m_nRotateRate) {
            const auto lastPos = m->m_mat.GetPosition();
            m->m_mat.RotateZ(DegreesToRadians((float)(m->m_nRotateRate) * CTimer::GetTimeStep()));
            m->m_mat.GetPosition().x = lastPos.x;
            m->m_mat.GetPosition().y = lastPos.y;
            if (m->m_nLastMapReadX != (uint16)lastPos.x || m->m_nLastMapReadY != (uint16)lastPos.y) {
                m->m_mat.GetPosition().z = lastPos.z;
            }
        }

        //> 0x7257FB
        switch (type) {
        case MARKER3D_ARROW:
        case MARKER3D_CYLINDER:
        case MARKER3D_CONE:
        case MARKER3D_CONE_NO_COLLISION: {
            m->m_mat.GetPosition().x = pos.x;
            m->m_mat.GetPosition().y = pos.y;
            if (m->m_nLastMapReadX != (uint16)pos.x || m->m_nLastMapReadY != (uint16)pos.y) {
                m->m_mat.GetPosition().z = pos.z;
            }
            break;
        }
        }

        switch (type) {
        case MARKER3D_TORUS:
        case MARKER3D_TUBE: { //> 0x725855
            m->m_mat.GetPosition().x = pos.x;
            m->m_mat.GetPosition().y = pos.y;
            if (type == MARKER3D_TUBE && zCheck) {
                if (m->m_nLastMapReadX != (uint16)pos.x || m->m_nLastMapReadY != (uint16)pos.y) {
                    m->m_mat.GetPosition().z = pos.z;
                }
                m->UpdateZCoordinate(pos, 10.f);
            }
            break;
        }
        case MARKER3D_CYLINDER: { //> 0x7258C2
            auto& markerZ = m->m_mat.GetPosition().z;
            if (float waterZ; !CWaterLevel::GetWaterLevelNoWaves(m->m_mat.GetPosition(), &waterZ, nullptr, nullptr) || waterZ < markerZ) {
                m->UpdateZCoordinate(pos, size);
            } else {
                markerZ = waterZ;
            }
            break;
        }
        }
    } else { //> 0x725956
        if (m->m_nIdentifier) {
            m->m_nIdentifier              = 0;
            m->m_nStartTime               = 0;
            m->m_bIsUsed                  = 0;
            m->m_bMustBeRenderedThisFrame = false;
            m->m_nType                    = MARKER3D_NA;

            const auto f = RpAtomicGetFrame(m->m_pAtomic);
            RpAtomicDestroy(m->m_pAtomic);
            RwFrameDestroy(f);
            m->m_pAtomic = nullptr;
        }

        m->m_nLastMapReadX = 30'000;

        // 0x725991
        switch (type) {
        case MARKER3D_CONE:
        case MARKER3D_CONE_NO_COLLISION: {
            pos.z += std::sin(DegreesToRadians(m_angleDiamondDeg) * 0.3f); // NOTE/BUG: See code from the branch below, they aren't the same for some reason.
        }
        }

        m->m_mat.SetTranslate(pos);

        m->AddMarker(id, type, size, red, green, blue, alpha, pulsePeriod, pulseFraction, rotateRate);

        //> 0x725A16 (Same as 0x7258C2)
        switch (type) {
        case MARKER3D_CYLINDER: {
            auto& markerZ = m->m_mat.GetPosition().z;
            if (float waterZ; !CWaterLevel::GetWaterLevelNoWaves(m->m_mat.GetPosition(), &waterZ, nullptr, nullptr) || waterZ < markerZ) {
                m->UpdateZCoordinate(pos, size);
            } else {
                markerZ = waterZ;
            }
            break;
        }
        }

        m->m_mat.UpdateRW();

        //> 0x725A8D
        switch (type) {
        case MARKER3D_ARROW:
        case MARKER3D_ARROW2:
        case MARKER3D_CONE:
        case MARKER3D_CONE_NO_COLLISION: {
            m->m_fStdSize = size * (1.f - 0.3f * (25.f - std::clamp(markerToPlayerDist2D, 5.f, 25.f)) / 20.f);
        }
        }
    }


    //> 0x725AEC & 0x7256D1
    if (type != MARKER3D_ARROW2) {
        if (size + 12.f <= markerToPlayerDist2D) {
            m->m_colour.a = alpha;
        } else if (size + 1.f >= markerToPlayerDist2D) {
            m->m_colour.a = (uint8)((float)alpha * 0.65f);
        } else {
            m->m_colour.a = (uint8)((float)alpha * (1.f - ((size + 12.f - markerToPlayerDist2D) * 0.35f) / 11.f));
        }
    }

    m->m_vecNormal = normal;
    m->m_bIsUsed   = true;

    return m;
}

//> 0x726D40
void C3dMarkers::PlaceMarkerCone(uint32 id, CVector& point, float size, uint8 red, uint8 green, uint8 blue, uint8 alpha, uint16 pulsePeriod, float pulseFraction, int16 rotateRate, bool bEnableCollision) {
    if ((point - TheCamera.GetPosition()).SquaredMagnitude() >= sq(1.6f)) {
        PlaceMarker(
            id,
            bEnableCollision
                ? MARKER3D_CONE
                : MARKER3D_CONE_NO_COLLISION,
            point,
            size,
            red, green, blue, m_colDiamond,
            pulsePeriod,
            pulseFraction,
            0,
            CVector{0.f, 0.f, 0.f},
            false
        );
    }
}

//> 0x725BA0
void C3dMarkers::PlaceMarkerSet(uint32 id, e3dMarkerType type, CVector& posn, float size, uint8 red, uint8 green, uint8 blue, uint8 alpha, uint16 pulsePeriod, float pulseFraction, int16 rotateRate) {
    PlaceMarker(
        id,
        type,
        posn,
        size,
        red, green, blue, static_cast<uint8>((float)alpha * 1.0f / 3.0f),
        pulsePeriod,
        pulseFraction,
        1,
        CVector{0.0f, 0.0f, 0.0f},
        false
    );
}

//> 0x7210D0
// only set material color (m_user3dMarkerColor) for first material in first atomic; 'data' is unused
RpAtomic* C3dMarkers::User3dMarkerAtomicCB(RpAtomic* atomic, void*) {
    const auto color = m_user3dMarkerColor.ToRwRGBA();
    RpMaterialSetColor(RpGeometryGetMaterial(RpAtomicGetGeometry(atomic), 0), &color);
    return nullptr;
}

//> 0x721090
void C3dMarkers::User3dMarkerDelete(int32 slotIndex) {
    if (slotIndex >= 0 && slotIndex <= (int32)(ms_user3dMarkers.size() - 1)) {
        ms_user3dMarkers[slotIndex].m_bIsUsed = false;
    }
}

//> 0x7210B0
void C3dMarkers::User3dMarkerDeleteAll() {
    for (auto& marker : ms_user3dMarkers) {
        marker.m_bIsUsed = false;
    }
}

// Returns slot index; -1 if not found
//> 0x720FB0
int32 C3dMarkers::User3dMarkerFindFirstFreeSlot() {
    for (const auto& [index, marker] : notsa::enumerate(ms_user3dMarkers)) {
        if (!marker.m_bIsUsed) {
            return index;
        }
    }
    return -1;
}

// Returns slot index; -1 if not created; for 'color', see eHudColours
//> 0x720FD0
int32 C3dMarkers::User3dMarkerSet(float x, float y, float z, eHudColours color) {
    const auto markerIndex = User3dMarkerFindFirstFreeSlot();
    if (markerIndex != -1) {
        auto& marker = ms_user3dMarkers[markerIndex];

        marker.m_vecPosition.Set(x, y, z);
    
        const auto colour = HudColour.GetRGB(color);
        marker.m_nRed = colour.r;
        marker.m_nGreen = colour.g;
        marker.m_nBlue = colour.b;

        marker.m_bIsUsed = true;
    }

    return markerIndex;
}

//> 0x723240
void C3dMarkers::User3dMarkersDraw() {
    bool bRenderParamsSet = false;

    for (auto& marker : ms_user3dMarkers) {
        if (!marker.m_bIsUsed) {
            continue;
        }

        if (!bRenderParamsSet) {
            RwRenderStateGet(rwRENDERSTATECULLMODE, &CPostEffects::ms_imf.cullMode);
            RwRenderStateGet(rwRENDERSTATEZTESTENABLE, &CPostEffects::ms_imf.bZTest);
            RwRenderStateGet(rwRENDERSTATEZWRITEENABLE, &CPostEffects::ms_imf.bZWrite);

            RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));

            bRenderParamsSet = true;
        }

        marker.Render(m_pRpClumpArray[MARKER3D_CONE]);
    }

    if (bRenderParamsSet) {
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(CPostEffects::ms_imf.cullMode));
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(CPostEffects::ms_imf.bZTest));
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(CPostEffects::ms_imf.bZWrite));
    }
}

//> 0x5D42E0
bool C3dMarkers::LoadUser3dMarkers() {
    for (auto& marker : ms_user3dMarkers) {
        CGenericGameStorage::LoadDataFromWorkBuffer(marker);
    }
    return true;
}

//> 0x5D4300
bool C3dMarkers::SaveUser3dMarkers() {
    for (auto& marker : ms_user3dMarkers) {
        CGenericGameStorage::SaveDataToWorkBuffer(marker);
    }
    return true;
}

// Code from (beginning at): 0x7212A4
void tDirectionArrow::Render(RpClump* clump) {
    const auto frame = RpClumpGetFrame(clump);

    // Reset frame's matrix (As we re-calculate it below)
    RwFrameSetIdentity(frame);

    // Update position to our's
    RwFrameTranslate(frame, &m_vecPosition, rwCOMBINEREPLACE);

    auto DoRender = [
        &,
        transform = CMatrix{ RwFrameGetMatrix(frame) } * CMatrix::WithUp(m_normal) // Build transform matrix we'll use to render
    ](CRGBA color, float scale) mutable {
        transform.Scale(scale);
        transform.UpdateRwMatrix(RwFrameGetMatrix(frame)); // Push this matrix to the underlaying RW matrix

        // After matrix update, update frame too
        RwFrameUpdateObjects(frame);

        // Apply color to material of clump
        C3dMarkers::m_user3dMarkerColor = color;
        RpClumpForAllAtomics(clump, C3dMarkers::User3dMarkerAtomicCB, nullptr); // TODO: Can use a lambda here without `m_user3dMarkerColor` too!

        // Finally, render it
        RpClumpRender(clump);
    };

    DoRender({ 0, 0, 0, 255 }, m_fSize);
    DoRender(GetColor(),       0.8f   ); // Matrix overall scale comes out to be `m_fSize * 0.8f` (Because scaling is preserved from the last step)
}

// Code from (beginning at): 0x7232BF
void tUser3dMarker::Render(RpClump* clump) const {
    const auto frame = RpClumpGetFrame(clump);

    // Reset rotation
    RwFrameSetIdentity(frame);

    // Update position
    CVector pos = m_vecPosition;
    pos.z += std::sin(DegreesToRadians(C3dMarkers::m_angleDiamondDeg)) * 0.25f;
    RwFrameTranslate(frame, &pos, rwCOMBINEREPLACE);

    // Apply color to material of clump
    C3dMarkers::m_user3dMarkerColor = GetColor();
    RpClumpForAllAtomics(clump, C3dMarkers::User3dMarkerAtomicCB, nullptr);

    // Finally, render it
    RpClumpRender(clump);
}
