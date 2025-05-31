#pragma once

#include "AEAudioEntity.h"

enum eCollisionSoundStatus : uint8 {
    COLLISION_SOUND_INACTIVE,
    COLLISION_SOUND_ONE_SHOT,
    COLLISION_SOUND_LOOPING,
};

struct tCollisionAudioEntry {
    CEntity*              EntityA{ nullptr };
    CEntity*              EntityB{ nullptr };
    CAESound*             Sound{ nullptr };
    uint32                Time{ 0 };
    eCollisionSoundStatus Status{ COLLISION_SOUND_INACTIVE };
    eSurfaceType          SurfaceA{ SURFACE_UNKNOWN_194 + 1 }; // ?
    eSurfaceType          SurfaceB{ SURFACE_UNKNOWN_194 + 1 }; // ?
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

    void PlayLoopingCollisionSound(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, float a5, const CVector& posn, bool isForceLooping);
    void UpdateLoopingCollisionSound(
        CAESound*      pSound,
        CEntity*       entityA,
        CEntity*       entityB,
        eSurfaceType   surfA,
        eSurfaceType   surfB,
        float          impulseForce,
        const CVector& position,
        bool           isForceLooping
    );

    std::pair<float, float> GetLoopingCollisionSoundVolumeAndSpeed(CEntity* entityA, CEntity* entityB, eSurfaceType surfA, eSurfaceType surfB, bool isForceLooping); // notsa
    void PlayOneShotCollisionSound(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, float a5, const CVector& posn);
    void PlayBulletHitCollisionSound(eSurfaceType surface, const CVector& posn, float angleWithColPointNorm);

    void ReportGlassCollisionEvent(eAudioEvents glassSoundType, const CVector& posn, uint32 time);
    void ReportWaterSplash(CVector posn, float volume);
    void ReportWaterSplash(CPhysical* physical, float height, bool splashMoreThanOnce);
    void ReportObjectDestruction(CEntity* entity);
    void ReportCollision(
        CEntity*       entityA,
        CEntity*       entityB,
        eSurfaceType   surfA,
        eSurfaceType   surfB,
        const CVector& pos,
        const CVector* normal,
        float          impulseForce,
        float          relVelSq,
        bool           isForceOneShot,
        bool           isForceLooping
    );
    void ReportBulletHit(CEntity* entity, eSurfaceType surface, const CVector& posn, float angleWithColPointNorm);

    void Service();

    bool CanAddNewSound() const { return m_Entries.size() != m_nActiveCollisionSounds; }

protected:
    eSoundID ChooseCollisionSoundID(eSurfaceType surface);
};

VALIDATE_SIZE(CAECollisionAudioEntity, 0x1978);
