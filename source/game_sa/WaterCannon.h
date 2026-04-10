#pragma once

#include "AEWaterCannonAudioEntity.h"

class CVector;

class CWaterCannon {
public:
    static constexpr auto SECTIONS_COUNT = 32;

    uint32                    m_nId;
    int32                     m_nSectionsCount;
    uint32                    m_nCreationTime;
    CVector                   m_sectionPoint[SECTIONS_COUNT];
    CVector                   m_sectionMoveSpeed[SECTIONS_COUNT];
    bool                      m_abUsed[SECTIONS_COUNT];
    CAEWaterCannonAudioEntity m_Audio;

    static inline auto& m_auRenderIndices = StaticRef<std::array<RxVertexIndex, 18>>(0xC80700);

public:
    static void InjectHooks();

    CWaterCannon() = default; // 0x728B10
    ~CWaterCannon() = default; // 0x728B30

    void Init();
    void Update_OncePerFrame(int16);
    void Update_NewInput(const CVector* start, const CVector* end);
    void PushPeds();
    void Render();

private:
    // NOTSA
    [[nodiscard]] bool IsSectionActive(size_t idx) const;
    CVector& GetSectionPosn(size_t idx);
    CVector& GetSectionMoveSpeed(size_t idx);
};

VALIDATE_SIZE(CWaterCannon, 0x3CC);
