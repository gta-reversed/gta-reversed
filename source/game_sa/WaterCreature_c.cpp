#include "StdInc.h"

#include "WaterCreature_c.h"

void WaterCreature_c::InjectHooks()
{
    RH_ScopedClass(WaterCreature_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x6E4160);
    RH_ScopedInstall(Exit, 0x6E3E60);
    RH_ScopedInstall(Update, 0x6E4670);
}

bool WaterCreature_c::Init(int32 nType, CVector* vecPos, WaterCreature_c* parent, float fWaterLevel, float fWaterDepth)
{
    if (CObject::nNoTempObjects >= 150)
        return false;

    const auto& info = WaterCreatureManager_c::GetCreatureInfo(nType);
    if (!parent)
    {
        auto fMaxZ = fWaterLevel - info.m_fMinSpawnDepth;
        auto fMinZ = fWaterLevel - fWaterDepth + 2.0F;
        fMinZ = std::max(fMinZ, fWaterLevel - 50.0F);

        if (fMaxZ < fMinZ)
            return false;

        vecPos->z = CGeneral::GetRandomNumberInRange(fMinZ, fMaxZ);
    }

    GetObjectPool()->m_bIsLocked = true;
    m_pObject = new CObject(*info.m_pModelId, false);
    GetObjectPool()->m_bIsLocked = false;

    if (!m_pObject)
        return false;

    m_pObject->m_nAreaCode = static_cast<eAreaCodes>(CGame::currArea);
    m_pObject->SetIsStatic(true);
    m_pObject->m_bUnderwater = true;
    m_pObject->physicalFlags.bApplyGravity = false;
    m_pObject->m_bUsesCollision = false;
    m_pObject->m_nObjectType = eObjectType::OBJECT_TYPE_DECORATION;
    
    m_nCreatureType = nType;
    m_pFollowedCreature = parent;
    m_bShouldBeDeleted = false;
    if (IsJellyfish())
        m_pObject->m_bDrawLast = true;

    if (parent)
    {
        CVector vecDirection;
        vecDirection.x = CGeneral::GetRandomNumberInRange(-1.0F, 1.0F);
        vecDirection.y = CGeneral::GetRandomNumberInRange(-1.0F, 1.0F);
        vecDirection.z = CGeneral::GetRandomNumberInRange(-0.5F, 0.5F);
        const auto fDist = CGeneral::GetRandomNumberInRange(info.m_fMinDistFromFollowed, info.m_fMaxDistFromFollowed);

        m_vecOffsetFromFollowed = vecDirection * fDist;
        m_pObject->SetPosn(*vecPos + m_vecOffsetFromFollowed);
        m_fHeading = parent->GetObject()->GetHeading();
        m_fDefaultSpeed = CGeneral::GetRandomNumberInRange(info.m_fMinSpeed, parent->m_fDefaultSpeed);
    }
    else
    {
        m_vecOffsetFromFollowed.Set(0.0F, 0.0F, 0.0F);
        m_pObject->SetPosn(*vecPos);
        m_fHeading = CGeneral::GetRandomNumberInRange(-PI, PI);
        m_fDefaultSpeed = CGeneral::GetRandomNumberInRange(info.m_fMinSpeed, info.m_fMaxSpeed);
    }

    m_ucTargetSwimSpeed = 0;
    m_bChangedDir = true;
    m_pObject->SetHeading(m_fHeading);
    m_pObject->UpdateRW();
    m_pObject->UpdateRwFrame();

    CWorld::Add(m_pObject);
    ++CObject::nNoTempObjects;
    return true;
}

void WaterCreature_c::Exit()
{
    g_waterCreatureMan.m_createdList.RemoveItem(this);
    g_waterCreatureMan.m_freeList.AddItem(this);
    CWorld::Remove(m_pObject);
    delete m_pObject;
    m_pObject = nullptr;
    --CObject::nNoTempObjects;
}

void WaterCreature_c::Update(float fTimeStep)
{
    const auto& pInfo = WaterCreatureManager_c::GetCreatureInfo(m_nCreatureType);
    if (m_pFollowedCreature)
    {
        const auto& vecParentPos = m_pFollowedCreature->GetObject()->GetPosition();
        const auto vecPosInGroup = vecParentPos + m_vecOffsetFromFollowed;

        auto vecDir = vecPosInGroup - m_pObject->GetPosition();
        const auto fDistFromGroup = vecDir.NormaliseAndMag();
        if (fDistFromGroup > 0.0F)
        {
            auto fNewHeading = CGeneral::GetRadianAngleBetweenPoints(vecDir.x, vecDir.y, 0.0F, 0.0F);
            fNewHeading = CGeneral::LimitRadianAngle(fNewHeading);
            m_pObject->SetHeading(fNewHeading);
        }

        if (CGeneral::GetRandomNumberInRange(0.0F, 100.0F) < 5.0F) // 5% chance to change speed
            m_fDefaultSpeed = CGeneral::GetRandomNumberInRange(pInfo.m_fMinSpeed, m_pFollowedCreature->m_fDefaultSpeed);


        auto fSpeed = m_fDefaultSpeed;
        if (m_pFollowedCreature->m_ucTargetSwimSpeed > 0)
            fSpeed = m_pFollowedCreature->m_fCurSpeed;

        if (fDistFromGroup > 2.0F)
            fSpeed *= (fDistFromGroup - 2.0F) * 1.1F; // Catch up with group

        fSpeed *= fTimeStep;
        fSpeed = std::min(fSpeed, std::max(0.0F, fDistFromGroup - 0.01F));

        const auto& vecPos = m_pObject->GetPosition();
        const auto vecForward = m_pObject->GetForwardVector();
        m_pObject->SetPosn(vecPos + vecForward * fSpeed);

        if (fDistFromGroup >= pInfo.m_fMaxDistFromFollowed * 5.0F)
            m_pFollowedCreature = nullptr; // Break out from group
    }
    else
    {
        auto bMove = false;
        if (!m_bChangedDir && !m_ucTargetSwimSpeed)
        {
            if (CGeneral::GetRandomNumberInRange(0.0F, 1.0F) < pInfo.m_fChanceToRandomizeRotation)
            {
                m_fHeading = CGeneral::GetRandomNumberInRange(-PI, PI);
                bMove = true;
            }
        }

        const auto& vecPos = m_pObject->GetPosition();
        if (pInfo.m_Speed > 0.0F)
        {
            const auto fRand = CGeneral::GetRandomNumberInRange(0.0F, 100.0F);
            if (fRand < 10.0F || bMove || m_bChangedDir)
            {
                const auto vecTargetPos = vecPos + m_pObject->GetForwardVector() * pInfo.m_Speed;
                CColPoint colPoint;
                CEntity* entity;
                if (CWorld::ProcessLineOfSight(vecPos, vecTargetPos, colPoint, entity, true, false, false, false, false, false, false, false)) {
                    const auto fHeading = CGeneral::GetRadianAngleBetweenPoints(colPoint.m_vecNormal.x, colPoint.m_vecNormal.y, 0.0F, 0.0F);
                    m_fHeading = CGeneral::LimitRadianAngle(fHeading);
                    m_bChangedDir = true;
                }
                else
                {
                    m_bChangedDir = false;
                }
            }
        }

        auto fHeading = m_pObject->GetHeading();
        if (m_fHeading + PI < fHeading)
        {
            m_fHeading += TWO_PI;
        }
        else if (m_fHeading - PI > fHeading)
        {
            m_fHeading -= TWO_PI;
        }

        const auto fHeadingDiff = m_fHeading - fHeading;
        if (fabs(fHeadingDiff) > pInfo.m_fMaxHeadingChange)
        {
            if (fHeadingDiff < 0.0F)
                fHeading -= pInfo.m_fMaxHeadingChange;
            else
                fHeading += pInfo.m_fMaxHeadingChange;
        }

        m_pObject->SetHeading(fHeading);
        if (m_bChangedDir)
        {
            if (!m_ucTargetSwimSpeed && CGeneral::GetRandomNumberInRange(0, 1000) < 8)
            {
                m_ucTargetSwimSpeed = CGeneral::GetRandomNumberInRange(5, 15);
                m_wSpeedChangeCurTime = 0;
                m_wSpeedChangeTotalTime = CGeneral::GetRandomNumberInRange(1000, 2500);
            }
        }
        else
        {
            auto* player = FindPlayerPed(0);
            auto& vecPlayerPos = player->GetPosition();
            auto vecSwimDir = vecPos - vecPlayerPos;
            if (vecSwimDir.NormaliseAndMag() < 5.0F) // Swim away from player
            {
                m_ucTargetSwimSpeed = CGeneral::GetRandomNumberInRange(35, 50);
                m_wSpeedChangeCurTime = 0;
                m_wSpeedChangeTotalTime = CGeneral::GetRandomNumberInRange(2000, 4500);

                const auto fNewHeading = CGeneral::GetRadianAngleBetweenPoints(vecSwimDir.x, vecSwimDir.y, 0.0F, 0.0F);
                m_fHeading = CGeneral::LimitRadianAngle(fNewHeading);
                if (IsSmallFish())
                    m_pObject->SetHeading(m_fHeading);
            }
        }

        auto fCurSpeed = m_fDefaultSpeed;
        if (m_ucTargetSwimSpeed > 0)
        {
            m_wSpeedChangeCurTime += static_cast<int16>(fTimeStep * 1000.0F);
            const auto fInvProgress = std::max(0.0F, 1.0F - static_cast<float>(m_wSpeedChangeCurTime) / static_cast<float>(m_wSpeedChangeTotalTime));
            fCurSpeed += static_cast<float>(m_ucTargetSwimSpeed) * 0.1F * fInvProgress * fCurSpeed;
            if (m_wSpeedChangeCurTime >= m_wSpeedChangeTotalTime)
                m_ucTargetSwimSpeed = 0;

            m_fCurSpeed = fCurSpeed;
        }

        const auto vecForward = m_pObject->GetForwardVector();
        const auto vecNewPos = m_pObject->GetPosition() + vecForward * fTimeStep * fCurSpeed;
        m_pObject->SetPosn(vecNewPos);
    }

    if (IsJellyfish())
    {
        auto& vecJellyPos = m_pObject->GetPosition();
        float fWaterLevel;
        if (CWaterLevel::GetWaterLevel(vecJellyPos.x, vecJellyPos.y, vecJellyPos.z, fWaterLevel, true, nullptr))
            m_pObject->SetPosn(CVector(vecJellyPos.x, vecJellyPos.y, fWaterLevel - 0.2F));
    }

    m_pObject->UpdateRW();
    m_pObject->UpdateRwFrame();
    CWorld::Remove(m_pObject); //BUG? Is this remove needed here?
    CWorld::Add(m_pObject);

    const auto& vecCamPos = TheCamera.GetPosition();
    if (DistanceBetweenPointsSquared(vecCamPos, m_pObject->GetPosition()) >= WaterCreatureManager_c::ms_fMaxWaterCreaturesDrawDistanceSquared)
        g_waterCreatureMan.TryToExitGroup(this);
}
