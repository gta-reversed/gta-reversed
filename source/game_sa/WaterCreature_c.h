#pragma once

#include "ListItem_c.h"

class CObject;

#ifdef GetObject
#undef GetObject
#endif

enum eWaterCreatureType : uint8 {
    FISH1,
    FISH2,
    FISH3,
    JELLYFISH1,
    JELLYFISH2,
    TURTLE,
    DOLPHIN
};

class WaterCreature_c : public ListItem_c<WaterCreature_c> {
public:
    WaterCreature_c() : ListItem_c(), m_pObject(nullptr) {}
    ~WaterCreature_c() = default;
public:
    CObject*         m_pObject;
    uint8            m_nCreatureType; // see eWaterCreatureType
    uint8            m_bShouldBeDeleted;
    uint8            m_ucTargetSwimSpeed;
    char             _pad;
    int16            m_wSpeedChangeCurTime;
    int16            m_wSpeedChangeTotalTime;
    float            m_fDefaultSpeed;
    float            m_fCurSpeed;
    WaterCreature_c* m_pFollowedCreature;
    CVector          m_vecOffsetFromFollowed;
    float            m_fHeading;
    bool             m_bChangedDir;

public:
    static void InjectHooks();

    bool Init(int32 nType, CVector* vecPos, WaterCreature_c* parent, float fWaterLevel, float fWaterDepth);
    void Exit();
    void Update(float fTimeStep);

public:
    [[nodiscard]] CObject* GetObject() const { return m_pObject; }
    [[nodiscard]] bool IsJellyfish() const { return m_nCreatureType == JELLYFISH1 || m_nCreatureType == JELLYFISH2; }
    [[nodiscard]] bool IsSmallFish() const {
        return m_nCreatureType == FISH1
            || m_nCreatureType == FISH2
            || m_nCreatureType == FISH3;
    }
};
VALIDATE_SIZE(WaterCreature_c, 0x34);
