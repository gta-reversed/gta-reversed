#include "StdInc.h"

#include "CFontDebugModule.h"
#include "imgui.h"
#include "Timer.h"

using namespace ImGui;

constexpr const char* FONT_STYLE_STRING[] = { "GOTHIC", "SUBTITLES", "MENU", "PRICEDOWN" };
constexpr const char* ALIGNMENT_STRING[] = { "UNDEF", "CENTER", "LEFT", "RIGHT" };

void FontDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "Font Debug", {860.f, 290.f}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    static eFontStyle m_FontStyle{};
    static RwRGBAReal m_Color{1.0f,1.0f,1.0f,1.0f}, m_DropColor{0.0f,0.0f,0.0f,1.0f}, m_BackgroundColor{0.0f,0.0f,0.0f,0.5f};
    static CVector2D m_Scale{1.0f, 1.0f}, m_SlantRef{}, m_DrawPos{110.0f, 50.0f};
    static float m_Slant{}, m_Wrapx{}, m_CenterSize{}, m_AlphaFade{255.0f};
    static bool m_Proportional{true}, m_Justify{}, m_BackgroundEnabled{}, m_BackgroundInclWrap{}, m_Draw{}, m_IgnoreSettings{};
    static int m_DropShadowPos{}, m_Edge{};
    static eFontAlignment m_Alignment{};

    static GxtChar m_Text[256];
    // People on the streets will talk to Carl.~n~You can respond to these comments: Use ~k~~CONVERSATION_NO~ for a negative reply or use ~k~~CONVERSATION_YES~ to reply positively

    BeginGroup();
    Checkbox("Draw", &m_Draw); SameLine(); Checkbox("Ignore settings", &m_IgnoreSettings);
    InputText("Text to print", (char*)m_Text, std::size(m_Text));
    SameLine();
    if (Button("I'm feeling lucky")) {
        strcpy((char*)m_Text, (char*)TheText.m_MainKeyArray.m_data[CGeneral::GetRandomNumberInRange(0u, TheText.m_MainKeyArray.m_size)].string);
    }
    InputFloat2("Position", (float*)&m_DrawPos, "%.2f");
    Separator();
    if (BeginCombo("Font style", FONT_STYLE_STRING[+m_FontStyle])) {
        for (auto&& [typeId, wepName] : rngv::enumerate(FONT_STYLE_STRING)) {
            const notsa::ui::ScopedID id{ typeId };
            if (Selectable(wepName, m_FontStyle == typeId)) {
                m_FontStyle = eFontStyle(typeId);
            }
            if (m_FontStyle == eFontStyle(typeId)) {
                SetItemDefaultFocus();
            }
        }
        EndCombo();
    }
    if (BeginCombo("Font alignment", ALIGNMENT_STRING[+m_Alignment + 1])) {
        for (auto&& [typeId, wepName] : rngv::enumerate(ALIGNMENT_STRING)) {
            const notsa::ui::ScopedID id{ typeId };
            if (Selectable(wepName, m_Alignment == eFontAlignment(+typeId - 1))) {
                m_Alignment = eFontAlignment(+typeId - 1);
            }
            if (m_Alignment == eFontAlignment(+typeId - 1)) {
                SetItemDefaultFocus();
            }
        }
        EndCombo();
    }
    ColorEdit4("Color", (float*)&m_Color);
    ColorEdit4("Drop color", (float*)&m_DropColor);
    ColorEdit4("Background color", (float*)&m_BackgroundColor);
    InputFloat2("Scale", (float*)&m_Scale, "%.2f");
    InputFloat2("Slant ref", (float*)&m_SlantRef, "%.2f");
    InputFloat("Slant", &m_Slant, 0.0f, 0.0f, "%.2f");
    InputFloat("Wrap X", &m_Wrapx, 0.0f, 0.0f, "%.2f");
    InputFloat("Center size", &m_CenterSize, 0.0f, 0.0f, "%.2f");
    SliderFloat("Alpha fade", &m_AlphaFade, 0.0f, 255.0f, "%.2f");
    Checkbox("Proportional", &m_Proportional);
    Checkbox("Justify", &m_Justify);
    Checkbox("Bg enabled", &m_BackgroundEnabled); SameLine();
    Checkbox("Bg incl. wrap", &m_BackgroundInclWrap);
    InputInt("Drop shadow pos (disables edge)", &m_DropShadowPos);
    InputInt("Edge (disables drop shadow)", &m_Edge);
    m_DropShadowPos &= 255;
    m_Edge &= 255;

    if (m_Draw) {
        if (!m_IgnoreSettings) {
            CFont::SetDropColor(m_DropColor);
            CFont::SetBackgroundColor(m_BackgroundColor);
            CFont::SetScale(SCREEN_SCALE_X(m_Scale.x), SCREEN_SCALE_Y(m_Scale.y));
            CFont::SetSlantRefPoint(m_SlantRef.x, m_SlantRef.y);
            CFont::SetSlant(m_Slant);
            CFont::SetWrapx(SCREEN_SCALE_X(m_Wrapx));
            CFont::SetCentreSize(SCREEN_SCALE_X(m_CenterSize));
            CFont::SetAlphaFade(m_AlphaFade);
            CFont::SetProportional(m_Proportional);
            CFont::SetJustify(m_Justify);
            CFont::SetBackground(m_BackgroundEnabled, m_BackgroundInclWrap);
            CFont::SetDropShadowPosition(m_DropShadowPos);
            if (!m_DropShadowPos) {
                CFont::SetEdge(m_Edge);
            }
            CFont::SetFontStyle(m_FontStyle);
            CFont::SetOrientation(m_Alignment);
            CFont::SetColor(m_Color);
        }
        CFont::PrintString(SCREEN_SCALE_X(m_DrawPos.x), SCREEN_SCALE_Y(m_DrawPos.y), m_Text);
    }
    EndGroup();
}

void FontDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Font", nullptr, &m_IsOpen);
    });
}
