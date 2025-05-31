#include "StdInc.h"

#include "TheCarGenerators.h"
#include "CarGenerator.h"

uint8& CTheCarGenerators::GenerateEvenIfPlayerIsCloseCounter = *reinterpret_cast<uint8*>(0xC279D0);
uint8& CTheCarGenerators::ProcessCounter = *reinterpret_cast<uint8*>(0xC279D1);
uint32& CTheCarGenerators::NumOfCarGenerators = *reinterpret_cast<uint32*>(0xC279D4);
CSpecialPlateHandler& CTheCarGenerators::m_SpecialPlateHandler = *reinterpret_cast<CSpecialPlateHandler*>(0xC279D8);
CCarGenerator(&CTheCarGenerators::CarGeneratorArray)[NUM_CAR_GENERATORS] = *reinterpret_cast<CCarGenerator(*)[NUM_CAR_GENERATORS]>(0xC27AD0);

void CTheCarGenerators::InjectHooks()
{
    RH_ScopedClass(CTheCarGenerators);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(CreateCarGenerator, 0x6F31A0);
    RH_ScopedInstall(Init, 0x6F3270);
    RH_ScopedInstall(Load, 0x5D39B0);
    RH_ScopedInstall(Process, 0x6F3F40);
    RH_ScopedInstall(RemoveCarGenerators, 0x6F3240);
    RH_ScopedInstall(Save, 0x5D38C0);
    RH_ScopedInstall(Get, 0x479D60);
}

// return index of CarGenerator in CTheCarGenerators::CarGeneratorArray
// 0x6F31A0
int32 CTheCarGenerators::CreateCarGenerator(CVector posn, float angle, int32 modelId, int16 color1, int16 color2, uint8 forceSpawn, uint8 alarmChances, uint8 doorLockChances,
                                                 uint16 minDelay, uint16 maxDelay, uint8 iplId, bool ignorePopulationLimit)
{
    if (modelId != -1 && (modelId < MODEL_LANDSTAL || modelId > MODEL_VEG_PALMKB8))
        return -1;

    int32 carGenIndex = 0;
    for (auto& carGen : CarGeneratorArray) {
        if (!carGen.m_bIsUsed) {
            carGen.Setup(posn, angle, modelId, color1, color2, forceSpawn, alarmChances, doorLockChances, minDelay, maxDelay, iplId, ignorePopulationLimit);
            NumOfCarGenerators++;
            return carGenIndex;
        }
        carGenIndex++;
    }
    return -1;
}

// 0x6F3270
void CTheCarGenerators::Init() {
    ZoneScoped;

    NumOfCarGenerators = 0;
    ProcessCounter = 0;
    GenerateEvenIfPlayerIsCloseCounter = 0;
    m_SpecialPlateHandler.m_nCount = 0;
    for (auto& plateTextEntry : m_SpecialPlateHandler.m_plateTextEntries) {
        plateTextEntry.m_nCarGenId = -1;
        plateTextEntry.m_szPlateText[0] = 0;
    }
    for (auto& carGen : CarGeneratorArray) {
        carGen.m_bIsUsed = false;
    }
}

// 0x5D39B0
void CTheCarGenerators::Load() {
    Init();

    CGenericGameStorage::LoadDataFromWorkBuffer(NumOfCarGenerators);
    CGenericGameStorage::LoadDataFromWorkBuffer(ProcessCounter);
    CGenericGameStorage::LoadDataFromWorkBuffer(GenerateEvenIfPlayerIsCloseCounter);
    for (uint32 i = 0; i < NumOfCarGenerators; i++) {
        auto carGenIndex = CGenericGameStorage::LoadDataFromWorkBuffer<uint16>();
        if (carGenIndex < NUM_CAR_GENERATORS) {
            CGenericGameStorage::LoadDataFromWorkBuffer(CarGeneratorArray[carGenIndex]);
        }
    }

    CGenericGameStorage::LoadDataFromWorkBuffer(m_SpecialPlateHandler.m_nCount);
    for (int32 i = 0; i < 15; i++) {
        CGenericGameStorage::LoadDataFromWorkBuffer(m_SpecialPlateHandler.m_plateTextEntries[i]);
    }
}

// 0x6F3F40
void CTheCarGenerators::Process() {
    ZoneScoped;

    if (FindPlayerTrain() || CCutsceneMgr::IsCutsceneProcessing() || CReplay::Mode == MODE_PLAYBACK)
        return;

    ProcessCounter++;
    if (ProcessCounter == 4) {
        ProcessCounter = 0;
    }

    for (int32 i = ProcessCounter; i < NUM_CAR_GENERATORS; i += 4) {
        if (CarGeneratorArray[i].m_bIsUsed) {
            CarGeneratorArray[i].Process();
        }
    }

    if (GenerateEvenIfPlayerIsCloseCounter) {
        GenerateEvenIfPlayerIsCloseCounter--;
    }
}

// 0x6F3240
void CTheCarGenerators::RemoveCarGenerators(uint8 IplID) {
    for (auto& carGen : CarGeneratorArray) {
        if (carGen.m_nIplId == IplID) {
            carGen.m_nIplId = 0;
            carGen.m_bIsUsed = false;
            NumOfCarGenerators--;
        }
    }
}

// 0x5D38C0
void CTheCarGenerators::Save() {
    uint32 numOfCarGeneratorsToSave = 0;

    for (auto& carGen : CarGeneratorArray) {
        if (carGen.m_bIsUsed && carGen.m_nIplId == 0) {
            numOfCarGeneratorsToSave++;
        }
    }

    CGenericGameStorage::SaveDataToWorkBuffer(numOfCarGeneratorsToSave);
    CGenericGameStorage::SaveDataToWorkBuffer(ProcessCounter);
    CGenericGameStorage::SaveDataToWorkBuffer(GenerateEvenIfPlayerIsCloseCounter);

    for (uint16 i = 0; i < NUM_CAR_GENERATORS; i++) {
        auto& carGenerator = CarGeneratorArray[i];
        if (carGenerator.m_bIsUsed && carGenerator.m_nIplId == 0) {
            CGenericGameStorage::SaveDataToWorkBuffer(i);
            CGenericGameStorage::SaveDataToWorkBuffer(carGenerator);
        }
    }
    CGenericGameStorage::SaveDataToWorkBuffer(m_SpecialPlateHandler.m_nCount);
    for (int32 i = 0; i < 15; i++) {
        CGenericGameStorage::SaveDataToWorkBuffer(m_SpecialPlateHandler.m_plateTextEntries[i]);
    }
}

CCarGenerator* CTheCarGenerators::Get(uint16 index) {
    return &CarGeneratorArray[index];
}

int32 CTheCarGenerators::GetIndex(CCarGenerator* carGen) {
    return carGen - CarGeneratorArray;
}
