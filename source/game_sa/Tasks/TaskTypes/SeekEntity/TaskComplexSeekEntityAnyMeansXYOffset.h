#pragma once

#include "Base.h"
#include "TaskComplexSeekEntityAnyMeans.h"
#include "PosCalculators/EntitySeekPosCalculatorXYOffset.h"

class NOTSA_EXPORT_VTABLE CTaskComplexSeekEntityAnyMeansXYOffset : public CTaskComplexSeekEntityAnyMeans<CEntitySeekPosCalculatorXYOffset> {
public:
    static void InjectHooks();

    CTaskComplexSeekEntityAnyMeansXYOffset(CEntity* entity, const CVector& offsetXY, int32 period = 1000, float radius = 8.f) :
        CTaskComplexSeekEntityAnyMeans{ entity, period, radius, { offsetXY } }
    {
    }
};
