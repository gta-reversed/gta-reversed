/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "CarGenerator.h"
#include "TheCarGenerators.h"
#include "Occlusion.h"
#include "PedType.h"
#include "ColHelpers.h"
#include "TempColModels.h"
#include "PlantMgr.h"
#include "StuntJumpManager.h"
#include "EntryExitManager.h"
#include "PedStats.h"
#include "LoadingScreen.h"
#include "Garages.h"

char(&CFileLoader::ms_line)[512] = *reinterpret_cast<char(*)[512]>(0xB71848);
uint32& gAtomicModelId = *reinterpret_cast<uint32*>(0xB71840);

void LinkLods(int32 a1);

void CFileLoader::InjectHooks() {
    RH_ScopedClass(CFileLoader);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(AddTexDictionaries, 0x5B3910);
    RH_ScopedInstall(LoadTexDictionary, 0x5B3860);
    RH_ScopedOverloadedInstall(LoadAtomicFile, "stream", 0x5371F0, bool(*)(RwStream*, unsigned));
    RH_ScopedOverloadedInstall(LoadAtomicFile, "", 0x5B39D0, void(*)(const char*));
    RH_ScopedInstall(LoadAudioZone, 0x5B4D70);
    RH_ScopedOverloadedInstall(LoadCarGenerator, "0", 0x537990, void(*)(CFileCarGenerator*, int32));
    RH_ScopedOverloadedInstall(LoadCarGenerator, "1", 0x5B4740, void(*)(const char*, int32));
    RH_ScopedInstall(StartLoadClumpFile, 0x5373F0);
    RH_ScopedInstall(FinishLoadClumpFile, 0x537450);
    RH_ScopedOverloadedInstall(LoadClumpFile, "", 0x5B3A30, void (*)(const char*));
    RH_ScopedOverloadedInstall(LoadClumpFile, "1", 0x5372D0, bool (*)(RwStream*, uint32));
    RH_ScopedInstall(LoadClumpObject, 0x5B4040);
    RH_ScopedInstall(LoadCullZone, 0x5B4B40);
    RH_ScopedInstall(LoadObject, 0x5B3C60);
    RH_ScopedOverloadedInstall(LoadObjectInstance, "inst", 0x538090, CEntity * (*)(CFileObjectInstance*, const char*));
    RH_ScopedOverloadedInstall(LoadObjectInstance, "file", 0x538690, CEntity * (*)(const char*));
    RH_ScopedInstall(LoadOcclusionVolume, 0x5B4C80);
    RH_ScopedInstall(LoadPathHeader, 0x5B41C0);
    RH_ScopedInstall(LoadStuntJump, 0x5B45D0);
    RH_ScopedInstall(LoadTXDParent, 0x5B75E0);
    RH_ScopedInstall(LoadTimeCyclesModifier, 0x5B81D0);
    RH_ScopedInstall(LoadTimeObject, 0x5B3DE0);
    RH_ScopedInstall(LoadWeaponObject, 0x5B3FB0);
    RH_ScopedInstall(LoadZone, 0x5B4AB0);
    RH_ScopedInstall(FindRelatedModelInfoCB, 0x5B3930);
    RH_ScopedInstall(SetRelatedModelInfoCB, 0x537150);

    RH_ScopedOverloadedInstall(LoadCollisionFile, "Buffer", 0x538440, bool(*)(uint8*, uint32, uint8));
    RH_ScopedOverloadedInstall(LoadCollisionFile, "File", 0x5B4E60, void(*)(const char*, uint8));
    RH_ScopedInstall(LoadCollisionFileFirstTime, 0x5B5000);
    RH_ScopedInstall(LoadCollisionModel, 0x537580);
    RH_ScopedInstall(LoadCollisionModelVer2, 0x537EE0);
    RH_ScopedInstall(LoadCollisionModelVer3, 0x537CE0);
    RH_ScopedInstall(LoadCollisionModelVer4, 0x537AE0);

    RH_ScopedInstall(LoadAnimatedClumpObject, 0x5B40C0);
    RH_ScopedOverloadedInstall(LoadLine, "File", 0x536F80, char* (*)(FILESTREAM));
    RH_ScopedOverloadedInstall(LoadLine, "Bufer", 0x536FE0, char* (*)(char*&, int32&));
    RH_ScopedInstall(LoadCarPathNode, 0x5B4380);
    RH_ScopedInstall(LoadPedPathNode, 0x5B41F0);
    RH_ScopedInstall(LoadVehicleObject, 0x5B6F30);
    RH_ScopedInstall(LoadPedObject, 0x5B7420);
    RH_ScopedInstall(LoadPickup, 0x5B47B0);
    RH_ScopedInstall(LoadEntryExit, 0x5B8030);
    RH_ScopedInstall(LoadGarage, 0x5B4530);
    RH_ScopedInstall(LoadLevel, 0x5B9030);
    RH_ScopedInstall(LoadScene, 0x5B8700);
    RH_ScopedInstall(LoadObjectTypes, 0x5B8400);

    RH_ScopedGlobalInstall(LinkLods, 0x5B51E0, { .reversed = false });
}

// copy textures from dictionary to baseDictionary
// 0x5B3910
void CFileLoader::AddTexDictionaries(RwTexDictionary* dictionary, RwTexDictionary* baseDictionary) {
    RwTexDictionaryForAllTextures(baseDictionary, AddTextureCB, dictionary);
}

// save txd to file
// unused
// 0x5B38C0
void CFileLoader::SaveTexDictionary(RwTexDictionary* dictionary, const char* filename) {
    RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, filename);
    if (stream) {
        RwTexDictionaryStreamWrite(dictionary, stream);
        RwStreamClose(stream, nullptr);
    }
}

// load txd from file
// 0x5B3860
RwTexDictionary* CFileLoader::LoadTexDictionary(const char* filename) {
    RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, filename);
    if (stream) {
        if (RwStreamFindChunk(stream, rwID_TEXDICTIONARY, nullptr, nullptr)) {
            RwTexDictionary* txd = RwTexDictionaryGtaStreamRead(stream);
            RwStreamClose(stream, nullptr);
            if (txd)
                return txd; // TODO: Stream closed twice if txd is nullptr.
        }
        RwStreamClose(stream, nullptr);
    }
    return RwTexDictionaryCreate();
}

// 0x5B40C0
int32 CFileLoader::LoadAnimatedClumpObject(const char* line) {
    int32  objID{ MODEL_INVALID };
    char   modelName[24]{};
    char   txdName[24]{};
    char   animName[16]{ "null" };
    float  drawDist{ 2000.f };
    uint32 flags{};

    if (sscanf_s(line, "%d %s %s %s %f %d", &objID, SCANF_S_STR(modelName), SCANF_S_STR(txdName), SCANF_S_STR(animName), &drawDist, &flags) != 6)
        return -1;

    auto mi = CModelInfo::AddClumpModel(objID);

    mi->m_nKey = CKeyGen::GetUppercaseKey(modelName);
    mi->SetTexDictionary(txdName);
    mi->m_fDrawDistance = drawDist; // if you forgot to add this line here, you will meet with UB on closing game, have a nice day
    mi->SetAnimFile(animName);
    SetClumpModelInfoFlags(mi, flags);

    if (std::string_view{ animName } != "null")
        mi->bHasAnimBlend = true;

    return objID;
}

// 0x5371F0
bool CFileLoader::LoadAtomicFile(RwStream* stream, uint32 modelId) {
    auto mi = CModelInfo::GetModelInfo(modelId)->AsAtomicModelInfoPtr();
    bool bUseCommonVehicleTexDictionary = false;
    if (mi && mi->bUseCommonVehicleDictionary) {
        bUseCommonVehicleTexDictionary = true;
        CVehicleModelInfo::UseCommonVehicleTexDicationary();
    }

    if (RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr)) {
        RpClump* pReadClump = RpClumpStreamRead(stream);
        if (!pReadClump) {
            if (bUseCommonVehicleTexDictionary) {
                CVehicleModelInfo::StopUsingCommonVehicleTexDicationary();
            }
            return false;
        }
        gAtomicModelId = modelId;
        RpClumpForAllAtomics(pReadClump, (RpAtomicCallBack)SetRelatedModelInfoCB, pReadClump);
        RpClumpDestroy(pReadClump);
    }

    if (mi && !mi->m_pRwObject) // FIX_BUGS: V1004 The 'mi' pointer was used unsafely after it was verified against nullptr.
        return false;

    if (bUseCommonVehicleTexDictionary)
        CVehicleModelInfo::StopUsingCommonVehicleTexDicationary();

    return true;
}

// 0x5B39D0
void CFileLoader::LoadAtomicFile(const char* filename) {
    RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, filename);
    if (RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr))
    {
        if (RpClump* clump = RpClumpStreamRead(stream)) {
            RpClumpForAllAtomics(clump, FindRelatedModelInfoCB, clump);
            RpClumpDestroy(clump);
        }
    }
    RwStreamClose(stream, nullptr);
}

// unused
// 0x537060
RpClump* CFileLoader::LoadAtomicFile2Return(const char* filename) {
    RpClump* clump = nullptr;
    RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, filename);
    if (RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr))
        clump = RpClumpStreamRead(stream);
    RwStreamClose(stream, nullptr);

    return clump;
}

// NOTSA
char* CFileLoader::FindFirstNonNullOrWS(char* it) {
    // Have to cast to uint8, because signed ASCII is retarded
    for (; *it && (uint8)*it <= (uint8)' '; it++);
    return it;
}

// NOTSA
char* CFileLoader::FindFirstNullOrWS(char* it) {
    for (; *it && *it > ' '; it++);
    return it;
}

// 0x536F80
// Load line into static buffer (`ms_line`)
char* CFileLoader::LoadLine(auto file) {
    if (!CFileMgr::ReadLine(file, ms_line, sizeof(ms_line)))
        return nullptr;

    // Sanitize it (otherwise random crashes appear)
    for (char* it = ms_line; *it; it++) {
        // Have to cast to uint8, because signed ASCII is retarded
        if ((uint8)*it < (uint8)' ' || *it == ',')
            *it = ' ';
    }

    return FindFirstNonNullOrWS(ms_line);
}

/*!
* Load line from a text buffer with sanitization (replaces chars < 32 (space) with a space)
* @param bufferIt Iterator into buffer. It is modified by this function to point after the last character of this line
* @param buffSize Size of buffer. It is modified to represent the size of the buffer remaining after the end of this line
* @returns The beginning of the line - Note, this isn't a pointer into the passed in buffer!
* @addr 0x536FE0
*/
char* CFileLoader::LoadLine(char*& bufferIt, int32& buffSize) {
    if (buffSize <= 0 || !*bufferIt)
        return nullptr;

    // Copy with sanitization (Otherwise random crashes appear)
    char* copyIt = s_MemoryHeapBuffer;
    for (; *bufferIt && *bufferIt != '\n' && buffSize != 0; bufferIt++, buffSize--) {
        // Handle EOL (\r\n) correctly
        // Technically a bugfix, but can't place it under the macro
        // cause code(See `LoadCutSceneFile`) relies on it filtering `\r` even in vanilla mode
        if (*bufferIt == '\r') {
            continue;
        }
        // Have to cast to uint8, because signed ASCII is retarded
        *copyIt++ = ((uint8)*bufferIt < (uint8)' ' || *bufferIt == ',') ? ' ' : *bufferIt; // Replace chars before space and ',' (comma) by space, otherwise copy
    }

    bufferIt++;    // Make it point after end of line
    *copyIt = 0;   // Null terminate the copy

    return FindFirstNonNullOrWS(s_MemoryHeapBuffer);
}

// IPL -> AUZO
// 0x5B4D70
void CFileLoader::LoadAudioZone(const char* line) {
    char name[8];
    int32 id;
    int32 flags;
    float x1, y1, z1;
    float x2, y2, z2;
    float radius;

    if (sscanf_s(line, "%s %d %d %f %f %f %f %f %f", SCANF_S_STR(name), &id, &flags, &x1, &y1, &z1, &x2, &y2, &z2) == 9) {
        CAudioZones::RegisterAudioBox(name, id, flags == 1, {x1, y1, z1}, {x2, y2, z2});
    } else {
        VERIFY(sscanf_s(line, "%s %d %d %f %f %f %f", SCANF_S_STR(name), &id, &flags, &x1, &y1, &z1, &radius) == 7);
        CAudioZones::RegisterAudioSphere(name, id, flags == 1, {x1, y1, z1}, radius);
    }
}

// unused?
// 0x0
void CFileLoader::LoadBoundingBox(uint8* data, CBoundingBox& outBoundBox) {

}

// 0x537990
void CFileLoader::LoadCarGenerator(CFileCarGenerator* carGen, int32 iplId) {
    auto index = CTheCarGenerators::CreateCarGenerator(
        carGen->m_vecPosn,
        RadiansToDegrees(carGen->m_fAngle),
        carGen->m_nModelId,
        carGen->m_nPrimaryColor,
        carGen->m_nSecondaryColor,
        carGen->m_bForceSpawn,
        carGen->m_nAlarmChance,
        carGen->m_nDoorLockChance,
        carGen->m_nMinDelay,
        carGen->m_nMaxDelay,
        iplId,
        carGen->m_bIgnorePopulationLimit
    );
    if (index >= 0)
        CTheCarGenerators::Get(index)->SwitchOn();
}

// IPL -> CARS
// 0x5B4740
void CFileLoader::LoadCarGenerator(const char* line, int32 iplId) {
    CFileCarGenerator carGen{};
    if (sscanf_s(
        line,
        "%f %f %f %f %d %d %d %d %d %d %d %d",
        &carGen.m_vecPosn.x,
        &carGen.m_vecPosn.y,
        &carGen.m_vecPosn.z,
        &carGen.m_fAngle,
        &carGen.m_nModelId,
        &carGen.m_nPrimaryColor,
        &carGen.m_nSecondaryColor,
        &carGen.m_nFlags,
        &carGen.m_nAlarmChance,
        &carGen.m_nDoorLockChance,
        &carGen.m_nMinDelay,
        &carGen.m_nMaxDelay
    ) == 12) {
        LoadCarGenerator(&carGen, iplId);
    }
}

// 0x5B4380
void CFileLoader::LoadCarPathNode(const char* line, int32 objModelIndex, int32 pathEntryIndex, bool a4) {
    // Loads some data from the line, and calls a function which does nothing, so the whole function.. does nothing.
    // Leftover from VC
    // See code in re3
}

// 0x5373F0
bool CFileLoader::StartLoadClumpFile(RwStream* stream, uint32 modelIndex) {
    if (!RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr))
        return false;

    CBaseModelInfo* mi = CModelInfo::GetModelInfo(modelIndex);
    bool isVehicle = mi->GetModelType() == MODEL_INFO_VEHICLE;

    if (isVehicle)
        CVehicleModelInfo::UseCommonVehicleTexDicationary();

    auto clumpReaded = RpClumpGtaStreamRead1(stream);

    if (isVehicle)
        CVehicleModelInfo::StopUsingCommonVehicleTexDicationary();

    return clumpReaded;
}

// 0x537450
bool CFileLoader::FinishLoadClumpFile(RwStream* stream, uint32 modelIndex) {
    auto mi = static_cast<CClumpModelInfo*>(CModelInfo::GetModelInfo(modelIndex));
    bool isVehicle = mi->GetModelType() == MODEL_INFO_VEHICLE;

    if (isVehicle)
        CVehicleModelInfo::UseCommonVehicleTexDicationary();

    RpClump* clump = RpClumpGtaStreamRead2(stream);

    if (isVehicle)
        CVehicleModelInfo::StopUsingCommonVehicleTexDicationary();

    if (!clump)
        return false;

    mi->SetClump(clump);
    return true;
}

// 0x5372D0
bool CFileLoader::LoadClumpFile(RwStream* stream, uint32 modelIndex) {
    auto mi = static_cast<CClumpModelInfo*>(CModelInfo::GetModelInfo(modelIndex));

    if (mi->bHasComplexHierarchy) {
        RpClump* parentClump = RpClumpCreate();
        RwFrame* parentFrame = RwFrameCreate();
        RpClumpSetFrame(parentClump, parentFrame);
        if (!RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr)) {
            mi->SetClump(parentClump);
            return true;
        }

        while (true) {
            RpClump* childClump = RpClumpStreamRead(stream);
            if (!childClump)
                return false;

            RwFrame* childFrame = _rwFrameCloneAndLinkClones(RpClumpGetFrame(childClump));
            RwFrameAddChild(parentFrame, childFrame);
            RpClumpForAllAtomics(childClump, CloneAtomicToClumpCB, parentClump);
            RpClumpDestroy(childClump);

            if (!RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr)) {
                mi->SetClump(parentClump);
                return true;
            }
        }
    }

    if (!RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr))
        return false;

    bool isVehicle = mi->GetModelType() == MODEL_INFO_VEHICLE;
    if (isVehicle) {
        CCollisionPlugin::SetModelInfo(mi);
        CVehicleModelInfo::UseCommonVehicleTexDicationary();
    }

    RpClump* clump = RpClumpStreamRead(stream);

    if (isVehicle) {
        CCollisionPlugin::SetModelInfo(nullptr);
        CVehicleModelInfo::StopUsingCommonVehicleTexDicationary();
    }

    if (!clump)
        return false;

    mi->SetClump(clump);

    if (modelIndex == MODEL_JOURNEY)
        mi->AsVehicleModelInfoPtr()->m_nNumDoors = 2;

    return true;
}

// 0x5B3A30
void CFileLoader::LoadClumpFile(const char* filename) {
    RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, filename);
    while (RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr))
    {
        if (RpClump* clump = RpClumpStreamRead(stream)) {
            const char* nodeName = GetFrameNodeName(RpClumpGetFrame(clump));
            if (auto mi = static_cast<CClumpModelInfo*>(CModelInfo::GetModelInfo(nodeName, nullptr)))
                mi->SetClump(clump);
            else
                RpClumpDestroy(clump);
        }
    }
    RwStreamClose(stream, nullptr);
}

// 0x5B4040
int32 CFileLoader::LoadClumpObject(const char* line) {
    int32 objId{ MODEL_INVALID };
    char  modelName[24];
    char  texName[24];

    if (sscanf_s(line, "%d %s %s", &objId, SCANF_S_STR(modelName), SCANF_S_STR(texName)) != 3)
        return MODEL_INVALID;

    auto mi = static_cast<CVehicleModelInfo*>(CModelInfo::AddClumpModel(objId));
    mi->SetModelName(modelName);
    mi->SetTexDictionary(texName);
    mi->SetColModel(&CTempColModels::ms_colModelBBox, false);
    return objId;
}

void LoadCollisionModelAnyVersion(const ColHelpers::FileHeader& header, uint8* colData, CColModel& cm) {
    using namespace ColHelpers;

    switch (header.GetVersion()) {
    case ColModelVersion::COLL:
        CFileLoader::LoadCollisionModel(colData, cm);
        break;
    case ColModelVersion::COL2:
        CFileLoader::LoadCollisionModelVer2(colData, header.GetDataSize(), cm, header.modelName);
        break;
    case ColModelVersion::COL3:
        CFileLoader::LoadCollisionModelVer3(colData, header.GetDataSize(), cm, header.modelName);
        break;
    case ColModelVersion::COL4:
        // Originally this function didn't deal with V4.
        // But given there are no V4 col files, for simplicity we'll leave it in here.
        CFileLoader::LoadCollisionModelVer4(colData, header.GetDataSize(), cm, header.modelName);
        break;
    }
}

// 0x538440
// Load one, or multiple, collision models from the given buffer
bool CFileLoader::LoadCollisionFile(uint8* buff, uint32 buffSize, uint8 colId) {
    using namespace ColHelpers;
    assert(buffSize >= sizeof(FileHeader) && "LoadCollisionFileFirstTime called with not enough data"); // Buffer should be big enough to have at least 1 col header in it

    auto fileTotalSize{0u};
    for (auto buffPos = 0u; buffPos < buffSize; buffPos += fileTotalSize) {
        const auto buffRemainingSize = buffSize - buffPos;
        const auto buffIt            = &buff[buffPos];

        if (buffRemainingSize < sizeof(FileHeader::FileInfo)) {
            return true; // No more data
        }

        if (!reinterpret_cast<FileHeader::FileInfo*>(buffIt)->IsValid()) {
            return true; // Finished reading all data, but there's some padding left.
        }

        auto& h = *reinterpret_cast<FileHeader*>(buffIt);
        fileTotalSize = h.GetTotalSize();

        assert(fileTotalSize <= buffRemainingSize && "Not enough data in buffer for col data"); // NOTSA

        auto mi = IsModelDFF(h.modelId) ? CModelInfo::GetModelInfo(h.modelId) : nullptr;
        if (!mi || mi->m_nKey != CKeyGen::GetUppercaseKey(h.modelName)) {
            auto colDef = CColStore::ms_pColPool->GetAt(colId);
            mi = CModelInfo::GetModelInfo(h.modelName, colDef->m_nModelIdStart, colDef->m_nModelIdEnd);
        }

        if (!mi || !mi->bIsLod) {
            continue;
        }

        if (!mi->GetColModel()) {
            mi->SetColModel(new CColModel, true);
        }

        auto& cm = *mi->GetColModel();
        LoadCollisionModelAnyVersion(h, buffIt + sizeof(FileHeader), cm);

        cm.m_nColSlot = colId;
        if (mi->GetModelType() == MODEL_INFO_ATOMIC) {
            CPlantMgr::SetPlantFriendlyFlagInAtomicMI(static_cast<CAtomicModelInfo*>(mi));
        }
    }

    return true;
}

// 0x5B4E60
void CFileLoader::LoadCollisionFile(const char* filename, uint8 colId) {
    uint8 (&buffer)[0x8000] = *(uint8(*)[0x8000])0xBC40D8; // 32 kB

    using namespace ColHelpers;

    FileHeader header{};

    auto f = CFileMgr::OpenFile(filename, "rb");
    while (CFileMgr::Read(f, &header.info, sizeof(header.info))) {

        // Read remaining header info. This is stupid, no idea why it's read separately like this.
        CFileMgr::Read(f, &header.info + 1, sizeof(FileHeader) - sizeof(FileHeader::FileInfo));

        assert(std::size(buffer) >= header.GetDataSize()); // Just some sanity check to avoid undetectable bugs

        // Read actual data
        CFileMgr::Read(f, buffer, header.GetDataSize());

        const auto mi = CModelInfo::GetModelInfo(header.modelName, nullptr);
        if (!mi || !mi->bIsLod)
            continue;

        if (!mi->GetColModel()) {// TODO: Perhaps this should be in `CModelInfo` ? Like `GetColModel(bool bCreate = false)` or something
            mi->SetColModel(new CColModel, true);
        }

        auto& cm = *mi->GetColModel();
        LoadCollisionModelAnyVersion(header, buffer, cm);

        cm.m_nColSlot = colId;
    }
    CFileMgr::CloseFile(f);
}

// 0x5B5000
bool CFileLoader::LoadCollisionFileFirstTime(uint8* buff, uint32 buffSize, uint8 colId) {
    using namespace ColHelpers;
    assert(buffSize >= sizeof(FileHeader) && "LoadCollisionFileFirstTime called with not enough data"); // Buffer should be big enough to have at least 1 col header in it

    auto fileTotalSize{0u};
    for (auto buffPos = 0u; buffPos < buffSize; buffPos += fileTotalSize) {
        const auto buffRemainingSize = buffSize - buffPos;
        const auto buffIt            = &buff[buffPos];

        if (buffRemainingSize < sizeof(FileHeader::FileInfo)) {
            return true; // No more data
        }

        if (!reinterpret_cast<FileHeader::FileInfo*>(buffIt)->IsValid()) {
            return true; // Finished reading all data, but there's some padding left.
        }

        auto& header = *reinterpret_cast<FileHeader*>(buffIt);
        fileTotalSize = header.GetTotalSize();
        assert(fileTotalSize <= buffRemainingSize && "Not enough data in buffer for col data");

        auto modelId = (int32)header.modelId;

        auto mi = IsModelDFF(modelId) ? CModelInfo::GetModelInfo(modelId) : nullptr;
        if (!mi || mi->m_nKey != CKeyGen::GetUppercaseKey(header.modelName)) {
            mi = CModelInfo::GetModelInfo(header.modelName, &modelId);
        }

        if (!mi) {
            continue;
        }

        CColStore::IncludeModelIndex(colId, modelId);

        if (!mi->bIsLod) {
            continue;
        }

        auto& cm = *new CColModel;
        LoadCollisionModelAnyVersion(header, buffIt + sizeof(FileHeader), cm);

        cm.m_nColSlot = colId;
        mi->SetColModel(&cm, true);
        CColAccel::addCacheCol(PackedModelStartEnd{ .modelId = modelId }, cm);
    }

    return true;
}

// 0x537580
// Load collision V1 file from buffer. Just note that `buffer` is pointing to after `FileHeader`.
void CFileLoader::LoadCollisionModel(uint8* buffer, CColModel& cm) {
    using namespace ColHelpers;
    using namespace ColHelpers::V1;

    auto bufferIt = buffer;

    auto& h = *reinterpret_cast<Header*&>(bufferIt)++;
    cm.m_boundBox = h.bounds.box;
    cm.m_boundSphere = h.bounds.sphere;

    auto cd = new CCollisionData{};
    assert(cd);
    cm.m_pColData = cd;

    // Spheres
    cd->m_nNumSpheres = *reinterpret_cast<uint32*&>(bufferIt)++;
    if (cd->m_nNumSpheres) {
        cd->m_pSpheres = (CColSphere*)CMemoryMgr::Malloc(cd->m_nNumSpheres * sizeof(CColSphere));
        for (auto i = 0u; i < cd->m_nNumSpheres; i++) {
            cd->m_pSpheres[i] = *reinterpret_cast<TSphere*&>(bufferIt)++;
        }
    } else {
        cd->m_pSpheres = nullptr;
    }

    // Lines (Unused, so just skip)
    cd->m_nNumLines = *reinterpret_cast<uint32*&>(bufferIt)++;
    if (cd->m_nNumLines)
        bufferIt += cd->m_nNumLines * 24;
    cd->m_pLines = nullptr;

    // Boxes
    cd->m_nNumBoxes = *reinterpret_cast<uint32*&>(bufferIt)++;
    if (cd->m_nNumBoxes) {
        cd->m_pBoxes = (CColBox*)CMemoryMgr::Malloc(cd->m_nNumBoxes * sizeof(CColBox));
        for (auto i = 0u; i < cd->m_nNumBoxes; i++) {
            cd->m_pBoxes[i] = *reinterpret_cast<TBox*&>(bufferIt)++;
        }
    } else {
        cd->m_pBoxes = nullptr;
    }

    // Vertices
    if (auto nVertices = *reinterpret_cast<uint32*&>(bufferIt)++) {
        cd->m_pVertices = (CompressedVector*)CMemoryMgr::Malloc(nVertices * sizeof(CompressedVector));

        // Here they (or the compiler) originally did an unroll (with 4 vertices / iteration)
        // We are going to let the compiler do that.
        for (auto i = 0u; i < nVertices; i++) {
            cd->m_pVertices[i] = CompressVector(*reinterpret_cast<TVertex*&>(bufferIt)++);
        }
    } else {
        cd->m_pVertices = nullptr;
    }

    // Triangles
    cd->m_nNumTriangles = *reinterpret_cast<uint32*&>(bufferIt)++;
    if (cd->m_nNumTriangles) {
        cd->m_pTriangles = (CColTriangle*)CMemoryMgr::Malloc(cd->m_nNumTriangles * sizeof(CColTriangle));
        for (auto i = 0; i < cd->m_nNumTriangles; i++) {
            cd->m_pTriangles[i] = *reinterpret_cast<TFace*&>(bufferIt)++;
        }
    } else {
        cd->m_pTriangles = nullptr;
    }

    cd->bHasShadowInfo = false;

    cd->m_nNumShadowVertices = 0;
    cd->m_pShadowVertices = nullptr;

    cd->m_nNumShadowTriangles = 0;
    cd->m_pShadowTriangles = nullptr;

    if (cd->m_nNumSpheres || cd->m_nNumBoxes || cd->m_nNumTriangles)
        cm.m_bHasCollisionVolumes = true;
}

// 0x537EE0
// Load collision V2 file from buffer. Just note that `data` is pointing to after `FileHeader`
// `dataSize` - <`size` parameter in the header> - 24 (That is, basically `<total size of file> - sizeof(FileHeader)`)
// `buffer`   - Pointer to data after `FileHeader` (So that's an offset of 32)
void CFileLoader::LoadCollisionModelVer2(uint8* buffer, uint32 dataSize, CColModel& cm, const char* modelName) {
    using namespace ColHelpers;
    using namespace ColHelpers::V2;

    auto& h = *reinterpret_cast<Header*>(buffer);
    cm.m_boundBox = h.bounds.box;
    cm.m_boundSphere = h.bounds.sphere;
    cm.m_bHasCollisionVolumes = !h.IsEmpty();

    auto dataSizeAfterHeader = dataSize - sizeof(Header);
    if (!dataSizeAfterHeader) {
        assert(h.IsEmpty()); // Make sure flag says its empty as well, otherwise something went wrong.
        return; // No data present, other than the header
    }

    // Here's the meat. We allocate some memory to hold both the file's contents and the CCollisionData struct.
    // So, memory layout is as follows:
    //                [              DATA FROM FILE COPIED                  ]
    // CCollisionData | Spheres | Boxes | Suspension Lines | Vertices | Faces

    auto p = (uint8*)CMemoryMgr::Malloc(dataSizeAfterHeader + sizeof(CCollisionData));
    assert(p);
    cm.m_pColData = new(p) CCollisionData; // R* used a cast here, but that's not really a good idea.
    memcpy(p + sizeof(CCollisionData), buffer + sizeof(Header), dataSizeAfterHeader); // Copy actual data into allocated memory after CCollisionData

    auto cd = cm.m_pColData;
    cd->m_nNumSpheres = h.nSpheres;
    cd->m_nNumBoxes = h.nBoxes;
    cd->m_nNumLines = (uint8)h.nLines;
    cd->m_nNumTriangles = h.nFaces;

    cd->bUsesDisks = false;
    cd->bHasShadowInfo = false;
    cd->bHasFaceGroups = h.HasFaceGroups();

    // Set given field in `CCollisionData` based on offset in file.
    // If it's 0 then nullptr, otherwise a pointer to where the data is in memory.
    const auto SetColDataPtr = [&]<typename T>(T& colDataPtr, auto fileOffset) {
        // Return pointer for offset in allocated memory (relative to where it was in the file)
        const auto GetDataPtr = [&]() {
            return reinterpret_cast<T>(
                  p
                + sizeof(CCollisionData)                // Must offset by this (See memory layout above)
                + fileOffset
                + sizeof(FileHeader::FileInfo::fourcc)  // All offsets are relative to this, but since it is already included in the header's size, so we gotta compensate for it.
                - sizeof(FileHeader)                    // Offset includes these headers, but we haven't copied them into our memory
                - sizeof(Header)
            );
        };
        colDataPtr = fileOffset ? GetDataPtr() : nullptr;
    };

    SetColDataPtr(cd->m_pSpheres, h.offSpheres);
    SetColDataPtr(cd->m_pBoxes, h.offBoxes);
    SetColDataPtr(cd->m_pLines, h.offLines);
    SetColDataPtr(cd->m_pVertices, h.offVerts);
    SetColDataPtr(cd->m_pTriangles, h.offFaces);

    cd->m_pTrianglePlanes = nullptr;

    cd->m_nNumShadowVertices = 0;
    cd->m_pShadowVertices = nullptr;

    cd->m_nNumShadowTriangles = 0;
    cd->m_pShadowTriangles = nullptr;

    cm.m_bIsSingleColDataAlloc = true;

    assert(!cd->bHasFaceGroups || cd->m_pTriangles); // If it has face groups it should also have triangles allocated
}

// 0x537CE0
// Same arguments as above function, but for V3
void CFileLoader::LoadCollisionModelVer3(uint8* buffer, uint32 dataSize, CColModel& cm, const char* modelName) {
    using namespace ColHelpers;
    using namespace ColHelpers::V3;

    auto& h = *reinterpret_cast<V3::Header*>(buffer);
    cm.m_boundBox = h.bounds.box;
    cm.m_boundSphere = h.bounds.sphere;
    cm.m_bHasCollisionVolumes = !h.IsEmpty();

    auto dataSizeAfterHeader = dataSize - sizeof(V3::Header);
    if (!dataSizeAfterHeader) {
        assert(h.IsEmpty()); // Make sure flag says its empty as well, otherwise something went wrong.
        return; // No data present, other than the header
    }

    // Here's the meat. We allocate some memory to hold both the file's contents and the CCollisionData struct.
    // So, memory layout is as follows:
    //                [              DATA FROM FILE COPIED                  ]
    // CCollisionData | Spheres | Boxes | Suspension Lines | Vertices | Faces

    auto p = (uint8*)CMemoryMgr::Malloc(dataSizeAfterHeader + sizeof(CCollisionData));
    assert(p);
    cm.m_pColData = new(p) CCollisionData; // R* used a cast here, but that's not really a good idea.
    memcpy(p + sizeof(CCollisionData), buffer + sizeof(V3::Header), dataSizeAfterHeader); // Copy actual data into allocated memory after CCollisionData

    auto cd = cm.m_pColData;
    cd->m_nNumSpheres = h.nSpheres;
    cd->m_nNumBoxes = h.nBoxes;
    cd->m_nNumLines = (uint8)h.nLines;
    cd->m_nNumTriangles = h.nFaces;
    cd->m_nNumShadowTriangles = h.nShdwFaces;

    cd->bUsesDisks = false;
    cd->bHasFaceGroups = h.HasFaceGroups();

    // Set given field in `CCollisionData` based on offset in file.
    // If it's 0 then nullptr, otherwise a pointer to where the data is in memory.
    const auto SetColDataPtr = [&]<typename T>(T& colDataPtr, auto fileOffset) {
        // Return pointer for offset in allocated memory (relative to where it was in the file)
        const auto GetDataPtr = [&]() {
            return reinterpret_cast<T>(
                p
                + sizeof(CCollisionData)                // Must offset by this (See memory layout above)
                + fileOffset
                + sizeof(FileHeader::FileInfo::fourcc)  // All offsets are relative to this, but since it is already included in the header's size, so we gotta compnensate for it.
                - sizeof(FileHeader)                    // Offset includes these headers, but we haven't copied them into our memory
                - sizeof(V3::Header)
            );
        };
        colDataPtr = fileOffset ? GetDataPtr() : nullptr;
    };

    SetColDataPtr(cd->m_pSpheres, h.offSpheres);
    SetColDataPtr(cd->m_pBoxes, h.offBoxes);
    SetColDataPtr(cd->m_pLines, h.offLines);
    SetColDataPtr(cd->m_pVertices, h.offVerts);
    SetColDataPtr(cd->m_pTriangles, h.offFaces);
    SetColDataPtr(cd->m_pShadowVertices, h.offShdwVerts);
    SetColDataPtr(cd->m_pShadowTriangles, h.offShdwFaces);

    cd->bHasShadowInfo = h.offShdwFaces && h.offShdwVerts && h.nShdwFaces;
    cd->m_nNumShadowVertices = cd->bHasShadowInfo ? h.GetNoOfShdwVerts(cd) : 0;

    cd->m_pTrianglePlanes = nullptr;

    cm.m_bIsSingleColDataAlloc = true;

    assert(!cd->bHasFaceGroups || cd->m_pTriangles); // If it has face groups it should also have triangles allocated
}

// 0x537AE0
// Same shit different packaing of V3, but using V4 header
void CFileLoader::LoadCollisionModelVer4(uint8* buffer, uint32 dataSize, CColModel& cm, const char* modelName) {
    using namespace ColHelpers;
    using namespace ColHelpers::V4;

    auto& h = *reinterpret_cast<V4::Header*>(buffer);
    cm.m_boundBox = h.bounds.box;
    cm.m_boundSphere = h.bounds.sphere;
    cm.m_bHasCollisionVolumes = !h.IsEmpty();

    auto dataSizeAfterHeader = dataSize - sizeof(V4::Header);
    if (!dataSizeAfterHeader) {
        assert(h.IsEmpty());
        return; // No data present, other than the header
    }

    // Here's the meat. We allocate some memory to hold both the file's contents and the CCollisionData struct.
    // So, memory layout is as follows:
    //                [              DATA FROM FILE COPIED                  ]
    // CCollisionData | Spheres | Boxes | Suspension Lines | Vertices | Faces

    auto p = (uint8*)CMemoryMgr::Malloc(dataSizeAfterHeader + sizeof(CCollisionData));
    assert(p);
    cm.m_pColData = new(p) CCollisionData; // R* used a cast here, but that's not really a good idea.
    memcpy(p + sizeof(CCollisionData), buffer + sizeof(V4::Header), dataSizeAfterHeader); // Copy actual data into allocated memory after CCollisionData

    auto cd = cm.m_pColData;
    cd->m_nNumSpheres = h.nSpheres;
    cd->m_nNumBoxes = h.nBoxes;
    cd->m_nNumLines = (uint8)h.nLines;
    cd->m_nNumTriangles = h.nFaces;
    cd->m_nNumShadowTriangles = h.nShdwFaces;

    cd->bUsesDisks = false;
    cd->bHasFaceGroups = h.HasFaceGroups();

    // Set given field in `CCollisionData` based on offset in file.
    // If it's 0 then nullptr, otherwise a pointer to where the data is in memory.
    const auto SetColDataPtr = [&]<typename T>(T& colDataPtr, auto fileOffset) {
        // Return pointer for offset in allocated memory (relative to where it was in the file)
        const auto GetDataPtr = [&]() {
            return reinterpret_cast<T>(
                p
                + sizeof(CCollisionData)                // Must offset by this (See memory layout above)
                + fileOffset
                + sizeof(FileHeader::FileInfo::fourcc)  // All offsets are relative to this, but since it is already included in the header's size, so we gotta compnensate for it.
                - sizeof(FileHeader)                    // Offset includes these headers, but we haven't copied them into our memory
                - sizeof(V4::Header)
            );
        };
        colDataPtr = fileOffset ? GetDataPtr() : nullptr;
    };

    SetColDataPtr(cd->m_pSpheres, h.offSpheres);
    SetColDataPtr(cd->m_pBoxes, h.offBoxes);
    SetColDataPtr(cd->m_pLines, h.offLines);
    SetColDataPtr(cd->m_pVertices, h.offVerts);
    SetColDataPtr(cd->m_pTriangles, h.offFaces);
    SetColDataPtr(cd->m_pShadowVertices, h.offShdwVerts);
    SetColDataPtr(cd->m_pShadowTriangles, h.offShdwFaces);

    cd->bHasShadowInfo = h.offShdwFaces && h.offShdwVerts && h.nShdwFaces;
    cd->m_nNumShadowVertices = cd->bHasShadowInfo ? h.GetNoOfShdwVerts(cd) : 0;

    cd->m_pTrianglePlanes = nullptr;

    cm.m_bIsSingleColDataAlloc = true;

    assert(!cd->bHasFaceGroups || cd->m_pTriangles); // If it has face groups it should also have triangles allocated
}

// 0x5B3C60
int32 CFileLoader::LoadObject(const char* line) {
    int32  modelId{ MODEL_INVALID};
    char   modelName[24];
    char   texName[24];
    float  fDrawDist;
    uint32 nFlags;

    auto iNumRead = sscanf_s(line, "%d %s %s %f %d", &modelId, SCANF_S_STR(modelName), SCANF_S_STR(texName), &fDrawDist, &nFlags);
    if (iNumRead != 5 || fDrawDist < 4.0f)
    {
        int32 objType;
        float fDrawDist2_unused, fDrawDist3_unused;
        iNumRead = sscanf_s(line, "%d %s %s %d", &modelId, SCANF_S_STR(modelName), SCANF_S_STR(texName), &objType);
        if (iNumRead != 4)
            return -1;

        switch (objType)
        {
        case 1:
            VERIFY(sscanf_s(line, "%d %s %s %d %f %d", &modelId, SCANF_S_STR(modelName), SCANF_S_STR(texName), &objType, &fDrawDist, &nFlags) >= 5);
            break;
        case 2:
            VERIFY(sscanf_s(line, "%d %s %s %d %f %f %d", &modelId, SCANF_S_STR(modelName), SCANF_S_STR(texName), &objType, &fDrawDist, &fDrawDist2_unused, &nFlags) == 7);
            break;
        case 3:
            VERIFY(sscanf_s(line, "%d %s %s %d %f %f %f %d", &modelId, SCANF_S_STR(modelName), SCANF_S_STR(texName), &objType, &fDrawDist, &fDrawDist2_unused, &fDrawDist3_unused, &nFlags) == 8);
            break;
        }
    }

    sItemDefinitionFlags flags(nFlags);
    const auto mi = flags.bIsDamageable ? CModelInfo::AddDamageAtomicModel(modelId) : CModelInfo::AddAtomicModel(modelId);
    mi->m_fDrawDistance = fDrawDist;
    mi->SetModelName(modelName);
    mi->SetTexDictionary(texName);
    SetAtomicModelInfoFlags(mi, nFlags);

    return modelId;
}

// 0x5B7670
void CFileLoader::Load2dEffect(const char* line) {
    return plugin::Call<0x5B7670, const char*>(line);

    // todo:
    auto modelId{ MODEL_INVALID };
    CVector pos{};
    int32 type;
    VERIFY(sscanf_s(line, "%d %f %f %f %d", &modelId, &pos.x, &pos.y, &pos.z, &type) == 5);

    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));

    auto& effect = CModelInfo::Get2dEffectStore()->AddItem();
    CModelInfo::GetModelInfo(modelId)->Add2dEffect(&effect);
    effect.m_pos = pos;
    effect.m_type = *reinterpret_cast<e2dEffectType*>(type);

    switch (type) {
    case EFFECT_LIGHT:
        break;
    case EFFECT_PARTICLE:
        break;
    case EFFECT_ATTRACTOR:
        break;
    case EFFECT_INTERIOR:
        break;
    case EFFECT_ENEX:
        break;
    case EFFECT_ROADSIGN:
        break;
    case EFFECT_TRIGGER_POINT:
        break;
    case EFFECT_COVER_POINT:
        break;
    case EFFECT_ESCALATOR:
        break;
    default:
        break;
    }
    CTxdStore::PopCurrentTxd();
}

// 0x538090
CEntity* CFileLoader::LoadObjectInstance(CFileObjectInstance* objInstance, const char* modelName) {
    auto* mi = CModelInfo::GetModelInfo(objInstance->m_nModelId);
    if (!mi)
        return nullptr;

    CEntity* newEntity = nullptr;
    if (mi->m_nObjectInfoIndex == -1)
    {
        if (mi->GetModelType() == ModelInfoType::MODEL_INFO_CLUMP && mi->bHasAnimBlend)
            newEntity = new CAnimatedBuilding();
        else
            newEntity = new CBuilding();

        newEntity->SetModelIndexNoCreate(objInstance->m_nModelId);
        if (mi->bDontCastShadowsOn)
            newEntity->m_bDontCastShadowsOn = true;

        if (mi->m_fDrawDistance < 2.0F)
            newEntity->m_bIsVisible = false;
    }
    else
    {
        newEntity = new CDummyObject();
        newEntity->SetModelIndexNoCreate(objInstance->m_nModelId);
        if (IsGlassModel(newEntity) && !CModelInfo::GetModelInfo(newEntity->m_nModelIndex)->IsGlassType2()) {
            newEntity->m_bIsVisible = false;
        }
    }

    const auto& rot = objInstance->m_qRotation.imag;
    if (std::fabs(rot.x) > 0.05F || std::fabs(rot.y) > 0.05F ||
        (
            objInstance->m_bDontStream &&
            rot.x != 0.0f && rot.y != 0.0f
        )
    ) {
        objInstance->m_qRotation.imag = -rot;
        newEntity->AllocateStaticMatrix();
        newEntity->GetMatrix().SetRotate(objInstance->m_qRotation);
    }
    else
    {
        const auto fMult = rot.z < 0.0f ? 2.0f : -2.0f;
        const auto fHeading = std::acos(objInstance->m_qRotation.real) * fMult;
        newEntity->SetHeading(fHeading);
    }

    newEntity->SetPosn(objInstance->m_vecPosition);

    if (objInstance->m_bUnderwater)
        newEntity->m_bUnderwater = true;

    if (objInstance->m_bTunnel)
        newEntity->m_bTunnel = true;

    if (objInstance->m_bTunnelTransition)
        newEntity->m_bTunnelTransition = true;

    if (objInstance->m_bRedundantStream)
        newEntity->m_bUnimportantStream = true;

    newEntity->m_nAreaCode = static_cast<eAreaCodes>(objInstance->m_nAreaCode);
    newEntity->m_nLodIndex = objInstance->m_nLodInstanceIndex;

    if (objInstance->m_nModelId == ModelIndices::MI_TRAINCROSSING)
    {
        newEntity->GetMatrix();
        newEntity->AllocateStaticMatrix();
        CObject::SetMatrixForTrainCrossing(&newEntity->GetMatrix(), PI * 0.43f);
    }

    auto* cm = mi->GetColModel();
    if (cm) {
        if (cm->m_bHasCollisionVolumes)
        {
            if (cm->m_nColSlot) 
            {
                CColStore::ms_pColPool->GetAt(cm->m_nColSlot)->m_Area.Restrict(newEntity->GetBoundRect());
            }
        }
        else
        {
            newEntity->m_bUsesCollision = false;
        }

        if (cm->GetBoundingBox().m_vecMin.z + newEntity->GetPosition().z < 0.0f)
            newEntity->m_bUnderwater = true;
    }

    return newEntity;
}

// 0x538090
CEntity* CFileLoader::LoadObjectInstance(const char* line) {
    char modelName[24];
    CFileObjectInstance instance;
    VERIFY(sscanf_s(
        line,
        "%d %s %d %f %f %f %f %f %f %f %d",
        &instance.m_nModelId,
        SCANF_S_STR(modelName),
        &instance.m_nInstanceType,
        &instance.m_vecPosition.x,
        &instance.m_vecPosition.y,
        &instance.m_vecPosition.z,
        &instance.m_qRotation.imag.x,
        &instance.m_qRotation.imag.y,
        &instance.m_qRotation.imag.z,
        &instance.m_qRotation.real,
        &instance.m_nLodInstanceIndex
    ) == 11);
    return LoadObjectInstance(&instance, modelName);
}

// 0x5B4B40
void CFileLoader::LoadCullZone(const char* line) {
    CVector center{}, mirrorDirection{};
    float unknown, length, bottom, width, unknown2, zTop, cm;
    int32 flags, flags2 = 0;

    if (sscanf_s(
        line,
        "%f %f %f %f %f %f %f %f %f %d %f %f %f %f",
        &center.x, &center.y, &center.z,
        &unknown,
        &length,
        &bottom,
        &width,
        &unknown2,
        &zTop,
        &flags,
        &mirrorDirection.x, &mirrorDirection.y, &mirrorDirection.z,
        &cm
    ) == 14) {
        CCullZones::AddMirrorAttributeZone(
            center,
            unknown,
            length,
            bottom,
            width,
            unknown2,
            zTop,
            static_cast<eZoneAttributes>(flags),
            cm,
            mirrorDirection.x, mirrorDirection.y, mirrorDirection.z
        );
        return;
    }

    VERIFY(sscanf_s(
        line,
        "%f %f %f %f %f %f %f %f %f %d %d",
        &center.x, &center.y, &center.z,
        &unknown,
        &length,
        &bottom,
        &width,
        &unknown2,
        &zTop,
        &flags,
        &flags2
    ) == 11);
    CCullZones::AddCullZone(center, unknown, length, bottom, width, unknown2, zTop, flags);
}

// IPL -> ENEX
// 0x5B8030
void CFileLoader::LoadEntryExit(const char* line) {
    uint32 numOfPeds = 2;
    uint32 timeOn = 0, timeOff = 24;
    CVector enter{}, exit{};
    CVector2D range;
    float entranceAngle;
    float unused;
    float exitAngle;
    int32 area;
    char interiorName[64]{};
    uint32 skyColor;
    uint32 flags;

    VERIFY(sscanf_s(
        line,
        "%f %f %f %f %f %f %f %f %f %f %f %d %d %s %d %d %d %d",
        &enter.x, &enter.y, &enter.z,
        &entranceAngle,
        &range.x, &range.y,
        &unused,
        &exit.x, &exit.y, &exit.z,
        &exitAngle,
        &area,
        &flags,
        SCANF_S_STR(interiorName),
        &skyColor,
        &numOfPeds,
        &timeOn,
        &timeOff
    ) == 18);

    auto name = strrchr(interiorName, '"');
    if (name) {
        *name = 0;
        name = &interiorName[1];
    }

    const auto enexPoolIdx = CEntryExitManager::AddOne(
        enter.x, enter.y, enter.z,
        entranceAngle,
        range.x, range.y,
        unused,
        exit.x, exit.y, exit.z,
        exitAngle,
        area,
        (CEntryExit::eFlags)flags,
        skyColor,
        timeOn,
        timeOff,
        numOfPeds,
        name
    );
    auto enex = CEntryExitManager::mp_poolEntryExits->GetAt(enexPoolIdx);
    assert(enex);

    enum Flags {
        UNKNOWN_INTERIOR      = 1 << 0,
        UNKNOWN_PAIRING       = 1 << 1,
        CREATE_LINKED_PAIR    = 1 << 2,
        REWARD_INTERIOR       = 1 << 3,
        USED_REWARD_ENTRANCE  = 1 << 4,
        CARS_AND_AIRCRAFT     = 1 << 5,
        BIKES_AND_MOTORCYCLES = 1 << 6,
        DISABLE_ONFOOT        = 1 << 7,
    };

    if (flags & UNKNOWN_INTERIOR)
        enex->bUnknownInterior = true;

    if (flags & UNKNOWN_PAIRING)
        enex->bUnknownPairing = true;

    if (flags & CREATE_LINKED_PAIR)
        enex->bCreateLinkedPair = true;

    if (flags & REWARD_INTERIOR)
        enex->bRewardInterior = true;

    if (flags & USED_REWARD_ENTRANCE)
        enex->bUsedRewardEntrance = true;

    if (flags & CARS_AND_AIRCRAFT)
        enex->bCarsAndAircraft = true;

    if (flags & BIKES_AND_MOTORCYCLES)
        enex->bBikesAndMotorcycles = true;

    if (flags & DISABLE_ONFOOT)
        enex->bDisableOnFoot = true;
}

// IPL -> GRGE
// 0x5B4530
void CFileLoader::LoadGarage(const char* line) {
    uint32 flags;
    uint32 type;
    CVector p1{}, p2{};
    float frontX, frontY;
    char name[128];

    if (sscanf_s(
        line,
        "%f %f %f %f %f %f %f %f %d %d %s",
        &p1.x, &p1.y, &p1.z,
        &frontX,
        &frontY,
        &p2.x, &p2.y, &p2.z,
        &flags,
        &type,
        SCANF_S_STR(name)
    ) == 11) {
        CGarages::AddOne(p1.x, p1.y, p1.z, frontX, frontY, p2.x, p2.y, p2.z, (eGarageType)type, 0, name, flags);
    }
}

// 0x5B9030
void CFileLoader::LoadLevel(const char* levelFileName) {
    ZoneScoped;

    auto txd = RwTexDictionaryGetCurrent();
    if (!txd) {
        txd = RwTexDictionaryCreate();
        RwTexDictionarySetCurrent(txd);
    }

    bool hasLoadedAnyIPLs{};

    const auto f = CFileMgr::OpenFile(levelFileName, "r");
    for (auto l = LoadLine(f); l; l = LoadLine(f)) {
        const auto LineBeginsWith = [lsv = std::string_view{l}](auto what) {
            return lsv.starts_with(what);
        };

        // Extract path after identifier like: <ID> <PATH>
        char pathBuffer[MAX_PATH]{};
        const auto ExtractPathFor = [&](std::string_view id) {
            // Okay, so..
            // Originally they didn't copy the path into a separate buffer for each "id"
            // But we are going to, for two reasons:
            // - Copy is cheap
            // - Lifetime issue
            // The latter is the reason why they had to make a copy in the first place.
            // If we were to just return `l + strlen(id) + 1` we'd depend on the line buffer's content not to change,
            // but the line is just a static buffer (ms_line) which is modified each time `LoadLine` is called.
            // So if any of the invoked functions call `LoadLine` the path will no longer be valid
            // So in order to prevent this nasty bug we're just going to copy it each time.
            strcpy_s(pathBuffer, l + id.size() + 1);
            return pathBuffer;
        };

        if (LineBeginsWith("#"))
            continue; // Skip comment

        if (LineBeginsWith("EXIT"))
            break; // Done

        if (LineBeginsWith("TEXDICTION")) {
            ZoneScopedN("TEXDICTION");

            const auto path = ExtractPathFor("TEXDICTION");
            ZoneText(path, strlen(path));

            LoadingScreenLoadingFile(path);

            const auto txd = LoadTexDictionary(path);
            RwTexDictionaryForAllTextures(txd, AddTextureCB, txd);
            RwTexDictionaryDestroy(txd);
        } else if (LineBeginsWith("IPL")) {
            ZoneScopedN("IPL");

            // Have to call this here, because line buffer's content may change after the `if` below
            const auto path = ExtractPathFor("IPL");
            ZoneText(path, strlen(path));

            if (!hasLoadedAnyIPLs) {
                MatchAllModelStrings();

                LoadingScreenLoadingFile("Object Data");
                CObjectData::Initialise("DATA\\OBJECT.DAT");

                LoadingScreenLoadingFile("Setup vehicle info data");
                CVehicleModelInfo::SetupCommonData();

                LoadingScreenLoadingFile("Streaming Init");
                CStreaming::Init2();

                CLoadingScreen::NewChunkLoaded();

                LoadingScreenLoadingFile("Collision");
                CColStore::LoadAllBoundingBoxes();

                // TODO: Probably inlined.
                for (auto mi : CModelInfo::ms_modelInfoPtrs) {
                    if (mi) {
                        mi->ConvertAnimFileIndex();
                    }
                }

                hasLoadedAnyIPLs = true;
            }

            // Originally here they've copied the path into a buffer
            // We ain't gonna do that, there's no point to it
            LoadingScreenLoadingFile(path);
            LoadScene(path);
        } else {
            // Deal with the rest (Originally more `else-if`s were used)
            // Sadly we can't put all of the above in here as well
            // because they'd need a capturing lambda, and that would require
            // us to use `std::function` which is just overkill in this case.

            using FnType = void(*)(const char*);
            const struct { std::string_view id;  FnType fn; } functions[] {
                { "IMG", [](const char* path) {
                    if (path != std::string_view{ "MODELS\\GTA_INT.IMG" }) {
                        CStreaming::AddImageToList(path, true);
                    }
                }},
                { "COLFILE", [](const char* path) { LoadCollisionFile(path + 2, 0); }}, // Gotta add 3 to the path, because we have to skip the `0` before the actual path
                { "MODELFILE", LoadAtomicFile},
                { "HIERFILE", LoadClumpFile},
                { "IDE", LoadObjectTypes},
                //{"SPLASH", [](const char*) {}} - Unused
            };
            for (const auto& v : functions) {
                if (LineBeginsWith(v.id)) {
                    ZoneScoped;
                    ZoneText(v.id.data(), v.id.size());

                    const auto path = ExtractPathFor(v.id);
                    ZoneText(path, strlen(path));

                    LoadingScreenLoadingFile(path);
                    v.fn(path);

                    break;
                }
            }
        }
    }
    CFileMgr::CloseFile(f);

    RwTexDictionarySetCurrent(txd);

    if (hasLoadedAnyIPLs) {
        CIplStore::LoadAllRemainingIpls();
        CColStore::BoundingBoxesPostProcess();
        CTrain::InitTrains();
        CColStore::RemoveAllCollision();
    }
}

// IPL -> OCCL
// 0x5B4C80
void CFileLoader::LoadOcclusionVolume(const char* line, const char* filename) {
    float fRotX = 0.0F, fRotY = 0.0F;
    uint32 nFlags = 0;
    float fCenterX, fCenterY, fBottomZ, fWidth, fLength, fHeight, fRotZ;

    VERIFY(sscanf_s(line, "%f %f %f %f %f %f %f %f %f %d ", &fCenterX, &fCenterY, &fBottomZ, &fWidth, &fLength, &fHeight, &fRotX, &fRotY, &fRotZ, &nFlags) == 10);
    auto fCenterZ = fHeight * 0.5F + fBottomZ;
    auto strLen = strlen(filename);

    bool bIsInterior = false;
    if (filename[strLen - 7] == 'i' && filename[strLen - 6] == 'n' && filename[strLen - 5] == 't') // todo:
        bIsInterior = true;

    COcclusion::AddOne(fCenterX, fCenterY, fCenterZ, fWidth, fLength, fHeight, fRotX, fRotY, fRotZ, nFlags, bIsInterior);
}

// 0x5B41C0
int32 CFileLoader::LoadPathHeader(const char* line, int32& outPathType) {
    int32 id;
    VERIFY(sscanf_s(line, "%d %d %*s", &outPathType, &id) == 2);
    return id;
}

// PEDS
// 0x5B7420
int32 CFileLoader::LoadPedObject(const char* line) {
    auto   modelId{ MODEL_INVALID };
    int32  radio1{}, radio2{};
    uint32 flags{};
    int32  carsCanDriveMask{};

    // TODO: Should probably increase the size of these to avoid possible buffer overflow
    char animFile[12]{};
    char modelName[20]{};
    char pedVoiceType[16]{};
    char statName[20]{};
    char animGroup[20]{};
    char pedType[24]{};
    char texName[20]{};
    char voiceMin[56]{};
    char voiceMax[60]{};

    VERIFY(sscanf_s(
        line,
        "%d %s %s %s %s %s %x %x %11s %d %d %s %s %s",
        &modelId,
        SCANF_S_STR(modelName),
        SCANF_S_STR(texName),
        SCANF_S_STR(pedType),
        SCANF_S_STR(statName),
        SCANF_S_STR(animGroup),
        &carsCanDriveMask,
        &flags,
        SCANF_S_STR(animFile),
        &radio1,
        &radio2,
        SCANF_S_STR(pedVoiceType),
        SCANF_S_STR(voiceMin),
        SCANF_S_STR(voiceMax)
    ) == 14);

    const auto mi = CModelInfo::AddPedModel(modelId);

    mi->m_nKey = CKeyGen::GetUppercaseKey(modelName);
    mi->SetTexDictionary(texName);
    mi->SetAnimFile(animFile);
    mi->SetColModel(&CTempColModels::ms_colModelPed1, false);
    mi->m_nPedType          = CPedType::FindPedType(pedType);
    mi->m_nStatType         = CPedStats::GetPedStatType(statName);
    mi->m_nAnimType         = CAnimManager::GetAnimationGroupIdByName(animGroup);
    mi->m_nCarsCanDriveMask = carsCanDriveMask;
    mi->m_nPedFlags         = flags;
    mi->m_nRadio2           = (eRadioID)(radio2 + 1);
    mi->m_nRadio1           = (eRadioID)(radio1 + 1);
    mi->m_nRace             = CPopulation::FindPedRaceFromName(modelName);
    mi->m_nPedAudioType     = CAEPedSpeechAudioEntity::GetAudioPedType(pedVoiceType);
    mi->m_nVoiceMin         = CAEPedSpeechAudioEntity::GetVoice(voiceMin, mi->m_nPedAudioType);
    mi->m_nVoiceMax         = CAEPedSpeechAudioEntity::GetVoice(voiceMax, mi->m_nPedAudioType);
    mi->m_nVoiceId          = mi->m_nVoiceMin;

    return modelId;
}

// useless
// 0x5B41F0
void CFileLoader::LoadPedPathNode(const char* line, int32 objModelIndex, int32 pathEntryIndex) {
    // This function loads the file, reads some data
    // and calls `CPathFind::StoreDetachedNodeInfoPed` or `CPathFind::StoreNodeInfoPed`
    // but both functions are NOPs, so this function basically doesn't do anything useful.
}

// 0x5B47B0
// https://gta.fandom.com/wiki/PICK
void CFileLoader::LoadPickup(const char* line) {
    CVector pos{};
    int32   weaponType{};

    if (sscanf_s(line, "%d %f %f %f", &weaponType, &pos.x, &pos.y, &pos.z) != 4)
        return;

    const auto GetModel = [weaponType] {
        switch (weaponType) {
        case WEAPON_KNIFE:
            return MODEL_BRASSKNUCKLE;

        case WEAPON_BASEBALLBAT:
            return MODEL_NITESTICK;

        case WEAPON_SHOVEL:
            return MODEL_KNIFECUR;

        case WEAPON_CHAINSAW:
            return MODEL_GOLFCLUB;

        case WEAPON_DILDO1:
            return MODEL_BAT;

        case WEAPON_DILDO2:
            return MODEL_SHOVEL;

        case WEAPON_VIBE1:
            return MODEL_POOLCUE;

        case WEAPON_VIBE2:
            return MODEL_KATANA;

        case WEAPON_FLOWERS:
            return MODEL_CHNSAW;

        case WEAPON_CANE:
            return MODEL_MOLOTOV;

        case WEAPON_GRENADE:
            return MODEL_GRENADE;

        case WEAPON_TEARGAS:
            return MODEL_SATCHEL;

        case WEAPON_MOLOTOV:
            return MODEL_COLT45;

        case WEAPON_ROCKET:
            return MODEL_SILENCED;

        case WEAPON_ROCKET_HS:
            return MODEL_DESERT_EAGLE;

        case WEAPON_FREEFALL_BOMB:
            return MODEL_CHROMEGUN;

        case WEAPON_PISTOL:
        case WEAPON_INFRARED:
            return MODEL_SHOTGSPA;

        case WEAPON_PISTOL_SILENCED:
            return MODEL_TEC9;

        case WEAPON_DESERT_EAGLE:
            return MODEL_MICRO_UZI;

        case WEAPON_SHOTGUN:
            return MODEL_MP5LNG;

        case WEAPON_SAWNOFF_SHOTGUN:
            return MODEL_AK47;

        case WEAPON_SPAS12_SHOTGUN:
            return MODEL_M4;

        case WEAPON_MICRO_UZI:
            return MODEL_CUNTGUN;

        case WEAPON_MP5:
            return MODEL_SNIPER;

        case WEAPON_M4:
            return MODEL_FLAME;

        case WEAPON_TEC9:
        case WEAPON_NIGHTVISION:
            return MODEL_MINIGUN;

        case WEAPON_COUNTRYRIFLE:
            return MODEL_GUN_DILDO1;

        case WEAPON_SNIPERRIFLE:
            return MODEL_GUN_DILDO2;

        case WEAPON_RLAUNCHER:
            return MODEL_GUN_VIBE1;

        case WEAPON_RLAUNCHER_HS:
            return MODEL_GUN_VIBE2;

        case WEAPON_FLAMETHROWER:
            return MODEL_FLOWERA;

        case WEAPON_MINIGUN:
            return MODEL_GUN_CANE;

        case WEAPON_REMOTE_SATCHEL_CHARGE:
            return MODEL_GUN_BOXWEE;

        case WEAPON_DETONATOR:
            return MODEL_GUN_BOXBIG;

        case WEAPON_SPRAYCAN:
            return MODEL_CELLPHONE;

        case WEAPON_CAMERA:
            return MODEL_TEARGAS;

        case WEAPON_PARACHUTE:
            return MODEL_ROCKETLA;

        case WEAPON_LAST_WEAPON:
            return MODEL_HEATSEEK;

        case WEAPON_ARMOUR:
            return MODEL_BOMB;

        case WEAPON_RAMMEDBYCAR:
            return MODEL_SPRAYCAN;

        case WEAPON_RUNOVERBYCAR:
            return MODEL_FIRE_EX;

        case WEAPON_EXPLOSION:
            return MODEL_CAMERA;

        case WEAPON_UZI_DRIVEBY:
            return MODEL_NVGOGGLES;

        case WEAPON_DROWNING:
            return MODEL_IRGOGGLES;

        case WEAPON_FALL:
            return MODEL_JETPACK;

        case WEAPON_UNIDENTIFIED:
            return MODEL_GUN_PARA;

        default:
            return MODEL_INVALID;
        }
    };

    if (const auto model = GetModel(); model != MODEL_INVALID) {
        CPickups::GenerateNewOne(pos, model, PICKUP_ON_STREET, 0, 0, false, nullptr);
    }
}

// 0x5B45D0
void CFileLoader::LoadStuntJump(const char* line) {
    CBoundingBox start, target;
    CVector cameraPosn{};
    int32 reward;

    int32 iNumRead = sscanf_s(
        line,
        "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d",
        &start.m_vecMin.x,
        &start.m_vecMin.y,
        &start.m_vecMin.z,
        &start.m_vecMax.x,
        &start.m_vecMax.y,
        &start.m_vecMax.z,

        &target.m_vecMin.x,
        &target.m_vecMin.y,
        &target.m_vecMin.z,
        &target.m_vecMax.x,
        &target.m_vecMax.y,
        &target.m_vecMax.z,

        &cameraPosn.x,
        &cameraPosn.y,
        &cameraPosn.z,
        &reward
    );
    if (iNumRead == 16) {
         CStuntJumpManager::AddOne(start, target, cameraPosn, reward);
    }
}

// 0x5B75E0
int32 CFileLoader::LoadTXDParent(const char* line) {
    char name[32], parentName[32];
    VERIFY(sscanf_s(line, "%s %s", SCANF_S_STR(name), SCANF_S_STR(parentName)) == 2);
    auto txdSlot = CTxdStore::FindOrAddTxdSlot(name);
    auto parentSlot = CTxdStore::FindOrAddTxdSlot(parentName);
    CTxdStore::ms_pTxdPool->GetAt(txdSlot)->m_wParentIndex = parentSlot;
    return parentSlot;
}

// IPL -> TCYC
// 0x5B81D0
void CFileLoader::LoadTimeCyclesModifier(const char* line) {
    CVector vec1{}, vec2{};
    int32 farClip;
    int32 extraColor;
    float extraColorIntensity;
    float falloffDist = 100.0f;
    float unused = 1.0f;
    float lodDistMult = 1.0f;

    auto iNumRead = sscanf_s(
        line,
        "%f %f %f %f %f %f %d %d %f %f %f %f",
        &vec1.x,
        &vec1.y,
        &vec1.z,
        &vec2.x,
        &vec2.y,
        &vec2.z,
        &farClip,
        &extraColor,
        &extraColorIntensity,
        &falloffDist,
        &unused,
        &lodDistMult
    );
    if (iNumRead < 12)
        lodDistMult = unused;

    CBox box{};
    box.Set(vec1, vec2);
    return CTimeCycle::AddOne(box, farClip, extraColor, extraColorIntensity, falloffDist, lodDistMult);
}

// 0x5B3DE0
int32 CFileLoader::LoadTimeObject(const char* line) {
    auto  modelId{ MODEL_INVALID };
    char  modelName[24];
    char  texName[24];
    float drawDistance[3]{};
    int32 flags;
    int32 timeOn;
    int32 timeOff;

    int32 numValuesRead = sscanf_s(line, "%d %s %s %f %d %d %d", &modelId, SCANF_S_STR(modelName), SCANF_S_STR(texName), &drawDistance[0], &flags, &timeOn, &timeOff);

    if (numValuesRead != 7 || drawDistance[0] < 4.0) {
        int32 numObjs;

        if (sscanf_s(line, "%d %s %s %d", &modelId, SCANF_S_STR(modelName), SCANF_S_STR(texName), &numObjs) != 4)
            return -1;

        switch (numObjs) {
        case 1:
            VERIFY(sscanf_s(line, "%d %s %s %d %f %d %d %d", &modelId, SCANF_S_STR(modelName), SCANF_S_STR(texName), &numObjs, &drawDistance[0], &flags, &timeOn, &timeOff) == 8);
            break;
        case 2:
            VERIFY(sscanf_s(line, "%d %s %s %d %f %f %d %d %d", &modelId, SCANF_S_STR(modelName), SCANF_S_STR(texName), &numObjs, &drawDistance[0], &drawDistance[1], &flags, &timeOn, &timeOff) == 9);
            break;
        case 3:
            VERIFY(sscanf_s(line, "%d %s %s %d %f %f %f %d %d %d", &modelId, SCANF_S_STR(modelName), SCANF_S_STR(texName), &numObjs, &drawDistance[0], &drawDistance[1], &drawDistance[2], &flags, &timeOn, &timeOff) == 10);
            break;
        default:
            NOTSA_UNREACHABLE();
        }
    }

    CTimeModelInfo* mi = CModelInfo::AddTimeModel(modelId);
    mi->m_fDrawDistance = drawDistance[0];
    mi->SetModelName(modelName);
    mi->SetTexDictionary(texName);

    CTimeInfo* timeInfo = mi->GetTimeInfo();
    timeInfo->SetTimes(timeOn, timeOff);

    SetAtomicModelInfoFlags(mi, flags);

    CTimeInfo* otherTimeInfo = timeInfo->FindOtherTimeModel(modelName);
    if (otherTimeInfo)
        otherTimeInfo->SetOtherTimeModel(modelId);

    return modelId;
}

// 0x5B6F30
int32 CFileLoader::LoadVehicleObject(const char* line) {
    auto  modelId{ MODEL_INVALID };
    char  modelName[24]{};
    char  texName[24]{};
    char  type[8]{};
    char  handlingName[16]{};
    char  gameName[32]{};
    char  anims[16]{};
    char  vehCls[16]{};
    uint32 frq{}, flags{};
    tVehicleCompsUnion vehComps{};
    uint32 misc{}; // `m_fBikeSteerAngle` if model type is BMX/Bike, otherwise `m_nWheelModelIndex`
    float wheelSizeFront{}, wheelSizeRear{};
    int32 wheelUpgradeCls{ -1 };

    VERIFY(sscanf_s(line, "%d %s %s %s %s %s %s %s %d %d %x %d %f %f %d",
        &modelId,
        SCANF_S_STR(modelName),
        SCANF_S_STR(texName),
        SCANF_S_STR(type),
        SCANF_S_STR(handlingName),
        SCANF_S_STR(gameName),
        SCANF_S_STR(anims),
        SCANF_S_STR(vehCls),
        &frq,
        &flags,             // optional
        &vehComps.m_nComps, // optional
        &misc,              // optional
        &wheelSizeFront,    // optional
        &wheelSizeRear,     // optional
        &wheelUpgradeCls    // optional
    ) >= 10);

    auto mi = CModelInfo::AddVehicleModel(modelId);
    mi->SetModelName(modelName);
    mi->SetTexDictionary(texName);
    CTxdStore::ms_pTxdPool->GetAt(mi->m_nTxdIndex)->m_wParentIndex = CTxdStore::FindOrAddTxdSlot("vehicle");
    mi->SetAnimFile(anims);

    // Replace `_` with ` ` (space)
    std::replace(gameName, gameName + strlen(gameName), '_', ' ');
    mi->SetGameName(gameName);

    mi->m_nFlags = flags;
    mi->m_extraComps = vehComps;


    // This isn't exactly R* did it, but that code is hot garbage anyways
    // They've used strcmp all the way, and.. It's bad.

    const auto GetVehicleType = [&] {
        static constexpr struct { std::string_view name; eVehicleType type; } mapping[] = {
            { "car",     VEHICLE_TYPE_AUTOMOBILE },
            { "mtruck",  VEHICLE_TYPE_MTRUCK     },
            { "quad",    VEHICLE_TYPE_QUAD       },
            { "heli",    VEHICLE_TYPE_HELI       },
            { "plane",   VEHICLE_TYPE_PLANE      },
            { "boat",    VEHICLE_TYPE_BOAT       },
            { "train",   VEHICLE_TYPE_TRAIN      },
            { "f_heli",  VEHICLE_TYPE_FHELI      }, // NOTE: Originally this mapped to HELI, but since f_heli isn't used anywhere in the data files we've corrected the typo.
            { "f_plane", VEHICLE_TYPE_FPLANE     },
            { "bike",    VEHICLE_TYPE_BIKE       },
            { "bmx",     VEHICLE_TYPE_BMX        },
            { "trailer", VEHICLE_TYPE_TRAILER    },
        };

        for (const auto& [name, vtype] : mapping) {
            if (name == type) {
                return vtype;
            }
        }

        assert(0);             // NOTSA - Something went really wrong
        return VEHICLE_TYPE_IGNORE; // fix warning
    };

    mi->m_nVehicleType = GetVehicleType();
    switch (mi->m_nVehicleType) {
    case VEHICLE_TYPE_AUTOMOBILE:
    case VEHICLE_TYPE_MTRUCK:
    case VEHICLE_TYPE_QUAD:
    case VEHICLE_TYPE_HELI:
    case VEHICLE_TYPE_PLANE:
    case VEHICLE_TYPE_TRAILER: {
        mi->SetWheelSizes(wheelSizeFront, wheelSizeRear);
        mi->m_nWheelModelIndex = misc;
        break;
    }
    case VEHICLE_TYPE_FPLANE: {
        mi->SetWheelSizes(1.0f, 1.0f);
        mi->m_nWheelModelIndex = misc;
        break;
    }
    case VEHICLE_TYPE_BIKE:
    case VEHICLE_TYPE_BMX: {
        mi->SetWheelSizes(wheelSizeFront, wheelSizeRear);
        mi->m_fBikeSteerAngle = (float)misc;
        break;
    }
    }

    mi->SetHandlingId(handlingName);
    mi->m_nWheelUpgradeClass = wheelUpgradeCls;

    const auto GetVehicleClass = [&] {
        static constexpr struct { std::string_view name; eVehicleClass cls; } mapping[] = {
            { "normal",      VEHICLE_CLASS_NORMAL      },
            { "poorfamily",  VEHICLE_CLASS_POORFAMILY  },
            { "richfamily",  VEHICLE_CLASS_RICHFAMILY  },
            { "executive",   VEHICLE_CLASS_EXECUTIVE   },
            { "worker",      VEHICLE_CLASS_WORKER      },
            { "big",         VEHICLE_CLASS_BIG         },
            { "taxi",        VEHICLE_CLASS_TAXI        },
            { "moped",       VEHICLE_CLASS_MOPED       },
            { "motorbike",   VEHICLE_CLASS_MOTORBIKE   },
            { "leisureboat", VEHICLE_CLASS_LEISUREBOAT },
            { "workerboat",  VEHICLE_CLASS_WORKERBOAT  },
            { "bicycle",     VEHICLE_CLASS_BICYCLE     },
            { "ignore",      VEHICLE_CLASS_IGNORE      },
        };

        for (const auto& [name, cls] : mapping) {
            if (name == vehCls) {
                return cls;
            }
        }

        // NOTSA - Something has went really wrong
        DebugBreak();
        return (eVehicleClass)-1;
    };
    mi->m_nVehicleClass = GetVehicleClass();
    if (mi->m_nVehicleClass != eVehicleClass::VEHICLE_CLASS_IGNORE) {
        mi->m_nFrq = frq;
    }

    return modelId;
}

// 0x5B3FB0
int32 CFileLoader::LoadWeaponObject(const char* line) {
    auto objId{ MODEL_INVALID };
    char modelName[24];
    char texName[24];
    char animName[16];
    int32 weaponType;
    float drawDist;

    VERIFY(sscanf_s(line, "%d %s %s %s %d %f", &objId, SCANF_S_STR(modelName), SCANF_S_STR(texName), SCANF_S_STR(animName), &weaponType, &drawDist) == 6);
    CWeaponModelInfo* mi = CModelInfo::AddWeaponModel(objId);
    mi->SetModelName(modelName);
    mi->m_fDrawDistance = drawDist;
    mi->SetTexDictionary(texName);
    mi->SetAnimFile(animName);
    mi->SetColModel(&CTempColModels::ms_colModelWeapon, false);
    return objId;
}

// 0x5B4AB0
void CFileLoader::LoadZone(const char* line) {
    char    infoLabel[24];
    int32   type;
    CVector min{}, max{};
    int32   level;
    char    textLabel[12];

    VERIFY(sscanf_s(line, "%s %d %f %f %f %f %f %f %d %s", SCANF_S_STR(infoLabel), &type, &min.x, &min.y, &min.z, &max.x, &max.y, &max.z, &level, SCANF_S_STR(textLabel)) == 10);
    CTheZones::CreateZone(infoLabel, static_cast<eZoneType>(type), min, max, static_cast<eLevelName>(level), textLabel);
}

// 0x5B51E0
void LinkLods(int32 a1) {
    plugin::Call<0x5B51E0, int32>(a1);
}

// 0x5B8700
void CFileLoader::LoadScene(const char* filename) {
    ZoneScoped;

    gCurrIplInstancesCount = 0;

    enum class SectionID {
        NONE = 0, // NOTSA - Placeholder value

        PATH = 1,
        INST = 2,
        MULT = 3,
        ZONE = 4,
        CULL = 5,
        OCCL = 6,
        GRGE = 8,
        ENEX = 9,
        PICK = 10,
        CARS = 11,
        JUMP = 12,
        TCYC = 13,
        AUZO = 14,
    };

    auto sectionId{ SectionID::NONE };

    int32 nPathEntryIndex{ -1 }, pathHeaderId{};
    int32 pathType{};

    auto file = CFileMgr::OpenFile(filename, "rb");
    for (char* line = LoadLine(file); line; line = LoadLine(file)) {
        if (!line[0]) // Emtpy line
            continue;

        const auto LineBeginsWith = [linesv = std::string_view{line}](auto what) {
            return linesv.starts_with(what);
        };

        if (LineBeginsWith("#"))
            continue; // Skip comment

        if (sectionId != SectionID::NONE) {
            if (LineBeginsWith("end")) {
                sectionId = SectionID::NONE;
                continue;
            }

            switch (sectionId) {
            case SectionID::INST: {
                gCurrIplInstances[gCurrIplInstancesCount++] = LoadObjectInstance(line);
                break;
            }
            case SectionID::ZONE:
                LoadZone(line);
                break;
            case SectionID::CULL:
                LoadCullZone(line);
                break;
            case SectionID::OCCL:
                LoadOcclusionVolume(line, filename);
                break;
            case SectionID::PATH: {
                // This section doesn't do anything useful.
                // `LoadPedPathNode` is a NOP basically.
                // This is a leftover from VC. (Source: https://gta.fandom.com/wiki/Item_Placement#PATH )

                if (nPathEntryIndex == -1) {
                    pathHeaderId = LoadPathHeader(line, pathType);
                }
                else {
                    switch (pathType) {
                    case 0:
                        LoadPedPathNode(line, pathHeaderId, nPathEntryIndex);
                        break;
                    case 1:
                        LoadCarPathNode(line, pathHeaderId, nPathEntryIndex, false);
                        break;
                    case 2:
                        LoadCarPathNode(line, pathHeaderId, nPathEntryIndex, true);
                        break;
                    }
                    if (++nPathEntryIndex == 12)
                        nPathEntryIndex = -1;
                }
                break;
            }
            case SectionID::GRGE:
                LoadGarage(line);
                break;
            case SectionID::ENEX:
                LoadEntryExit(line);
                break;
            case SectionID::PICK:
                LoadPickup(line);
                break;
            case SectionID::CARS:
                LoadCarGenerator(line, 0);
                break;
            case SectionID::JUMP:
                LoadStuntJump(line);
                break;
            case SectionID::TCYC:
                LoadTimeCyclesModifier(line);
                break;
            case SectionID::AUZO:
                LoadAudioZone(line);
                break;
            }

            if (sectionId == SectionID::PATH)
                break; // TODO: Unsure why it stops after a path section.

        } else {
            const auto FindSectionID = [&] {
                static const struct { std::string_view name; SectionID id; } mapping[]{
                    { "path", SectionID::PATH },
                    { "inst", SectionID::INST },
                    { "mult", SectionID::MULT },
                    { "zone", SectionID::ZONE },
                    { "cull", SectionID::CULL },
                    { "occl", SectionID::OCCL },
                    { "grge", SectionID::GRGE },
                    { "enex", SectionID::ENEX },
                    { "pick", SectionID::PICK },
                    { "cars", SectionID::CARS },
                    { "jump", SectionID::JUMP },
                    { "tcyc", SectionID::TCYC },
                    { "auzo", SectionID::AUZO },
                };

                for (const auto& [itname, id] : mapping) {
                    if (LineBeginsWith(itname)) {
                        return id;
                    }
                }

                return SectionID::NONE; // Possible if the line was empty, let's move on to the next line.
            };
            sectionId = FindSectionID();
        }
    }
    CFileMgr::CloseFile(file);

    // This really seems like should be in CIplStore...
    auto newIPLIndex{ -1 };
    if (gCurrIplInstancesCount > 0) {
        newIPLIndex = CIplStore::GetNewIplEntityIndexArray(gCurrIplInstancesCount);
        rng::copy(gCurrIplInstances | std::views::take(gCurrIplInstancesCount), CIplStore::GetIplEntityIndexArray(newIPLIndex));
    }
    LinkLods(CIplStore::SetupRelatedIpls(filename, newIPLIndex, &gCurrIplInstances[gCurrIplInstancesCount]));
    CIplStore::RemoveRelatedIpls(newIPLIndex); // I mean this totally makes sense, doesn't it?
}

// todo: add original min/max check
// 0x5B8400
void CFileLoader::LoadObjectTypes(const char* filename) {
    /* Unused
    char filenameCopy[MAX_PATH]{};
    strcpy_s(filenameCopy, filename);
    */

    enum class SectionID {
        NONE = 0, // NOTSA - Placeholder value

        OBJS = 1,
        TOBJ = 3,
        WEAP = 4,
        HIER = 5,
        ANIM = 6,
        CARS = 7,
        PEDS = 8,
        PATH = 9,
        TDFX = 10, // 2DFX (but enum names can't start with a number, so..)
        TXDP = 11
    };

    auto sectionId{ SectionID::NONE };

    int32 nPathEntryIndex{ -1 }, pathHeaderId{};
    int32 pathType{};

    auto file = CFileMgr::OpenFile(filename, "rb");
    for (const char* line = LoadLine(file); line; line = LoadLine(file)) {
        if (!line[0])
            continue;

        const auto LineBeginsWith = [linesv = std::string_view{ line }](auto what) {
            return linesv.starts_with(what);
        };

        if (LineBeginsWith("#"))
            continue;

        if (sectionId != SectionID::NONE) {
            // Process section

            if (LineBeginsWith("end")) {
                sectionId = SectionID::NONE;
                continue;
            }

            switch (sectionId) {
            case SectionID::OBJS:
                LoadObject(line);
                break;
            case SectionID::TOBJ:
                LoadTimeObject(line);
                break;
            case SectionID::WEAP:
                LoadWeaponObject(line);
                break;
            case SectionID::HIER:
                LoadClumpObject(line);
                break;
            case SectionID::ANIM:
                LoadAnimatedClumpObject(line);
                break;
            case SectionID::CARS:
                LoadVehicleObject(line);
                break;
            case SectionID::PEDS:
                LoadPedObject(line);
                break;
                // R* does something weird with the object IDs frm the above cases,
                // but it isn't used, so I wont put it in here, cause it would require jumps..
            case SectionID::PATH: {
                // Leftover from VC, as path's are loaded differently in SA.
                // That is, all this does nothing in the end.

                if (nPathEntryIndex == -1) {
                    pathHeaderId = LoadPathHeader(line, pathType);
                } else {
                    switch (pathType) {
                    case 0:
                        LoadPedPathNode(line, pathHeaderId, nPathEntryIndex);
                        break;
                    case 1:
                        LoadCarPathNode(line, pathHeaderId, nPathEntryIndex, false);
                        break;
                    case 2:
                        LoadCarPathNode(line, pathHeaderId, nPathEntryIndex, true);
                        break;
                    }
                    if (++nPathEntryIndex == 12)
                        nPathEntryIndex = -1;
                }
                break;
            }
            case SectionID::TDFX:
                Load2dEffect(line);
                break;
            case SectionID::TXDP:
                LoadTXDParent(line);
                break;
            }
        } else {
            // Find out next section

            const auto FindSectionID = [&] {
                static const struct { std::string_view name; SectionID id; } mapping[]{
                    { "objs", SectionID::OBJS },
                    { "tobj", SectionID::TOBJ },
                    { "weap", SectionID::WEAP },
                    { "hier", SectionID::HIER },
                    { "anim", SectionID::ANIM },
                    { "cars", SectionID::CARS },
                    { "peds", SectionID::PEDS },
                    { "path", SectionID::PATH },
                    { "2dfx", SectionID::TDFX },
                    { "txdp", SectionID::TXDP },
                };

                for (const auto& [name, id] : mapping) {
                    if (LineBeginsWith(name)) {
                        return id;
                    }
                }

                return SectionID::NONE; // May happen if line was empty. It's fine.
            };
            sectionId = FindSectionID();
        }
    }
    CFileMgr::CloseFile(file);
}

// 0x5B3AC0
void CFileLoader::ReloadObjectTypes(const char* arg1) {
    // NOP
}

// Izzotop: Untested and may be wrong, see at your own risk
// unused
// 0x5B6E10
void CFileLoader::ReloadPaths(const char* filename) {
    int32 objModelIndex;
    int32 id;
    char  unused[4]{};

    bool pathAllocated = false;
    int32 pathEntryIndex = -1;
    auto file = CFileMgr::OpenFile(filename, "r");
    for (char* line = LoadLine(file); line; line = LoadLine(file)) {
        if (*line == '#' || !*line)
            continue;

        if (pathAllocated) {
            if (make_fourcc3(line, "end")) {
                pathAllocated = false;
            } else if (pathEntryIndex == -1) {
                VERIFY(sscanf_s(line, "%d %d %*s", &id, &objModelIndex) == 2);
                pathEntryIndex = 0;
            } else {
                if (id) {
                    if (id == 1) {
                        LoadCarPathNode(line, objModelIndex, pathEntryIndex, false);
                    } else if (id == 2) {
                        LoadCarPathNode(line, objModelIndex, pathEntryIndex, true);
                    }
                } else {
                    LoadPedPathNode(line, objModelIndex, pathEntryIndex);
                }

                if (++pathEntryIndex == 12)
                    pathEntryIndex = -1;
            }
        } else if (make_fourcc4(line, "path")) {
            pathAllocated = true;
            CPathFind::AllocatePathFindInfoMem();
        }
    }

    CFileMgr::CloseFile(file);
}

/**
 * @param atomic callback atomic
 * @param data clump object (RpClump*)
 * @return callback atomic
 * @addr 0x5B3930
 */
RpAtomic* CFileLoader::FindRelatedModelInfoCB(RpAtomic* atomic, void* data) {
    char name[24] = {0};
    bool bDamage = false;

    const char* nodeName = GetFrameNodeName(RpAtomicGetFrame(atomic));
    GetNameAndDamage(nodeName, name, bDamage);

    int32 modelId = MODEL_INVALID;
    CBaseModelInfo* mi = CModelInfo::GetModelInfo(name, &modelId);
    if (mi) {
        CAtomicModelInfo* ami = mi->AsAtomicModelInfoPtr();
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, nullptr);
        if (bDamage) {
            ami->AsDamageAtomicModelInfoPtr()->SetDamagedAtomic(atomic);
        } else {
            ami->SetAtomic(atomic);
        }
        RpClumpRemoveAtomic(static_cast<RpClump*>(data), atomic);
        RwFrame* pRwFrame = RwFrameCreate();
        RpAtomicSetFrame(atomic, pRwFrame);
        CVisibilityPlugins::SetAtomicId(atomic, modelId);
    }
    return atomic;
}

/**
 * @param atomic callback atomic
 * @param data clump object (RpClump*)
 * @return callback atomic
 * @addr 0x537150
 */
RpAtomic* CFileLoader::SetRelatedModelInfoCB(RpAtomic* atomic, void* data) {
    char name[24] = {0};
    bool bDamage = false;

    auto mi = CModelInfo::GetModelInfo(gAtomicModelId)->AsAtomicModelInfoPtr();
    auto frameNodeName = GetFrameNodeName(RpAtomicGetFrame(atomic));

    GetNameAndDamage(frameNodeName, name, bDamage);

    CVisibilityPlugins::SetAtomicRenderCallback(atomic, nullptr);

    if (bDamage)
        mi->AsDamageAtomicModelInfoPtr()->SetDamagedAtomic(atomic);
    else
        mi->SetAtomic(atomic);

    RpClumpRemoveAtomic(static_cast<RpClump*>(data), atomic);

    RpAtomicSetFrame(atomic, RwFrameCreate()); // Just create a new empty frame for it

    CVisibilityPlugins::SetAtomicId(atomic, gAtomicModelId);

    return atomic;
}

/**
 * Adds texture to the dictionary
 * @param texture callback texture
 * @param dict texture dictionary (RwTexDictionary*)
 * @return callback texture
 * @addr 0x5B38F0
 */
RwTexture* AddTextureCB(RwTexture* texture, void* dict) {
    RwTexDictionaryAddTexture((RwTexDictionary*)dict, texture);
    return texture;
}

/**
 * Makes a copy of atomic and adds it to clump
 * @param atomic callback atomic
 * @param data clump object (RpClump*)
 * @return callback atomic
 * @addr 0x537290
 */
RpAtomic* CloneAtomicToClumpCB(RpAtomic* atomic, void* data) {
    RpAtomic* clone = RpAtomicClone(atomic);
    auto frame = RpClumpGetFrame(atomic);
    RpAtomicSetFrame(clone, frame->root);
    RpClumpAddAtomic(static_cast<RpClump*>(data), clone);
    return atomic;
}

// Gets file name from a path
// 0x5B3660
const char* GetFilename(const char* filepath) {
    const char* pch = strrchr(filepath, '\\');
    return pch ? pch + 1 : filepath;
}

// 0x5B3680
void LoadingScreenLoadingFile(const char* str) {
    const char* screenName = GetFilename(str);
    sprintf_s(gString, "Loading %s", screenName);
    LoadingScreen("Loading the Game", gString);
}
