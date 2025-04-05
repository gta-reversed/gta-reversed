#include "StdInc.h"

#include "UIRenderer.h"
#include "TaskComplexDestroyCarMelee.h"
#include <TaskComplexEnterCarAsPassengerTimed.h>
#include "TaskComplexWalkAlongsidePed.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskComplexFollowNodeRoute.h"
#include "TaskComplexExtinguishFires.h"
#include "TaskComplexStealCar.h"
#include "TaskComplexFleeAnyMeans.h"
#include "TaskComplexDriveWander.h"
#include "TaskComplexCarSlowBeDraggedOut.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <libs/imgui/misc/cpp/imgui_stdlib.h>

#ifdef NOTSA_USE_SDL3
#include <SDL3/SDL.h>
#include <libs/imgui/bindings/imgui_impl_sdl3.h>
#else
#include <libs/imgui/bindings/imgui_impl_win32.h>
#endif
#include <libs/imgui/bindings/imgui_impl_dx9.h>

#include <Windows.h>
#include "DebugModules/DebugModules.h"

namespace notsa {
namespace ui {
UIRenderer::UIRenderer() :
    m_ImCtx{ ImGui::CreateContext() },
    m_ImIO{ &m_ImCtx->IO }
{
    IMGUI_CHECKVERSION();

    m_ImIO->ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard
                        | ImGuiConfigFlags_NavEnableGamepad
                        | ImGuiConfigFlags_DockingEnable
                        | ImGuiConfigFlags_ViewportsEnable;
    m_ImIO->DisplaySize = ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT);

#ifdef NOTSA_USE_SDL3
    ImGui_ImplSDL3_InitForD3D((SDL_Window*)(PSGLOBAL(sdlWindow)));
#else
    ImGui_ImplWin32_Init(PSGLOBAL(window));
#endif
    ImGui_ImplDX9_Init(GetD3D9Device());

    SetIsActive(false);

    NOTSA_LOG_DEBUG("I say hello!");
}

UIRenderer::~UIRenderer() {
    ImGui_ImplDX9_Shutdown();
#ifdef NOTSA_USE_SDL3
    ImGui_ImplSDL3_Shutdown();
#else
    ImGui_ImplWin32_Shutdown();
#endif
    ImGui::DestroyContext(m_ImCtx);

    //NOTSA_LOG_DEBUG("Good bye!");
}

void UIRenderer::SetIsActive(bool active) {
    const auto pad = CPad::GetPad(0);

    m_InputActive = active;

    if (active) { // Clear controller states
        pad->OldMouseControllerState
            = pad->NewMouseControllerState
            = CMouseControllerState{};
    } else {
        SetFocus(PSGLOBAL(window)); // Re-focus GTA main window
    }
    pad->Clear(false, true);

    m_ImIO->MouseDrawCursor = active;
    m_ImIO->ConfigNavCaptureKeyboard = active;
    
    if (active) {
        m_ImIO->ConfigFlags &= ~(ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoKeyboard);
    } else {
        m_ImIO->ConfigFlags |= ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoKeyboard;
    }

    if (active) {
        ImGui::TeleportMousePos(m_LastMousePos);
    } else {
        m_LastMousePos = ImGui::GetMousePos();
    }

#ifdef NOTSA_USE_SDL3
    SDL_SetWindowRelativeMouseMode((SDL_Window*)(PSGLOBAL(sdlWindow)), !active);
#endif
}

void UIRenderer::PreRenderUpdate() {
    ZoneScoped;

    m_ImIO->DeltaTime   = CTimer::GetTimeStepInSeconds();
    m_ImIO->DisplaySize = ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT); // Update display size, in case of window resize after ImGui was already initialized

    m_DebugModules.PreRenderUpdate();
    DebugCode();
    ReversibleHooks::CheckAll();

    if (ImGui::IsKeyChordPressed(ImGuiKey_F7, ImGuiInputFlags_RouteAlways) || CPad::GetPad()->IsFKeyJustDown(FKEY7)) {
        SetIsActive(!m_InputActive);
    }
}

void UIRenderer::PostRenderUpdate() {
    //m_ImIO->NavActive = m_InputActive; // ImGUI clears `NavActive` every frame, so have to set it here.
}

void UIRenderer::DrawLoop() {
    ZoneScoped;

    if (m_ReInitRequested) {
        ResetSingleton(); // This will destruct the current object so we gotta stop here.
        return;
    }

    PreRenderUpdate();
#ifdef NOTSA_USE_SDL3
    ImGui_ImplSDL3_NewFrame();
#else
    ImGui_ImplWin32_NewFrame();
#endif
    ImGui_ImplDX9_NewFrame();
    ImGui::NewFrame();

    Render2D();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    //ImGui_ImplDX9_InvalidateDeviceObjects();

    PostRenderUpdate();

    // Update and Render additional Platform Windows
    if (m_ImIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void UIRenderer::Render2D() {
    ZoneScoped;

    m_DebugModules.Render2D();
}

void UIRenderer::Render3D() {
    ZoneScoped;

    m_DebugModules.Render3D();
}

void UIRenderer::DebugCode() {
    CPad* pad = CPad::GetPad();

    const auto player = FindPlayerPed();

    if (UIRenderer::IsActive() || CPad::NewKeyState.lctrl || CPad::NewKeyState.rctrl)
        return;

    if (pad->IsStandardKeyJustPressed('8')) {
        player->GetTaskManager().SetTask(
            new CTaskComplexFollowNodeRoute{
                PEDMOVE_SPRINT,
                CVector{0.f, 0.f, 10.f}
            },
            TASK_PRIMARY_PRIMARY
        );
        NOTSA_LOG_DEBUG("GOING!");
        //CPointRoute route{};
        //
        //const auto r = 10.f;
        //const auto totalAngle = PI * 2.f;
        //for (auto a = 0.f; a < totalAngle; a += totalAngle / 8.f) {
        //    route.AddPoints(player->GetPosition() + CVector{std::cosf(a), std::sinf(a), 0.f} *r);
        //}
        //
        //player->GetTaskManager().SetTask(
        //    new CTaskComplexFollowPointRoute{
        //        PEDMOVE_SPRINT,
        //        route,
        //        CTaskComplexFollowPointRoute::Mode::ONE_WAY,
        //        3.f,
        //        3.f,
        //        false,
        //        true,
        //        true
        //    },
        //    TASK_PRIMARY_PRIMARY
        //);
    }

    if (pad->IsStandardKeyJustPressed('J')) {
        CCheat::JetpackCheat();
    }

    if (pad->IsStandardKeyJustPressed('9')) {
        if (const auto veh = FindPlayerVehicle()) {
            veh->Fix();
            veh->AsAutomobile()->SetRandomDamage(true);
        }
    }

    if (pad->IsStandardKeyJustPressed('2')) {
        CCheat::MoneyArmourHealthCheat();
    }
    if (pad->IsStandardKeyJustPressed('3')) {
        CCheat::VehicleCheat(MODEL_INFERNUS);
    }
    if (pad->IsStandardKeyJustDown('8')) {
        TheCamera.AddShakeSimple(10000.f, 1, 10.f);
        NOTSA_LOG_DEBUG("Hey");
    }
    if (pad->IsStandardKeyJustPressed('5')) {
        if (const auto veh = FindPlayerVehicle()) {
            player->GetTaskManager().SetTask(
                new CTaskComplexCarDriveWander{veh, eCarDrivingStyle::DRIVING_STYLE_PLOUGH_THROUGH, 100.f},
                TASK_PRIMARY_PRIMARY
            );
        }
    }
    if (pad->IsStandardKeyJustPressed('6')) {
        FindPlayerPed()->Say(CTX_GLOBAL_JACKED_CAR);
    }

    if (pad->IsStandardKeyJustPressed('T')) {
        CCurves::TestCurves();
    }

    //if (pad->IsStandardKeyJustPressed('T')) {
    //    const auto ped = new CPed(ePedType::PED_TYPE_GANG1);
    //    ped->SetCreatedBy(PED_GAME);
    //    ped->SetModelIndex(MODEL_MALE01);
    //    ped->SetHeading(player->GetHeading());
    //    CWorld::Add(ped);
    //    ped->SetPosn(player->GetPosition() + player->GetForward() * 6.f);
    //    ped->GetTaskManager().SetTask(
    //        new CTaskSimpleGoToPoint{PEDMOVE_SPRINT, ped->GetPosition() + ped->GetForward() * 40.f},
    //        TASK_PRIMARY_PRIMARY
    //    );
    //    player->GetTaskManager().SetTask(
    //        new CTaskComplexWalkAlongsidePed{ped, 15.f},
    //        TASK_PRIMARY_PRIMARY
    //    );
    //}

    //if (pad->IsStandardKeyJustPressed('8')) {
    //    CMessages::AddToPreviousBriefArray("PRESS ~k~~PED_ANSWER_PHONE~ TO FUCK");
    //}
}
}; // namespace ui
}; // namespace notsa
