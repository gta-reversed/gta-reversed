#pragma once

#include "PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.h"
#include "TaskComplexSeekEntity.h"

class NOTSA_EXPORT_VTABLE CTaskComplexSeekEntityRadiusAngleOffset : public CTaskComplexSeekEntity<CEntitySeekPosCalculatorRadiusAngleOffset> {
public:
    static void InjectHooks();

    using CTaskComplexSeekEntity::CTaskComplexSeekEntity;
};
VALIDATE_SIZE(CTaskComplexSeekEntityRadiusAngleOffset, 0x54);
