/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

struct EmissionInfo_t;
struct MovementInfo_t;
struct RenderInfo_t;
class FxInfo_c;

class FxInfoManager_c {
public:
    int32      m_nNumInfos;
    FxInfo_c** m_pInfos;
    int8       m_MovementOffset;
    int8       m_RenderOffset;
    char       _align[2]; // urgent

    // FxEmitterBP_c
    uint16     m_nLodStart;
    uint16     m_nLodEnd;
    bool       m_bHasFlatParticleEmitter;
    bool       m_bHasHeatHazeParticleEmitter;
    char       field_12;
    char       field_13;

public:
    static void InjectHooks();

    FxInfoManager_c() = default;
    ~FxInfoManager_c() = default; // 0x4A4950

    FxInfo_c* AddFxInfo(int32 info);
    void Load(FILESTREAM file, int32 version);

    void ProcessEmissionInfo(float currentTime, float mult, float totalTime, bool useConst, EmissionInfo_t* emissionInfo);
    void ProcessMovementInfo(float currentTime, float mult, float totalTime, float length, bool useConst, MovementInfo_t* movementInfo);
    void ProcessRenderInfo(float currentTime, float mult, float totalTime, float length, bool useConst, RenderInfo_t* renderInfo);

    auto GetInfos()       { return std::span{ m_pInfos, size_t(m_nNumInfos) }; }
    auto GetInfos() const { return std::span{ m_pInfos, size_t(m_nNumInfos) }; }

    auto GetEmissionInfos();
    auto GetMovementInfos();
    auto GetRenderInfos();
};
VALIDATE_SIZE(FxInfoManager_c, 0x14);
