#pragma once

#include <RenderWare.h>
#include "Vector.h"

enum class ERenderColorType : int32 {
    DEFAULT = 0,
    RANGE   = 1,
    BRIGHT  = 2,
};

struct RenderInfo_t {
    RwRGBA m_Color1;
    RwRGBA m_Color2;
    ERenderColorType m_nColorType;

    float m_fSizeX; // todo: CVector2D m_Size, m_SizeBias
    float m_fSizeY;
    float m_fSizeXBias;
    float m_fSizeYBias;

    float m_fSpriteTop; // todo: RwV4d m_SpriteRect
    float m_fSpriteBottom;
    float m_fSpriteLeft;
    float m_fSpriteRight;

    int32 m_nTrailScreenMode; // trail id
    float m_fTrailTime;

    bool m_bHasAnimTextures;
    int8 m_nCurrentTexId;

    bool m_bHeatHaze;
    bool m_bSelfLit;

    RwMatrix m_FlatMatrix;
    bool m_bIsFlat;
    bool m_bHasDir;
    bool m_bUseVel;

    int8 m_SmokeType;
    CVector  m_Direction;
    RwRGBAReal m_SmokeColor;
    float m_SmokeBrightness;
    float m_SmokeSize;
    float m_SmokeLife;

    void SetColor(float red, float green, float blue, float alpha) {
        m_Color1.red   = (uint8)red;
        m_Color1.green = (uint8)green;
        m_Color1.blue  = (uint8)blue;
        m_Color1.alpha = (uint8)alpha;
    }

    void SetRangeColor(float red, float green, float blue) {
        m_Color2.red   = (uint8)red;
        m_Color2.green = (uint8)green;
        m_Color2.blue  = (uint8)blue;
    }

    void Process() {
        m_Color1.red          = 255;
        m_Color1.green        = 255;
        m_Color1.blue         = 255;
        m_Color1.alpha        = 255;
        m_fSizeX              = 0.1f;
        m_fSizeY              = 0.1f;
        m_nColorType          = ERenderColorType::DEFAULT;
        m_fSizeXBias          = 0.0f;
        m_fSizeYBias          = 0.0f;
        m_fSpriteTop          = 0.5f;
        m_fSpriteBottom       = -0.5f;
        m_fSpriteLeft         = 0.5f;
        m_fSpriteRight        = -0.5f;
        m_bHeatHaze           = false;
        m_bSelfLit            = false;
        m_bIsFlat             = false;
        m_bHasAnimTextures    = false;
        m_nTrailScreenMode    = 0;
        m_bHasDir             = false;
        m_bUseVel             = false;
        m_SmokeType           = -1;
    }
};
VALIDATE_SIZE(RenderInfo_t, 0xA4);
