/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Base.h"

#include "eCamMode.h"
#include "Vector.h"

/* http://code.google.com/p/mtasa-blue/source/browse/tags/1.3.4/MTA10/game_sa/CCamSA.h */

class CEntity;
class CPed;
class CVehicle;

extern bool& gbFirstPersonRunThisFrame;

class CCam {
public:
    bool      m_bBelowMinDist;
    bool      m_bBehindPlayerDesired;
    bool      m_bCamLookingAtVector;
    bool      m_bCollisionChecksOn;
    bool      m_bFixingBeta;
    bool      m_bTheHeightFixerVehicleIsATrain;
    bool      m_bLookBehindCamWasInFront;
    bool      m_bLookingBehind;
    bool      m_bLookingLeft;
    bool      m_bLookingRight;
    bool      m_bResetStatics;
    bool      m_bRotating;
    eCamMode  m_nMode;
    uint32    m_nFinishTime;
    uint32    m_nDoCollisionChecksOnFrameNum;
    uint32    m_nDoCollisionCheckEveryNumOfFrames;
    uint32    m_nFrameNumWereAt;
    uint32    m_nRunningVectorArrayPos;
    uint32    m_nRunningVectorCounter;
    uint32    m_nDirectionWasLooking;
    float     m_fMaxRoleAngle;
    float     m_fRoll;
    float     m_fRollSpeed;
    float     m_fSyphonModeTargetZOffSet;
    float     m_fAmountFractionObscured;
    float     m_fAlphaSpeedOverOneFrame;
    float     m_fBetaSpeedOverOneFrame;
    float     m_fBufferedTargetBeta;
    float     m_fBufferedTargetOrientation;
    float     m_fBufferedTargetOrientationSpeed;
    float     m_fCamBufferedHeight;
    float     m_fCamBufferedHeightSpeed;
    float     m_fCloseInPedHeightOffset;
    float     m_fCloseInPedHeightOffsetSpeed;
    float     m_fCloseInCarHeightOffset;
    float     m_fCloseInCarHeightOffsetSpeed;
    float     m_fDimensionOfHighestNearCar;
    float     m_fDistanceBeforeChanges;
    float     m_fFovSpeedOverOneFrame;
    float     m_fMinDistAwayFromCamWhenInterPolating;
    float     m_fPedBetweenCameraHeightOffset;
    float     m_fPlayerInFrontSyphonAngleOffSet;
    float     m_fRadiusForDead;
    float     m_fRealGroundDist;
    float     m_fTargetBeta;
    float     m_fTimeElapsedFloat;
    float     m_fTilt;
    float     m_fTiltSpeed;
    float     m_fTransitionBeta;
    float     m_fTrueBeta;
    float     m_fTrueAlpha;
    float     m_fInitialPlayerOrientation;
    float     m_fVerticalAngle;  ///< The radian angle ([-pi/2, pi/2]) relative to the entity the camera is locked on (the player usually)
    float     m_fAlphaSpeed;
    float     m_fFOV;
    float     m_fFOVSpeed;
    float     m_fHorizontalAngle;
    float     m_fBetaSpeed;
    float     m_fDistance;
    float     m_fDistanceSpeed;
    float     m_fCaMinDistance;
    float     m_fCaMaxDistance;
    float     m_fSpeedVar;
    float     m_fCameraHeightMultiplier;
    float     m_fTargetZoomGroundOne;
    float     m_fTargetZoomGroundTwo;
    float     m_fTargetZoomGroundThree;
    float     m_fTargetZoomOneZExtra;
    float     m_fTargetZoomTwoZExtra;
    float     m_fTargetZoomTwoInteriorZExtra;
    float     m_fTargetZoomThreeZExtra;
    float     m_fTargetZoomZCloseIn;
    float     m_fMinRealGroundDist;
    float     m_fTargetCloseInDist;
    float     m_fBeta_Targeting;
    float     m_fX_Targetting;
    float     m_fY_Targetting;
    CVehicle* m_pCarWeAreFocussingOn;
    CVehicle* m_pCarWeAreFocussingOnI;
    float     m_fCamBumpedHorz;
    float     m_fCamBumpedVert;
    uint32    m_nCamBumpedTime; // TODO: Probably float
    CVector   m_vecSourceSpeedOverOneFrame;
    CVector   m_vecTargetSpeedOverOneFrame;
    CVector   m_vecUpOverOneFrame;
    CVector   m_vecTargetCoorsForFudgeInter;
    CVector   m_vecCamFixedModeVector;
    CVector   m_vecCamFixedModeSource;
    CVector   m_vecCamFixedModeUpOffSet;
    CVector   m_vecLastAboveWaterCamPosition;
    CVector   m_vecBufferedPlayerBodyOffset;
    CVector   m_vecFront;
    CVector   m_vecSource;
    CVector   m_vecSourceBeforeLookBehind;
    CVector   m_vecUp;
    CVector   m_avecPreviousVectors[2];
    CVector   m_avecTargetHistoryPos[4];
    uint32    m_anTargetHistoryTime[4];
    uint32    m_nCurrentHistoryPoints;
    CEntity*  m_pCamTargetEntity; // Owner entity. e.g.: player
    float     m_fCameraDistance;
    float     m_fIdealAlpha;
    float     m_fPlayerVelocity;
    CVehicle* m_pLastCarEntered;
    CPed*     m_pLastPedLookedAt;
    bool      m_bFirstPersonRunAboutActive;

public:
    static void InjectHooks();

    CCam();

    void Init();

    void CacheLastSettingsDWCineyCam();
    void DoCamBump(float horizontal, float vertical);
    void Finalise_DW_CineyCams(const CVector& src, const CVector& dest, float roll, float fov, float nearClip, float shakeDegree);
    void GetCoreDataForDWCineyCamMode(CEntity*& entity, CVehicle*& vehicle, CVector& dest, CVector& src, CVector& targetUp, CVector& targetRight, CVector& targetFwd, CVector& targetVel, float& targetSpeed, CVector& targetAngVel, float& targetAngSpeed, CColSphere& colSphere);
    void GetLookFromLampPostPos(CEntity* target, CPed* cop, const CVector& vecTarget, const CVector& vecSource);
    void GetVectorsReadyForRW();
    void Get_TwoPlayer_AimVector(CVector&);
    bool IsTimeToExitThisDWCineyCamMode(int32 camId, const CVector& src, const CVector& dst, float t, bool lineOfSightCheck);
    void KeepTrackOfTheSpeed(const CVector&, const CVector&, const CVector&, const float&, const float&, const float&);
    void LookBehind();
    void LookRight(bool bLookRight);
    void RotCamIfInFrontCar(const CVector&, float);
    bool Using3rdPersonMouseCam() const;
    bool GetWeaponFirstPersonOn();
    void ClipAlpha();
    void ClipBeta();

    void Process();
    void ProcessArrestCamOne();
    void ProcessPedsDeadBaby();
    void Process_1rstPersonPedOnPC(const CVector&, float, float, float);
    void Process_1stPerson(const CVector&, float, float, float);
    void Process_AimWeapon(const CVector&, float, float, float);
    void Process_AttachedCam();
    void Process_Cam_TwoPlayer();
    void Process_Cam_TwoPlayer_InCarAndShooting();
    void Process_Cam_TwoPlayer_Separate_Cars();
    void Process_Cam_TwoPlayer_Separate_Cars_TopDown();
    void Process_DW_BirdyCam(bool);
    void Process_DW_CamManCam(bool);
    void Process_DW_HeliChaseCam(bool);
    void Process_DW_PlaneCam1(bool);
    void Process_DW_PlaneCam2(bool);
    void Process_DW_PlaneCam3(bool);
    void Process_DW_PlaneSpotterCam(bool);
    void Process_Editor(const CVector& target, float orientation, float speedVar, float speedVarWanted);
    void Process_Fixed(const CVector& target, float orientation, float speedVar, float speedVarWanted);
    void Process_FlyBy(const CVector& target, float orientation, float speedVar, float speedVarWanted);
    void Process_FollowCar_SA(const CVector& target, float orientation, float speedVar, float speedVarWanted, bool);
    void Process_FollowPedWithMouse(const CVector& target, float orientation, float speedVar, float speedVarWanted);
    void Process_FollowPed_SA(const CVector& target, float orientation, float speedVar, float speedVarWanted, bool);
    void Process_M16_1stPerson(const CVector& target, float orientation, float speedVar, float speedVarWanted);
    void Process_Rocket(const CVector& target, float orientation, float speedVar, float speedVarWanted, bool isHeatSeeking);
    void Process_SpecialFixedForSyphon(const CVector& target, float orientation, float speedVar, float speedVarWanted);
    bool Process_WheelCam(const CVector& target, float orientation, float speedVar, float speedVarWanted);

    // inlined
    void ApplyUnderwaterMotionBlur();

private:
    CCam* Constructor() {
        this->CCam::CCam();
        return this;
    }
};

VALIDATE_SIZE(CCam, 0x238);

int32 ConvertPedNode2BoneTag(int32 simpleId);
bool  bIsLampPost(eModelID modelId);
