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
#include "PlaceName.h"

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
        m_nTargetBlipIndex = CRadar::SetCoordBlip(BLIP_COORD, { m_vMousePos.x, m_vMousePos.y, 0.0f }, BLIP_COLOUR_RED, BLIP_DISPLAY_BLIPONLY, "CODEWAY");
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

    if (CTimer::GetTimeInMSPauseMode() - field_1B38 <= 20)
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
    auto pad = CPad::GetPad(m_nPlayerNumber);
    if (CPad::NewKeyState.standardKeys['Z'] || (pad, CPad::NewKeyState.standardKeys['z'])) {
        m_bMapLoaded = false;
        m_bDrawMouse = false;
    }
    m_bDrawingMap = true;
    CRadar::InitFrontEndMap();
    if (!m_bMapLoaded) {
        if (m_nSysMenu >= 0u) {
            CMenuSystem::SwitchOffMenu(m_nSysMenu % 2); // TODO: Remove `% 2`?
            m_nSysMenu = 157;
        }

        m_vMapOrigin = CVector2D(320.0f, 206.0f);
        m_fMapZoom = 140.0f;
        CVector worldPos = FindPlayerCentreOfWorld_NoSniperShift(0);

        CVector2D out;
        CVector2D insideRect;
        CRadar::TransformRealWorldPointToRadarSpace(out, worldPos);
        CRadar::LimitRadarPoint(out);
        CRadar::TransformRadarPointToScreenSpace(insideRect, out);

        // 0x575208
        const float dx = insideRect.x - m_vMapOrigin.x;
        const float dy = insideRect.y - m_vMapOrigin.y;
        const float BOUNDARY = 140.0f;

        if (dx > BOUNDARY) {
            m_fMapZoom -= (dx - BOUNDARY);
        } else if (dx < -BOUNDARY) {
            m_fMapZoom -= (-BOUNDARY - dx);
        } else if (dy > BOUNDARY) {
            m_fMapZoom -= (dy - BOUNDARY);
        } else if (dy < -BOUNDARY) {
            m_fMapZoom -= (-BOUNDARY - dy);
        }
        m_fMapZoom = std::max(m_fMapZoom, 70.0f);
    }
    const auto zoomFactorForStretchX = m_fMapZoom / 6;
    const auto zoomFactorForStretchY = m_fMapZoom / 5;
    const CRect rect = {
        StretchX(60.0f),
        StretchY(60.0f),
        StretchX(580.0f),
        StretchY(388.0f)
    };
    const CVector2D vect = {
        StretchX(m_vMapOrigin.x - m_fMapZoom),
        StretchY(m_vMapOrigin.y - m_fMapZoom)
    };

    if (m_bMapLoaded) {
        if (m_bStreamingDisabled && !m_bAllStreamingStuffLoaded) {
            FrontEndMenuManager.m_iRadarVisibilityChangeTime = CTimer::GetTimeInMSPauseMode();
            FrontEndMenuManager.m_bViewRadar = false;
        }
        if (CTimer::GetTimeInMSPauseMode() - FrontEndMenuManager.m_iRadarVisibilityChangeTime > 400) {
            FrontEndMenuManager.m_bViewRadar = true;
        }
    } else {
        FrontEndMenuManager.m_iRadarVisibilityChangeTime = CTimer::GetTimeInMSPauseMode();
        FrontEndMenuManager.m_bViewRadar = false;
    }
    if (m_bMapLoaded) {
        if (m_bAllStreamingStuffLoaded) {
            m_bStreamingDisabled = false;
        }
        const CRect coords = { 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT };
        if (FrontEndMenuManager.m_bViewRadar) {
            CSprite2d::DrawRect(coords, CRGBA(111, 137, 170, 255)); // blue background
            const auto stretchX = StretchX(zoomFactorForStretchX);
            const auto stretchYValue = StretchY(zoomFactorForStretchX);
            for (auto x = 0; x < 12; x++) {
                for (auto y = 0; y < 12; y++) {
                    if (rect.left - stretchX < x * stretchX + vect.x) {
                        if (rect.bottom - stretchYValue < y * stretchYValue + vect.y) {
                            if (rect.right + stretchX > (x + 1) * stretchX + vect.x) {
                                if (rect.top + stretchYValue > (y + 1) * stretchYValue + vect.y) {
                                    CRadar::DrawRadarSectionMap(
                                        x, y,
                                        { StretchX(zoomFactorForStretchX) * x + vect.x,
                                          StretchY(zoomFactorForStretchX) * y + vect.y,
                                          StretchX(zoomFactorForStretchX) * x + 1 + vect.x,
                                          StretchY(zoomFactorForStretchX) * y + 1 + vect.y }
                                    );
                                }
                            }
                        }
                    }
                }
            }
        } else {
            CSprite2d::DrawRect(coords, CRGBA(0, 0, 0, 255));
            SmallMessageScreen("FEM_PWT");
            m_bAllStreamingStuffLoaded = true;
        }
    } else {
        CSprite2d::DrawRect(
            { StretchX(m_vMapOrigin.x - 145.0f),
              StretchY(m_vMapOrigin.y - 145.0f),
              StretchX(m_vMapOrigin.x + 145.0f),
              StretchY(m_vMapOrigin.y + 145.0f) },
            CRGBA(100, 100, 100, 255)
        );

        CSprite2d::DrawRect(
            { StretchX(m_vMapOrigin.x - 141.0f),
              StretchY(m_vMapOrigin.y - 141.0f),
              StretchX(m_vMapOrigin.x + 141.0f),
              StretchY(m_vMapOrigin.y + 141.0f) },
            CRGBA(0, 0, 0, 255)
        );

        CSprite2d::DrawRect(
            { StretchX(m_vMapOrigin.x - 140.0f),
              StretchY(m_vMapOrigin.y - 140.0f),
              StretchX(m_vMapOrigin.x + 140.0f),
              StretchY(m_vMapOrigin.y + 140.0f) },
            CRGBA(111, 137, 170, 255)
        );

        m_apBackgroundTextures[7].Draw(
            { StretchX(m_vMapOrigin.x - m_fMapZoom),
              StretchY(m_vMapOrigin.y - m_fMapZoom),
              StretchX(m_vMapOrigin.x + m_fMapZoom),
              StretchY(m_vMapOrigin.y + m_fMapZoom) },
            CRGBA(255, 255, 255, 255)
        );
    }

    if (FrontEndMenuManager.m_bViewRadar || !m_bMapLoaded) {
        CRadar::DrawRadarGangOverlay(1);
        if (CTheZones::ZonesRevealed < 80) {
            for (auto x = 0; x < 10; x++) {
                for (auto y = 0; y < 10; y++) {
                    if (!CTheZones::ZonesVisited[x][y]) {
                        CSprite2d::DrawRect(
                            { StretchX(zoomFactorForStretchY) * x + vect.x,
                              StretchY(zoomFactorForStretchY) * y + vect.y,
                              StretchX(zoomFactorForStretchY) * x + 1 + vect.x,
                              StretchY(zoomFactorForStretchY) * y + 1 + vect.y },
                            CRGBA(111, 137, 170, 200)
                        ); // radar fog
                    }
                }
            }
        }
        if (!CTheScripts::HideAllFrontEndMapBlips && !CTheScripts::bPlayerIsOffTheMap) {
            CRadar::DrawBlips();
        }
        if (!m_bMapLoaded && FrontEndMenuManager.m_bViewRadar) {
            if (m_nSysMenu >= 0u) {
                CMenuSystem::Process(m_nSysMenu);
            }
        }
    }

    // black fields
    // TODO: Fix in Full Map (Press Z)
    CSprite2d::DrawRect(
        { 0.0f, 0.0f, SCREEN_WIDTH, rect.bottom },
        CRGBA(100, 100, 100, 255)
    );

    CSprite2d::DrawRect(
        { 0.0f, rect.top, SCREEN_WIDTH, SCREEN_HEIGHT },
        CRGBA(100, 100, 100, 255)
    );

    CSprite2d::DrawRect(
        { 0.0f, 0.0f, rect.left, SCREEN_HEIGHT },
        CRGBA(100, 100, 100, 255)
    );

    CSprite2d::DrawRect(
        { rect.right, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT },
        CRGBA(100, 100, 100, 255)
    );

    CSprite2d::DrawRect(
        { 0.0f, 0.0f, SCREEN_WIDTH, rect.bottom - StretchY(4.0f) },
        CRGBA(0, 0, 0, 255)
    );

    CSprite2d::DrawRect(
        { 0.0f, rect.top + StretchY(4.0f), SCREEN_WIDTH, SCREEN_HEIGHT },
        CRGBA(0, 0, 0, 255)
    );

    CSprite2d::DrawRect(
        { 0.0f, 0.0f, rect.left - StretchX(4.0f), SCREEN_HEIGHT },
        CRGBA(0, 0, 0, 255)
    );

    CSprite2d::DrawRect(
        { rect.right + StretchX(4.0f), 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT },
        CRGBA(0, 0, 0, 255)
    );

    // 0x575E21
    if (FrontEndMenuManager.m_bViewRadar) {
        if (CTheZones::ZonesRevealed >= 80
            || CTheZones::GetCurrentZoneLockedOrUnlocked(m_vMousePos)
                && !pad->NewMouseControllerState.lmb)
        {
            CPlaceName placeName;
            CFont::SetFontStyle(FONT_PRICEDOWN);
            CFont::SetWrapx(640.0);
            CFont::SetDropColor(CRGBA(0, 0, 0, 255u));
            CFont::SetScale(StretchX(0.8f), StretchY(0.8f));
            CFont::SetColor(CRGBA(225, 225, 225, 255));
            CFont::SetEdge(2);
            CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
            CFont::PrintString(
                rect.right - StretchX(30.0f), rect.top - StretchY(30.0f),
                placeName.GetForMap(m_vMousePos.x, m_vMousePos.y)
            );
        }

        if (m_bMapLegend) {
            const auto iterations = (CRadar::MapLegendCounter - 1) / 2 + 3;
            float y = StretchY(100.0f);
            if (iterations > 0) {
                y += StretchY(19.0f) * iterations;
            }

            CMenuManager::DrawWindow(
                { StretchX(95.0f), StretchY(100.0f), StretchX(550.0f), y },
                "FE_MLG", 0, CRGBA(0, 0, 0, 190), true, true
            ); // map legend
            CFont::SetWrapx(SCREEN_WIDTH - 40);
            CFont::SetRightJustifyWrap(84.0);
            CFont::SetDropShadowPosition(1);
            CFont::SetDropColor(CRGBA(0, 0, 0, 255));
            CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
            CFont::SetFontStyle(FONT_MENU);
            CFont::SetScale(StretchX(0.3f), StretchY(0.55f));
            CFont::SetColor(CRGBA(172, 203, 241, 255));
            if (CRadar::MapLegendCounter) {
                float currentY = StretchY(127.0f);
                float currentX = StretchX(160.0f);
                const auto midPoint = (CRadar::MapLegendCounter - 1) / 2;
                for (auto legend = 0; legend < CRadar::MapLegendCounter; ++legend) {
                    CRadar::DrawLegend(
                        static_cast<int32>(currentX),
                        static_cast<int32>(currentY),
                        static_cast<eRadarSprite>(CRadar::MapLegendList[legend])
                    );

                    if (legend == midPoint) {
                        currentX = StretchX(350.0f);
                        currentY = StretchY(127.0f);
                    } else {
                        currentY += StretchY(19.0f);
                    }
                }
            }
        }
        if (m_nSysMenu >= 0u) {
            CMenuSystem::Process(m_nSysMenu);
        }
    }
    m_bDrawingMap = false;
    CFont::SetWrapx(SCREEN_WIDTH - 10);
    CFont::SetRightJustifyWrap(10.0f);
    if (m_bMapLoaded) {
        if (m_nSysMenu < 128u) {
            CMenuManager::DisplayHelperText("FEH_MPB");
            m_bMapLoaded = true;
            return;
        }
        CMenuManager::DisplayHelperText("FEH_MPH");
    }
    m_bMapLoaded = true;
}

// 0x574900
void CMenuManager::PrintStats() {
    plugin::CallMethod<0x574900, CMenuManager*>(this);
}

// 0x576320
void CMenuManager::PrintBriefs() {
    CFont::SetColor({ 255, 255, 255, 255 });
    CFont::SetDropColor({0, 0, 0, 255});
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetFontStyle(FONT_SUBTITLES);
    CFont::SetScaleForCurrentLanguage(StretchX(0.49f), StretchY(0.7f));
    CFont::SetWrapx(StretchX(560.0f));
    CFont::SetDropShadowPosition(1);
    CFont::SetJustify(false);

    static constexpr size_t BRIEFS_MAX_LINES_ON_SCREEN = 4u;

    // underflow check
    if (m_nSelectedRow >= m_nSelectedRow - (BRIEFS_MAX_LINES_ON_SCREEN - 1)) {
        float h = 100.0f;
        for (auto i = 0u; i < BRIEFS_MAX_LINES_ON_SCREEN; i++) {
            auto& brief = CMessages::PreviousBriefs[m_nSelectedRow - i];
            if (!brief.Text)
                continue;

            CMessages::InsertNumberInString2(brief.Text, brief.NumbersToInsert, gGxtString);
            CMessages::InsertStringInString(gGxtString, brief.StringToInsert);
            CMessages::InsertPlayerControlKeysInString(gGxtString);
            CFont::PrintString(StretchX(70.0f), StretchY(h), gGxtString);

            h += 70.0f; // 210 / (BRIEFS_MAX_LINES_ON_SCREEN - 1)
        }
    }
    CFont::SetJustify(false); // redundant

    static bool& drawArrows = *reinterpret_cast<bool*>(0x8CDFF9);

    if (!m_bMapLoaded)
        return;

    if (CTimer::GetTimeInMSPauseMode() - m_nBriefsArrowBlinkTimeMs > 700) {
        m_nBriefsArrowBlinkTimeMs = CTimer::GetTimeInMSPauseMode();
        drawArrows = !drawArrows;
    }

    if (drawArrows) {
        // up arrow
        if (m_nSelectedRow < 19u && CMessages::PreviousBriefs[m_nSelectedRow + 1].Text) {
            CSprite2d::Draw2DPolygon(
                StretchX(50.0f), StretchY(100.0f),
                StretchX(55.0f), StretchY(110.0f),
                StretchX(45.0f), StretchY(110.0f),
                StretchX(50.0f), StretchY(100.0f),
                { 225, 225, 225, 255 }
            );
        }

        // down arrow
        if (m_nSelectedRow > 3u) {
            CSprite2d::Draw2DPolygon(
                StretchX(50.0f), StretchY(348.0f),
                StretchX(55.0f), StretchY(338.0f),
                StretchX(45.0f), StretchY(338.0f),
                StretchX(50.0f), StretchY(348.0f),
                { 225, 225, 225, 255 }
            );
        }
    }
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
    if (m_nRadioStation > 0 && m_nRadioStation < (int8)std::size(m_apRadioSprites)) {
        m_apRadioSprites[m_nRadioStation].Draw(
            StretchX((float)(47 * m_nRadioStation - 15)),
            StretchY(290.0f),
            StretchX(60.0f),
            StretchY(60.0f),
            { 255, 255, 255, 255 }
        );
    }
}
