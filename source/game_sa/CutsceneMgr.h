/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include <extensions/utility.hpp>

class CEntity;
class CObject;
class CDirectory;
class CCutsceneObject;
class CAnimBlendAssocGroup;

struct tCutsceneParticleEffect {
    char        m_szEffectName[32];
    FxSystem_c* m_pFxSystem;
    int32       m_nStartTime;
    int32       m_nEndTime;
    int32       m_nObjectId;
    char        m_szObjectPart[32];
    CVector     m_vecPosn;
    CVector     m_vecDirection;
    bool        m_bPlaying;
    bool        m_bStopped;
};
VALIDATE_SIZE(tCutsceneParticleEffect, 0x6C);

struct tCutsceneAttachment {
    int32 m_nCutscenePedObjectId;
    int32 m_nCutsceneAttachmentObjectId;
    int32 m_nBoneId;
};

struct tCutsceneRemoval {
    CVector m_vecPosn;
    char    m_szObjectName[32];
};

extern uint32 MAX_NUM_CUTSCENE_OBJECTS; // default: 50
extern uint32 MAX_NUM_CUTSCENE_PARTICLE_EFFECTS; // default: 8
extern uint32 MAX_NUM_CUTSCENE_ITEMS_TO_HIDE; // default: 50
extern uint32 MAX_NUM_CUTSCENE_ATTACHMENTS; // default: 50


class CCutsceneMgr {
public:
    enum class LoadStatus {
        NOT_LOADED,
        LOADING,
        LOADED,
    };

    enum class PlayStatus {
        S0,
        STARTING = 1,
        S2,
        S3,
        S4
    };

public:
    static inline auto& ms_useCutsceneShadows = StaticRef<int8>(0x8AC158);
    static inline auto& numPlayerWeaponsToRestore = StaticRef<int32>(0xB5EB58);

    static inline auto& playerWeaponsToRestore_Ammo = StaticRef<std::array<int32, 13>>(0xB5EB5C); // TODO: Where does the 13 number come from? 
    static inline auto& playerWeaponsToRestore_Type = StaticRef<std::array<int32, 13>>(0xB5EB90); // TODO: Where does the 13 number come from?

    static inline auto& ms_cAppendAnimName = StaticRef<std::array<char[32], 50>>(0xB5EBC8);
    static inline auto& ms_cAppendObjectName = StaticRef<std::array<char[32], 50>>(0xB5F208);

    static inline auto& ms_pCutsceneDir = StaticRef<CDirectory*>(0xB5F848);
    static inline auto& ms_cutsceneLoadStatus = StaticRef<LoadStatus>(0xB5F84C);
    static inline auto& ms_running = StaticRef<int8>(0xB5F851);
    static inline auto& ms_cutsceneProcessing = StaticRef<bool>(0xB5F852);
    static inline auto& ms_useLodMultiplier = StaticRef<bool>(0xB5F853);
    static inline auto& ms_wasCutsceneSkipped = StaticRef<bool>(0xB5F854);
    static inline auto& ms_hasFileInfo = StaticRef<int8>(0xB5F855);
    static inline auto& ms_numAppendObjectNames = StaticRef<int32>(0xB5F858);
    static inline auto& restoreEverythingAfterCutscene = StaticRef<bool>(0xB5F85C);
    static inline auto& m_fPrevCarDensity = StaticRef<float>(0xBC1D68);
    static inline auto& m_fPrevPedDensity = StaticRef<float>(0xBC1D6C);
    static inline auto& ms_pParticleEffects = StaticRef<std::array<tCutsceneParticleEffect, 8>>(0xBC1D70);
    static inline auto& ms_crToHideItems = StaticRef<std::array<tCutsceneRemoval, 50>>(0xBC20D0);
    static inline auto& ms_pHiddenEntities = StaticRef<std::array<CEntity*, 50>>(0xBC2968);
    static inline auto& ms_numAttachObjectToBones = StaticRef<int32>(0xBC2A30);
    static inline auto& ms_bRepeatObject = StaticRef<std::array<char, 50>>(0xBC2A34);
    static inline auto& ms_iAttachObjectToBone = StaticRef<std::array<tCutsceneAttachment, 50>>(0xBC2A68);
    static inline auto& ms_aUncompressedCutsceneAnims = StaticRef<std::array<char[32], 8>>(0xBC2CC0); 
    static inline auto& ms_iTextDuration = StaticRef<std::array<int32, 64>>(0xBC2DC0);
    static inline auto& ms_iTextStartTime = StaticRef<std::array<int32, 64>>(0xBC2EC0);
    static inline auto& ms_cTextOutput = StaticRef<std::array<char[8], 64>>(0xBC2FC0);
    static inline auto& ms_iModelIndex = StaticRef<std::array<eModelID, 50>>(0xBC31C0);
    static inline auto& ms_cLoadAnimName = StaticRef<std::array<char[32], 50>>(0xBC3288);
    static inline auto& ms_cLoadObjectName = StaticRef<std::array<char[32], 50>>(0xBC38C8);
    static inline auto& ms_cutsceneTimerS = StaticRef<float>(0xBC3F08); // In seconds
    static inline auto& ms_cutsceneName = StaticRef<char[8]>(0xBC3F0C);
    static inline auto& ms_pCutsceneObjects = StaticRef<std::array<CCutsceneObject*, 50>>(0xBC3F18);
    static inline auto& ms_cutscenePlayStatus = StaticRef<PlayStatus>(0xBC3FE0);
    static inline auto& ms_numCutsceneObjs = StaticRef<int32>(0xBC3FE4);
    static inline auto& ms_numLoadObjectNames = StaticRef<int32>(0xBC3FE8);
    static inline auto& ms_numTextOutput = StaticRef<int32>(0xBC3FEC);
    static inline auto& ms_currTextOutput = StaticRef<int32>(0xBC3FF0);
    static inline auto& ms_numUncompressedCutsceneAnims = StaticRef<uint32>(0xBC3FF4);
    static inline auto& ms_iNumHiddenEntities = StaticRef<uint32>(0xBC3FF8);
    static inline auto& ms_iNumParticleEffects = StaticRef<uint32>(0xBC3FFC);
    static inline auto& m_PrevExtraColour = StaticRef<int32>(0xBC4000);
    static inline auto& m_PrevExtraColourOn = StaticRef<bool>(0xBC4004);
    static inline auto& m_bDontClearZone = StaticRef<bool>(0xBC4005);

    //! If the camera splines were loaded (See `LoadCutsceneData_postload`)
    //! from the .DAT file of the cutscene found in CUTS.IMG
    static inline auto& dataFileLoaded = StaticRef<bool>(0xBC4006);

    //! If the anims were loaded (From the cutscene's .IFP file found in CUTS.IMG)
    static inline auto& ms_animLoaded = StaticRef<bool>(0xB5F850);

    static inline auto& ms_cutsceneAssociations = StaticRef<CAnimBlendAssocGroup>(0xBC4020);
    static inline auto& ms_cutsceneOffset = StaticRef<CVector>(0xBC4034);

    static void InjectHooks();

    static int32 AddCutsceneHead(CObject* object, int32 arg1);
    static void AppendToNextCutscene(const char* objectName, const char* animName);
    static void AttachObjectToBone(CCutsceneObject* attachment, CCutsceneObject* object, int32 boneId);
    static void AttachObjectToFrame(CCutsceneObject* attachment, CEntity* object, const char* frameName);
    static void AttachObjectToParent(CCutsceneObject* attachment, CEntity* object);
    static void BuildCutscenePlayer();
    static void UpdateCutsceneObjectBoundingBox(RpClump* clump, eModelID modellId);
    static CCutsceneObject* CreateCutsceneObject(eModelID modelId);
    static void DeleteCutsceneData();
    static void DeleteCutsceneData_overlay();
    static void FinishCutscene();
    static uint64 GetCutsceneTimeInMilleseconds();
    static bool HasCutsceneFinished();
    static void HideRequestedObjects();
    static void Initialise();
    static bool IsCutsceneSkipButtonBeingPressed();
    static void LoadAnimationUncompressed(const char* animName);
    static void LoadCutsceneData(const char* cutsceneName);
    static void LoadCutsceneData_loading();
    static void LoadCutsceneData_overlay(const char* cutsceneName);
    static void LoadCutsceneData_postload();
    static bool LoadCutSceneFile(const char* csFileName);
    static void LoadCutsceneData_preload();
    static void LoadEverythingBecauseCutsceneDeletedAllOfIt();
    static void RemoveCutscenePlayer();
    static void RemoveEverythingBecauseCutsceneDoesntFitInMemory();
    static void SetCutsceneAnim(const char* animName, CObject* object);
    static void SetCutsceneAnimToLoop(const char* animName);
    static void SetHeadAnim(const char* animName, CObject* headObject);
    static void SetupCutsceneToStart();
    static void Shutdown();
    static void SkipCutscene();
    static void StartCutscene();
    static void Update();
    static void Update_overlay();


    static bool IsRunning() { return ms_running; }
    static bool IsCutsceneProcessing() { return ms_cutsceneProcessing; }
    static bool HasLoaded() { return ms_cutsceneLoadStatus == LoadStatus::LOADED; }
    static bool IsLoading() { return ms_cutsceneLoadStatus == LoadStatus::LOADING; }
    static bool IsPlayingCSTheFinale();
};

int16 FindCutsceneAudioTrackId(const char* cutsceneName);
void UpdateCutsceneObjectBoundingBox(RpClump* clump, int32 modelId);
