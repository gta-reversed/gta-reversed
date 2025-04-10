#include "StdInc.h"

#include "Escalator.h"

void CEscalator::InjectHooks() {
    RH_ScopedClass(CEscalator);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x717110);
    RH_ScopedInstall(Destructor, 0x717130);

    RH_ScopedInstall(SwitchOff, 0x717860);
    RH_ScopedInstall(AddThisOne, 0x717970);
    RH_ScopedInstall(Update, 0x717D30);
}

// 0x717860
void CEscalator::SwitchOff() {
    // debug leftover
    // static bool& deletingEscalator = StaticRef<bool>(0xC6E98C);

    if (!m_StepObjectsCreated) {
        return;
    }

    for (auto* object : m_StepObjects | rngv::take(m_NumTopPlanes + m_NumBottomPlanes + m_NumIntermediatePlanes)) {
        if (!object) {
            continue;
        }

        CWorld::Remove(object);
        // deletingEscalator = true;
        delete std::exchange(object, nullptr);
        // deletingEscalator = false;
    }
    m_StepObjectsCreated = 0;
}

// 0x717970
void CEscalator::AddThisOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecTop, const CVector& vecEnd, bool moveDown, CEntity* entity) {
    m_Exist = true;

    const CVector z = { 0.0f, 0.0f, CModelInfo::GetModelInfo(ModelIndices::MI_ESCALATORSTEP)->GetColModel()->GetBoundingBox().m_vecMax.z };
    m_Start  = vecStart - z;
    m_Bottom = vecBottom - z;
    m_Top    = vecTop - z;
    m_End    = vecEnd - z;

    // 0x717A14
    m_NumIntermediatePlanes = 1 + static_cast<int32>(DistanceBetweenPoints(m_Bottom, m_Top) * 2.5f);
    m_NumBottomPlanes       = 1 + static_cast<int32>(DistanceBetweenPoints(m_Bottom, m_Start) / 3.2f);
    m_NumTopPlanes          = 1 + static_cast<int32>(DistanceBetweenPoints(m_End, m_Top) / 3.2f);

    const auto dir = CVector2D{vecStart - vecBottom}.Normalized();
    m_Rotation.GetRight().Set(dir.y, -dir.x, 0.0f);
    m_Rotation.GetForward().Set(dir.x, dir.y, 0.0f);
    m_Rotation.GetUp().Set(0.0f, 0.0f, 1.0f);
    m_Rotation.GetPosition().Reset();
    m_MoveDown = moveDown;
    m_Entity   = entity;

    m_Bounding.m_vecCenter = CVector::Centroid({ m_Start, m_End });
    m_Bounding.m_fRadius   = DistanceBetweenPoints(m_Bounding.m_vecCenter, m_Start);
}

// 0x717D30
void CEscalator::Update() {
    ZoneScoped;

    constexpr float EXTRA_DIST_TO_RENDER = 20.0f; // 0x858BA4

    if (!m_StepObjectsCreated) {
        if (m_Bounding.m_fRadius + EXTRA_DIST_TO_RENDER > DistanceBetweenPoints(m_Bounding.m_vecCenter, TheCamera.GetPosition())) {
            if (GetObjectPool()->GetNoOfFreeSpaces() > m_NumIntermediatePlanes + m_NumBottomPlanes + m_NumTopPlanes + 10) {
                m_StepObjectsCreated  = true;
                m_CurrentPosition = 0.0f;

                for (auto i = 0u; i < m_NumIntermediatePlanes + m_NumBottomPlanes + m_NumTopPlanes; i++) {
                    auto& obj = m_StepObjects[i];
                    obj       = new CObject((i < m_NumIntermediatePlanes) ? ModelIndices::MI_ESCALATORSTEP : ModelIndices::MI_ESCALATORSTEP8, true);
                    obj->SetPosn(m_Bottom);
                    CWorld::Add(obj);
                    obj->m_nObjectType = OBJECT_TYPE_DECORATION;
                }
            } else {
                NOTSA_LOG_WARN("Too many shit in the object pool now we can't create escalators!");
            }
        }
    }

    if (!m_StepObjectsCreated) {
        // still not created after all that effort!
        return; // 0x717F70 - Invert
    }

    const auto posStep = m_MoveDown
        ? m_CurrentPosition - CTimer::GetTimeStep() / 25.0f + 1.0f
        : m_CurrentPosition + CTimer::GetTimeStep() / 25.0f;

    m_CurrentPosition = posStep - std::floor(posStep);

    for (auto i = 0u; i < m_NumIntermediatePlanes + m_NumBottomPlanes + m_NumTopPlanes; i++) {
        auto* obj = m_StepObjects[i];
        if (!obj) {
            continue;
        }

        const auto [t, dir, beg] = [&] {
            if (i < m_NumIntermediatePlanes) {
                // intermediate
                return std::make_tuple(((float)i - m_CurrentPosition + 1.0f) / 2.5f, (m_Bottom - m_Top).Normalized(), m_Top);
            } else if (i < m_NumIntermediatePlanes + m_NumBottomPlanes) {
                // bottom
                return std::make_tuple(((float)(8 * (i - m_NumIntermediatePlanes) + 4) + m_CurrentPosition) / 2.5f, (m_Bottom - m_Start).Normalized(), m_Start);
            } else {
                // top
                return std::make_tuple(((float)(8 * (i - m_NumIntermediatePlanes - m_NumBottomPlanes) + 4) + m_CurrentPosition) / 2.5f, (m_End - m_Top).Normalized(), m_Top);
            }
        }();

        obj->GetMatrix() = m_Rotation;
        obj->SetPosn(beg + dir * t);
        obj->GetMoveSpeed() = (i < m_NumIntermediatePlanes ? -dir : dir) * 0.016f * (m_MoveDown ? -1.0f : 1.0f);
        obj->UpdateRW();
        obj->UpdateRwFrame();
        obj->RemoveAndAdd();
    }

    // Out of sight
    if (m_Bounding.m_fRadius + EXTRA_DIST_TO_RENDER + 3.0f < DistanceBetweenPoints(m_Bounding.m_vecCenter, TheCamera.GetPosition())) {
        SwitchOff();
    }
}
