#include "StdInc.h"

#include "IdleCam.h"
#include "InterestingEvents.h"
#include "HandShaker.h"

CIdleCam& gIdleCam = *(CIdleCam*)0xB6FDA0;
uint32& gbCineyCamProcessedOnFrame = *(uint32*)0xB6EC40;

void CIdleCam::InjectHooks() {
    RH_ScopedClass(CIdleCam);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetLookAtPositionOnTarget, 0x50EAE0);
    RH_ScopedInstall(Init, 0x50E6D0);
    RH_ScopedInstall(Reset, 0x50A160);
    RH_ScopedInstall(ProcessIdleCamTicker, 0x50A200);
    RH_ScopedInstall(SetTarget, 0x50A280);
    RH_ScopedInstall(FinaliseIdleCamera, 0x50E760);
    RH_ScopedInstall(SetTargetPlayer, 0x50EB50);
    RH_ScopedInstall(IsTargetValid, 0x517770);
    RH_ScopedInstall(ProcessTargetSelection, 0x517870);
    RH_ScopedInstall(ProcessSlerp, 0x5179E0);
    RH_ScopedInstall(ProcessFOVZoom, 0x517BF0, { .reversed = false });
    RH_ScopedInstall(Run, 0x51D3E0);
    RH_ScopedInstall(Process, 0x522C80);
    RH_ScopedInstall(IdleCamGeneralProcess, 0x50E690);
}

// 0x517760
CIdleCam::CIdleCam() {
    Init();
}

// 0x50E6D0
void CIdleCam::Init() {
    m_DistStartFOVZoom                         = 15.0f;
    m_ZoomNearest                              = 15.0f;
    m_SlerpDuration                            = 2000.0f;
    m_TimeControlsIdleForIdleToKickIn          = 90000.0f;
    m_TimeToConsiderNonVisibleEntityAsOccluded = 3000.0f;
    m_DistTooClose                             = 4.0f;
    m_DistTooFar                               = 80.0f;
    m_DegreeShakeIdleCam                       = 1.0f;
    m_ShakeBuildUpTime                         = 3000.0f;
    m_ZoomFarthest                             = 70.0f;
    m_DurationFOVZoom                          = 1000.0f;
    m_TargetLOSFramestoReject                  = 14;
    m_TimeBeforeNewZoomIn                      = 12000.0f;
    m_TimeMinimumToLookAtSomething             = 5000.0f;
    m_IncreaseMinimumTimeFactorForZoomedIn     = 2.0f;
    m_LastTimePadTouched                       = 0;
    m_IdleTickerFrames                         = 0;
    Reset(false);
}

// 0x50A160
void CIdleCam::Reset(bool resetControls) {
    m_PositionToSlerpFrom.Reset();
    m_LastIdlePos.Reset();
    m_TimeSlerpStarted               = -1.0f;
    m_TimeIdleCamStarted             = -1.0f;
    m_TimeLastTargetSelected         = -1.0f;
    m_ZoomFrom                       = -1.0f;
    m_ZoomTo                         = -1.0f;
    m_TimeZoomStarted                = -1.0f;
    m_TimeTargetEntityWasLastVisible = -1.0f;
    m_TimeLastZoomIn                 = -1.0f;
    m_Target                         = 0;
    m_ZoomState                      = eIdleCamZoomState::UNK_3;
    m_nForceAZoomOut                 = 0;
    m_CurFOV                         = 70.0f;
    m_TargetLOSCounter               = 0;
    m_bHasZoomedIn                   = 0;
    m_SlerpTime                      = 1.0f;

    if (resetControls) {
        CPad::GetPad()->LastTimeTouched = CTimer::GetTimeInMS();
    }
}

// 0x50A200
void CIdleCam::ProcessIdleCamTicker() {
    if (m_LastTimePadTouched == CPad::GetPad(0)->LastTimeTouched) {
        m_IdleTickerFrames += static_cast<uint32>(CTimer::ms_fTimeStep * 20.0f);
    } else {
        m_LastTimePadTouched = CPad::GetPad(0)->LastTimeTouched;
        m_IdleTickerFrames   = 0;
    }
}

// inlined
bool CIdleCam::IsItTimeForIdleCam() {
    // Always false in Android
    return static_cast<float>(m_IdleTickerFrames) > m_TimeControlsIdleForIdleToKickIn;
}

// wrong name?
// 0x50E690
void CIdleCam::IdleCamGeneralProcess() {
    if (!IsItTimeForIdleCam()) {
        g_InterestingEvents.m_b1 = false;
    }

    if (TheCamera.GetActiveCam().m_nMode != MODE_FOLLOWPED) {
        g_InterestingEvents.m_b1    = false;
        gIdleCam.m_IdleTickerFrames = 0;
    }
}

// 0x50EAE0
void CIdleCam::GetLookAtPositionOnTarget(const CEntity* target, CVector& outPos) {
    outPos = target->GetPosition();
    if (target->IsPed()) {
        switch (target->AsPed()->m_nPedType) {
        case PED_TYPE_CIVFEMALE:
        case PED_TYPE_PROSTITUTE:
            outPos.z += 0.1f;
            break;
        default:
            outPos.z += 0.5f;
            break;
        }
    }
}

// 0x517BF0
void CIdleCam::ProcessFOVZoom(float time) {
    NOTSA_UNREACHABLE();
}

// 0x517770
bool CIdleCam::IsTargetValid(CEntity* target) {
    if (!target) {
        return false;
    }

    if (target == FindPlayerPed()) {
        return true;
    }
    CVector lookAtPos{};
    GetLookAtPositionOnTarget(target, lookAtPos);

    const auto dist = DistanceBetweenPoints(m_Cam->m_vecSource, lookAtPos);
    if (dist < m_DistTooClose || dist > m_DistTooFar) {
        return false;
    }

    if (m_SlerpTime < 1.0f) {
        return true;
    }

    const auto oldIgnore  = CWorld::pIgnoreEntity;
    CWorld::pIgnoreEntity = target;
    notsa::ScopeGuard _([&]{ CWorld::pIgnoreEntity = oldIgnore; });

    if (CWorld::GetIsLineOfSightClear(
        m_Cam->m_vecSource,
        lookAtPos,
        true,
        false,
        false,
        true,
        false,
        false,
        true
    )) {
        return true;
    }

    return m_TargetLOSCounter++ < m_TargetLOSFramestoReject;
}

// 0x50A280
void CIdleCam::SetTarget(CEntity* target) {
    const auto time = static_cast<float>(CTimer::GetTimeInMS());
    if (m_Target) {
        m_PositionToSlerpFrom = m_LastIdlePos;
    } else {
        m_PositionToSlerpFrom = m_Cam->m_vecSource + m_Cam->m_vecFront;
    }

    CEntity::ChangeEntityReference(m_Target, target);

    m_TimeSlerpStarted       = time;
    m_TimeLastTargetSelected = time;
    m_TargetLOSCounter       = 0;
    m_bHasZoomedIn           = false;
}

// 0x50EB50
void CIdleCam::SetTargetPlayer() {
    SetTarget(FindPlayerPed());
    m_nForceAZoomOut = true;
}

// 0x517870
void CIdleCam::ProcessTargetSelection() {
    auto timeDelta = static_cast<float>(CTimer::GetTimeInMS()) - m_TimeLastTargetSelected;
    if (m_ZoomState != eIdleCamZoomState::UNK_3 && m_TargetLOSCounter <= 0) {
        timeDelta /= m_IncreaseMinimumTimeFactorForZoomedIn;
    }

    if (timeDelta > m_TimeMinimumToLookAtSomething) {
        g_InterestingEvents.InvalidateNonVisibleEvents();
        auto* event = g_InterestingEvents.GetInterestingEvent();

        if (event) {
            auto* eventEntity = event->entity;
            if (m_Target != eventEntity) {
                if (IsTargetValid(eventEntity)) {
                    if (m_ZoomState == eIdleCamZoomState::UNK_3) {
                        SetTarget(eventEntity);
                    } else {
                        m_nForceAZoomOut = true;
                    }
                }
            } else if (!IsTargetValid(eventEntity)) {
                g_InterestingEvents.InvalidateEvent(event);
            }
        } else if (!m_Target || !IsTargetValid(m_Target) && m_Target != FindPlayerPed()) {
            if (m_ZoomState == eIdleCamZoomState::UNK_3) {
                SetTargetPlayer();
            } else {
                m_nForceAZoomOut = true;
            }
        }
    }

    if (!m_Target) {
        SetTargetPlayer();
    }

    if (!IsTargetValid(m_Target) && timeDelta > m_TimeMinimumToLookAtSomething) {
        m_nForceAZoomOut = true;
        if (m_ZoomState == eIdleCamZoomState::UNK_3 || m_TargetLOSCounter > 0) {
            SetTargetPlayer();
        }
    }

    if (m_TargetLOSCounter > m_TargetLOSFramestoReject) {
        SetTargetPlayer();
    }
}

// 0x5179E0
float CIdleCam::ProcessSlerp(float& outX, float& outZ) {
    const auto beginTime = CTimer::GetTimeInMS();

    CVector lookAtPos{};
    if (m_TargetLOSCounter >= m_TargetLOSFramestoReject) {
        lookAtPos = m_LastIdlePos;
    } else {
        GetLookAtPositionOnTarget(m_Target, lookAtPos);
    }

    auto [slerpAtan, slerpDistAtan]   = VectorToAnglesRotXRotZ(m_PositionToSlerpFrom - m_Cam->m_vecSource);
    auto [lookAtAtan, lookAtDistAtan] = VectorToAnglesRotXRotZ(lookAtPos - m_Cam->m_vecSource);

    const auto ClampAngle = [](float& angle, float compare) {
        // TODO: simplify
        if (compare <= DegreesToRadians(180.0f)) {
            if (compare < -DegreesToRadians(180.0f)) {
                angle += DegreesToRadians(360.0f);
            }
        } else {
            angle -= DegreesToRadians(360.0f);
        }
    };

    ClampAngle(lookAtDistAtan, lookAtDistAtan - slerpDistAtan);
    ClampAngle(lookAtAtan, lookAtAtan - slerpAtan);

    const auto slerpT = std::min((beginTime - m_TimeLastTargetSelected) / m_SlerpDuration, 1.0f);
    const auto lerpT  = (std::sin(DegreesToRadians(270.0f - 180.0f * slerpT)) + 1.0f) / 2.0f;

    outX = lerp(slerpDistAtan, lookAtDistAtan - slerpDistAtan, lerpT);
    outZ = lerp(slerpAtan, lookAtAtan - slerpAtan, lerpT);
    return slerpT;
}

// 0x50E760
void CIdleCam::FinaliseIdleCamera(float curAngleX, float curAngleY, float shakeDegree) {
    auto &vecFwd = m_Cam->m_vecFront, vecUp = m_Cam->m_vecUp;

    vecFwd = CVector{
        -(std::cos(curAngleY) * std::cos(curAngleX)),
        -(std::sin(curAngleY) * std::cos(curAngleX)),
        std::sin(curAngleX)
    }.Normalized();
    m_LastIdlePos = vecFwd + m_Cam->m_vecSource;

    auto& hs = gHandShaker[0];
    hs.Process(shakeDegree);
    const auto angle = hs.m_ang.z * m_DegreeShakeIdleCam * shakeDegree;
    vecFwd = hs.m_resultMat.TransformPoint(vecFwd);

    vecUp.Set(std::sin(angle), 0.0f, std::cos(angle));
    auto rightDir = CrossProduct(vecFwd, vecUp).Normalized();
    vecUp         = CrossProduct(rightDir, vecFwd);
    if (vecFwd.x == 0 && vecFwd.y == 0.0f) {
        vecFwd.x = vecFwd.y = 0.0001f;
    }
    rightDir = CrossProduct(vecFwd, vecUp).Normalized();
    vecUp    = CrossProduct(rightDir, vecFwd);
    m_Cam->GetVectorsReadyForRW();
}

// 0x51D3E0
void CIdleCam::Run() {
    const auto beginTime = CTimer::GetTimeInMS();
    ProcessTargetSelection();

    float angleX{}, angleZ{};
    m_SlerpTime = ProcessSlerp(angleX, angleZ);
    ProcessFOVZoom(m_SlerpTime);

    const auto delta = beginTime - m_TimeIdleCamStarted;
    FinaliseIdleCamera(angleX, angleZ, delta < m_ShakeBuildUpTime ? delta / m_ShakeBuildUpTime : 1.f);
}

// 0x522C80
bool CIdleCam::Process() {
    ProcessIdleCamTicker();
    if (!IsItTimeForIdleCam()) {
        return false;
    }

    m_Cam = &TheCamera.GetActiveCam();
    if (m_LastFrameProcessed < CTimer::GetFrameCounter() - 1) {
        g_InterestingEvents.m_b1 = true;
        Reset(false);
        m_TimeIdleCamStarted = static_cast<float>(CTimer::GetTimeInMS());
        SetTarget(FindPlayerPed());
        m_nForceAZoomOut = true;
    }
    m_LastFrameProcessed = CTimer::GetFrameCounter();
    Run();
    gbCineyCamProcessedOnFrame = m_LastFrameProcessed;
    return true;
}
