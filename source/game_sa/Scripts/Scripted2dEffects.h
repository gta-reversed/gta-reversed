#pragma once

struct C2dEffect;

class CScriptedEffectPair {
public:
    struct {
        int32 Effect{ -1 };
        int32 WaitingTask{ -1 };
        int32 PartnerTask{ -1 }; //!< Sequence index (See `CTaskComplexUseSequence`)
        int32 PartnerUseMode{ -1 };
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
    static inline auto& ms_effects               = *(std::array<C2dEffect, NUM_SCRIPTED_2D_EFFECTS>*)0xC3AB00; // actual type is `C2dEffectPedAttractor`
    static inline auto& ms_effectPairs           = *(std::array<CScriptedEffectPairs, NUM_SCRIPTED_2D_EFFECTS>*)0xC3BB00;
    static inline auto& ms_userLists             = *(std::array<tUserList, NUM_SCRIPTED_2D_EFFECTS>*)0xC3A200; // class maybe
    static inline auto& ms_activated             = *(std::array<bool, NUM_SCRIPTED_2D_EFFECTS>*)0xC3A1A0;
    static inline auto& ScriptReferenceIndex     = *(std::array<uint16, NUM_SCRIPTED_2D_EFFECTS>*)0xC3A120;
    static inline auto& ms_effectSequenceTaskIDs = *(std::array<int32, NUM_SCRIPTED_2D_EFFECTS>*)0xC3A020;
    static inline auto& ms_useAgainFlags         = *(std::array<bool, NUM_SCRIPTED_2D_EFFECTS>*)0xC39FE0;
    static inline auto& ms_radii                 = *(std::array<float, NUM_SCRIPTED_2D_EFFECTS>*)0xC39EE0;

public:
    static void InjectHooks();

    static void Init();

    static CScriptedEffectPairs* GetEffectPairs(const C2dEffectPedAttractor* effect);
    static int32 GetIndex(const C2dEffectPedAttractor* effect);

    static int32 AddScripted2DEffect(float radius);
    static void ReturnScripted2DEffect(int32 index);

    /// Index of the effect if it's from `ms_effects` `nullopt` otherwise.
    static auto IndexOfEffect(const C2dEffectPedAttractor* effect) ->std::optional<size_t>;

    static auto GetEffect(int32 index) -> C2dEffectPedAttractor* {
        return notsa::cast<C2dEffectPedAttractor>(&ms_effects[index]);
    }

    static void Load() { } // NOP
    static void Save() { } // NOP

    // NOTSA
    static int32 FindFreeSlot() {
        for (auto&& [i, activated] : rngv::enumerate(ms_activated)) {
            if (!activated) {
                return (int32)i;
            }
        }

        return -1;
    }
};
