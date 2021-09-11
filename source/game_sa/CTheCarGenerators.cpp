#include "StdInc.h"

#include "CTheCarGenerators.h"

uint8& CTheCarGenerators::GenerateEvenIfPlayerIsCloseCounter = *reinterpret_cast<uint8*>(0xC279D0);
uint8& CTheCarGenerators::ProcessCounter = *reinterpret_cast<uint8*>(0xC279D1);
uint32& CTheCarGenerators::NumOfCarGenerators = *reinterpret_cast<uint32*>(0xC279D4);
CSpecialPlateHandler& CTheCarGenerators::m_SpecialPlateHandler = *reinterpret_cast<CSpecialPlateHandler*>(0xC279D8);
CCarGenerator(&CTheCarGenerators::CarGeneratorArray)[NUM_CAR_GENERATORS] = *reinterpret_cast<CCarGenerator(*)[NUM_CAR_GENERATORS]>(0xC27AD0);

void CTheCarGenerators::InjectHooks()
{
    ReversibleHooks::Install("CTheCarGenerators", "CreateCarGenerator", 0x6F31A0, &CTheCarGenerators::CreateCarGenerator);
    ReversibleHooks::Install("CTheCarGenerators", "Init", 0x6F3270, &CTheCarGenerators::Init);
    ReversibleHooks::Install("CTheCarGenerators", "Load", 0x5D39B0, &CTheCarGenerators::Load);
    ReversibleHooks::Install("CTheCarGenerators", "Process", 0x6F3F40, &CTheCarGenerators::Process);
    ReversibleHooks::Install("CTheCarGenerators", "RemoveCarGenerators", 0x6F3240, &CTheCarGenerators::RemoveCarGenerators);
    ReversibleHooks::Install("CTheCarGenerators", "Save", 0x5D38C0, &CTheCarGenerators::Save);
    ReversibleHooks::Install("CTheCarGenerators", "Get", 0x479D60, &CTheCarGenerators::Get);
}

// return index of CarGenerator in CTheCarGenerators::CarGeneratorArray
// 0x6F31A0
signed int CTheCarGenerators::CreateCarGenerator(CVector posn, float angle, int32 modelId, int16 color1, int16 color2, uint8 forceSpawn, uint8 alarmChances, uint8 doorLockChances,
                                                 uint16 minDelay, uint16 maxDelay, uint8 iplId, bool ignorePopulationLimit)
{
    if (modelId != -1 && (modelId < MODEL_LANDSTAL || modelId > MODEL_VEG_PALMKB8))
        return -1;

    int32 carGenIndex = 0;
    for (auto& carGen : CarGeneratorArray)
    {
        if (!carGen.m_bIsUsed)
        {
            carGen.Setup(posn, angle, modelId, color1, color2, forceSpawn, alarmChances, doorLockChances, minDelay, maxDelay, iplId, ignorePopulationLimit);
            NumOfCarGenerators++;
            return carGenIndex;
        }
        carGenIndex++;
    }
    return -1;
}

void CTheCarGenerators::Init() {
    NumOfCarGenerators = 0;
    ProcessCounter = 0;
    GenerateEvenIfPlayerIsCloseCounter = 0;
    m_SpecialPlateHandler.m_nCount = 0;
    for (auto& plateTextEntry : m_SpecialPlateHandler.m_plateTextEntries)
    {
        plateTextEntry.m_nCarGenId = -1;
        plateTextEntry.m_szPlateText[0] = 0;
    }
    for (auto& carGen : CarGeneratorArray)
        carGen.m_bIsUsed = false;
}

void CTheCarGenerators::Load() {
    Init();

    CGenericGameStorage::LoadDataFromWorkBuffer(&NumOfCarGenerators, sizeof(NumOfCarGenerators));
    CGenericGameStorage::LoadDataFromWorkBuffer(&ProcessCounter, sizeof(ProcessCounter));
    CGenericGameStorage::LoadDataFromWorkBuffer(&GenerateEvenIfPlayerIsCloseCounter, sizeof(char));
    for (uint32 i = 0; i < NumOfCarGenerators; i++)
    {
        uint16 carGenIndex;
        CGenericGameStorage::LoadDataFromWorkBuffer(&carGenIndex, sizeof(int16));
        CGenericGameStorage::LoadDataFromWorkBuffer(CarGeneratorArray + carGenIndex, sizeof(CCarGenerator));
    }

    CGenericGameStorage::LoadDataFromWorkBuffer(&m_SpecialPlateHandler.m_nCount, sizeof(int32));
    for (int32 i = 0; i < 15; i++)
        CGenericGameStorage::LoadDataFromWorkBuffer(m_SpecialPlateHandler.m_plateTextEntries + i, sizeof(tCarGenPlateText));
}

void CTheCarGenerators::Process() {
    if (FindPlayerTrain(-1) || CCutsceneMgr::ms_cutsceneProcessing || CReplay::Mode == eReplayMode::REPLAY_MODE_1)
        return;

    ProcessCounter++;
    if (ProcessCounter == 4)
        ProcessCounter = 0;

    for (int32 i = ProcessCounter; i < NUM_CAR_GENERATORS; i += 4)
    {
        if (CarGeneratorArray[i].m_bIsUsed)
            CarGeneratorArray[i].Process();
    }

    if (GenerateEvenIfPlayerIsCloseCounter)
        GenerateEvenIfPlayerIsCloseCounter--;
}

void CTheCarGenerators::RemoveCarGenerators(uint8 IplID) {
    for (auto& carGen : CarGeneratorArray)
        if (carGen.m_nIplId == IplID)
        {
            carGen.m_nIplId = 0;
            carGen.m_bIsUsed = false;
            NumOfCarGenerators--;
        }
}

void CTheCarGenerators::Save() {
    uint32 numOfCarGeneratorsToSave = 0;

    for (auto& carGen : CarGeneratorArray)
        if (carGen.m_bIsUsed && carGen.m_nIplId == 0)
            numOfCarGeneratorsToSave++;

    CGenericGameStorage::SaveDataToWorkBuffer(&numOfCarGeneratorsToSave, sizeof(numOfCarGeneratorsToSave));
    CGenericGameStorage::SaveDataToWorkBuffer(&ProcessCounter, sizeof(ProcessCounter));
    CGenericGameStorage::SaveDataToWorkBuffer(&GenerateEvenIfPlayerIsCloseCounter, sizeof(GenerateEvenIfPlayerIsCloseCounter));
    for (uint16 i = 0; i < NUM_CAR_GENERATORS; i++) {
        if (CarGeneratorArray[i].m_bIsUsed && CarGeneratorArray[i].m_nIplId == 0) {
            CGenericGameStorage::SaveDataToWorkBuffer(&i, sizeof(int16));
            CGenericGameStorage::SaveDataToWorkBuffer(CarGeneratorArray + i, sizeof(CCarGenerator));
        }
    }
    CGenericGameStorage::SaveDataToWorkBuffer(&m_SpecialPlateHandler.m_nCount, sizeof(int32));
    for (int32 i = 0; i < 15; i++)
        CGenericGameStorage::SaveDataToWorkBuffer(m_SpecialPlateHandler.m_plateTextEntries + i, sizeof(tCarGenPlateText));
}

CCarGenerator* CTheCarGenerators::Get(uint16 index)
{
    return &CarGeneratorArray[index];
}

int32 CTheCarGenerators::GetIndex(CCarGenerator* pCarGen)
{
    return  pCarGen - CarGeneratorArray;
}
