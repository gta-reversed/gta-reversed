/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "Pool.h"
#include "Object.h"

#define rwID_ENVMAPPLUGIN (MAKECHUNKID(rwVENDORID_DEVELOPER, 0xFC))
#define rwID_ENVMAPATMPLUGIN (MAKECHUNKID(rwVENDORID_DEVELOPER, 0xF4))
#define rwID_SPECMAPATMPLUGIN (MAKECHUNKID(rwVENDORID_DEVELOPER, 0xF6))

constexpr auto CUSTOM_CAR_ENV_MAP_PIPELINE_PLUGIN_ID = 0x53F2009A;

struct EnvMapPipeMaterialDataBuffer {
    CVector2D  Scale;
    CVector2D  TranslationScale;
    float      Shininess;
    RwTexture* Texture;
};

struct CustomEnvMapPipeMaterialData {
    FixedVector2D<int8, 8.f> Scale{1.f, 1.f};
    FixedVector2D<int8, 8.f> TranslationScale{1.f, 1.f};
    FixedFloat<int8, 255.f>  Shininess{1.f};
    RwUInt16                 RenderFrame{};
    RwTexture*               Texture{};

    static auto FromBuffer(const EnvMapPipeMaterialDataBuffer& b) { // 0x5D8BE0
        return CustomEnvMapPipeMaterialData{
            .Scale            = b.Scale,
            .TranslationScale = b.TranslationScale,
            .Shininess        = b.Shininess,
            .Texture          = b.Texture,
        };
    };

    auto ToBuffer() const {
        return EnvMapPipeMaterialDataBuffer{
            .Scale            = Scale,
            .TranslationScale = TranslationScale,
            .Shininess        = Shininess,
            .Texture          = Texture,
        };
    }

    friend bool operator==(const CustomEnvMapPipeMaterialData&, const CustomEnvMapPipeMaterialData&) = default;
};
VALIDATE_SIZE(CustomEnvMapPipeMaterialData, 0xC);

struct CustomEnvMapPipeAtomicData {
    float OffsetU;
    float PrevPosX, PrevPosY; //!< World map pos
};
VALIDATE_SIZE(CustomEnvMapPipeAtomicData, 0xC);

struct SpecMapPipeMaterialDataBuffer {
    float Specularity{};
    char TexName[24]{};
};

struct CustomSpecMapPipeMaterialData {
    float Specularity;
    RwTexture* Texture;

    static auto FromBuffer(const SpecMapPipeMaterialDataBuffer& b) {
        return CustomSpecMapPipeMaterialData{
            .Specularity = b.Specularity,
            .Texture = RwTextureRead(b.TexName, nullptr),
        };
    }

    auto ToBuffer() const {
        SpecMapPipeMaterialDataBuffer b{.Specularity = Specularity};
        strcpy_s(b.TexName, RwTextureGetName(Texture));
        return b;
    }
};
VALIDATE_SIZE(CustomSpecMapPipeMaterialData, 0x8);

typedef CPool<CustomEnvMapPipeMaterialData, CustomEnvMapPipeMaterialData>  CustomEnvMapPipeMaterialDataPool;
typedef CPool<CustomEnvMapPipeAtomicData, CustomEnvMapPipeAtomicData>    CustomEnvMapPipeAtomicDataPool;
typedef CPool<CustomSpecMapPipeMaterialData, CustomSpecMapPipeMaterialData> CustomSpecMapPipeMaterialDataPool;

class CCustomCarEnvMapPipeline {
public:
    static inline int32& ms_envMapPluginOffset = *(int32*)0x8D12C4;      // -1
    static inline int32& ms_envMapAtmPluginOffset = *(int32*)0x8D12C8;   // -1
    static inline int32& ms_specularMapPluginOffset = *(int32*)0x8D12CC; // -1

    static inline RxPipeline*& ObjPipeline = *(RxPipeline**)0xC02D24;

    static inline CustomEnvMapPipeMaterialData& fakeEnvMapPipeMatData = *(CustomEnvMapPipeMaterialData*)0xC02D18;
    static inline CustomEnvMapPipeMaterialDataPool*& m_gEnvMapPipeMatDataPool = *(CustomEnvMapPipeMaterialDataPool**)0xC02D28;
    static inline CustomEnvMapPipeAtomicDataPool*& m_gEnvMapPipeAtmDataPool = *(CustomEnvMapPipeAtomicDataPool**)0xC02D2C;
    static inline CustomSpecMapPipeMaterialDataPool*& m_gSpecMapPipeMatDataPool = *(CustomSpecMapPipeMaterialDataPool**)0xC02D30;

    static inline D3DLIGHT9& g_GameLight = *(D3DLIGHT9*)0xC02CB0;

public:
    static void InjectHooks();

    static bool RegisterPlugin();

    static bool CreatePipe();
    static void DestroyPipe();
    static RxPipeline* CreateCustomObjPipe();
    static void CustomPipeRenderCB(RwResEntry* repEntry, void* object, RwUInt8 type, RwUInt32 flags);
    static RwBool CustomPipeInstanceCB(void* object, RwResEntry* resEntry, RxD3D9AllInOneInstanceCallBack instanceCallback);

    static RpMaterial* CustomPipeMaterialSetup(RpMaterial* material, void* data);
    static RpAtomic* CustomPipeAtomicSetup(RpAtomic* atomic);

    static void PreRenderUpdate();
    static CustomEnvMapPipeMaterialData* DuplicateCustomEnvMapPipeMaterialData(CustomEnvMapPipeMaterialData** data);

    static CustomEnvMapPipeAtomicData* AllocEnvMapPipeAtomicData(RpAtomic* atomic);
    static void SetCustomEnvMapPipeAtomicDataDefaults(CustomEnvMapPipeAtomicData* data);
    static void SetCustomEnvMapPipeMaterialDataDefaults(CustomEnvMapPipeMaterialData* data);

    // Env Mat
    static auto*& EnvMapPlGetData(RpMaterial* obj) { return *RWPLUGINOFFSET(CustomEnvMapPipeMaterialData*, obj, ms_envMapPluginOffset); }
    static auto*& EnvMapPlGetData(const RpMaterial* obj) { return *RWPLUGINOFFSETCONST(CustomEnvMapPipeMaterialData*, obj, ms_envMapPluginOffset); }

    static void* pluginEnvMatConstructorCB(void* object, int32 offsetInObject, int32 sizeInObject);
    static void* pluginEnvMatDestructorCB(void* object, int32 offsetInObject, int32 sizeInObject);
    static void* pluginEnvMatCopyConstructorCB(void* dstObject, const void* srcObject, int32 offsetInObject, int32 sizeInObject);
    static RwStream* pluginEnvMatStreamReadCB(RwStream* stream, int32 binaryLength, void* object, int32 offsetInObject, int32 sizeInObject);
    static RwStream* pluginEnvMatStreamWriteCB(RwStream* stream, int32 binaryLength, const void* object, int32 offsetInObject, int32 sizeInObject);
    static int32 pluginEnvMatStreamGetSizeCB(const void* object, int32 offsetInObject, int32 sizeInObject);

    static float GetFxEnvScaleX(RpMaterial* material);
    static float GetFxEnvScaleY(RpMaterial* material);
    static void SetFxEnvScale(RpMaterial* material, float x, float y);

    static float GetFxEnvTransSclX(RpMaterial* material);
    static float GetFxEnvTransSclY(RpMaterial* material);
    static void SetFxEnvTransScl(RpMaterial* material, float x, float y);

    static float GetFxEnvShininess(RpMaterial* material);
    static void SetFxEnvShininess(RpMaterial* material, float value);

    static RwTexture* GetFxEnvTexture(RpMaterial* material);
    static void SetFxEnvTexture(RpMaterial* material, RwTexture* texture);

    // Env Atm
    static auto*& EnvMapAtmPlGetData(RpAtomic* obj) { return *RWPLUGINOFFSET(CustomEnvMapPipeAtomicData*, obj, ms_envMapAtmPluginOffset); }
    static auto*& EnvMapAtmPlGetData(const RpAtomic* obj) { return *RWPLUGINOFFSETCONST(CustomEnvMapPipeAtomicData*, obj, ms_envMapAtmPluginOffset); }

    static void* pluginEnvAtmConstructorCB(void* object, int32 offsetInObject, int32 sizeInObject);
    static void* pluginEnvAtmDestructorCB(void* object, int32 offsetInObject, int32 sizeInObject);
    static void* pluginEnvAtmCopyConstructorCB(void* dstObject, const void* srcObject, int32 offsetInObject, int32 sizeInObject);

    // Spec Mat
    static auto*& SpecMapPlGetData(RpMaterial* obj) { return *RWPLUGINOFFSET(CustomSpecMapPipeMaterialData*, obj, ms_specularMapPluginOffset); }
    static auto*& SpecMapPlGetData(const RpMaterial* obj) { return *RWPLUGINOFFSETCONST(CustomSpecMapPipeMaterialData*, obj, ms_specularMapPluginOffset); }

    static void* pluginSpecMatConstructorCB(void* object, int32 offsetInObject, int32 sizeInObject);
    static void* pluginSpecMatDestructorCB(void* object, int32 offsetInObject, int32 sizeInObject);
    static void* pluginSpecMatCopyConstructorCB(void* dstObject, const void* srcObject, int32 offsetInObject, int32 sizeInObject);
    static RwStream* pluginSpecMatStreamReadCB(RwStream* stream, int32 binaryLength, void* object, int32 offsetInObject, int32 sizeInObject);
    static RwStream* pluginSpecMatStreamWriteCB(RwStream* stream, int32 binaryLength, const void* object, int32 offsetInObject, int32 sizeInObject);
    static int32 pluginSpecMatStreamGetSizeCB(const void* object, int32 offsetInObject, int32 sizeInObject);

    static float GetFxSpecSpecularity(const RpMaterial* material);
    static RwTexture* GetFxSpecTexture(const RpMaterial* material);
    static void SetFxSpecTexture(RpMaterial* material, RwTexture* texture);
    static void SetFxSpecSpecularity(RpMaterial* material, float value);
};
