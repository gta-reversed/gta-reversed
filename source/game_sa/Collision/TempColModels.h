#pragma once

#include "ColModel.h"
#include <array>
#include "Base.h"

class CTempColModels {
public:
    static inline auto& ms_colModelBBox      = StaticRef<CColModel, 0x968A00>();
    static inline auto& ms_colModelPed1      = StaticRef<CColModel, 0x968DF0>();
    static inline auto& ms_colModelPed2      = StaticRef<CColModel, 0x968E20>();
    static inline auto& ms_colModelDoor1     = StaticRef<CColModel, 0x968E50>();
    static inline auto& ms_colModelBumper1   = StaticRef<CColModel, 0x968E80>();
    static inline auto& ms_colModelPanel1    = StaticRef<CColModel, 0x968EB0>();
    static inline auto& ms_colModelBonnet1   = StaticRef<CColModel, 0x968EE0>();
    static inline auto& ms_colModelBoot1     = StaticRef<CColModel, 0x968F10>();
    static inline auto& ms_colModelWheel1    = StaticRef<CColModel, 0x968F40>();
    static inline auto& ms_colModelBodyPart1 = StaticRef<CColModel, 0x968F70>();
    static inline auto& ms_colModelBodyPart2 = StaticRef<CColModel, 0x968FA0>();
    static inline auto& ms_colModelWeapon    = StaticRef<CColModel, 0x968FD0>();

    static constexpr auto MAX_NUM_CUTSCENE_COLMODELS = MODEL_CUTOBJ20 - MODEL_CUTOBJ01 + 1; // 20
    static inline auto&   ms_colModelCutObj          = StaticRef<std::array<CColModel, MAX_NUM_CUTSCENE_COLMODELS>, 0x968A30>();
    
public:
    static void InjectHooks();

public:
    static void Initialise();
    static void Shutdown();
};
