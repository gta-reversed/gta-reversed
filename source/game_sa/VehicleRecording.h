#pragma once

#include <functional>
#include <span>
#include "Vector.h"
#include "PathFind.h"

#include "extensions/FixedFloat.hpp"
#include "extensions/FixedVector.hpp"

class CVehicleStateEachFrame {
public:
    uint32 m_nTime;
    FixedVector<int16, 16383.5f> m_sVelocity;
    FixedVector<int8, 127.0f> m_bRight;
    FixedVector<int8, 127.0f> m_bTop;
    FixedFloat<uint8, 20.0f> m_bSteeringAngle;
    FixedFloat<uint8, 100.0f> m_bGasPedalPower;
    FixedFloat<uint8, 100.0f> m_bBreakPedalPower;
    bool m_bHandbrakeUsed;
    CVector m_vecPosn;
};
VALIDATE_SIZE(CVehicleStateEachFrame, 0x20);

constexpr auto TOTAL_VEHICLE_RECORDS = 16;

class CPath {
public:
    int32                   m_nNumber;
    CVehicleStateEachFrame* m_pData;
    int32                   m_nSize; // Byte size, use ::Size() to get number of elements!
    int8                    m_nRefCount;

    uint32 GetIndex() const;
    void   AddRef();
    void   RemoveRef();

    void Remove() {
        if (m_pData) {
            CMemoryMgr::Free(m_pData);
            m_pData = nullptr;
            CStreaming::RemoveModel(RRRToModelId(GetIndex()));
        }
    }

    size_t Size() const {
        return m_nSize / sizeof(CVehicleStateEachFrame);
    }

    auto GetFrames() {
        return std::span{m_pData, Size()};
    }
};
VALIDATE_SIZE(CPath, 0x10);

struct RwStream;

class CVehicleRecording {
public:
    static inline int32& NumPlayBackFiles = *(int32*)0x97F630;
    static inline std::array<CPath, TOTAL_RRR_MODEL_IDS>& StreamingArray = *(std::array<CPath, TOTAL_RRR_MODEL_IDS>*)0x97D880;
    static inline std::array<CVehicle*, TOTAL_VEHICLE_RECORDS>& pVehicleForPlayback = *(std::array<CVehicle*, TOTAL_VEHICLE_RECORDS>*)0x97D840;
    static inline std::array<CVehicleStateEachFrame*, TOTAL_VEHICLE_RECORDS>& pPlaybackBuffer = *(std::array<CVehicleStateEachFrame*, TOTAL_VEHICLE_RECORDS>*)0x97D800;
    static inline std::array<int32, TOTAL_VEHICLE_RECORDS>& PlaybackIndex = *(std::array<int32, TOTAL_VEHICLE_RECORDS>*)0x97D7C0;
    static inline std::array<int32, TOTAL_VEHICLE_RECORDS>& PlaybackBufferSize = *(std::array<int32, TOTAL_VEHICLE_RECORDS>*)0x97D780;
    static inline std::array<float, TOTAL_VEHICLE_RECORDS>& PlaybackRunningTime = *(std::array<float, TOTAL_VEHICLE_RECORDS>*)0x97D740;
    static inline std::array<float, TOTAL_VEHICLE_RECORDS>& PlaybackSpeed = *(std::array<float, TOTAL_VEHICLE_RECORDS>*)0x97D700;
    static inline std::array<bool, TOTAL_VEHICLE_RECORDS>& bPlaybackGoingOn = *(std::array<bool, TOTAL_VEHICLE_RECORDS>*)0x97D6F0;
    static inline std::array<bool, TOTAL_VEHICLE_RECORDS>& bPlaybackLooped = *(std::array<bool, TOTAL_VEHICLE_RECORDS>*)0x97D6E0;
    static inline std::array<bool, TOTAL_VEHICLE_RECORDS>& bPlaybackPaused = *(std::array<bool, TOTAL_VEHICLE_RECORDS>*)0x97D6D0;
    static inline std::array<bool, TOTAL_VEHICLE_RECORDS>& bUseCarAI = *(std::array<bool, TOTAL_VEHICLE_RECORDS>*)0x97D6C0;
    static inline std::array<uint32, 3>& PlayBackStreamingIndex = *(std::array<uint32, 3>*)0x97D670;
    // DisplayMode

public:
    static void InjectHooks();

    static void Init();
    static void InitAtStartOfGame();
    static void ShutDown();
    static void Render();

    static void ChangeCarPlaybackToUseAI(CVehicle* vehicle);
    static uint32 FindIndexWithFileNameNumber(int32 fileNumber);
    static void InterpolateInfoForCar(CVehicle* vehicle, const CVehicleStateEachFrame& frame, float interpValue);

    static bool HasRecordingFileBeenLoaded(int32 fileNumber);
    static void Load(RwStream* stream, int32 recordId, int32 totalSize);
    static void SmoothRecording(int32 recordId);

    static int32 RegisterRecordingFile(const char* name);
    static void RemoveRecordingFile(int32 fileNumber);
    static void RequestRecordingFile(int32 fileNumber);

    static void SetPlaybackSpeed(CVehicle* vehicle, float speed);
    static void RenderLineSegment(int32& numVertices);

    static void RemoveAllRecordingsThatArentUsed();
    static void RestoreInfoForCar(CVehicle* vehicle, const CVehicleStateEachFrame& frame, bool pause);
    static void RestoreInfoForMatrix(CMatrix& matrix, const CVehicleStateEachFrame& frame);
    static void SaveOrRetrieveDataForThisFrame();
    static void SetRecordingToPointClosestToCoors(int32 playbackId, CVector posn);
    static void SkipForwardInRecording(CVehicle* vehicle, float distance);
    static void SkipToEndAndStopPlaybackRecordedCar(CVehicle* vehicle);

    static void StopPlaybackWithIndex(int32 playbackId);
    static void StartPlaybackRecordedCar(CVehicle* vehicle, int32 fileNumber, bool useCarAI, bool looped);
    static void StopPlaybackRecordedCar(CVehicle* vehicle);
    static void PausePlaybackRecordedCar(CVehicle* vehicle);
    static void UnpausePlaybackRecordedCar(CVehicle* vehicle);

    static bool IsPlaybackGoingOnForCar(CVehicle* vehicle);
    static bool IsPlaybackPausedForCar(CVehicle* vehicle);

    // @notsa
    static std::span<CPath> GetRecordings() {
        return std::span{StreamingArray.data(), static_cast<size_t>(NumPlayBackFiles)};
    }

    static auto GetActivePlaybackIndices() {
        return rng::views::iota(0, TOTAL_VEHICLE_RECORDS) | std::views::filter([](auto&& i) { return bPlaybackGoingOn[i]; });
    }

    static CPath* FindRecording(int32 number) {
        if (!NumPlayBackFiles)
            return nullptr;

        const auto idx = FindIndexWithFileNameNumber(number);
        // FindIndexWithFileNameNumber returns zero if not found, but
        // can also return zero if the found path is at the first index.
        if (idx != 0 || StreamingArray[idx].m_nNumber == number) {
            return &StreamingArray[idx];
        }
        return nullptr;
    }

    static int32 FindVehicleRecordingIndex(CVehicle* vehicle) {
        for (const auto i : GetActivePlaybackIndices()) {
            if (pVehicleForPlayback[i] == vehicle) {
                return i;
            }
        }
        return -1;
    }

    static auto GetFramesFromPlaybackBuffer(size_t bufferIdx) {
        return std::span{pPlaybackBuffer[bufferIdx], PlaybackBufferSize[bufferIdx] / sizeof(CVehicleStateEachFrame)};
    }

    static auto GetCurrentFrameIndex(size_t bufferIdx) {
        return PlaybackIndex[bufferIdx] / sizeof(CVehicleStateEachFrame);
    }
};
