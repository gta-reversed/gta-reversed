#include "StdInc.h"

#include "Securom.h"
#include "GxtChar.h"

static int32 (&trapDisplay)[32]    = *(int32(*)[32])0xC8CDE8;
static int32 (&triggerDisplay)[32] = *(int32(*)[32])0xC8CE68;

namespace Securom {
    void InjectHooks() {
        RH_ScopedNamespace(Securom);
        RH_ScopedCategoryGlobal();
        RH_ScopedGlobalInstall(SecuromDisplayInit, 0x744A70);
        RH_ScopedGlobalInstall(SecuromStateDisplay, 0x744AE0);
    }
}

// 0x744A70
void SecuromDisplayInit() {
    std::fill(std::begin(trapDisplay), std::end(trapDisplay), 1000);
    std::fill(std::begin(triggerDisplay), std::end(triggerDisplay), 1000);
}

// 0x744AE0
void SecuromStateDisplay() {
    CFont::SetColor(CRGBA(200, 200, 200, 255));
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    CFont::SetScale(0.5f, 0.8f);
    CFont::SetEdge(2);
    CFont::SetDropColor(CRGBA(0, 0, 0, 255));
    CFont::SetProportional(true);
    CFont::SetWrapx(640.f);
    CFont::SetRightJustifyWrap(0.0f);
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);

    char fmttmp[256];
    GxtChar gxt[256];

    float y = 140.f;
    for (int32 i = 0; i < 16; i++) {
        if (triggerDisplay[i]) {
            notsa::format_to_sz(fmttmp, "Trigger {}", i);
            AsciiToGxtChar(fmttmp, gxt);
            CFont::PrintString(30.f, y, gxt);
            --triggerDisplay[i];
        }
        if (triggerDisplay[i + 16]) {
            notsa::format_to_sz(fmttmp, "Trigger {}", i + 16);
            AsciiToGxtChar(fmttmp, gxt);
            CFont::PrintString(160.f, y, gxt);
            --triggerDisplay[i + 16];
        }
        y += 12.f;
    }

    y = 140.f;
    for (int32 i = 0; i < 16; i++) {
        if (trapDisplay[i]) {
            notsa::format_to_sz(fmttmp, "Trap {}", i);
            AsciiToGxtChar(fmttmp, gxt);
            CFont::PrintString(350.f, y, gxt);
            --trapDisplay[i];
        }
        if (trapDisplay[i + 16]) {
            notsa::format_to_sz(fmttmp, "Trap {}", i + 16);
            AsciiToGxtChar(fmttmp, gxt);
            CFont::PrintString(450.f, y, gxt);
            --trapDisplay[i + 16];
        }
        y += 12.f;
    }
}
