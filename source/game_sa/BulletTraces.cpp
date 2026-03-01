#include "StdInc.h"
#include "BulletTraces.h"

CBulletTrace (&CBulletTraces::aTraces)[16] = *(CBulletTrace(*)[16])0xC7C748;

void CBulletTraces::InjectHooks()
{
    RH_ScopedClass(CBulletTraces);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x721D50);
    RH_ScopedOverloadedInstall(AddTrace, "", 0x723750, void(*)(const CVector&, const CVector&, float, uint32, uint8));
    RH_ScopedOverloadedInstall(AddTrace, "Wrapper", 0x726AF0, void(*)(const CVector&, const CVector&, eWeaponType, CEntity*));
    RH_ScopedInstall(Render, 0x723C10);
    RH_ScopedInstall(Update, 0x723FB0);
}

// 0x721D50
void CBulletTraces::Init()
{
    for (CBulletTrace& trace : aTraces) {
        trace.m_bExists = false;
    }
}

// 0x723FB0
void CBulletTraces::Update()
{
    for (CBulletTrace& trace : aTraces) {
        trace.Update();
    }
}

CBulletTrace* CBulletTraces::GetFree() {
    for (CBulletTrace& trace : aTraces) {
        if (!trace.m_bExists) {
            return &trace;
        }
    }
    return nullptr;
}

void PlayFrontEndSoundForTrace(CVector fromWorldSpace, CVector toWorldSpace) {
    CMatrix camMat = TheCamera.GetMatrix();
    const CVector& camPos = camMat.GetPosition();

    // Make their position relative to the camera's
    const auto fromRelToCam = fromWorldSpace - camPos;
    const auto toRelToCam = toWorldSpace - camPos;

    // Transform both points into the camera's space ((C)cam (S)pace - CS)
    const float fromCSY = DotProduct(fromRelToCam, camMat.GetForward());

    const float toCSY = DotProduct(toRelToCam, camMat.GetForward());

    if (std::signbit(toCSY) == std::signbit(fromCSY)) { // Originally: toCSY * fromCSY < 0.0f - Check if signs differ
        return; // Both points are either in front or behind us
    }

    // They do, in this case points are on opposite sides (one behind, one in front of the camera)

    // Now calculate the remaining coordinates
    const float fromCSX = DotProduct(fromRelToCam, camMat.GetRight());
    const float fromCSZ = DotProduct(fromRelToCam, camMat.GetUp());

    const float toCSX = DotProduct(toRelToCam, camMat.GetRight());
    const float toCSZ = DotProduct(toRelToCam, camMat.GetUp());

    // Calculate distance to point on line that is on the same Y axis as the camera
    // (This point on line is basically the bullet when passing by the camera)
        
    // Interpolation on line
    const float t = fabs(fromCSY) / (fabs(fromCSY) + fabs(toCSY));

    const float pointOnLineZ = fromCSZ + (toCSZ - fromCSZ) * t;
    const float pointOnLineX = fromCSX + (toCSX - fromCSX) * t;

    // Calculate distance from camera to point on line
    const float camToLineDist = std::hypotf(pointOnLineZ, pointOnLineX);

    if (camToLineDist >= 2.0f) {
        return; // Point too far from camera
    }

    const auto ReportBulletAudio = [&](auto event) {
        const float volDistFactor = 1.0f - camToLineDist * 0.5f;
        const float volumeChange  = volDistFactor == 0.0f ? -100.0f : std::log10(volDistFactor);
        AudioEngine.ReportFrontendAudioEvent(event, volumeChange, 1.0f);
    };

    const bool isComingFromBehind = fromCSY <= 0.0f; // Is the bullet coming from behind us?
    if (0.f <= pointOnLineX) { // Is bullet passing on the right of the camera?
        ReportBulletAudio(isComingFromBehind ? AE_FRONTEND_BULLET_PASS_RIGHT_REAR : AE_FRONTEND_BULLET_PASS_RIGHT_FRONT);
    } else { // Bullet passing on left of the camera.
        ReportBulletAudio(isComingFromBehind ? AE_FRONTEND_BULLET_PASS_LEFT_REAR : AE_FRONTEND_BULLET_PASS_LEFT_FRONT);
    }
}

// 0x723750
void CBulletTraces::AddTrace(const CVector& from, const CVector& to, float radius, uint32 disappearTime, uint8 alpha)
{
    if (CBulletTrace* pTrace = GetFree()) {
        pTrace->m_vecStart = from;
        pTrace->m_vecEnd = to;
        pTrace->m_nCreationTime = CTimer::GetTimeInMS();
        pTrace->m_nTransparency = alpha;
        pTrace->m_bExists = true;
        pTrace->m_fRadius = radius;

        // Determinate lifetime based on index in aTraces array
        // This way we can kinda make sure all traces will be visible (even if for only a brief amount of time)
        const auto traceIdx = GetTraceIndex(pTrace);
        if (traceIdx < 10) {
            pTrace->m_nLifeTime = (uint32)(traceIdx < 5 ? disappearTime : disappearTime / 2.0f);
        } else {
            pTrace->m_nLifeTime = (uint32)(disappearTime / 4.0f);
        }
    }
    PlayFrontEndSoundForTrace(from, to);
}

// 0x726AF0
void CBulletTraces::AddTrace(const CVector& posMuzzle, const CVector& posBulletHit, eWeaponType weaponType, CEntity* fromEntity)
{
    if (FindPlayerPed() == fromEntity || FindPlayerVehicle() && FindPlayerVehicle() == fromEntity) {
        switch (CCamera::GetActiveCamera().m_nMode) {
        case MODE_M16_1STPERSON:
        case MODE_SNIPER:
        case MODE_CAMERA:
        case MODE_ROCKETLAUNCHER:
        case MODE_ROCKETLAUNCHER_HS:
        case MODE_M16_1STPERSON_RUNABOUT:
        case MODE_SNIPER_RUNABOUT:
        case MODE_ROCKETLAUNCHER_RUNABOUT:
        case MODE_ROCKETLAUNCHER_RUNABOUT_HS:
        case MODE_HELICANNON_1STPERSON: {
            if (FindPlayerEntity()->AsPhysical()->m_vecMoveSpeed.Magnitude() < 0.05f) {
                return;
            }
        }
        }
    }

    CVector dir = posBulletHit - posMuzzle;
    const float traceLengthOriginal = dir.Magnitude();
    dir.Normalise();

    const float traceLengthNew = CGeneral::GetRandomNumberInRange(0.0f, traceLengthOriginal);
    const float fRadius = std::min(CGeneral::GetRandomNumberInRange(2.0f, 5.0f), traceLengthOriginal - traceLengthNew);

    CVector from = posMuzzle + dir * traceLengthNew;
    CVector to = from + dir * fRadius;

    AddTrace(
        from,
        to,
        0.01f,
        300,
        70
    );
}

// 0x723C10
void CBulletTraces::Render() {
    ZoneScoped;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(RwBlendFunction::rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(RwBlendFunction::rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(RwCullMode::rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));

    RxObjSpace3DVertex verts[6];
    for (CBulletTrace& trace : aTraces) {
        if (!trace.m_bExists) {
            continue;
        }

        // Time progress 1 => 0 as time passes
        const float t = 1.0f - (float)trace.GetRemainingLifetime() / (float)trace.m_nLifeTime;

        // Set vertex positions
        {
            // Direction from camera to trace's origin (aka `start`)
            const auto camToOriginDir = (trace.m_vecStart - TheCamera.GetPosition()).Normalized();

            // Upward direction relative to the camera's position
            const auto up = camToOriginDir.Cross(trace.GetDirection().Normalized()).Normalized();

            // Size vector
            const auto sizeVec = up * (trace.m_fRadius * t);

            // Current position on the trace's line segment (Inverted because `t` is going 1 => 0 not 0 => 1)
            const CVector currPosOnTrace = trace.m_vecEnd - trace.GetDirection() * t;

            // Finally, calculate the position for each vertex
            const CVector vertPositions[std::size(verts)] = {
                currPosOnTrace,
                currPosOnTrace + sizeVec,
                currPosOnTrace - sizeVec,

                trace.m_vecEnd,
                trace.m_vecEnd + sizeVec,
                trace.m_vecEnd - sizeVec,
            };

            for (const auto& [idx, pos] : rngv::enumerate(vertPositions)) {
                RwV3dAssign(RwIm3DVertexGetPos(&verts[idx]), &pos);
            }
        }

        // Set colors
        for (auto& vert : verts) {
            RwIm3DVertexSetRGBA(&vert, 255, 255, 128, 0);
        }
        RwIm3DVertexSetRGBA(&verts[3], 255, 255, 128, (char)((float)trace.m_nTransparency * t)); // Only vertex 3 has non-zero alpha

        if (RwIm3DTransform(verts, std::size(verts), nullptr, rwIM3D_VERTEXRGBA)) {
            RwImVertexIndex indices[] = {
                // Each row represents a triangle
                4, 1, 3,
                1, 0, 3,
                0, 2, 3,
                3, 2, 5
            };
            RwIm3DRenderIndexedPrimitive(RwPrimitiveType::rwPRIMTYPETRILIST, indices, std::size(indices));
            RwIm3DEnd();
        }
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,     RWRSTATE(RwBlendFunction::rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,    RWRSTATE(RwBlendFunction::rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATECULLMODE,     RWRSTATE(RwCullMode::rwCULLMODECULLBACK));
}
