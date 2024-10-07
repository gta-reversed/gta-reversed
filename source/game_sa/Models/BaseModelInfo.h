/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "ColModel.h"
#include "KeyGen.h"
#include "Plugins\TwoDEffectPlugin\2dEffect.h"

#include "eModelID.h"

enum ModelInfoType : uint8 {
    MODEL_INFO_ATOMIC = 1,
    MODEL_INFO_TIME = 3,
    MODEL_INFO_WEAPON = 4,
    MODEL_INFO_CLUMP = 5,
    MODEL_INFO_VEHICLE = 6,
    MODEL_INFO_PED = 7,
    MODEL_INFO_LOD = 8
};

enum eModelInfoSpecialType : uint8 {
    TREE = 1,
    PALM = 2,
    GLASS_TYPE_1 = 4,
    GLASS_TYPE_2 = 5,
    TAG = 6,
    GARAGE_DOOR = 7,
    CRANE = 9,
    UNKNOWN = 10,
    BREAKABLE_STATUE = 11,
};

class CTimeInfo;

class CAtomicModelInfo;
class CClumpModelInfo;
class CDamageAtomicModelInfo;
class CLodAtomicModelInfo;
class CLodTimeModelInfo;
class CPedModelInfo;
class CTimeModelInfo;
class CVehicleModelInfo;
class CWeaponModelInfo;
struct RwObject;

// originally an abstract class
class NOTSA_EXPORT_VTABLE CBaseModelInfo {
public:
    uint32 m_nKey;
    uint16 m_nRefCount;
    int16  m_nTxdIndex;
    uint8  m_nAlpha;
    uint8  m_n2dfxCount;
    int16  m_n2dEffectIndex;
    int16  m_nObjectInfoIndex;
    union {
        uint16 m_nFlags;
        struct {
            uint8 m_nFlagsUpperByte;
            uint8 m_nFlagsLowerByte;
        };
        struct {
            /* https://github.com/multitheftauto/mtasa-blue/blob/master/Client/game_sa/CModelInfoSA.h */
            uint8 bHasBeenPreRendered : 1; // we use this because we need to apply changes only once
            uint8 bDrawLast : 1;
            uint8 bAdditiveRender : 1;
            uint8 bDontWriteZBuffer : 1;
            uint8 bDontCastShadowsOn : 1;
            uint8 bDoWeOwnTheColModel : 1;
            uint8 bIsBackfaceCulled : 1;
            uint8 bIsLod : 1;

            // 1st byte
            union {
                struct { // Atomic flags
                    uint8 bIsRoad : 1;
                    uint8 bAtomicFlag0x200: 1;
                    uint8 bDontCollideWithFlyer : 1;
                    uint8 nSpecialType : 4;
                    uint8 bWetRoadReflection : 1;
                };
                struct { // Vehicle flags
                    uint8 bUsesVehDummy : 1;
                    uint8 : 1;
                    uint8 nCarmodId : 5;
                    uint8 bUseCommonVehicleDictionary : 1;
                };
                struct { // Clump flags
                    uint8 bHasAnimBlend : 1;
                    uint8 bHasComplexHierarchy : 1;
                    uint8 bAnimSomething : 1;
                    uint8 bOwnsCollisionModel : 1;
                    uint8 : 3;
                    uint8 bTagDisabled : 1;
                };
            };
        };
    };
    CColModel* m_pColModel;     // 20
    float      m_fDrawDistance; // 24
    union {
        RwObject* m_pRwObject;
        RpClump*  m_pRwClump;
        RpAtomic* m_pRwAtomic;
    };

public:
    CBaseModelInfo();
    virtual ~CBaseModelInfo() { assert(0); }

    virtual CAtomicModelInfo* AsAtomicModelInfoPtr();
    virtual CDamageAtomicModelInfo* AsDamageAtomicModelInfoPtr();
    virtual CLodAtomicModelInfo* AsLodAtomicModelInfoPtr();
    virtual ModelInfoType GetModelType() = 0;
    virtual CTimeInfo* GetTimeInfo();
    virtual void Init();
    virtual void Shutdown();
    virtual void DeleteRwObject() = 0;
    virtual uint32 GetRwModelType() = 0;
    virtual RwObject* CreateInstance() = 0;                 // todo: check order
    virtual RwObject* CreateInstance(RwMatrix* matrix) = 0; // todo: check order
    virtual void SetAnimFile(const char* filename);
    virtual void ConvertAnimFileIndex();
    virtual int32 GetAnimFileIndex();

    void SetTexDictionary(const char* txdName);
    void ClearTexDictionary();
    void AddTexDictionaryRef();
    void RemoveTexDictionaryRef();
    void AddRef();
    void RemoveRef();
    // initPairedModel defines if we need to set col model for time model
    void SetColModel(CColModel* colModel, bool bIsLodModel = false);
    void Init2dEffects();
    void DeleteCollisionModel();
    // index is a number of effect (max number is (m_n2dfxCount - 1))
    C2dEffect* Get2dEffect(int32 index);
    auto Get2dEffects() {
        return rng::views::iota(m_n2dfxCount) | rng::views::transform([this](size_t i) {
            return Get2dEffect((int32)i);
        });
    }
    void Add2dEffect(C2dEffect* effect);

    // Those further ones are completely inlined in final version, not present at all in android version;
    CVehicleModelInfo* AsVehicleModelInfoPtr() { return reinterpret_cast<CVehicleModelInfo*>(this); }
    CPedModelInfo*     AsPedModelInfoPtr()     { return reinterpret_cast<CPedModelInfo*>(this); }
    CWeaponModelInfo*  AsWeaponModelInfoPtr()  { return reinterpret_cast<CWeaponModelInfo*>(this); }

    [[nodiscard]] CColModel* GetColModel() const { return m_pColModel; }

    [[nodiscard]] bool GetIsDrawLast() const { return bDrawLast; }
    [[nodiscard]] bool HasBeenPreRendered() const { return bHasBeenPreRendered; }
    [[nodiscard]] bool HasComplexHierarchy() const { return bHasComplexHierarchy; }
    [[nodiscard]] bool IsBackfaceCulled() const { return bIsBackfaceCulled; }
    [[nodiscard]] bool IsLod() const { return bIsLod; }
    [[nodiscard]] bool IsRoad() const { return bIsRoad; }
    void SetHasBeenPreRendered(int32 bPreRendered) { bHasBeenPreRendered = bPreRendered; }
    void SetIsLod(bool bLod) { bIsLod = bLod; }
    void SetOwnsColModel(bool bOwns) { bDoWeOwnTheColModel = bOwns; }
    void IncreaseAlpha() {
        if (m_nAlpha >= 239)
            m_nAlpha = 255;
        else
            m_nAlpha += 16;
    };
    [[nodiscard]] auto GetModelName() const noexcept { return m_nKey; }
    void SetModelName(const char* modelName) { m_nKey = CKeyGen::GetUppercaseKey(modelName); }

    [[nodiscard]] bool IsSwayInWind1()         const { return nSpecialType == eModelInfoSpecialType::TREE; }               // 0x0800
    [[nodiscard]] bool IsSwayInWind2()         const { return nSpecialType == eModelInfoSpecialType::PALM; }               // 0x1000
    [[nodiscard]] bool SwaysInWind()           const { return IsSwayInWind1() || IsSwayInWind2(); }
    [[nodiscard]] bool IsGlassType1()          const { return nSpecialType == eModelInfoSpecialType::GLASS_TYPE_1; }       // 0x2000
    [[nodiscard]] bool IsGlassType2()          const { return nSpecialType == eModelInfoSpecialType::GLASS_TYPE_2; }       // 0x2800
    [[nodiscard]] bool IsGlass()               const { return IsGlassType1() || IsGlassType2(); }
    [[nodiscard]] bool IsTagModel()            const { return nSpecialType == eModelInfoSpecialType::TAG; }                // 0x3000
    [[nodiscard]] bool IsGarageDoor()          const { return nSpecialType == eModelInfoSpecialType::GARAGE_DOOR; }        // 0x3800
    [[nodiscard]] bool IsBreakableStatuePart() const { return nSpecialType == eModelInfoSpecialType::BREAKABLE_STATUE; }
    [[nodiscard]] bool IsCrane()               const { return nSpecialType == eModelInfoSpecialType::CRANE; }              // 0x4800

    void SetBaseModelInfoFlags(uint32 flags); // Wrapper for the static function. I honestly think this is how they did it..

    // NOTSA helpers
    auto CreateInstanceAddRef() {
        auto* inst = CreateInstance();
        AddRef();
        return inst;
    }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};
VALIDATE_SIZE(CBaseModelInfo, 0x20);

void SetBaseModelInfoFlags(CBaseModelInfo* modelInfo, uint32 dwFlags);
