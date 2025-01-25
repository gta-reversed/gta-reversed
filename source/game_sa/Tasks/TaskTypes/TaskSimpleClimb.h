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

class NOTSA_EXPORT_VTABLE CTaskSimpleClimb final : public CTaskSimple {
public:
    constexpr static float ms_fMinForStretchGrab = +1.40f; // 0x8D2F34

public:
    static constexpr auto Type = TASK_SIMPLE_CLIMB;

    CTaskSimpleClimb(CEntity* climbEntity, const CVector& vecTarget, float fHeading, uint8 nSurfaceType, eClimbHeights nHeight, bool bForceClimb);
    ~CTaskSimpleClimb() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskSimpleClimb(m_pClimbEnt, m_vecHandholdPos, m_fHandholdHeading, m_nSurfaceType, (eClimbHeights)m_nHeightForAnim, m_bForceClimb); }
    bool      ProcessPed(CPed* ped) override;
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;

    // 0x6803A0
    static CEntity* TestForClimb(
        CPed*    ped,             //!< [in]  The ped that will preform the climbing
        CVector& outClimbPos,     //!< [out] Climbing position
        float&   outClimbHeading, //!< [out] Climbing heading
        uint8&   outSurfaceType,  //!< [out] Surface type
        bool     bLaunch          //!< [in]  Not sure
    );
    static CEntity* ScanToGrabSectorList(CPtrList* sectorList, CPed* ped, CVector& climbPos, float& angle, uint8& pSurfaceType, bool flag1, bool bStandUp, bool bVault);
    static CEntity* ScanToGrab(CPed* ped, CVector& climbPos, float& angle, uint8& pSurfaceType, bool flag1, bool bStandUp, bool bVault, CVector* pedPosition);
    static bool CreateColModel();
    static void Shutdown();
    bool TestForStandUp(CPed* ped, const CVector& point, float fAngle);
    bool TestForVault(CPed* ped, const CVector& point, float fAngle);
    void StartAnim(CPed* ped);
    void StartSpeech(CPed* ped) const;
    static void DeleteAnimCB(CAnimBlendAssociation* anim, void* data);
    void GetCameraStickModifier(CEntity* entity, float& fVerticalAngle, float& fHorizontalAngle, float& a5, float& a6);
    void GetCameraTargetPos(CPed* ped, CVector& vecTarget);

    auto GetHeightForPos() const { return m_nHeightForPos; }
    auto GetIsInvalidClimb() const { return m_bInvalidClimb; }

private:
    CVector                   GetClimbOffset3D(CVector2D offset, float angle) const;
    std::pair<CVector, float> GetHandholdPosAndAngle() const;

private:
    bool                   m_bIsFinished;
    bool                   m_bChangeAnimation;
    bool                   m_bChangePosition;
    bool                   m_bForceClimb;
    bool                   m_bInvalidClimb;
    eClimbHeights          m_nHeightForAnim; // eClimbHeights
    eClimbHeights          m_nHeightForPos;  // eClimbHeights
    uint8                  m_nSurfaceType;
    int8                   m_nFallAfterVault;
    float                  m_fHandholdHeading;
    CVector                m_vecHandholdPos;
    CEntity*               m_pClimbEnt;
    uint16                 m_nGetToPosCounter; // we can use u32 without any problems
    CAnimBlendAssociation* m_pAnim;

public:
    static void InjectHooks();

private:
    CTaskSimpleClimb* Constructor(CEntity* pClimbEnt, const CVector& vecTarget, float fHeading, uint8 nSurfaceType, eClimbHeights nHeight, bool bForceClimb) {
        this->CTaskSimpleClimb::CTaskSimpleClimb(pClimbEnt, vecTarget, fHeading, nSurfaceType, nHeight, bForceClimb);
        return this;
    }
};
VALIDATE_SIZE(CTaskSimpleClimb, 0x30);
