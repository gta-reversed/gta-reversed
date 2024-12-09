/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"
#include <platform/win/Platform.h>
#include <extensions/ci_string.hpp>

#include "Fx.h"
#include "CutsceneMgr.h"
#include "TaskSimpleCarSetPedOut.h"
#include "Rubbish.h"
#include <TempColModels.h>
#include <PlantMgr.h>

uint32 MAX_NUM_CUTSCENE_OBJECTS = 50;
uint32 MAX_NUM_CUTSCENE_PARTICLE_EFFECTS = 8;
uint32 MAX_NUM_CUTSCENE_ITEMS_TO_HIDE = 50;
uint32 MAX_NUM_CUTSCENE_ATTACHMENTS = 50;

//! Is the cutscene close to finishing (If this is set the camera is already fading in)
static inline auto& g_bCutSceneFinishing = StaticRef<bool, 0xBC1CF8>();

// 0x5B0380
int32 CCutsceneMgr::AddCutsceneHead(CObject* object, int32 arg1) {
    return 0;
}

// 0x4D5DB0
void CCutsceneMgr::AppendToNextCutscene(const char* objectName, const char* animName) {
    const auto CopyAndLower = [](auto&& dst, const char* src) {
        strcpy_s(dst, src);
        _strlwr_s(dst);
    };

    auto& num = ms_numAppendObjectNames;
    CopyAndLower(ms_cAppendObjectName[num], objectName);
    CopyAndLower(ms_cAppendAnimName[num], animName);
    num++;
}

// 0x5B0450
void CCutsceneMgr::AttachObjectToBone(CCutsceneObject* attachment, CCutsceneObject* object, int32 boneId) {
    attachment->m_pAttachmentObject = object;
    attachment->m_nAttachBone       = RpHAnimIDGetIndex(GetAnimHierarchyFromSkinClump(object->m_pRwClump), boneId);
}

// 0x5B0480
void CCutsceneMgr::AttachObjectToFrame(CCutsceneObject* attachment, CEntity* object, const char* frameName) {
    attachment->m_pAttachmentObject = nullptr;
    attachment->m_pAttachToFrame    = RpAnimBlendClumpFindFrame(object->m_pRwClump, frameName)->Frame;
}

// 0x5B04B0
void CCutsceneMgr::AttachObjectToParent(CCutsceneObject* attachment, CEntity* object) {
    attachment->m_pAttachmentObject = nullptr;
    attachment->m_pAttachToFrame    = RpClumpGetFrame(object->m_pRwClump);
}

// 0x4D5E20
void CCutsceneMgr::BuildCutscenePlayer() {
    const auto plyr = FindPlayerPed();
    CClothes::RebuildPlayerIfNeeded(plyr);
    CStreaming::RequestModel(MODEL_CSPLAY, STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED);
    CStreaming::LoadAllRequestedModels(true);
    CClothes::RebuildCutscenePlayer(plyr, true);
}

// 0x5B0130
RpAtomic* CalculateBoundingSphereRadiusCB(RpAtomic* atomic, void* data) {
    auto& maxRadius = *(float*)data;

    const auto sp = RpAtomicGetBoundingSphere(atomic);

    // Take bound sphere's center, and transform it all back to the world
    auto center = sp->center;
    for (RwFrame* frame = RpAtomicGetFrame(atomic); RwFrameGetParent(frame); frame = RwFrameGetParent(frame)) {
        RwV3dTransformPoint(&center, &center, RwFrameGetMatrix(frame));
    }

    // Not sure? I guess `center` is now just the offset from the object space center?
    // Fuck knows...
    maxRadius = std::max(RwV3dLength(&center) + sp->radius, maxRadius);

    return atomic;
}

// 0x5B01E0
void CCutsceneMgr::UpdateCutsceneObjectBoundingBox(RpClump* clump, eModelID modelId) {
    assert(IsModelIDForCutScene(modelId));
    assert(RwObjectGetType(clump) == rpCLUMP);

    // Figure out bounding sphere radius
    float radius = 0.f;
    RpClumpForAllAtomics(clump, CalculateBoundingSphereRadiusCB, &radius);

    // Now update the col model with it
    auto& cm = CTempColModels::ms_colModelCutObj[modelId - MODEL_CUTOBJ01];
    cm.GetBoundingSphere().m_fRadius = radius;
    cm.GetBoundingBox() = CBoundingBox{ // Bounding box of the sphere basically
        {-radius, -radius, -radius},
        { radius,  radius,  radius}
    };
}

// 0x5B02A0
CCutsceneObject* CCutsceneMgr::CreateCutsceneObject(eModelID modelId) {
    CStreaming::ImGonnaUseStreamingMemory();

    // Create col model for it (If cutscene object)
    if (IsModelIDForCutScene(modelId)) {
        const auto mi = CModelInfo::GetModelInfo(modelId);
        mi->SetColModel(&CTempColModels::ms_colModelCutObj[modelId - MODEL_CUTOBJ01]);
        UpdateCutsceneObjectBoundingBox(mi->m_pRwClump, modelId);
    }

    // Actually create the object now
    const auto obj = ms_pCutsceneObjects[ms_numCutsceneObjs++] = new CCutsceneObject{};
    obj->SetModelIndex(modelId);

    CStreaming::IHaveUsedStreamingMemory();

    return obj;
}

// 0x4D5ED0
void CCutsceneMgr::DeleteCutsceneData() {
    DeleteCutsceneData_overlay();
    CStreaming::SetMissionDoesntRequireModel(MODEL_CSPLAY);
    if (restoreEverythingAfterCutscene) {
        LoadEverythingBecauseCutsceneDeletedAllOfIt();
    }
}

// 0x5AFD60
void CCutsceneMgr::DeleteCutsceneData_overlay() {
    if (ms_cutsceneLoadStatus == LoadStatus::NOT_LOADED) {
        return;
    }

    // We suspend the timer here (is resumed at the end)
    CTimer::Suspend();

    // Restore multipliers
    CPopulation::PedDensityMultiplier = m_fPrevPedDensity;
    CCarCtrl::CarDensityMultiplier = m_fPrevCarDensity;

    // Restore extra colors
    if (m_PrevExtraColourOn) {
        CTimeCycle::StartExtraColour(m_PrevExtraColour, false);
    } else {
        CTimeCycle::StopExtraColour(false);
    }

    // Make hidden entities visible again
    for (auto& e : ms_pHiddenEntities | rngv::take(ms_iNumHiddenEntities)) {
        if (e) {
            CEntity::CleanUpOldReference(e);
            e->m_bIsVisible = true;
        }
    }
    ms_iNumHiddenEntities = 0;

    // Destroy particle effects
    for (auto& prt : ms_pParticleEffects | rngv::take(ms_iNumParticleEffects)) {
        if (const auto fx = std::exchange(prt.m_pFxSystem, nullptr)) {
            g_fxMan.DestroyFxSystem(fx);
        }
    }
    ms_iNumParticleEffects = 0;

    CMessages::ClearMessages(false);
    CRubbish::SetVisibility(true);

    ms_cutsceneProcessing = false;
    ms_useLodMultiplier = false;

    // Delete cutscene objects
    for (auto& obj : ms_pCutsceneObjects | rngv::take(ms_numCutsceneObjs)) {
        assert(obj);

        CWorld::Remove(obj);
        obj->DeleteRwObject();
        delete std::exchange(obj, nullptr);
    }
    ms_numCutsceneObjs = 0;

    // Unload anims
    if (ms_animLoaded) {
        CAnimManager::RemoveLastAnimFile();
    }
    ms_animLoaded = false;
    ms_cutsceneAssociations.DestroyAssociations();
    ms_aUncompressedCutsceneAnims[0][0] = 0;
    ms_numUncompressedCutsceneAnims = 0;

    if (dataFileLoaded) {
        TheCamera.RestoreWithJumpCut();
        TheCamera.SetWideScreenOff();
        TheCamera.DeleteCutSceneCamDataMemory();
    }

    CIplStore::ClearIplsNeededAtPosn();

    ms_cutsceneLoadStatus = LoadStatus::NOT_LOADED;
    ms_running = false;

    const auto player = FindPlayerPed();
    const auto pad = CPad::GetPad(0);
    player->m_bIsVisible = true;
    pad->bPlayerSafeForCutscene = false;
    pad->Clear(false, false); // moved up here
    player->GetPlayerInfoForThisPlayerPed()->MakePlayerSafe(0, 10000.0);

    if (!IsPlayingCSTheFinale()) {
        AudioEngine.StopCutsceneTrack(true);
        CAudioEngine::EnableEffectsLoading();
        CAEPedSpeechAudioEntity::EnableAllPedSpeech();
    }

    CStreaming::ms_disableStreaming = false;
    CWorld::bProcessCutsceneOnly = false;

    if (dataFileLoaded) {
        CGame::DrasticTidyUpMemory(TheCamera.CCamera::GetScreenFadeStatus() == NAME_FADE_IN ? true : false);
    }

    // cpad clear moved up a few lines

    // there's some code to restore the player's stored weapons, but it
    // doesn't seem to do anything, cause the guarding `if`'s
    // condition is always false
    assert(!m_bDontClearZone);

    // Tell the streamer that we don't need any (possibly) loaded special chars
    for (const auto modelId : ms_iModelIndex | rngv::take(ms_numLoadObjectNames)) {
        if (!CTheScripts::ScriptResourceManager.HasResourceBeenRequested(modelId, RESOURCE_TYPE_MODEL_OR_SPECIAL_CHAR)) {
            CStreaming::SetMissionDoesntRequireModel(modelId);
        }
    }

    CStreaming::SetMissionDoesntRequireModel(MODEL_CSPLAY);
    CStreaming::StreamZoneModels(FindPlayerCoors());

    CTimer::Resume();

    CStreaming::ForceLayerToRead(true);
}

// 0x5B04D0
void CCutsceneMgr::FinishCutscene() {
    if (dataFileLoaded) {
        ms_cutsceneTimerS = TheCamera.GetCutSceneFinishTime() / 1000.f;
        TheCamera.FinishCutscene();
    }
    FindPlayerPed()->m_bIsVisible = true;
    FindPlayerInfo().MakePlayerSafe(false, 10000.f);
}

// 0x5B0550
uint64 CCutsceneMgr::GetCutsceneTimeInMilleseconds() {
    return (uint64)ms_cutsceneTimerS * 1000;
}

// 0x5B0570
bool CCutsceneMgr::HasCutsceneFinished() {
    return !dataFileLoaded || TheCamera.GetPositionAlongSpline() == 1.0;;
}

// 0x5AFAD0
void CCutsceneMgr::HideRequestedObjects() {
    if (ms_iNumHiddenEntities == 0) {
        return;
    }

    for (const auto& cr : ms_crToHideItems | rngv::take(ms_iNumHiddenEntities)) {
        int32 modelId;
        if (!CModelInfo::GetModelInfo(cr.m_szObjectName, &modelId)) {
            DEV_LOG("Invalid model name(\"{}\")", cr.m_szObjectName);
            continue;
        }

        int16 nObjInRng;
        CEntity* objInRng[32];
        CWorld::FindObjectsOfTypeInRange(modelId, cr.m_vecPosn, 1.5f, true, &nObjInRng, (int16)std::size(objInRng), objInRng, true, false, false, true, true);
        for (auto e : objInRng | rngv::take((size_t)nObjInRng)) {
            if (!e->m_bIsVisible) {
                continue;
            }
            e->m_bIsVisible = false;
            CEntity::SetEntityReference(ms_pHiddenEntities[ms_iNumHiddenEntities++], e);
        }
    }
}

// 0x4D5A20
void CCutsceneMgr::Initialise() {
    ZoneScoped;

    ms_cutsceneLoadStatus = LoadStatus::NOT_LOADED;
    ms_running            = false;
    ms_animLoaded         = false;
    ms_cutsceneProcessing = false;
    ms_useLodMultiplier   = false;
    ms_wasCutsceneSkipped = false;
    ms_hasFileInfo        = false;
    ms_pCutsceneDir       = new CDirectory{512};
}

// 0x4D5D10
bool CCutsceneMgr::IsCutsceneSkipButtonBeingPressed() {
    const auto pad = CPad::GetPad(0);
    return pad->IsCrossPressed()
        || pad->IsMouseLButtonPressed()
        || pad->IsEnterJustPressed()
        || pad->IsStandardKeyJustDown(' ')
        || !isForeground; // ????
}

// 0x4D5AB0
void CCutsceneMgr::LoadAnimationUncompressed(const char* animName) {
    DEV_LOG("Loading uncompressed anim (\"{}\")", animName);

    strcpy_s(ms_aUncompressedCutsceneAnims[ms_numUncompressedCutsceneAnims++], animName);
    ms_aUncompressedCutsceneAnims[ms_numUncompressedCutsceneAnims][0] = 0; // Null terminate next
}

// 0x4D5E80
void CCutsceneMgr::LoadCutsceneData(const char* cutsceneName) {
    DEV_LOG("Loading cutscene data (\"{}\")", cutsceneName);

    const auto plyr = FindPlayerPed(-1);

    CClothes::RebuildPlayerIfNeeded(plyr);

    CStreaming::RequestModel(MODEL_CSPLAY, STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED);
    CStreaming::LoadAllRequestedModels(true);

    CClothes::RebuildCutscenePlayer(plyr, MODEL_CSPLAY);

    if (!ms_pCutsceneDir->HasLoaded()) {
        ms_pCutsceneDir->ReadDirFile("ANIM\\CUTS.IMG");
    }

    LoadCutsceneData_overlay(cutsceneName);
}

// 0x5B11C0
void CCutsceneMgr::LoadCutsceneData_loading() {
    // Make sure all neceesary models are streamed in
    for (const auto model : ms_iModelIndex | rngv::take(ms_numLoadObjectNames)) {
        if (!CStreaming::IsModelLoaded(model)) {
            return;
        }
    }

    // All models are streamed in now
    LoadCutsceneData_postload();

    // Load custcene anims and create objects
    CCutsceneObject* obj = nullptr;
    for (auto i = 0; i < ms_numLoadObjectNames; i++) {
        if (!ms_bRepeatObject[i]) {
            obj = CreateCutsceneObject(ms_iModelIndex[i]);
        }
        if (const auto& animName = ms_cLoadAnimName[i]; animName[0]) {
            SetCutsceneAnim(animName, obj);
        }
    }

    // Create FXs that are attached to objects
    for (auto& csfx : ms_pParticleEffects | rngv::take(ms_iNumParticleEffects)) {
        // Create the effect's transform matrix
        RwMatrix fxTransform;
        g_fx.CreateMatFromVec(&fxTransform, &csfx.m_vecPosn, &csfx.m_vecDirection);

        // Find object's matrix
        const auto objMat = [&]() -> RwMatrix* {
            const auto objIdx = csfx.m_nObjectId;

            // If not a cutscene object we don't have an object matrix - i'm not quite sure how this works, but okay.
            if (objIdx < 0 || objIdx >= ms_numCutsceneObjs + 1) { // TODO: Bug? Pretty sure the +1 is erronous...
                return nullptr;
            }

            // If it's a skinned object, we use bone indencies
            const auto csobj = ms_pCutsceneObjects[objIdx];
            if (const auto atomic = GetFirstAtomic(csobj->m_pRwClump); atomic && RpSkinGeometryGetSkin(RpAtomicGetGeometry(atomic))) {
                const auto nodeIdx = notsa::ston<uint32>({ csfx.m_szObjectPart }); // Obj Part is just an node index in this case
                const auto hier = GetAnimHierarchyFromSkinClump(csobj->m_pRwClump);
                return &RpHAnimHierarchyGetMatrixArray(hier)[RpHAnimIDGetIndex(hier, nodeIdx)];
            }

            // If not skinned, it's the name of a frame
            if (const auto frame = CClumpModelInfo::GetFrameFromName(csobj->m_pRwClump, csfx.m_szObjectPart)) {
                return RwFrameGetMatrix(frame);
            } else {
                DEV_LOG("Part(\"{}\") of object not found", csfx.m_szObjectPart);
            }

            // Otherwise we're fucked
            return nullptr;
        }();

        // Finally, create the fx
        csfx.m_pFxSystem = g_fxMan.CreateFxSystem(csfx.m_szEffectName, fxTransform, objMat, true);
    }

    // Finally, process attachments
    for (const auto& v : ms_iAttachObjectToBone | rngv::take(ms_numAttachObjectToBones)) {
        AttachObjectToBone(ms_pCutsceneObjects[v.m_nCutscenePedObjectId], ms_pCutsceneObjects[v.m_nCutscenePedObjectId], v.m_nBoneId);
    }
}

// 0x5B13F0
void CCutsceneMgr::LoadCutsceneData_overlay(const char* cutsceneName) {
    DEV_LOG("LoadCutsceneData_overlay(\"{}\")", cutsceneName);

    CTimer::Suspend();

    ms_cutsceneProcessing = true;
    ms_wasCutsceneSkipped = false;

    if (!m_bDontClearZone) {
        CStreaming::RemoveCurrentZonesModels();
    }

    ms_pCutsceneDir->Clear();
    ms_pCutsceneDir->ReadDirFile("ANIM\\CUTS.IMG");

    CStreaming::RemoveUnusedModelsInLoadedList();
    CGame::DrasticTidyUpMemory(true);
    strcpy_s(ms_cutsceneName, cutsceneName);
    CCutsceneMgr::LoadCutsceneData_preload();

    CTimer::Resume();
}

// 0x5AFBC0
void CCutsceneMgr::LoadCutsceneData_postload() {
    CMessages::ClearThisPrintBigNow(STYLE_MIDDLE);

    CPopulation::PedDensityMultiplier = 0.0;
    CCarCtrl::CarDensityMultiplier = 0.0;
    CStreaming::ms_disableStreaming = 0;

    // Load animations for this cutscene
    {
        NOTSA_LOG_TRACE("Loading anims for cutscene: ", ms_cutsceneName);

        const auto stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, "ANIM\\CUTS.IMG");
        const auto raii   = notsa::ScopeGuard{ [&] { RwStreamClose(stream, nullptr); } };

        char csIFPFile[1024];
        *std::format_to(csIFPFile, "{}.IFP", ms_cutsceneName) = 0;
        
        if (uint32 streamOffset, streamSz; ms_animLoaded = ms_pCutsceneDir->FindItem(csIFPFile, streamOffset, streamSz)) {
            CStreaming::MakeSpaceFor(streamSz * STREAMING_SECTOR_SIZE / 2); // Not sure why it's only half, but okay

            CStreaming::ImGonnaUseStreamingMemory();

            // Load ifp file
            RwStreamSkip(stream, streamOffset * STREAMING_SECTOR_SIZE);
            CAnimManager::LoadAnimFile(stream, 1, ms_aUncompressedCutsceneAnims.data());

            // Now create the anims in memory
            assert(ms_cLoadAnimName.size() == ms_cLoadObjectName.size());
            assert(std::size(ms_cLoadAnimName[0]) == std::size(ms_cLoadObjectName[0]));
            ms_cutsceneAssociations.CreateAssociations(
                ms_cutsceneName,
                ms_cLoadAnimName[0],
                ms_cLoadObjectName[0],
                std::size(ms_cLoadAnimName[0])
            );

            CStreaming::IHaveUsedStreamingMemory();
        }
    }

    // Load camera path splines for this cutscene
    {
        NOTSA_LOG_TRACE("Loading camera paths for cutscene: ", ms_cutsceneName);

        const auto img  = CFileMgr::OpenFile("ANIM\\CUTS.IMG", "rb");
        const auto raii = notsa::ScopeGuard{ [&] { CFileMgr::CloseFile(img); } };
        
        char csSplinesFile[1024];
        *std::format_to(csSplinesFile, "{}.DAT", ms_cutsceneName) = 0;

        if (uint32 streamOffset, streamSz; dataFileLoaded = ms_pCutsceneDir->FindItem(csSplinesFile, streamOffset, streamSz)) {
            CStreaming::ImGonnaUseStreamingMemory();

            CFileMgr::Seek(img, streamOffset * STREAMING_SECTOR_SIZE, SEEK_SET);
            TheCamera.LoadPathSplines(img);

            CStreaming::IHaveUsedStreamingMemory();
        }
    }

    ms_cutsceneLoadStatus = LoadStatus::LOADED;

    ms_cutsceneTimerS = 0.f;

    FindPlayerWanted()->ClearQdCrimes();
}

//! NOTSA: Code from at 0x5B0794
//! @param csFileName cutscene file name in the CUTS.IMG archive
bool CCutsceneMgr::LoadCutSceneFile(const char* csFileName) {
    DEV_LOG("LoadCutSceneFile(\"{}\")", csFileName);

    uint32 csFileOffsetBytes, csFileSzBytes;
    if (!ms_pCutsceneDir->FindItem(csFileName, csFileOffsetBytes, csFileSzBytes)) {
        return false;
    }
    csFileOffsetBytes *= STREAMING_SECTOR_SIZE;
    csFileSzBytes     *= STREAMING_SECTOR_SIZE;

    // Allocate data for the file
    auto csFileData{ std::make_unique<char[]>(csFileSzBytes * STREAMING_SECTOR_SIZE) };

    // Load cutscene data
    {
        const auto s = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, "ANIM\\CUTS.IMG");
        RwStreamSkip(s, csFileOffsetBytes); // Skip to beginning of it
        RwStreamRead(s, csFileData.get(), csFileSzBytes);
        RwStreamClose(s, nullptr);
    }

    // Def sections
    enum Section {
        NONE,
        INFO,
        MODEL,
        TEXT,
        UNCOMPRESS,
        ATTACH,
        REMOVE,
        PEFFECT,
        EXTRACOL,
        MOTION      // Unused section, but has to be present in order to avoid asserts
    };

    using namespace std::string_view_literals;

    // Parse this line as a section beginning
    const auto StringToSection = [&](std::string_view str) {
        constexpr struct { std::string_view name;  Section sec; } mapping[]{
            { "info"sv,       INFO       },
            { "model"sv,      MODEL      },
            { "text"sv,       TEXT       },
            { "uncompress"sv, UNCOMPRESS },
            { "attach"sv,     ATTACH     },
            { "remove"sv,     REMOVE     },
            { "peffect"sv,    PEFFECT    },
            { "extracol"sv,   EXTRACOL   },
            { "motion"sv,     MOTION     } 
        };
        for (const auto& [name, sec] : mapping) {
            if (str.starts_with(name)) {
                return sec;
            }
        }
        return NONE;
    };

    // We're cheating here, and using `CFileLoader::LoadLine`.
    // They originally didn't do that.
    // Now, LoadLine originally didn't handle \r\n properly,
    // but we've fixed it do so.
    // Oh, also, LoadLine changes `,` to ` ` (space), so
    // I had to change the `,`'s in the sscanf' below to be spaces.
    auto bufRemSz = (int32)csFileSzBytes;
    auto bufIt    = csFileData.get();
    auto curSec   = NONE;

    auto hasSetExtraColors = false;

    for (auto lineno = 0; ; lineno++) {
        const auto ln = CFileLoader::LoadLine(bufIt, bufRemSz);
        if (!ln) {
            break; // No more lines found
        }
        const auto lnsv = std::string_view{ ln };
        if (lnsv.empty()) {
            continue;
        }

        if (lnsv.starts_with("end"sv)) { // End of section
            curSec = NONE;
            continue;
        }

        switch (curSec) {
        case NONE: {
            curSec = StringToSection(lnsv);
            assert(curSec != NONE);
            break;
        }
        case MOTION:
            break; // Unused section, so skip parsing
        case INFO: { // 0x5B09F2
            if (lnsv.starts_with("offset"sv)) {
                auto& o = ms_cutsceneOffset;
                VERIFY(sscanf_s(ln + strlen("offset"), "%f %f %f", &o.x, &o.y, &o.z) == 3);

                // Warp ped out of the vehicle
                if (const auto plyr = FindPlayerPed(); plyr->IsInVehicle()) {
                    CTaskSimpleCarSetPedOut{ plyr->m_pVehicle, TARGET_DOOR_FRONT_LEFT, true }.ProcessPed(plyr);
                }

                // Load IPLs
                CIplStore::AddIplsNeededAtPosn(ms_cutsceneOffset);

                // Load the scene now
                CStreaming::SetLoadVehiclesInLoadScene(false);
                CStreaming::LoadScene(ms_cutsceneOffset);
                CStreaming::SetLoadVehiclesInLoadScene(true);
            } else {
                assert(0 && "incorrect operand type in info section");
            }
            break;
        }
        case MODEL: { // 0x5B0B11 
            char* stctx; // strok ctx

            (void)strtok_s(ln, ", ", &stctx); // ignore first value (it was originally a number, but it's unused - maybe model id?)

            // Find out model name
            char modelName[1024];
            strcpy_s(modelName, strtok_s(NULL, ", ", &stctx)); // This will hard crash if no model name is set
            _strlwr_s(modelName);

            // Start loading anims
            bool first = true;
            for(auto& numObj = ms_numLoadObjectNames; ;numObj++, first = false) {
                auto pAnimName = strtok_s(NULL, ", ", &stctx);
                if (!pAnimName) {
                    assert(!first);
                    break;
                }
                
                // Store model name
                strcpy_s(ms_cLoadObjectName[numObj], modelName);

                // Store anim to use
                strcpy_s(ms_cLoadAnimName[numObj], pAnimName);
                _strlwr_s(ms_cLoadAnimName[numObj]);

                ms_iModelIndex[numObj]   = first ? MODEL_LOAD_THIS : MODEL_USE_PREV;
                ms_bRepeatObject[numObj] = !first;
            }
            break;
        }
        case TEXT: { // 0x5B0C62
            auto& numTxt = ms_numTextOutput;

            int32 startTime, duration;
            auto& gxtEntry = ms_cTextOutput[numTxt];
            VERIFY(sscanf_s(ln, "%d %d %s", &startTime, &duration, gxtEntry, std::size(gxtEntry)) == 3);
            _strupr_s(gxtEntry);

            ms_iTextStartTime[numTxt] = startTime;
            ms_iTextDuration[numTxt] = duration;

            ms_numTextOutput++;
            break;
        }
        case UNCOMPRESS: { // 0x5B0D09
            char* ctx;
            LoadAnimationUncompressed(strtok_s(ln, ", ", &ctx)); // I'm not quite sure what purpose `strtok` serves here, but okay
            break;
        }
        case ATTACH: { // 0x5B0D40
            auto& a = ms_iAttachObjectToBone[ms_numAttachObjectToBones++];
            VERIFY(sscanf_s(ln, "%d %d %d", &a.m_nCutscenePedObjectId, &a.m_nCutsceneAttachmentObjectId, &a.m_nBoneId) == 3);
            break;
        }
        case REMOVE: { // 0x5B0DBA
            auto& cr = ms_crToHideItems[ms_iNumHiddenEntities++];
            auto& p  = cr.m_vecPosn;
            VERIFY(sscanf_s(ln, "%f %f %f %s", &p.x, &p.y, &p.z, cr.m_szObjectName, std::size(cr.m_szObjectName)) == 4);
            break;
        }
        case PEFFECT: { // 0x5B0E1D
            // Originally used strtok, not sure why
            auto& csfx = ms_pParticleEffects[ms_iNumParticleEffects++];
            VERIFY(sscanf_s(
                ln, "%s %d %d %d %s %f %f %f %f %f %f",
                csfx.m_szEffectName, std::size(csfx.m_szEffectName),
                &csfx.m_nStartTime,
                &csfx.m_nEndTime,
                &csfx.m_nObjectId,
                csfx.m_szObjectPart, std::size(csfx.m_szObjectPart),
                &csfx.m_vecPosn.x, &csfx.m_vecPosn.y, &csfx.m_vecPosn.z,
                &csfx.m_vecDirection.x, &csfx.m_vecDirection.y, &csfx.m_vecDirection.z
            ) == 11);
            break;
        }
        case EXTRACOL: { // 0x5B099E
            if (hasSetExtraColors) { // Pretty sure they fucked up something, see note below VVVV
                continue;
            }
            int32 extraColIdx;
            VERIFY(sscanf_s(ln, "%d", &extraColIdx) == 1);
            if (extraColIdx) {
                CTimeCycle::StartExtraColour(extraColIdx - 1, false);
                hasSetExtraColors = true;
            } else {
                hasSetExtraColors = false; // unreachable, see note above ^^^^^^
            }
            break;
        }
        }
    }

    if (!hasSetExtraColors) {
        CTimeCycle::StopExtraColour(false);
    }

    return true;
}

// 0x5B05A0
void CCutsceneMgr::LoadCutsceneData_preload() {
    // Preload cutscene track (Except for the "finale" cutscene)
    if (!IsPlayingCSTheFinale()) {
        if (const auto trkId = FindCutsceneAudioTrackId(ms_cutsceneName); trkId != -1) {
            AudioEngine.PreloadCutsceneTrack(trkId, true);
        }
    }

    // Unload cutscene models
    for (int32 i = MODEL_CUTOBJ01; i < MODEL_CUTOBJ20; i++) {
        const auto mi = CModelInfo::GetModelInfo(i);
        if (mi->m_nRefCount) {
            continue;
        }
        if (!CStreaming::IsModelLoaded(i)) {
            continue;
        }
        CStreaming::RemoveModel(i);
        mi->m_nKey = CKeyGen::GetUppercaseKey("&*%"); // Set some invalid key I guess? I don't think anything like this is used anywhere :D
    }

    // Save states
    m_PrevExtraColour   = CTimeCycle::m_ExtraColour;
    m_PrevExtraColourOn = CTimeCycle::m_bExtraColourOn;

    m_fPrevCarDensity   = CCarCtrl::CarDensityMultiplier;
    m_fPrevPedDensity   = CPopulation::PedDensityMultiplier;

    // Reset internal states
    ms_cutsceneOffset               = CVector{};
    ms_numTextOutput                = 0;
    ms_currTextOutput               = 0;
    ms_numLoadObjectNames           = 0;
    ms_numUncompressedCutsceneAnims = 0;
    ms_numAttachObjectToBones       = 0;
    ms_iNumHiddenEntities           = 0;
    ms_iNumParticleEffects          = 0;
    g_bCutSceneFinishing            = false;

    for (auto&& name : ms_aUncompressedCutsceneAnims) { // Clear all names
        name[0] = '\0';
    }

    rng::fill(ms_iModelIndex, MODEL_PLAYER); // TODO: Change to MODEL_INVALID

    TheCamera.SetNearClipScript(0.1f);

    CRubbish::SetVisibility(false);

    FindPlayerWanted()->ClearQdCrimes();
    auto player = FindPlayerPed();
    player->m_bIsVisible = false;
    player->ResetSprintEnergy();
    CPad::GetPad()->bPlayerSafeForCutscene = true;
    FindPlayerInfo().MakePlayerSafe(true, 10000.f);

    // Load cutscene data file from `CUTS.IMG`
    char csFileName[1024];
    *std::format_to(csFileName, "{}.CUT", ms_cutsceneName) = 0;
    if (!LoadCutSceneFile(csFileName)) {
        DEV_LOG("Failed loading cutscene(\"{}\") def", ms_cutsceneName);
        LoadCutsceneData_postload();
        return;
    }

    // 0x5B1011
    // Append objects added by script to the ones added by `LoadCutSceneFile`
    rng::fill(ms_iModelIndex | rngv::drop(ms_numLoadObjectNames) | rngv::take(ms_numAppendObjectNames), MODEL_LOAD_THIS); // Appended objects are loaded directly
    for (auto i = 0; i < ms_numAppendObjectNames; i++) {
        const auto objId = ms_numLoadObjectNames++;
        strcpy_s(ms_cLoadObjectName[objId], ms_cAppendObjectName[objId]);
        strcpy_s(ms_cLoadAnimName[objId], ms_cAppendAnimName[objId]);
    }
    ms_numAppendObjectNames = 0;

    if (ms_numLoadObjectNames == 0) {
        LoadCutsceneData_postload();
        return;
    }

    // Request all models to be loaded [Added by `LoadCutSceneFile` and the scripts]
    size_t specialModelOffset = 0;
    for (auto i = 0; i < ms_numLoadObjectNames; i++) {
        const auto& modelName = ms_cLoadObjectName[i];

        if (_stricmp(modelName, "csplay") == 0) {
            ms_iModelIndex[i] = MODEL_CSPLAY;
            continue;
        }

        switch (ms_iModelIndex[i]) {
        case MODEL_LOAD_THIS: { // 0x5B10F0 - Load a new model
            auto& modelId = ms_iModelIndex[i];

            if (CModelInfo::GetModelInfo(modelName, (int32*)&modelId)) { // Regular model
                CStreaming::RequestModel(modelId, STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED);
            } else { // Special model
                // Figure out cut-scene model ID
                modelId = (eModelID)(MODEL_CUTOBJ01 + specialModelOffset++);

                // Request the model to be loaded
                CStreaming::RequestSpecialModel(modelId, modelName, STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED);

                // Find next not-yet loaded cutscene model
                while (CStreaming::IsModelLoaded((eModelID)(MODEL_CUTOBJ01 + specialModelOffset))) { // TODO/BUG: Missing check (should only try cutscene models (eg.: up to MODEL_CUTOBJ20), nothing more)
                    specialModelOffset++;
                }
                assert(specialModelOffset <= (MODEL_CUTOBJ20 - MODEL_CUTOBJ01));
            }
            break;
        }
        case MODEL_USE_PREV: { // Just use the previous model
            assert(i > 0);     // The model index array should always start with `MODEL_LOAD_THIS`
            ms_iModelIndex[i] = ms_iModelIndex[i - 1];
            break;
        }
        }
    }
    CStreaming::LoadAllRequestedModels(true);
    ms_cutsceneLoadStatus = LoadStatus::LOADING;
}

// 0x4D5C10
void CCutsceneMgr::LoadEverythingBecauseCutsceneDeletedAllOfIt() {
    restoreEverythingAfterCutscene = false;
    CStreaming::LoadInitialPeds();
    CStreaming::LoadInitialWeapons();

    //There's some code below that is unreachable
    //reason being that `numPlayerWeaponsToRestore` is only increased in a function
    //that's never called (`RemoveEverythingBecauseCutsceneDoesntFitInMemory`)
    assert(numPlayerWeaponsToRestore <= 0);
}

// 0x4D5E50
void CCutsceneMgr::RemoveCutscenePlayer() {
    CStreaming::SetMissionDoesntRequireModel(MODEL_CSPLAY);
}

// 0x4D5AF0
void CCutsceneMgr::RemoveEverythingBecauseCutsceneDoesntFitInMemory() {
    NOTSA_UNREACHABLE(); // Unused function
}

// 0x5B0390
void CCutsceneMgr::SetCutsceneAnim(const char* animName, CObject* object) {
    const auto theAnim = ms_cutsceneAssociations.GetAnimation(animName);
    if (!theAnim) {
        DEV_LOG("Animation (\"{}\") not found!", animName);
        return;
    }

    if (theAnim->m_BlendHier->m_bIsCompressed) {
        theAnim->m_BlendHier->m_bKeepCompressed = true;
    }

    CStreaming::ImGonnaUseStreamingMemory();
    const auto cpyOfTheAnim = ms_cutsceneAssociations.CopyAnimation(animName);
    CStreaming::IHaveUsedStreamingMemory();

    cpyOfTheAnim->SetFlag(ANIMATION_CAN_EXTRACT_VELOCITY, true);
    cpyOfTheAnim->Start(0.f);

    const auto blendData = RpAnimBlendClumpGetData(object->m_pRwClump);
    blendData->m_AnimList.Prepend(&cpyOfTheAnim->m_Link);

    if (cpyOfTheAnim->m_BlendHier->m_bKeepCompressed) {
        blendData->m_FrameDatas[0].IsCompressed = true;
    }
}

// 0x5B0420
void CCutsceneMgr::SetCutsceneAnimToLoop(const char* animName) {
    ms_cutsceneAssociations.GetAnimation(animName)->m_Flags |= ANIMATION_IS_LOOPED;
}

// 0x5B0440
void CCutsceneMgr::SetHeadAnim(const char* animName, CObject* headObject) {
    // NOP
}

// 0x5B14D0
void CCutsceneMgr::SetupCutsceneToStart() {
    // Calculate cutscene object world positions
    ms_cutsceneOffset.z += 1.f;
    for (auto i = ms_numCutsceneObjs; i-- > 0;) {
        const auto csobj = ms_pCutsceneObjects[i];
        assert(csobj);

        const auto SetObjPos = [csobj](const CVector& pos) {
            csobj->m_vWorldPosition = pos;
            csobj->SetPosn(pos);
        };

        if (const auto anim = RpAnimBlendClumpGetFirstAssociation(csobj->m_pRwClump)) {
            if (csobj->m_pAttachToFrame) {
                anim->SetFlag(ANIMATION_CAN_EXTRACT_VELOCITY, false);
                anim->SetFlag(ANIMATION_IS_PLAYING, true);
            } else {
                // Get anim translation and offset the object's position by it
                const CVector animTrans = anim->m_BlendHier->m_bIsCompressed
                    ? (CVector)anim->m_BlendHier->m_pSequences[0].GetCKeyFrame(0)->Trans
                    : (CVector)anim->m_BlendHier->m_pSequences[0].GetUKeyFrame(0)->Trans;
                SetObjPos(ms_cutsceneOffset + animTrans);

                // Start the anim
                anim->SetFlag(ANIMATION_IS_PLAYING, true);
            }
        } else { // Object has no animation applied
            SetObjPos(ms_cutsceneOffset);
        }

        // Add it to the world and update skinning
        CWorld::Add(csobj);
        if (RwObjectGetType(csobj->m_pRwObject) == rpCLUMP) {
            csobj->UpdateRpHAnim();
        }
    }

    CTimer::Update();
    CTimer::Update(); // unnecessary

    ms_cutsceneTimerS = 0.f;
    ms_running       = true;
}

// 0x4D5E60
void CCutsceneMgr::Shutdown() {
    delete ms_pCutsceneDir;
}

// 0x5B1700
void CCutsceneMgr::SkipCutscene() {
    CHud::m_BigMessage[STYLE_BOTTOM_RIGHT][0] = 0; // todo: add CHud::ClearBigMessage
    ms_wasCutsceneSkipped = true;
    FinishCutscene();
}

// 0x5B1460
void CCutsceneMgr::StartCutscene() {
    CCutsceneMgr::ms_cutscenePlayStatus = PlayStatus::STARTING;
    if (dataFileLoaded) {
        TheCamera.SetCamCutSceneOffSet(ms_cutsceneOffset);
        TheCamera.TakeControlWithSpline(eSwitchType::JUMPCUT);
        TheCamera.SetWideScreenOn();

        CHud::SetHelpMessage(nullptr, true, false, false);
        CPlantMgr::PreUpdateOnceForNewCameraPos(TheCamera.GetPosition());
    }
}

// 0x4D5D00
void CCutsceneMgr::Update() {
    ZoneScoped;

    if (ms_cutsceneLoadStatus != LoadStatus::NOT_LOADED) {
        Update_overlay();
    }
}

// 0x5B1720
void CCutsceneMgr::Update_overlay() {
    CIplStore::AddIplsNeededAtPosn(ms_cutsceneOffset);
    switch (ms_cutsceneLoadStatus) {
    case LoadStatus::LOADING: {
        CTimer::Suspend();
        LoadCutsceneData_loading();
        CTimer::Resume();
        break;
    }
    case LoadStatus::LOADED: {
        ms_cutscenePlayStatus = [&]{
            switch (ms_cutscenePlayStatus) {
            case PlayStatus::S0:
                return ms_cutscenePlayStatus; // no change
            case PlayStatus::STARTING: {
                SetupCutsceneToStart();
                HideRequestedObjects();
                if (!IsPlayingCSTheFinale()) {
                    CAudioEngine::DisableEffectsLoading();
                    CAEPedSpeechAudioEntity::DisableAllPedSpeech();
                    CAudioEngine::PlayPreloadedCutsceneTrack();
                }
                return PlayStatus::S2;
            }
            case PlayStatus::S2: return PlayStatus::S3;
            case PlayStatus::S3: return PlayStatus::S4;
            case PlayStatus::S4: return PlayStatus::S0;
            default:             NOTSA_UNREACHABLE();
            }
        }();

        if (!ms_running) {
            break;
        }

        ms_cutsceneTimerS += CTimer::GetTimeStepNonClippedInSeconds();
        const auto csTimeMS = (int32)(ms_cutsceneTimerS * 1000.f);

        // Deal with fx [Start and stop them]
        for (auto& csfx : ms_pParticleEffects | rngv::take(ms_iNumParticleEffects)) {
            const auto fxsys = csfx.m_pFxSystem;
            if (!fxsys) {
                continue; // I'm not sure how this could happen, but okay.
            }
            if (csfx.m_bPlaying) {
                if (csfx.m_nEndTime != -1) { // Time to end?
                    if (csTimeMS >= csfx.m_nEndTime + csfx.m_nStartTime) {
                        fxsys->Stop();
                        csfx.m_bPlaying = false;
                        csfx.m_bStopped = true;
                    }
                }
            } else if (!csfx.m_bStopped && csTimeMS >= csfx.m_nStartTime) { // Time to start? (Only if not stopped once already)
                fxsys->Play();
                csfx.m_bPlaying = true;
            }
        }

        // Deal with text upcoming texts
        if (ms_currTextOutput < ms_numTextOutput && csTimeMS >= ms_iTextStartTime[ms_currTextOutput]) {
            CMessages::AddMessageJump(
                TheText.Get(ms_cTextOutput[ms_currTextOutput]),
                ms_iTextDuration[ms_currTextOutput],
                1,
                true
            );
            ms_currTextOutput++;
        }

        // Update cutscene specific model bounding boxes
        for (const auto csobj : ms_pCutsceneObjects | rngv::take(ms_numCutsceneObjs)) {
            if (IsModelIDForCutScene(csobj->GetModelID())) {
                UpdateCutsceneObjectBoundingBox(csobj->m_pRwClump, csobj->GetModelID());
            }
        }

        // Deal with cutscene skip user input + fading at the end
        if (dataFileLoaded && !IsPlayingCSTheFinale()) {
            if (TheCamera.GetActiveCam().m_nMode == MODE_FLYBY) { // load status check is redudant here
                if (csTimeMS + 1000 > (int32)TheCamera.GetCutSceneFinishTime() && !g_bCutSceneFinishing) {
                    g_bCutSceneFinishing = true;
                    TheCamera.Fade(1.f, eFadeFlag::FADE_IN);
                }
                if (IsCutsceneSkipButtonBeingPressed()) {
                    SkipCutscene();
                }
            }
        }
        break;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

bool CCutsceneMgr::IsPlayingCSTheFinale() {
    return notsa::ci_string_view{ ms_cutsceneName } == "finale";
}

// 0x5AFA50
int16 FindCutsceneAudioTrackId(const char* cutsceneName) {
    // from 0x8D0AA8 
    constexpr struct {
        notsa::ci_string_view name;
        int32 id;
    } mapping[]{
        { "BCESAR2", 626 }, { "BCESAR4", 627 }, { "BCESA4W", 628 }, { "BCESAR5", 629 }, { "BCESA5W", 630 }, { "BCRAS1", 631 }, { "BCRAS2", 632 }, { "BHILL1", 633 }, { "BHILL2", 634 }, { "BHILL3a", 635 }, { "BHILL3b", 636 }, { "BHILL3c", 637 }, { "BHILL5a", 638 }, { "BHILL5b", 639 }, { "CAS_1a", 640 }, { "CAS_1b", 642 }, { "CAS_2", 643 }, { "CAS_3", 644 }, { "CAS_4a", 645 }, { "CAS_4b", 646 }, { "CAS_4c", 647 }, { "CAS_5a", 648 }, { "CAS_6a", 649 }, { "CAS6b_1", 650 }, { "CAS6b_2", 651 }, { "CAS_7b", 652 }, { "CAS_9a1", 653 }, { "CAS_9a2", 654 }, { "CAS_11a", 641 }, { "CAT_1", 655 }, { "CAT_2", 656 }, { "CAT_3", 657 }, { "CAT_4", 658 }, { "CESAR1A", 659 }, { "CESAR2A", 660 }, { "CRASH1A", 661 }, { "CRASH2A", 662 }, { "CRASH3A", 663 }, { "CRASHV1", 664 }, { "CRASv2a", 665 }, { "CRASv2b", 666 }, { "D10_ALT", 667 }, { "D8_ALT", 668 }, { "DESERT1", 671 }, { "DESERT2", 672 }, { "DESERT3", 673 }, { "DESERT4", 674 }, { "DESERT6", 675 }, { "DESERT8", 676 }, { "DESERT9", 677 }, { "DES_10A", 678 }, { "DES_10B", 679 }, { "DOC_2", 680 }, { "EPILOG", 681 }, { "FARL_2A", 682 }, { "FARL_3A", 683 }, { "FARL_3B", 684 }, { "FARL_4A", 685 }, { "FARL_5A", 686 }, { "FINAL1A", 687 }, { "FINAL2A", 688 }, { "FINAL2B", 689 }, { "GARAG1B", 690 }, { "GARAG1C", 691 }, { "GARAG3A", 692 }, { "GROVE1a", 693 }, { "GROVE1b", 694 }, { "GROVE1c", 695 }, { "GROVE2", 696 }, { "HEIST1a", 697 }, { "HEIST2a", 698 }, { "HEIST4a", 699 }, { "HEIST5a", 700 }, { "HEIST6a", 701 }, { "HEIST8a", 702 }, { "INTRO1A", 703 }, { "INTRO1B", 704 }, { "INTRO2A", 705 }, { "PROLOG1", 706 }, { "PROLOG2", 707 }, { "PROLOG3", 708 }, { "RIOT_1a", 709 }, { "RIOT_1b", 710 }, { "RIOT_2", 711 }, { "RIOT_4a", 712 }, { "RIOT_4b", 713 }, { "RIOT_4c", 714 }, { "RIOT_4d", 715 }, { "RIOT4e1", 716 }, { "RIOT4e2", 717 }, { "RYDER1A", 718 }, { "RYDER2A", 719 }, { "RYDER3A", 720 }, { "SCRASH1", 721 }, { "SCRASH2", 722 }, { "SMOKE1A", 723 }, { "SMOKE1B", 724 }, { "SMOKE2A", 725 }, { "SMOKE2B", 726 }, { "SMOKE3A", 727 }, { "SMOKE4A", 728 }, { "STEAL_1", 729 }, { "STEAL_2", 730 }, { "STEAL_4", 731 }, { "STEAL_5", 732 }, { "STRAP1A", 733 }, { "STRAP2A", 734 }, { "STRAP3A", 735 }, { "STRAP4A", 736 }, { "STRP4B1", 737 }, { "STRP4B2", 738 }, { "SWEET1A", 739 }, { "SWEET1B", 740 }, { "SWEET1C", 741 }, { "SWEET2A", 742 }, { "SWEET2B", 743 }, { "SWEET3A", 744 }, { "SWEET3B", 745 }, { "SWEET4A", 746 }, { "SWEET5A", 747 }, { "SWEET6A", 748 }, { "SWEET6B", 749 }, { "SWEET7A", 750 }, { "SYND_2A", 751 }, { "SYND_2B", 752 }, { "SYND_3A", 753 }, { "SYND_4A", 754 }, { "SYND_4B", 755 }, { "SYND_7", 756 }, { "TRUTH_1", 758 }, { "TRUTH_2", 757 }, { "W2_ALT", 759 }, { "WOOZI1A", 761 }, { "WOOZI1B", 762 }, { "WOOZIE2", 760 }, { "WOOZIE4", 763 }, { "ZERO_1", 764 }, { "ZERO_2", 765 }, { "ZERO_4", 766 }, { "DATE1a", 670 }, { "DATE1b", 669 }, { "DATE2a", 670 }, { "DATE2b", 669 }, { "DATE3a", 670 }, { "DATE3b", 669 }, { "DATE4a", 670 }, { "DATE4b", 669 }, { "DATE5a", 670 }, { "DATE5b", 669 }, { "DATE6a", 670 }, { "DATE6b", 669 }
    };
    const auto csName = notsa::ci_string_view{ cutsceneName };
    for (const auto& [name, id] : mapping) {
        if (csName == name) {
            return id;
        }
    }
    return -1;
}

void CCutsceneMgr::InjectHooks() {
    RH_ScopedClass(CCutsceneMgr);
    RH_ScopedCategoryGlobal();

    //RH_ScopedGlobalInstall(FindCutsceneAudioTrackId, 0x8D0AA8, {.locked = true}); // Calling the original function from our code crashes, and vice versa
    RH_ScopedGlobalInstall(SetCutsceneAnim, 0x5B0390);
    RH_ScopedGlobalInstall(SetCutsceneAnimToLoop, 0x5B0420);
    RH_ScopedGlobalInstall(SetHeadAnim, 0x5B0440);
    RH_ScopedGlobalInstall(AttachObjectToBone, 0x5B0450);
    RH_ScopedGlobalInstall(AttachObjectToFrame, 0x5B0480);
    RH_ScopedGlobalInstall(AttachObjectToParent, 0x5B04B0);
    RH_ScopedGlobalInstall(AddCutsceneHead, 0x5B0380);
    RH_ScopedGlobalInstall(FinishCutscene, 0x5B04D0);
    RH_ScopedGlobalInstall(HasCutsceneFinished, 0x5B0570);
    RH_ScopedGlobalInstall(LoadCutsceneData_preload, 0x5B05A0);
    RH_ScopedGlobalInstall(LoadCutsceneData_loading, 0x5B11C0);
    RH_ScopedGlobalInstall(LoadCutsceneData_overlay, 0x5B13F0);
    RH_ScopedGlobalInstall(StartCutscene, 0x5B1460);
    RH_ScopedGlobalInstall(SetupCutsceneToStart, 0x5B14D0);
    RH_ScopedGlobalInstall(GetCutsceneTimeInMilleseconds, 0x5B0550);
    RH_ScopedGlobalInstall(CreateCutsceneObject, 0x5B02A0);
    RH_ScopedGlobalInstall(DeleteCutsceneData_overlay, 0x5AFD60);
    RH_ScopedGlobalInstall(LoadCutsceneData_postload, 0x5AFBC0);
    RH_ScopedGlobalInstall(Initialise, 0x4D5A20);
    RH_ScopedGlobalInstall(LoadAnimationUncompressed, 0x4D5AB0);
    RH_ScopedGlobalInstall(RemoveEverythingBecauseCutsceneDoesntFitInMemory, 0x4D5AF0);
    RH_ScopedGlobalInstall(LoadEverythingBecauseCutsceneDeletedAllOfIt, 0x4D5C10);
    RH_ScopedGlobalInstall(Update, 0x4D5D00);
    RH_ScopedGlobalInstall(IsCutsceneSkipButtonBeingPressed, 0x4D5D10);
    RH_ScopedGlobalInstall(AppendToNextCutscene, 0x4D5DB0);
    RH_ScopedGlobalInstall(BuildCutscenePlayer, 0x4D5E20);
    RH_ScopedGlobalInstall(RemoveCutscenePlayer, 0x4D5E50);
    RH_ScopedGlobalInstall(Shutdown, 0x4D5E60);
    RH_ScopedGlobalInstall(LoadCutsceneData, 0x4D5E80);
    RH_ScopedGlobalInstall(DeleteCutsceneData, 0x4D5ED0);
    RH_ScopedGlobalInstall(HideRequestedObjects, 0x5AFAD0);
    RH_ScopedGlobalInstall(UpdateCutsceneObjectBoundingBox, 0x5B01E0);
    RH_ScopedGlobalInstall(CalculateBoundingSphereRadiusCB, 0x5B0130);
    RH_ScopedGlobalInstall(SkipCutscene, 0x5B1700);
    RH_ScopedGlobalInstall(Update_overlay, 0x5B1720);
}
