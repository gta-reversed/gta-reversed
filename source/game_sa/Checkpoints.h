#pragma once

#include "Checkpoint.h"

class CCheckpoint;

constexpr auto MAX_NUM_CHECKPOINTS{ 32u };

class CCheckpoints {
public:
    static inline uint32& NumActiveCPts = *(uint32*)0xC7C6D4; // not used, only initialised (0)
    static inline std::array<CCheckpoint, 32>& m_aCheckPtArray = *(std::array<CCheckpoint, 32>*)0xC7F158;

public:
    static void InjectHooks();

    static void Init();
    static void Shutdown();
    static void SetHeading(uint32 id, float angle);
    static void Update();
    static CCheckpoint* PlaceMarker(uint32 id, eCheckpointTypeU16 type, const CVector& posn, const CVector& direction, float size, CRGBA color, uint16 pulsePeriod, float pulseFraction, int16 rotateRate);
    /*!
     * @brief Set the position of a checkpoint with the given `id`
     * @param id ID of the checkpoint
     * @param posn The new position
    */
    static void UpdatePos(uint32 id, const CVector& posn);
    static void DeleteCP(uint32 id, uint16 type);
    static void Render();

    static inline CCheckpoint* FindById(uint32 id);
};
