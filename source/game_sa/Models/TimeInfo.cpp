#include "StdInc.h"

#include "TimeInfo.h"

void CTimeInfo::InjectHooks() {
    RH_ScopedClass(CTimeInfo);
    RH_ScopedCategory("Models");

    RH_ScopedInstall(FindOtherTimeModel, 0x4C47E0);
    RH_ScopedInstall(GetOtherTimeModel, 0x4C4A30);
    RH_ScopedInstall(GetTimeOff, 0x407330);
    RH_ScopedInstall(GetTimeOn, 0x407320);
    RH_ScopedInstall(SetOtherTimeModel, 0x5B3440);
    RH_ScopedInstall(SetTimes, 0x5B3430);
}

// 0x4C47E0
CTimeInfo* CTimeInfo::FindOtherTimeModel(const char* modelName) {
    char timeSwitchModelName[24];
    strcpy_s(timeSwitchModelName, modelName);

    char* nightSuffix = strstr(timeSwitchModelName, "_nt");

    if (nightSuffix) {
        strncpy_s(nightSuffix, 4, "_dy", 4);
    }
    else {
        char* daySuffix = strstr(timeSwitchModelName, "_dy");

        if (!daySuffix)
            return nullptr;

        strncpy_s(daySuffix, 4, "_nt", 4);
    }

    int32 index = 0;
    CBaseModelInfo* modelInfo = CModelInfo::GetModelInfoFromHashKey(CKeyGen::GetUppercaseKey(timeSwitchModelName), &index);
    CTimeInfo* timeInfo = modelInfo ? modelInfo->GetTimeInfo() : nullptr;

    if (timeInfo) {
        m_nOtherTimeModel = index;
        return timeInfo;
    }

    return nullptr;
}
