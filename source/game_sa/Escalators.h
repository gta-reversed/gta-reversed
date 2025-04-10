#pragma once

#include "Escalator.h"

#define NUM_ESCALATORS 32

class CEscalators {
public:
    static CEscalator (&aEscalators)[NUM_ESCALATORS];

public:
    static void InjectHooks();

public:
    static void Shutdown();
    static void Init();
    static void AddOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecEnd, const CVector& vecTop, bool bMoveDown, CEntity* entity);
    static void Update();
};
