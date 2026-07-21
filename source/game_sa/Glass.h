#pragma once

#include <rwcore.h>
#include <FallingGlassPane.h>
#include "Vector2D.h"
#include "Vector.h"

class CVehicle;
class CEntity;
class CObject;

enum class ePaneType : uint32 {
    DEFAULT = 0,
    DELAYED = 1,
    CAR     = 2
};

class CGlass {
public:
    static inline auto& PanePolyPositions = StaticRef<CVector2D[4][3]>(0x8D5CD8);
    static inline auto& ReflectionPolyVertexBaseIdx = StaticRef<int32>(0xC71B18);
    static inline auto& ReflectionPolyIndexBaseIdx = StaticRef<int32>(0xC71B1C);
    static inline auto& ReflectionPolyVertexBuffer = StaticRef<std::array<RxObjSpace3DLitVertex, 1706>>(0xC5B158);
    static inline auto& ShatteredPolyVertexBuffer = StaticRef<std::array<RxObjSpace3DLitVertex, 512>>(0xC56958);
    static inline auto& ShatteredVerticesBaseIdx = StaticRef<int32>(0xC71B20);
    static inline auto& ShatteredIndicesBaseIdx = StaticRef<int32>(0xC71B24);
    static inline auto& H1iLightPolyVerticesIdx = StaticRef<uint32>(0xC71B28);
    static inline auto& HiLightPolyIndicesIdx = StaticRef<int32>(0xC71B2C);
    static inline auto& PanePolyCenterPositions = StaticRef<std::array<CVector2D, 5>>(0xC71B30);
    static inline auto& apEntitiesToBeRendered = StaticRef<CEntity*[32]>(0xC71B58);
    static inline auto& NumGlassEntities = StaticRef<uint32>(0xC71BD8);
    static inline auto& aGlassPanes = StaticRef<std::array<CFallingGlassPane, 44>>(0xC71BF8);
    static inline auto& LastColCheckMS = StaticRef<uint32>(0xC72FA8);

public:
    static void InjectHooks();

    static void Init();
    static bool HasGlassBeenShatteredAtCoors(CVector point);
    static void CarWindscreenShatters(CVehicle* vehicle);
    static void WasGlassHitByBullet(CEntity* entity, CVector hitPos);
    static void WindowRespondsToCollision(CEntity* entity, float fDamageIntensity, CVector vecMoveSpeed, CVector vecPoint, bool max1PaneSection);
    static void GeneratePanesForWindow(ePaneType type, CVector pos, CVector fwd, CVector right, CVector velocity, CVector center, float velocityCenterDragCoeff,
                                       bool bShatter, bool numSectionsMax1, int32 numSections, bool unk);
    static void Update();
    static void Render();
    template<typename PtrListType>
    static void FindWindowSectorList(PtrListType& objList, float& outDist, CEntity*& outEntity, CVector point);
    static void RenderReflectionPolys();
    static void RenderShatteredPolys();
    static void RenderHiLightPolys();
    static bool IsObjectGlass(CEntity* entity);
    static uint8 CalcAlphaWithNormal(const CVector& normal);
    static void AskForObjectToBeRenderedInGlass(CEntity* entity);
    static CFallingGlassPane* FindFreePane();
    static void WindowRespondsToSoftCollision(CEntity* entity, float fDamageIntensity);
    static void BreakGlassPhysically(CVector pos, float radius);
    static void WindowRespondsToExplosion(CEntity* entity, Const CVector point);
};
