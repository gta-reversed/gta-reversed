#pragma once

#include "DebugModule.h"

class FontDebugModule final : public DebugModule {
public:
    void RenderWindow() override final;
    void RenderMenuEntry() override final;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(FontDebugModule, m_IsOpen,
        m_FontStyle, m_Color, m_DropColor, m_BackgroundColor, m_Scale, m_SlantRef, m_DrawPos, m_Slant, m_Wrapx, m_CenterSize,
        m_AlphaFade, m_Proportional, m_Justify, m_BackgroundEnabled, m_BackgroundInclWrap, m_Draw, m_IgnoreSettings, m_DropShadowPos,
        m_Edge, m_Alignment, m_Text
    );

private:
    bool           m_IsOpen{};
    eFontStyle     m_FontStyle{};
    RwRGBAReal     m_Color{ 1.0f, 1.0f, 1.0f, 1.0f }, m_DropColor{ 0.0f, 0.0f, 0.0f, 1.0f }, m_BackgroundColor{ 0.0f, 0.0f, 0.0f, 0.5f };
    CVector2D      m_Scale{ 1.0f, 1.0f }, m_SlantRef{}, m_DrawPos{ 110.0f, 50.0f };
    float          m_Slant{}, m_Wrapx{}, m_CenterSize{}, m_AlphaFade{ 255.0f };
    bool           m_Proportional{ true }, m_Justify{}, m_BackgroundEnabled{}, m_BackgroundInclWrap{}, m_Draw{}, m_IgnoreSettings{};
    int            m_DropShadowPos{}, m_Edge{};
    eFontAlignment m_Alignment{};
    GxtChar        m_Text[256];
};
