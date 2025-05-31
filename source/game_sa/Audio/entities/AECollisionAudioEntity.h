#pragma once

#include "AEAudioEntity.h"

enum eCollisionSoundStatus : uint8 {
    COLLISION_SOUND_INACTIVE,
    COLLISION_SOUND_ONE_SHOT,
    COLLISION_SOUND_LOOPING,
};

struct tCollisionAudioEntry {
    CEntity*  m_Entity1{nullptr};
    CEntity*  m_Entity2{nullptr};
    CAESound* m_Sound{nullptr};
    uint32    m_nTime{0};
    eCollisionSoundStatus m_nStatus{COLLISION_SOUND_INACTIVE};
    eSurfaceType m_nSurface1{SURFACE_UNKNOWN_194 + 1}; // ?
    eSurfaceType m_nSurface2{SURFACE_UNKNOWN_194 + 1}; // ?

    tCollisionAudioEntry() = default;
};
VALIDATE_SIZE(tCollisionAudioEntry, 0x14);

class NOTSA_EXPORT_VTABLE CAECollisionAudioEntity : public CAEAudioEntity {
public:
    static constexpr auto NUM_ENTRIES = 300u;

    int16                m_aHistory[SURFACE_UNKNOWN_194]{255};
    int16                m_nLastBulletHitSoundID{-1};
    int16                m_nRandom{-1};
    int32                m_nActiveCollisionSounds{0};
    std::array<tCollisionAudioEntry, NUM_ENTRIES> m_Entries{};

public:
    static void InjectHooks();

    void Initialise();
    static void InitialisePostLoading();
    void Reset();

    void AddCollisionSoundToList(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, CAESound* sound, eCollisionSoundStatus status);

    eCollisionSoundStatus GetCollisionSoundStatus(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, int32& outIndex);

    void PlayLoopingCollisionSound(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, float a5, CVector& posn, uint8 a7);
    void UpdateLoopingCollisionSound(CAESound *pSound, CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, float impulseForce, CVector& position, bool bForceLooping);

    void PlayOneShotCollisionSound(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, float a5, CVector& posn);
    void PlayBulletHitCollisionSound(eSurfaceType surface, const CVector& posn, float angleWithColPointNorm);

    void ReportGlassCollisionEvent(eAudioEvents glassSoundType, Const CVector& posn, uint32 time);
    void ReportWaterSplash(CVector posn, float volume);
    void ReportWaterSplash(CPhysical* physical, float height, bool splashMoreThanOnce);
    void ReportObjectDestruction(CEntity* entity);
    void ReportCollision(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, const CVector& pos, const CVector* normal, float collisionImpact1, float collisionImpact2, bool bOnlyPlayOneShotCollisionSound, bool unknown);
    void ReportBulletHit(CEntity* entity, eSurfaceType surface, const CVector& posn, float angleWithColPointNorm);

    void Service();

    bool CanAddNewSound() const { return m_Entries.size() != m_nActiveCollisionSounds; }
};

VALIDATE_SIZE(CAECollisionAudioEntity, 0x1978);
