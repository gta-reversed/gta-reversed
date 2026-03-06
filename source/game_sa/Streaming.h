// Working?
/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "StreamingInfo.h"
#include "RenderWare.h"
#include "LinkList.h"
#include "constants.h"
#include <CdStreamInfo.h>

class CEntity;
class CLoadedCarGroup;
class CDirectory;

enum class eChannelState
{
    // Doing nothing
    IDLE = 0,

    // Currently reading model(s)
    READING = 1,

    // A big model (also called a large file) is loaded in steps:
    // First, the variable `ms_bLoadingBigModel` is set to `true` in `RequestModelStream`.
    // When the first half has finished loading, and `ProcessLoadingChannel` is called
    // `ConvertBufferToObject` will be called by it, which will set the model's load state to
    // `LOADSTATE_FINISHING`. When the latter function returns the former checks if
    // the model's loadstate is `FINISHING`, if it is the channel's state is set to
    // `STARTED` to indicate a large model's loading has started and is yet to be finished.
    // Loading a large model is finished when `ProcessLoadingChannel`.
    // (In which case it's state still should be `STARTED`)
    STARTED = 2,

    // Also called ERROR, but that's a `windgi.h` macro
    ERR = 3,
};

enum eResourceFirstID : int32 {
    // First ID of the resource
    RESOURCE_ID_DFF                = 0,                                     // default: 0
    RESOURCE_ID_TXD                = RESOURCE_ID_DFF + TOTAL_DFF_MODEL_IDS, // default: 20000
    RESOURCE_ID_COL                = RESOURCE_ID_TXD + TOTAL_TXD_MODEL_IDS, // default: 25000
    RESOURCE_ID_IPL                = RESOURCE_ID_COL + TOTAL_COL_MODEL_IDS, // default: 25255
    RESOURCE_ID_DAT                = RESOURCE_ID_IPL + TOTAL_IPL_MODEL_IDS, // default: 25511
    RESOURCE_ID_IFP                = RESOURCE_ID_DAT + TOTAL_DAT_MODEL_IDS, // default: 25575
    RESOURCE_ID_RRR                = RESOURCE_ID_IFP + TOTAL_IFP_MODEL_IDS, // default: 25755   (vehicle recordings)
    RESOURCE_ID_SCM                = RESOURCE_ID_RRR + TOTAL_RRR_MODEL_IDS, // default: 26230   (streamed scripts)

    // Used for CStreaming lists, just search for xrefs (VS: shift f12)
    RESOURCE_ID_LOADED_LIST_START  = RESOURCE_ID_SCM + TOTAL_SCM_MODEL_IDS, // default: 26312
    RESOURCE_ID_LOADED_LIST_END    = RESOURCE_ID_LOADED_LIST_START + 1,     // default: 26313

    RESOURCE_ID_REQUEST_LIST_START = RESOURCE_ID_LOADED_LIST_END + 1,       // default: 26314
    RESOURCE_ID_REQUEST_LIST_END   = RESOURCE_ID_REQUEST_LIST_START + 1,    // default: 26315
    RESOURCE_ID_TOTAL                                               // default: 26316
};

enum class eModelType {
    DFF,
    TXD,
    COL,
    IPL,
    DAT,
    IFP,
    RRR,
    SCM,

    INTERNAL_1,
    INTERNAL_2,
    INTERNAL_3,
    INTERNAL_4
};

// Helper functions to deal with modelID's
constexpr bool IsModelDFF(int32 model) { return RESOURCE_ID_DFF <= model && model < RESOURCE_ID_TXD; }
constexpr bool IsModelTXD(int32 model) { return RESOURCE_ID_TXD <= model && model < RESOURCE_ID_COL; }
constexpr bool IsModelCOL(int32 model) { return RESOURCE_ID_COL <= model && model < RESOURCE_ID_IPL; }
constexpr bool IsModelIPL(int32 model) { return RESOURCE_ID_IPL <= model && model < RESOURCE_ID_DAT; }
constexpr bool IsModelDAT(int32 model) { return RESOURCE_ID_DAT <= model && model < RESOURCE_ID_IFP; }
constexpr bool IsModelIFP(int32 model) { return RESOURCE_ID_IFP <= model && model < RESOURCE_ID_RRR; }
constexpr bool IsModelRRR(int32 model) { return RESOURCE_ID_RRR <= model && model < RESOURCE_ID_SCM; }
constexpr bool IsModelSCM(int32 model) { return RESOURCE_ID_SCM <= model && model < RESOURCE_ID_LOADED_LIST_START; }

constexpr eModelType GetModelType(int32 model) {
    if (IsModelDFF(model))
        return eModelType::DFF;

    else if (IsModelTXD(model))
        return eModelType::TXD;

    else if (IsModelCOL(model))
        return eModelType::COL;

    else if (IsModelIPL(model))
        return eModelType::IPL;

    else if (IsModelDAT(model))
        return eModelType::DAT;

    else if (IsModelIFP(model))
        return eModelType::IFP;

    else if (IsModelRRR(model))
        return eModelType::RRR;

    else if (IsModelSCM(model))
        return eModelType::SCM;

    else {
        assert(0); // NOTSA
        return (eModelType)-1;
    }
}

// Turn relative IDs into absolute ones.
constexpr int32 DFFToModelId(int32 relativeId) { return RESOURCE_ID_DFF + relativeId; }
constexpr int32 TXDToModelId(int32 relativeId) { return RESOURCE_ID_TXD + relativeId; }
constexpr int32 COLToModelId(int32 relativeId) { return RESOURCE_ID_COL + relativeId; }
constexpr int32 IPLToModelId(int32 relativeId) { return RESOURCE_ID_IPL + relativeId; }
constexpr int32 DATToModelId(size_t relativeId) { return (size_t)RESOURCE_ID_DAT + relativeId; }
constexpr int32 IFPToModelId(int32 relativeId) { return RESOURCE_ID_IFP + relativeId; }
constexpr int32 RRRToModelId(int32 relativeId) { return RESOURCE_ID_RRR + relativeId; }
constexpr int32 SCMToModelId(int32 relativeId) { return RESOURCE_ID_SCM + relativeId; }

// Turn absolute IDs into relative ones
constexpr int32 ModelIdToDFF(int32 absId) { return absId - RESOURCE_ID_DFF; }
constexpr int32 ModelIdToTXD(int32 absId) { return absId - RESOURCE_ID_TXD; }
constexpr int32 ModelIdToCOL(int32 absId) { return absId - RESOURCE_ID_COL; }
constexpr int32 ModelIdToIPL(int32 absId) { return absId - RESOURCE_ID_IPL; }
constexpr int32 ModelIdToDAT(int32 absId) { return absId - RESOURCE_ID_DAT; }
constexpr int32 ModelIdToIFP(int32 absId) { return absId - RESOURCE_ID_IFP; }
constexpr int32 ModelIdToRRR(int32 absId) { return absId - RESOURCE_ID_RRR; }
constexpr int32 ModelIdToSCM(int32 absId) { return absId - RESOURCE_ID_SCM; }

struct tRwStreamInitializeData {
    uint8* m_pBuffer;
    uint32 m_uiBufferSize;
};

VALIDATE_SIZE(tRwStreamInitializeData, 0x8);

struct tStreamingFileDesc {
    tStreamingFileDesc() = default;

    tStreamingFileDesc(const char* name, bool bNotPlayerImg) :
          IsNotPlayerImg(bNotPlayerImg),
          StreamHandle(CdStreamOpen(name))
    {
        strncpy_s(Name, name, std::size(Name));
    }

    [[nodiscard]] bool IsInUse() const noexcept { return Name[0]; }

    char   Name[40]{}; // If this string is empty (eg.: first elem in array is NULL) the entry isnt in use
    bool   IsNotPlayerImg{};
    uint32 StreamHandle{(uint32)(-1)};
};

VALIDATE_SIZE(tStreamingFileDesc, 0x30);

struct tStreamingChannel {
    int32               modelIds[16];
    int32               modelStreamingBufferOffsets[16];
    eChannelState       LoadStatus;
    int32               loadingLevel; // the value gets modified, but it's not used
    CdStreamPos         pos;
    int32               sectorCount;
    int32               totalTries;
    eCdStreamStatus     m_nCdStreamStatus;

    [[nodiscard]] bool IsIdle() const noexcept    { return LoadStatus == eChannelState::IDLE; }
    [[nodiscard]] bool IsReading() const noexcept { return LoadStatus == eChannelState::READING; }
    [[nodiscard]] bool IsStarted() const noexcept { return LoadStatus == eChannelState::STARTED; }
};

VALIDATE_SIZE(tStreamingChannel, 0x98);

class CStreaming {
public:
    static inline auto& ms_memoryAvailable = StaticRef<size_t>(0x8A5A80); // 25'600'000 == 25.6 MB
    static inline auto& desiredNumVehiclesLoaded = StaticRef<uint32>(0x8A5A84);
    static inline auto& ms_bLoadVehiclesInLoadScene = StaticRef<bool>(0x8A5A88);

    // Default models for each level (see eLevelNames):

    static inline auto& ms_aDefaultCopCarModel = StaticRef<int32[5]>(0x8A5A8C); // Last one is bike cop, not matching any level name
    static inline auto& ms_aDefaultCopModel = StaticRef<int32[5]>(0x8A5AA0); // Last one is bike cop, not matching any level name

    static inline auto& ms_nTimePassedSinceLastCopBikeStreamedIn = StaticRef<uint32>(0x9654C0);

    static inline auto& ms_aDefaultAmbulanceModel = StaticRef<int32[4]>(0x8A5AB4);
    static inline auto& ms_aDefaultMedicModel = StaticRef<int32[4]>(0x8A5AC4);
    static inline auto& ms_aDefaultFireEngineModel = StaticRef<int32[4]>(0x8A5AD4);
    static inline auto& ms_aDefaultFiremanModel = StaticRef<int32[4]>(0x8A5AE4);

    // Default models for current level
    static int32& ms_DefaultCopBikeModel;
    static int32& ms_DefaultCopBikerModel;

    static inline auto& ms_pExtraObjectsDir = StaticRef<CDirectory*>(0x8E48D0);
    static inline auto& ms_files = StaticRef<tStreamingFileDesc[TOTAL_IMG_ARCHIVES]>(0x8E48D8);
    static inline auto& ms_bLoadingBigModel = StaticRef<bool>(0x8E4A58);
    // There are only two channels within CStreaming::ms_channel
    static inline auto& ms_channel = StaticRef<tStreamingChannel[2]>(0x8E4A60);
    static inline auto& ms_channelError = StaticRef<int32>(0x8E4B90);
    static inline auto& m_bHarvesterModelsRequested = StaticRef<bool>(0x8E4B9C);
    static inline auto& m_bStreamHarvesterModelsThisFrame = StaticRef<bool>(0x8E4B9D);
    static inline auto& ms_numPriorityRequests = StaticRef<uint32>(0x8E4BA0);
    //! Initialized to -1 and never used
    static inline auto& ms_lastCullZone = StaticRef<int32>(0x8E4BA4);
    static inline auto& ms_loadedGangCars = StaticRef<uint16>(0x8E4BA8);
    // Bitfield of gangs loaded. Each gang is a bit. (0th bit being BALLAS, following the ordering in POPCYCLE_GROUP_BALLAS)
    static inline auto& ms_loadedGangs = StaticRef<uint16>(0x8E4BAC);

    //! Currently loaded peds (For/from ped groups) - Prefer using `GetLoadedPeds()` to access.
    static inline auto& ms_pedsLoaded = StaticRef<std::array<eModelID, 8>>(0x8E4C00);

    //! Number of active values in `ms_pedsLoaded`
    static inline auto& ms_numPedsLoaded = StaticRef<uint32>(0x8E4BB0);

    //! Contains the next slot, that is, index at which the next model to load of a group is.
    //! This is used by `PickPedMIToStreamInForCurrentZone`
    //! And the modelId to load can be accessed by `CPopulation::GetPedGroupModelId` for a given 
    static inline auto& ms_NextPedToLoadFromGroup = StaticRef<std::array<int32, 18>>(0x8E4BB8);

    static inline auto& ms_currentZoneType = StaticRef<int32>(0x8E4C20);
    static inline auto& ms_vehiclesLoaded = StaticRef<CLoadedCarGroup>(0x8E4C24);
    static inline auto& ms_pEndRequestedList = StaticRef<CStreamingInfo*>(0x8E4C54);
    static inline auto& ms_pStartRequestedList = StaticRef<CStreamingInfo*>(0x8E4C58);
    static inline auto& ms_pEndLoadedList = StaticRef<CStreamingInfo*>(0x8E4C5C);
    static inline auto& ms_startLoadedList = StaticRef<CStreamingInfo*>(0x8E4C60);

    static inline auto& ms_lastImageRead = StaticRef<int32>(0x8E4C64); // initialized but not used?
    static inline auto& ms_imageOffsets = StaticRef<int32[6]>(0x8E4C8C); // initialized but never used?

    static inline auto& ms_bEnableRequestListPurge = StaticRef<bool>(0x8E4CA4);
    static inline auto& ms_streamingBufferSize = StaticRef<uint32>(0x8E4CA8);
    static inline auto& ms_pStreamingBuffer = StaticRef<uint8*[2]>(0x8E4CAC);
    static inline auto& ms_memoryUsedBytes = StaticRef<uint32>(0x8E4CB4);
    static inline auto& ms_numModelsRequested = StaticRef<int32>(0x8E4CB8);
    static inline auto& ms_aInfoForModel = StaticRef<CStreamingInfo[26316]>(0x8E4CC0);
    static inline auto& ms_disableStreaming = StaticRef<bool>(0x9654B0);
    static inline auto& ms_bIsInitialised = StaticRef<int32>(0x9654B8);
    static inline auto& m_bBoatsNeeded = StaticRef<bool>(0x9654BC);
    static inline auto& ms_bLoadingScene = StaticRef<bool>(0x9654BD);
    static inline auto& m_bCopBikeLoaded = StaticRef<bool>(0x9654BE);
    static inline auto& m_bDisableCopBikes = StaticRef<bool>(0x9654BF);
    static inline auto& ms_rwObjectInstances = StaticRef<CLinkList<CEntity*>>(0x9654F0);
    static inline auto& ms_renderEntityLink = StaticRef<CLink<CEntity*>*>(0x8E48A0);
    static inline auto& m_bLoadingAllRequestedModels = StaticRef<bool>(0x965538);
    static inline auto& m_bModelStreamNotLoaded = StaticRef<bool>(0x9654C4);
    static inline auto& ms_bReadLayerForceFully = StaticRef<bool>(0x9654C4);
    static inline auto& ms_oldSectorX = StaticRef<int32>(0x8E4B98);
    static inline auto& ms_oldSectorY = StaticRef<int32>(0x8E4B94);

public:
    static void InjectHooks();

    static CLink<CEntity*>* AddEntity(CEntity* entity);
    static uint32 AddImageToList(const char* fileName, bool bNotPlayerImg);
    static void AddLodsToRequestList(const CVector& point, int32 flags);
    static void AddModelsToRequestList(const CVector& point, int32 flags);
    static bool AddToLoadedVehiclesList(int32 modelId);
    static bool AreAnimsUsedByRequestedModels(int32 animModelId);
    static bool AreTexturesUsedByRequestedModels(int32 txdModelId);
    static void ClearFlagForAll(uint32 streamingFlag);
    static void ClearSlots(uint32 totalSlots);
    static bool ConvertBufferToObject(uint8* pFileBuffer, int32 modelId);
    static void DeleteAllRwObjects();
    static bool DeleteLeastUsedEntityRwObject(bool bNotOnScreen, int32 flags);
    static void DeleteRwObjectsAfterDeath(const CVector& point);
    static void DeleteRwObjectsBehindCamera(size_t memoryToCleanInBytes);
    template<typename PtrListType>
    static bool DeleteRwObjectsBehindCameraInSectorList(PtrListType& list, size_t memoryToCleanInBytes);
    template<typename PtrListType>
    static void DeleteRwObjectsInSectorList(PtrListType& list, int32 sectorX = -1, int32 sectorY = -1);
    template<typename PtrListType>
    static bool DeleteRwObjectsNotInFrustumInSectorList(PtrListType& list, size_t memoryToCleanInBytes);

    /*!
    * @addr 0x40D2F0
    * @brief Removes (some) unreferenced TXDs in order to reduce memory usage to be below goalMemoryUsageBytes
    * @return If the memory usage is below `goalMemoryUsageBytes`
    */
    static bool RemoveReferencedTxds(size_t goalMemoryUsageBytes);

    static void DisableCopBikes(bool bDisable);
    static int32 FindMIPedSlotForInterior(int32 randFactor);
    static void FinishLoadingLargeFile(uint8* pFileBuffer, int32 modelId);
    static void FlushChannels();
    static void FlushRequestList();
    static void ForceLayerToRead(int32 arg1);
    static int32 GetDefaultCabDriverModel();
    static eModelID GetDefaultCopCarModel(bool ignoreLvpd1Model = true);
    static eModelID GetDefaultCopModel();
    static eModelID GetDefaultFiremanModel();
    static eModelID GetDefaultMedicModel();
    static int32 GetDiscInDrive();
    static int32 GetNextFileOnCd(uint32 streamLastPosn, bool bNotPriority);
    static bool HasSpecialCharLoaded(int32 slot);
    static bool HasVehicleUpgradeLoaded(int32 modelId);

    static void Init();
    static void Init2();
    static void InitImageList();
    static void InstanceLoadedModels(const CVector& point);
    template<typename PtrListType>
    static void InstanceLoadedModelsInSectorList(PtrListType& list);
    static bool IsCarModelNeededInCurrentZone(int32 modelId);
    static bool IsInitialised();
    static bool IsObjectInCdImage(int32 modelId);
    static bool IsVeryBusy();
    static void LoadAllRequestedModels(bool bOnlyPriorityRequests);
    static void LoadCdDirectory(const char* filename, StreamingImgID img);
    static void LoadCdDirectory();
    static void LoadInitialPeds();
    static void LoadInitialVehicles();
    static void LoadInitialWeapons();
    static void LoadRequestedModels();
    static void LoadScene(const CVector& point);
    static void LoadSceneCollision(const CVector& point);
    static void LoadZoneVehicle(const CVector& point);
    static void MakeSpaceFor(size_t memoryToCleanInBytes);
    static void PossiblyStreamCarOutAfterCreation(int32 modelId);
    template<typename PtrListType>
    static void ProcessEntitiesInSectorList(PtrListType& list, float posX, float posY, float minX, float minY, float maxX, float maxY, float radius, int32 flags);
    template<typename PtrListType>
    static void ProcessEntitiesInSectorList(PtrListType& list, int32 flags);
    static bool ProcessLoadingChannel(int32 channelIndex);
    static void PurgeRequestList();
    static void ReInit();
    static void ReadIniFile();
    static void ReclassifyLoadedCars();
    static void RemoveAllUnusedModels();
    static void RemoveBigBuildings();
    static void RemoveBuildingsNotInArea(eAreaCodes areaCode);
    static void RemoveCarModel(eModelID modelId);
    static void RemoveCurrentZonesModels();
    static void RemoveDodgyPedsFromRandomSlots();
    static void RemoveEntity(CLink<CEntity*>* streamingLink);
    static void RemoveInappropriatePedModels();
    static bool RemoveLeastUsedModel(int32 flags);
    static bool CarIsCandidateForRemoval(int32 modelId);
    static bool RemoveLoadedVehicle();
    static bool RemoveLoadedZoneModel();
    static void RemoveModel(int32 modelId);
    static void RemoveTxdModel(int32 modelId);
    static void RemoveUnusedModelsInLoadedList();
    static void RenderEntity(CLink<CEntity*>* streamingLink);
    static void RequestBigBuildings(const CVector& point);
    static void RequestFile(int32 modelId, CdStreamPos posn, uint32 size, StreamingImgID imgId, int32 streamingFlags);
    static void RequestFilesInChannel(int32 channelId);
    static void RequestModel(int32 modelId, int32 flags);
    static void RequestModelStream(int32 channelId);
    static void RequestPlayerSection(int32 modelId, const char* string, int32 flags);
    static void RequestSpecialChar(int32 modelId, const char* name, int32 flags);
    static void RequestSpecialModel(int32 modelId, const char* name, int32 flags);
    static void RequestTxdModel(int32 slot, int32 flags);
    static void RequestVehicleUpgrade(int32 modelId, int32 flags);
    static void RetryLoadFile(int32 channelId);
    static void SetLoadVehiclesInLoadScene(bool bEnable);
    static void SetMissionDoesntRequireAnim(int32 slot);
    static void SetMissionDoesntRequireModel(int32 modelId);
    static void SetMissionDoesntRequireSpecialChar(int32 slot);
    static void SetModelIsDeletable(int32 modelId, bool mission = false);
    static void SetModelTxdIsDeletable(int32 modelId);
    static void SetModelAndItsTxdDeletable(int32 modelId);
    static void SetSpecialCharIsDeletable(int32 slot);
    static void Shutdown();
    static void StartRenderEntities();
    static bool StreamAmbulanceAndMedic(bool bStreamForAccident);
    static void StreamCopModels(eLevelName level);
    static bool StreamFireEngineAndFireman(bool bStreamForFire);
    static void StreamOneNewCar();
    static void StreamPedsForInterior(int32 interiorType);
    static void StreamPedsIntoRandomSlots(const int32 (&modelArray)[TOTAL_LOADED_PEDS]);
    static void StreamVehiclesAndPeds();
    static void StreamVehiclesAndPeds_Always(const CVector& unused);
    static void StreamZoneModels(const CVector& unused);
    static void StreamZoneModels_Gangs(const CVector& unused);
    static void Update();
    static void UpdateForAnimViewer();
    static bool WeAreTryingToPhaseVehicleOut(int32 modelId);

    static void UpdateMemoryUsed();
    static void IHaveUsedStreamingMemory();
    static void ImGonnaUseStreamingMemory();

    static bool Load();
    static bool Save();

    // Inlined or NOTSA
    static bool IsModelLoaded(int32 model) { return ms_aInfoForModel[model].m_LoadState == eStreamingLoadState::LOADSTATE_LOADED; }
    static CStreamingInfo& GetInfo(int32 modelId) { assert(modelId >= 0); return ms_aInfoForModel[modelId]; }
    static bool IsRequestListEmpty() { return ms_pEndRequestedList->GetPrev() == ms_pStartRequestedList; }
    static ptrdiff_t GetModelFromInfo(const CStreamingInfo* info) { return info - CStreaming::ms_aInfoForModel; }
    static auto GetLoadedPeds() { return ms_pedsLoaded | rng::views::take(ms_numPedsLoaded); }
};
