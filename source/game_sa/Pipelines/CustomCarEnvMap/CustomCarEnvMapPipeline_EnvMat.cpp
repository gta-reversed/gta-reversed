#include "StdInc.h"

#include "CustomCarEnvMapPipeline.h"
#include "CustomBuildingDNPipeline.h"

// 0x5D8BD0
void* CCustomCarEnvMapPipeline::pluginEnvMatConstructorCB(void* object, int32 offsetInObject, int32 sizeInObject) {
    *RWPLUGINOFFSET(CustomEnvMapPipeMaterialData*, object, offsetInObject) = &fakeEnvMapPipeMatData;
    return object;
}

// 0x5D95B0
void* CCustomCarEnvMapPipeline::pluginEnvMatDestructorCB(void* object, int32 offsetInObject, int32 sizeInObject) {
    auto matData = *RWPLUGINOFFSET(CustomEnvMapPipeMaterialData*, object, offsetInObject);
    if (matData && matData != &fakeEnvMapPipeMatData) {
        m_gEnvMapPipeMatDataPool->Delete(matData);
        *RWPLUGINOFFSET(CustomEnvMapPipeMaterialData*, object, offsetInObject) = nullptr;
    }
    return object;
}

// 0x5D9600
void* CCustomCarEnvMapPipeline::pluginEnvMatCopyConstructorCB(void* dstObject, const void* srcObject, int32 offsetInObject, int32 sizeInObject) {
    auto srcMatData = *RWPLUGINOFFSET(CustomEnvMapPipeMaterialData*, srcObject, offsetInObject);
    auto dstMatData = *RWPLUGINOFFSET(CustomEnvMapPipeMaterialData*, dstObject, offsetInObject);
    if (srcMatData) {
        if (!dstMatData) {
            dstMatData = m_gEnvMapPipeMatDataPool->New();
            *RWPLUGINOFFSET(CustomEnvMapPipeMaterialData*, dstObject, offsetInObject) = dstMatData;
            if (!dstMatData) {
                *RWPLUGINOFFSET(CustomEnvMapPipeMaterialData*, dstObject, offsetInObject) = &fakeEnvMapPipeMatData;
                return dstObject;
            }
        }
        memcpy(dstMatData, srcMatData, sizeInObject);
    }
    return dstObject;
}

// 0x5D9660
RwStream* CCustomCarEnvMapPipeline::pluginEnvMatStreamReadCB(RwStream* stream, int32 binaryLength, void* object, int32 offsetInObject, int32 sizeInObject) {
    auto* const mat = static_cast<RpMaterial*>(object);

    EnvMapPipeMaterialDataBuffer buffer{};
    assert(binaryLength == sizeof(buffer));
    RwStreamRead(stream, &buffer, binaryLength);
    const auto data = CustomEnvMapPipeMaterialData::FromBuffer(buffer);
    EnvMapPlGetData(mat) = data == fakeEnvMapPipeMatData
        ? &fakeEnvMapPipeMatData
        : new (m_gEnvMapPipeMatDataPool->New()) CustomEnvMapPipeMaterialData{data};
    return stream;
}

// 0x5D8CD0
RwStream* CCustomCarEnvMapPipeline::pluginEnvMatStreamWriteCB(RwStream* stream, int32 binaryLength, const void* object, int32 offsetInObject, int32 sizeInObject) {
    auto* const mat = static_cast<const RpMaterial*>(object);

    const auto buffer = EnvMapPlGetData(mat)->ToBuffer();
    assert(binaryLength == sizeof(buffer));
    RwStreamWrite(stream, &buffer, binaryLength);

    return stream;
}

// 0x5D8D10
int32 CCustomCarEnvMapPipeline::pluginEnvMatStreamGetSizeCB(const void* object, int32 offsetInObject, int32 sizeInObject) {
    return object
        ? sizeof(EnvMapPipeMaterialDataBuffer)
        : 0;
}

// 0x5D6F90
float CCustomCarEnvMapPipeline::GetFxEnvScaleX(RpMaterial* material) {
    auto* const data = EnvMapPlGetData(material);
    if (data) {
        return data->Scale.x;
    }
    NOTSA_UNREACHABLE();
}

// 0x5D6FC0
float CCustomCarEnvMapPipeline::GetFxEnvScaleY(RpMaterial* material) {
    auto* const data = EnvMapPlGetData(material);
    if (data) {
        return data->Scale.y;
    }
    NOTSA_UNREACHABLE();
}

// 0x5D6F40
void CCustomCarEnvMapPipeline::SetFxEnvScale(RpMaterial* material, float x, float y) {
    auto* const data = EnvMapPlGetData(material);
    if (data) {
        data->Scale = {x, y};
    }
}

// 0x5D7040
float CCustomCarEnvMapPipeline::GetFxEnvTransSclX(RpMaterial* material) {
    auto* const data = EnvMapPlGetData(material);
    if (data) {
        return data->TranslationScale.x;
    }
    NOTSA_UNREACHABLE();
}

// 0x5D7070
float CCustomCarEnvMapPipeline::GetFxEnvTransSclY(RpMaterial* material) {
    auto* const data = EnvMapPlGetData(material);
    if (data) {
        return data->TranslationScale.y;
    }
    NOTSA_UNREACHABLE();
}

// 0x5D6FF0
void CCustomCarEnvMapPipeline::SetFxEnvTransScl(RpMaterial* material, float x, float y) {
    auto* const data = EnvMapPlGetData(material);
    if (data) {
        data->TranslationScale = {x, y};
    }
}

// 0x5D8AD0
float CCustomCarEnvMapPipeline::GetFxEnvShininess(RpMaterial* material) {
    auto* const data = EnvMapPlGetData(material);
    if (data) {
        return data->Shininess;
    }
    NOTSA_UNREACHABLE();
}

// 0x5D70A0
void CCustomCarEnvMapPipeline::SetFxEnvShininess(RpMaterial* material, float value) {
    auto* const data = EnvMapPlGetData(material);
    if (data) {
        data->Shininess = value;
    }
    NOTSA_UNREACHABLE();
}

// 0x5D6F20
RwTexture* CCustomCarEnvMapPipeline::GetFxEnvTexture(RpMaterial* material) {
    auto* const data = EnvMapPlGetData(material);
    if (data) {
        return data->Texture;
    }
    NOTSA_UNREACHABLE();
}

// TODO: Hacky shit
//RwTexture* GetFXMatEnvTexture(RpMaterial* material) { // Can't use `RpMatFXMaterialGetEnvMapTexture` because it loops thru and checks extra stuff... 
//    const auto MatFXMaterialDataOffset = StaticRef<RwUInt32>(0xC9AB74);
//    auto** const data = *RWPLUGINOFFSET(RwTexture**, material, MatFXMaterialDataOffset);
//    return *(data + 1); // data[0].data.envMap;
//}

// 0x5DA230
void CCustomCarEnvMapPipeline::SetFxEnvTexture(RpMaterial* material, RwTexture* texture) {
    auto*& data = EnvMapPlGetData(material);

    if (data == &fakeEnvMapPipeMatData) {
        data = new (m_gEnvMapPipeMatDataPool->New()) CustomEnvMapPipeMaterialData{fakeEnvMapPipeMatData};
    }

    if (!data) {
        return;
    }

    if (texture) {
        data->Texture = texture;
    } else {
        data->Texture = material
            ? RpMatFXMaterialGetEnvMapTexture(material) // NOTE/TODO: If this doesn't work as expected use the hacky `GetFXMatEnvTexture` above
            : nullptr;
        if (!data->Texture || data->Shininess == 0.f) {
            m_gEnvMapPipeMatDataPool->Delete(std::exchange(data, &fakeEnvMapPipeMatData));
        }
    }

    if (auto* const t = data->Texture) {
        RwTextureSetAddressing(t, rwTEXTUREADDRESSWRAP);
        RwTextureSetFilterModeMacro(t, rwFILTERLINEAR);
    }
}

// 0x5D8BB0
void CCustomCarEnvMapPipeline::SetCustomEnvMapPipeMaterialDataDefaults(CustomEnvMapPipeMaterialData* data) {
    *data = CustomEnvMapPipeMaterialData{};
}
