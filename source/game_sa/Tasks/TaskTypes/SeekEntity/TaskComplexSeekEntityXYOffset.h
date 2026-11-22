#pragma once

#include "PosCalculators/EntitySeekPosCalculatorXYOffset.h"
#include "TaskComplexSeekEntity.h"

class NOTSA_EXPORT_VTABLE CTaskComplexSeekEntityXYOffset : public CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> {
public:
    static void InjectHooks();

    using CTaskComplexSeekEntity::CTaskComplexSeekEntity;
};
VALIDATE_SIZE(CTaskComplexSeekEntityXYOffset, 0x58);
