/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once


#include "TaskSimple.h"
#include "AnimBlendAssociation.h"
#include "Entity.h"

enum eClimbHeights : int8 {
    CLIMB_NOT_READY = 0,
    CLIMB_GRAB,
    CLIMB_PULLUP,
    CLIMB_STANDUP,
    CLIMB_FINISHED,
    CLIMB_VAULT,
    CLIMB_FINISHED_V
};

class NOTSA_EXPORT_VTABLE CTaskSimpleClimb : public CTaskSimple {
public:
    bool                   m_bIsFinished;
    bool                   m_bChangeAnimation;
    bool                   m_bChangePosition;
    bool                   m_bForceClimb;
    bool                   m_bInvalidClimb;
    int8                   m_nHeightForAnim; // eClimbHeights
    int8                   m_nHeightForPos;  // eClimbHeights
    uint8                  m_nSurfaceType;
    int8                   m_nFallAfterVault;
    float                  m_fHandholdHeading;
    CVector                m_vecHandholdPos;
    CEntity*               m_pClimbEnt;
    uint16                 m_nGetToPosCounter; // we can use u32 without any problems
    CAnimBlendAssociation* m_pAnim;

    static CMatrix& tempMatrix;
    static bool& tempMatrix_Initialized;
    static CColModel& ms_ClimbColModel;
    static CColModel& ms_StandUpColModel;
    static CColModel& ms_VaultColModel;
    static CColModel& ms_FindEdgeColModel;

    static float ms_fHangingOffsetHorz;
    static float ms_fHangingOffsetVert;

    static float ms_fAtEdgeOffsetHorz;
    static float ms_fAtEdgeOffsetVert;

    static float ms_fStandUpOffsetHorz;
    static float ms_fStandUpOffsetVert;

    static float ms_fVaultOffsetHorz;
    static float ms_fVaultOffsetVert;

    static float ms_fMinForStretchGrab;

public:
    static constexpr auto Type = TASK_SIMPLE_CLIMB;

    CTaskSimpleClimb(CEntity* climbEntity, const CVector& vecTarget, float fHeading, uint8 nSurfaceType, eClimbHeights nHeight, bool bForceClimb);
    ~CTaskSimpleClimb() override;

    CTask* Clone() const override { return new CTaskSimpleClimb(m_pClimbEnt, m_vecHandholdPos, m_fHandholdHeading, m_nSurfaceType, (eClimbHeights)m_nHeightForAnim, m_bForceClimb); }
    bool ProcessPed(CPed* ped) override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    eTaskType GetTaskType() const override { return Type; }

    // 0x6803A0
    static CEntity* TestForClimb(
        CPed*    ped,             //!< [in]  The ped that will preform the climbing
        CVector& outClimbPos,     //!< [out] Climbing position
        float&   outClimbHeading, //!< [out] Climbing heading
        uint8&   outSurfaceType,  //!< [out] Surface type
        bool     bLaunch          //!< [in]  Not sure
    );
    static void* ScanToGrabSectorList(CPtrList* sectorList, CPed* ped, CVector& climbPos, float& angle, uint8& pSurfaceType, bool flag1, bool bStandUp, bool bVault);
    static CEntity* ScanToGrab(CPed* ped, CVector& climbPos, float& angle, uint8& pSurfaceType, bool flag1, bool bStandUp, bool bVault, CVector* pedPosition);
    static bool CreateColModel();
    static void Shutdown();
    bool TestForStandUp(CPed* ped, CVector* point, float fAngle);
    bool TestForVault(CPed* ped, CVector* point, float fAngle);
    void StartAnim(CPed* ped);
    void StartSpeech(CPed* ped);
    static void DeleteAnimCB(CAnimBlendAssociation* anim, void* data);
    void GetCameraStickModifier(CEntity* entity, float& fVerticalAngle, float& fHorizontalAngle, float& a5, float& a6);
    void GetCameraTargetPos(CPed* ped, CVector& vecTarget);

    static void InjectHooks();
    CTaskSimpleClimb* Constructor(CEntity* pClimbEnt, const CVector& vecTarget, float fHeading, uint8 nSurfaceType, eClimbHeights nHeight, bool bForceClimb);
};
VALIDATE_SIZE(CTaskSimpleClimb, 0x30);
