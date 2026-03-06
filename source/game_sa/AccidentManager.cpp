#include "StdInc.h"

#include "AccidentManager.h"

auto& CAccidentManager::gAccidentManager = StaticRef<CAccidentManager*>(0xB9B7D0);

void CAccidentManager::InjectHooks()
{
    RH_ScopedClass(CAccidentManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetInstance, 0x56CF20);
    RH_ScopedInstall(ReportAccident, 0x56CE80);
    RH_ScopedInstall(GetNumberOfFreeAccidents, 0x56CEE0);
    RH_ScopedInstall(GetNearestFreeAccidentExceptThisOne, 0x56CF90);
    RH_ScopedInstall(GetNearestFreeAccident, 0x56D050);
}

// 0x56CF20
CAccidentManager* CAccidentManager::GetInstance()
{
    if (!gAccidentManager)
        gAccidentManager = new CAccidentManager();

    return gAccidentManager;
}

// 0x56CE80
void CAccidentManager::ReportAccident(CPed* ped)
{
    if (!ped->IsCreatedBy(PED_MISSION) && ped->bAllowMedicsToReviveMe)
    {
        int32 slotIndex = -1;
        for (int32 i = 0; i < NUM_ACCIDENTS; i++)
        {
            if (m_Accidents[i].m_pPed == ped)
                return;

            if (!m_Accidents[i].m_pPed && slotIndex == -1)
                slotIndex = i;
        }

        if (slotIndex != -1)
        {
            m_Accidents[slotIndex].m_pPed = ped;
            m_Accidents[slotIndex].m_bIsTreated = false;
            m_Accidents[slotIndex].m_bIsRevived = false;
            ped->RegisterReference(reinterpret_cast<CEntity**>(&m_Accidents[slotIndex].m_pPed));
        }
    }
}

// 0x56CEE0
int32 CAccidentManager::GetNumberOfFreeAccidents()
{
    int32 count = 0;

    for (auto& acc : m_Accidents)
        if (acc.IsFree())
            count++;

    return count;
}

// 0x56CF90
CAccident* CAccidentManager::GetNearestFreeAccidentExceptThisOne(CVector& posn, CAccident* thisOne, bool bIgnoreHeadless)
{
    float minDistance = FLT_MAX;
    CAccident* nearestAcc = nullptr;

    for (auto& acc : m_Accidents)
    {
        if (acc.IsFree()
            && !(acc.m_pPed->bRemoveHead && bIgnoreHeadless)
            && !(thisOne && thisOne == &acc)
            )
        {
            float distance = DistanceBetweenPoints(acc.m_pPed->GetPosition(), posn);
            if (distance < minDistance)
            {
                minDistance = distance;
                nearestAcc = &acc;
            }
        }
    }

    return nearestAcc;
}

// 0x56D050
CAccident* CAccidentManager::GetNearestFreeAccident(CVector& posn, bool bIgnoreHeadless)
{
    return GetNearestFreeAccidentExceptThisOne(posn, nullptr, bIgnoreHeadless);
}
