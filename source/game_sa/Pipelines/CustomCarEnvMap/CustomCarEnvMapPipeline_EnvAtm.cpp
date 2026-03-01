#include "StdInc.h"

#include "CustomCarEnvMapPipeline.h"

// 0x5D8D30
void* CCustomCarEnvMapPipeline::pluginEnvAtmConstructorCB(void* object, int32 offsetInObject, int32 sizeInObject) {
    auto* const self = static_cast<RpAtomic*>(object);

    EnvMapAtmPlGetData(self) = nullptr;
    return object;
}

// 0x5D9730
void* CCustomCarEnvMapPipeline::pluginEnvAtmDestructorCB(void* object, int32 offsetInObject, int32 sizeInObject) {
    auto* const self = static_cast<RpAtomic*>(object);

    if (auto* const data = std::exchange(EnvMapAtmPlGetData(self), nullptr)) {
        m_gEnvMapPipeAtmDataPool->Delete(data);
    }
    return object;
}

// 0x5D9780
void* CCustomCarEnvMapPipeline::pluginEnvAtmCopyConstructorCB(void* dstObject, const void* srcObject, int32 offsetInObject, int32 sizeInObject) {
    if (auto* const src = EnvMapAtmPlGetData((RpAtomic*)(srcObject))) {
        if (auto* const dst = AllocEnvMapPipeAtomicData((RpAtomic*)(dstObject))) {
            *dst = *src;
        }
    }
    return dstObject;
}

// 0x5D96F0
CustomEnvMapPipeAtomicData* CCustomCarEnvMapPipeline::AllocEnvMapPipeAtomicData(RpAtomic* atomic) {
    auto*& data = EnvMapAtmPlGetData(atomic);
    if (!data) {
        if (data = m_gEnvMapPipeAtmDataPool->New()) {
            SetCustomEnvMapPipeAtomicDataDefaults(data);
        }
    }
    return data;
}

void CCustomCarEnvMapPipeline::SetCustomEnvMapPipeAtomicDataDefaults(CustomEnvMapPipeAtomicData* data) {
    data->OffsetU = 0.0f;
    data->PrevPosY = 0.0f;
    data->PrevPosX = 0.0f;
}
