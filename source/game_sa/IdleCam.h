#pragma once

#include "Vector.h"

class CEntity;

enum class eIdleCamZoomState {
    UNK_2 = 2,
    UNK_3 = 3,
};

class CIdleCam {
public:
    static void InjectHooks();

    CIdleCam();

    void  Init();
    void  Reset(bool resetControls);
    void  ProcessIdleCamTicker();
    bool  IsItTimeForIdleCam();
    void  IdleCamGeneralProcess();
    void  GetLookAtPositionOnTarget(const CEntity* target, CVector& outPos);
    void  ProcessFOVZoom(float time);
    bool  IsTargetValid(CEntity* target);
    void  SetTarget(CEntity* target);
    void  SetTargetPlayer();
    void  ProcessTargetSelection();
    float ProcessSlerp(float& outX, float& outZ);
    void  FinaliseIdleCamera(float curAngleX, float curAngleY, float shakeDegree);
    void  Run();
    bool  Process();

    // Inlined: ProcessSlerp, return type is NOTSA
    auto VectorToAnglesRotXRotZ(const CVector& pos) {
        return std::make_pair(
            CGeneral::GetATanOfXY(pos.x, pos.y) + DegreesToRadians(180.0f),
            CGeneral::GetATanOfXY(pos.Magnitude2D(), pos.z)
        );
    }

public:
    CEntity*          m_Target;
    CVector           m_PositionToSlerpFrom;
    float             m_TimeSlerpStarted;
    float             m_SlerpDuration;
    CVector           m_LastIdlePos;
    float             m_SlerpTime;
    float             m_TimeControlsIdleForIdleToKickIn;
    float             m_TimeIdleCamStarted;
    uint32            m_LastFrameProcessed;
    float             m_TimeLastTargetSelected;
    float             m_TimeMinimumToLookAtSomething;
    float             m_TimeTargetEntityWasLastVisible;
    float             m_TimeToConsiderNonVisibleEntityAsOccluded;
    float             m_DistTooClose;
    float             m_DistStartFOVZoom;
    float             m_DistTooFar;
    int32             m_TargetLOSFramestoReject;
    int32             m_TargetLOSCounter;
    eIdleCamZoomState m_ZoomState;
    float             m_ZoomFrom;
    float             m_ZoomTo;
    float             m_TimeZoomStarted;
    float             m_ZoomNearest;
    float             m_ZoomFarthest;
    float             m_CurFOV;
    float             m_DurationFOVZoom;
    bool              m_nForceAZoomOut;
    bool              m_bHasZoomedIn;
    uint16            _pad;
    float             m_TimeBeforeNewZoomIn;
    float             m_TimeLastZoomIn;
    float             m_IncreaseMinimumTimeFactorForZoomedIn;
    float             m_DegreeShakeIdleCam;
    float             m_ShakeBuildUpTime;
    int32             m_LastTimePadTouched;
    int32             m_IdleTickerFrames;
    CCam*             m_Cam;
};

VALIDATE_SIZE(CIdleCam, 0x9C);

extern CIdleCam& gIdleCam;
extern uint32& gbCineyCamProcessedOnFrame;
