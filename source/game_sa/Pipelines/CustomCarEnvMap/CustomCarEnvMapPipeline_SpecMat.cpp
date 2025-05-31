#include "StdInc.h"

#include "CustomCarEnvMapPipeline.h"

// 0x5D8D40
void* CCustomCarEnvMapPipeline::pluginSpecMatConstructorCB(void* object, int32 offsetInObject, int32 sizeInObject) {
    *RWPLUGINOFFSET(CustomSpecMapPipeMaterialData*, object, ms_specularMapPluginOffset) = nullptr;
    return object;
}

// 0x5D97D0
void* CCustomCarEnvMapPipeline::pluginSpecMatDestructorCB(void* object, int32 offsetInObject, int32 sizeInObject) {
    auto* data = *RWPLUGINOFFSET(CustomSpecMapPipeMaterialData*, object, ms_specularMapPluginOffset);
    if (data) {
        if (data->Texture) {
            RwTextureDestroy(data->Texture);
            data->Texture = nullptr;
        }
        m_gSpecMapPipeMatDataPool->Delete(data);
        data = nullptr;
    }
    return object;
}

// 0x5D9830
void* CCustomCarEnvMapPipeline::pluginSpecMatCopyConstructorCB(void* dstObject, const void* srcObject, int32 offsetInObject, int32 sizeInObject) {
    auto* const srcMat = static_cast<const RpMaterial*>(srcObject);
    auto* const dstMat = static_cast<RpMaterial*>(dstObject);

    auto* const  src = SpecMapPlGetData(srcMat);
    auto*& dst = SpecMapPlGetData(dstMat);
    if (src) {
        if (!dst) {
            dst = m_gSpecMapPipeMatDataPool->New();
        }
        dst->Specularity = src->Specularity;
        RwTextureAddRef(src->Texture);
        if (auto* const t = std::exchange(dst->Texture, src->Texture)) {
            if (notsa::IsFixBugs()) { // Memory leak
                RwTextureDestroy(t);
            }
        }
    }
    return dst;
}

// 0x5D9880
RwStream* CCustomCarEnvMapPipeline::pluginSpecMatStreamReadCB(RwStream* stream, int32 binaryLength, void* object, int32 offsetInObject, int32 sizeInObject) {
    auto* const mat = static_cast<const RpMaterial*>(object);

    assert(!SpecMapPlGetData(mat));

    SpecMapPipeMaterialDataBuffer buffer{};
    assert(binaryLength == sizeof(SpecMapPipeMaterialDataBuffer));
    RwStreamRead(stream, &buffer, binaryLength);

    if (buffer.Specularity != 0.f) {
        const auto data = CustomSpecMapPipeMaterialData::FromBuffer(buffer);
        if (data.Texture) {
            SpecMapPlGetData(mat) = new (m_gSpecMapPipeMatDataPool->New()) CustomSpecMapPipeMaterialData{data};
        }
    }

    return stream;
}

// 0x5D8D60
RwStream* CCustomCarEnvMapPipeline::pluginSpecMatStreamWriteCB(RwStream* stream, int32 binaryLength, const void* object, int32 offsetInObject, int32 sizeInObject) {
    auto* const mat = static_cast<const RpMaterial*>(object);

    auto* const data = SpecMapPlGetData(mat);
    const auto buffer = data
        ? data->ToBuffer()
        : SpecMapPipeMaterialDataBuffer{};
    assert(binaryLength == sizeof(buffer));
    RwStreamWrite(stream, &buffer, binaryLength);

    return stream;
}

// 0x5D8DD0
int32 CCustomCarEnvMapPipeline::pluginSpecMatStreamGetSizeCB(const void* object, int32 offsetInObject, int32 sizeInObject) {
    return ms_specularMapPluginOffset == -1 ? -1 : sizeof(SpecMapPipeMaterialDataBuffer);
}

// 0x5D8B90
float CCustomCarEnvMapPipeline::GetFxSpecSpecularity(const RpMaterial* material) {
    auto* const data = SpecMapPlGetData(material);
    return data ? data->Specularity : 0.0f;
}

// 0x5D8B50
RwTexture* CCustomCarEnvMapPipeline::GetFxSpecTexture(const RpMaterial* material) {
    auto* const data = SpecMapPlGetData(material);
    return data ? data->Texture : nullptr;
}

// 0x5D8B00
void CCustomCarEnvMapPipeline::SetFxSpecTexture(RpMaterial* material, RwTexture* texture) {
    auto* const data = SpecMapPlGetData(material);
    if (!data) {
        return;
    }

    if (texture) {
        if (auto* const tex = std::exchange(data->Texture, texture)) {
            RwTextureDestroy(tex);
        }
        RwTextureAddRef(texture);
    }

    if (auto* const t = data->Texture) {
        RwTextureSetAddressing(t, rwTEXTUREADDRESSCLAMP);
        RwTextureSetFilterMode(t, rwFILTERLINEAR);
    }
}

// 0x5D8B70
void CCustomCarEnvMapPipeline::SetFxSpecSpecularity(RpMaterial* material, float value) {
    auto* const data = SpecMapPlGetData(material);
    if (data) {
        data->Specularity = value;
    }
}
