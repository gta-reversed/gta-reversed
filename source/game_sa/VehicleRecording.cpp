#include "StdInc.h"
#include <span>

#include "TimecycEditor.h"
#include "VehicleRecording.h"

#include "toolsmenu\DebugModules\CStreamingDebugModule.h"
#include "toolsmenu\DebugModules\CullZonesDebugModule.h"
#include "extensions\enumerate.hpp"

uint32 CPath::GetIndex() const {
    const auto index = this - CVehicleRecording::StreamingArray.data();
    assert(index >= 0 && index < TOTAL_VEHICLE_RECORDS);

    return index;
}

// VehicleRecording naming convention:
//
// `StreamingArray` contains 'recordings' that are typed CPath.
// It's indices are named `recordId`
// 
// CPath contains variable-sized frame array.
// It's 'number' is taken from the loaded file hence named `fileNumber`
// 
// `pPlaybackBuffer` contains 16 'recordings' that can be played
// simultaneously.
// It's incides are named `playbackId`

void CVehicleRecording::InjectHooks() {
    RH_ScopedClass(CVehicleRecording);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x459390);
    RH_ScopedInstall(InitAtStartOfGame, 0x45A1B0);
    RH_ScopedInstall(ShutDown, 0x459400);
    RH_ScopedInstall(Render, 0x459F70, {.locked = true});
    RH_ScopedInstall(ChangeCarPlaybackToUseAI, 0x45A360);
    RH_ScopedInstall(FindIndexWithFileNameNumber, 0x459FF0);
    RH_ScopedInstall(InterpolateInfoForCar, 0x459B30);
    RH_ScopedInstall(HasRecordingFileBeenLoaded, 0x45A060);
    RH_ScopedInstall(Load, 0x45A8F0);
    RH_ScopedInstall(SmoothRecording, 0x45A0F0);
    RH_ScopedInstall(RegisterRecordingFile, 0x459F80);
    RH_ScopedInstall(RemoveRecordingFile, 0x45A0A0);
    RH_ScopedInstall(RequestRecordingFile, 0x45A020);
    RH_ScopedInstall(StopPlaybackWithIndex, 0x459440);
    RH_ScopedInstall(StartPlaybackRecordedCar, 0x45A980);
    RH_ScopedInstall(StopPlaybackRecordedCar, 0x45A280);
    RH_ScopedInstall(PausePlaybackRecordedCar, 0x459740);
    RH_ScopedInstall(UnpausePlaybackRecordedCar, 0x459850);
    RH_ScopedInstall(SetPlaybackSpeed, 0x459660);
    RH_ScopedInstall(RenderLineSegment, 0x459F00);
    RH_ScopedInstall(RemoveAllRecordingsThatArentUsed, 0x45A160);
    RH_ScopedInstall(RestoreInfoForCar, 0x459A30);
    RH_ScopedInstall(RestoreInfoForMatrix, 0x459960);
    RH_ScopedInstall(SaveOrRetrieveDataForThisFrame, 0x45A610);
    RH_ScopedInstall(SetRecordingToPointClosestToCoors, 0x45A1E0);
    RH_ScopedInstall(IsPlaybackGoingOnForCar, 0x4594C0);
    RH_ScopedInstall(IsPlaybackPausedForCar, 0x4595A0);
    RH_ScopedInstall(SkipForwardInRecording, 0x459D10);
    RH_ScopedInstall(SkipToEndAndStopPlaybackRecordedCar, 0x45A4A0);
}

// 0x459390
void CVehicleRecording::Init() {
    rng::fill(bPlaybackGoingOn, false);
    rng::fill(bPlaybackPaused, false);
    rng::fill(pPlaybackBuffer, nullptr);
    rng::fill(pVehicleForPlayback, nullptr);

    for (auto& recording : StreamingArray) {
        recording.m_pData = nullptr;
        recording.m_nRefCount = 0;
    }
}

// 0x45A1B0
void CVehicleRecording::InitAtStartOfGame() {
    for (auto& recording : StreamingArray) {
        recording.m_pData = nullptr;
    }
    Init();
}

// 0x459400
void CVehicleRecording::ShutDown() {
    rng::for_each(StreamingArray, &CPath::Remove);
}

// 0x459F70 hook not needed
void CVehicleRecording::Render() {
// NOTSA: Originally an empty function, called late in rendering pipeline, used for debug stuff
#ifdef EXTRA_DEBUG_FEATURES
    TimeCycleDebugModule::ProcessRender();
    COcclusionDebugModule::ProcessRender();
    CStreamingDebugModule::ProcessRender();
    CullZonesDebugModule::ProcessRender();
#endif
}

// 0x45A360
void CVehicleRecording::ChangeCarPlaybackToUseAI(CVehicle* vehicle) {
    FindVehicleRecordingIndex(vehicle).and_then([vehicle](uint32 index) -> std::optional<uint32> {
        bUseCarAI[index] = true;

        vehicle->m_autoPilot.m_nCarMission = MISSION_FOLLOW_PRE_RECORDED_PATH;
        SetRecordingToPointClosestToCoors(index, vehicle->GetPosition());
        vehicle->physicalFlags.bDisableCollisionForce = false;
        vehicle->ProcessControlCollisionCheck(false);
        vehicle->m_nStatus = STATUS_PHYSICS;

        return {};
    });
}

// inlined
// 0x459FF0
uint32 CVehicleRecording::FindIndexWithFileNameNumber(int32 fileNumber) {
    for (auto&& [i, recording] : notsa::enumerate(GetRecordings())) {
        if (recording.m_nNumber == fileNumber) {
            return i;
        }
    }
    return 0;
}

// 0x459B30
void CVehicleRecording::InterpolateInfoForCar(CVehicle* vehicle, const CVehicleStateEachFrame& frame, float interpValue) {
    CMatrix transition;
    RestoreInfoForMatrix(transition, frame);

    vehicle->GetMatrix().Lerp(transition, interpValue);
    vehicle->GetMoveSpeed() = Lerp(vehicle->GetMoveSpeed(), frame.m_sVelocity, interpValue);
}

// 0x45A060
bool CVehicleRecording::HasRecordingFileBeenLoaded(int32 fileNumber) {
    const auto recording = FindRecording(fileNumber);
    return recording.has_value() && recording.value()->m_pData;
}

// 0x45A8F0
void CVehicleRecording::Load(RwStream* stream, int32 recordId, int32 totalSize) {
    const auto allocated = CMemoryMgr::Malloc(totalSize);
    StreamingArray[recordId].m_pData = static_cast<CVehicleStateEachFrame*>(allocated);
    const auto size = RwStreamRead(stream, allocated, 9'999'999u);
    StreamingArray[recordId].m_nSize = size;
    RwStreamClose(stream, nullptr);

#ifdef EXTRA_CARREC_LOGS
    DEV_LOG("Load carrec to streaming slot idx:{} (size={})", recordId, totalSize);
#endif

    for (auto&& [i, frame] : notsa::enumerate(StreamingArray[recordId].GetFrames())) {
        if (i != 0 && frame.m_nTime == 0) {
            // no valid frame that is not zeroth can have zero as a time.
            // so we count them as invalid and prune the following including itself.
#ifdef EXTRA_CARREC_LOGS
            DEV_LOG("\tRecording pruned at index {}", i);
#endif

            StreamingArray[recordId].m_nSize = i * sizeof(CVehicleStateEachFrame);
            break;
        }
    }
    SmoothRecording(recordId);
}

// 0x45A0F0
void CVehicleRecording::SmoothRecording(int32 recordId) {
    auto frames = StreamingArray[recordId].GetFrames();

    for (auto i = 4u; i < frames.size(); i++) {
        frames[i - 1].m_nTime = (uint32)((float)(frames[i].m_nTime + frames[i - 2].m_nTime) / 2.0f);
    }
}

// 0x459F80
int32 CVehicleRecording::RegisterRecordingFile(const char* name) {
    auto fileNumber = 850;
    if (sscanf(name, "carrec%d", &fileNumber) == 0) {
        RET_IGNORED(sscanf(name, "CARREC%d", &fileNumber));
    }

#ifdef EXTRA_CARREC_LOGS
    DEV_LOG("Registering carrec file '{}', (streamIdx={})", name, NumPlayBackFiles);
#endif

    StreamingArray[NumPlayBackFiles].m_nNumber = fileNumber;
    StreamingArray[NumPlayBackFiles].m_pData = nullptr;
    return NumPlayBackFiles++;
}

// 0x45A0A0
void CVehicleRecording::RemoveRecordingFile(int32 fileNumber) {
    FindRecording(fileNumber).and_then([](CPath* recording) -> std::optional<uint32> {
        if (recording->m_pData && !recording->m_nRefCount) {
            recording->Remove();
        }
        return {};
    });
}

// 0x45A020
void CVehicleRecording::RequestRecordingFile(int32 fileNumber) {
    auto index = 0;
    FindRecording(fileNumber).and_then([&index](CPath* recording) -> std::optional<uint32> {
        index = recording->GetIndex();
        recording->Remove();
        return {};
    });

    CStreaming::RequestModel(RRRToModelId(index), STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED);
}

// 0x459440
void CVehicleRecording::StopPlaybackWithIndex(int32 playbackId) {
    if (auto vehicle = pVehicleForPlayback[playbackId]) {
        vehicle->m_autoPilot.m_vehicleRecordingId = -1;
        pVehicleForPlayback[playbackId]->physicalFlags.bDisableCollisionForce = false;
    }
    pVehicleForPlayback[playbackId] = nullptr;
    pPlaybackBuffer[playbackId] = nullptr;
    PlaybackBufferSize[playbackId] = 0;
    bPlaybackGoingOn[playbackId] = false;

    StreamingArray[PlayBackStreamingIndex[playbackId]].RemoveRef();
}

// 0x45A980
void CVehicleRecording::StartPlaybackRecordedCar(CVehicle* vehicle, int32 fileNumber, bool useCarAI, bool looped) {
    const auto GetInactivePlaybackIndices = [] {
        return rng::views::iota(0, TOTAL_VEHICLE_RECORDS) | std::views::filter([](auto&& i) { return !bPlaybackGoingOn[i]; });
    };
    const auto recordId = FindIndexWithFileNameNumber(fileNumber);
    const auto playbackId = *GetInactivePlaybackIndices().begin();

    pVehicleForPlayback[playbackId] = vehicle;
    CEntity::RegisterReference(pVehicleForPlayback[playbackId]);
    bPlaybackLooped[playbackId] = looped;
    PlayBackStreamingIndex[playbackId] = recordId;
    pPlaybackBuffer[playbackId] = StreamingArray[recordId].m_pData;
    PlaybackBufferSize[playbackId] = StreamingArray[recordId].m_nSize;
    bUseCarAI[playbackId] = useCarAI;
    PlaybackIndex[playbackId] = 0;
    PlaybackRunningTime[playbackId] = 0.0f;
    PlaybackSpeed[playbackId] = 1.0f;
    bPlaybackGoingOn[playbackId] = true;
    bPlaybackPaused[playbackId] = false;
    StreamingArray[recordId].AddRef();
    
    if (useCarAI) {
        vehicle->m_autoPilot.m_nCarMission = MISSION_FOLLOW_PRE_RECORDED_PATH;
        SetRecordingToPointClosestToCoors(playbackId, vehicle->GetPosition());
    } else {
        vehicle->physicalFlags.bDisableCollisionForce = true;
        vehicle->physicalFlags.bCollidable = false;
    }
    vehicle->m_autoPilot.m_vehicleRecordingId = playbackId;
}

// 0x45A280
void CVehicleRecording::StopPlaybackRecordedCar(CVehicle* vehicle) {
    FindVehicleRecordingIndex(vehicle).and_then([](auto i) -> std::optional<uint32> { StopPlaybackWithIndex(i); return {}; });
}

// 0x459740
void CVehicleRecording::PausePlaybackRecordedCar(CVehicle* vehicle) {
    FindVehicleRecordingIndex(vehicle).and_then([](auto i) -> std::optional<uint32> { bPlaybackPaused[i] = true; return {}; });
}

// 0x459850
void CVehicleRecording::UnpausePlaybackRecordedCar(CVehicle* vehicle) {
    FindVehicleRecordingIndex(vehicle).and_then([](auto i) -> std::optional<uint32> { bPlaybackPaused[i] = false; return {}; });
}

// 0x459660
void CVehicleRecording::SetPlaybackSpeed(CVehicle* vehicle, float speed) {
    FindVehicleRecordingIndex(vehicle).and_then([speed](auto i) -> std::optional<uint32> { PlaybackSpeed[i] = speed; return {}; });
}

// [debug]
// 0x459F00
void CVehicleRecording::RenderLineSegment(int32& numVertices) {
    if (numVertices > 1) {
        for (int32 i = 0; i < numVertices - 1; i++) {
            aTempBufferIndices[2 * i] = i;
            aTempBufferIndices[2 * i + 1] = i + 1;
        }
        if (RwIm3DTransform(aTempBufferVertices, numVertices, nullptr, 0)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, aTempBufferIndices, (numVertices - 1) * 2);
            RwIm3DEnd();
        }
    }
    numVertices = 0;
}

// 0x45A160
void CVehicleRecording::RemoveAllRecordingsThatArentUsed() {
    for (auto&& [i, recording] : notsa::enumerate(GetRecordings())) {
        if (recording.m_nNumber == i && recording.m_pData && !recording.m_nRefCount) {
            recording.Remove();
        }
    }
}

// 0x459A30
void CVehicleRecording::RestoreInfoForCar(CVehicle* vehicle, const CVehicleStateEachFrame& frame, bool pause) {
    RestoreInfoForMatrix(vehicle->GetMatrix(), frame);
    vehicle->ResetTurnSpeed();
    vehicle->m_fSteerAngle = frame.m_bSteeringAngle;
    vehicle->m_fGasPedal   = frame.m_bGasPedalPower;
    vehicle->m_fBreakPedal = frame.m_bBreakPedalPower;
    vehicle->vehicleFlags.bIsHandbrakeOn = pause || frame.m_bHandbrakeUsed;

    if (pause) {
        vehicle->m_fSteerAngle = vehicle->m_fGasPedal = vehicle->m_fBreakPedal = 0.0f;
        vehicle->ResetMoveSpeed();
    }
}

// 0x459960
void CVehicleRecording::RestoreInfoForMatrix(CMatrix& matrix, const CVehicleStateEachFrame& frame) {
    matrix.GetRight()    = frame.m_bRight;
    matrix.GetForward()  = frame.m_bTop;
    matrix.GetUp()       = matrix.GetRight().Cross(matrix.GetForward());
    matrix.GetPosition() = frame.m_vecPosn;
}

// 0x45A610
void CVehicleRecording::SaveOrRetrieveDataForThisFrame() {
    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    for (auto i : GetActivePlaybackIndices()) {
        auto vehicle = pVehicleForPlayback[i];

        if (!vehicle || vehicle->physicalFlags.bDestroyed) {
            StopPlaybackWithIndex(i);
            continue;
        }
        if (bUseCarAI[i])
            continue;

        const auto delta = static_cast<float>(CTimer::GetTimeInMS() - CTimer::m_snPPPPreviousTimeInMilliseconds);
        const auto step = delta * PlaybackSpeed[i] / 4.0f;
        if (step > 500.0f) {
            DEV_LOG("That's a really big step (={:2f})\n", step);
        }
        PlaybackRunningTime[i] += step;

        const auto frames = GetFramesFromPlaybackBuffer(i);
        auto current = GetCurrentFrameIndex(i);

        // find the exact frame that matches the playback time.
        for (auto& next = frames[current + 1]; current + 1 < frames.size() && next.m_nTime < PlaybackRunningTime[i]; current++)
            ;
        // current can not be back from the current frame index.
        for (; current > GetCurrentFrameIndex(i) && frames[current].m_nTime > PlaybackRunningTime[i]; current--)
            ;
        PlaybackIndex[i] = current * sizeof(CVehicleStateEachFrame);

        if (current + 1 < frames.size()) {
            // current is not the last frame, so we interpolate with the next.
            const auto& frameCurrent = frames[current];
            const auto& frameNext = frames[current + 1];

            RestoreInfoForCar(vehicle, frameCurrent, false);

            const auto interp = (PlaybackRunningTime[i] - (float)frameCurrent.m_nTime) / (float)(frameNext.m_nTime - frameCurrent.m_nTime);
            InterpolateInfoForCar(vehicle, frameNext, interp);

            if (vehicle->IsSubTrain()) {
                vehicle->AsTrain()->FindPositionOnTrackFromCoors();
            }

            vehicle->ProcessControlCollisionCheck(false); // FIXME: crashes in car school - 'alley-oop'
            vehicle->RemoveAndAdd();
            vehicle->UpdateRW();
            vehicle->UpdateRwFrame();

            MarkSurroundingEntitiesForCollisionWithTrain(vehicle->GetPosition(), 5.0f, vehicle, true);
        } else if (bPlaybackLooped[i]) {
            // current is the last frame, set next frame to be processed to first frame cuz we're looping.
            PlaybackRunningTime[i] = 0.0f;
            PlaybackIndex[i] = 0;
        } else {
            // current is the last frame, farewell.
            StopPlaybackRecordedCar(vehicle);
        }
    }
}

// 0x45A1E0
void CVehicleRecording::SetRecordingToPointClosestToCoors(int32 playbackId, CVector posn) {
    auto minDist = 1'000'000.0f; // FLT_MAX
    for (auto&& [i, frame] : notsa::enumerate(GetFramesFromPlaybackBuffer(playbackId))) {
        if (const auto d = DistanceBetweenPoints(frame.m_vecPosn, posn); d < minDist) {
            PlaybackIndex[playbackId] = i;
            minDist = d;
        }
    }
}

// 0x4594C0
bool CVehicleRecording::IsPlaybackGoingOnForCar(CVehicle* vehicle) {
    return FindVehicleRecordingIndex(vehicle).has_value();
}

// 0x4595A0
bool CVehicleRecording::IsPlaybackPausedForCar(CVehicle* vehicle) {
    // SA code loops through all playbacks but always returns false.
    return false;
}

// unused
// 0x459D10
void CVehicleRecording::SkipForwardInRecording(CVehicle* vehicle, float distance) {
    // Not tested as it's unused.

    // NOTSA: Original code does OOB-access if no index found.
    FindVehicleRecordingIndex(vehicle).and_then([vehicle, distance](auto i) -> std::optional<uint32> {
        const auto frames = GetFramesFromPlaybackBuffer(i);
        auto index = GetCurrentFrameIndex(i);
        float pace = 0.0f;

        for (; pace < distance && index + 1 < frames.size(); index++) {
            pace += DistanceBetweenPoints2D(frames[index].m_vecPosn, frames[index + 1].m_vecPosn);
        }

        // if we overshoot, we try to get the closest but smaller than or equal the `distance` pace.
        for (; pace > distance && index > 1; index--) {
            pace -= DistanceBetweenPoints2D(frames[index].m_vecPosn, frames[index - 1].m_vecPosn);
        }

        PlaybackRunningTime[i] = static_cast<float>(frames[index].m_nTime);
        if (const auto usesAI = bUseCarAI[i]) {
            RestoreInfoForCar(vehicle, frames[index], false);
            vehicle->ProcessControlCollisionCheck(false);
        }

        return {};
    });
}

// unused
// 0x45A4A0
void CVehicleRecording::SkipToEndAndStopPlaybackRecordedCar(CVehicle* vehicle) {
    FindVehicleRecordingIndex(vehicle).and_then([vehicle](auto i) -> std::optional<uint32> {
        assert(!GetFramesFromPlaybackBuffer(i).empty());

        vehicle->physicalFlags.bCollidable = false;
        RestoreInfoForCar(vehicle, GetFramesFromPlaybackBuffer(i).back(), false);
        vehicle->ProcessControlCollisionCheck(false);
        pVehicleForPlayback[i] = nullptr;
        pPlaybackBuffer[i] = nullptr;
        PlaybackBufferSize[i] = 0;
        bPlaybackGoingOn[i] = false;
        vehicle->m_autoPilot.m_vehicleRecordingId = -1;

        StreamingArray[PlayBackStreamingIndex[i]].RemoveRef();

        return {};
    });
}
