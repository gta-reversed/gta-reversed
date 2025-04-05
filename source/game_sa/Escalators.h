#pragma once

#include "Escalator.h"

static constexpr size_t NUM_ESCALATORS = 32;

class CEscalators {
public:
    static inline std::array<CEscalator, NUM_ESCALATORS>& aEscalators = StaticRef<std::array<CEscalator, NUM_ESCALATORS>>(0xC6E9A8);

public:
    static void InjectHooks();

    static void Shutdown();
    static void Init();
    static void AddOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecTop, const CVector& vecEnd, bool moveDown, CEntity* entity);
    static void Update();
};
