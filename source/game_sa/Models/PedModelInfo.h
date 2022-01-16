/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "ClumpModelInfo.h"
#include "ColModel.h"

struct tPedColNodeInfo {
    char    _pad[4];
    int32   m_nBoneID; // see ePedBones
    int32   m_nFlags;
    CVector m_vecCenter;
    float   m_fRadius;
};

class CPedModelInfo : public CClumpModelInfo {
public:
    CPedModelInfo() : CClumpModelInfo(), m_pHitColModel(nullptr) {}
    ~CPedModelInfo() { if (m_pHitColModel) delete m_pHitColModel; }
public:
    int32      m_nAnimType;
    uint32     m_nPedType;
    uint32     m_nStatType;
    uint16     m_nCarsCanDriveMask;
    uint16     m_nPedFlags;
    CColModel* m_pHitColModel;
    uint8      m_nRadio1;
    uint8      m_nRadio2;
    uint8      m_nRace;
    char       _pad;
    int16      m_nPedAudioType;
    int16      m_nVoiceMin;
    int16      m_nVoiceMax;
    int16      m_nVoiceId;

public:
    static constexpr int32 NUM_PED_NAME_ID_ASSOC = 13;
    static RwObjectNameIdAssocation (&m_pPedIds)[NUM_PED_NAME_ID_ASSOC];
    static constexpr int32 NUM_PED_COL_NODE_INFOS = 12;
    static tPedColNodeInfo(&m_pColNodeInfos)[NUM_PED_COL_NODE_INFOS];

public:
    static void InjectHooks();

// virtual
    ModelInfoType GetModelType() override;
    void DeleteRwObject() override;
    void SetClump(RpClump* clump) override;

// virtual implementations
    ModelInfoType GetModelType_Reversed();
    void DeleteRwObject_Reversed();
    void SetClump_Reversed(RpClump* clump);

// class
    void AddXtraAtomics(RpClump* pClump); //empty
    void SetFaceTexture(RwTexture* pTexture); //empty
    void CreateHitColModelSkinned(RpClump* pClump);
    CColModel* AnimatePedColModelSkinned(RpClump* pClump);
    CColModel* AnimatePedColModelSkinnedWorld(RpClump* pClump);
    void IncrementVoice();
};

VALIDATE_SIZE(CPedModelInfo, 0x44);
