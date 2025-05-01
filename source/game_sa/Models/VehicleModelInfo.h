/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "ClumpModelInfo.h"
#include "RenderWare.h"
#include "Vector.h"
#include "Quaternion.h"
#include "RwObjectNameIdAssocation.h"
#include "RGBA.h"
#include "Pool.h"

#include "eVehicleClass.h"
#include "eVehicleType.h"
#include "eCarWheel.h"

class CAnimBlock;

// enum by forkerer (https://github.com/forkerer/)
enum eVehicleDummy {
    DUMMY_LIGHT_FRONT_MAIN      = 0,
    DUMMY_LIGHT_REAR_MAIN       = 1,

    DUMMY_LIGHT_FRONT_SECONDARY = 2,
    DUMMY_LIGHT_REAR_SECONDARY  = 3,

    DUMMY_SEAT_FRONT            = 4,
    DUMMY_SEAT_REAR             = 5,

    DUMMY_EXHAUST               = 6,
    DUMMY_ENGINE                = 7,
    DUMMY_GAS_CAP               = 8,
    DUMMY_TRAILER_ATTACH        = 9,
    DUMMY_HAND_REST             = 10,
    DUMMY_EXHAUST_SECONDARY     = 11,
    DUMMY_WING_AIR_TRAIL        = 12,
    DUMMY_VEHICLE_GUN           = 13,
};

enum eVehicleUpgradePosn {
    UPGRADE_BONNET,
    UPGRADE_BONNET_LEFT,
    UPGRADE_BONNET_RIGHT,
    UPGRADE_BONNET_DAM,
    UPGRADE_BONNET_LEFT_DAM,
    UPGRADE_BONNET_RIGHT_DAM,
    UPGRADE_SPOILER,
    UPGRADE_SPOILER_DAM,
    UPGRADE_WING_LEFT,
    UPGRADE_WING_RIGHT,
    UPGRADE_FRONTBULLBAR,
    UPGRADE_BACKBULLBAR,
    UPGRADE_LIGHTS,
    UPGRADE_LIGHTS_DAM,
    UPGRADE_ROOF,
    UPGRADE_NITRO,
};

enum class eCarColLineType : uint32 {
    IGNORED = 0,
    GLOBAL_RGB = 1,
    CAR_2COL = 2,
    CAR_4COL = 3
};

enum class eCarModsLineType : uint32 {
    IGNORED = 0,
    LINK = 1,
    MODS = 2,
    WHEEL = 3
};

enum eComponentsRules {
    ALLOW_ALWAYS = 1,
    ONLY_WHEN_RAINING = 2,
    MAYBE_HIDE = 3,
    FULL_RANDOM = 4,
};

struct tRestoreEntry {
    void* m_pAddress;
    void* m_pValue;
};
VALIDATE_SIZE(tRestoreEntry, 0x8);

union tVehicleCompsUnion {
    uint32 m_nComps;
    struct {
        uint32 nExtraA_comp1 : 4;
        uint32 nExtraA_comp2 : 4;
        uint32 nExtraA_comp3 : 4;
        uint32 : 4;

        uint32 nExtraB_comp1 : 4;
        uint32 nExtraB_comp2 : 4;
        uint32 nExtraB_comp3 : 4;
        uint32 : 4;
    };
    struct {
        uint32 nExtraAComp : 12;
        uint32 nExtraARule : 4;

        uint32 nExtraBComp : 12;
        uint32 nExtraBRule : 4;
    };

    struct {
        uint32 nExtraA : 16;
        uint32 nExtraB : 16;
    };
};
VALIDATE_SIZE(tVehicleCompsUnion, 0x4);

struct UpgradePosnDesc {
public:
    UpgradePosnDesc(){};
    ~UpgradePosnDesc(){};

public:
    CVector     m_vPosition;
    CQuaternion m_qRotation;
    int32       m_nParentComponentId;
};
VALIDATE_SIZE(UpgradePosnDesc, 0x20);

class NOTSA_EXPORT_VTABLE CVehicleModelInfo : public CClumpModelInfo {
public:
    RpMaterial*        m_pPlateMaterial;
    char               m_szPlateText[9];
    uint8              m_nPlateType;
    char               m_szGameName[8];
    eVehicleType       m_nVehicleType;
    float              m_fWheelSizeFront;
    float              m_fWheelSizeRear;
    int16              m_nWheelModelIndex;
    uint16             m_nHandlingId;
    uint8              m_nNumDoors;
    eVehicleClass      m_nVehicleClass;
    uint8              m_nFlags;
    uint8              m_nWheelUpgradeClass;
    uint8              m_nTimesUsed;
    char               field_51;
    uint16             m_nFrq;
    tVehicleCompsUnion m_extraComps;
    float              m_fBikeSteerAngle;

    class CVehicleStructure {
    public:
        CVehicleStructure();
        ~CVehicleStructure();
        static void* operator new(unsigned size);
        static void  operator delete(void* data);

    public:
        static constexpr int32 NUM_DUMMIES = 15;
        static constexpr int32 NUM_UPGRADES = 18;
        static constexpr int32 NUM_EXTRAS = 6;

    public:
        CVector         m_avDummyPos[NUM_DUMMIES];
        UpgradePosnDesc m_aUpgrades[NUM_UPGRADES];
        RpAtomic*       m_apExtras[NUM_EXTRAS];
        uint8           m_nNumExtras;
        uint32          m_nMaskComponentsDamagable;

    public:
        static inline CPool<CVehicleStructure>*& m_pInfoPool = *(CPool<CVehicleStructure>**)0xB4E680;

    public: // Helpers
        [[nodiscard]] bool IsDummyActive(eVehicleDummy dummy) const {
            return m_avDummyPos[dummy] != 0.0F;
        }

        [[nodiscard]] bool IsComponentDamageable(int32 nodeIndex) const {
            return m_nMaskComponentsDamagable & (1 << nodeIndex);
        }

    } * m_pVehicleStruct;

    char        field_60[464];
    RpMaterial* m_apDirtMaterials[32];
    uint8       m_anPrimaryColors[8];
    uint8       m_anSecondaryColors[8];
    uint8       m_anTertiaryColors[8];
    uint8       m_anQuaternaryColors[8];
    uint8       m_nNumColorVariations;
    uint8       m_nLastColorVariation;
    uint8       m_nCurrentPrimaryColor;
    uint8       m_nCurrentSecondaryColor;
    uint8       m_nCurrentTertiaryColor;
    uint8       m_nCurrentQuaternaryColor;
    int16       m_anUpgrades[18];
    int16       m_anRemapTxds[4];

    union {
        CAnimBlock* m_pAnimBlock;
        char* m_animBlockFileName;
        uint32 m_nAnimBlockIndex;
    };

    static class CLinkedUpgradeList {
    public:
        int16 m_anUpgrade1[30];
        int16 m_anUpgrade2[30];
        uint32 m_nLinksCount;

    public:
        // add upgrade with components upgrade1 and upgrade2
        void AddUpgradeLink(int16 upgrade1, int16 upgrade2);
        // find linked upgrade for this upgrade. In this case upgrade param could be upgrade1 or upgrade2
        int16 FindOtherUpgrade(int16 upgrade);
    } & ms_linkedUpgrades;

    // vehicle components description tables
    // static RwObjectNameIdAssocation ms_vehicleDescs[12];
    static constexpr int32 NUM_VEHICLE_MODEL_DESCS = 12;
    static RwObjectNameIdAssocation* (&ms_vehicleDescs)[NUM_VEHICLE_MODEL_DESCS]; // use eVehicleType to access

    // remap texture
    static RwTexture*(&ms_pRemapTexture);
    // vehiclelights128 texture
    static RwTexture*(&ms_pLightsTexture);
    // vehiclelightson128 texture
    static RwTexture*(&ms_pLightsOnTexture);

    // color of currently rendered car
    // static uint8 ms_currentCol[4];
    static constexpr int32 NUM_CURRENT_COLORS = 4;
    static uint8 (&ms_currentCol)[NUM_CURRENT_COLORS];

    // number of wheel upgrades available
    // static int16 ms_numWheelUpgrades[4];
    static constexpr int32 NUM_WHEELS = 4;
    static int16 (&ms_numWheelUpgrades)[NUM_WHEELS];

    static int32 (&ms_wheelFrameIDs)[NUM_WHEELS];

    // wheels upgrades data
    // static int16 ms_upgradeWheels[15][4];
    static constexpr int32 NUM_WHEEL_UPGRADES = 15;
    static int16 (&ms_upgradeWheels)[NUM_WHEEL_UPGRADES][NUM_WHEELS];

    // Light states for currently rendered car
    static constexpr int32 NUM_LIGHTS = 4;
    static uint8 (&ms_lightsOn)[NUM_LIGHTS];

    // extras ids for next-spawned car
    // static char ms_compsUsed[2];
    static constexpr int32 NUM_COMPS_USAGE = 2;
    static inline int8(&ms_compsUsed)[NUM_COMPS_USAGE] = *(int8(*)[NUM_COMPS_USAGE])0xB4E478;
    static inline int8(&ms_compsToUse)[NUM_COMPS_USAGE] = *(int8(*)[NUM_COMPS_USAGE])0x8A6458;

    // vehicle colours from carcols.dat
    // static CRGBA ms_vehicleColourTable[128];
    static constexpr int32 NUM_VEHICLE_COLORS = 128;
    static CRGBA (&ms_vehicleColourTable)[NUM_VEHICLE_COLORS];

    static RwTextureCallBackFind& SavedTextureFindCallback;

public:
    static void InjectHooks();

    CVehicleModelInfo();
    ~CVehicleModelInfo() override = default; // 0x4C5920;

    ModelInfoType GetModelType() override;
    void Init() override;
    void DeleteRwObject() override;
    RwObject* CreateInstance() override;
    void SetAnimFile(const char* filename) override;
    void ConvertAnimFileIndex() override;
    int32 GetAnimFileIndex() override;
    void SetClump(RpClump* clump) override;

    // VTable implementations

    // Class methods
    // setup model render callbacks
    void SetAtomicRenderCallbacks();
    // set component flags
    void SetVehicleComponentFlags(RwFrame* component, uint32 flags);
    // get wheel position. Wheel is wheel id [0-3]. Local - get local offset (if false it will get world position)
    void GetWheelPosn(int32 wheel, CVector& outVec, bool local) const;
    // get component local offset. Component is a frame hierarchy id. Returns true if component present
    bool GetOriginalCompPosition(CVector& outVec, int32 component);
    // get vehicle extra with rules. Returns extra id.
    int32 ChooseComponent();
    // get vehicle second extra with rules. Returns extra id.
    int32 ChooseSecondComponent();
    // check if upgrade is available
    bool IsUpgradeAvailable(eVehicleUpgradePosn upgrade);
    // set current vehicle colour for model
    void SetVehicleColour(uint8 prim, uint8 sec, uint8 tert, uint8 quat);
    // get color for car. variationShift determines how many color variations to skip.
    // For example, 1 will simply give you next color variation.
    void ChooseVehicleColour(uint8& prim, uint8& sec, uint8& tert, uint8& quat, int32 variationShift);
    // get num remaps in this model
    int32 GetNumRemaps();
    // add remap to model. Txd is id of tex dictionary.
    void AddRemap(int32 txd);
    // setups rendering pipelines for atomics in model (CCustomCarEnvMapPipeline::CustomPipeAtomicSetup)
    void SetRenderPipelines();
    // gets car plate text
    char* GetCustomCarPlateText();
    // sets plate text
    void SetCustomCarPlateText(char* text);
    // remove some unused materials in model?
    void ReduceMaterialsInVehicle();
    // setup vehicle model components
    void PreprocessHierarchy();
    // setup custom plate
    void SetCarCustomPlate();
    // disable environment map effect on model
    void DisableEnvMap();
    // setup environment map intensity for model
    void SetEnvMapCoeff(float coeff);
    // get num doors in this model
    int32 GetNumDoors();
    // get position of dummy in model-space
    auto&& GetModelDummyPosition(this auto&& self, eVehicleDummy dummy) { return self.m_pVehicleStruct->m_avDummyPos[dummy]; } // NOTSA
    // Static method's
    // setup lights states for currently rendered vehicle
    static void SetupLightFlags(class CVehicle* vehicle);
    // destroying vehiclelights textures
    static void ShutdownLightTexture();
    // find remap texture with name
    static RwTexture* FindTextureCB(const char* name);
    // start using special finding callback
    static void UseCommonVehicleTexDicationary();
    // stop using special finding callback
    static void StopUsingCommonVehicleTexDicationary();
    // set new parent frame for object. Data is actually RwFrame *
    static RwObject* MoveObjectsCB(RwObject* object, void* data);
    // change colors and settings of material according to vehicle color and lights states.  Data
    // contains pointer to restore entries
    static RpMaterial* SetEditableMaterialsCB(RpMaterial* material, void* data);
    // execute SetEditableMaterialsCB(RpMaterial *, void *) for atomic materials and also remove
    // vehicle window if needed. Data contains pointer to restore entries
    static RpAtomic* SetEditableMaterialsCB(RpAtomic* atomic, void* data);
    // execute SetEditableMaterialsCB(RpAtomic *, void *) for atomics in clump. This one is called
    // before vehicle rendering
    static void SetEditableMaterials(RpClump* clump);
    // reset materials settings. This one is called after vehicle rendering
    static void ResetEditableMaterials(RpClump* clump);
    // this is used to disable _dam atomic and "enable" _ok atomic at vehicle model setup. Data is unused
    static RpAtomic* HideDamagedAtomicCB(RpAtomic* atomic, void* data);
    // hide all atomics with state data (data is actually uint8)
    static RpAtomic* HideAllComponentsAtomicCB(RpAtomic* atomic, void* data);
    // check if material has alpha. Boolean result is stored to data (data is actually bool *)
    static RpMaterial* HasAlphaMaterialCB(RpMaterial* material, void* data);
    // setup atomic renderer. Data is unused
    static RpAtomic* SetAtomicRendererCB(RpAtomic* atomic, void* data);
    // setup heli renderer. Data is unused
    static RpAtomic* SetAtomicRendererCB_RealHeli(RpAtomic* atomic, void* data);
    // setup plane renderer. Data is unused
    static RpAtomic* SetAtomicRendererCB_Plane(RpAtomic* atomic, void* data);
    // setup boat renderer. Data is unused
    static RpAtomic* SetAtomicRendererCB_Boat(RpAtomic* atomic, void* data);
    // setup heli renderer. Data is unused
    static RpAtomic* SetAtomicRendererCB_Heli(RpAtomic* atomic, void* data);
    // setup train renderer. Data is unused
    static RpAtomic* SetAtomicRendererCB_Train(RpAtomic* atomic, void* data);
    // setup objects flag. Data is actually flag (uint16)
    static RwObject* SetAtomicFlagCB(RwObject* object, void* data);
    // clear all atomic flag. Data is actually flag (uint16)
    static RwObject* ClearAtomicFlagCB(RwObject* object, void* data);
    // adds wheel upgrade. This one is called from LoadVehicleUpgrades()
    static void AddWheelUpgrade(int32 wheelSetNumber, int32 modelId);
    // gets num upgrades for this set
    static int32 GetWheelUpgrade(int32 wheelSetNumber, int32 wheelUpgradeNumber);
    // gets wheel upgrade
    static int32 GetNumWheelUpgrades(int32 wheelSetNumber);
    // do nothing
    static void DeleteVehicleColourTextures();
    // Set vehicle dirt textures
    static void SetDirtTextures(CVehicleModelInfo* info, int32 dirtLevel);
    // unloads 'white' texture
    static void ShutdownEnvironmentMaps();
    // gets mat effect of this material. Data is actually int32 *
    static RpMaterial* GetMatFXEffectMaterialCB(RpMaterial* material, void* data);
    // sets mat effect for this material. Data is actually int32
    static RpMaterial* SetEnvironmentMapCB(RpMaterial* material, void* data);
    // sets environment map intensity. Data is actually uint32
    static RpMaterial* SetEnvMapCoeffCB(RpMaterial* material, void* data);
    // do nothing
    static RpAtomic* SetRenderPipelinesCB(RpAtomic* atomic, void* data);
    // gets max number of passengers for model
    static int32 GetMaximumNumberOfPassengersFromNumberOfDoors(int32 modelId);
    // move all objects from data (it is actually RwFrame *) to frame
    static RwFrame* CollapseFramesCB(RwFrame* frame, void* data);
    // setup environment map for atomic's materials. Data is actually int32 and it represents effect id
    static RpAtomic* SetEnvironmentMapAtomicCB(RpAtomic* atomic, void* data);
    // setup environment map intensity for atomic with data (uint32)
    static RpAtomic* SetEnvMapCoeffAtomicCB(RpAtomic* atomic, void* data);
    static void AssignRemapTxd(const char* name, int16 txdSlot);
    static RpAtomic* StoreAtomicUsedMaterialsCB(RpAtomic* atomic, void* data); // data is RpMaterialList**

    static void SetupCommonData();
    static void LoadVehicleColours();
    static void LoadVehicleUpgrades();
    // loads 'white' texture
    static void LoadEnvironmentMaps();

    // Helpers
    // ctrl+c, ctrl+v from CVehicle
    [[nodiscard]] bool IsVehicleTypeValid()     const { return m_nVehicleType != VEHICLE_TYPE_IGNORE; }
    [[nodiscard]] bool IsAutomobile()           const { return m_nVehicleType == VEHICLE_TYPE_AUTOMOBILE; }
    [[nodiscard]] bool IsMonsterTruck()         const { return m_nVehicleType == VEHICLE_TYPE_MTRUCK; }
    [[nodiscard]] bool IsQuad()                 const { return m_nVehicleType == VEHICLE_TYPE_QUAD; }
    [[nodiscard]] bool IsHeli()                 const { return m_nVehicleType == VEHICLE_TYPE_HELI; }
    [[nodiscard]] bool IsPlane()                const { return m_nVehicleType == VEHICLE_TYPE_PLANE; }
    [[nodiscard]] bool IsBoat()                 const { return m_nVehicleType == VEHICLE_TYPE_BOAT; }
    [[nodiscard]] bool IsTrain()                const { return m_nVehicleType == VEHICLE_TYPE_TRAIN; }
    [[nodiscard]] bool IsFakeAircraft()         const { return m_nVehicleType == VEHICLE_TYPE_FHELI || m_nVehicleType == VEHICLE_TYPE_FPLANE; }
    [[nodiscard]] bool IsBike()                 const { return m_nVehicleType == VEHICLE_TYPE_BIKE; }
    [[nodiscard]] bool IsBMX()                  const { return m_nVehicleType == VEHICLE_TYPE_BMX; }
    [[nodiscard]] bool IsTrailer()              const { return m_nVehicleType == VEHICLE_TYPE_TRAILER; }

    // These were probably inlined:
    void SetWheelSizes(float front, float rear) {
        m_fWheelSizeFront = front;
        m_fWheelSizeRear = rear;
    }
    void SetGameName(const char* name) {
        strcpy_s(m_szGameName, name);
    }
    void SetHandlingId(const char* handlingName);

    // These two should probably be moved to a better place..
    [[nodiscard]] bool IsFrontWheel(eCarWheel wheel) const {
        switch (wheel) {
        case eCarWheel::CAR_WHEEL_FRONT_LEFT:
        case eCarWheel::CAR_WHEEL_FRONT_RIGHT:
            return true;
        }
        return false;
    }

    [[nodiscard]] bool IsRearWheel(eCarWheel door) const {
        return !IsFrontWheel(door);
    }

    // Return size of give wheel. If it's a front wheel `m_fWheelSizeFront` is returned, otherwise `m_fWheelSizeRear`
    [[nodiscard]] float GetSizeOfWheel(eCarWheel wheel) const {
        return IsFrontWheel(wheel) ? m_fWheelSizeFront : m_fWheelSizeRear;
    }

    float GetWheelSize(bool front) { return front ? m_fWheelSizeFront : m_fWheelSizeRear; } // 0x6A06F0

    tHandlingData& GetHandlingData() const;
    tFlyingHandlingData& GetFlyingHandlingData() const;

    auto GetVehicleStruct() const { return m_pVehicleStruct; }
};
VALIDATE_SIZE(CVehicleModelInfo::CVehicleStructure, 0x314);
VALIDATE_SIZE(CVehicleModelInfo, 0x308);

static bool IsValidCompRule(int32 nRule);
static int32 ChooseComponent(int32 rule, int32 comps);
static int32 CountCompsInRule(int32 comps);
static int32 GetListOfComponentsNotUsedByRules(uint32 compRules, int32 numExtras, int32* outList);
static RpMaterial* RemoveWindowAlphaCB(RpMaterial* material, void* data); // data is RpMaterialList**
static RwObject* GetOkAndDamagedAtomicCB(RwObject* object, void* data);   // data is &RpAtomic[2]
static RpAtomic* atomicDefaultRenderCB(RpAtomic* atomic);

extern RwTexDictionary*& vehicleTxd;
extern RwFrame*& carFrame;
extern RwSurfaceProperties& gLightSurfProps;
static constexpr int32 NUM_RESTORE_ENTRIES = 256;
extern tRestoreEntry (&gRestoreEntries)[NUM_RESTORE_ENTRIES];
extern RwTexture*& gpWhiteTexture;
extern float& fEnvMapDefaultCoeff;
extern float& fRearDoubleWheelOffsetFactor;
