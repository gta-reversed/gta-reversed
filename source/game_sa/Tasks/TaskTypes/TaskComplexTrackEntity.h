#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CEntity;

class NOTSA_EXPORT_VTABLE CTaskComplexTrackEntity : public CTaskComplex {
public:
    CEntity* m_toTrack{};          // 0xC
    CVector m_offsetPosn{};              // 0x10
    uint8 a{};                     // 0x1C
    int32 b{};                     // 0x20
    float m_rangeMin{};            // 0x24
    float m_rangeMax{};            // 0x28
    uint8 f{};                     // 0x2C
    uint32 m_someStartTimeMs{};    // 0x30
    uint32 m_expirationInterval{}; // 0x34
    bool gap2{};                   // 0x38
    bool gap3{};                   // 0x39
    float m_fMoveRatio{};          // 0x3C
    float float40{};               // 0x40
    CVector m_goToPos{};                 // 0x44
    float m_distToTargetSq{};      // 0x50

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_COMPLEX_TRACK_ENTITY;

    CTaskComplexTrackEntity(CEntity* entity, CVector posn, uint8 a6, int32 a7, float rangeMin, float rangeMax, uint8 a10);
    CTaskComplexTrackEntity(const CTaskComplexTrackEntity&);
    ~CTaskComplexTrackEntity();

    void SetOffsetPos(CVector posn);
    void CalcTargetPos(CPed* ped);
    void CalcMoveRatio(CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexTrackEntity{*this}; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x65F3B0
    CTaskComplexTrackEntity* Constructor(CEntity* entity, CVector posn, uint8 a6, int32 a7, float rangeMin, float rangeMax, uint8 a10) { this->CTaskComplexTrackEntity::CTaskComplexTrackEntity(entity,  posn,  a6,  a7,  rangeMin,  rangeMax,  a10); return this; }

    // 0x65F460
    CTaskComplexTrackEntity* Destructor() { this->CTaskComplexTrackEntity::~CTaskComplexTrackEntity(); return this; }
};
VALIDATE_SIZE(CTaskComplexTrackEntity, 0x54);
