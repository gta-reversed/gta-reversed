/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once


#include "TaskSimple.h"
#include "TaskTimer.h"
#include "Vector.h"
#include "AnimBlendAssociation.h"
#include "Entity.h"

class CTaskSimpleInAir : public CTaskSimple {
public:
    CVector                m_vecPosn;
    float                  m_fAngle;
    uint8                  m_nSurfaceType;
    CAnimBlendAssociation* m_pAnim;
    float                  m_fMinZSpeed;
    union {
        uint8 m_nFlags;
        struct
        {
            uint8 m_bUsingJumpGlide : 1;
            uint8 m_bUsingFallGlide : 1;
            uint8 m_bUsingClimbJump : 1;
        };
    };
    uint32     m_nProcessCounter;
    CTaskTimer m_timer;
    CEntity*   m_pClimbEntity;

    static float ms_fSlowFallThreshold;  // 0x8D2EFC
    static uint32 ms_nMaxSlowFallFrames; // 0x8D2EF8

public:
    CTaskSimpleInAir(bool bUsingJumpGlide, bool bUsingFallGlide, bool bUsingClimbJump);
    ~CTaskSimpleInAir() override;

    CTask* Clone() override { return new CTaskSimpleInAir(m_bUsingJumpGlide, m_bUsingFallGlide, m_bUsingClimbJump); }
    eTaskType GetTaskType() override { return TASK_SIMPLE_IN_AIR; }
    bool ProcessPed(CPed* ped) override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;

    bool ProcessPed_Reversed(CPed* ped);
    bool MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event);

    static void DeleteAnimCB(CAnimBlendAssociation* anim, void* data);

    static void InjectHooks();
    CTaskSimpleInAir* Constructor(bool bUsingJumpGlide, bool bUsingFallGlide, bool bUsingClimbJump);
};

VALIDATE_SIZE(CTaskSimpleInAir, 0x3C);
