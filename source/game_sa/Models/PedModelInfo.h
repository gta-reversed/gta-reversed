/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "ClumpModelInfo.h"
#include "ColModel.h"
#include <extensions/utility.hpp>
#include <Audio/Enums/ePedAudioType.h>
#include <Audio/Enums/PedSpeechVoices.h>

struct tPedColNodeInfo {
    char    _pad[4];
    int32   m_nBoneID; // see eBoneTag
    int32   m_nFlags;
    CVector m_vecCenter;
    float   m_fRadius;
};

class NOTSA_EXPORT_VTABLE CPedModelInfo : public CClumpModelInfo {
public:
    AssocGroupId       m_nAnimType;
    ePedType           m_nPedType;
    ePedStats          m_nStatType;
    uint16             m_nCarsCanDriveMask; //< Bitset of vehicle classes ped can drive. To check if it can drive a given class check if the bit is set (eg.: & (1 << eVehicleClass))
    uint16             m_nPedFlags;
    CColModel*         m_pHitColModel;
    eRadioID           m_nRadio1;
    eRadioID           m_nRadio2;
    uint8              m_nRace; // See `ePedRace` - TODO: Maybe we can change this? Check if `ePedRace` can be made 1 byte.
    eAudioPedType      m_nPedAudioType;
    ePedSpeechVoiceS16 m_nVoiceMin; // Also called voice1
    ePedSpeechVoiceS16 m_nVoiceMax; // Also called voice2
    ePedSpeechVoiceS16 m_nVoiceId;  // In `LoadPedObject` this is set to be the same as `m_nVoiceMin` (Which doesn't mean it will always be the same)

    static constexpr int32 NUM_PED_NAME_ID_ASSOC = 13;
    static constexpr int32 NUM_PED_COL_NODE_INFOS = 12;

    static inline RwObjectNameIdAssocation(&m_pPedIds)[NUM_PED_NAME_ID_ASSOC] = *(RwObjectNameIdAssocation(*)[NUM_PED_NAME_ID_ASSOC])0x8A6268;
    static inline tPedColNodeInfo(&m_pColNodeInfos)[NUM_PED_COL_NODE_INFOS] = *(tPedColNodeInfo(*)[NUM_PED_COL_NODE_INFOS])0x8A6308;

public:
    static void InjectHooks();

    CPedModelInfo() : CClumpModelInfo(), m_pHitColModel(nullptr) {} // 0x4C57A0
    ~CPedModelInfo() { delete m_pHitColModel; } // 0x4C62F0

    ModelInfoType GetModelType() override;
    void DeleteRwObject() override;
    void SetClump(RpClump* clump) override;


    void AddXtraAtomics(RpClump* clump);
    void SetFaceTexture(RwTexture* texture);
    void CreateHitColModelSkinned(RpClump* clump);
    CColModel* AnimatePedColModelSkinned(RpClump* clump);
    CColModel* AnimatePedColModelSkinnedWorld(RpClump* clump);
    void IncrementVoice();
    auto GetRace() const { return (ePedRace)m_nRace; }
    auto GetPedType() const { return (ePedType)(m_nPedType); }
    auto GetPedStatType() const { return (ePedStats)(m_nStatType); }
    auto CanPedDriveVehicleClass(eVehicleClass cls) const { return (m_nCarsCanDriveMask & (1 << (size_t)cls)) != 0; }
    bool IsPedTypeFemale() const noexcept { return notsa::contains({PED_TYPE_CIVFEMALE, PED_TYPE_PROSTITUTE}, GetPedType()); }
};

VALIDATE_SIZE(CPedModelInfo, 0x44);
