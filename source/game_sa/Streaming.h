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

class CPtrList;
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


inline bool IsModelDFF(int32 model) { return RESOURCE_ID_DFF <= model && model < RESOURCE_ID_TXD; }
inline bool IsModelTXD(int32 model) { return RESOURCE_ID_TXD <= model && model < RESOURCE_ID_COL; }
inline bool IsModelCOL(int32 model) { return RESOURCE_ID_COL <= model && model < RESOURCE_ID_IPL; }
inline bool IsModelIPL(int32 model) { return RESOURCE_ID_IPL <= model && model < RESOURCE_ID_DAT; }
inline bool IsModelDAT(int32 model) { return RESOURCE_ID_DAT <= model && model < RESOURCE_ID_IFP; }
inline bool IsModelIFP(int32 model) { return RESOURCE_ID_IFP <= model && model < RESOURCE_ID_RRR; }
inline bool IsModelRRR(int32 model) { return RESOURCE_ID_RRR <= model && model < RESOURCE_ID_SCM; }
inline bool IsModelSCM(int32 model) { return RESOURCE_ID_SCM <= model && model < RESOURCE_ID_LOADED_LIST_START; }

inline eModelType GetModelType(int32 model) {
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
inline int32 DFFToModelId(int32 relativeId) { return RESOURCE_ID_DFF + relativeId; }
inline int32 TXDToModelId(int32 relativeId) { return RESOURCE_ID_TXD + relativeId; }
inline int32 COLToModelId(int32 relativeId) { return RESOURCE_ID_COL + relativeId; }
inline int32 IPLToModelId(int32 relativeId) { return RESOURCE_ID_IPL + relativeId; }
inline int32 DATToModelId(size_t relativeId) { return (size_t)RESOURCE_ID_DAT + relativeId; }
inline int32 IFPToModelId(int32 relativeId) { return RESOURCE_ID_IFP + relativeId; }
inline int32 RRRToModelId(int32 relativeId) { return RESOURCE_ID_RRR + relativeId; }
inline int32 SCMToModelId(int32 relativeId) { return RESOURCE_ID_SCM + relativeId; }

// Turn absolute IDs into relative ones
inline int32 ModelIdToDFF(int32 absId) { return absId - RESOURCE_ID_DFF; }
inline int32 ModelIdToTXD(int32 absId) { return absId - RESOURCE_ID_TXD; }
inline int32 ModelIdToCOL(int32 absId) { return absId - RESOURCE_ID_COL; }
inline int32 ModelIdToIPL(int32 absId) { return absId - RESOURCE_ID_IPL; }
inline int32 ModelIdToDAT(int32 absId) { return absId - RESOURCE_ID_DAT; }
inline int32 ModelIdToIFP(int32 absId) { return absId - RESOURCE_ID_IFP; }
inline int32 ModelIdToRRR(int32 absId) { return absId - RESOURCE_ID_RRR; }
inline int32 ModelIdToSCM(int32 absId) { return absId - RESOURCE_ID_SCM; }

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
    static size_t& ms_memoryAvailable;
    static uint32& desiredNumVehiclesLoaded;
    static bool& ms_bLoadVehiclesInLoadScene;

    static int32(&ms_aDefaultCopCarModel)[5];
    static int32& ms_DefaultCopBikeModel;
    static int32(&ms_aDefaultCopModel)[5];
    static int32& ms_DefaultCopBikerModel;

    static uint32& ms_nTimePassedSinceLastCopBikeStreamedIn;

    static int32(&ms_aDefaultAmbulanceModel)[4];
    static int32(&ms_aDefaultMedicModel)[4];
    static int32(&ms_aDefaultFireEngineModel)[4];
    static int32(&ms_aDefaultFiremanModel)[4];

    static CDirectory*& ms_pExtraObjectsDir;
    static tStreamingFileDesc (&ms_files)[TOTAL_IMG_ARCHIVES];
    static bool& ms_bLoadingBigModel;
    // There are only two channels within CStreaming::ms_channel
    static tStreamingChannel(&ms_channel)[2];
    static int32& ms_channelError;
    static bool& m_bHarvesterModelsRequested;
    static bool& m_bStreamHarvesterModelsThisFrame;
    static uint32& ms_numPriorityRequests;
    //! Initialized to -1 and never used
    static int32& ms_lastCullZone;
    static uint16& ms_loadedGangCars;
    static uint16& ms_loadedGangs;

    //! Currently loaded peds (For/from ped groups) - Prefer using `GetLoadedPeds()` to access.
    static inline auto& ms_pedsLoaded = *(std::array<eModelID, 8>*)0x8E4C00;

    //! Number of active values in `ms_pedsLoaded`
    static inline auto& ms_numPedsLoaded = *reinterpret_cast<uint32*>(0x8E4BB0);

    //! Contains the next slot, that is, index at which the next model to load of a group is.
    //! This is used by `PickPedMIToStreamInForCurrentZone`
    //! And the modelId to load can be accessed by `CPopulation::GetPedGroupModelId` for a given 
    static inline auto& ms_NextPedToLoadFromGroup = *(std::array<int32, 18>*)0x8E4BB8;

    static int32& ms_currentZoneType;
    static CLoadedCarGroup& ms_vehiclesLoaded;
    static CStreamingInfo*& ms_pEndRequestedList;
    static CStreamingInfo*& ms_pStartRequestedList;
    static CStreamingInfo*& ms_pEndLoadedList;
    static CStreamingInfo*& ms_startLoadedList;

    static int32& ms_lastImageRead; // initialized but not used?
    static int32(&ms_imageOffsets)[6]; // initialized but never used?

    static bool& ms_bEnableRequestListPurge;
    static uint32& ms_streamingBufferSize;
    static uint8* (&ms_pStreamingBuffer)[2];
    static uint32& ms_memoryUsedBytes;
    static int32& ms_numModelsRequested;
    static CStreamingInfo(&ms_aInfoForModel)[RESOURCE_ID_TOTAL];
    static bool& ms_disableStreaming;
    static int32& ms_bIsInitialised;
    static bool& m_bBoatsNeeded;
    static bool& ms_bLoadingScene;
    static bool& m_bCopBikeLoaded;
    static bool& m_bDisableCopBikes;
    static CLinkList<CEntity*>& ms_rwObjectInstances;
    static CLink<CEntity*>*& ms_renderEntityLink;
    static bool& m_bLoadingAllRequestedModels;
    static bool& m_bModelStreamNotLoaded;
    static inline bool& ms_bReadLayerForceFully = *(bool*)0x9654C4;
    static inline int32& ms_oldSectorX = *(int32*)0x8E4B98;
    static inline int32& ms_oldSectorY = *(int32*)0x8E4B94;

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
    static bool DeleteRwObjectsBehindCameraInSectorList(CPtrList& list, size_t memoryToCleanInBytes);
    static void DeleteRwObjectsInSectorList(CPtrList& list, int32 sectorX = -1, int32 sectorY = -1);
    static bool DeleteRwObjectsNotInFrustumInSectorList(CPtrList& list, size_t memoryToCleanInBytes);

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
    static void InstanceLoadedModelsInSectorList(CPtrList& list);
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
    static void ProcessEntitiesInSectorList(CPtrList& list, float posX, float posY, float minX, float minY, float maxX, float maxY, float radius, int32 flags);
    static void ProcessEntitiesInSectorList(CPtrList& list, int32 flags);
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
