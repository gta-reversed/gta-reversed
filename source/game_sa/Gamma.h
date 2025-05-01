#pragma once

class CGamma {
public:
    inline static auto& ms_SavedGamma = StaticRef<D3DGAMMARAMP, 0xC8D4C8>();
    inline static auto& ms_GammaTable = StaticRef<D3DGAMMARAMP, 0xC8DAC8>();

public:
    bool  m_IsEnabled;
    float m_CurrentLevel;
    float m_TargetLevel;
    float m_PrevLevel;

public:
    static void InjectHooks();

    void Init();
    void SetGamma(float gammaLevel, bool fade);
};

VALIDATE_SIZE(CGamma, 0x10);

inline static bool& gbGammaChanged = StaticRef<bool>(0xC920C8);
inline static CGamma& gamma        = StaticRef<CGamma>(0xC92134);
