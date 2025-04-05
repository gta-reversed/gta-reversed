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

    if (!m_nStepObjectsCreated) {
        return;
    }

    for (auto* object : m_pStepObjects | rngv::take(m_nNumTopPlanes + m_nNumBottomPlanes + m_nNumIntermediatePlanes)) {
        if (!object) {
            continue;
        }

        CWorld::Remove(object);
        // deletingEscalator = true;
        delete std::exchange(object, nullptr);
        // deletingEscalator = false;
    }
    m_nStepObjectsCreated = 0;
}

// 0x717970
void CEscalator::AddThisOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecTop, const CVector& vecEnd, bool moveDown, CEntity* entity) {
    m_bExist = true;

    const CVector z = { 0.0f, 0.0f, CModelInfo::GetModelInfo(ModelIndices::MI_ESCALATORSTEP)->GetColModel()->GetBoundingBox().m_vecMax.z };
    m_vStart  = vecStart - z;
    m_vBottom = vecBottom - z;
    m_vTop    = vecTop - z;
    m_vEnd    = vecEnd - z;

// 0x717A14
    m_nNumIntermediatePlanes = 1 + static_cast<int32>(DistanceBetweenPoints(m_vBottom, m_vTop) * 2.5f);
    m_nNumBottomPlanes       = 1 + static_cast<int32>(DistanceBetweenPoints(m_vBottom, m_vStart) / 3.2f);
    m_nNumTopPlanes          = 1 + static_cast<int32>(DistanceBetweenPoints(m_vEnd, m_vTop) / 3.2f);

    const auto dir = CVector2D{vecStart - vecBottom}.Normalized();
    m_mRotation.GetRight().Set(dir.y, -dir.x, 0.0f);
    m_mRotation.GetForward().Set(dir.x, dir.y, 0.0f);
    m_mRotation.GetUp().Set(0.0f, 0.0f, 1.0f);
    m_mRotation.GetPosition().Reset();
    m_bMoveDown = moveDown;
    m_pEntity   = entity;

    m_Bounding.m_vecCenter = CVector::Centroid({ m_vStart, m_vEnd });
    m_Bounding.m_fRadius   = DistanceBetweenPoints(m_Bounding.m_vecCenter, m_vStart);
    CEntity::RegisterReference(m_pEntity);
}

// 0x717D30
void CEscalator::Update() {
    ZoneScoped;

    constexpr float EXTRA_DIST_TO_RENDER = 20.0f; // 0x858BA4

    if (!m_nStepObjectsCreated) {
        if (m_Bounding.m_fRadius + EXTRA_DIST_TO_RENDER > DistanceBetweenPoints(m_Bounding.m_vecCenter, TheCamera.GetPosition())) {
            const auto allValidObjects = rng::distance(GetObjectPool()->GetAllValid());

            if (GetObjectPool()->GetSize() - allValidObjects > m_nNumIntermediatePlanes + m_nNumBottomPlanes + m_nNumTopPlanes + 10) {
                m_nStepObjectsCreated  = true;
                m_nCurrentPosition = 0.0f;

                for (auto i = 0u; i < m_nNumIntermediatePlanes + m_nNumBottomPlanes + m_nNumTopPlanes; i++) {
                    auto& obj = m_pStepObjects[i];
                    obj       = new CObject((i < m_nNumIntermediatePlanes) ? ModelIndices::MI_ESCALATORSTEP : ModelIndices::MI_ESCALATORSTEP8, true);
                    obj->SetPosn(m_vBottom);
                    CWorld::Add(obj);
                    obj->m_nObjectType = OBJECT_TYPE_DECORATION;
                }
            } else {
                NOTSA_LOG_WARN("Too many shit in the object pool now we can't create escalators!");
            }
        }
    }

    if (!m_nStepObjectsCreated) {
        // still not created after all that effort!
        return; // 0x717F70 - Invert
    }

    const auto posStep = [&] {
        if (m_bMoveDown) {
            return m_nCurrentPosition - CTimer::GetTimeStep() / 25.0f + 1.0f;
        } else {
            return m_nCurrentPosition + CTimer::GetTimeStep() / 25.0f;
        }
    }();
    m_nCurrentPosition = posStep - std::floor(posStep);

    for (auto i = 0u; i < m_nNumIntermediatePlanes + m_nNumBottomPlanes + m_nNumTopPlanes; i++) {
        auto* obj = m_pStepObjects[i];
        if (!obj) {
            continue;
        }

        const auto [t, dir, beg] = [&] {
            if (i < m_nNumIntermediatePlanes) {
                // intermediate
                return std::make_tuple(((float)i - m_nCurrentPosition + 1.0f) / 2.5f, (m_vBottom - m_vTop).Normalized(), m_vTop);
            } else if (i < m_nNumIntermediatePlanes + m_nNumBottomPlanes) {
                // bottom
                return std::make_tuple(((float)(8 * (i - m_nNumIntermediatePlanes) + 4) + m_nCurrentPosition) / 2.5f, (m_vBottom - m_vStart).Normalized(), m_vStart);
            } else {
                // top
                return std::make_tuple(((float)(8 * (i - m_nNumIntermediatePlanes - m_nNumBottomPlanes) + 4) + m_nCurrentPosition) / 2.5f, (m_vEnd - m_vTop).Normalized(), m_vTop);
            }
        }();

        obj->GetMatrix() = m_mRotation;
        obj->SetPosn(beg + dir * t);
        obj->GetMoveSpeed() = (i < m_nNumIntermediatePlanes ? -dir : dir) * 0.016f * (m_bMoveDown ? -1.0f : 1.0f);
        obj->UpdateRW();
        obj->UpdateRwFrame();
        obj->RemoveAndAdd();
    }

    // Out of sight
    if (m_Bounding.m_fRadius + EXTRA_DIST_TO_RENDER + 3.f < DistanceBetweenPoints(m_Bounding.m_vecCenter, TheCamera.GetPosition())) {
        SwitchOff();
    }
}
