#include "StdInc.h"

#include "Escalator.h"

void CEscalator::InjectHooks() {
    RH_ScopedClass(CEscalator);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x717110);
    RH_ScopedInstall(Destructor, 0x717130);

    RH_ScopedInstall(SwitchOff, 0x717860);
    RH_ScopedInstall(AddThisOne, 0x717970);
    RH_ScopedInstall(Update, 0x717D30, { .reversed = false });
}

// 0x717860
void CEscalator::SwitchOff() {
    // debug leftover
    // static bool& deletingEscalator = StaticRef<bool>(0xC6E98C);

    if (!m_nObjectsCreated) {
        return;
    }

    for (auto* object : m_pObjects | rngv::take(m_nNumTopPlanes + m_nNumBottomPlanes + m_nNumIntermediatePlanes)) {
        if (!object) {
            continue;
        }

        CWorld::Remove(object);
        // deletingEscalator = true;
        delete std::exchange(object, nullptr);
        // deletingEscalator = false;
    }
    m_nObjectsCreated = 0;
}

// 0x717970
void CEscalator::AddThisOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecTop, const CVector& vecEnd, bool moveDown, CEntity* entity) {
    m_bExist = true;

    const CVector z = { 0.0f, 0.0f, CModelInfo::GetModelInfo(ModelIndices::MI_ESCALATORSTEP)->GetColModel()->GetBoundingBox().m_vecMax.z };
    m_vStart  = vecStart - z;
    m_vBottom = vecBottom - z;
    m_vTop    = vecTop - z;
    m_vEnd    = vecEnd - z;

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
    plugin::CallMethod<0x717D30, CEscalator*>(this);
}
