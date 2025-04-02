#pragma once

#include "Vector2D.h"
#include "Vector.h"

class CVehicle;
class CEntity;
class CObject;
class CFallingGlassPane;

enum class ePaneType : uint32 {
    DEFAULT = 0,
    DELAYED = 1,
    CAR     = 2
};

class CGlass {
public:
    static CVector2D (&PanePolyPositions)[4][3];
    static int32& ReflectionPolyVertexBaseIdx;
    static int32& ReflectionPolyIndexBaseIdx;
    static RxObjSpace3DLitVertex(&ReflectionPolyVertexBuffer)[1706];
    static RxObjSpace3DLitVertex(&ShatteredPolyVertexBuffer)[512];
    static int32& ShatteredVerticesBaseIdx;
    static int32& ShatteredIndicesBaseIdx;
    static uint32& H1iLightPolyVerticesIdx;
    static int32& HiLightPolyIndicesIdx;
    static CVector2D (&PanePolyCenterPositions)[5];
    static CEntity*(&apEntitiesToBeRendered)[32];
    static inline uint32& NumGlassEntities = *(uint32*)0xC71BD8;
    static CFallingGlassPane (&aGlassPanes)[44];
    static uint32& LastColCheckMS;

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
    static uint8 CalcAlphaWithNormal(const CVector& normal);
    static void AskForObjectToBeRenderedInGlass(CEntity* entity);
    static CFallingGlassPane* FindFreePane();
    static void WindowRespondsToSoftCollision(CEntity* entity, float fDamageIntensity);
    static void BreakGlassPhysically(CVector pos, float radius);
    static void WindowRespondsToExplosion(CEntity* entity, Const CVector point);
};
