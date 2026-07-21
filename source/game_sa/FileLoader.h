/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"

#include "FileMgr.h"

class CColModel;
class CBoundingBox;
class CFileCarGenerator;
class CEntity;
class CFileObjectInstance;

enum eSection : uint8 {
    UNDEFINED             = 0,
    OBJECT                = 1,
    TIME_OBJECT           = 3,
    WEAPON_OBJECT         = 4,
    CLUMP_OBJECT          = 5,
    ANIMATED_CLUMP_OBJECT = 6,
    VEHICLE_OBJECT        = 7,
    PED_OBJECT            = 8,
    PATH_NODE             = 9,
    TWO_D_EFFECT          = 10,
    TXD_PARENT            = 11,
};

enum eIDE : uint8 {
    IDE_NONE,
    IDE_OBJS,
    IDE_MLO, // ?
    IDE_TOBJ,
    IDE_WEAP,
    IDE_HIER,
    IDE_ANIM,
    IDE_CARS,
    IDE_PEDS,
    IDE_PATH,
    IDE_2DFX,
    IDE_TXDP,
};

enum eIPL : uint8 {
    IPL_NONE,
    IPL_PATH,
    IPL_INST,
    IPL_MULT,
    IPL_ZONE,
    IPL_CULL,
    IPL_OCCL,
    IPL_UNK7,
    IPL_GRGE,
    IPL_ENEX,
    IPL_PICK,
    IPL_CARS,
    IPL_JUMP,
    IPL_TCYC,
    IPL_AUZO,
};

class CFileLoader {
public:
    static inline auto& ms_line = StaticRef<char[512]>(0xB71848);

public:
    static void InjectHooks();

    static void AddTexDictionaries(RwTexDictionary* dictionary, RwTexDictionary* baseDictionary);
    static void SaveTexDictionary(RwTexDictionary* dictionary, const char* filename);
    static RwTexDictionary* LoadTexDictionary(const char* filename);

    static int32 LoadAnimatedClumpObject(const char* line);
    static bool LoadAtomicFile(RwStream* stream, uint32 modelId);
    static void LoadAtomicFile(const char* filename);
    static RpClump* LoadAtomicFile2Return(const char* filename);


    //! Find first non-null, non-whitespace character
    static char* FindFirstNonNullOrWS(char* it);

    //! Find the first whitespace (or null) character
    static char* FindFirstNullOrWS(char* it);

    static char* LoadLine(auto file);
    static char* LoadLine(char*& outLine, int32& outSize);

    static void LoadAudioZone(const char* line);
    static void LoadBoundingBox(uint8* data, CBoundingBox& outBoundBox);
    static void LoadCarGenerator(CFileCarGenerator* carGen, int32 iplId);
    static void LoadCarGenerator(const char* line, int32 iplId);
    static void LoadCarPathNode(const char* line, int32 objModelIndex, int32 pathEntryIndex, bool a4);

    static bool StartLoadClumpFile(RwStream* stream, uint32 modelIndex);
    static bool FinishLoadClumpFile(RwStream* stream, uint32 modelIndex);
    static bool LoadClumpFile(RwStream* stream, uint32 modelIndex);
    static void LoadClumpFile(const char* filename);
    static int32 LoadClumpObject(const char* line);

    static bool LoadCollisionFile(uint8* data, uint32 dataSize, uint8 colId);
    static void LoadCollisionFile(const char* filename, uint8 colId);
    static bool LoadCollisionFileFirstTime(uint8* data, uint32 dataSize, uint8 colId);
    static void LoadCollisionModel(uint8* data, CColModel& outColModel);
    static void LoadCollisionModelVer2(uint8* buffer, uint32 fileSize, CColModel& cm, const char* modelName);
    static void LoadCollisionModelVer3(uint8* buffer, uint32 fileSize, CColModel& cm, const char* modelName);
    static void LoadCollisionModelVer4(uint8* buffer, uint32 fileSize, CColModel& cm, const char* modelName);

    static void LoadCullZone(const char* line);
    static void LoadEntryExit(const char* line);
    static void LoadGarage(const char* line);
    static void LoadLevel(const char* filename);

    static int32 LoadObject(const char* line);
    static void Load2dEffect(const char* line);
    static CEntity* LoadObjectInstance(CFileObjectInstance* objInstance, const char* modelName);
    static CEntity* LoadObjectInstance(const char* line);
    static void LoadOcclusionVolume(const char* line, const char* filename);
    static int32 LoadPathHeader(const char* line, int32& outPathType);
    static int32 LoadPedObject(const char* line);
    static void LoadPedPathNode(const char* line, int32 objModelIndex, int32 pathEntryIndex);
    static void LoadPickup(const char* line);
    static void LoadStuntJump(const char* line);
    static int32 LoadTXDParent(const char* line);
    static void LoadTimeCyclesModifier(const char* line);
    static int32 LoadTimeObject(const char* line);
    static int32 LoadVehicleObject(const char* line);
    static int32 LoadWeaponObject(const char* line);
    static void LoadZone(const char* line);
    static void LoadScene(const char* filename);
    static void LoadObjectTypes(const char* filename);

    static void ReloadObjectTypes(const char* arg1);
    static void ReloadPaths(const char* filename);

    static RpAtomic* FindRelatedModelInfoCB(RpAtomic* atomic, void* data);
    static RpAtomic* SetRelatedModelInfoCB(RpAtomic* atomic, void* data);
};

// global variable to be used in a callback
extern uint32& gAtomicModelId;

const char* GetFilename(const char* filepath);
void LoadingScreenLoadingFile(const char* str);

RwTexture* AddTextureCB(RwTexture* texture, void* dict);
RpAtomic* CloneAtomicToClumpCB(RpAtomic* atomic, void* data);
