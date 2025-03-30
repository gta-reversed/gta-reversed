/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"
#include <imgui.h>
#include "Shadows.h"
#include "FireManager.h"
#include <CustomBuildingDNPipeline.h>

void CShadows::InjectHooks() {
    RH_ScopedClass(CShadows);
    RH_ScopedCategory("Shadows");

    RH_ScopedInstall(Init, 0x706CD0);
    RH_ScopedInstall(Shutdown, 0x706ED0);
    RH_ScopedInstall(TidyUpShadows, 0x707770);
    RH_ScopedInstall(AddPermanentShadow, 0x706F60, { .reversed = false });
    RH_ScopedInstall(UpdatePermanentShadows, 0x70C950, { .reversed = false });
    RH_ScopedOverloadedInstall(StoreShadowToBeRendered, "Texture", 0x707390, void(*)(uint8, RwTexture*, const CVector&, float, float, float, float, int16, uint8, uint8, uint8, float, bool, float, CRealTimeShadow*, bool));
    RH_ScopedOverloadedInstall(StoreShadowToBeRendered, "Type", 0x707930, void(*)(uint8, const CVector&, float, float, float, float, int16, uint8, uint8, uint8));
    RH_ScopedInstall(SetRenderModeForShadowType, 0x707460);
    RH_ScopedInstall(RemoveOilInArea, 0x7074F0);
    RH_ScopedInstall(GunShotSetsOilOnFire, 0x707550);
    RH_ScopedInstall(PrintDebugPoly, 0x7076B0);
    RH_ScopedInstall(CalcPedShadowValues, 0x7076C0);
    RH_ScopedInstall(AffectColourWithLighting, 0x707850);
    RH_ScopedInstall(StoreShadowForPedObject, 0x707B40);
    RH_ScopedInstall(StoreRealTimeShadow, 0x707CA0);
    RH_ScopedInstall(UpdateStaticShadows, 0x707F40);
    RH_ScopedInstall(RenderExtraPlayerShadows, 0x707FA0);
    RH_ScopedInstall(RenderStaticShadows, 0x708300);
    RH_ScopedInstall(CastShadowEntityXY, 0x7086B0, { .reversed = false });
    RH_ScopedInstall(CastShadowEntityXYZ, 0x70A040, { .reversed = false });
    RH_ScopedInstall(CastPlayerShadowSectorList, 0x70A470);
    RH_ScopedInstall(CastShadowSectorList, 0x70A630, { .reversed = false });
    RH_ScopedInstall(CastRealTimeShadowSectorList, 0x70A7E0, { .reversed = false });
    RH_ScopedInstall(RenderStoredShadows, 0x70A960);
    RH_ScopedInstall(GeneratePolysForStaticShadow, 0x70B730, { .reversed = false });
    RH_ScopedInstall(StoreStaticShadow, 0x70BA00, { .reversed = false });
    RH_ScopedInstall(StoreShadowForVehicle, 0x70BDA0, { .reversed = false });
    RH_ScopedInstall(StoreCarLightShadow, 0x70C500);
    RH_ScopedInstall(StoreShadowForPole, 0x70C750);
    RH_ScopedInstall(RenderIndicatorShadow, 0x70CCB0);
    RH_ScopedGlobalInstall(ShadowRenderTriangleCB, 0x709CF0, { .reversed = false });
}

void CStaticShadow::InjectHooks() {
    RH_ScopedClass(CStaticShadow);
    RH_ScopedCategory("Shadows");

    RH_ScopedInstall(Free, 0x707670);
    //RH_ScopedInstall(Init, 0x0);
}

// 0x707670 
void CStaticShadow::Free() {
    if (m_pPolyBunch) {
        const auto prevHead = CShadows::pEmptyBunchList;
        CShadows::pEmptyBunchList = m_pPolyBunch;

        // Find last in the list and make it point to the previous head
        auto it{ m_pPolyBunch };
        while (it->m_pNext) {
            it = static_cast<CPolyBunch*>(it->m_pNext);
        }
        it->m_pNext = prevHead;

        m_pPolyBunch = nullptr;
        m_nId = 0;
    }
}

// 0x706CD0
void CShadows::Init() {
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));

    gpShadowCarTex         = RwTextureRead("shad_car",     nullptr);
    gpShadowPedTex         = RwTextureRead("shad_ped",     nullptr);
    gpShadowHeliTex        = RwTextureRead("shad_heli",    nullptr);
    gpShadowBikeTex        = RwTextureRead("shad_bike",    nullptr);
    gpShadowBaronTex       = RwTextureRead("shad_rcbaron", nullptr);
    gpShadowExplosionTex   = RwTextureRead("shad_exp",     nullptr);
    gpShadowHeadLightsTex  = RwTextureRead("headlight",    nullptr);
    gpShadowHeadLightsTex2 = RwTextureRead("headlight1",   nullptr);
    gpBloodPoolTex         = RwTextureRead("bloodpool_64", nullptr);
    gpHandManTex           = RwTextureRead("handman",      nullptr);
    gpCrackedGlassTex      = RwTextureRead("wincrack_32",  nullptr);
    gpPostShadowTex        = RwTextureRead("lamp_shad_64", nullptr);

    CTxdStore::PopCurrentTxd();

    g_ShadowVertices = { 0, 2, 1, 0, 3, 2, 0, 4, 3, 0, 5, 4, 0, 6, 5, 0, 7, 6, 0, 8, 7, 0, 9, 8 };

    std::ranges::for_each(aStaticShadows, [&](auto& shadow) { shadow.Init(); });

    pEmptyBunchList = aPolyBunches.data();

    for (auto i = 0u; i < aPolyBunches.size() - 1; i++) {
        aPolyBunches[i].m_pNext = &aPolyBunches[i + 1];
    }
    aPolyBunches.back().m_pNext = nullptr;

    std::ranges::for_each(aPermanentShadows, [&](auto& shadow) { shadow.Init(); });
}

// 0x706ED0
void CShadows::Shutdown() {
    RwTextureDestroy(gpShadowCarTex);
    RwTextureDestroy(gpShadowPedTex);
    RwTextureDestroy(gpShadowHeliTex);
    RwTextureDestroy(gpShadowBikeTex);
    RwTextureDestroy(gpShadowBaronTex);
    RwTextureDestroy(gpShadowExplosionTex);
    RwTextureDestroy(gpShadowHeadLightsTex);
    RwTextureDestroy(gpShadowHeadLightsTex2);
    RwTextureDestroy(gpBloodPoolTex);
    RwTextureDestroy(gpHandManTex);
    RwTextureDestroy(gpCrackedGlassTex);
    RwTextureDestroy(gpPostShadowTex);
}

// 0x707770
void CShadows::TidyUpShadows() {
    std::ranges::for_each(
        aPermanentShadows,
        [&](auto& shadow) { shadow.m_nType = SHADOW_NONE; }
    );
}

// 0x706F60
void CShadows::AddPermanentShadow(uint8 type, RwTexture* texture, CVector* posn, float topX, float topY, float rightX, float rightY, int16 intensity, uint8 red, uint8 greeb, uint8 blue, float drawDistance, uint32 time, float upDistance) {
    ((void(__cdecl*)(uint8, RwTexture*, CVector*, float, float, float, float, int16, uint8, uint8, uint8, float, uint32, float))0x706F60)(type, texture, posn, topX, topY, rightX, rightY, intensity, red, greeb, blue, drawDistance, time, upDistance);
}

// 0x70C950
void CShadows::UpdatePermanentShadows() {
    ZoneScoped;

    ((void(__cdecl*)())0x70C950)();
}

// 0x707930
void CShadows::StoreShadowToBeRendered(uint8 type, const CVector& posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue) {
    const auto Store = [=](auto mtype, auto texture) {
        StoreShadowToBeRendered(mtype, texture, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, 15.0f, 0, 1.0f, nullptr, 0);
    };

    switch (type) {
    case SHADOW_DEFAULT:
        Store(SHADOW_TEX_CAR, gpShadowCarTex);
        break;
    case SHADOW_ADDITIVE:
        Store(SHADOW_TEX_CAR, gpShadowPedTex);
        break;
    case SHADOW_INVCOLOR:
        Store(SHADOW_TEX_PED, gpShadowExplosionTex);
        break;
    case SHADOW_OIL_1:
        Store(SHADOW_TEX_CAR, gpShadowHeliTex);
        break;
    case SHADOW_OIL_2:
        Store(SHADOW_TEX_PED, gpShadowHeadLightsTex);
        break;
    case SHADOW_OIL_3:
        Store(SHADOW_TEX_CAR, gpBloodPoolTex);
        break;
    default:
        return;
    }
}

// 0x707390
void CShadows::StoreShadowToBeRendered(uint8 type, RwTexture* texture, const CVector& posn, float topX, float topY, float rightX, float rightY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, bool drawOnWater, float scale, CRealTimeShadow* realTimeShadow, bool drawOnBuildings) {
    if (ShadowsStoredToBeRendered >= asShadowsStored.size())
        return;

    auto& shadow = asShadowsStored[ShadowsStoredToBeRendered];

    shadow.m_nType      = (eShadowType)type;
    shadow.m_pTexture   = texture;
    shadow.m_vecPosn    = posn;
    shadow.m_Front.x    = topX;
    shadow.m_Front.y    = topY;
    shadow.m_Side.x     = rightX;
    shadow.m_Side.y     = rightY;
    shadow.m_nIntensity = intensity;
    shadow.m_nRed       = red;
    shadow.m_nGreen     = green;
    shadow.m_nBlue      = blue;
    shadow.m_fZDistance = zDistance;
    shadow.m_bDrawOnWater     = drawOnWater;
    shadow.m_bDrawOnBuildings = drawOnBuildings;
    shadow.m_fScale     = scale;
    shadow.m_pRTShadow  = realTimeShadow;

    ShadowsStoredToBeRendered++;
}

void CShadows::StoreShadowToBeRendered(eShadowType type, RwTexture* tex, const CVector& posn, CVector2D top, CVector2D right, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, bool drawOnWater, float scale, CRealTimeShadow* realTimeShadow, bool drawOnBuildings) {
    StoreShadowToBeRendered(
        type,
        tex,
        posn,
        top.x, top.y,
        right.x, right.y,
        intensity,
        red, green, blue,
        zDistance,
        drawOnWater,
        scale,
        realTimeShadow,
        drawOnBuildings
    );
}

// 0x707460
void CShadows::SetRenderModeForShadowType(eShadowType type) {
    switch (type) {
    case SHADOW_DEFAULT:
    case SHADOW_OIL_1:
    case SHADOW_OIL_2:
    case SHADOW_OIL_3:
    /* case SHADOW_OIL_4: */ // missing
    case SHADOW_OIL_5:
        RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
        break;
    case SHADOW_ADDITIVE:
        RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDONE));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));
        break;
    case SHADOW_INVCOLOR:
        RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDZERO));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCCOLOR));
        break;
    default:
        return;
    }
}

// 0x7074F0
void CShadows::RemoveOilInArea(float x1, float x2, float y1, float y2) {
    CRect rect{ {x1, y1}, {x2, y2} };
    for (auto& shadow : aPermanentShadows) {
        switch (shadow.m_nType) {
        case SHADOW_OIL_1:
        case SHADOW_OIL_5:
            break;
        default:
            continue;
        }
        if (rect.IsPointInside(shadow.m_vecPosn)) {
            shadow.m_nType = SHADOW_NONE;
        }
    }
}

// 0x707550
void CShadows::GunShotSetsOilOnFire(const CVector& shotOrigin, const CVector& shotTarget) {
    const auto V3DChangeZ = [] (const CVector& pos) {
        return CVector{ pos.x, pos.y, pos.z * 0.27f };
    };

    const CColLine line{V3DChangeZ(shotOrigin), V3DChangeZ(shotTarget)};

    // Find closest shadow (but not further than 1 unit)
    CPermanentShadow* closest{};
    auto              closestDist{1.f};
    for (auto& shdw : aPermanentShadows) {
        switch (shdw.m_nType) {
        case SHADOW_OIL_1:
        case SHADOW_OIL_5:
            break;
        default:
            continue;
        }

        const auto dist = line.DistTo(V3DChangeZ(shdw.m_vecPosn));
        if (dist < closestDist) {
            closestDist = dist;
            closest     = &shdw;
        }
    }

    if (closest) {
        closest->m_nType = SHADOW_OIL_2;
        gFireManager.StartFire(closest->m_vecPosn, 1.8f, 0, nullptr, 2000u, 0, 1);
    }
}

// 0x7076B0
void CShadows::PrintDebugPoly(CVector* a, CVector* b, CVector* c) {
    // NOP
}

// 0x7076C0
void CShadows::CalcPedShadowValues(CVector sunPosn, float& displacementX, float& displacementY, float& frontX, float& frontY, float& sideX, float& sideY) {
    const auto sunDist = sunPosn.Magnitude2D();
    const auto recip = 1.0f / sunDist;
    const auto mult = (sunDist + 1.0f) * recip;

    displacementX = -sunPosn.x * mult / 2.0f;
    displacementY = -sunPosn.y * mult / 2.0f;

    frontX = -sunPosn.y * recip / 2.0f;
    frontY = +sunPosn.x * recip / 2.0f;

    sideX = -sunPosn.x / 2.0f;
    sideY = -sunPosn.y / 2.0f;
}

// 0x707850
void CShadows::AffectColourWithLighting(
    eShadowType shadowType,
    uint8 dayNightIntensity, // packed 2x4 bits for day/night
    uint8 r, uint8 g, uint8 b,
    uint8& outR, uint8& outG, uint8& outB
) {
    if (shadowType != SHADOW_ADDITIVE) {
        const auto mult = std::min(
            0.4f + 0.6f * (1.f - CCustomBuildingDNPipeline::m_fDNBalanceParam),
            0.3f + 0.7f * lerp(
                (float)(dayNightIntensity >> 0 & 0b1111) / 30.f,
                (float)(dayNightIntensity >> 4 & 0b1111) / 30.f,
                CCustomBuildingDNPipeline::m_fDNBalanceParam
            )
        );
        outR = (uint8)((float)r * mult);
        outG = (uint8)((float)g * mult);
        outB = (uint8)((float)b * mult);
    } else {
        outR = r;
        outG = g;
        outB = b;
    }
}

// NOTSA
uint16 CalculateShadowStrength(float currDist, float maxDist, uint16 maxStrength) {
    assert(maxDist >= currDist); // Otherwise integer underflow will occur

    const auto halfMaxDist = maxDist / 2.f;
    if (currDist >= halfMaxDist) { // Anything further than half the distance is faded out
        return (uint16)((1.f - (currDist - halfMaxDist) / halfMaxDist) * maxStrength);
    } else { // Anything closer than half the max distance is full strength
        return (uint16)maxStrength;
    }
}

// 0x707B40
void CShadows::StoreShadowForPedObject(CPed* ped, float displacementX, float displacementY, float frontX, float frontY, float sideX, float sideY) {
    // Okay, so.
    // This function is called from `CCutsceneObject::PreRender`
    // And you might ask "what the fuck, an object is not a ped!!"
    // well, in R* world it is.
    // it has bones (as anything can have bones actually, that's a known fact)
    // the `GetBonePosition` below *just works* because it doesn't access any
    // `CPed` specific member variables.
    // But we really should fix this in a sensible way in the future.
    assert(ped->IsPed() || ped->IsObject());

    const auto  bonePos           = ped->GetBonePosition(BONE_ROOT);
    const auto& camPos            = TheCamera.GetPosition();
    const auto  boneToCamDist2DSq = (bonePos - camPos).SquaredMagnitude2D();

    // Check if ped is close enough
    if (boneToCamDist2DSq >= MAX_DISTANCE_PED_SHADOWS_SQR) {
        return;
    }

    const auto isPlayerPed = FindPlayerPed() == ped;

    // Check if ped is visible to the camera
    if (!isPlayerPed) {  // Optimization: Assume player ped is always visible
        if (!TheCamera.IsSphereVisible(ped->GetPosition(), 2.f)) {
            return;
        }
    }

    // Now store a shadow to be rendered
    const auto strength = (uint8)CalculateShadowStrength(std::sqrt(boneToCamDist2DSq), MAX_DISTANCE_PED_SHADOWS, CTimeCycle::m_CurrentColours.m_nShadowStrength);
    StoreShadowToBeRendered(
        SHADOW_DEFAULT,
        gpShadowPedTex,
        bonePos + CVector{displacementX, displacementY, 0.f},
        frontX, frontY,
        sideX, sideY,
        strength,
        strength, strength, strength,
        4.f,
        false,
        1.f,
        nullptr,
        isPlayerPed || g_fx.GetFxQuality() >= FX_QUALITY_VERY_HIGH // NOTSA: At higher FX quality draw all ped's shadows
    );
}

// 0x707CA0
void CShadows::StoreRealTimeShadow(CPhysical* physical, float displacementX, float displacementY, float frontX, float frontY, float sideX, float sideY) {
    const auto rtshdw = physical->m_pShadowData;
    if (!rtshdw) {
        return;
    }
    const auto& camPos = TheCamera.GetPosition();
    const auto  shdwPos = physical->IsPed()
        ? physical->AsPed()->GetBonePosition(BONE_ROOT)
        : physical->GetPosition();
    const auto shdwToCamDist2DSq = (shdwPos - camPos).SquaredMagnitude2D();

    // Check distance to camera
    if (shdwToCamDist2DSq > MAX_DISTANCE_PED_SHADOWS_SQR) {
        return;
    }

    // Check if the object is visible to the camera
    if (FindPlayerPed() != physical) {  // Optimization: Assume player ped is always visible
        if (!TheCamera.IsSphereVisible(shdwPos, 2.f)) {
            return;
        }
    }

    const auto strength = (float)CalculateShadowStrength(std::sqrt(shdwToCamDist2DSq), MAX_DISTANCE_PED_SHADOWS, CTimeCycle::m_CurrentColours.m_nShadowStrength);
    const auto cc       = (uint8)((float)rtshdw->m_nIntensity / 100.f * strength);

    const auto& vecToSun = CTimeCycle::m_VectorToSun[CTimeCycle::m_CurrentStoredValue];
    const auto lightFrame = rtshdw->SetLightProperties(
        RWRAD2DEG(+std::atan2(-vecToSun.x, -vecToSun.y)),
        RWRAD2DEG(-std::atan2(+vecToSun.x, -vecToSun.z)),
        true
    );
    CalcPedShadowValues(
        *RwMatrixGetAt(RwFrameGetMatrix(lightFrame)),
        displacementX, displacementY,
        frontX, frontY,
        sideX, sideY
    );
    StoreShadowToBeRendered(
        SHADOW_INVCOLOR,
        rtshdw->GetShadowRwTexture(),
        shdwPos - CVector{ displacementX, displacementY, 0.f } * 2.5f,
        frontX * 1.5f, frontY * 1.5f,
        sideX * 1.5f, sideY * 1.5f,
        cc,
        cc, cc, cc,
        4.f,
        false,
        1.f,
        rtshdw,
        g_fx.GetFxQuality() >= FX_QUALITY_VERY_HIGH // NOTSA: At higher FX quality draw all shadows on buildings too
    );
}

// 0x707F40
void CShadows::UpdateStaticShadows() {
    // Remove shadows that have no polies/are temporary and have expired
    for (auto& sshdw : aStaticShadows) {
        if (!sshdw.m_pPolyBunch || sshdw.m_bJustCreated) {
            goto skip; // Not even created fully
        }

        if (sshdw.m_bTemporaryShadow && CTimer::GetTimeInMS() <= sshdw.m_nTimeCreated + 5000u) {
            goto skip; // Not expired yet
        }

        sshdw.Free();

    skip:
        sshdw.m_bJustCreated = false;
    }
}

// 0x707FA0
void CShadows::RenderExtraPlayerShadows() {
    if (!CTimeCycle::m_CurrentColours.m_nShadowStrength) {
        return;
    }

    const auto plyrVeh = FindPlayerVehicle();
    if (!plyrVeh) {
        return;
    }

    if (plyrVeh->m_nModelIndex == eModelID::MODEL_RCBANDIT) {
        return;
    }

    if (plyrVeh->GetVehicleAppearance() == VEHICLE_APPEARANCE_BIKE) {
        return;
    }

    switch (plyrVeh->m_nVehicleType) {
    case VEHICLE_TYPE_BIKE:
    case VEHICLE_TYPE_FPLANE:
    case VEHICLE_TYPE_BOAT:
        return;
    }

    const auto  plyrPos    = FindPlayerCoors();
    const auto& plyrVehPos = plyrVeh->GetPosition();
    const auto& plyVehMat  = plyrVeh->GetMatrix();
    for (auto& ptl : CPointLights::GetActiveLights()) {
        if (ptl.m_nType != ePointLightType::PLTYPE_POINTLIGHT) {
            continue;
        }
        if (!ptl.m_bGenerateShadows) {
            continue;
        }
        if (ptl.m_fColorRed == 0.f && ptl.m_fColorGreen == 0.f && ptl.m_fColorBlue == 0.f) { // If it's black => ignore
            continue;
        }

        const auto lightToPlyr{ ptl.m_vecPosn - plyrPos };
        const auto lightToPlyrDist = lightToPlyr.Magnitude();
        if (lightToPlyrDist >= ptl.m_fRadius || lightToPlyrDist == 0.f) { // NOTSA: Zero check to prevent (possible) division-by-zero
            continue;
        }

        const auto lightToPlyrDir  = lightToPlyr / lightToPlyrDist; // Normalize vector

        const auto& plyrVehBB = plyrVeh->GetColModel()->GetBoundingBox();
        const auto  shdwSize  = CVector2D{ plyrVehBB.GetSize() } / 2.f;

        StoreShadowToBeRendered(
            SHADOW_DEFAULT,
            gpShadowCarTex,
            plyrVehPos - CVector{
                  CVector2D{ lightToPlyr / lightToPlyrDist } * 1.2f // Compensate for elvation of the light
                - CVector2D{ plyVehMat.GetForward() } * (shdwSize.y - plyrVehBB.m_vecMax.y) // Move point to center (as the shadow's position is it's center)
            },
            CVector2D{ plyVehMat.GetForward() } * shdwSize.y,
            CVector2D{ plyVehMat.GetRight() } * (plyVehMat.GetUp().z >= 0.f ? shdwSize.x : -shdwSize.x), // If vehicle is flipped, we gotta flip the `right` vector back
            CalculateShadowStrength( // 0x70809A
                lightToPlyrDist,
                ptl.m_fRadius,
                5 * CTimeCycle::m_CurrentColours.m_nShadowStrength / 8u // Same as mult by `0.625` and then casting to int
            ),
            0, 0, 0,
            4.5f,
            false,
            1.f,
            nullptr,
            false
        );
    }
}

// 0x708300
void CShadows::RenderStaticShadows() {
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,        RWRSTATE(rwFILTERLINEAR));

    RenderBuffer::ClearRenderBuffer();

    // Mark all as not-yet-rendered
    for (auto& shdw : aStaticShadows) {
        shdw.m_bRendered = false;
    }

    // Render all in batches
    for (auto& oshdw : aStaticShadows) {
        if (!oshdw.m_pPolyBunch || oshdw.m_bRendered) {
            continue;
        }

        // Setup additional render states for this shadow
        SetRenderModeForShadowType(oshdw.m_nType);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(oshdw.m_pTexture)));
         
        // Batch all other shadows with the same texture and type into the buffer
        for (auto& ishdw : aStaticShadows) {
            if (!ishdw.m_pPolyBunch) {
                continue;
            }
            if (ishdw.m_nType != oshdw.m_nType) {
                continue;
            }
            if (ishdw.m_pTexture != oshdw.m_pTexture) {
                continue;
            }
            // No need to check if this one was rendered (because of the batching)
            
            // Render polies of this shadow
            for (auto poly = ishdw.m_pPolyBunch; poly; poly = poly->m_pNext) {
                // 0x70841F: Calculate color
                uint8 r, g, b;
                CShadows::AffectColourWithLighting(
                    ishdw.m_nType,
                    ishdw.m_nDayNightIntensity,
                    ishdw.m_nRed, ishdw.m_nGreen, ishdw.m_nBlue,
                    r, g, b
                );

                const auto totalNoIdx = 3 * (poly->m_wNumVerts - 2); // Total no. of indices we'll use

                // 0x708432: Begin render buffer store
                RwIm3DVertex*    vtxIt{};
                RwImVertexIndex* vtxIdxIt{};
                RenderBuffer::StartStoring(
                    totalNoIdx,
                    poly->m_wNumVerts,
                    vtxIdxIt, vtxIt
                );

                // 0x70851D: Write vertices (`if` not necessary, it's part of the loop condition)
                const auto a = (uint8)((float)ishdw.m_nIntensity * (1.f - CWeather::Foggyness * 0.5f));
                for (auto i{ 0 }; i < poly->m_wNumVerts; i++, vtxIt++) {
                    const auto& pos = poly->m_avecPosn[i];
                    RwIm3DVertexSetPos(vtxIt, pos.x, pos.y, pos.z + 0.06f);
                    RwIm3DVertexSetRGBA(vtxIt, r, g, b, a);
                    RwIm3DVertexSetU(vtxIt, (float)poly->m_aU[i] / 200.f);
                    RwIm3DVertexSetV(vtxIt, (float)poly->m_aV[i] / 200.f);
                }

                // 0x7085BC: Write indices  (`if` not necessary, it's part of the loop condition)
                for (auto i = 0; i < totalNoIdx; i++) {
                    *vtxIdxIt++ = g_ShadowVertices[i];
                }
                
                // Finish storing 
                RenderBuffer::StopStoring();

                // Mark this as rendered
                ishdw.m_bRendered = true;
            }
        }

        // Render out this batch
        RenderBuffer::RenderStuffInBuffer();
    }

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
}

// 0x7086B0
void CShadows::CastShadowEntityXY(CEntity* entity, float conrerAX, float cornerAY, float cornerBX, float cornerBY, CVector* posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, float scale, CPolyBunch** ppPolyBunch, uint8* pDayNightIntensity, int32 shadowType) {
    ((void(__cdecl*)(CEntity*, float, float, float, float, CVector*, float, float, float, float, int16, uint8, uint8, uint8, float, float, CPolyBunch**, uint8*, int32))0x7086B0)(entity, conrerAX, cornerAY, cornerBX, cornerBY, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, zDistance, scale, ppPolyBunch, pDayNightIntensity, shadowType);
}

// 0x70A040
void CShadows::CastShadowEntityXYZ(CEntity* entity, CVector* posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, float scale, CPolyBunch** ppPolyBunch, CRealTimeShadow* realTimeShadow) {
    ((void(__cdecl*)(CEntity*, CVector*, float, float, float, float, int16, uint8, uint8, uint8, float, float, CPolyBunch**, CRealTimeShadow*))0x70A040)(entity, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, zDistance, scale, ppPolyBunch, realTimeShadow);
}

// 0x70A470
void CShadows::CastPlayerShadowSectorList(
    CPtrList& ptrList,
    float cornerAX,
    float cornerAY,
    float cornerBX,
    float cornerBY,
    CVector* posn,
    float frontX,
    float frontY,
    float sideX,
    float sideY,
    int16 intensity,
    uint8 red,
    uint8 green,
    uint8 blue,
    float zDistance,
    float scale,
    CPolyBunch** ppPolyBunch,
    uint8* pDayNightIntensity,
    int32 shadowType
) {
    const CRect shadowRect{
        cornerAX, cornerAY,
        cornerBX, cornerBY
    };
    for (CPtrNode* it = ptrList.m_node, *next{}; it; it = next) {
        next = it->GetNext();

        auto* entity = reinterpret_cast<CEntity*>(it->m_item);

        if (entity->IsScanCodeCurrent()) {
            continue;
        }
        entity->SetCurrentScanCode();

        if (!entity->m_bUsesCollision || entity->m_bDontCastShadowsOn) {
            continue;
        }

        if (!entity->IsInCurrentAreaOrBarberShopInterior()) {
            continue;
        }

        // If slightly tilted, ignore
        if (entity->GetMatrix().GetUp().z <= 0.97f) {
            continue;
        }

        // 0x70A526
        if (!entity->GetBoundRect().OverlapsWith(shadowRect)) {
            continue;
        }

        // Quick Z height check of the bounding box
        const auto& cm         = entity->GetColModel();
        const auto  entityPosZ = entity->GetPosition().z;
        if (cm->m_boundBox.m_vecMax.z + entityPosZ <= posn->z - zDistance) {
            continue;
        }
        if (cm->m_boundBox.m_vecMin.z + entityPosZ >= posn->z) {
            continue;
        }

        CastShadowEntityXY(
            entity,
            cornerAX,
            cornerAY,
            cornerBX,
            cornerBY,
            posn,
            frontX,
            frontY,
            sideX,
            sideY,
            intensity,
            red,
            green,
            blue,
            zDistance,
            scale,
            ppPolyBunch,
            pDayNightIntensity,
            shadowType
        );
    }
}

// 0x70A630
void CShadows::CastShadowSectorList(CPtrList& ptrList, float conrerAX, float cornerAY, float cornerBX, float cornerBY, CVector* posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, float scale, CPolyBunch** ppPolyBunch, uint8* pDayNightIntensity, int32 shadowType) {
    // Nearly identical to `CastPlayerShadowSectorList`, the difference is 1 check is missing... :D
    ((void(__cdecl*)(CPtrList&, float, float, float, float, CVector*, float, float, float, float, int16, uint8, uint8, uint8, float, float, CPolyBunch**, uint8*, int32))0x70A630)(ptrList, conrerAX, cornerAY, cornerBX, cornerBY, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, zDistance, scale, ppPolyBunch, pDayNightIntensity, shadowType);
}

// 0x70A7E0
void CShadows::CastRealTimeShadowSectorList(CPtrList& ptrList, float conrerAX, float cornerAY, float cornerBX, float cornerBY, CVector* posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, float scale, CPolyBunch** ppPolyBunch, CRealTimeShadow* realTimeShadow, uint8* pDayNightIntensity) {
    ((void(__cdecl*)(CPtrList&, float, float, float, float, CVector*, float, float, float, float, int16, uint8, uint8, uint8, float, float, CPolyBunch**, CRealTimeShadow*, uint8*))0x70A7E0)(ptrList, conrerAX, cornerAY, cornerBX, cornerBY, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, zDistance, scale, ppPolyBunch, realTimeShadow, pDayNightIntensity);
}

// 0x70A960
void CShadows::RenderStoredShadows() {
    // Originally renderstates are still set even though there are no shadows to be rendered
    // I don't think this is necessary, so we early-out here
    if (!CShadows::ShadowsStoredToBeRendered) {
        return;
    }

    RenderBuffer::ClearRenderBuffer();

    for (auto i = 0; i < ShadowsStoredToBeRendered; i++) {
        asShadowsStored[i].m_bAlreadyRenderedInBatch = false;
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(rwRENDERSTATENARENDERSTATE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,       RWRSTATE(rwTEXTUREADDRESSCLAMP));
    RwRenderStateSet(rwRENDERSTATECULLMODE,             RWRSTATE(rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,        RWRSTATE(rwFILTERLINEAR));

    const auto GetShadowRect = [](CRegisteredShadow& shdw) {
        return CRect{
            shdw.m_vecPosn,
            shdw.m_vecPosn + abs(shdw.m_Side) + abs(shdw.m_Front)
        };
    };

    for (auto o = 0; o < ShadowsStoredToBeRendered; o++) {
        auto& oshdw = asShadowsStored[o];

        // Setup additional render states for this shadow (and others in the batch below)
        SetRenderModeForShadowType(oshdw.m_nType);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(oshdw.m_pTexture)));

        //
        // Render onto ground (If not already)
        //
        if (!oshdw.m_bAlreadyRenderedInBatch) {
            // We do a batched rendering here:
            // All shadows of the same type and texture are rendered together to save on drawcalls

            for (auto i = o; i < ShadowsStoredToBeRendered; i++) {
                auto& ishdw = asShadowsStored[i];

                if (&ishdw != &oshdw) {
                    if (ishdw.m_nType != oshdw.m_nType) {
                        continue;
                    }
                    if (ishdw.m_pTexture != oshdw.m_pTexture) {
                        continue;
                    }
                }

                CWorld::IncrementCurrentScanCode();

                const auto ishdwRect = GetShadowRect(ishdw);

                // Do shadow casting
                CWorld::IterateSectorsOverlappedByRect(
                    ishdwRect,
                    [&] (int32 x, int32 y) -> bool {
                        const auto sector = GetSector(x, y);

                        if (const auto rtshdw = ishdw.m_pRTShadow) {
                            CastRealTimeShadowSectorList(
                                sector->m_buildings,
                                ishdwRect.left, ishdwRect.bottom,
                                ishdwRect.right, ishdwRect.top,
                                &ishdw.m_vecPosn,
                                ishdw.m_Front.x, ishdw.m_Front.y,
                                ishdw.m_Side.x, ishdw.m_Side.y,
                                ishdw.m_nIntensity,
                                ishdw.m_nRed, ishdw.m_nGreen, ishdw.m_nBlue,
                                ishdw.m_fZDistance,
                                ishdw.m_fScale,
                                nullptr,
                                rtshdw,
                                nullptr // Unused
                            );
                        } else {
                            uint8 unused{};
                            if (ishdw.m_bDrawOnBuildings) {
                                CastShadowSectorList(
                                    sector->m_buildings,
                                    ishdwRect.left, ishdwRect.bottom,
                                    ishdwRect.right, ishdwRect.top,
                                    &ishdw.m_vecPosn,
                                    ishdw.m_Front.x, ishdw.m_Front.y,
                                    ishdw.m_Side.x, ishdw.m_Side.y,
                                    ishdw.m_nIntensity,
                                    ishdw.m_nRed, ishdw.m_nGreen, ishdw.m_nBlue,
                                    ishdw.m_fZDistance,
                                    ishdw.m_fScale,
                                    nullptr,
                                    &unused,
                                    ishdw.m_nType
                                );
                            } else {
                                CastPlayerShadowSectorList(
                                    sector->m_buildings,
                                    ishdwRect.left, ishdwRect.bottom,
                                    ishdwRect.right, ishdwRect.top,
                                    &ishdw.m_vecPosn,
                                    ishdw.m_Front.x, ishdw.m_Front.y,
                                    ishdw.m_Side.x, ishdw.m_Side.y,
                                    ishdw.m_nIntensity,
                                    ishdw.m_nRed, ishdw.m_nGreen, ishdw.m_nBlue,
                                    ishdw.m_fZDistance,
                                    ishdw.m_fScale,
                                    nullptr,
                                    &unused,
                                    ishdw.m_nType
                                );
                            }
                        }
                        return true; // Inisde lambda -> Continue sector loop
                    }
                );

                // Mark this as rendered
                ishdw.m_bAlreadyRenderedInBatch = true;
            }

            // Render out shadows (Can't batch together with next iteration, as renderstates change)
            RenderBuffer::RenderStuffInBuffer();
        }
        
        // Render onto water (If needed)
        if (oshdw.m_bDrawOnWater) {  
            float waterLevelZ{}, bigWavesZ{}, smallWavesZ{};
            if (!CWaterLevel::GetWaterLevelNoWaves(oshdw.m_vecPosn, &waterLevelZ, &bigWavesZ, &smallWavesZ)) {
                continue;
            }

            // Shadow under water?
            if (waterLevelZ >= oshdw.m_vecPosn.z) {
                continue;
            }

            // Clear the buffer as this is a new render pass (separate from the other)
            RenderBuffer::ClearRenderBuffer();

            // The smaller this value the detailed the shadows, but also (exponentially?) slower
            // This value basically represents the side of a square that is projected onto the 
            const auto STEP_SIZE = 2;

            const auto shdwSideMagSq = oshdw.m_Side.SquaredMagnitude();
            const auto shdwFrontMagSq = oshdw.m_Front.SquaredMagnitude();

            const auto oshdwRect = GetShadowRect(oshdw);

            // Iterate shadow sectors (Using `CWorld::IterateSectors` for convenience)
            CWorld::IterateSectors(
                (int32)(std::floor(oshdwRect.left / (float)STEP_SIZE)),
                (int32)(std::floor(oshdwRect.top / (float)STEP_SIZE)),
                (int32)(std::ceil(oshdwRect.right / (float)STEP_SIZE)),
                (int32)(std::ceil(oshdwRect.bottom / (float)STEP_SIZE)),
                [&](int32 ox, int32 oy) {
                    // 0x70B000 - 0x70B0DA: Start storing 
                    RwIm3DVertex* vtxIt{};
                    RwImVertexIndex* vtxIdxIt{};
                    RenderBuffer::StartStoring(6, 4, vtxIdxIt, vtxIt);

                    // Function to process 1 vertex (out of the 4)
                    const auto ProcessOneVertex = [&] (int32 x, int32 y) {
                        // The x, y passed in are sector coords, so make them into world coords
                        x *= STEP_SIZE;
                        y *= STEP_SIZE;

                        const auto currPos = CVector2D{ (float)x, (float)y };

                        // Set color
                        RwIm3DVertexSetRGBA(vtxIt, oshdw.m_nRed, oshdw.m_nGreen, oshdw.m_nBlue, (uint8)((float)oshdw.m_nIntensity * 0.6f));

                        // Set texture coords (UV)
                        const auto CalcTexCoord  = [
                            currPosToShdw = CVector2D{ currPos - oshdw.m_vecPosn }
                        ](CVector2D v, float vmagsq) {
                            return (currPosToShdw.Dot(v) / vmagsq + 1.f) * 0.5f;
                        };
                        RwIm3DVertexSetU(vtxIt, CalcTexCoord(oshdw.m_Side, shdwSideMagSq));
                        RwIm3DVertexSetV(vtxIt, CalcTexCoord(oshdw.m_Front, shdwFrontMagSq));

                        // Set position
                        RwIm3DVertexSetPos(
                            vtxIt,
                            currPos.x,
                            currPos.y,
                            CWaterLevel::CalculateWavesOnlyForCoordinate2_Direct(x, y, waterLevelZ, bigWavesZ, smallWavesZ) + 0.06f // Z pos of the wave at the given coord
                        );
                    };

                    // Process 4 vertices of this square
                    ProcessOneVertex(ox,     oy    ); // top left
                    ProcessOneVertex(ox + 1, oy    ); // top right
                    ProcessOneVertex(ox,     oy + 1); // bottom left
                    ProcessOneVertex(ox + 1, oy + 1); // bottom right

                    // Copy vertices into index buffer
                    rng::copy(std::to_array({ 0, 1, 2, 1, 3, 2 }), vtxIdxIt);

                    // And we're done with this one... onto the next!
                    RenderBuffer::StopStoring();

                    // Continue iteration...
                    return true;
                }
            );

            // Render it all
            RenderBuffer::RenderStuffInBuffer();
        }
    }

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,    RWRSTATE(rwTEXTUREADDRESSWRAP));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLBACK));

    ShadowsStoredToBeRendered = 0;
}

// 0x70B730
void CShadows::GeneratePolysForStaticShadow(int16 staticShadowIndex) {
    ((void(__cdecl*)(int16))0x70B730)(staticShadowIndex);
}

// 0x70BA00
bool CShadows::StoreStaticShadow(uint32 id, eShadowType type, RwTexture* texture, const CVector& posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistane, float scale, float drawDistance, bool temporaryShadow, float upDistance) {
    return ((bool(__cdecl*)(uint32, eShadowType, RwTexture*, const CVector&, float, float, float, float, int16, uint8, uint8, uint8, float, float, float, bool, float))0x70BA00)(
        id, type, texture, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, zDistane, scale, drawDistance, temporaryShadow, upDistance);
}

// 0x70BDA0
void CShadows::StoreShadowForVehicle(CVehicle* vehicle, VEH_SHD_TYPE vehShadowType) {
    ((void(__cdecl*)(CVehicle*, VEH_SHD_TYPE))0x70BDA0)(vehicle, vehShadowType);
    // So far so good (most likely), I'm just lazy to finish it
    /*
    const auto shdwStrength = CTimeCycle::m_CurrentColours.m_nShadowStrength;

    if (GraphicsHighQuality() || !shdwStrength) {
        return;
    }

    const auto isPlyrVeh = FindPlayerVehicle() == vehicle;

    const auto& camPos = TheCamera.GetPosition();
    const auto& vehPos = vehicle->GetPosition();

    auto camToVehDist2DSq = (camPos - vehPos).SquaredMagnitude2D();
    if (CCutsceneMgr::IsRunning()) {
        camToVehDist2DSq /= sq(TheCamera.m_fLODDistMultiplier) * 4.f;
    }
    const auto camToVehDist2D = std::sqrt(camToVehDist2DSq);

    const auto maxDist = [&] {
        switch (vehShadowType) {
        case VEH_SHD_HELI:
        case VEH_SHD_PLANE:
        case VEH_SHD_RC:
            return 144.f;
        case VEH_SHD_BIG_PLANE:
            return 288.f;
        default:
            return 18.f;
        }
    }();

    if (camToVehDist2DSq >= sq(maxDist)) {
        return;
    }

    const auto vehBB = vehicle->GetColModel()->GetBoundingBox();

    auto shdwSize = CVector2D{ vehBB.GetSize() };

    float sizeMultY = 1.f;
    switch ((eModelID)vehicle->m_nModelIndex) {
    case MODEL_VORTEX:
        return;
    case UNLOAD_MODEL:
        shdwSize.x *= 0.4f;
        shdwSize.y *= 0.9f;
        sizeMultY = 1.f;
        break;
    case MODEL_FREEWAY:
    case MODEL_SANCHEZ:
    case MODEL_COPBIKE:
        shdwSize.x *= 1.f;
        shdwSize.y *= 1.f;
        sizeMultY = 0.03f;
        break;
    case MODEL_LEVIATHN:
    case MODEL_HUNTER:
    case MODEL_SEASPAR:
    case MODEL_SPARROW:
    case MODEL_MAVERICK:
    case MODEL_POLMAV:
        shdwSize.x *= 0.4f;
        shdwSize.y *= 3.0f;
        sizeMultY = 0.5f;
        break;
    case MODEL_RCRAIDER:
    case MODEL_RCGOBLIN:
        shdwSize.x *= 2.0f;
        shdwSize.y *= 1.5f;
        sizeMultY = 0.2f;
        break;
    case MODEL_BIKE:
    case MODEL_MTBIKE:
    case MODEL_BMX:
    case MODEL_PIZZABOY:
    case MODEL_PCJ600:
    case MODEL_FAGGIO:
        shdwSize.x *= 1.f;
        shdwSize.y *= 1.2f;
        sizeMultY = 0.05f;
        break;
    }

    auto cc = CTimeCycle::m_CurrentColours.m_nShadowStrength;
    if (camToVehDist2D >= maxDist * 0.75f) {
        auto cc = (uint16)(1.f - (camToVehDist2D - maxDist * 0.75f) / (camToVehDist2D * 0.25f)) * (float)CTimeCycle::m_CurrentColours.m_nShadowStrength;
    }

    const auto pos = vehPos - CVector{CVector2D{ vehicle->GetMatrix().GetForward() } * ((shdwSize.y * 0.5f - vehBB.m_vecMax.y) * sizeMultY)};

    auto zDistance = 4.5;

    const auto texture = [&] {
        switch (vehShadowType) {
        case VEH_SHD_CAR:
            return gpShadowCarTex;
        case VEH_SHD_BIKE: {
            auto mult = vehicle->AsBike()->m_RideAnimData.m_fAnimLean * 5.092958f + 1.f; // TODO: Magic number
            if (vehicle->GetStatus() == STATUS_ABANDONED) {
                if (const auto tilt = std::abs(vehicle->GetMatrix().GetRight().z); tilt >= 0.6f) {
                    mult += tilt * 4.f;
                }
            }
            shdwSize.x *= mult;

            return gpShadowBikeTex;
        }
        case VEH_SHD_HELI:
            if (isPlyrVeh) {
                zDistance = 50.f;
            }
            return gpShadowHeliTex;
        case VEH_SHD_BIG_PLANE:
        case VEH_SHD_PLANE:
            cc = CTimeCycle::m_CurrentColours.m_nShadowStrength;
            if (isPlyrVeh) {
                zDistance = 50.f;
            }
            return gpShadowBaronTex;
        case VEH_SHD_RC:
            shdwSize.x *= 2.2f;
            shdwSize.y *= 1.5f;
            return gpShadowBaronTex;
        }
    }();

    // 0x70C143...
    */
}

// 0x70C500
void CShadows::StoreCarLightShadow(CVehicle* vehicle, int32 id, RwTexture* texture, const CVector& posn, float frontX, float frontY, float sideX, float sideY, uint8 red, uint8 green, uint8 blue, float maxViewAngleCosine) {
    // Maximum distance (from camera to `posn`) after which shadows aren't stored (and rendered)
    constexpr auto MAX_CAM_TO_LIGHT_DIST = 27.f;

    if ([] { // Maybe ignore camera distance?
        switch (CCamera::GetActiveCamera().m_nMode) {
        case MODE_TOPDOWN:
        case MODE_TOP_DOWN_PED:
            return false;
        }

        return !CCutsceneMgr::IsRunning();
    }()) {
        const auto shdwToCam2D       = CVector2D{ TheCamera.GetPosition() - posn };
        const auto shdwToCamDist2DSq = shdwToCam2D.SquaredMagnitude();

        if (shdwToCamDist2DSq >= sq(MAX_CAM_TO_LIGHT_DIST)) {
            return;
        }

        // Check if the camera is facing the lights closely (in which case the camera can't see the shadow)
        if (shdwToCam2D.Dot(TheCamera.GetFrontNormal2D()) > maxViewAngleCosine) {
            return;
        }

        // If far enough from the camera, start fading out
        if (const auto dist = std::sqrt(shdwToCamDist2DSq); dist >= MAX_CAM_TO_LIGHT_DIST * 0.75f) {
            const auto t = 1.f - invLerp(MAX_CAM_TO_LIGHT_DIST * (2.f / 3.f), MAX_CAM_TO_LIGHT_DIST, dist);
            red   = (uint8)((float)red * t);
            green = (uint8)((float)green * t);
            blue  = (uint8)((float)blue * t);
        }
    }

    const auto isPlyrVeh = FindPlayerVehicle() == vehicle;
    if (isPlyrVeh || vehicle->GetMoveSpeed().Magnitude() * CTimer::GetTimeStep() >= 0.4f) {
        StoreShadowToBeRendered(
            SHADOW_ADDITIVE,
            texture,
            posn,
            frontX, frontY,
            sideX, sideY,
            128,
            red, green, blue,
            6.f,
            false,
            1.f,
            nullptr,
            isPlyrVeh || g_fx.GetFxQuality() >= FX_QUALITY_VERY_HIGH // NOTSA: At higher FX quality draw all vehicles's shadows on buildings too
        );
    } else {
        StoreStaticShadow(
            reinterpret_cast<uint32>(vehicle) + id,
            SHADOW_ADDITIVE,
            texture,
            posn,
            frontX, frontY,
            sideX, sideY,
            128,
            red, green, blue,
            6.f,
            1.f,
            0.f,
            false,
            0.4f
        );
    }
}

// 0x70C750
void CShadows::StoreShadowForPole(CEntity* entity, float offsetX, float offsetY, float offsetZ, float poleHeight, float poleWidth, uint32 localId) {
    if (GraphicsHighQuality() || !CTimeCycle::m_CurrentColours.m_nPoleShadowStrength) {
        return;
    }

    const auto& mat = entity->GetMatrix();

    if (mat.GetUp().z < .5f) { // More than 45 deg tilted
        return;
    }

    const auto intensity = 2.f * (mat.GetUp().z - 0.5f) * (float)(CTimeCycle::m_CurrentColours.m_nPoleShadowStrength);

    const auto front     = CVector2D{ CTimeCycle::GetVectorToSun() } * (-poleHeight / 2.f);
    const auto right     = CVector2D{ CTimeCycle::GetShadowSide() } * poleWidth;

    StoreStaticShadow(
        reinterpret_cast<uint32>(&entity->m_pLod) + localId + 3,
        SHADOW_DEFAULT,
        gpPostShadowTex,
        mat.GetPosition() + CVector{ front, 0.f } + CVector{
            offsetX * mat.GetRight().x + offsetY * mat.GetForward().x, // Simplified matrix transform (Ignoring the Z axis)
            offsetX * mat.GetRight().y + offsetY * mat.GetForward().y, // >^^^
            offsetZ
        },
        front.x, front.y,
        right.x, right.y,
        2 * (int16)intensity / 3,
        0, 0, 0,
        9.f,
        1.f,
        40.f,
        false,
        0.f
    );
}

// 0x70CCB0
void CShadows::RenderIndicatorShadow(
    uint32 id,
    eShadowType,
    RwTexture*,
    const CVector& posn,
    float frontX, float frontY,
    float sideX, float sideY,
    int16 /*intensity*/
) {
    const auto size = std::max(frontX, -sideX);
    for (auto mult : { 0.8f, 0.9f, 1.0f }) {
        CVector markerPos = posn;
        C3dMarkers::PlaceMarkerSet(id, MARKER3D_CYLINDER, markerPos, size * mult, 255, 0, 0, 255u, 2048, 0.2f, 0);
    }
}

// 0x709CF0
#ifdef _MSC_VER
CVector* ShadowRenderTriangleCB(CVector* normal, CVector* trianglePos, _ProjectionParam* param) {
    CVector* result = nullptr;
    __asm mov eax, normal
    __asm mov ebx, trianglePos
    __asm mov edi, param
    __asm mov ecx, 0x709CF0
    __asm call ecx
    __asm mov result, eax
    return result;
}
#endif
