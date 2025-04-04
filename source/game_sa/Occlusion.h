#pragma once

#include <span>

#include "Vector.h"
#include "ActiveOccluder.h"
#include "Occluder.h"

class COcclusion {
public:
    static constexpr int32 MAX_INTERIOR_OCCLUDERS            = 40;
    static constexpr int32 MAX_MAP_OCCLUDERS                 = 1'000;
    static constexpr int32 MAX_ACTIVE_OCCLUDERS              = 28;
    static constexpr int32 NUM_OCCLUDERS_PROCESSED_PER_FRAME = 16;

    static inline auto&    InteriorOccluders                 = StaticRef<std::array<COccluder, MAX_INTERIOR_OCCLUDERS>>(0xC73CC8);
    static inline auto&    Occluders                         = StaticRef<std::array<COccluder, MAX_MAP_OCCLUDERS>>(0xC73FA0);
    static inline auto&    ActiveOccluders                   = StaticRef<std::array<CActiveOccluder, MAX_ACTIVE_OCCLUDERS>>(0xC78610);

    static inline auto&    NumInteriorOccludersOnMap         = StaticRef<size_t>(0xC73CC4);
    static inline auto&    NumOccludersOnMap                 = StaticRef<size_t>(0xC73F98);
    static inline auto&    NumActiveOccluders                = StaticRef<size_t>(0xC73CC0);

    static inline auto&    NearbyList                        = StaticRef<int16>(0x8D5D6C);

    static inline auto&    FarAwayList                       = StaticRef<int16>(0x8D5D68);
    static inline auto&    ListWalkThroughFA                 = StaticRef<int16>(0x8D5D70);
    static inline auto&    PreviousListWalkThroughFA         = StaticRef<int16>(0x8D5D74);

public:
    static void InjectHooks();

public:
    static void Init();
    static void AddOne(float centerX, float centerY, float centerZ, float width, float length, float height, float rotZ, float rotY, float rotX, uint32 flags, bool isInterior);
    static bool OccluderHidesBehind(CActiveOccluder* first, CActiveOccluder* second);
    static bool IsPositionOccluded(CVector vecPos, float fRadius);
    static void ProcessBeforeRendering();
    static auto GetActiveOccluders() { return ActiveOccluders | rng::views::take((size_t)NumActiveOccluders); }
};
