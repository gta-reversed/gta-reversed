#pragma once

#include "PosCalculators/EntitySeekPosCalculatorStandard.h"
#include "TaskComplexSeekEntity.h"

class NOTSA_EXPORT_VTABLE CTaskComplexSeekEntityStandard : public CTaskComplexSeekEntity<CEntitySeekPosCalculatorStandard> {
public:
    static void InjectHooks();

    using CTaskComplexSeekEntity::CTaskComplexSeekEntity;
};
VALIDATE_SIZE(CTaskComplexSeekEntityStandard, 0x4C);
