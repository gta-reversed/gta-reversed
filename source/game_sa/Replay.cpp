#include "StdInc.h"

#include "Replay.h"

eReplayMode &CReplay::Mode = *reinterpret_cast<eReplayMode*>(0xA43088);
bool& CReplay::bReplayEnabled = *reinterpret_cast<bool*>(0x8A6160);

void CReplay::InjectHooks() {
    RH_ScopedClass(CReplay);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(DisableReplays, 0x45B150);
    RH_ScopedInstall(EnableReplays, 0x45B160);
//    RH_ScopedInstall(StorePedAnimation, 0x0);
//    RH_ScopedInstall(StorePedUpdate, 0x0);
//    RH_ScopedInstall(RetrievePedAnimation, 0x45B4D0);
//    RH_ScopedInstall(ProcessReplayCamera, 0x45D060);
//    RH_ScopedInstall(Display, 0x45C210);
//    RH_ScopedInstall(MarkEverythingAsNew, 0x45D430);
//    RH_ScopedInstall(EmptyReplayBuffer, 0x45EC90);
//    RH_ScopedInstall(GoToNextBlock, 0x0);
//    RH_ScopedInstall(RecordVehicleDeleted, 0x45EBB0);
//    RH_ScopedInstall(RecordPedDeleted, 0x0);
//    RH_ScopedInstall(Init, 0x45E220);
//    RH_ScopedInstall(Init, 0x460625);
//    RH_ScopedInstall(SaveReplayToHD, 0x45C340);
    RH_ScopedInstall(ShouldStandardCameraBeProcessed, 0x45C440);
//    RH_ScopedInstall(ProcessLookAroundCam, 0x45D760);
//    RH_ScopedInstall(FindPoolIndexForPed, 0x0);
//    RH_ScopedInstall(FindPoolIndexForVehicle, 0x0);
//    RH_ScopedInstall(ProcessPedUpdate, 0x45CA70);
//    RH_ScopedInstall(CanWeFindPoolIndexForPed, 0x0);
//    RH_ScopedInstall(CanWeFindPoolIndexForVehicle, 0x0);
//    RH_ScopedInstall(StorePlayerInfoVariables, 0x0);
//    RH_ScopedInstall(StoreStuffInMem, 0x45F180);
//    RH_ScopedInstall(RestorePlayerInfoVariables, 0x45E1F0);
//    RH_ScopedInstall(RestoreStuffFromMem, 0x45ECD0);
//    RH_ScopedInstall(FinishPlayback, 0x45F050);
//    RH_ScopedInstall(StoreClothesDesc, 0x0);
//    RH_ScopedInstall(RecordThisFrame, 0x45E300);
//    RH_ScopedInstall(RestoreClothesDesc, 0x45C7D0);
//    RH_ScopedInstall(DealWithNewPedPacket, 0x45CEA0);
//    RH_ScopedInstall(PlayBackThisFrameInterpolation, 0x45F380);
//    RH_ScopedInstall(FastForwardToTime, 0x460350);
//    RH_ScopedInstall(PlayBackThisFrame, 0x4604A0);
//    RH_ScopedInstall(FindSizeOfPacket, 0x45C8);
//    RH_ScopedInstall(IsThisVehicleUsedInRecording, 0x0);
//    RH_ScopedInstall(IsThisPedUsedInRecording, 0x0);
//    RH_ScopedInstall(InitialiseVehiclePoolConversionTable, 0x0);
//    RH_ScopedInstall(InitialisePedPoolConversionTable, 0x0);
//    RH_ScopedInstall(InitialisePoolConversionTables, 0x0);
//    RH_ScopedInstall(FindFirstFocusCoordinate, 0x45D6C0);
//    RH_ScopedInstall(NumberFramesAvailableToPlay, 0x0);
//    RH_ScopedInstall(StreamAllNecessaryCarsAndPeds, 0x45D4B0);
//    RH_ScopedInstall(CreatePlayerPed, 0x45D540);
//    RH_ScopedInstall(TriggerPlayback, 0x4600F0);
//    RH_ScopedInstall(Update, 0x460500);
}

// 0x460625 thunk
// 0x45E220
void CReplay::Init() {
    plugin::Call<0x45E220>();
}

// 0x460500
void CReplay::Update() {
    plugin::Call<0x460500>();
}

// 0x45B150
void CReplay::DisableReplays() {
    CReplay::bReplayEnabled = false;
}

// 0x45B160
void CReplay::EnableReplays() {
    CReplay::bReplayEnabled = true;
}

// 0x
void CReplay::StorePedAnimation() {
    plugin::Call<0x0>();
}

// 0x
void CReplay::StorePedUpdate(CPed* ped, int32 a2) {
    plugin::Call<0x0, CPed*, int32>(ped, a2);
}

// 0x45B4D0
void CReplay::RetrievePedAnimation() {
    plugin::Call<0x45B4D0>();
}

// 0x45C210
void CReplay::Display() {
    plugin::Call<0x45C210>();
}

// 0x45D430
void CReplay::MarkEverythingAsNew() {
    plugin::Call<0x45D430>();
}

// 0x45EC90
void CReplay::EmptyReplayBuffer() {
    plugin::Call<0x45EC90>();
}

// 0x
void CReplay::GoToNextBlock() {
    plugin::Call<0x0>();
}

// 0x45EBB0
void CReplay::RecordVehicleDeleted(CVehicle* vehicle) {
    plugin::Call<0x45EBB0, CVehicle*>(vehicle);
}

// 0x
void CReplay::RecordPedDeleted(CPed* ped) {
    plugin::Call<0x0, CPed*>(ped);
}

// 0x
void CReplay::InitialiseVehiclePoolConversionTable() {
    plugin::Call<0x0>();
}

// 0x
void CReplay::InitialisePedPoolConversionTable() {
    plugin::Call<0x0>();
}

// 0x
void CReplay::InitialisePoolConversionTables() {
    plugin::Call<0x0>();
}

// 0x45C340
void CReplay::SaveReplayToHD() {
    plugin::Call<0x45C340>();
}

// 0x45C440
bool CReplay::ShouldStandardCameraBeProcessed() {
    return CReplay::Mode != REPLAY_MODE_1;
}

// 0x
void CReplay::FindPoolIndexForPed(int32 a1) {
    plugin::Call<0x0, int32>(a1);
}

// 0x
void CReplay::FindPoolIndexForVehicle(int32 a1) {
    plugin::Call<0x0, int32>(a1);
}

// 0x45CA70
void CReplay::ProcessPedUpdate() {
    plugin::Call<0x45CA70>();
}

// 0x45D060
void CReplay::ProcessReplayCamera() {
    plugin::Call<0x45D060>();
}

// 0x45D760
void CReplay::ProcessLookAroundCam() {
    plugin::Call<0x45D760>();
}

// 0x
void CReplay::CanWeFindPoolIndexForPed(int32 a1) {
    plugin::Call<0x0, int32>(a1);
}

// 0x
void CReplay::CanWeFindPoolIndexForVehicle(int32 a1) {
    plugin::Call<0x0, int32>(a1);
}

// 0x0
void CReplay::StorePlayerInfoVariables() {
    plugin::Call<0x0>();
}

// 0x45F180
void CReplay::StoreStuffInMem() {
    plugin::Call<0x45F180>();
}

// 0x45E1F0
void CReplay::RestorePlayerInfoVariables() {
    plugin::Call<0x45E1F0>();
}

// 0x45ECD0
void CReplay::RestoreStuffFromMem() {
    plugin::Call<0x45ECD0>();
}

// 0x45F050
void CReplay::FinishPlayback() {
    plugin::Call<0x45F050>();
}

// 0x
void CReplay::StoreClothesDesc() {
    plugin::Call<0x0>();
}

// 0x45E300
void CReplay::RecordThisFrame() {
    plugin::Call<0x45E300>();
}

// 0x45C7D0
void CReplay::RestoreClothesDesc() {
    plugin::Call<0x45C7D0>();
}

// 0x45CEA0
void CReplay::DealWithNewPedPacket() {
    plugin::Call<0x45CEA0>();
}

// 0x45F380
void CReplay::PlayBackThisFrameInterpolation() {
    plugin::Call<0x45F380>();
}

// 0x460350
void CReplay::FastForwardToTime(uint32 a1) {
    plugin::Call<0x460350, uint32>(a1);
}

// 0x4604A0
void CReplay::PlayBackThisFrame() {
    plugin::Call<0x4604A0>();
}

// 0x45C850
void CReplay::FindSizeOfPacket(uint16 id) {
    plugin::Call<0x45C850, uint16>(id);
}

// 0x
bool CReplay::IsThisVehicleUsedInRecording(int32 a1) {
    return plugin::CallAndReturn<bool, 0x0, int32>(a1);
}

// 0x
bool CReplay::IsThisPedUsedInRecording(int32 a1) {
    return plugin::CallAndReturn<bool, 0x0, int32>(a1);
}

// 0x45D6C0
void CReplay::FindFirstFocusCoordinate(CVector* a1) {
    plugin::Call<0x45D6C0, CVector*>(a1);
}

// 0x0
void CReplay::NumberFramesAvailableToPlay() {
    plugin::Call<0x0>();
}

// 0x45D4B0
void CReplay::StreamAllNecessaryCarsAndPeds() {
    plugin::Call<0x45D4B0>();
}

// 0x45D540
CPed* CReplay::CreatePlayerPed() {
    return plugin::CallAndReturn<CPed*, 0x45D4B0>();
}

// 0x4600F0
void CReplay::TriggerPlayback(bool a1, float a2, float a3, float a4, bool a5) {
    plugin::Call<0x4600F0, bool, float, float, float, bool>(a1, a2, a3, a4, a5);
}
