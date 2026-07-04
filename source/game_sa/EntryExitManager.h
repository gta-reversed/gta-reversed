/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "Vector2D.h"

#include "Pool.h"

class QuadTreeNode;
class CEntryExit;
class CEntity;

enum ExitEnterState : int32 {
    EXIT_ENTER_STATE_0,
    EXIT_ENTER_STATE_1,
    EXIT_ENTER_STATE_2,
    EXIT_ENTER_STATE_3,
    EXIT_ENTER_STATE_4,
};

using CEntryExitsPool = CPool<CEntryExit>;
class CEntryExitManager {
public:
    static inline auto& ms_entryExitStack = StaticRef<CEntryExit*[4]>(0x96A720);
    static inline auto& ms_bBurglaryHousesEnabled = StaticRef<bool>(0x96A730);
    static inline auto& ms_oldAreaCode = StaticRef<int32>(0x96A734);
    static inline auto& ms_visibleEntityList = StaticRef<CEntity*[32]>(0x96A738);
    static inline auto& ms_entryExitStackPosn = StaticRef<uint32>(0x96A7C4); // No. of enex's on the stack
    static inline auto& ms_bDisabled = StaticRef<int8>(0x96A7C8);
    static inline auto& ms_exitEnterState = StaticRef<int32>(0x96A7CC); // TODO: Some kind of enum, seems to have values [0, 4]
    static inline auto& mp_Active = StaticRef<CEntryExit*>(0x96A7D4);
    static inline auto& ms_numVisibleEntities = StaticRef<int32>(0x96A7DC);

public:
    static void InjectHooks();

    static void Init();
    static void Shutdown();
    static void ShutdownForRestart();
    static void Update();

    static int32 AddOne(float entranceX, float entranceY, float entranceZ,
                        float entranceAngle, float entranceRangeX, float entranceRangeY,
                        float fUnused,
                        float exitX, float exitY, float exitZ,
                        float exitAngle, eAreaCodes area, CEntryExit::eFlags flags, int32 skyColor, int32 timeOn, int32 timeOff, int32 numberOfPeds, const char* name);
    static void AddEntryExitToStack(CEntryExit* entryExit);
    static void DeleteOne(int32 index);
    static CObject* FindNearestDoor(const CEntryExit& exit, float radius);
    static int32 FindNearestEntryExit(const CVector2D& position, float range, int32 ignoreArea);
    static void EnableBurglaryHouses(bool enable);
    static void GetPositionRelativeToOutsideWorld(CVector& pos);
    static void PostEntryExitsCreation();
    static void LinkEntryExit(CEntryExit* entryExit);
    static int32 GetEntryExitIndex(const char* name, uint16 enabledFlags, uint16 disabledFlags);
    static void ResetAreaCodeForVisibleObjects();
    static void SetAreaCodeForVisibleObjects();

    static void GotoEntryExitVC(const char* name);
    static void GotoNextEntryExit();
    static void GotoPreviousEntryExit();
    static void SetEnabledByName(const char* name, bool enable);
    static void SetEntryExitFlagWithIndex(int32 index, uint32 flag, bool enable);
    static bool WeAreInInteriorTransition();

    static bool Load();
    static bool Save();

    static CEntryExit* GetInSlot(int32 slot);
    static CEntryExitsPool* GetPool();

private:
    static void AddEnExToWorld(CEntryExit* enex);
};
