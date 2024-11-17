#include "StdInc.h"

#include "ProcObjectMan.h"
#include "PlantLocTri.h"

using EntityItem_c = void*;

void ProcObjectMan_c::InjectHooks() {
    RH_ScopedClass(ProcObjectMan_c);
    RH_ScopedCategory("Plant");

    RH_ScopedInstall(Init, 0x5A3EA0);
    RH_ScopedInstall(Update, 0x5A3110);
    RH_ScopedInstall(Exit, 0x5A3EE0);
    RH_ScopedInstall(LoadDataFile, 0x5A3140);
    RH_ScopedInstall(GetEntityFromPool, 0x5A3120);
    RH_ScopedInstall(ReturnEntityToPool, 0x5A3130);
    RH_ScopedInstall(ProcessTriangleAdded, 0x5A3F20);
    RH_ScopedInstall(ProcessTriangleRemoved, 0x5A3F70, {.reversed = false});
}

// 0x5A3EA0
void ProcObjectMan_c::Init() {
    for (auto& obj : m_Objects) {
        m_ObjectsList.AddItem(&obj);
    }
    m_numProcSurfaceInfos = 0;
    LoadDataFile();
    m_numAllocatedMatrices = 0;
}

// 0x5A3110
void ProcObjectMan_c::Update() {
    ZoneScoped;

    // NOP
}

// 0x5A3EE0
void ProcObjectMan_c::Exit() {
    for (auto& info : std::span{ m_ProcObjSurfaceInfos, (size_t)m_numProcSurfaceInfos}) {
        info.Exit();
    }
    m_ObjectsList.RemoveAll();
}

// 0x5A3140
void ProcObjectMan_c::LoadDataFile() {
    auto file = CFileMgr::OpenFile("data\\procobj.dat", "r");
    assert(file);

    for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (line[0] == '\0' || line[0] == '#')
            continue;

        char surfaceType[64]{};
        char objectName[64]{};
        float spacing;
        float minDist;
        int32 minRot, maxRot;
        float minScale, maxScale;
        float minScaleZ, maxScaleZ;
        float zOffsetMin, zOffsetMax;
        int32 align;
        int32 useGrid;

        VERIFY(sscanf_s(
            line, "%s %s %f %f %d %d %f %f %f %f %f %f %d %d",
            SCANF_S_STR(surfaceType),
            SCANF_S_STR(objectName),
            &spacing,
            &minDist,
            &minRot, &maxRot,
            &minScale, &maxScale,
            &minScaleZ, &maxScaleZ,
            &zOffsetMin, &zOffsetMax,
            &align, &useGrid
        ) == 14);
        m_ProcObjSurfaceInfos[m_numProcSurfaceInfos].Init(
            surfaceType,
            objectName,
            spacing,
            minDist,
            minRot, maxRot,
            minScale, maxScale,
            minScaleZ, maxScaleZ,
            zOffsetMin, zOffsetMax,
            align, useGrid
        );
        m_numProcSurfaceInfos++;
    }
    CFileMgr::CloseFile(file);
}

// 0x5A3120
ProcObjectListItem* ProcObjectMan_c::GetEntityFromPool() {
    return m_ObjectsList.RemoveHead();
}

// 0x5A3130
void ProcObjectMan_c::ReturnEntityToPool(ProcObjectListItem* item) {
    m_ObjectsList.AddItem(item);
}

// 0x5A3F20
int32 ProcObjectMan_c::ProcessTriangleAdded(CPlantLocTri* plant) {
    auto count = 0;
    for (auto& info : std::span{ m_ProcObjSurfaceInfos, (size_t)m_numProcSurfaceInfos}) {
        if (info.m_SurfaceId == plant->m_SurfaceId) {
            count += info.AddObjects(plant);
        }
    }
    return count;
}

// 0x5A3F70
void ProcObjectMan_c::ProcessTriangleRemoved(CPlantLocTri* triangle) {
    return plugin::CallMethod<0x5A3F70, ProcObjectMan_c*, CPlantLocTri*>(this, triangle);
    // todo: fix the fucking list loop
    /*
    if (m_numProcSurfaceInfos <= 0)
        return;

    for (auto& info : std::span{m_ProcObjSurfaceInfos, (size_t)m_numProcSurfaceInfos}) {
        if (info.m_SurfaceId != triangle->m_SurfaceId)
            continue;

        for (auto i = info.m_Objects.GetHead(); i; i = info.m_Objects.GetNext(i)) {
            auto& obj = *(ProcObjectListItem*)i;
            if (obj.m_LocTri == triangle) {
                if (obj.m_Obj->m_nType == ENTITY_TYPE_OBJECT) {
                    CObject::nNoTempObjects--;
                }

                info.m_Objects.RemoveItem(&obj);
                g_procObjMan.m_ObjectsList.AddItem(&obj);
                obj.m_Obj->DeleteRwObject();
                CWorld::Remove(obj.m_Obj);
                if (obj.m_Obj) {
                    delete obj.m_Obj;
                }

                if (obj.m_bAllocatedMatrix) {
                    m_numAllocatedMatrices--;
                    obj.m_bAllocatedMatrix = false;
                }
            }
        }
    }*/
}
