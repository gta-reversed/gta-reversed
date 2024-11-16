#include "StdInc.h"

#include "ProcObjectDebugModule.h"
#include "ProcObjectMan.h"
#include "PlantMgr.h"
#include "GrassRenderer.h"


void ProcObjectDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{
        "Procedural Objects", {446.f, 512.f},
         m_IsOpen
    };
    if (!m_IsOpen) {
        return;
    }

    {
        const notsa::ui::ScopedDisable disabledScope{ true };

        ImGui::InputScalar("CProcObjMan::m_numProcSurfaceInfos", ImGuiDataType_S32, &g_procObjMan.m_numProcSurfaceInfos);
        ImGui::InputScalar("CPlantMgr::m_scanCode", ImGuiDataType_S16, &CPlantMgr::m_scanCode);
        ImGui::InputScalar("CPlantMgr::m_UnusedLocTriListHead", ImGuiDataType_U32, &CPlantMgr::m_UnusedLocTriListHead, nullptr, nullptr, "%08X");
        ImGui::InputScalar("CPlantMgr::m_UnusedColEntListHead", ImGuiDataType_U32, &CPlantMgr::m_UnusedColEntListHead, nullptr, nullptr, "%08X");
        ImGui::InputScalar("CPlantMgr::m_CloseColEntListHead", ImGuiDataType_U32, &CPlantMgr::m_CloseColEntListHead, nullptr, nullptr, "%08X");

        ImGui::InputScalar("CPlantMgr::Update()::nUpdateEntCache", ImGuiDataType_U8, (int8*)0xC09171);
        ImGui::InputScalar("CPlantMgr::Update()::nLocTriSkipCounter", ImGuiDataType_U8, (int8*)0xC09170);
        

        ImGui::NewLine();

        ImGui::InputScalar("g_GrassCurrentScanCode", ImGuiDataType_U16, &g_GrassCurrentScanCode);
        ImGui::InputScalar("gTriPlantBuf", ImGuiDataType_U32, &gTriPlantBuf, nullptr, nullptr, "%08X");
        ImGui::Text("CGrassRenderer::m_vecCameraPos %.2f %.2f %.2f", CGrassRenderer::m_vecCameraPos.x, CGrassRenderer::m_vecCameraPos.y, CGrassRenderer::m_vecCameraPos.z);
        ImGui::InputScalar("CGrassRenderer::m_closeDist", ImGuiDataType_Float, &CGrassRenderer::m_closeDist);
        ImGui::InputScalar("CGrassRenderer::m_farDist", ImGuiDataType_Float, &CGrassRenderer::m_farDist);
        ImGui::InputScalar("CGrassRenderer::m_windBending", ImGuiDataType_Float, &CGrassRenderer::m_windBending);
    }
}

void ProcObjectDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Procedural Objects", nullptr, &m_IsOpen);
    });
}
