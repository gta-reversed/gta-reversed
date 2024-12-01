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
    RH_ScopedInstall(PlaceMarker, 0x725120, { .reversed = false });
    RH_ScopedInstall(PlaceMarkerCone, 0x726D40);
    RH_ScopedInstall(PlaceMarkerSet, 0x725BA0);
    RH_ScopedInstall(User3dMarkerAtomicCB, 0x7210D0);
    RH_ScopedInstall(User3dMarkerDelete, 0x721090);
    RH_ScopedInstall(User3dMarkerDeleteAll, 0x7210B0);
    RH_ScopedInstall(User3dMarkerFindFirstFreeSlot, 0x720FB0);
    RH_ScopedInstall(User3dMarkerSet, 0x720FD0);
    RH_ScopedInstall(User3dMarkersDraw, 0x723240);
    RH_ScopedInstall(LoadUser3dMarkers, 0x5D42E0, {.enabled = LOAD_HOOKS_ENABLED });
    RH_ScopedInstall(SaveUser3dMarkers, 0x5D4300, {.enabled = SAVE_HOOKS_ENABLED });
}

// 0x724E40
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

// 0x7227B0
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

// 0x722710
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

// 0x725040
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
                if (marker.m_fCameraRange < 150.0f || IgnoreRenderLimit || marker.m_nType == MARKER3D_TORUS) {
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

// Returns slot index; -1 if not found
// 0x721120
int32 C3dMarkers::DirectionArrowFindFirstFreeSlot() {
    for (const auto& [index, arrow] : notsa::enumerate(ms_directionArrows)) {
        if (!arrow.m_bIsUsed) {
            return index;
        }
    }
    return -1;
}

// 0x721140
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

// 0x7211F0
void C3dMarkers::DirectionArrowsDraw() {
    auto bRenderParamsSet = false;

    for (auto& arrow : ms_directionArrows) {
        if (!arrow.m_bIsUsed) {
            continue;
        }

        if (!bRenderParamsSet) {
            RwRenderStateGet(rwRENDERSTATECULLMODE,          &gStoredRenderStateCullMode);
            RwRenderStateGet(rwRENDERSTATEZTESTENABLE,       &gStoredRenderStateZTestEnable);
            RwRenderStateGet(rwRENDERSTATEZWRITEENABLE,      &gStoredRenderStateZWriteEnable);
            RwRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, &gStoredRenderStateVertexAlphaEnable);

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
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(gStoredRenderStateCullMode));
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(gStoredRenderStateZTestEnable));
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(gStoredRenderStateZWriteEnable));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(gStoredRenderStateVertexAlphaEnable));
    }
}

// 0x721100
void C3dMarkers::DirectionArrowsInit() {
    for (auto& arrow : ms_directionArrows) {
        arrow.m_bIsUsed = false;
    }
}

// 0x722870
void C3dMarkers::ForceRender(bool ignore) {
    IgnoreRenderLimit = ignore;
}

// 0x722810
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

// 0x725120
C3dMarker* C3dMarkers::PlaceMarker(uint32 id, e3dMarkerType type, CVector& posn, float size, uint8 red, uint8 green, uint8 blue, uint8 alpha, uint16 pulsePeriod, float pulseFraction, int16 rotateRate, float nrm_x, float nrm_y, float nrm_z, bool zCheck) {
    return plugin::CallAndReturn<C3dMarker*, 0x725120, uint32, e3dMarkerType, CVector&, float, uint8, uint8, uint8, uint8, uint16, float, int16, float, float, float, bool>(id, type, posn, size, red, green, blue, alpha, pulsePeriod, pulseFraction, rotateRate, nrm_x, nrm_y, nrm_z, zCheck);
}

// 0x726D40
void C3dMarkers::PlaceMarkerCone(uint32 id, CVector& point, float size, uint8 red, uint8 green, uint8 blue, uint8 alpha, uint16 pulsePeriod, float pulseFraction, int16 rotateRate, bool bEnableCollision) {
    if ((point - TheCamera.GetPosition()).SquaredMagnitude() >= sq(1.6f)) {
        PlaceMarker(id, bEnableCollision ? MARKER3D_CONE : MARKER3D_CONE_NO_COLLISION, point, size, red, green, blue, m_colDiamond, pulsePeriod, pulseFraction, 0, 0.0f, 0.0f, 0.0f, false);
    }
}

// 0x725BA0
void C3dMarkers::PlaceMarkerSet(uint32 id, e3dMarkerType type, CVector& posn, float size, uint8 red, uint8 green, uint8 blue, uint8 alpha, uint16 pulsePeriod, float pulseFraction, int16 rotateRate) {
    PlaceMarker(id, type, posn, size, red, green, blue, static_cast<uint8>((float)alpha * 1.0f / 3.0f), pulsePeriod, pulseFraction, 1, 0.0f, 0.0f, 0.0f, false);
}

// 0x7210D0
// only set material color (m_user3dMarkerColor) for first material in first atomic; 'data' is unused
RpAtomic* C3dMarkers::User3dMarkerAtomicCB(RpAtomic* atomic, void*) {
    const auto color = m_user3dMarkerColor.ToRwRGBA();
    RpMaterialSetColor(RpGeometryGetMaterial(RpAtomicGetGeometry(atomic), 0), &color);
    return nullptr;
}

// 0x721090
void C3dMarkers::User3dMarkerDelete(int32 slotIndex) {
    if (slotIndex >= 0 && slotIndex <= (int32)(ms_user3dMarkers.size() - 1)) {
        ms_user3dMarkers[slotIndex].m_bIsUsed = false;
    }
}

// 0x7210B0
void C3dMarkers::User3dMarkerDeleteAll() {
    for (auto& marker : ms_user3dMarkers) {
        marker.m_bIsUsed = false;
    }
}

// Returns slot index; -1 if not found
// 0x720FB0
int32 C3dMarkers::User3dMarkerFindFirstFreeSlot() {
    for (const auto& [index, marker] : notsa::enumerate(ms_user3dMarkers)) {
        if (!marker.m_bIsUsed) {
            return index;
        }
    }
    return -1;
}

// Returns slot index; -1 if not created; for 'color', see eHudColours
// 0x720FD0
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

// 0x723240
void C3dMarkers::User3dMarkersDraw() {
    bool bRenderParamsSet = false;

    for (auto& marker : ms_user3dMarkers) {
        if (!marker.m_bIsUsed) {
            continue;
        }

        if (!bRenderParamsSet) {
            RwRenderStateGet(rwRENDERSTATECULLMODE, &gStoredRenderStateCullMode);
            RwRenderStateGet(rwRENDERSTATEZTESTENABLE, &gStoredRenderStateZTestEnable);
            RwRenderStateGet(rwRENDERSTATEZWRITEENABLE, &gStoredRenderStateZWriteEnable);

            RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));

            bRenderParamsSet = true;
        }

        marker.Render(m_pRpClumpArray[MARKER3D_CONE]);
    }

    if (bRenderParamsSet) {
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(gStoredRenderStateCullMode));
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(gStoredRenderStateZTestEnable));
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(gStoredRenderStateZWriteEnable));
    }
}

// 0x5D42E0
bool C3dMarkers::LoadUser3dMarkers() {
    for (auto& marker : ms_user3dMarkers) {
        CGenericGameStorage::LoadDataFromWorkBuffer_Org(&marker, sizeof(marker));
    }
    return true;
}

// 0x5D4300
bool C3dMarkers::SaveUser3dMarkers() {
    for (auto& marker : ms_user3dMarkers) {
        CGenericGameStorage::SaveDataToWorkBuffer(&marker, sizeof(marker));
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
