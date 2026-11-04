#pragma once

struct CWindModifier {
    CVector m_vecPos;
    int32   m_iActive;
    float   m_fPower;
};

class CWindModifiers {
public:
    static constexpr int32 MAX_NUM_MODIFIERS = 16;

    static inline auto& Number = StaticRef<int32>(0xC81450);
    static inline auto& Array = StaticRef<CWindModifier[MAX_NUM_MODIFIERS]>(0xC81458);

public:
    static void InjectHooks();

    static void RegisterOne(CVector vecPos, int32 iActive, float fPower);
    static bool FindWindModifier(CVector vecPos, float* outX, float* outY);
};
