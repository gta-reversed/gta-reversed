#include "StdInc.h"

#include "TrafficLights.h"
#include "Shadows.h"

uint8(&CTrafficLights::aTrafficLightColoursR)[3] = *(uint8(*)[3])0x8A6214;
uint8(&CTrafficLights::aTrafficLightColoursG)[3] = *(uint8(*)[3])0x8A6218;
uint8(&CTrafficLights::aTrafficLightColoursB)[3] = *(uint8(*)[3])0xA9AD94;
float& CTrafficLights::fLightMult = *(float*)0x8A621C;

CVector& CTrafficLights::vecTrafficLights5_1 = *(CVector*)0xA9ADD4;
CVector& CTrafficLights::vecTrafficLights5_2 = *(CVector*)0xA9ADC8;
CVector& CTrafficLights::vecTrafficLights_1 = *(CVector*)0xA9ADBC;
CVector& CTrafficLights::vecTrafficLights_2 = *(CVector*)0xA9ADB0;
uint32& CTrafficLights::uiPedLightFlags = *(uint32*)0xA9ADE0;

void CTrafficLights::InjectHooks()
{
    RH_ScopedClass(CTrafficLights);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ShouldCarStopForLight, 0x49D610);
    RH_ScopedInstall(ShouldCarStopForBridge, 0x49D420);
    RH_ScopedInstall(IsMITrafficLight, 0x49D5A0);
    RH_ScopedInstall(DisplayActualLight, 0x49DAB0);
    RH_ScopedInstall(FindTrafficLightType, 0x49D580);
    RH_ScopedInstall(FindTrafficLightTypeFromOrientation, 0x49D4D0);
    RH_ScopedInstall(FindOrientationForTrafficLightType, 0x49D520);
    RH_ScopedInstall(LightForPeds, 0x49D400);
    RH_ScopedInstall(LightForCars1, 0x49D2D0);
    RH_ScopedInstall(LightForCars2, 0x49D310);
    RH_ScopedInstall(LightForCars1_Visual, 0x49D350);
    RH_ScopedInstall(LightForCars2_Visual, 0x49D3A0);
}

// 0x49D610
bool CTrafficLights::ShouldCarStopForLight(CVehicle* vehicle, bool bUnkn) {
    auto appearance = vehicle->GetVehicleAppearance();
    if (appearance == VEHICLE_APPEARANCE_HELI || appearance == VEHICLE_APPEARANCE_PLANE)
        return false;

    // todo: Refactor, 3x ctrl+c, ctrl+v

    const auto CalcDot = [&vehicle](const CCarPathLink& naviNode) {
        const auto& pos = vehicle->GetPosition();
        return (pos - naviNode.m_posn).Dot(naviNode.m_dir);
    };

    auto& currentNodeInfo = vehicle->m_autoPilot.m_nCurrentPathNodeInfo;
    if (currentNodeInfo.IsValid() && ThePaths.m_pPathNodes[currentNodeInfo.m_wAreaId]) {
        const auto& naviNode = ThePaths.GetCarPathLink(currentNodeInfo);
        if (naviNode.m_nTrafficLightState) {
            if ((!naviNode.m_bTrafficLightDirection || naviNode.m_attachedTo == vehicle->m_autoPilot.m_currentAddress) &&
                (naviNode.m_bTrafficLightDirection || naviNode.m_attachedTo != vehicle->m_autoPilot.m_currentAddress)
            ) {
                if (!bUnkn
                    && (naviNode.m_nTrafficLightState != DIR_NORTH_SOUTH || !LightForCars1())
                    && (naviNode.m_nTrafficLightState != DIR_WEST_EAST   || !LightForCars2())
                ) {
                    return false;
                }

                const auto dot = CalcDot(naviNode);
                if (vehicle->m_autoPilot._smthCurr == -1 ? dot > 0.0F && dot < 12.0F : dot < 0.0F && dot > -12.0F) {
                    return true;
                }
            }
        }
    }

    auto& nextNodeInfo = vehicle->m_autoPilot.m_nNextPathNodeInfo;
    if (nextNodeInfo.IsValid() && ThePaths.m_pPathNodes[nextNodeInfo.m_wAreaId]) {
        const auto& naviNode = ThePaths.GetCarPathLink(nextNodeInfo);
        if (naviNode.m_nTrafficLightState) {
            if ((!naviNode.m_bTrafficLightDirection || naviNode.m_attachedTo == vehicle->m_autoPilot.m_startingRouteNode) &&
                (naviNode.m_bTrafficLightDirection || naviNode.m_attachedTo != vehicle->m_autoPilot.m_startingRouteNode)
            ) {
                if (!bUnkn
                    && (naviNode.m_nTrafficLightState != DIR_NORTH_SOUTH || !LightForCars1())
                    && (naviNode.m_nTrafficLightState != DIR_WEST_EAST   || !LightForCars2())
                ) {
                    return false;
                }

                const auto dot = CalcDot(naviNode);
                if (vehicle->m_autoPilot._smthNext == -1 ? dot > 0.0F && dot < 12.0F : dot < 0.0F && dot > -12.0F) {
                    return true;
                }
            }
        }
    }

    auto& prevNodeInfo = vehicle->m_autoPilot.m_nPreviousPathNodeInfo;
    if (prevNodeInfo.IsValid() && ThePaths.m_pPathNodes[prevNodeInfo.m_wAreaId]) {
        const auto& naviNode = ThePaths.GetCarPathLink(prevNodeInfo);
        if (vehicle->m_nStatus == eEntityStatus::STATUS_PHYSICS && naviNode.m_nTrafficLightState) {
            if ((!naviNode.m_bTrafficLightDirection || naviNode.m_attachedTo == vehicle->m_autoPilot.m_endingRouteNode) &&
                ( naviNode.m_bTrafficLightDirection || naviNode.m_attachedTo != vehicle->m_autoPilot.m_endingRouteNode)
            ) {
                if (!bUnkn
                    && (naviNode.m_nTrafficLightState != DIR_NORTH_SOUTH || !LightForCars1())
                    && (naviNode.m_nTrafficLightState != DIR_WEST_EAST   || !LightForCars2())
                ) {
                    return false;
                }

                const auto dot = CalcDot(naviNode);
                if (vehicle->m_autoPilot._smthPrev == -1 ? dot > 0.0F && dot < 6.0F : dot < 0.0F && dot > -6.0F) {
                    return true;
                }
            }
        }
    }

    return false;
}

// 0x49D420
bool CTrafficLights::ShouldCarStopForBridge(CVehicle* vehicle) {
    auto appearance = vehicle->GetVehicleAppearance();
    if (appearance == VEHICLE_APPEARANCE_HELI || appearance == VEHICLE_APPEARANCE_PLANE)
        return false;

    auto& nextNodeInfo = vehicle->m_autoPilot.m_nNextPathNodeInfo;
    if (!nextNodeInfo.IsValid() || !ThePaths.m_pPathNodes[nextNodeInfo.m_wAreaId])
        return false;

    auto& curNodeInfo = vehicle->m_autoPilot.m_nCurrentPathNodeInfo;
    if (!curNodeInfo.IsValid() || ThePaths.m_pPathNodes[curNodeInfo.m_wAreaId])
        return false;

    if (ThePaths.GetCarPathLink(nextNodeInfo).m_bridgeLights &&
        ThePaths.GetCarPathLink(curNodeInfo).m_bridgeLights
    ) {
        return true;
    }
    return false;
}

// 0x49D580
int32 CTrafficLights::FindTrafficLightType(CEntity* entity) {
    auto heading = FindOrientationForTrafficLightType(entity);
    return FindTrafficLightTypeFromOrientation(heading);
}

// 0x49D520
float CTrafficLights::FindOrientationForTrafficLightType(CEntity* entity) {
    const auto& mat = entity->GetMatrix();
    return RadiansToDegrees(CGeneral::GetATanOfXY(mat.GetForward().x, mat.GetForward().y));
}

// 0x49D4D0
int32 CTrafficLights::FindTrafficLightTypeFromOrientation(float fOrientation) {
    if ((fOrientation <= 60.0F || fOrientation >= 150.0F) &&
        (fOrientation <= 240.0F || fOrientation >= 330.0F)
    ) {
        return eTrafficLightsDirection::DIR_WEST_EAST;
    }
    return eTrafficLightsDirection::DIR_NORTH_SOUTH;
}

// 0x49DAB0
void CTrafficLights::DisplayActualLight(CEntity* entity) {
    const auto& entMat = entity->GetMatrix();
    if (entMat.GetUp().z < 0.96F || entity->m_bRenderDamaged)
        return;

    auto iLightColorBase = eBrightLightColor::BRIGHTLIGHT_GREEN_BIG;
    if (entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_3
        || entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_4
        || entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_5
        || entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_GAY
        || entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_MIAMI
    ) {
        iLightColorBase = eBrightLightColor::BRIGHTLIGHT_GREEN_SMALL;
    }

    int32 iLightState;
    if (FindTrafficLightType(entity) == eTrafficLightsDirection::DIR_NORTH_SOUTH)
        iLightState = LightForCars1_Visual();
    else
        iLightState = LightForCars2_Visual();

    auto mi = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    bool bSameDir = DotProduct(TheCamera.m_mCameraMatrix.GetForward(), entMat.GetForward()) > 0.0F;

    CVector vecCenter(0.0F, 0.0F, 0.0F);
    for (int32 iFxInd = 0; iFxInd < mi->m_n2dfxCount; ++iFxInd) {
        auto effect = mi->Get2dEffect(iFxInd);
        if (effect->m_type != e2dEffectType::EFFECT_LIGHT)
            continue;

        auto vecLightPos = entity->GetMatrix().TransformPoint(effect->m_pos);
        vecCenter += vecLightPos;
        int32 iColorState = eTrafficLightsState::LIGHT_GREEN;
        if (effect->light.m_color.red > 200) {
            if (effect->light.m_color.green > 100)
                iColorState = eTrafficLightsState::LIGHT_YELLOW;
            else
                iColorState = eTrafficLightsState::LIGHT_RED;
        }

        if (bSameDir == effect->m_pos.y > 0.0F || iColorState != iLightState)
            continue;

        auto fBrightness = CTimeCycle::m_CurrentColours.m_fSpriteBrightness * 0.07F;
        auto fSize = CTimeCycle::m_CurrentColours.m_fSpriteSize * 0.175F;

        CCoronas::RegisterCorona(
            reinterpret_cast<uint32>(entity) + iFxInd,
            nullptr,
            static_cast<uint8>((float)aTrafficLightColoursR[iColorState] * fBrightness),
            static_cast<uint8>((float)aTrafficLightColoursG[iColorState] * fBrightness),
            static_cast<uint8>((float)aTrafficLightColoursB[iColorState] * fBrightness),
            255,
            vecLightPos,
            fSize,
            50.0F,
            eCoronaType::CORONATYPE_SHINYSTAR,
            eCoronaFlareType::FLARETYPE_NONE,
            true,
            false,
            0,
            0.0F,
            false,
            1.5F,
            0,
            15.0F,
            false,
            false
        );

        CBrightLights::RegisterOne(
            vecLightPos,
            entity->GetMatrix().GetUp(),
            entity->GetMatrix().GetRight(),
            CVector(0.0F, 0.0F, 0.0F),
            (eBrightLightColor)(iLightState + iLightColorBase),
            0,
            0,
            0
        );
    }

    if (mi->m_n2dfxCount) {
        vecCenter /= mi->m_n2dfxCount;
        if (iLightState < eTrafficLightsState::LIGHT_OFF) {
            if (CWeather::TrafficLightsBrightness > 0.5F) {
                auto ucRed =   (float)std::max(aTrafficLightColoursR[iLightState], uint8(50));
                auto ucGreen = (float)std::max(aTrafficLightColoursG[iLightState], uint8(50));
                auto ucBlue =  (float)std::max(aTrafficLightColoursB[iLightState], uint8(50));

                CPointLights::AddLight(
                    ePointLightType::PLTYPE_POINTLIGHT,
                    vecCenter,
                    CVector(0.0F, 0.0F, 0.0F),
                    14.0F,
                    fLightMult * ucRed,
                    fLightMult * ucGreen,
                    fLightMult * ucBlue,
                    RwFogType::rwFOGTYPELINEAR,
                    true,
                    nullptr
                );
            }

            if (CWeather::TrafficLightsBrightness > 0.05F) {
                auto fColMult = CTimeCycle::m_CurrentColours.m_fLightsOnGroundBrightness * CWeather::TrafficLightsBrightness / 80.0F;
                auto ucRed   = static_cast<uint8>((float)aTrafficLightColoursR[iLightState] * fColMult);
                auto ucGreen = static_cast<uint8>((float)aTrafficLightColoursG[iLightState] * fColMult);
                auto ucBlue  = static_cast<uint8>((float)aTrafficLightColoursB[iLightState] * fColMult);

                CShadows::StoreStaticShadow(
                    reinterpret_cast<uint32>(entity),
                    eShadowType::SHADOW_ADDITIVE,
                    gpShadowExplosionTex,
                    vecCenter,
                    8.0F,
                    0.0F,
                    0.0F,
                    -8.0F,
                    128,
                    ucRed,
                    ucGreen,
                    ucBlue,
                    12.0F,
                    1.0F,
                    40.0F,
                    false,
                    0.0F
                );
            }
        }
    }

    CVector vecCorner1(0.0F, 0.0F, 0.0F);
    CVector vecCorner2(0.0F, 0.0F, 0.0F);
    if (entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_4
     || entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_GAY) {

        vecCorner1.Set(0.189F, -0.157F, 1.848F);
        vecCorner2.Set(-0.141F, -0.157F, 1.607F);
    }
    else if (entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_5) {
        if (!(uiPedLightFlags & 1)) {
            uiPedLightFlags |= 1;
            vecTrafficLights5_1.Set(-0.044F, -0.261F, 1.561F);
        }

        if (!(uiPedLightFlags & 2)) {
            uiPedLightFlags |= 2;
            vecTrafficLights5_2.Set(0.222F, -0.261F, 1.867F);
        }
        vecCorner1 = vecTrafficLights5_2;
        vecCorner2 = vecTrafficLights5_1;
    }
    else if (entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS) {
        if (!(uiPedLightFlags & 4)) {
            uiPedLightFlags |= 4;
            vecTrafficLights_1.Set(2.673F, 1.262F, -0.539F);
        }
        if (!(uiPedLightFlags & 8)) {
            uiPedLightFlags |= 8;
            vecTrafficLights_2.Set(2.673F, 0.713F, -0.127F);
        }
        vecCorner1 = vecTrafficLights_2;
        vecCorner2 = vecTrafficLights_1;
    }
    else {
        return;
    }

    auto iPedLightState = LightForPeds();
    if (iPedLightState == eTrafficLightsState::LIGHT_RED) {
        // Ped lights are composed of 2 images side by side, we want to draw only the required half
        auto vecMidPoint = (vecCorner1 + vecCorner2) * 0.5F;

        auto p1 = CVector(vecCorner2.x, vecCorner2.y, vecCorner1.z);
        auto p2 = CVector(vecMidPoint.x, vecMidPoint.y, vecCorner1.z);
        auto p3 = vecCorner2;
        auto p4 = CVector(vecMidPoint.x, vecMidPoint.y, vecCorner2.z);

        CShinyTexts::RegisterOne(
            entity->GetMatrix().TransformPoint(p1),
            entity->GetMatrix().TransformPoint(p2),
            entity->GetMatrix().TransformPoint(p3),
            entity->GetMatrix().TransformPoint(p4),
            0.5F,
            0.0F,
            0.0F,
            0.0F,
            0.5F,
            1.0F,
            0.0F,
            1.0F,
            1,
            255,
            0,
            0,
            60.0F
        );

    }
    else if (iPedLightState == eTrafficLightsState::LIGHT_GREEN || CTimer::GetTimeInMS() & 0x100) {
        // Ped lights are composed of 2 images side by side, we want to draw only the required half
        auto vecMidPoint = (vecCorner1 + vecCorner2) * 0.5F;

        auto p1 = CVector(vecMidPoint.x, vecMidPoint.y, vecCorner1.z);
        auto p2 = vecCorner1;
        auto p3 = CVector(vecMidPoint.x, vecMidPoint.y, vecCorner2.z);
        auto p4 = CVector(vecCorner1.x, vecCorner1.y, vecCorner2.z);

        CShinyTexts::RegisterOne(
            entity->GetMatrix().TransformPoint(p1),
            entity->GetMatrix().TransformPoint(p2),
            entity->GetMatrix().TransformPoint(p3),
            entity->GetMatrix().TransformPoint(p4),
            1.0F,
            0.0F,
            0.5F,
            0.0F,
            1.0F,
            1.0F,
            0.5F,
            1.0F,
            true,
            255,
            255,
            255,
            60.0F
        );
    }
}

// 0x49D5A0
bool CTrafficLights::IsMITrafficLight(int32 modelIndex) {
    return modelIndex == ModelIndices::MI_TRAFFICLIGHTS
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_VERTICAL
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_MIAMI
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_VEGAS
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_TWOVERTICAL
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_3
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_4
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_5
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_GAY;
}

// 0x49D400
uint8 CTrafficLights::LightForPeds() {
    auto uiMaskedTime = (CTimer::GetTimeInMS() / 2) & 16383;
    if (uiMaskedTime < 12000)
        return eTrafficLightsState::LIGHT_RED;

    if (uiMaskedTime < 16383 - 1000)
        return eTrafficLightsState::LIGHT_GREEN;

    return eTrafficLightsState::LIGHT_YELLOW;
}

// 0x49D2D0
uint8 CTrafficLights::LightForCars1() {
    if (CGameLogic::LaRiotsActiveHere() || CCheat::IsActive(CHEAT_BGREEN_LIGHTS_CHEAT))
        return eTrafficLightsState::LIGHT_GREEN;

    auto uiMaskedTime = (CTimer::GetTimeInMS() / 2) & 16383;
    if (uiMaskedTime < 5000)
        return eTrafficLightsState::LIGHT_GREEN;

    if (uiMaskedTime < 6000)
        return eTrafficLightsState::LIGHT_YELLOW;

    return eTrafficLightsState::LIGHT_RED;
}

// 0x49D310
uint8 CTrafficLights::LightForCars2() {
    if (CGameLogic::LaRiotsActiveHere() || CCheat::IsActive(CHEAT_BGREEN_LIGHTS_CHEAT))
        return eTrafficLightsState::LIGHT_GREEN;

    auto uiMaskedTime = (CTimer::GetTimeInMS() / 2) & 16383;
    if (uiMaskedTime < 6000)
        return eTrafficLightsState::LIGHT_RED;

    if (uiMaskedTime < 11000)
        return eTrafficLightsState::LIGHT_GREEN;

    if (uiMaskedTime < 12000)
        return eTrafficLightsState::LIGHT_YELLOW;

    return eTrafficLightsState::LIGHT_RED;
}

// 0x49D350
uint8 CTrafficLights::LightForCars1_Visual() {
    if (CGameLogic::LaRiotsActiveHere()) {
        if ((CTimer::GetTimeInMS() / 1024) & 1)
            return eTrafficLightsState::LIGHT_OFF;
        else
            return eTrafficLightsState::LIGHT_YELLOW;
    }

    return LightForCars1();
}

// 0x49D3A0
uint8 CTrafficLights::LightForCars2_Visual() {
    if (CGameLogic::LaRiotsActiveHere()) {
        if ((CTimer::GetTimeInMS() / 1024) & 1)
            return eTrafficLightsState::LIGHT_OFF;
        else
            return eTrafficLightsState::LIGHT_YELLOW;
    }

    return LightForCars2();
}
