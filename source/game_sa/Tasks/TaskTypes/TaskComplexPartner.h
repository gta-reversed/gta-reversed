#pragma once

#include "TaskComplex.h"

class CTaskComplexSequence;

enum ePartnerState : int8 {
    PARTNER_STATE_UNK_1 = 1
};

class NOTSA_EXPORT_VTABLE CTaskComplexPartner : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_PARTNER;

    CTaskComplexPartner(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, bool makePedAlwaysFacePartner, int8 updateDirectionCount, CVector point);
    ~CTaskComplexPartner() override;

    void  CalcTargetPositions(CPed* ped, CVector* point, CVector* targetPoint);
    int32 GetPartnerState();

    eTaskType    GetTaskType() const override { return Type; } // TODO/BUG: Supposed to return the actual member variable `m_taskId`?
    CTask*       CreateNextSubTask(CPed* ped) override;
    CTask*       CreateFirstSubTask(CPed* ped) override;
    CTask*       ControlSubTask(CPed* ped) override;
    virtual void StreamRequiredAnims();
    virtual void RemoveStreamedAnims();

public:
    static void InjectHooks();

public:
    int32         field_C{};
    int32         field_10{};
    char          m_commandName[32]{};
    int32         m_taskId{};
    CPed::Ref     m_partner{};
    float         m_distanceMultiplier{};
    CVector       m_point{};
    CVector       m_targetPoint{};
    bool          m_leadSpeaker{};
    ePartnerState m_partnerState{ PARTNER_STATE_UNK_1 };
    int8          m_firstToTargetFlag{ -1 };
    int8          m_updateDirectionCount{};
    bool          m_taskCompleted{};
    bool          m_makePedAlwaysFacePartner{};
    char          m_animBlockName[16]{};
    bool          m_requiredAnimsStreamedIn{};
};

VALIDATE_SIZE(CTaskComplexPartner, 0x70);
