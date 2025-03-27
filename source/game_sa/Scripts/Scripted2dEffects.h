#pragma once

class C2dEffect;

struct CScriptedEffectPair {
    int32 dword0;
    int32 dword4;
    int32 dword8;
    int32 dwordC;
    int32 dword10;
    int32 dword14;
    int32 dword18;
    int32 dword1C;
    int8  byte20;

    void Flush() {
        dword0  = -1;
        dword4  = -1;
        dwordC  = -1;
        dword10 = -1;
        dword8  = -1;
        dword14 = -1;
        dword1C = -1;
        byte20  = 0;
    }
};

struct CScriptedEffectPairs {
    int32 m_nCurrentPairIndex;
    CScriptedEffectPair pair1[4];
    CScriptedEffectPair pair2[4];

    void Flush() {
        std::ranges::for_each(pair1, [](auto& pair) { pair.Flush(); });
        m_nCurrentPairIndex  = 0;
    }
};
VALIDATE_SIZE(CScriptedEffectPairs, 0x124);

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
    static inline auto& ms_effects               = *(std::array<C2dEffect, NUM_SCRIPTED_2D_EFFECTS>*)0xC3AB00; // class maybe
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

    static CScriptedEffectPairs* GetEffectPairs(const C2dEffect* effect);
    static int32 GetIndex(const C2dEffectBase* effect);

    static int32 AddScripted2DEffect(float radius);
    static void ReturnScripted2DEffect(int32 index);

    /// Index of the effect if it's from `ms_effects` `nullopt` otherwise.
    static auto IndexOfEffect(const C2dEffectBase* effect) ->std::optional<size_t>;

    static void Load() { } // NOP
    static void Save() { } // NOP

    // NOTSA
    static int32 FindFreeSlot() {
        for (auto&& [i, activated] : notsa::enumerate(ms_activated)) {
            if (!activated) {
                return (int32)i;
            }
        }

        return -1;
    }
};
