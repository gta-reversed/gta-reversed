/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "eVisibleArea.h"

class CColModel;
class CEntity;

class CGame {
public:
    //! Current number of area
    inline static char (&aDatFile)[32] = *reinterpret_cast<char (*)[32]>(0xB728EC);
    inline static int32& currLevel = *reinterpret_cast<int32*>(0xB7290C);
    inline static uint8& bMissionPackGame = *reinterpret_cast<uint8*>(0xB72910);
    inline static int32& currArea = *reinterpret_cast<int32*>(0xB72914);
    inline static RwMatrix*& m_pWorkingMatrix1 = *reinterpret_cast<RwMatrix**>(0xB72920);
    inline static RwMatrix*& m_pWorkingMatrix2 = *reinterpret_cast<RwMatrix**>(0xB72924);


public:
    static void InjectHooks();

    static bool CanSeeOutSideFromCurrArea();
    static bool CanSeeWaterFromCurrArea();

    static void GenerateTempPedAtStartOfNetworkGame();

    static bool Init1(const char* datFile);
    static bool Init2(const char* datFile);
    static bool Init3(const char* datFile);
    static void Initialise(char const *datFile);
    static bool InitialiseCoreDataAfterRW();
    static bool InitialiseEssentialsAfterRW();
    static void InitialiseOnceBeforeRW();
    static bool InitialiseRenderWare();
    static void InitialiseWhenRestarting();
    static void Process();
    static void ReInitGameObjectVariables();
    static void ReloadIPLs();

    static void ShutDownForRestart();
    static bool Shutdown();
    static void ShutdownRenderWare();
    static void DrasticTidyUpMemory(bool a1);
    static void FinalShutdown();
    static void TidyUpMemory(bool a1, bool clearD3Dmem);

    inline static bool IsInInterior() { return CGame::currArea != AREA_MAIN_MAP; };
};

inline static int32& gameTxdSlot = *reinterpret_cast<int32*>(0xB728E8);
inline static bool& gbLARiots = *reinterpret_cast<bool*>(0xB72958);
inline static bool& gbLARiots_NoPoliceCars = *reinterpret_cast<bool*>(0xB72959);
inline static std::array<CColModel, 2>& col1 = *reinterpret_cast<std::array<CColModel, 2>*>(0xC17824);

bool MoveMem(void **pMem);
bool MoveColModelMemory(CColModel &colModel, bool a2);
RpGeometry *MoveGeometryMemory(RpGeometry *geometry);
bool TidyUpModelInfo2(CEntity *entity, bool a2);
void ValidateVersion();
void D3DDeviceRestoreCallback();
bool DoHaspChecks();
