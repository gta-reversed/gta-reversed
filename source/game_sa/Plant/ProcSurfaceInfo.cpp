#include "StdInc.h"

#include "ProcSurfaceInfo.h"
#include "ProcObjectMan.h"

void ProcSurfaceInfo_c::InjectHooks() {
    RH_ScopedClass(ProcSurfaceInfo_c);
    RH_ScopedCategory("Plant");

    RH_ScopedInstall(Init, 0x5A2EB0);
    RH_ScopedInstall(Exit, 0x5A3270);
    RH_ScopedInstall(AddObject, 0x5A32D0, {.reversed = false});
    RH_ScopedInstall(AddObjects, 0x5A3850, {.reversed = false});
}

// 0x5A2EB0
void ProcSurfaceInfo_c::Init(
    const char* surfaceType,
    const char* modelName,
    float spacing,
    float minDist,
    int32 minRot, int32 maxRot,
    float minScale, float maxScale,
    float minScaleZ, float maxScaleZ,
    float zOffsetMin, float zOffsetMax,
    bool align, uint8 useGrid
) {

    m_SurfaceId = g_surfaceInfos.GetSurfaceIdFromName(surfaceType);
    auto mi = CModelInfo::GetModelInfo(modelName, &m_ModelIndex); // todo: int16 cast here, why?
    if (!mi) {
        return;
    }

    minDist = std::max(minDist, 80.0f);

    m_fMinScale             = minScale;
    m_fMaxScale             = maxScale;
    m_fSpacing              = spacing;
    m_fMinScaleZ            = minScaleZ;
    m_fMaxScaleZ            = maxScaleZ;
    m_fOffsetMinZ           = zOffsetMin;
    m_fOffsetMaxZ           = zOffsetMax;
    m_Align                 = align;
    m_fSquaredSpacingRadius = 1.0f / (spacing * spacing);
    m_fSquaredMinDistance   = minDist * minDist;
    m_fMinRotInRadians      = DegreesToRadians(float(minRot));
    m_fMaxRotInRadians      = DegreesToRadians(float(maxRot));
    m_fUseGrid              = float(useGrid);
}

// 0x5A3270
void ProcSurfaceInfo_c::Exit() {
    for (auto it = m_Objects.GetHead(); it;) {
        auto* next = m_Objects.GetNext(it);

        m_Objects.RemoveItem(it);
        g_procObjMan.m_ObjectsList.AddItem(it);
        it->m_Obj->DeleteRwObject();
        CWorld::Remove(it->m_Obj);
        delete std::exchange(it->m_Obj, nullptr);

        it = next;
    }
}

// 0x5A32D0
void ProcSurfaceInfo_c::AddObject(CVector p1, CVector p2, bool a3) {
    plugin::CallMethod<0x5A32D0, ProcSurfaceInfo_c*, CVector, CVector, bool>(this, p1, p2, a3);
}

// 0x5A3850
int32 ProcSurfaceInfo_c::AddObjects(CPlantLocTri* plant) {
    return plugin::CallMethodAndReturn<int32, 0x5A3850, ProcSurfaceInfo_c*, CPlantLocTri*>(this, plant);
}
