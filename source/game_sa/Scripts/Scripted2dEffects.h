#pragma once

struct C2dEffect;

class CScriptedEffectPair {
public:
    enum class eMode : int32 {
        NONE = -1,
        USE_PARTNER_ONCE = 0x0,
        LOOK_FOR_ANOTHER_PARTNER = 0x1,
    };

public:
    struct {
        int32 Effect{ -1 };                  //!< Index of the effect (See `CScripted2dEffects::GetEffect`)
        int32 WaitingTask{ -1 };             //!< Task sequence index (Used in `CTaskComplexAttractorPartnerWait`)
        int32 PartnerTask{ -1 };             //!< Task sequence index (Used in `CTaskComplexUseAttractorPartner`)
        eMode PartnerUseMode{ eMode::NONE }; //!< Use mode
    } Effects[2];
    bool UsePartnerImmediately{ false };
};

class CScriptedEffectPairs {
public:
    enum {
        MAX_NUM_EFFECT_PAIRS = 0x4,
    };

    int32 NumPairs{};
    CScriptedEffectPair Pairs[MAX_NUM_EFFECT_PAIRS]{};
};
VALIDATE_SIZE(CScriptedEffectPairs, 148);

struct tUserList_Child {
    int32 m_Id;      // eModelID
    int32 m_PedType; // ePedType
};
VALIDATE_SIZE(tUserList_Child, 0x8);

struct tUserList {
    int32 m_UserTypes[4]{ -1 };
    int32 m_UserTypesByPedType[4]{ -1 };
    bool  m_bUseList{};
};
VALIDATE_SIZE(tUserList, 0x24);

constexpr auto NUM_SCRIPTED_2D_EFFECTS = 64;

class CScripted2dEffects {
public:
    static inline auto& ms_effects               = StaticRef<std::array<C2dEffect, NUM_SCRIPTED_2D_EFFECTS>>(0xC3AB00); // actual type is `C2dEffectPedAttractor`
    static inline auto& ms_effectPairs           = StaticRef<std::array<CScriptedEffectPairs, NUM_SCRIPTED_2D_EFFECTS>>(0xC3BB00);
    static inline auto& ms_userLists             = StaticRef<std::array<tUserList, NUM_SCRIPTED_2D_EFFECTS>>(0xC3A200); // class maybe
    static inline auto& ms_activated             = StaticRef<std::array<bool, NUM_SCRIPTED_2D_EFFECTS>>(0xC3A1A0);
    static inline auto& ScriptReferenceIndex     = StaticRef<std::array<uint16, NUM_SCRIPTED_2D_EFFECTS>>(0xC3A120);
    static inline auto& ms_effectSequenceTaskIDs = StaticRef<std::array<int32, NUM_SCRIPTED_2D_EFFECTS>>(0xC3A020);
    static inline auto& ms_useAgainFlags         = StaticRef<std::array<bool, NUM_SCRIPTED_2D_EFFECTS>>(0xC39FE0);
    static inline auto& ms_radii                 = StaticRef<std::array<float, NUM_SCRIPTED_2D_EFFECTS>>(0xC39EE0);

public:
    static void InjectHooks();

    static void Init();

    static CScriptedEffectPairs* GetEffectPairs(const C2dEffectPedAttractor* effect);
    static int32 GetIndex(const C2dEffectPedAttractor* effect);

    static int32 AddScripted2DEffect(float radius = -1.f);
    static void ReturnScripted2DEffect(int32 index);

    /// Index of the effect if it's from `ms_effects` `nullopt` otherwise.
    static auto IndexOfEffect(const C2dEffectPedAttractor* effect) ->std::optional<size_t>;

    static auto GetEffect(int32 index) -> C2dEffectPedAttractor*;

    static void Load() { } // NOP
    static void Save() { } // NOP

    // NOTSA
    static int32 FindFreeSlot();
};
