#include "StdInc.h"

#include "Camera.h"
#include "ControllerConfigManager.h"
#include "EntryExitManager.h"
#include "FireManager.h"
#include "PedClothesDesc.h"
#include "PlaneBanners.h"
#include "RealTimeShadowManager.h"
#include "Replay.h"
#include "Skidmarks.h"

void CReplay::InjectHooks() {
    RH_ScopedClass(CReplay);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x45E220);
    RH_ScopedInstall(Update, 0x460500);
    RH_ScopedInstall(DisableReplays, 0x45B150);
    RH_ScopedInstall(EnableReplays, 0x45B160);
    RH_ScopedInstall(StorePedAnimation, 0x45B170);
    RH_ScopedInstall(StorePedUpdate, 0x45C940);
    RH_ScopedInstall(RetrievePedAnimation, 0x45B4D0);
    RH_ScopedInstall(ProcessReplayCamera, 0x45D060);
    RH_ScopedInstall(Display, 0x45C210);
    RH_ScopedInstall(MarkEverythingAsNew, 0x45D430);
    RH_ScopedInstall(EmptyReplayBuffer, 0x45EC90);
    RH_ScopedInstall(EmptyPedsAndVehiclePools_NoDestructors, 0x45D390);
    RH_ScopedInstall(GoToNextBlock, 0x45E2A0);
    RH_ScopedInstall(RecordVehicleDeleted, 0x45EBB0);
    RH_ScopedInstall(RecordPedDeleted, 0x45EC20);
    RH_ScopedInstall(SaveReplayToHD, 0x45C340);
    RH_ScopedInstall(PlayReplayFromHD, 0x460390);
    RH_ScopedInstall(ShouldStandardCameraBeProcessed, 0x45C440);
    RH_ScopedInstall(ProcessLookAroundCam, 0x45D760);
    RH_ScopedInstall(FindPoolIndexForPed, 0x45C450);
    RH_ScopedInstall(FindPoolIndexForVehicle, 0x45C460);
    RH_ScopedInstall(ProcessPedUpdate, 0x45CA70);
    RH_ScopedInstall(CanWeFindPoolIndexForPed, 0x45C470);
    RH_ScopedInstall(CanWeFindPoolIndexForVehicle, 0x45C490);
    RH_ScopedInstall(StorePlayerInfoVariables, 0x45F020);
    RH_ScopedInstall(RestorePlayerInfoVariables, 0x45E1F0);
    RH_ScopedInstall(StoreStuffInMem, 0x45F180);
    RH_ScopedInstall(RestoreStuffFromMem, 0x45ECD0);
    RH_ScopedInstall(FinishPlayback, 0x45F050);
    RH_ScopedInstall(RecordThisFrame, 0x45E300);
    RH_ScopedInstall(StoreClothesDesc, 0x45C750);
    RH_ScopedInstall(RestoreClothesDesc, 0x45C7D0);
    RH_ScopedInstall(DealWithNewPedPacket, 0x45CEA0);
    RH_ScopedInstall(PlayBackThisFrameInterpolation, 0x45F380);
    RH_ScopedInstall(FastForwardToTime, 0x460350);
    RH_ScopedInstall(PlayBackThisFrame, 0x4604A0);
    RH_ScopedInstall(FindSizeOfPacket, 0x45C850);
    RH_ScopedInstall(IsThisVehicleUsedInRecording, 0x45DE40);
    RH_ScopedInstall(IsThisPedUsedInRecording, 0x45DDE0);
    RH_ScopedInstall(InitialiseVehiclePoolConversionTable, 0x45EFA0);
    RH_ScopedInstall(InitialisePedPoolConversionTable, 0x45EF20);
    RH_ScopedInstall(InitialisePoolConversionTables, 0x45F370);
    RH_ScopedInstall(FindFirstFocusCoordinate, 0x45D6C0);
    RH_ScopedInstall(NumberFramesAvailableToPlay, 0x45D670);
    RH_ScopedInstall(StreamAllNecessaryCarsAndPeds, 0x45D4B0);
    RH_ScopedInstall(CreatePlayerPed, 0x45D540);
    RH_ScopedInstall(TriggerPlayback, 0x4600F0);
}

// 0x460625 thunk
// 0x45E220
void CReplay::Init() {
    Mode = MODE_RECORD;
    Playback = {};
    EmptyReplayBuffer();
    pReferences = nullptr;
    pPickups = nullptr;
    pEmptyReferences = nullptr;
    pWorld1 = nullptr;
    pStoredCam = nullptr;
    pRadarBlips = nullptr;
    bPlayingBackFromFile = false;
    bDoLoadSceneWhenDone = false;
    FramesActiveLookAroundCam = 0;
    bReplayEnabled = true;
}

// 0x460500
void CReplay::Update() {
    ZoneScoped;

    if (CCutsceneMgr::IsCutsceneProcessing() || CPad::GetPad()->ArePlayerControlsDisabled()
        || FrontEndMenuManager.m_bMenuActive || CEntryExitManager::ms_exitEnterState) {
        Init();
        return;
    }

    if (CTimer::GetIsCodePaused() || CTimer::GetIsUserPaused())
        return;

    static uint32& s_FrameRecordCounter = *reinterpret_cast<uint32*>(0x97FB40); // STATICREF
    if (Mode == MODE_PLAYBACK) {
        PlayBackThisFrame();
    } else if (CTimer::GetTimeInMS() - s_FrameRecordCounter > 26) {
        s_FrameRecordCounter = CTimer::GetTimeInMS();
        RecordThisFrame();
    }

    if (CDraw::IsFading() || !bReplayEnabled)
        return;

    if (Mode == MODE_PLAYBACK) {
        if (ControlsManager.GetIsKeyboardKeyDown(rsF1) || ControlsManager.GetIsKeyboardKeyDown(rsF3)) {
            FinishPlayback();
        }
    } else {
        if (ControlsManager.GetIsKeyboardKeyDown(rsF1)) {
            // Play stored replay
            TriggerPlayback(REPLAY_CAM_MODE_AS_STORED, CVector{}, false);
        } else if (ControlsManager.GetIsKeyboardKeyDown(rsF2)) {
            // Save to hard disk
            SaveReplayToHD();
        } else if (ControlsManager.GetIsKeyboardKeyDown(rsF3)) {
            // Play from hard disk
            PlayReplayFromHD();
        }
    }
}

// 0x45B150
void CReplay::DisableReplays() {
    bReplayEnabled = false;
}

// 0x45B160
void CReplay::EnableReplays() {
    bReplayEnabled = true;
}

// 0x45B170
void CReplay::StorePedAnimation(CPed* ped, CStoredAnimationState& state) {
    float blendValue{};
    CAnimBlendAssociation* second{};
    const auto main = RpAnimBlendClumpGetMainAssociation(ped->m_pRwClump, &second, &blendValue);
    if (main) {
        state[0] = AnimationState::Make(main->m_AnimId, main->m_CurrentTime, main->m_Speed, static_cast<uint8>(main->m_AnimGroupId));
    } else {
        state[0] = AnimationState::Make(ANIM_ID_IDLE, 0.0f, 1.0f, 0);
    }

    if (second) {
        state[1] = AnimationState::MakeBlend(second->m_AnimId, second->m_CurrentTime, second->m_Speed, static_cast<uint8>(second->m_AnimGroupId), blendValue);
    } else {
        state[1] = AnimationState::MakeBlend(ANIM_ID_WALK, 0.0f, 0.0f, 0, 0.0f);
    }

    const auto partial = RpAnimBlendClumpGetMainPartialAssociation(ped->m_pRwClump);
    if (partial && partial->m_AnimId >= ANIM_ID_WALK) {
        state[2] = AnimationState::MakeBlend(partial->m_AnimId, partial->m_CurrentTime, partial->m_Speed, static_cast<uint8>(partial->m_AnimGroupId), partial->m_BlendAmount);
    } else {
        state[2] = AnimationState::MakeBlend(ANIM_ID_WALK, 0.0f, 0.0f, 0, 0.0f);
    }
}

// 0x45C940
void CReplay::StorePedUpdate(CPed* ped, uint8 index) {
    auto vehicleIdx = [ped]() {
        if (ped->IsInVehicle()) {
            return GetVehiclePool()->GetIndex(ped->GetVehicleIfInOne()) + 1;
        } else {
            return 0;
        }
    }();

    CStoredAnimationState animState{};
    StorePedAnimation(ped, animState);

    Record.Write<tReplayPedUpdateBlock>({
        .poolRef                  = index,
        .heading                  = (int8)(ped->m_fCurrentRotation * HEADING_COMPRESS_VALUE),
        .vehicleIndex             = (uint8)vehicleIdx,
        .animState                = animState,
        .matrix                   = CCompressedMatrixNotAligned::Compress(ped->GetMatrix()),
        .weaponModel              = (int16)ped->m_nWeaponModelId,
        .animGroup                = (uint16)ped->m_nAnimGroup,
        .contactSurfaceBrightness = (uint8)(ped->m_fContactSurfaceBrightness * 100.0f),
        .flags = {
            .isTalking            = (bool)(ped == FindPlayerPed() && gbFirstPersonRunThisFrame && ped->bIsTalking),
            .stillOnValidPoly     = (bool)ped->bStillOnValidPoly,
            .usesCollision        = (bool)ped->m_bUsesCollision
        }
    });
}

// 0x45B4D0
void CReplay::RetrievePedAnimation(CPed* ped, const CStoredAnimationState& state) {
    // todo: refactor

    constexpr auto ANIM_TIME_COMPRESS_VALUE  = 63.75f;
    constexpr auto ANIM_SPEED_COMPRESS_VALUE = 85.0f;
    constexpr auto ANIM_BLEND_COMPRESS_VALUE = 127.5f;

    CAnimBlendAssociation* anim = nullptr;
    if (auto first = state[0]; first.m_nAnimId > 3u) {
        auto animBlock = CAnimManager::GetAssocGroups()[first.m_nGroupId1].m_AnimBlock;
        if (animBlock && animBlock->IsLoaded) {
            anim = CAnimManager::BlendAnimation(ped->m_pRwClump, (AssocGroupId)first.m_nGroupId1, (AnimationId)first.m_nAnimId, 100.0f);
        } else {
            anim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_WALK, 100.0f);
        }
    } else {
        anim = CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, (AnimationId)first.m_nAnimId, 100.0f);
    }
    anim->SetCurrentTime(state[0].m_nTime / ANIM_TIME_COMPRESS_VALUE);
    anim->SetSpeed(state[0].m_nSpeed / ANIM_SPEED_COMPRESS_VALUE);
    anim->SetBlend(1.0f, 1.0f);
    anim->m_nCallbackType = ANIM_BLEND_CALLBACK_NONE;

    anim = nullptr;
    if (auto second = state[1]; second.m_nGroupId1 && second.m_nAnimId && CAnimManager::GetAssocGroups()[second.m_nGroupId2].m_NumAnims > 0) {
        if (second.m_nAnimId > 3u) {
            anim = CAnimManager::BlendAnimation(ped->m_pRwClump, (AssocGroupId)second.m_nGroupId2, (AnimationId)second.m_nAnimId, 100.0f);
        } else {
            anim = CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, (AnimationId)second.m_nAnimId, 100.0f);
        }

        if (anim) {
            anim->SetCurrentTime(second.m_nTime / ANIM_TIME_COMPRESS_VALUE);
            anim->SetSpeed(second.m_nSpeed / ANIM_SPEED_COMPRESS_VALUE);
            anim->SetBlend(second.m_nGroupId1 / ANIM_BLEND_COMPRESS_VALUE, 1.0f);
            anim->m_nCallbackType = ANIM_BLEND_CALLBACK_NONE;
        }
    }

    RpAnimBlendClumpRemoveAssociations(ped->m_pRwClump, ANIMATION_IS_PARTIAL);
    if (auto third = state[2]; third.m_nGroupId1 && third.m_nAnimId) {
        if (/*third.m_nGroupId1 >= 0 &&*/ third.m_nAnimId != 3u) {
            if (auto animBlock = CAnimManager::GetAssocGroups()[third.m_nGroupId2].m_AnimBlock; animBlock && animBlock->IsLoaded) {
                anim = CAnimManager::BlendAnimation(ped->m_pRwClump, (AssocGroupId)third.m_nGroupId2, (AnimationId)third.m_nAnimId, 1000.0f);

                anim->SetCurrentTime(third.m_nTime / ANIM_TIME_COMPRESS_VALUE);
                anim->SetSpeed(third.m_nSpeed / ANIM_SPEED_COMPRESS_VALUE);
                anim->SetBlend(third.m_nGroupId1 / ANIM_BLEND_COMPRESS_VALUE, 0.0f);
                anim->m_nCallbackType = ANIM_BLEND_CALLBACK_NONE;
            }
        }
    }

}

// 0x45C210
void CReplay::Display() {
    if (ShouldStandardCameraBeProcessed())
        return;

    // NOTSA: FPS-independent 'REPLAY' text counter.
    static uint32 s_ReplayTextCounter = CTimer::GetTimeInMS();
    static bool s_ShowReplayText = true;
    if (CTimer::GetTimeInMS() - s_ReplayTextCounter >= 1000) {
        s_ReplayTextCounter = CTimer::GetTimeInMS();
        s_ShowReplayText = !s_ShowReplayText;
    }

    if (!s_ShowReplayText)
        return;

    CFont::SetScale(SCREEN_SCALE_X(1.5f), SCREEN_SCALE_Y(1.5f));
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetBackground(false, false);
    CFont::SetCentreSize(float(RsGlobal.maximumWidth - 20));
    CFont::SetProportional(true);
    CFont::SetColor({ 255u, 255u, 200u, 200u });
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    CFont::PrintString(float(RsGlobal.maximumWidth / 10), float(RsGlobal.maximumHeight / 15), TheText.Get("REPLAY"));
}

// 0x45D430
void CReplay::MarkEverythingAsNew() {
    for (auto i = 0; i < GetPedPool()->GetSize(); i++) {
        if (auto ped = GetPedPool()->GetAt(i)) {
            ped->bHasAlreadyBeenRecorded = false;
        }
    }

    for (auto i = 0; i < GetVehiclePool()->GetSize(); i++) {
        if (auto vehicle = GetVehiclePool()->GetAt(i)) {
            vehicle->vehicleFlags.bHasAlreadyBeenRecorded = false;
        }
    }
}

// 0x45EC90
void CReplay::EmptyReplayBuffer() {
    if (Mode == MODE_PLAYBACK)
        return;

    rng::fill(BufferStatus, REPLAYBUFFER_NOT_AVAILABLE);
    BufferStatus[0] = REPLAYBUFFER_IN_USE;
    Buffers[0].Write<tReplayEndBlock>(0u);
    Record = CAddressInReplayBuffer(Buffers[0]);
    MarkEverythingAsNew();
}

// 0x45D390
void CReplay::EmptyPedsAndVehiclePools_NoDestructors() {
    for (auto i = 0; i < GetVehiclePool()->GetSize(); i++) {
        if (auto vehicle = GetVehiclePool()->GetAt(i)) {
            CWorld::Remove(vehicle);
        }
    }

    for (auto i = 0; i < GetPedPool()->GetSize(); i++) {
        if (auto ped = GetPedPool()->GetAt(i)) {
            CWorld::Remove(ped);

            if (auto shadow = ped->m_pShadowData) {
                g_realTimeShadowMan.ReturnRealTimeShadow(shadow);
            }
        }
    }
}

// 0x45E2A0
void CReplay::GoToNextBlock() {
    Record.Write<tReplayEndBlock>();
    BufferStatus[Record.m_bSlot] = REPLAYBUFFER_FULL;

    Record.Next();
    Record.Write<tReplayEndBlock>();
    BufferStatus[Record.m_bSlot] = REPLAYBUFFER_IN_USE;

    MarkEverythingAsNew();
}

// 0x45EBB0
void CReplay::RecordVehicleDeleted(CVehicle* vehicle) {
    if (Mode != MODE_RECORD)
        return;

    if (Record.m_nOffset + FindSizeOfPacket(REPLAY_PACKET_DELETED_VEH) > 99'984) {
        GoToNextBlock();
    }

    Record.Write<tReplayDeletedVehicleBlock>({.poolRef = (int16)GetVehiclePool()->GetIndex(vehicle)});
    Record.Write<tReplayEndBlock>();
}

// 0x45EC20
void CReplay::RecordPedDeleted(CPed* ped) {
    if (Mode != MODE_RECORD)
        return;

    if (Record.m_nOffset + FindSizeOfPacket(REPLAY_PACKET_DELETED_PED) > 99'984) {
        GoToNextBlock();
    }

    Record.Write<tReplayDeletedPedBlock>({.poolRef = (int16)GetPedPool()->GetIndex(ped)});
    Record.Write<tReplayEndBlock>();
}

// 0x45EFA0
void CReplay::InitialiseVehiclePoolConversionTable() {
    rng::fill(m_VehiclePoolConversion, -1);
    auto convIdx = 0u;
    for (auto poolIdx = 0; poolIdx < GetVehiclePool()->GetSize(); poolIdx++) {
        if (GetVehiclePool()->GetAt(poolIdx))
            continue;

        if (!IsThisVehicleUsedInRecording(convIdx)) {
            while (++convIdx <= std::size(m_VehiclePoolConversion) && !IsThisVehicleUsedInRecording(convIdx))
                ;

            if (convIdx >= std::size(m_VehiclePoolConversion))
                break;
        }

        m_VehiclePoolConversion[convIdx++] = poolIdx;
    }
}

// 0x45EF20
void CReplay::InitialisePedPoolConversionTable() {
    rng::fill(m_PedPoolConversion, -1);
    auto convIdx = 0u;
    for (auto poolIdx = 0; poolIdx < GetPedPool()->GetSize(); poolIdx++) {
        if (GetPedPool()->GetAt(poolIdx))
            continue;

        if (!IsThisPedUsedInRecording(convIdx)) {
            while (++convIdx <= std::size(m_PedPoolConversion) && !IsThisPedUsedInRecording(convIdx))
                ;

            if (convIdx >= std::size(m_PedPoolConversion))
                break;
        }

        m_PedPoolConversion[convIdx++] = poolIdx;
    }
}

// 0x45F370
void CReplay::InitialisePoolConversionTables() {
    InitialiseVehiclePoolConversionTable();
    InitialisePedPoolConversionTable();
}

// 0x45C340
void CReplay::SaveReplayToHD() {
    CFileMgr::SetDirMyDocuments();

    if (auto file = CFileMgr::OpenFileForWriting("replay.rep")) {
        CFileMgr::Write(file, "GtaSA29", 8u);

        // TODO: Refactor
        const auto NextSlot = [](uint8 slot) { return (slot + 1) % NUM_REPLAY_BUFFERS; };

        auto inUse = std::distance(BufferStatus.begin(), rng::find(BufferStatus, REPLAYBUFFER_IN_USE));
        auto slot = NextSlot(inUse);

        for (auto status = BufferStatus[slot]; status != REPLAYBUFFER_IN_USE; status = BufferStatus[slot]) {
            if (status == REPLAYBUFFER_FULL)
                break;
            slot = NextSlot(slot);
        }
        assert(0 <= slot && slot <= std::size(Buffers));
        CFileMgr::Write(file, Buffers[slot].buffer.data(), sizeof(tReplayBuffer));

        while (BufferStatus[slot] != REPLAYBUFFER_IN_USE) {
            slot = NextSlot(slot);
            CFileMgr::Write(file, Buffers[slot].buffer.data(), sizeof(tReplayBuffer));
        }
        CFileMgr::CloseFile(file);
    }
    CFileMgr::SetDir("");
}

// 0x460390
void CReplay::PlayReplayFromHD() {
    CFileMgr::SetDirMyDocuments();
    if (auto file = CFileMgr::OpenFile("replay.rep", "rb")) {
        CFileMgr::Read(file, gString, 8u);

        if (strncmp(gString, "GtaSA29", 8u) != 0) {
            DEV_LOG("Invalid replay file data, header unmatch (='{}')", std::string_view{gString, 8u});
        } else {
            auto bufferIdx = 0u;
            for (; bufferIdx < NUM_REPLAY_BUFFERS && CFileMgr::Read(file, Buffers[bufferIdx].buffer.data(), sizeof(tReplayBuffer)); bufferIdx++) {
                BufferStatus[bufferIdx] = REPLAYBUFFER_FULL;
            }
            assert(bufferIdx >= 1);
            BufferStatus[bufferIdx - 1] = REPLAYBUFFER_IN_USE; // Mark last used buffer as in-use.

            for (auto i = bufferIdx; i < NUM_REPLAY_BUFFERS; i++) { // Mark unfilled buffer as n/a.
                BufferStatus[i] = REPLAYBUFFER_NOT_AVAILABLE;
            }

            CFileMgr::SetDir(""); // FileMgr dir should be resetted to root before the TriggerPlayback call.
            TriggerPlayback(REPLAY_CAM_MODE_AS_STORED, CVector{}, false);
            bPlayingBackFromFile = true;
            bAllowLookAroundCam = true;
            StreamAllNecessaryCarsAndPeds();
        }
        CFileMgr::CloseFile(file);
    }
    CFileMgr::SetDir("");
}

// 0x45C440
bool CReplay::ShouldStandardCameraBeProcessed() {
    return Mode != MODE_PLAYBACK;
}

// inlined
// 0x45C450
int32 CReplay::FindPoolIndexForPed(int32 index) {
    return m_PedPoolConversion[index];
}

// inlined
// 0x45C460
int32 CReplay::FindPoolIndexForVehicle(int32 index) {
    return m_VehiclePoolConversion[index];
}

// 0x45CA70
void CReplay::ProcessPedUpdate(CPed* ped, float interpValue, CAddressInReplayBuffer& address) {
    assert(address.Read<tReplayBlockBase>().type == REPLAY_PACKET_PED_UPDATE);
    const auto& packet = address.Read<tReplayPedUpdateBlock>();

    address.m_nOffset += FindSizeOfPacket(REPLAY_PACKET_PED_UPDATE); // count packet read beforehand.
    if (!ped)
        return;

    ped->m_fCurrentRotation = (float)packet.heading / HEADING_COMPRESS_VALUE;
    ped->m_fAimingRotation  = (float)packet.heading / HEADING_COMPRESS_VALUE;

    ped->GetMatrix() = Lerp(ped->GetMatrix(), CCompressedMatrixNotAligned::Decompress(packet.matrix), interpValue);

    if (const auto vehIdx = packet.vehicleIndex) {
        auto& vehicle = ped->m_pVehicle;
        CEntity::SafeCleanUpRef(ped->m_pVehicle);
        vehicle = nullptr;

        if (auto poolRef = FindPoolIndexForVehicle(vehIdx - 1); !GetVehiclePool()->IsFreeSlotAtIndex(poolRef)) {
            vehicle = GetVehiclePool()->GetAt(poolRef);
            vehicle->RegisterReference(vehicle);
            ped->bInVehicle = true;
        }
    } else {
        CEntity::SafeCleanUpRef(ped->m_pVehicle);
        ped->m_pVehicle = nullptr;
        ped->bInVehicle = false;
    }
    if (const auto ag = packet.animGroup; ag != ped->m_nAnimGroup) {
        ped->m_nAnimGroup = static_cast<AssocGroupId>(ag);
        if (ped == FindPlayerPed()) {
            ped->AsPlayer()->ReApplyMoveAnims();
        }
    }

    ped->bIsTalking = packet.flags.isTalking;
    ped->bStillOnValidPoly = packet.flags.stillOnValidPoly;
    ped->m_bUsesCollision = packet.flags.usesCollision;
    ped->m_fContactSurfaceBrightness = static_cast<float>(packet.contactSurfaceBrightness) / 100.0f;
    RetrievePedAnimation(ped, packet.animState);

    ped->RemoveWeaponModel();
    if (const auto wm = packet.weaponModel; wm != -1) {
        if (CStreaming::ms_aInfoForModel[wm].IsLoaded()) {
            ped->AddWeaponModel(wm);
        } else {
            CStreaming::RequestModel(static_cast<eModelID>(wm), STREAMING_DEFAULT);
        }
    }

    ped->m_nAreaCode = static_cast<eAreaCodes>(CGame::currArea);
    CWorld::Remove(ped);
    CWorld::Add(ped);
}

// 0x45D060
void CReplay::ProcessReplayCamera() {
    assert(TheCamera.m_pRwCamera);

    auto modelling = TheCamera.GetRwMatrix();
    switch (CameraMode) {
    case REPLAY_CAM_MODE_TOPDOWN:
        TheCamera.GetRight()    = CVector{1.0f, 0.0f,  0.0f};
        TheCamera.GetUp()       = CVector{0.0f, 1.0f,  0.0f};
        TheCamera.GetForward()  = CVector{0.0f, 0.0f, -1.0f};
        TheCamera.GetPosition() = CVector{CameraFocus.x, CameraFocus.y, CameraFocus.z + 15.0f};

        modelling->pos   = CameraFocus;
        modelling->at    = TheCamera.GetForward();
        modelling->up    = TheCamera.GetUp();
        modelling->right = TheCamera.GetRight();
        break;
    case REPLAY_CAM_MODE_FIXED:
        const auto direction = (CameraFocus - CameraFixed).Normalized();
        const auto right = direction.Cross({0.0f, 0.0f, 1.0f}).Normalized();

        TheCamera.GetPosition() = CameraFixed;
        TheCamera.GetForward()  = direction;
        TheCamera.GetUp()       = direction.Cross(right);
        TheCamera.GetRight()    = right;

        modelling->pos   = CameraFocus;
        modelling->at    = TheCamera.GetForward();
        modelling->up    = TheCamera.GetUp();
        modelling->right = TheCamera.GetRight();
        break;
    default:
        break;
    }
    TheCamera.m_vecGameCamPos = TheCamera.GetPosition();
    TheCamera.CalculateDerivedValues(false, true);
    RwMatrixUpdate(modelling);
    RwFrameUpdateObjects(RwCameraGetFrame(TheCamera.m_pRwCamera));
    RwFrameOrthoNormalize(RwCameraGetFrame(TheCamera.m_pRwCamera));
}

// 0x45D760
void CReplay::ProcessLookAroundCam() {
    if (bAllowLookAroundCam) {
        static float& playerCameraDistance = *(float*)0x97FAD4;
        static float& playerCameraDirAngle = *(float*)0x97FADC;
        static float& viewAngle = *(float*)0x97FAD8;

        const auto& pad = CPad::GetPad();
        auto steer = CVector2D{pad->NewMouseControllerState.m_AmountMoved.x / 200.0f, pad->NewMouseControllerState.m_AmountMoved.y / 200.0f};
        // steerX2 = steer.x;
        // steerY2 = steer.y;
        // if (v2 | v2)
        // if (steerX2 > 0.01f)
        //     goto LABEL_8
        // if (v6 | v7)
        //     steerY2 = -steerY2;
        // if (steerY2 > 0.01f)
        //     goto LABEL_8
        //

        if (steer.y > 0.01f) {
            if (!FramesActiveLookAroundCam) {
                const auto& camPos    = TheCamera.GetPosition();
                const auto& playerPos = FindPlayerCoors();
                playerCameraDistance = std::clamp(DistanceBetweenPoints(playerPos, camPos), 3.0f, 15.0f);
                playerCameraDirAngle = std::atan2(camPos.x - playerPos.x, camPos.y - playerPos.y);
            }
            FramesActiveLookAroundCam = 60;
        }

        static bool& s_FrameActiveLookAroundCamReset = *(bool*)0x97f66d;
        if (s_FrameActiveLookAroundCamReset) {
            FramesActiveLookAroundCam = 0;
        } else if (FramesActiveLookAroundCam) {
            playerCameraDirAngle += steer.x;
            FramesActiveLookAroundCam--;

            if (pad->NewMouseControllerState.isMouseLeftButtonPressed && pad->NewMouseControllerState.isMouseRightButtonPressed) {
                playerCameraDistance = std::clamp(playerCameraDistance + 2.0f * steer.y, 3.0f, 15.0f);
            } else {
                viewAngle = std::clamp(viewAngle + steer.y, 0.1f, 1.5f); // probably some kind of cheap clamping between [0, pi/2].
            }

            auto target = CVector{
                    std::sinf(playerCameraDirAngle) * std::cosf(viewAngle) * playerCameraDistance,
                    std::cosf(playerCameraDirAngle) * std::cosf(viewAngle) * playerCameraDistance,
                    std::sinf(viewAngle) * playerCameraDistance
            } + CameraFocus;

            CColPoint colPoint;
            CEntity* outEntity{};
            if (CWorld::ProcessLineOfSight(CameraFocus, target, colPoint, outEntity, true, false, false, false, false, true, true, false)) {
                target = colPoint.m_vecPoint;
                const auto focusCollisionDir = (CameraFocus - colPoint.m_vecPoint).Normalized();
                target.x += focusCollisionDir.x / 4.0f;
                target.y += focusCollisionDir.y / 4.0f;
                target.z += focusCollisionDir.z;
            }

            const auto forward = (CameraFocus - target).Normalized();
            const auto right   = CVector{0.0f, 0.0f, 1.0f}.Cross(forward);
            const auto up      = forward.Cross(right);

            TheCamera.GetForward()  = forward;
            TheCamera.GetUp()       = up;
            TheCamera.GetRight()    = right;
            TheCamera.GetPosition() = target;

            auto modelling = TheCamera.GetRwMatrix();
            modelling->pos   = target;
            modelling->at    = forward;
            modelling->up    = up;
            modelling->right = right;
            TheCamera.CalculateDerivedValues(false, true);
            RwMatrixUpdate(modelling);
            RwFrameUpdateObjects(RwCameraGetFrame(TheCamera.m_pRwCamera));
            RwFrameOrthoNormalize(RwCameraGetFrame(TheCamera.m_pRwCamera));
        }
    }

    RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.5f);
    if (FramesActiveLookAroundCam) {
        const auto MakeVisible = [](auto* entity) {
            if (entity) {
                entity->m_bIsVisible = true;
            }
        };

        MakeVisible(FindPlayerPed());
        MakeVisible(FindPlayerVehicle());
    }
}

// inlined
// 0x45C470
bool CReplay::CanWeFindPoolIndexForPed(int32 index) {
    return FindPoolIndexForPed(index) >= 0;
}

// inlined
// 0x45C490
bool CReplay::CanWeFindPoolIndexForVehicle(int32 index) {
    return FindPoolIndexForVehicle(index) >= 0;
}

// 0x45F020
void CReplay::StorePlayerInfoVariables() {
    auto& playerInfo = FindPlayerInfo(PED_TYPE_PLAYER1);

    PlayerInfo = playerInfo;
    playerInfo.m_PlayerData.m_pWanted = nullptr;
    playerInfo.m_PlayerData.m_pPedClothesDesc = nullptr;
    playerInfo.m_PlayerData.m_pArrestingCop = nullptr;
    playerInfo.m_PlayerData.m_nMeleeWeaponAnimReferenced = 0;
    playerInfo.m_PlayerData.m_nMeleeWeaponAnimReferencedExtra = 0;
    playerInfo.m_pSkinTexture = nullptr;
}

// 0x45E1F0
void CReplay::RestorePlayerInfoVariables() {
    FindPlayerInfo(PED_TYPE_PLAYER1) = PlayerInfo;
    PlayerInfo.m_PlayerData.m_pWanted = nullptr;
    PlayerInfo.m_PlayerData.m_pPedClothesDesc = nullptr;
    PlayerInfo.m_PlayerData.m_pArrestingCop = nullptr;
    PlayerInfo.m_pSkinTexture = nullptr;
}

// 0x45F180
void CReplay::StoreStuffInMem() {
    pStoredCam = new CCamera;
    memcpy(pStoredCam, &TheCamera, sizeof(CCamera));

    StorePlayerInfoVariables();
    Time1 = CTimer::GetTimeInMS();
    Time2 = CTimer::GetTimeInMSNonClipped();
    Time3 = CTimer::GetPreviousTimeInMS();
    Time4 = CTimer::m_snPPreviousTimeInMilliseconds;
    Time5 = CTimer::m_snPPPreviousTimeInMilliseconds;
    Time6 = CTimer::m_snPPPPreviousTimeInMilliseconds;
    Time7 = CTimer::GetTimeInMSPauseMode();
    Frame = CTimer::GetFrameCounter();
    TimeScale = CTimer::GetTimeScale();
    TimeStep = CTimer::GetTimeStep();
    TimeStepNonClipped = CTimer::GetTimeStepNonClipped();

    ClockHours = CClock::GetGameClockHours();
    ClockMinutes = CClock::GetGameClockMinutes();

    OldWeatherType = CWeather::OldWeatherType;
    NewWeatherType = CWeather::NewWeatherType;
    WeatherInterpolationValue = CWeather::InterpolationValue;

    ms_nNumCivMale_Stored = CPopulation::ms_nNumCivMale;
    ms_nNumCivFemale_Stored = CPopulation::ms_nNumCivFemale;
    ms_nNumCop_Stored = CPopulation::ms_nNumCop;
    ms_nNumEmergency_Stored = CPopulation::ms_nNumEmergency;
    ms_nNumDealers_Stored = CPopulation::ms_nNumDealers;
    ms_nTotalCarPassengerPeds_Stored = CPopulation::ms_nTotalCarPassengerPeds;
    ms_nTotalCivPeds_Stored = CPopulation::ms_nTotalCivPeds;
    ms_nTotalGangPeds_Stored = CPopulation::ms_nTotalGangPeds;
    ms_nTotalPeds_Stored = CPopulation::ms_nTotalPeds;
    ms_nTotalMissionPeds_Stored = CPopulation::ms_nTotalMissionPeds;
    rng::copy(CPopulation::ms_nNumGang, ms_nNumGang_Stored.begin());
}

// 0x45ECD0
void CReplay::RestoreStuffFromMem() {
    for (auto poolIdx = 0; poolIdx < GetPedPool()->GetSize(); poolIdx++) {
        if (auto ped = GetPedPool()->GetAt(poolIdx)) {
            if (ped->bUsedForReplay) {
                if (auto playerData = ped->m_pPlayerData) {
                    playerData->DeAllocateData();
                }
                CWorld::Remove(ped);
                delete ped;
            } else { // if not, restore it
                CWorld::Add(ped);
            }
        }
    }

    for (auto poolIdx = 0; poolIdx < GetVehiclePool()->GetSize(); poolIdx++) {
        if (auto veh = GetVehiclePool()->GetAt(poolIdx)) {
            if (veh->vehicleFlags.bUsedForReplay) {
                CWorld::Remove(veh);
                delete veh;
            } else { // if not, restore it
                CWorld::Add(veh);
            }
        }
    }

    memcpy(&TheCamera, pStoredCam, sizeof(TheCamera));
    delete pStoredCam;
    pStoredCam = nullptr;

    CTimer::SetTimeInMS(Time1);
    CTimer::SetTimeInMSNonClipped(Time2);
    CTimer::SetPreviousTimeInMS(Time3);
    CTimer::m_snPreviousTimeInMilliseconds = Time3;
    CTimer::m_snPPreviousTimeInMilliseconds = Time4;
    CTimer::m_snPPPreviousTimeInMilliseconds = Time5;
    CTimer::m_snPPPPreviousTimeInMilliseconds = Time6;
    CTimer::SetTimeInMSPauseMode(Time7);
    CTimer::SetTimeStep(TimeStep);
    CTimer::SetTimeStepNonClipped(TimeStepNonClipped);
    CTimer::SetTimeScale(TimeScale);
    CTimer::SetFrameCounter(Frame);
    CClock::SetGameClock(ClockHours, ++ClockMinutes, 0);

    RestorePlayerInfoVariables();
    CWeather::OldWeatherType = OldWeatherType;
    CWeather::NewWeatherType = NewWeatherType;
    CWeather::InterpolationValue = WeatherInterpolationValue;

    CGame::currArea = static_cast<int8>(CurrArea);
    CPopulation::ms_nNumCivMale = ms_nNumCivMale_Stored;
    CPopulation::ms_nNumCivFemale = ms_nNumCivFemale_Stored;
    CPopulation::ms_nNumCop = ms_nNumCop_Stored;
    CPopulation::ms_nNumEmergency = ms_nNumEmergency_Stored;
    CPopulation::ms_nTotalCarPassengerPeds = ms_nTotalCarPassengerPeds_Stored;
    CPopulation::ms_nTotalCivPeds = ms_nTotalCivPeds_Stored;
    CPopulation::ms_nTotalGangPeds = ms_nTotalGangPeds_Stored;
    CPopulation::ms_nTotalPeds = ms_nTotalPeds_Stored;
    CPopulation::ms_nTotalMissionPeds = ms_nTotalMissionPeds_Stored;

    // FIX_BUGS: ms_nNumGang not restored
    rng::copy(ms_nNumGang_Stored, CPopulation::ms_nNumGang.begin());
}

// 0x45F050
void CReplay::FinishPlayback() {
    if (ShouldStandardCameraBeProcessed())
        return;

    RestoreStuffFromMem();
    Mode = MODE_RECORD;

    if (bDoLoadSceneWhenDone) {
        CGame::currLevel = CTheZones::GetLevelFromPosition(LoadScene);
        CCollision::SortOutCollisionAfterLoad();
        CStreaming::LoadScene(LoadScene);
    }
    bDoLoadSceneWhenDone = false;

    if (bPlayingBackFromFile) {
        Init();
        MarkEverythingAsNew();
    }

    TheCamera.SetFadeColour(0, 0, 0);
    TheCamera.Fade(0.0f, eFadeFlag::FADE_IN);
    TheCamera.ProcessFade();
    TheCamera.Fade(1.0f, eFadeFlag::FADE_OUT);

    gFireManager.CreateAllFxSystems();
    AudioEngine.SetEffectsMasterVolume(FrontEndMenuManager.m_nSfxVolume);
    AudioEngine.SetMusicMasterVolume(FrontEndMenuManager.m_nRadioVolume);

    if (auto vehicle = FindPlayerVehicle()) {
        vehicle->m_vehicleAudio.TurnOnRadioForVehicle();
        AudioEngine.RetuneRadio(OldRadioStation);
    }
}

// 0x45E300
void CReplay::RecordThisFrame() {
    // Calculate the frame size beforehand.
    auto framePacketSize = 116u;
    for (auto i = 0; i < GetVehiclePool()->GetSize(); i++) {
        if (auto vehicle = GetVehiclePool()->GetAt(i); vehicle && vehicle->m_pRwObject) {
            switch (vehicle->m_nVehicleSubType) {
            case VEHICLE_TYPE_AUTOMOBILE:
            case VEHICLE_TYPE_MTRUCK:
            case VEHICLE_TYPE_QUAD:
            case VEHICLE_TYPE_BOAT:
            case VEHICLE_TYPE_TRAILER:
                framePacketSize += FindSizeOfPacket(REPLAY_PACKET_VEHICLE);
                break;
            case VEHICLE_TYPE_HELI:
            case VEHICLE_TYPE_BIKE:
            case VEHICLE_TYPE_BMX:
                framePacketSize += 56; // REPLAY_PACKET_BIKE/BMX/HELI
                break;
            case VEHICLE_TYPE_PLANE:
                framePacketSize += FindSizeOfPacket(REPLAY_PACKET_PLANE);
                break;
            case VEHICLE_TYPE_TRAIN:
                framePacketSize += FindSizeOfPacket(REPLAY_PACKET_TRAIN);
                break;
            default:
                break;
            }
        }
    }

    for (auto i = 0; i < GetPedPool()->GetSize(); i++) {
        if (auto ped = GetPedPool()->GetAt(i); ped && ped->m_pRwObject) {
            if (!ped->bHasAlreadyBeenRecorded) {
                // New ped!
                framePacketSize += FindSizeOfPacket(REPLAY_PACKET_PED_HEADER);
                if (ped->IsPlayer()) {
                    framePacketSize += FindSizeOfPacket(REPLAY_PACKET_CLOTHES);
                }
            }
            framePacketSize += FindSizeOfPacket(REPLAY_PACKET_PED_UPDATE);
        }
    }

    for (auto& trace : CBulletTraces::aTraces) {
        if (trace.m_bExists) {
            framePacketSize += FindSizeOfPacket(REPLAY_PACKET_BULLET_TRACES);
        }
    }

    if (Record.m_nOffset + framePacketSize + 20 > 99'984) {
        // writing the frame will overflow the current buffer, switch to next.
        GoToNextBlock();
    }

    auto cameraPacket = tReplayCameraBlock{
        .isUsingRemoteVehicle = FindPlayerInfo().m_pRemoteVehicle != nullptr,
        .matrix               = 0 /* to be filled */,
        .firstFocusPosn       = FindPlayerCoors()
    };
    cameraPacket.GetMatrix() = TheCamera.GetMatrix();
    Record.Write(cameraPacket);

    Record.Write<tReplayClockBlock>({
        .currentHour = CClock::GetGameClockHours(), .currentMinute = CClock::GetGameClockMinutes()
    });

    Record.Write<tReplayWeatherBlock>({
        .oldWeather  = (uint8)CWeather::OldWeatherType,
        .newWeather  = (uint8)CWeather::NewWeatherType,
        .interpValue = CWeather::InterpolationValue
    });

    Record.Write<tReplayTimerBlock>({.timeInMS = CTimer::GetTimeInMS()});

    for (auto i = 0; i < GetVehiclePool()->GetSize(); i++) {
        if (auto vehicle = GetVehiclePool()->GetAt(i); vehicle && vehicle->m_pRwObject) {
            switch (vehicle->m_nVehicleSubType) {
            case VEHICLE_TYPE_AUTOMOBILE:
            case VEHICLE_TYPE_MTRUCK:
            case VEHICLE_TYPE_QUAD:
            case VEHICLE_TYPE_BOAT:
            case VEHICLE_TYPE_TRAILER:
                Record.Write(tReplayVehicleBlock::MakeVehicleUpdateData(vehicle, i));
                break;
            case VEHICLE_TYPE_HELI: {
                tReplayHeliBlock packet{};
                *packet.As<tReplayVehicleBlock>() = tReplayVehicleBlock::MakeVehicleUpdateData(vehicle, i);

                packet.type = REPLAY_PACKET_HELI;
                packet.rotorSpeed = vehicle->AsHeli()->m_fHeliRotorSpeed;
                Record.Write(packet);
                break;
            }
            case VEHICLE_TYPE_PLANE: {
                tReplayPlaneBlock packet{};
                *packet.As<tReplayVehicleBlock>() = tReplayVehicleBlock::MakeVehicleUpdateData(vehicle, i);

                packet.type = REPLAY_PACKET_PLANE;
                packet.field_9C8 = vehicle->AsPlane()->field_9C8;
                packet.propSpeed = vehicle->AsPlane()->m_fPropSpeed;
                Record.Write(packet);
                break;
            }
            case VEHICLE_TYPE_TRAIN:
                Record.Write(tReplayTrainBlock::MakeTrainUpdateData(vehicle->AsTrain(), i));
                break;

            case VEHICLE_TYPE_BIKE:
                Record.Write(tReplayBikeBlock::MakeBikeUpdateData(vehicle->AsBike(), i));
                break;

            case VEHICLE_TYPE_BMX:
                Record.Write(tReplayBmxBlock::MakeBmxUpdateData(vehicle->AsBmx(), i));
                break;
            default:
                break;
            }
        }
    }

    for (auto i = 0; i < GetPedPool()->GetSize(); i++) {
        if (auto ped = GetPedPool()->GetAt(i); ped && ped->m_pRwObject) {
            if (!ped->bHasAlreadyBeenRecorded) {
                // New ped!
                const auto modelId = ped->m_nModelIndex;

                Record.Write<tReplayPedHeaderBlock>({
                    .poolRef = (uint8)i,
                    .modelId = (int16)((modelId >= MODEL_SPECIAL01 && modelId <= MODEL_CUTOBJ01) ? MODEL_MALE01 : modelId),
                    .pedType = (uint8)ped->m_nPedType
                });

                if (ped->IsPlayer()) {
                    tReplayClothesBlock clothesData{};
                    StoreClothesDesc(*ped->AsPlayer()->GetClothesDesc(), clothesData);
                    Record.Write(clothesData);
                }

                ped->bHasAlreadyBeenRecorded = true;
            }
            StorePedUpdate(ped, i);
        }
    }

    for (auto&& [i, trace] : notsa::enumerate(CBulletTraces::aTraces)) {
        if (!trace.m_bExists)
            continue;

        tReplayBulletTraceBlock packet{};
        packet.type = REPLAY_PACKET_BULLET_TRACES;
        packet.index = i;
        packet.start = trace.m_vecStart;
        packet.end = trace.m_vecEnd;

        Record.Write(packet);
    }

    Record.Write<tReplayMiscBlock>({
        .camShakeStart = TheCamera.m_nCamShakeStart,
        .camShakeForce = TheCamera.m_fCamShakeForce,
        .currArea      = (uint8)CGame::currArea,
        .camConfig     = {
            .videoCam = CSpecialFX::bVideoCam,
            .liftCam = CSpecialFX::bLiftCam
        }
    });

    Record.Write<tReplayEOFBlock>();
    Record.Write<tReplayEndBlock>();
}

// 0x45C750
void CReplay::StoreClothesDesc(const CPedClothesDesc& desc, tReplayClothesBlock& packet) {
    packet.type = REPLAY_PACKET_CLOTHES;
    rng::copy(desc.m_anModelKeys, packet.modelKeys.begin());
    rng::copy(desc.m_anTextureKeys, packet.textureKeys.begin());
    packet.fatStat = (int16)desc.m_fFatStat;
    packet.muscleStat = (int16)desc.m_fMuscleStat;
}

// 0x45C7D0
void CReplay::RestoreClothesDesc(CPedClothesDesc& desc, tReplayClothesBlock& packet) {
    assert(packet.type == REPLAY_PACKET_CLOTHES);
    rng::copy(packet.modelKeys, desc.m_anModelKeys.begin());
    rng::copy(packet.textureKeys, desc.m_anTextureKeys.begin());
    desc.m_fFatStat = (float)packet.fatStat;
    desc.m_fMuscleStat = (float)packet.muscleStat;
}

// 0x45CEA0
CPed* CReplay::DealWithNewPedPacket(const tReplayPedHeaderBlock& pedPacket, bool loadModel, tReplayClothesBlock* clothesPacket) {
    assert(pedPacket.type == REPLAY_PACKET_PED_HEADER);

    if (GetPedPool()->GetAt(FindPoolIndexForPed(pedPacket.poolRef)))
        return nullptr;

    if (pedPacket.pedType == PED_TYPE_PLAYER1 && loadModel) {
        CStreaming::RequestModel(pedPacket.modelId, STREAMING_DEFAULT);
        CStreaming::LoadAllRequestedModels(false);
    }

    if (CStreaming::GetInfo(pedPacket.modelId).IsLoaded()) {
        CPed* ped = nullptr;
        if (pedPacket.pedType == PED_TYPE_PLAYER1) {
            assert(clothesPacket && clothesPacket->type == REPLAY_PACKET_CLOTHES);

            ped = new (FindPoolIndexForPed(pedPacket.poolRef) << 8) CPlayerPed(PED_TYPE_PLAYER1, true);
            RestoreClothesDesc(*ped->GetClothesDesc(), *clothesPacket);
            CClothes::RebuildPlayer(static_cast<CPlayerPed*>(ped), true);
        } else if (!loadModel) {
            ped = new (FindPoolIndexForPed(pedPacket.poolRef) << 8) CCivilianPed((ePedType)pedPacket.pedType, pedPacket.modelId);
        } else {
            return nullptr;
        }

        ped->m_nStatus = STATUS_PLAYER_PLAYBACK_FROM_BUFFER;
        ped->bUsedForReplay = true;
        ped->GetMatrix().SetUnity();
        ped->SetCharCreatedBy(PED_GAME_MISSION);
        CWorld::Remove(ped);
        CWorld::Add(ped);
        return ped;
    } else {
        CStreaming::RequestModel(pedPacket.modelId, STREAMING_DEFAULT);
    }
    return nullptr;
}

// Returns true if the playback has ended.
// 0x45F380
bool CReplay::PlayBackThisFrameInterpolation(CAddressInReplayBuffer& buffer, float interpolation, uint32* outTimer) {
    const auto SetupVehicle = [](const tReplayVehicleBlock& packet, CVehicle* vehicle) {
        vehicle->m_nStatus = STATUS_PLAYER_PLAYBACK_FROM_BUFFER;
        vehicle->vehicleFlags.bUsedForReplay = true;
        vehicle->m_bUsesCollision = false;
        packet.matrix.DecompressIntoFullMatrix(vehicle->GetMatrix());
        vehicle->m_nPrimaryColor = packet.primaryColor;
        vehicle->m_nSecondaryColor = packet.secondaryColor;
        if (vehicle->IsSubBoat()) {
            vehicle->AsBoat()->m_nBoatFlags.bAnchored = false;
        }

        CWorld::Add(vehicle);
    };

    CBulletTraces::Init();

    while (buffer.Read<tReplayBlockBase>().type != REPLAY_PACKET_END_OF_FRAME) {
        auto& packet = buffer.Read<tReplayBlockBase>();

        switch (packet.type) {
        case REPLAY_PACKET_END:
            if (buffer.GetStatus() == REPLAYBUFFER_IN_USE) {
                FinishPlayback();
                return true;
            }
            buffer.Next();
            continue; // We do not increment when we read the end packet.
        case REPLAY_PACKET_VEHICLE: {
            auto vehiclePacket = buffer.Read<tReplayVehicleBlock>();
            const auto modelId = vehiclePacket.modelId;
            const auto poolIdx = FindPoolIndexForVehicle(vehiclePacket.poolRef);

            if (!GetVehiclePool()->GetAt(poolIdx)) {
                if (CStreaming::IsModelLoaded(modelId)) {
                    auto created = [&]() -> CVehicle* {
                        switch (vehiclePacket.vehicleSubType) {
                        case VEHICLE_TYPE_AUTOMOBILE:
                            return new (poolIdx << 8) CAutomobile(modelId, MISSION_VEHICLE, true);
                        case VEHICLE_TYPE_MTRUCK:
                            return new (poolIdx << 8) CMonsterTruck(modelId, MISSION_VEHICLE);
                        case VEHICLE_TYPE_QUAD:
                            return new (poolIdx << 8) CQuadBike(modelId, MISSION_VEHICLE);
                        case VEHICLE_TYPE_BOAT:
                            return new (poolIdx << 8) CBoat(modelId, MISSION_VEHICLE);
                        case VEHICLE_TYPE_TRAIN:
                            return new (poolIdx << 8) CTrain(modelId, MISSION_VEHICLE);
                        case VEHICLE_TYPE_TRAILER:
                            return new (poolIdx << 8) CTrailer(modelId, MISSION_VEHICLE);
                        default:
                            NOTSA_UNREACHABLE("Unknown vehicle subtype ={}", (int)vehiclePacket.vehicleSubType);
                        }
                    }();

                    SetupVehicle(vehiclePacket, created);
                } else {
                    CStreaming::RequestModel(modelId, STREAMING_DEFAULT);
                }
            } else {
                vehiclePacket.ExtractVehicleUpdateData(GetVehiclePool()->GetAt(poolIdx), interpolation);
            }
            break;
        }
        case REPLAY_PACKET_BIKE: {
            auto bikePacket = buffer.Read<tReplayBikeBlock>();
            const auto modelId = bikePacket.modelId;
            const auto poolIdx = FindPoolIndexForVehicle(bikePacket.poolRef);

            if (!GetVehiclePool()->GetAt(poolIdx)) {
                if (CStreaming::IsModelLoaded(modelId)) {
                    SetupVehicle(bikePacket, new (poolIdx << 8) CBike(modelId, MISSION_VEHICLE));
                } else {
                    CStreaming::RequestModel(modelId, STREAMING_DEFAULT);
                }
            } else {
                bikePacket.ExtractBikeUpdateData(GetVehiclePool()->GetAt(poolIdx)->AsBike(), interpolation);
            }
            break;
        }
        case REPLAY_PACKET_PED_HEADER:
            DealWithNewPedPacket(buffer.Read<tReplayPedHeaderBlock>(), false, nullptr);
            break;
        case REPLAY_PACKET_PED_UPDATE: {
            const auto poolIdx = FindPoolIndexForPed(buffer.Read<tReplayPedUpdateBlock>().poolRef);
            ProcessPedUpdate(GetPedPool()->GetAt(poolIdx), interpolation, buffer);
            continue; // ProcessPedUpdate increments the offset by itself already.
        }
        case REPLAY_PACKET_GENERAL: {
            auto cameraPacket = buffer.Read<tReplayCameraBlock>();

            TheCamera.GetMatrix() = Lerp(TheCamera.GetMatrix(), cameraPacket.GetMatrix(), interpolation);
            auto modelling = TheCamera.GetRwMatrix();
            modelling->pos = TheCamera.GetMatrix().GetPosition();
            modelling->at = TheCamera.GetMatrix().GetForward();
            modelling->up = TheCamera.GetMatrix().GetUp();
            modelling->right = TheCamera.GetMatrix().GetRight();

            CameraFocus = cameraPacket.firstFocusPosn * interpolation + CVector{ CameraFocus } * (1.0f - interpolation);
            bIsUsingRemoteCar = cameraPacket.isUsingRemoteVehicle;
            break;
        }
        case REPLAY_PACKET_CLOCK: {
            auto clockPacket = buffer.Read<tReplayClockBlock>();
            CClock::SetGameClock(clockPacket.currentHour, clockPacket.currentMinute, 0);
            break;
        }
        case REPLAY_PACKET_WEATHER: {
            auto weatherPacket = buffer.Read<tReplayWeatherBlock>();
            CWeather::OldWeatherType = (eWeatherType)weatherPacket.oldWeather;
            CWeather::NewWeatherType = (eWeatherType)weatherPacket.newWeather;
            CWeather::InterpolationValue = weatherPacket.interpValue;
            break;
        }
        case REPLAY_PACKET_END_OF_FRAME:
            break;
        case REPLAY_PACKET_TIMER: {
            auto timeInMS = buffer.Read<tReplayTimerBlock>().timeInMS;
            if (outTimer) {
                *outTimer = timeInMS;
            }

            CTimer::SetTimeInMS(timeInMS);
            break;
        }
        case REPLAY_PACKET_BULLET_TRACES: {
            auto bulletTracePacket = buffer.Read<tReplayBulletTraceBlock>();

            auto& trace = CBulletTraces::aTraces[bulletTracePacket.index];
            trace.m_bExists = true;
            trace.m_vecStart = bulletTracePacket.start;
            trace.m_vecEnd = bulletTracePacket.end;
            break;
        }
        case REPLAY_PACKET_MISC: {
            auto miscPacket = buffer.Read<tReplayMiscBlock>();

            TheCamera.m_nCamShakeStart = miscPacket.camShakeStart;
            TheCamera.m_fCamShakeForce = miscPacket.camShakeForce;
            CSpecialFX::bVideoCam = miscPacket.camConfig.videoCam;
            CSpecialFX::bLiftCam = miscPacket.camConfig.liftCam;
            CGame::currArea = miscPacket.currArea;
            break;
        }
        case REPLAY_PACKET_DELETED_VEH: {
            auto deletedVehPacket = buffer.Read<tReplayDeletedVehicleBlock>();
            if (!CanWeFindPoolIndexForVehicle(deletedVehPacket.poolRef))
                break;

            const auto idx = FindPoolIndexForVehicle(deletedVehPacket.poolRef);
            if (auto vehicle = GetVehiclePool()->GetAt(idx)) {
                CWorld::Remove(vehicle);
                delete vehicle;
            }
            break;
        }
        case REPLAY_PACKET_DELETED_PED: {
            auto deletedPedPacket = buffer.Read<tReplayDeletedPedBlock>();
            if (!CanWeFindPoolIndexForPed(deletedPedPacket.poolRef))
                break;

            const auto idx = FindPoolIndexForPed(deletedPedPacket.poolRef);
            if (auto ped = GetPedPool()->GetAt(idx)) {
                CWorld::Remove(ped);
                delete ped;
            }
            break;
        }
        case REPLAY_PACKET_BMX: {
            auto bmxPacket = buffer.Read<tReplayBmxBlock>();
            const auto modelId = bmxPacket.modelId;
            const auto poolIdx = FindPoolIndexForVehicle(bmxPacket.poolRef);

            if (!GetVehiclePool()->GetAt(poolIdx)) {
                if (CStreaming::IsModelLoaded(modelId)) {
                    SetupVehicle(bmxPacket, new (poolIdx << 8) CBmx(modelId, MISSION_VEHICLE));
                } else {
                    CStreaming::RequestModel(modelId, STREAMING_DEFAULT);
                }
            } else {
                // Originally BMX has an exclusive extractor for itself but it's exactly the
                // same with bike extractor, so it's fine to use bike one here.
                bmxPacket.As<tReplayBikeBlock>()->ExtractBikeUpdateData(GetVehiclePool()->GetAt(poolIdx)->AsBike(), interpolation);
            }
            break;
        }
        case REPLAY_PACKET_HELI: {
            auto heliPacket = buffer.Read<tReplayHeliBlock>();
            const auto modelId = heliPacket.modelId;
            const auto poolIdx = FindPoolIndexForVehicle(heliPacket.poolRef);

            if (!GetVehiclePool()->GetAt(poolIdx)) {
                if (CStreaming::IsModelLoaded(modelId)) {
                    SetupVehicle(heliPacket, new (poolIdx << 8) CHeli(modelId, MISSION_VEHICLE));
                } else {
                    CStreaming::RequestModel(modelId, STREAMING_DEFAULT);
                }
            } else {
                auto vehicle = GetVehiclePool()->GetAt(poolIdx);

                heliPacket.ExtractVehicleUpdateData(vehicle, interpolation);
                vehicle->AsHeli()->m_fHeliRotorSpeed = heliPacket.rotorSpeed;
            }
            break;
        }
        case REPLAY_PACKET_PLANE: {
            auto planePacket = buffer.Read<tReplayPlaneBlock>();
            const auto modelId = planePacket.modelId;
            const auto poolIdx = FindPoolIndexForVehicle(planePacket.poolRef);

            if (!GetVehiclePool()->GetAt(poolIdx)) {
                if (CStreaming::IsModelLoaded(modelId)) {
                    SetupVehicle(planePacket, new (poolIdx << 8) CPlane(modelId, MISSION_VEHICLE));
                } else {
                    CStreaming::RequestModel(modelId, STREAMING_DEFAULT);
                }
            } else {
                auto vehicle = GetVehiclePool()->GetAt(poolIdx);

                planePacket.ExtractVehicleUpdateData(vehicle, interpolation);
                vehicle->AsPlane()->field_9C8 = planePacket.field_9C8;
                vehicle->AsPlane()->m_fPropSpeed = planePacket.propSpeed;
            }
            break;
        }
        case REPLAY_PACKET_TRAIN: {
            auto trainPacket = buffer.Read<tReplayTrainBlock>();
            const auto modelId = trainPacket.modelId;
            const auto poolIdx = FindPoolIndexForVehicle(trainPacket.poolRef);

            if (!GetVehiclePool()->GetAt(poolIdx)) {
                if (CStreaming::IsModelLoaded(modelId)) {
                    SetupVehicle(trainPacket, new (poolIdx << 8) CTrain(modelId, MISSION_VEHICLE));
                } else {
                    CStreaming::RequestModel(modelId, STREAMING_DEFAULT);
                }
            } else {
                trainPacket.ExtractTrainUpdateData(GetVehiclePool()->GetAt(poolIdx)->AsTrain(), interpolation);
            }
            break;
        }
        case REPLAY_PACKET_CLOTHES:
            break;
        default:
            NOTSA_UNREACHABLE("Invalid packet read (type ID={})", (int32)packet.type);
        }

        buffer.m_nOffset += FindSizeOfPacket(packet.type);
    }
    buffer.m_nOffset += 4u;
    ProcessReplayCamera();

    return false;
}

// 0x460350
bool CReplay::FastForwardToTime(uint32 start) {
    if (!start)
        return true;

    uint32 timer = 0;
    while (!PlayBackThisFrameInterpolation(Playback, 1.0f, &timer)) {
        if (timer >= start) {
            return true;
        }
    }

    return false;
}

// 0x4604A0
void CReplay::PlayBackThisFrame() {
    if (PlayBackThisFrameInterpolation(Playback, 1.0f, nullptr)) {
        AudioEngine.SetEffectsMasterVolume(FrontEndMenuManager.m_nSfxVolume);
        AudioEngine.SetMusicMasterVolume(FrontEndMenuManager.m_nRadioVolume);
    } else {
        ProcessLookAroundCam();
        AudioEngine.SetEffectsMasterVolume(0);
        AudioEngine.SetMusicMasterVolume(0);
    }
}

// 0x45C850
constexpr uint32 CReplay::FindSizeOfPacket(eReplayPacket type) {
    switch (type) {
    case REPLAY_PACKET_END:          // <- actually 1 but we shouldn't call this func with this anyways so it's no problem.
    case REPLAY_PACKET_END_OF_FRAME:
    case REPLAY_PACKET_CLOCK:
    case REPLAY_PACKET_DELETED_VEH:
    case REPLAY_PACKET_DELETED_PED:
        return 4;
    case REPLAY_PACKET_VEHICLE:
    case REPLAY_PACKET_PED_UPDATE:
        return 52;
    case REPLAY_PACKET_BIKE:
    case REPLAY_PACKET_BMX:
    case REPLAY_PACKET_HELI:
        return 56;
    case REPLAY_PACKET_PED_HEADER:
    case REPLAY_PACKET_WEATHER:
    case REPLAY_PACKET_TIMER:
        return 8;
    case REPLAY_PACKET_GENERAL:
        return 88;
    case REPLAY_PACKET_BULLET_TRACES:
        return 28;
    case REPLAY_PACKET_PARTICLE:
        return 20;
    case REPLAY_PACKET_MISC:
        return 16;
    case REPLAY_PACKET_PLANE:
        return 60;
    case REPLAY_PACKET_TRAIN:
        return 76;
    case REPLAY_PACKET_CLOTHES:
        return 120;
    default:
        return 0;
    }
}

// 0x45DE40
bool CReplay::IsThisVehicleUsedInRecording(int32 index) {
    for (auto& buffer : Buffers) {
        for (auto& packet : buffer) {
            switch (packet.type) {
            case REPLAY_PACKET_VEHICLE:
            case REPLAY_PACKET_BIKE:
            case REPLAY_PACKET_BMX:
            case REPLAY_PACKET_HELI:
            case REPLAY_PACKET_PLANE:
            case REPLAY_PACKET_TRAIN:
                if (packet.As<tReplayVehicleBlock>()->poolRef == index) {
                    return true;
                }
                break;
            case REPLAY_PACKET_PED_UPDATE:
                if (auto pedIdx = packet.As<tReplayPedUpdateBlock>()->vehicleIndex; pedIdx && pedIdx - 1 == index) {
                    return true;
                }
                break;
            default:
                break;
            }
        }
    }
    return false;
}

// 0x45DDE0
bool CReplay::IsThisPedUsedInRecording(int32 index) {
    for (auto& buffer : Buffers) {
        const auto packet = rng::find_if(buffer, [index](auto&& p) {
            return p.type == REPLAY_PACKET_PED_HEADER && p.As<tReplayPedHeaderBlock>()->poolRef == index;
        });

        if (packet != buffer.end()) {
            return true;
        }
    }
    return false;
}

// 0x45D6C0
void CReplay::FindFirstFocusCoordinate(CVector& outPos) {
    for (auto& buffer : GetAllActiveBuffers()) {
        const auto packet = rng::find_if(buffer, [](auto&& p) { return p.type == REPLAY_PACKET_GENERAL; });
        if (packet != buffer.end()) {
            outPos = packet->As<tReplayCameraBlock>()->firstFocusPosn;
            break;
        }
    }
}

// 0x45D670
uint32 CReplay::NumberFramesAvailableToPlay() {
    auto frames = 0u;
    for (auto& buffer : GetAllActiveBuffers()) {
        for (auto it = buffer.begin(); it != buffer.end();) {
            it = std::find_if(it, buffer.end(), [](auto&& p) { return p.type == REPLAY_PACKET_END_OF_FRAME; });
            if (it != buffer.end())
                frames++;
        }
    }
    return frames;
}

// 0x45D4B0
void CReplay::StreamAllNecessaryCarsAndPeds() {
    for (auto& buffer : GetAllActiveBuffers()) {
        for (auto& packet : buffer) {
            switch (packet.type) {
            case REPLAY_PACKET_VEHICLE:
            case REPLAY_PACKET_BIKE:
                CStreaming::RequestModel(packet.As<tReplayVehicleBlock>()->modelId, STREAMING_DEFAULT);
                break;
            case REPLAY_PACKET_PED_HEADER:
                CStreaming::RequestModel(packet.As<tReplayPedHeaderBlock>()->modelId, STREAMING_DEFAULT);
                break;
            default:
                break;
            }
        }
    }
    CStreaming::LoadAllRequestedModels(false);
}

// 0x45D540
CPlayerPed* CReplay::CreatePlayerPed() {
    CPlayerPed* player = nullptr;

    auto i = 0;
    for (auto slot = Playback.GetNextSlot(i); i < NUM_REPLAY_BUFFERS && BufferStatus[slot]; slot = Playback.GetNextSlot(++i)) {
        auto& buffer = Buffers[slot];
        if (buffer.Read<tReplayBlockBase>(0).type == REPLAY_PACKET_END)
            break;

        for (auto& packet : buffer) {
            const auto offset = (size_t)((uint8*)&packet - (uint8*)&buffer);

            switch (packet.type) {
            case REPLAY_PACKET_PED_HEADER:
                if (!player) {
                    // get the next packet
                    auto next = buffer.Read<tReplayClothesBlock>(offset + FindSizeOfPacket(REPLAY_PACKET_PED_HEADER));
                    assert(next.type == REPLAY_PACKET_CLOTHES);

                    player = static_cast<CPlayerPed*>(DealWithNewPedPacket(*packet.As<tReplayPedHeaderBlock>(), true, &next));
                }
                break;
            case REPLAY_PACKET_PED_UPDATE:
                if (player && player == GetPedPool()->GetAt(FindPoolIndexForPed(packet.As<tReplayPedUpdateBlock>()->poolRef))) {
                    CAddressInReplayBuffer address(buffer, i, offset); // m_bSlot definition (i) is NOTSA
                    ProcessPedUpdate(player, 1.0f, address);
                    return player;
                }
            }
        }
    }
    return nullptr;
}

// 0x4600F0
void CReplay::TriggerPlayback(eReplayCamMode mode, CVector fixedCamPos, bool loadScene) {
    if (!ShouldStandardCameraBeProcessed())
        return;

    CSpecialFX::ReplayStarted();
    CameraMode = mode;
    Mode = MODE_PLAYBACK;
    CameraFixed = fixedCamPos;

    bPlayingBackFromFile = false;
    bAllowLookAroundCam = true;
    FramesActiveLookAroundCam = 0;

    OldRadioStation = [&]() -> eRadioID {
        if (FindPlayerVehicle()) {
            AudioEngine.StopRadio(nullptr, false);
            return AERadioTrackManager.GetCurrentRadioStationID();
        }

        return RADIO_EMERGENCY_AA; // Possibly not intended.
    }();

    CurrArea = CGame::currArea;

    // TODO: refactor
    auto idx = 7;
    for (auto&& [i, status] : notsa::enumerate(BufferStatus)) {
        if (status == REPLAYBUFFER_IN_USE) {
            idx = i;
            break;
        }
    }

    // TODO: refactor
    auto slot = (idx + 1) % 8;
    for (auto j = BufferStatus[slot]; j != REPLAYBUFFER_IN_USE && j != REPLAYBUFFER_FULL; j = BufferStatus[slot]) {
        slot = (slot + 1) % 8;
    }
    slot %= 8;
    assert(slot >= 0);
    Playback = CAddressInReplayBuffer(Buffers[slot], slot);

    CObject::DeleteAllTempObjectsInArea(CVector{0.0f}, 1'000'000.0f);
    StoreStuffInMem();
    InitialisePoolConversionTables();
    EmptyPedsAndVehiclePools_NoDestructors();
    CSkidmarks::Clear();
    StreamAllNecessaryCarsAndPeds();

    CWorld::Players[0].m_pPed = CreatePlayerPed();

    bDoLoadSceneWhenDone = [&] {
        if (!loadScene) {
            LoadScene = TheCamera.GetPosition();
            CVector point{};
            FindFirstFocusCoordinate(point);
            CGame::currLevel = CTheZones::GetLevelFromPosition(point);
            CCollision::SortOutCollisionAfterLoad();
            CStreaming::LoadScene(point);

            return true;
        }
        return false;
    }();

    CWeaponEffects::ClearCrossHairs();
    CExplosion::ClearAllExplosions();
    CPlaneBanners::Init();
    gFireManager.DestroyAllFxSystems();
    TheCamera.Restore();
    CDraw::SetFOV(70.0f);
    TheCamera.SetFadeColour(0, 0, 0);
    TheCamera.Fade(0.0f, eFadeFlag::FADE_IN);
    TheCamera.ProcessFade();
    TheCamera.Fade(1.5f, eFadeFlag::FADE_OUT);
}

CReplay::tReplayBuffer::Iterator& CReplay::tReplayBuffer::Iterator::operator++() {
#ifdef NOTSA_DEBUG
    if (m_offset >= REPLAY_BUFFER_SIZE || !m_buffer) {
        NOTSA_UNREACHABLE("Increment after end()!");
    }
#endif

    const auto type = m_buffer->Read<tReplayBlockBase>(m_offset).type;
    m_offset = (type != REPLAY_PACKET_END) ? m_offset + CReplay::FindSizeOfPacket(type) : REPLAY_BUFFER_SIZE;
    return *this;
}

#define VALIDATE_BLOCK(block) static_assert(sizeof(block) == CReplay::FindSizeOfPacket(block::Type), "Block size mismatch!")

VALIDATE_BLOCK(tReplayVehicleBlock);
VALIDATE_BLOCK(tReplayBikeBlock);
VALIDATE_BLOCK(tReplayPedHeaderBlock);
VALIDATE_BLOCK(tReplayPedUpdateBlock);
VALIDATE_BLOCK(tReplayCameraBlock);
VALIDATE_BLOCK(tReplayClockBlock);
VALIDATE_BLOCK(tReplayWeatherBlock);
VALIDATE_BLOCK(tReplayEOFBlock);
VALIDATE_BLOCK(tReplayTimerBlock);
VALIDATE_BLOCK(tReplayTimerBlock);
VALIDATE_BLOCK(tReplayBulletTraceBlock);
VALIDATE_BLOCK(tReplayParticleBlock);
VALIDATE_BLOCK(tReplayMiscBlock);
VALIDATE_BLOCK(tReplayDeletedVehicleBlock);
VALIDATE_BLOCK(tReplayDeletedPedBlock);
VALIDATE_BLOCK(tReplayBmxBlock);
VALIDATE_BLOCK(tReplayHeliBlock);
VALIDATE_BLOCK(tReplayPlaneBlock);
VALIDATE_BLOCK(tReplayTrainBlock);
VALIDATE_BLOCK(tReplayClothesBlock);
#undef VALIDATE_BLOCK
