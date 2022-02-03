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
    // When the first half is finished loading, and `ProcessLoadingChannel` is called
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

enum eResourceFirstID : uint32 {
    // First ID of the resource
    RESOURCE_ID_DFF = 0,                                            // default: 0
    RESOURCE_ID_TXD = RESOURCE_ID_DFF + TOTAL_DFF_MODEL_IDS,        // default: 20000
    RESOURCE_ID_COL = RESOURCE_ID_TXD + TOTAL_TXD_MODEL_IDS,        // default: 25000
    RESOURCE_ID_IPL = RESOURCE_ID_COL + TOTAL_COL_MODEL_IDS,        // default: 25255
    RESOURCE_ID_DAT = RESOURCE_ID_IPL + TOTAL_IPL_MODEL_IDS,        // default: 25511
    RESOURCE_ID_IFP = RESOURCE_ID_DAT + TOTAL_DAT_MODEL_IDS,        // default: 25575
    RESOURCE_ID_RRR = RESOURCE_ID_IFP + TOTAL_IFP_MODEL_IDS,        // default: 25755   (vehicle recordings)
    RESOURCE_ID_SCM = RESOURCE_ID_RRR + TOTAL_RRR_MODEL_IDS,        // default: 26230   (streamed scripts)
    RESOURCE_ID_INTERNAL_1 = RESOURCE_ID_SCM + TOTAL_SCM_MODEL_IDS, // default: 26312
    RESOURCE_ID_INTERNAL_2 = RESOURCE_ID_INTERNAL_1 + 1,            // default: 26313
    RESOURCE_ID_INTERNAL_3 = RESOURCE_ID_INTERNAL_2 + 1,            // default: 26314
    RESOURCE_ID_INTERNAL_4 = RESOURCE_ID_INTERNAL_3 + 1,            // default: 26315
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


inline bool IsModelDFF(uint32 model) { return eResourceFirstID::RESOURCE_ID_DFF <= model && model < eResourceFirstID::RESOURCE_ID_TXD; }
inline bool IsModelTXD(uint32 model) { return eResourceFirstID::RESOURCE_ID_TXD <= model && model < eResourceFirstID::RESOURCE_ID_COL; }
inline bool IsModelCOL(uint32 model) { return eResourceFirstID::RESOURCE_ID_COL <= model && model < eResourceFirstID::RESOURCE_ID_IPL; }
inline bool IsModelIPL(uint32 model) { return eResourceFirstID::RESOURCE_ID_IPL <= model && model < eResourceFirstID::RESOURCE_ID_DAT; }
inline bool IsModelDAT(uint32 model) { return eResourceFirstID::RESOURCE_ID_DAT <= model && model < eResourceFirstID::RESOURCE_ID_IFP; }
inline bool IsModelIFP(uint32 model) { return eResourceFirstID::RESOURCE_ID_IFP <= model && model < eResourceFirstID::RESOURCE_ID_RRR; }
inline bool IsModelRRR(uint32 model) { return eResourceFirstID::RESOURCE_ID_RRR <= model && model < eResourceFirstID::RESOURCE_ID_SCM; }
inline bool IsModelSCM(uint32 model) { return eResourceFirstID::RESOURCE_ID_SCM <= model && model < eResourceFirstID::RESOURCE_ID_INTERNAL_1; }

inline eModelType GetModelType(uint32 model) {
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
inline uint32 DFFToModelId(uint32 relativeId) { return (uint32)eResourceFirstID::RESOURCE_ID_DFF + relativeId; }
inline uint32 TXDToModelId(uint32 relativeId) { return (uint32)eResourceFirstID::RESOURCE_ID_TXD + relativeId; }
inline uint32 COLToModelId(uint32 relativeId) { return (uint32)eResourceFirstID::RESOURCE_ID_COL + relativeId; }
inline uint32 IPLToModelId(uint32 relativeId) { return (uint32)eResourceFirstID::RESOURCE_ID_IPL + relativeId; }
inline uint32 DATToModelId(uint32 relativeId) { return (uint32)eResourceFirstID::RESOURCE_ID_DAT + relativeId; }
inline uint32 IFPToModelId(uint32 relativeId) { return (uint32)eResourceFirstID::RESOURCE_ID_IFP + relativeId; }
inline uint32 RRRToModelId(uint32 relativeId) { return (uint32)eResourceFirstID::RESOURCE_ID_RRR + relativeId; }
inline uint32 SCMToModelId(uint32 relativeId) { return (uint32)eResourceFirstID::RESOURCE_ID_SCM + relativeId; }

// Turn absolute IDs into relative ones
inline uint32 ModelIdToDFF(uint32 absId) { return absId - (uint32)eResourceFirstID::RESOURCE_ID_DFF; }
inline uint32 ModelIdToTXD(uint32 absId) { return absId - (uint32)eResourceFirstID::RESOURCE_ID_TXD; }
inline uint32 ModelIdToCOL(uint32 absId) { return absId - (uint32)eResourceFirstID::RESOURCE_ID_COL; }
inline uint32 ModelIdToIPL(uint32 absId) { return absId - (uint32)eResourceFirstID::RESOURCE_ID_IPL; }
inline uint32 ModelIdToDAT(uint32 absId) { return absId - (uint32)eResourceFirstID::RESOURCE_ID_DAT; }
inline uint32 ModelIdToIFP(uint32 absId) { return absId - (uint32)eResourceFirstID::RESOURCE_ID_IFP; }
inline uint32 ModelIdToRRR(uint32 absId) { return absId - (uint32)eResourceFirstID::RESOURCE_ID_RRR; }
inline uint32 ModelIdToSCM(uint32 absId) { return absId - (uint32)eResourceFirstID::RESOURCE_ID_SCM; }

struct tRwStreamInitializeData {
    uint8* m_pBuffer;
    uint32 m_uiBufferSize;
};

VALIDATE_SIZE(tRwStreamInitializeData, 0x8);

struct tStreamingFileDesc {
    tStreamingFileDesc() = default;

    tStreamingFileDesc(const char* name, bool bNotPlayerImg) :
          m_bNotPlayerImg(bNotPlayerImg),
          m_StreamHandle(CdStreamOpen(name))
    {
        strncpy_s(m_szName, name, std::size(m_szName));
    }

    bool IsInUse() const noexcept {
        return m_szName[0];
    }

    char  m_szName[40]{}; // If this string is empty (eg.: first elem in array is NULL) the entry isnt in use
    bool  m_bNotPlayerImg{};
    int32 m_StreamHandle{-1};
};

VALIDATE_SIZE(tStreamingFileDesc, 0x30);

struct tStreamingChannel {
    int32               modelIds[16];
    int32               modelStreamingBufferOffsets[16];
    eChannelState       LoadStatus;
    int32               iLoadingLevel; // the value gets modified, but it's not used
    int32               offsetAndHandle;
    int32               sectorCount;
    int32               totalTries;
    eCdStreamStatus     m_nCdStreamStatus;

    bool IsReading() const noexcept {
        return LoadStatus == eChannelState::READING;
    }
    bool IsStarted() const noexcept {
        return LoadStatus == eChannelState::STARTED;
    }

};

VALIDATE_SIZE(tStreamingChannel, 0x98);

static bool IsModelDFF(int32 model) {
    return (uint32)model < RESOURCE_ID_DFF + TOTAL_DFF_MODEL_IDS;
}

class CStreaming {
public:
    static const int32 TOTAL_SPECIAL_MODELS = 10;
    static const int32 TOTAL_CLOTHES_MODELS = 10;
    static const int32 TOTAL_CUTSCENE_MODELS = 20;
    static const int32 SPECIAL_MODELS_RESOURCE_ID = 290;
    static const int32 CLOTHES_MODELS_RESOURCE_ID = 384;
    static const int32 CUTSCENE_MODELS_RESOURCE_ID = 300;
    static uint32& ms_memoryAvailable;
    static int32& desiredNumVehiclesLoaded;
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
    //! // initialized to -1 and never used
    static int32& ms_lastCullZone;
    static uint16& ms_loadedGangCars;
    static uint16& ms_loadedGangs;
    static int32& ms_numPedsLoaded;
    static int32(&ms_pedsLoaded)[8];
    static int32* ms_NextPedToLoadFromGroup;
    static int32& ms_currentZoneType;
    static CLoadedCarGroup& ms_vehiclesLoaded;
    static CStreamingInfo*& ms_pEndRequestedList;
    static CStreamingInfo*& ms_pStartRequestedList;
    static CStreamingInfo*& ms_pEndLoadedList;
    static CStreamingInfo*& ms_startLoadedList;
    //! initialized but not used?
    static int32& ms_lastImageRead;
    //! initialized but never used?
    static int32(&ms_imageOffsets)[6];
    static bool& ms_bEnableRequestListPurge;
    static uint32& ms_streamingBufferSize;
    static uint8** ms_pStreamingBuffer;
    static uint32& ms_memoryUsed;
    static int32& ms_numModelsRequested;
    static CStreamingInfo(&ms_aInfoForModel)[26316];
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

public:
    static void InjectHooks();

    static CStreamingInfo& GetInfo(uint32 modelId);
    static bool IsRequestListEmpty();
    static CLink<CEntity*>* AddEntity(CEntity* entity);
    //! return StreamingFile Index in CStreaming::ms_files
    static uint32 AddImageToList(const char* pFileName, bool bNotPlayerImg);
    static void AddLodsToRequestList(const CVector& point, uint32 streamingFlags);
    static void AddModelsToRequestList(const CVector& point, uint32 streamingFlags);
    static bool AddToLoadedVehiclesList(int32 modelId);
    static bool AreAnimsUsedByRequestedModels(int32 animModelId);
    static bool AreTexturesUsedByRequestedModels(int32 txdModelId);
    static void ClearFlagForAll(uint32 streamingFlag);
    static void ClearSlots(int32 totalSlots);
    static bool ConvertBufferToObject(uint8* pFileBuffer, int32 modelId);
    static void DeleteAllRwObjects();
    static bool DeleteLeastUsedEntityRwObject(bool bNotOnScreen, uint32 streamingFlags);
    static void DeleteRwObjectsAfterDeath(const CVector& point);
    static void DeleteRwObjectsBehindCamera(int32 memoryToCleanInBytes);
    static bool DeleteRwObjectsBehindCameraInSectorList(CPtrList& list, int32 memoryToCleanInBytes);
    static void DeleteRwObjectsInSectorList(CPtrList& list, int32 sectorX = -1, int32 sectorY = -1);
    static bool DeleteRwObjectsNotInFrustumInSectorList(CPtrList& list, int32 memoryToCleanInBytes);
    static bool RemoveReferencedTxds(int32 memoryToCleanInBytes);
    static void DisableCopBikes(bool bDisable);
    //! RandFactor : random number between 1-7
    static int32 FindMIPedSlotForInterior(int32 randFactor);
    static void FinishLoadingLargeFile(uint8* pFileBuffer, int32 modelId);
    static void FlushChannels();
    static void FlushRequestList();
    // Sets value of two global vars, the value is then set to false in CStreaming::RequestModelStream
    // It has no affect on the game, so let's leave it
    static void ForceLayerToRead(int32 arg1);
    static int32 GetDefaultCabDriverModel();
    static int32 GetDefaultCopCarModel(int32 ignoreLvpd1Model);
    static int32 GetDefaultCopModel();
    static int32 GetDefaultFiremanModel();
    static int32 GetDefaultMedicModel();
    //! unused
    static int32 GetDiscInDrive();
    static int32 GetNextFileOnCd(uint32 streamLastPosn, bool bNotPriority);
    static bool HasSpecialCharLoaded(int32 slot);
    static bool HasVehicleUpgradeLoaded(int32 modelId);
    //! does nothing (NOP)
    static void IHaveUsedStreamingMemory();
    //! does nothing (NOP)
    static void ImGonnaUseStreamingMemory();
    static void Init();
    static void Init2();
    static void InitImageList();
    static void InstanceLoadedModels(const CVector& point);
    static void InstanceLoadedModelsInSectorList(CPtrList& list);
    static bool IsCarModelNeededInCurrentZone(int32 modelId);
    //! unused
    static bool IsInitialised();
    static bool IsObjectInCdImage(int32 modelId);
    static bool IsVeryBusy();
    static void Load();
    static void LoadAllRequestedModels(bool bOnlyPriorityRequests);
    static void LoadCdDirectory(const char* filename, int32 archiveId);
    static void LoadCdDirectory();
    static void LoadInitialPeds();
    //! does nothing (NOP)
    static void LoadInitialVehicles();
    static void LoadInitialWeapons();
    static void LoadRequestedModels();
    static void LoadScene(const CVector& point);
    static void LoadSceneCollision(const CVector& point);
    //! unused
    static void LoadZoneVehicle(const CVector& point);
    static void MakeSpaceFor(int32 memoryToCleanInBytes);
    static void PossiblyStreamCarOutAfterCreation(int32 modelId);
    static void ProcessEntitiesInSectorList(CPtrList& list, float posX, float posY, float minX, float minY, float maxX, float maxY, float radius, int32 streamingflags);
    static void ProcessEntitiesInSectorList(CPtrList& list, int32 streamingFlags);
    static bool ProcessLoadingChannel(int32 channelIndex);
    static void PurgeRequestList();
    static void ReInit();
    static void ReadIniFile();
    static void ReclassifyLoadedCars();
    static void RemoveAllUnusedModels();
    static void RemoveBigBuildings();
    static void RemoveBuildingsNotInArea(eAreaCodes areaCode);
    static void RemoveCarModel(int32 modelId);
    static void RemoveCurrentZonesModels();
    static void RemoveDodgyPedsFromRandomSlots();
    static void RemoveEntity(CLink<CEntity*>* streamingLink);
    static void RemoveInappropriatePedModels();
    static bool RemoveLeastUsedModel(uint32 streamingFlags);
    static bool CarIsCandidateForRemoval(int32 modelId);
    static bool RemoveLoadedVehicle();
    static bool ZoneModelIsCandidateForRemoval(int32 modelId);
    static bool RemoveLoadedZoneModel();
    static void RemoveModel(int32 modelId);
    static void RemoveTxdModel(int32 modelId);
    //! does nothing (NOP)
    static void RemoveUnusedModelsInLoadedList();
    static void RenderEntity(CLink<CEntity*>* streamingLink);
    static void RequestBigBuildings(const CVector& point);
    static void RequestFile(uint32 modelId, int32 posn, uint32 size, int32 imgId, uint32 streamingFlags);
    //! unused
    static void RequestFilesInChannel(int32 channelId);
    static void RequestModel(int32 modelId, uint32 streamingFlags); // see eStreamingFlags
    static void RequestModelStream(int32 channelId);
    //! unused
    static void RequestPlayerSection(int32 modelId, const char* string, int32 streamingFlags);
    static void RequestSpecialChar(int32 modelId, const char* name, int32 flags);
    static void RequestSpecialModel(int32 modelId, const char* name, int32 flags);
    static void RequestTxdModel(int32 slot, int32 streamingFlags);
    static void RequestVehicleUpgrade(int32 modelId, uint32 streamingFlags);
    static void RetryLoadFile(int32 channelId);
    static void Save();
    static void SetLoadVehiclesInLoadScene(bool bEnable);
    static void SetMissionDoesntRequireAnim(int32 slot);
    static void SetMissionDoesntRequireModel(int32 modelId);
    static void SetMissionDoesntRequireSpecialChar(int32 slot);
    static void SetModelIsDeletable(int32 modelId);
    static void SetModelTxdIsDeletable(int32 modelId);
    static void SetModelAndItsTxdDeletable(int32 modelId);
    //! unused
    static void SetSpecialCharIsDeletable(int32 slot);
    static void Shutdown();
    static void StartRenderEntities();
    static bool StreamAmbulanceAndMedic(bool bStreamForAccident);
    static void StreamCopModels(eLevelName level);
    static bool StreamFireEngineAndFireman(bool bStreamForFire);
    static void StreamOneNewCar();
    //! interiorType : 0 - house , 1 - shop , 2 - office
    static void StreamPedsForInterior(int32 interiorType);
    static void StreamPedsIntoRandomSlots(int32 modelArray[TOTAL_LOADED_PEDS]);
    static void StreamVehiclesAndPeds();
    static void StreamVehiclesAndPeds_Always(const CVector& unused);
    static void StreamZoneModels(const CVector& unused);
    static void StreamZoneModels_Gangs(const CVector& unused);
    static void Update();
    //! unused
    static void UpdateForAnimViewer();
    static bool WeAreTryingToPhaseVehicleOut(int32 modelId);

    // Inlined or NOTSA
    static bool IsModelLoaded(int32 model) { return ms_aInfoForModel[model].m_nLoadState == eStreamingLoadState::LOADSTATE_LOADED; }
};

extern RwStream& gRwStream;
