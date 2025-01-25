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
    CTask*    Clone() const override { return new CTaskSimpleClimb(m_ClimbEntity, m_HandholdPos, m_HandholdHeading, m_SurfaceType, (eClimbHeights)m_HeightForAnim, m_HasToForceClimb); }
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

    auto GetHeightForPos() const { return m_HeightForPos; }
    auto GetIsInvalidClimb() const { return m_IsInvalidClimb; }

private:
    CVector                   GetClimbOffset3D(CVector2D offset, float angle) const;
    std::pair<CVector, float> GetHandholdPosAndAngle() const;

private:
    bool                   m_HasFinished;
    bool                   m_HasToChangeAnimation;
    bool                   m_HasToChangePosition;
    bool                   m_HasToForceClimb;
    bool                   m_IsInvalidClimb;
    eClimbHeights          m_HeightForAnim; // eClimbHeights
    eClimbHeights          m_HeightForPos;  // eClimbHeights
    eSurfaceType           m_SurfaceType;
    int8                   m_FallAfterVault;
    float                  m_HandholdHeading;
    CVector                m_HandholdPos;
    CEntity*               m_ClimbEntity;
    uint16                 m_GetToPosCounter; // we can use u32 without any problems
    CAnimBlendAssociation* m_Anim;

public:
    static void InjectHooks();

private:
    CTaskSimpleClimb* Constructor(CEntity* pClimbEnt, const CVector& vecTarget, float fHeading, uint8 nSurfaceType, eClimbHeights nHeight, bool bForceClimb) {
        this->CTaskSimpleClimb::CTaskSimpleClimb(pClimbEnt, vecTarget, fHeading, nSurfaceType, nHeight, bForceClimb);
        return this;
    }
};
VALIDATE_SIZE(CTaskSimpleClimb, 0x30);
