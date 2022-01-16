/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CutsceneObject.h"
#include "Directory.h"
#include "FxSystem_c.h"
#include "AnimBlendAssocGroup.h"

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
    char        _pad6A[2];
};

struct tCutsceneAttachment {
    int32 m_nCutscenePedObjectId;
    int32 m_nCutsceneAttachmentObjectId;
    int32 m_nBoneId;
};

struct tCutsceneRemoval {
    CVector m_vecPosn;
    char m_szObjectName[32];
};

extern uint32 MAX_NUM_CUTSCENE_OBJECTS; // default: 50
extern uint32 MAX_NUM_CUTSCENE_PARTICLE_EFFECTS; // default: 8
extern uint32 MAX_NUM_CUTSCENE_ITEMS_TO_HIDE; // default: 50
extern uint32 MAX_NUM_CUTSCENE_ATTACHMENTS; // default: 50

class CCutsceneMgr {
public:
    static bool &ms_useCutsceneShadows; // always 'true', doesn't change anything
    static uint32 &numPlayerWeaponsToRestore;
    static uint32 *playerWeaponsToRestore_Ammo; // static uint32 playerWeaponsToRestore_Ammo[13]
    static uint32 *playerWeaponsToRestore_Type; // static uint32 playerWeaponsToRestore_Type[13]
    static char (*ms_cAppendAnimName)[32]; // static char ms_cAppendAnimName[50][32]
    static char (*ms_cAppendObjectName)[32]; // static char ms_cAppendObjectName[50][32]
    static CDirectory *ms_pCutsceneDir;
    static uint32 &ms_cutsceneLoadStatus;
    static bool &ms_animLoaded;
    static bool &ms_running;
    static bool &ms_cutsceneProcessing;
    static bool &ms_useLodMultiplier;
    static bool &ms_wasCutsceneSkipped;
    static bool &ms_hasFileInfo;
    static uint32 &ms_numAppendObjectNames;
    static bool &restoreEverythingAfterCutscene;
    static float &m_fPrevCarDensity;
    static float &m_fPrevPedDensity;
    static tCutsceneParticleEffect *ms_pParticleEffects; // static tCutsceneParticleEffect ms_pParticleEffects[8]
    static tCutsceneRemoval *ms_crToHideItems; // static tCutsceneRemoval ms_crToHideItems[50]
    static CEntity **ms_pHiddenEntities; // static CEntity *ms_pHiddenEntities[50]
    static uint32 &ms_numAttachObjectToBones;
    static bool *ms_bRepeatObject; // static bool ms_bRepeatObject[50]
    static tCutsceneAttachment *ms_iAttachObjectToBone; // static tCutsceneAttachment ms_iAttachObjectToBone[]
    static char (*ms_aUncompressedCutsceneAnims)[32]; // static char ms_aUncompressedCutsceneAnims[8][32]
    static int32 *ms_iTextDuration; // static int32 ms_iTextDuration[64]
    static int32 *ms_iTextStartTime; // static int32 ms_iTextStartTime[64]
    static char (*ms_cTextOutput)[8]; // static char ms_cTextOutput[40][8]
    static int32 *ms_iModelIndex; // static int32 ms_iModelIndex[50]
    static char (*ms_cLoadAnimName)[32]; // static char ms_cLoadAnimName[50][32]
    static char (*ms_cLoadObjectName)[32]; // static char ms_cLoadObjectName[50][32]
    static float &ms_cutsceneTimer;
    static char *ms_cutsceneName; // static char ms_cutsceneName[8]
    static CCutsceneObject **ms_pCutsceneObjects; // static CCutsceneObject *ms_pCutsceneObject[50]
    static uint32 &ms_cutscenePlayStatus;
    static uint32 &ms_numCutsceneObjs;
    static uint32 &ms_numLoadObjectNames;
    static uint32 &ms_numTextOutput;
    static uint32 &ms_currTextOutput;
    static uint32 &ms_numUncompressedCutsceneAnims;
    static uint32 &ms_iNumHiddenEntities;
    static uint32 &ms_iNumParticleEffects;
    static uint32 &m_PrevExtraColour;
    static bool &m_PrevExtraColourOn;
    static bool &dataFileLoaded;
    static CAnimBlendAssocGroup &ms_cutsceneAssociations;
    static CVector &ms_cutsceneOffset;

    // dummy function
    static int32 AddCutsceneHead(CObject* object, int32 arg1);
    static void AppendToNextCutscene(char const* objectName, char const* animName);
    static void AttachObjectToBone(CObject* attachment, CObject* object, int32 boneId);
    static void AttachObjectToFrame(CObject* attachment, CEntity* object, char const* frameName);
    static void AttachObjectToParent(CObject* attachment, CEntity* object);
    static void BuildCutscenePlayer();
    static CCutsceneObject* CreateCutsceneObject(int32 modelId);
    static void DeleteCutsceneData();
    static void DeleteCutsceneData_overlay();
    static void FinishCutscene();
    static long long GetCutsceneTimeInMilleseconds();
    static bool HasCutsceneFinished();
    static void HideRequestedObjects();
    static void Initialise();
    static void IsCutsceneSkipButtonBeingPressed();
    static void LoadAnimationUncompressed(char const* animName);
    static void LoadCutsceneData(char const* cutsceneName);
    static void LoadCutsceneData_loading();
    static void LoadCutsceneData_overlay(char const* cutsceneName);
    static void LoadCutsceneData_postload();
    static void LoadCutsceneData_preload();
    static void LoadEverythingBecauseCutsceneDeletedAllOfIt();
    static void RemoveCutscenePlayer();
    static void RemoveEverythingBecauseCutsceneDoesntFitInMemory();
    static void SetCutsceneAnim(char const* animName, CObject* object);
    static void SetCutsceneAnimToLoop(char const* animName);
    // dummy function
    static void SetHeadAnim(char const* animName, CObject* headObject);
    static void SetupCutsceneToStart();
    static void Shutdown();
    static void SkipCutscene();
    static void StartCutscene();
    static void Update();
    static void Update_overlay();
};

int16 FindCutsceneAudioTrackId(char const* cutsceneName);
void UpdateCutsceneObjectBoundingBox(RpClump* clump, int32 modelId);