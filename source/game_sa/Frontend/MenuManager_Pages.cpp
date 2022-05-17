#include "StdInc.h"

#include "MenuManager.h"
#include "MenuManager_Internal.h"
#include "World.h"
#include "MenuSystem.h"
#include "Pad.h"
#include "Streaming.h"
#include "AudioEngine.h"
#include "Radar.h"
#include "TheZones.h"

/**
 * @addr 0x573C90
 */
void CMenuManager::CalculateMapLimits(float& bottom, float& top, float& left, float& right) {
    bottom = m_vMapOrigin.y - m_fMapZoom;
    top    = m_vMapOrigin.y + m_fMapZoom;
    left   = m_vMapOrigin.x - m_fMapZoom;
    right  = m_vMapOrigin.x + m_fMapZoom;
}

// NOTSA
// from 0x577483 to 0x5775E5
void CMenuManager::PlaceRedMarker() {
    const auto pad = CPad::GetPad(m_nPlayerNumber);
    auto pressed = (
         pad->IsCirclePressed()
      || pad->IsMouseLButtonPressed()
      || pad->IsMouseRButtonPressed()
      || pad->IsStandardKeyJustPressed('T') || pad->IsStandardKeyJustPressed('t')
    );
    if (!pressed)
        return;

    if (CTheScripts::HideAllFrontEndMapBlips && CTheScripts::bPlayerIsOffTheMap)
        return;

    if (m_nTargetBlipIndex) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_BACK);
        CRadar::ClearBlip(m_nTargetBlipIndex);
        m_nTargetBlipIndex = 0;
    } else {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT);
        m_nTargetBlipIndex = CRadar::SetCoordBlip(BLIP_COORD, { m_vMousePos.x, m_vMousePos.y, 0.0f }, 0, BLIP_DISPLAY_BLIPONLY, "CODEWAY");
        CRadar::SetBlipSprite(m_nTargetBlipIndex, RADAR_SPRITE_WAYPOINT);
    }
}

// NOTSA 0x5775E5 to 0x577918
void CMenuManager::RadarZoomIn() {
    const auto pad = CPad::GetPad(m_nPlayerNumber);
    auto pressed = ( // todo:
           pad->NewState.LeftShoulder2
        && !pad->NewState.RightShoulder2
        && CPad::NewMouseControllerState.wheelUp
        && CPad::NewKeyState.pgup
    );
    if (!pressed)
        return;

    // todo:
    auto v103 = 320.0f - m_vMapOrigin.x;
    auto v115 = 224.0f - m_vMapOrigin.y;
    auto v5 = 1.0f / m_fMapZoom;
    auto x = v5 * v103;
    auto y = v5 * v115;

    if (CTimer::GetTimeInMSPauseMode() - FrontEndMenuManager.field_1B38 <= 20)
        return;

    if (m_fMapZoom >= FRONTEND_MAP_RANGE_MAX) {
        m_fMapZoom = FRONTEND_MAP_RANGE_MAX;
    } else {
        m_fMapZoom += 7.0f;
        if (CPad::NewMouseControllerState.wheelUp) {
            m_fMapZoom += 21.0f;
        }
        m_vMapOrigin.x -= (x * m_fMapZoom - v103);
        m_vMapOrigin.y -= (y * m_fMapZoom - v115);
    }

    CVector2D radar, screen;

    CRadar::TransformRealWorldPointToRadarSpace(radar, m_vMousePos);
    CRadar::LimitRadarPoint(radar);
    CRadar::TransformRadarPointToScreenSpace(screen, radar);

    while (screen.x > 576.0f) {
        m_vMousePos.x = m_vMousePos.x - 1.0f;
        CRadar::TransformRealWorldPointToRadarSpace(radar, m_vMousePos);
        CRadar::LimitRadarPoint(radar);
        CRadar::TransformRadarPointToScreenSpace(screen, radar);
    }

    while (screen.x < 64.0f) {
        m_vMousePos.x = m_vMousePos.x + 1.0f;
        CRadar::TransformRealWorldPointToRadarSpace(radar, m_vMousePos);
        CRadar::LimitRadarPoint(radar);
        CRadar::TransformRadarPointToScreenSpace(screen, radar);
    }

    while (screen.y < 64.0f) {
        m_vMousePos.y = m_vMousePos.y - 1.0f;
        CRadar::TransformRealWorldPointToRadarSpace(radar, m_vMousePos);
        CRadar::LimitRadarPoint(radar);
        CRadar::TransformRadarPointToScreenSpace(screen, radar);
    }

    while (screen.y > 384.0f) {
        m_vMousePos.y = m_vMousePos.y + 1.0f;
        CRadar::TransformRealWorldPointToRadarSpace(radar, m_vMousePos);
        CRadar::LimitRadarPoint(radar);
        CRadar::TransformRadarPointToScreenSpace(screen, radar);
    }
}

// 0x575130
void CMenuManager::PrintMap() {
    plugin::CallMethod<0x575130, CMenuManager*>(this);
}

// 0x574900
void CMenuManager::PrintStats() {
    plugin::CallMethod<0x574900, CMenuManager*>(this);
}

// 0x576320
void CMenuManager::PrintBriefs() {
    plugin::CallMethod<0x576320, CMenuManager*>(this);
}

// 0x5746F0
void CMenuManager::PrintRadioStationList() {
    // draw all, except current
    for (auto i = 1u; i < std::size(m_apRadioSprites); i++) {
        if (m_nRadioStation == i)
            continue;

        m_apRadioSprites[i].Draw(
            StretchX(float(47 * (i - 1) + 44)),
            StretchY(300.0f),
            StretchX(35.0f),
            StretchY(35.0f),
            { 255, 255, 255, 30 } // todo: fix alpha
        );
    }

    // highlight current radio station
    if (m_nRadioStation > 0 && m_nRadioStation < std::size(m_apRadioSprites)) {
        m_apRadioSprites[m_nRadioStation].Draw(
            StretchX((float)(47 * m_nRadioStation - 15)),
            StretchY(290.0f),
            StretchX(60.0f),
            StretchY(60.0f),
            { 255, 255, 255, 255 }
        );
    }
}
