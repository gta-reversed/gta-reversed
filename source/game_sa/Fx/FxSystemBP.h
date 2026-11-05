/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "ListItem_c.h"

class FxSystem_c;
class FxPrimBP_c;
class FxSphere_c;

//! What a system does once its time passes the blueprint's length (`PLAYMODE:` in effects.fxp)
enum class eFxSystemPlayMode : uint8 {
    ONCE              = 0, //!< Stops (and is killed if `PlayAndKill` was used). The only mode that is never culled
    ONCE_HOLD         = 1, //!< Holds its time at the end
    LOOP              = 2, //!< Wraps around, waiting a random interval between loops if the blueprint has one
    ONCE_REPLAY_ON_IN = 3, //!< Stops, and plays again once the camera comes back within the cull distance
};

/* FX System BluePrint */

class FxSystemBP_c : public ListItem_c<FxSystemBP_c> {
public:
    uint32       m_nNameKey;
    float        m_fLength;
    float        m_fLoopIntervalMin;
    float        m_fLoopLength;
    uint16       m_nCullDist;
    eFxSystemPlayMode m_nPlayMode;
    uint8        m_nNumPrims;
    FxPrimBP_c** m_Prims;
    FxSphere_c*  m_BoundingSphere;

public:
    static void InjectHooks();

    FxSystemBP_c();
    ~FxSystemBP_c() = default; // 0x4AA0F0
    FxSystemBP_c* Constructor();
    FxSystemBP_c* Destructor();

    static void* operator new(unsigned size);
    static void  operator delete(void* data);

    void Load(Const char* filename, FILESTREAM file, int32 version);

    void Unload();
    void Update(float arg0);
    void Render(RwCamera* camera, float dayNightBalance, bool bHeatHaze);
    bool FreePrtFromSystem(FxSystem_c* system);
    FxSphere_c* GetBoundingSphere() const;
    void SetBoundingSphere(const CVector& center, float radius);

    // NOTSA
    [[nodiscard]] auto GetNameKey() const noexcept { return m_nNameKey; }
    auto GetPrims() { return std::span{ m_Prims, m_nNumPrims }; }
};

VALIDATE_SIZE(FxSystemBP_c, 0x24);
