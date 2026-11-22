#include "StdInc.h"

#include "Debug.h"

std::vector<CDebug::tDebugText> CDebug::m_debugStrings;

void CDebug::InjectHooks()
{
    RH_ScopedClass(CDebug);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(DebugDisplayTextBuffer, 0x532260);
}

void CDebug::DebugInitTextBuffer() {
    ZoneScoped;

    m_debugStrings.reserve(500);
}

void CDebug::DebugAddText(const char* str)
{
    float x = 0.0f, y = 0.0f;
    if (!m_debugStrings.empty()) {
        auto& lastElement = m_debugStrings.back();
        x = lastElement.x;
        y = lastElement.y + 40.0f;
    }
    m_debugStrings.push_back({str, x, y});
}

void CDebug::DebugAddText(const char* str, float x, float y)
{
    m_debugStrings.push_back({ str, x, y });
}

void CDebug::DebugDisplayTextBuffer() {
    ZoneScoped;

    CFont::SetBackground(0, 0);
    CFont::SetBackgroundColor(CRGBA(0, 0, 0, 128));
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetProportional(true);
    CFont::SetDropShadowPosition(0);
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    CFont::SetColor(CRGBA(225, 225, 225, 255));
    CFont::SetDropShadowPosition(2);
    CFont::SetDropColor(CRGBA(0, 0, 0, 255));
    CFont::SetCentreSize(SCREEN_SCALE_X(60.0f));
    CFont::SetScale(SCREEN_SCALE_X(0.58f), SCREEN_SCALE_Y(1.2f));
    for (size_t i = 0; i < m_debugStrings.size(); i++)
    {
        auto& debugStr = m_debugStrings[i];
        CFont::PrintString(debugStr.x, debugStr.y, GxtCharFromAscii(debugStr.text.c_str()));
    }
    CFont::DrawFonts();
    m_debugStrings.clear();
}
