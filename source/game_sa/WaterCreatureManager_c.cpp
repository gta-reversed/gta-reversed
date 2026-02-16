#include "StdInc.h"

#include "WaterCreatureManager_c.h"

WaterCreatureManager_c& g_waterCreatureMan = *(WaterCreatureManager_c*)0xC1DF30;
WaterCreatureInfo(&WaterCreatureManager_c::ms_waterCreatureInfos)[NUM_WATER_CREATURE_INFOS] = *(WaterCreatureInfo(*)[NUM_WATER_CREATURE_INFOS])0x8D3698;

void WaterCreatureManager_c::InjectHooks()
{
    RH_ScopedClass(WaterCreatureManager_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x6E3F90);
    RH_ScopedInstall(Exit, 0x6E3FD0);
    RH_ScopedInstall(Update, 0x6E4F10);
    RH_ScopedInstall(GetRandomWaterCreatureId, 0x6E4040);
    RH_ScopedInstall(CanAddWaterCreatureAtPos, 0x6E4510);
    RH_ScopedInstall(TryToFreeUpWaterCreatures, 0x6E40E0);
    RH_ScopedInstall(TryToExitGroup, 0x6E45B0);
}


WaterCreatureManager_c::WaterCreatureManager_c() : m_aCreatures(), m_freeList(), m_createdList()
{}

bool WaterCreatureManager_c::Init()
{
    for (auto i = 0; i < NUM_WATER_CREATURES; ++i)
        m_freeList.AddItem(&m_aCreatures[i]);

    m_nLastCreationCheckTime = 0;
    return true;
}

void WaterCreatureManager_c::Exit()
{
    auto* pCur = m_createdList.GetHead();
    while (pCur)
    {
        auto* pNext = m_createdList.GetNext(pCur);
        pCur->Exit();
        pCur = pNext;
    }

    m_freeList.RemoveAll();
}

int32 WaterCreatureManager_c::GetRandomWaterCreatureId()
{
    auto nRand = CGeneral::GetRandomNumberInRange(0, 100);
    if (nRand < 80)
        return CGeneral::GetRandomNumberInRange(0, 3); // Small fish

    if (nRand < 90)
        return CGeneral::GetRandomNumberInRange(3, 5); // Jelly fish

    if (nRand < 97)
        return eWaterCreatureType::TURTLE;

    return eWaterCreatureType::DOLPHIN;
}

void WaterCreatureManager_c::TryToFreeUpWaterCreatures(int32 numToFree)
{
    auto* pCur = m_createdList.GetHead();
    int32 iCounter = 0;
    while (iCounter < numToFree && pCur)
    {
        auto* pNext = m_createdList.GetNext(pCur);
        if (pCur->m_pObject->m_bOffscreen && pCur->m_pFollowedCreature)
        {
            pCur->Exit();
            ++iCounter;
        }
        pCur = pNext;
    }
}

bool WaterCreatureManager_c::CanAddWaterCreatureAtPos(int32 nCreatureType, CVector vecPos)
{
    auto* pCur = m_createdList.GetHead();
    if (!pCur)
        return true;

    const auto& info = WaterCreatureManager_c::GetCreatureInfo(nCreatureType); // Originally not assigned by reference, but copied to stack
    while (pCur)
    {
        if (pCur->m_nCreatureType == nCreatureType && !pCur->m_pFollowedCreature)
        {
            const auto& vecObjPos = pCur->GetObject()->GetPosition();
            if (DistanceBetweenPointsSquared(vecObjPos, vecPos) <= info.m_fMinDistFromSameCreature)
                return false;
        }
        pCur = m_createdList.GetNext(pCur);
    }
    return true;
}

void WaterCreatureManager_c::TryToExitGroup(WaterCreature_c* pCreature)
{
    auto* pExitCreature = pCreature->m_pFollowedCreature ? pCreature->m_pFollowedCreature : pCreature;
    int32 iCounter = 0;
    WaterCreature_c* apCreatures[32];
    auto* pCur = m_createdList.GetHead();
    while (pCur)
    {
        if (pCur == pExitCreature || pCur->m_pFollowedCreature == pExitCreature)
        {
            apCreatures[iCounter] = pCur;
            ++iCounter;
        }

        pCur = m_createdList.GetNext(pCur);
    }

    const auto& vecCamPos = TheCamera.GetPosition();
    for(int32 i = 0; i < iCounter; ++i)
        if (DistanceBetweenPointsSquared(vecCamPos, apCreatures[i]->GetObject()->GetPosition()) < WaterCreatureManager_c::ms_fMaxWaterCreaturesDrawDistanceSquared)
            return; // Jump out of function if any of the creatures in group aren't out of camera reach.
                    // All fishes in group have to be destroyed at once as no to leave any of them with dangling pointers


    for (int32 i = 0; i < iCounter; ++i)
        apCreatures[i]->m_bShouldBeDeleted = true;
}

void WaterCreatureManager_c::Update(float fTimestep) {
    ZoneScoped;

    if (FindPlayerPed(0)->GetPlayerData()->m_nWaterCoverPerc > 50)
    {
        const auto nCurTime = CTimer::GetTimeInMS();
        if (nCurTime - m_nLastCreationCheckTime > 1000 && m_freeList.GetNumItems() > 0)
        {
            m_nLastCreationCheckTime = nCurTime;
            CVector vecDirection;
            vecDirection.x = CGeneral::GetRandomNumberInRange(-1.0F, 1.0F);
            vecDirection.y = CGeneral::GetRandomNumberInRange(-1.0F, 1.0F);
            vecDirection.z = 0.0F;
            vecDirection.Normalise();

            const auto fDist = CGeneral::GetRandomNumberInRange(25.0F, 40.0F);
            auto vecCreationPos = TheCamera.GetPosition() + vecDirection * fDist;

            float fWaterDepth, fWaterLevel;
            if (!TheCamera.IsSphereVisible(vecCreationPos, 3.0F)
                && CWaterLevel::GetWaterDepth(vecCreationPos, &fWaterDepth, &fWaterLevel, nullptr)
                && fWaterDepth > 4.5F)
            {
                const auto nType = WaterCreatureManager_c::GetRandomWaterCreatureId();
                if (WaterCreatureManager_c::CanAddWaterCreatureAtPos(nType, vecCreationPos))
                {
                    const auto& pInfo = WaterCreatureManager_c::GetCreatureInfo(nType);
                    uint32 iNumCreated = CGeneral::GetRandomNumberInRange(pInfo.m_nMinCreated, pInfo.m_nMaxCreated);
                    iNumCreated = std::min(iNumCreated, m_freeList.GetNumItems());

                    WaterCreature_c* pGroupLeader = nullptr;
                    for (uint32 i = 0; i < iNumCreated; ++i)
                    {
                        auto* pNewCreature = m_freeList.RemoveHead();
                        if (pNewCreature->Init(nType, &vecCreationPos, pGroupLeader, fWaterLevel, fWaterDepth))
                            m_createdList.AddItem(pNewCreature);
                        else
                            m_freeList.AddItem(pNewCreature);

                        if (!pGroupLeader)
                            pGroupLeader = pNewCreature;
                    }
                }
            }
        }
    }

    // Trigger updates
    auto* pCur = m_createdList.GetHead();
    while (pCur)
    {
        auto* pNext = m_createdList.GetNext(pCur);
        pCur->Update(fTimestep);
        pCur = pNext;
    }

    // Remove creatures queued for removal
    pCur = m_createdList.GetHead();
    while (pCur)
    {
        auto* pNext = m_createdList.GetNext(pCur);
        if (pCur->m_bShouldBeDeleted)
            pCur->Exit();

        pCur = pNext;
    }
}
