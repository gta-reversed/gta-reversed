#pragma once

#include "FxTools.h"

#include "EmissionInfo.h"
#include "MovementInfo.h"
#include "RenderInfo.h"
#include "eFxInfoType.h"

class NOTSA_EXPORT_VTABLE FxInfo_c {
protected:
    eFxInfoType m_nType;
    bool        m_bTimeModeParticle;

public:
    FxInfo_c() = default;    // 0x4A4B30
    virtual ~FxInfo_c() = 0; // 0x4A4940
    virtual void Load(FILESTREAM file, int32 version) = 0;
    virtual void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) = 0;

    friend class FxInfoManager_c;
    friend class FxEmitterBP_c;
};
VALIDATE_SIZE(FxInfo_c, 0x8);
