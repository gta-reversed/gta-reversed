
#include "StdInc.h"
#include "WindowedMode.hpp"
#include <HookSystem.h>

#include "winincl.h"
#include <SDL3/SDL.h>
#include <bindings/imgui_impl_dx9.h>

#define MINZBUFFERVALUE 0.0f
#define MAXZBUFFERVALUE 1.0f


HRESULT DXCHECK(HRESULT hr) {
    VERIFY(SUCCEEDED(hr));
    return hr;
}

#define RASTEREXTFROMRASTER(raster) \
    ((_rwD3D9RasterExt *)(((RwUInt8 *)(raster)) + _RwD3D9RasterExtOffset))

#define RASTEREXTFROMCONSTRASTER(raster)                        \
    ((const _rwD3D9RasterExt *)                                 \
     (((const RwUInt8 *)(raster)) + _RwD3D9RasterExtOffset))

#define FMT2TBL(_p) (((_p) & rwRASTERFORMATPIXELFORMATMASK) >> 8)

#define RWFUNCTION(name)       /* No op */
#define RWASSERT assert
#define RWRETURN(ret) return (ret)
#define RWRETURNVOID() return

typedef struct _rwD3D9AdapterInformation _rwD3D9AdapterInformation;
struct _rwD3D9AdapterInformation
{
    RwChar          name[MAX_DEVICE_IDENTIFIER_STRING]; // 0
    RwInt32         modeCount; // 512
    D3DDISPLAYMODE  mode; // 512 + 4
    RwInt32         displayDepth;
    RwInt32         flags;
};

typedef struct _rwD3D9Palette _rwD3D9Palette;
struct _rwD3D9Palette
{
    PALETTEENTRY    entries[256];
    RwInt32     globalindex;
};

typedef LPDIRECT3DSURFACE9 LPSURFACE;
typedef LPDIRECT3DTEXTURE9 LPTEXTURE;
struct _rwD3D9RasterExt
{
    LPTEXTURE               texture;
    _rwD3D9Palette          *palette;
    RwUInt8                 alpha;              /* This texture has alpha */
    RwUInt8                 cube : 4;           /* This texture is a cube texture */
    RwUInt8                 face : 4;           /* The active face of a cube texture */
    RwUInt8                 automipmapgen : 4;  /* This texture uses automipmap generation */
    RwUInt8                 compressed : 4;     /* This texture is compressed */
    RwUInt8                 lockedMipLevel;
    LPSURFACE               lockedSurface;
    D3DLOCKED_RECT          lockedRect;
    D3DFORMAT               d3dFormat;          /* D3D format */
    LPDIRECT3DSWAPCHAIN9    swapChain;
    HWND                    window;
};

auto& _RwD3D9RasterExtOffset             = StaticRef<RwInt32>(0xB4E9E0); /* Raster extension offset */
auto& StencilClearValue                  = StaticRef<RwUInt32>(0xC97C44);
auto& WindowHandle                       = StaticRef<HWND>(0xC97C1C);
auto& _RwHasStencilBuffer                = StaticRef<RwBool>(0xC97C3C);
auto& _RwD3D9AdapterInformation          = StaticRef<_rwD3D9AdapterInformation>(0xC9BCE0);
auto& Present                            = StaticRef<D3DPRESENT_PARAMETERS>(0xC9C040);
auto& _RwD3D9DeviceCaps                  = StaticRef<D3DCAPS9>(0xC9BF00);
auto& _RwD3D9RenderSurface               = StaticRef<LPSURFACE>(0xC97C30);
auto& _RwD3D9DepthStencilSurface         = StaticRef<LPSURFACE>(0xC97C2C);
auto& _RwDirect3DObject                  = StaticRef<IDirect3D9*>(0xC97C20);
auto& _RwD3DAdapterIndex                 = StaticRef<RwUInt32>(0xC97C24);
auto& _RwD3DAdapterType                  = StaticRef<D3DDEVTYPE>(0x8E2428);
auto& FullScreenRefreshRateInHz          = StaticRef<RwUInt32>(0x8E243C);
auto& _RwD3D9ZBufferDepth                = StaticRef<RwInt32>(0xC9BEFC);
auto& SelectedMultisamplingLevels        = StaticRef<RwUInt32>(0x8E2430);
auto& SelectedMultisamplingLevelsNonMask = StaticRef<RwUInt32>(0x8E2438);
auto& DisplayModes                       = StaticRef<RwVideoMode*>(0xC97C48);

static RwBool EnableFullScreenDialogBoxMode = FALSE;


const auto D3D9DeviceReleaseVideoMemory  = plugin::CallAndReturn<RwBool, 0x7F7F70>;
const auto _rwD3D9SetDepthStencilSurface = plugin::Call<0x7F5EF0, LPSURFACE>;
const auto _rwD3D9SetRenderTarget        = plugin::CallAndReturn<RwBool, 0x7F5F20, RwUInt32, LPSURFACE>;
const auto D3D9DeviceRestoreVideoMemory  = plugin::CallAndReturn<RwBool, 0x7F7F70>;

namespace notsa::WindowedMode {
bool bWindowed        = true;
bool bFullMode        = !bWindowed;
bool bRequestFullMode = false;
bool bUseBorder       = false;
bool bResChanged      = false;
bool bChangingLocked = false;
bool bStopRecursion = false;

int nCurrentWidth = 0, nCurrentHeight = 0;
int nNonFullWidth = 600, nNonFullHeight = 450;
int nNonFullPosX = 0, nNonFullPosY = 0;

struct PosChangeNoRecursion {
    bool prev;

    PosChangeNoRecursion() {
        prev = std::exchange(bStopRecursion, true);
    }
    ~PosChangeNoRecursion() {
        bStopRecursion = prev;
    }
};

std::tuple<int32_t, int32_t> GetDesktopRes() {
    HMONITOR    monitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
    MONITORINFO info    = {};
    info.cbSize         = sizeof(MONITORINFO);
    GetMonitorInfo(monitor, &info);
    return { info.rcMonitor.right - info.rcMonitor.left, info.rcMonitor.bottom - info.rcMonitor.top};
}

void MainCameraRebuildRaster(void* camera) {
    auto* const cam = (RwCamera*)(camera);
    if (cam != Scene.m_pRwCamera) {
        return;
    }
    IDirect3DSurface9* pSurface;
    D3DSURFACE_DESC stSurfaceDesc;
    if (!GetD3D9Device()) {
        return;
    }

    GetD3D9Device()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
    pSurface->GetDesc(&stSurfaceDesc);
    pSurface->Release();

    RwVideoMode* pVM = &DisplayModes[RwEngineGetCurrentVideoMode()];
    if (pVM->width != (int)stSurfaceDesc.Width || pVM->height != (int)stSurfaceDesc.Height)
    {
        pVM->width = (int)stSurfaceDesc.Width;
        pVM->height = (int)stSurfaceDesc.Height;

    }

    int nGameWidth = pVM->width;
    int nGameHeight = pVM->height;

    _RwD3D9AdapterInformation.mode.Width = nGameWidth;
    _RwD3D9AdapterInformation.mode.Height = nGameHeight;

    if (cam->frameBuffer && (cam->frameBuffer->width != nGameWidth || cam->frameBuffer->height != nGameHeight))
    {
        RwRasterDestroy(cam->frameBuffer);
        cam->frameBuffer = NULL;
    }

    if (!cam->frameBuffer)
    {
        cam->frameBuffer = RwRasterCreate(nGameWidth, nGameHeight, 32, rwRASTERTYPECAMERA);
    }
    assert(cam->frameBuffer);

    if (cam->zBuffer && (cam->zBuffer->width != nGameWidth || cam->zBuffer->height != nGameHeight))
    {
        RwRasterDestroy(cam->zBuffer);
        cam->zBuffer = NULL;
    }

    if (!cam->zBuffer)
    {
        cam->zBuffer = RwRasterCreate(nGameWidth, nGameHeight, 0, rwRASTERTYPEZBUFFER);
    }
}

void AdjustGameToWindowSize() {
    RECT rcClient;
    GetClientRect(WindowHandle, &rcClient);

    int nModeIndex = RwEngineGetCurrentVideoMode();
    if (DisplayModes) {
        bool bSizeChanged = (DisplayModes[nModeIndex].width != rcClient.right || DisplayModes[nModeIndex].height != rcClient.bottom);
        if (bSizeChanged) {
            DisplayModes[nModeIndex].width  = rcClient.right;
            DisplayModes[nModeIndex].height = rcClient.bottom;

            if (Scene.m_pRwCamera) {
                RwCameraClear(Scene.m_pRwCamera, &gColourTop, rwCAMERACLEARZ);
            }
        }

        if (!bFullMode) {
            RECT rcWindow;
            GetWindowRect(WindowHandle, &rcWindow);

            if (rcWindow.left != 0 && rcWindow.top != 0) {
                nNonFullPosX = rcWindow.left;
                nNonFullPosY = rcWindow.top;
            }
        }
    }
}

void AdjustPresentParams(D3DPRESENT_PARAMETERS* pp) {
    if (!pp) {
        pp = &Present;
    }

    if (!bWindowed) {
        return;
    }

    pp->Windowed = TRUE;

    pp->PresentationInterval       = 0;
    pp->FullScreen_RefreshRateInHz = 0;
    pp->EnableAutoDepthStencil     = TRUE;
    pp->BackBufferFormat           = gGameState == GAME_STATE_IDLE
                  ? D3DFMT_A8R8G8B8
                  : D3DFMT_X8R8G8B8;

    if (pp->MultiSampleType > 0) {
        pp->SwapEffect = D3DSWAPEFFECT_DISCARD;
    }

    auto [desktopW, desktopH] = GetDesktopRes();

    RsGlobal.maximumWidth  = pp->BackBufferWidth;
    RsGlobal.maximumHeight = pp->BackBufferHeight;

    RECT rcClient = { 0, 0, (LONG)pp->BackBufferWidth, (LONG)pp->BackBufferHeight };
    AdjustWindowRectEx(&rcClient, GetWindowLong(WindowHandle, GWL_STYLE), FALSE, GetWindowLong(WindowHandle, GWL_EXSTYLE));

    SetWindowPos(
        WindowHandle,
        HWND_NOTOPMOST,
        0, 0,
        std::min<LONG>(desktopW, rcClient.right - rcClient.left), std::min<LONG>(rcClient.bottom - rcClient.top, desktopH),
        SWP_NOACTIVATE | SWP_NOSIZE | (bFullMode ? 0 : SWP_NOMOVE)
    );
    GetClientRect(WindowHandle, &rcClient);

    pp->BackBufferWidth  = rcClient.right;
    pp->BackBufferHeight = rcClient.bottom;
    pp->hDeviceWindow    = WindowHandle;
}

struct ScopedRestoreWindowInfo {
    //static inline bool PendingStore = false;
    //
    bool restore;
    int  w, h;
    int  x, y;
    LONG flags;

    ScopedRestoreWindowInfo() noexcept {
        this->restore = !std::exchange(bChangingLocked, true);
        if (!this->restore) {
            return;
        }

        this->flags = GetWindowLong(WindowHandle, GWL_STYLE);

        tagRECT r;
        GetWindowRect(WindowHandle, &r);
        this->w = r.right - r.left;
        this->h = r.bottom - r.top;
        this->x = r.left;
        this->y = r.top;
    }

    ~ScopedRestoreWindowInfo() noexcept {
        if (!this->restore) {
            return;
        }
        tagRECT r;
        GetWindowRect(WindowHandle, &r);
        const auto currW = r.right - r.left;
        const auto currH = r.bottom - r.top;

        if (currW != this->w || currH != this->h || GetWindowLong(WindowHandle, GWL_STYLE) != this->flags) {
            SetWindowLong(WindowHandle, GWL_STYLE, this->flags);
            {
                PosChangeNoRecursion r{};
                SetWindowPos(WindowHandle, NULL, this->x, this->y, this->w, this->h, SWP_NOACTIVATE | SWP_NOZORDER);
            }
        }
    }
};

struct D3D9ProxyDevice {
    using D3D9Device_Reset_Type = decltype(&IDirect3DDevice9::Reset); //HRESULT(__stdcall*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
    static inline D3D9Device_Reset_Type Real_Reset{};
    static inline constexpr auto D3D9Device_Reset_VMT_Index = 16;
    static HRESULT __stdcall Proxy_Reset(LPDIRECT3DDEVICE9 self, D3DPRESENT_PARAMETERS* pp) {
        //if (bWindowed) {
        //    AdjustPresentParams(pp);
        //}

        ImGui_ImplDX9_InvalidateDeviceObjects();

        HRESULT hr;

        {
            //ScopedRestoreWindowInfo restore{};
            hr = (self->*Real_Reset)(pp);
        }

        ImGui_ImplDX9_CreateDeviceObjects();

        if (SUCCEEDED(hr)) {
            auto& vm  = DisplayModes[RwEngineGetCurrentVideoMode()];
            vm.width  = pp->BackBufferWidth;
            vm.height = pp->BackBufferHeight;
        } else {
            NOTSA_DEBUGBREAK();
        }

        return hr;
    }

    using D3D9Device_SetViewport_Type = decltype(&IDirect3DDevice9::SetViewport);
    static inline constexpr auto D3D9Device_SetViewport_VMT_Index = 47;
    static inline D3D9Device_SetViewport_Type Real_SetViewport{};
    static HRESULT __stdcall Proxy_SetViewport(LPDIRECT3DDEVICE9 self, D3DVIEWPORT9* vp) {
        ScopedRestoreWindowInfo restore{};
        return (self->*Real_SetViewport)(vp);
    }

    static inline bool Initialized{ false };
    static void Initialize(LPDIRECT3DDEVICE9 dev) {
        void** vmt = plugin::GetVMT(dev);

        // Save original pointers
        if (!std::exchange(Initialized, true)) {
            Real_Reset       = VoidToFunctionPtr<D3D9Device_Reset_Type>(vmt[D3D9Device_Reset_VMT_Index]);
            Real_SetViewport = VoidToFunctionPtr<D3D9Device_SetViewport_Type>(vmt[D3D9Device_SetViewport_VMT_Index]);
        }

        // Overwrite vmt entries
        vmt[D3D9Device_Reset_VMT_Index]       = FunctionToVoidPtr(&Proxy_Reset);
        vmt[D3D9Device_SetViewport_VMT_Index] = FunctionToVoidPtr(&Proxy_SetViewport);
    }
};

// Allocate stack space for function
#define STACK_PUSH               \
    _asm push ebp                \
    _asm mov ebp, esp            \
    _asm sub esp, __LOCAL_SIZE 

// De-allocate stack space for function
#define STACK_POP       \
    _asm mov esp, ebp   \
    _asm pop ebp

void __declspec(naked) HookDirect3DDeviceReplacerSA() noexcept {
    _asm pushad
    STACK_PUSH

    WindowedMode::AdjustPresentParams(&Present);

    HRESULT hr;
    {
        //ScopedRestoreWindowInfo restore;

        STACK_POP
        _asm popad           // Restore original arguments
        _asm call[edx + 40h] // Call `CreateDevice` (as the original code did)
        STACK_PUSH
        _asm mov hr, eax     // Save result temporarily to `hr` (Because registers will be restored)
        _asm pushad
    }

    if (SUCCEEDED(hr)) {
        D3D9ProxyDevice::Initialize(GetD3D9Device());
    } else {
        NOTSA_LOG_ERR("CreateDevice failed, existing...");
        Sleep(10);
        exit(10);
    }

    _asm popad             // Restore registers to pre-jump
    STACK_POP
    _asm test eax, eax     // Restore original code (Replaced by the jump to this function)
    _asm mov eax, 0x7F6786 // eax isn't used after this, so we can re-use it
    _asm jmp eax           // Jump back to the `jge` instruction

}

//static auto HookDxCamFix_Return = 0x7F7C4C;
//void __declspec(naked) HookDxCamFix() {
//    _asm pushad
//    _asm mov ecx, [esp + 80h]
//    STACK_PUSH
//
//    void* cam;
//    _asm mov cam, ecx
//    MainCameraRebuildRaster(cam);
//
//    STACK_POP
//    _asm popad
//
//    // Restore original code
//    _asm mov     edx, [esp+78h+50h]
//    _asm cmp     byte ptr [edx+20h], 5
//    _asm jmp     HookDxCamFix_Return
//}

#undef STACK_PUSH
#undef STACK_POP
};

namespace RenderWare {
/****************************************************************************
rwD3D9FindDepth

On entry   : format

On exit    : Format depth 16,32 on success else 0 on failure.
*/
static RwInt32
D3D9FindDepth(D3DFORMAT format)
{
    RwInt32             depth;

    RWFUNCTION(RWSTRING("D3D9FindDepth"));

    switch (format)
    {
    case D3DFMT_A2R10G10B10:
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
        depth = 32;
        break;

    case D3DFMT_R5G6B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_X1R5G5B5:
        depth = 16;
        break;

    default:
        depth = 0;
        break;
    }

    RWRETURN(depth);
}

/****************************************************************************
 _rwD3D9CameraClear

 On entry   : Camera
            : RwRGBA colour to clear to
            : Buffer mask to clear
 On exit    : TRUE on success
 */
RwBool
_rwD3D9CameraClear(void *camera, void *color, RwInt32 clearFlags)
{
    RwRaster           *raster;
    RwRaster           *topRaster;
    _rwD3D9RasterExt   *topRasterExt;
    RwRaster           *zRaster;
    RwRaster            *zTopRaster;
    const _rwD3D9RasterExt   *zrasExt;
    D3DCOLOR            packedColor = 0;
    HRESULT             hr = D3D_OK;
    RECT                rect;
    static const DWORD  clearFlagsConvTable[8] = {
        0,                      /* Nothing */
        D3DCLEAR_TARGET,        /* Clear Frame buffer only */
        D3DCLEAR_ZBUFFER,       /* Clear Z buffer only */
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, /* Clear Frame & Z buffer */
        D3DCLEAR_STENCIL,       /* Clear Stencil buffer */
        D3DCLEAR_TARGET | D3DCLEAR_STENCIL, /* Clear Frame & Stencil buffer */
        D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, /* Clear Z Buffer & Stencil */
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, /* Clear Frame, Z & stencil */
    };

    RWFUNCTION(RWSTRING("_rwD3D9CameraClear"));

    RWASSERT(camera);
    RWASSERT(color);

    notsa::WindowedMode::MainCameraRebuildRaster(camera);

    /* The clear color */
    if (rwCAMERACLEARIMAGE & clearFlags)
    {
        RwRGBA             *col = (RwRGBA *) color;

        RWASSERT(NULL != color);

        /* Pack the clear color */
        packedColor = (D3DCOLOR) (((col->alpha) << 24) |
                                  ((col->red) << 16) |
                                  ((col->green) << 8) | (col->blue));
    }

    raster = RwCameraGetRaster((RwCamera *)camera);

    /*
     * Got to get the top level raster as this is the only one with a
     * texture surface
     */
    topRaster = RwRasterGetParent(raster);

    topRasterExt = RASTEREXTFROMRASTER(topRaster);

    if (topRasterExt->swapChain)
    {
        D3DVIEWPORT9        viewport;

        GetClientRect(topRasterExt->window, &rect);

        if (!rect.right || !rect.bottom)
        {
            RWRETURN(FALSE);
        }
        else
        {
            if (rect.right != topRaster->originalWidth ||
                rect.bottom != topRaster->originalHeight)
            {
                if (raster == topRaster)
                {
                    RwD3D9CameraAttachWindow(camera, topRasterExt->window);

                    /* Update raster pointers */
                    raster = RwCameraGetRaster((RwCamera *)camera);
                    topRaster = RwRasterGetParent(raster);
                    topRasterExt = RASTEREXTFROMRASTER(topRaster);
                }

                zRaster = RwCameraGetZRaster((RwCamera *) camera);
            }
            else
            {
                zRaster = RwCameraGetZRaster((RwCamera *) camera);

                if (zRaster && zRaster == RwRasterGetParent(zRaster))
                {
                    if (rect.right != zRaster->width ||
                        rect.bottom != zRaster->height)
                    {
                        (void)RwCameraSetZRaster((RwCamera *) camera,
                                               NULL);

                        RwRasterDestroy(zRaster);

                        zRaster =
                            RwRasterCreate(rect.right, rect.bottom, 0,
                                           rwRASTERTYPEZBUFFER);

                        if (zRaster)
                        {
                            (void)RwCameraSetZRaster((RwCamera *) camera,
                                                   zRaster);
                        }
                    }
                }
            }

            /* Set the render & depth/stencil surface */
            _rwD3D9SetRenderTarget(0, (LPSURFACE)topRasterExt->texture);

            if (zRaster)
            {
                zTopRaster = RwRasterGetParent(zRaster);

                zrasExt = RASTEREXTFROMCONSTRASTER(zTopRaster);

                _rwD3D9SetDepthStencilSurface((LPSURFACE)zrasExt->texture);

            }
            else
            {
                _rwD3D9SetDepthStencilSurface(NULL);
            }

            /* Initialize the viewport to clear */
            viewport.X = 0;
            viewport.Y = 0;
            viewport.Width = raster->width;
            viewport.Height = raster->height;
            viewport.MinZ = MINZBUFFERVALUE;
            viewport.MaxZ = MAXZBUFFERVALUE;

            /* Set the viewport to clear */
            DXCHECK(IDirect3DDevice9_SetViewport(_RwD3DDevice, &viewport));

            /* Clear */
            DXCHECK(IDirect3DDevice9_Clear
                    (_RwD3DDevice, 0, NULL,
                     clearFlagsConvTable[clearFlags], packedColor,
                     MAXZBUFFERVALUE, StencilClearValue));
        }
    }
    else
    {
        /* Check if the size of the main window has changed */
        GetClientRect(WindowHandle, &rect);

        if (!rect.right || !rect.bottom || IsIconic(WindowHandle))
        {
            RWRETURN(FALSE);
        }
        else if ((RwUInt32) rect.right !=
                 _RwD3D9AdapterInformation.mode.Width
                 || (RwUInt32) rect.bottom !=
                 _RwD3D9AdapterInformation.mode.Height)
        {
            D3D9DeviceReleaseVideoMemory();

            hr = IDirect3DDevice9_TestCooperativeLevel(_RwD3DDevice);

            if (SUCCEEDED(hr) && Present.Windowed)
            {
                Present.BackBufferWidth = rect.right;
                Present.BackBufferHeight = rect.bottom;
            }

            hr = DXCHECK(IDirect3DDevice9_Reset(_RwD3DDevice, &Present));

            if (SUCCEEDED(hr))
            {
                if (!D3D9DeviceRestoreVideoMemory())
                {
                    D3D9DeviceReleaseVideoMemory();

                    hr = E_FAIL;
                }
            }

            if (FAILED(hr))
            {
                if(Present.Windowed)
                {
                    #if defined(RWDEBUG)
                    RwChar              buffer[256];

                    rwsprintf(buffer, "Not enough memory to resize the window to %dx%d."
                                       "Switching back to %dx%d",
                                       rect.right, rect.bottom,
                                       _RwD3D9AdapterInformation.mode.Width,
                                       _RwD3D9AdapterInformation.mode.Height);

                    RwDebugSendMessage(rwDEBUGMESSAGE, "_rwD3D9CameraClear",
                                       buffer);
                    #endif

                    Present.BackBufferWidth = _RwD3D9AdapterInformation.mode.Width;
                    Present.BackBufferHeight = _RwD3D9AdapterInformation.mode.Height;

                    hr = DXCHECK(IDirect3DDevice9_Reset(_RwD3DDevice, &Present));

                    if (FAILED(hr))
                    {
                        RWRETURN(FALSE);
                    }

                    /* Change window size */
                    rect.right = rect.left + _RwD3D9AdapterInformation.mode.Width;
                    rect.bottom = rect.top + _RwD3D9AdapterInformation.mode.Height;

                    if (GetWindowLong(WindowHandle, GWL_STYLE) & WS_MAXIMIZE)
                    {
                        SetWindowLong(WindowHandle, GWL_STYLE, GetWindowLong(WindowHandle, GWL_STYLE) & ~WS_MAXIMIZE);
                    }

                    AdjustWindowRectEx(&rect,
                                    GetWindowLong(WindowHandle, GWL_STYLE),
                                    (GetMenu(WindowHandle)!=NULL),
                                    GetWindowLong(WindowHandle, GWL_EXSTYLE));

                    SetWindowPos(WindowHandle, 0,
                                rect.left, rect.bottom,
                                rect.right-rect.left,
                                rect.bottom-rect.top,
                                SWP_NOMOVE | SWP_NOZORDER);

                    D3D9DeviceRestoreVideoMemory();
                }
                else
                {
                    RWRETURN(FALSE);
                }
            }
            else
            {
                _RwD3D9AdapterInformation.mode.Width = rect.right;
                _RwD3D9AdapterInformation.mode.Height = rect.bottom;
            }
        }

        /* Check raster type */
        if (rwRASTERTYPECAMERATEXTURE == topRaster->cType)
        {
            LPSURFACE           surfaceLevel;
            D3DSURFACE_DESC     d3d9Desc;
            D3DVIEWPORT9        viewport;
            RwUInt32            d3d9ClearFlags;

            if (topRasterExt->cube)
            {
                IDirect3DCubeTexture9_GetCubeMapSurface((LPDIRECT3DCUBETEXTURE9)topRasterExt->texture,
                                                        (D3DCUBEMAP_FACES)topRasterExt->face,
                                                        0,
                                                        &surfaceLevel);
            }
            else
            {
                IDirect3DTexture9_GetSurfaceLevel(topRasterExt->texture, 0,
                                                  &surfaceLevel);
            }

            IDirect3DSurface9_GetDesc(surfaceLevel, &d3d9Desc);

            /* Set the render & depth/stencil surface */
            if (d3d9Desc.Usage & D3DUSAGE_RENDERTARGET)
            {
                _rwD3D9SetRenderTarget(0, surfaceLevel);

                zRaster = RwCameraGetZRaster((RwCamera *) camera);
                if (zRaster)
                {
                    /*
                    * Got to get the top level raster as this is the only one with a
                    * texture surface
                    */
                    zTopRaster = RwRasterGetParent(zRaster);

                    zrasExt = RASTEREXTFROMCONSTRASTER(zTopRaster);

                    if ( (RwRasterGetType(zTopRaster) & rwRASTERTYPEMASK) == rwRASTERTYPEZBUFFER )
                    {
                        _rwD3D9SetDepthStencilSurface((LPSURFACE)zrasExt->texture);
                    }
                    else
                    {
                        LPSURFACE   surfaceLevelZ;

                        IDirect3DTexture9_GetSurfaceLevel(zrasExt->texture, 0,
                                                          &surfaceLevelZ);

                        _rwD3D9SetDepthStencilSurface(surfaceLevelZ);

                        IDirect3DSurface9_Release(surfaceLevelZ);
                    }
                }
                else
                {
                    _rwD3D9SetDepthStencilSurface(NULL);
                }

                hr = D3D_OK;
            }
            else
            {
                hr = E_FAIL;
            }

            /* Initialize the viewport to clear */
            viewport.X = raster->nOffsetX;
            viewport.Y = raster->nOffsetY;
            viewport.Width = raster->width;
            viewport.Height = raster->height;
            viewport.MinZ = MINZBUFFERVALUE;
            viewport.MaxZ = MAXZBUFFERVALUE;

            /* Set the viewport to clear */
            DXCHECK(IDirect3DDevice9_SetViewport
                    (_RwD3DDevice, &viewport));

            /* check clear flags */
            d3d9ClearFlags = clearFlagsConvTable[clearFlags];

            if (d3d9ClearFlags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL))
            {
                if (RwCameraGetZRaster((RwCamera *) camera) == NULL)
                {
                    d3d9ClearFlags &= ~(D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
                }
            }

            /* Clear */
            DXCHECK(IDirect3DDevice9_Clear
                    (_RwD3DDevice, 0, NULL,
                     d3d9ClearFlags, packedColor,
                     MAXZBUFFERVALUE, StencilClearValue));

            /* If the set render target fails, we need to copy from the back buffer */
            if (FAILED(hr))
            {
                RECT    rectSrc;

                rectSrc.left = raster->nOffsetX;
                rectSrc.top = raster->nOffsetY;
                rectSrc.right = rectSrc.left + raster->width;
                rectSrc.bottom = rectSrc.top + raster->height;

                if (_RwD3D9DeviceCaps.StretchRectFilterCaps & (D3DPTFILTERCAPS_MINFPOINT | D3DPTFILTERCAPS_MAGFPOINT))
                {
                    hr = DXCHECK(IDirect3DDevice9_StretchRect
                                (_RwD3DDevice,
                                _RwD3D9RenderSurface, &rectSrc,
                                surfaceLevel, &rectSrc,
                                D3DTEXF_POINT));
                }
                else
                {
                    hr = DXCHECK(IDirect3DDevice9_StretchRect
                                (_RwD3DDevice,
                                _RwD3D9RenderSurface, &rectSrc,
                                surfaceLevel, &rectSrc,
                                D3DTEXF_NONE));
                }
            }

            /* Release the cleared surface */
            IDirect3DSurface9_Release(surfaceLevel);
        }
        else
        {
            D3DVIEWPORT9        viewport;
            RwUInt32            d3d9ClearFlags = 0;

            /* Set the render & depth/stencil surface */
            _rwD3D9SetRenderTarget(0, _RwD3D9RenderSurface);
            _rwD3D9SetDepthStencilSurface(_RwD3D9DepthStencilSurface);

            /* Initialize the viewport to clear */
            viewport.X = raster->nOffsetX;
            viewport.Y = raster->nOffsetY;
            viewport.Width = raster->width;
            viewport.Height = raster->height;
            viewport.MinZ = MINZBUFFERVALUE;
            viewport.MaxZ = MAXZBUFFERVALUE;

            /* Set the viewport to clear */
            DXCHECK(IDirect3DDevice9_SetViewport
                    (_RwD3DDevice, &viewport));

            /* Clear */
            if (rwCAMERACLEARIMAGE & clearFlags)
            {
                d3d9ClearFlags |= D3DCLEAR_TARGET;
            }

            if (rwCAMERACLEARZ & clearFlags)
            {
                d3d9ClearFlags |= D3DCLEAR_ZBUFFER;

                if (_RwHasStencilBuffer)
                {
                    d3d9ClearFlags |= D3DCLEAR_STENCIL;
                }
            }
            else if (rwCAMERACLEARSTENCIL & clearFlags)
            {
                if (_RwHasStencilBuffer)
                {
                    d3d9ClearFlags |= D3DCLEAR_STENCIL;
                }
            }

            DXCHECK(IDirect3DDevice9_Clear
                    (_RwD3DDevice, 0, NULL, d3d9ClearFlags, packedColor,
                     MAXZBUFFERVALUE, StencilClearValue));
        }
    }

    RWRETURN(TRUE);
}

/****************************************************************************
D3D9SetPresentParameters

On entry   :
*/
static void
D3D9SetPresentParameters(const D3DDISPLAYMODE *mode,
    RwUInt32 flags,
    D3DFORMAT adapterFormat)
{
    RWFUNCTION(RWSTRING("D3D9SetPresentParameters"));

    flags &= ~rwVIDEOMODEEXCLUSIVE; // NOTSA: We don't care for now

    if (flags & rwVIDEOMODEEXCLUSIVE)
    {
        /* On full screen the adapter format is the display format selected */
        adapterFormat = mode->Format;

        /* fullscreen */
        Present.Windowed = FALSE;

        Present.BackBufferCount = 1;

        /* As fast as we can */
        Present.FullScreen_RefreshRateInHz = std::min(
            mode->RefreshRate,
            FullScreenRefreshRateInHz);

#ifdef LOCK_AT_VSYNC
        Present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
#else
        Present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif

        Present.BackBufferWidth = mode->Width;
        Present.BackBufferHeight = mode->Height;

        Present.SwapEffect = D3DSWAPEFFECT_FLIP;
    }
    else
    {
        RECT rect;

        /* windowed */
        Present.Windowed = TRUE;

        Present.BackBufferCount = 1;

        Present.FullScreen_RefreshRateInHz = 0;

        /* As fast as we can */
#ifdef LOCK_AT_VSYNC
        Present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
#else
        Present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif

        /* Check window size */
        GetWindowRect(WindowHandle, &rect);

        if ((rect.right - rect.left) == 0)
        {
            Present.BackBufferWidth = 1;
        }
        else
        {
            Present.BackBufferWidth = 0;
        }

        if ((rect.bottom - rect.top) == 0)
        {
            Present.BackBufferHeight = 1;
        }
        else
        {
            Present.BackBufferHeight = 0;
        }

        Present.SwapEffect = D3DSWAPEFFECT_COPY;
    }

    if (EnableFullScreenDialogBoxMode)
    {
        Present.SwapEffect = D3DSWAPEFFECT_DISCARD;
        Present.BackBufferFormat = adapterFormat;
    }
    else
    {
        /* Try backbuffer with alpha channel if using standard 32 bits format*/
        if (D3DFMT_X8R8G8B8 == adapterFormat)
        {
            if (SUCCEEDED(IDirect3D9_CheckDeviceType(_RwDirect3DObject,
                _RwD3DAdapterIndex,
                _RwD3DAdapterType,
                adapterFormat,
                D3DFMT_A8R8G8B8,
                Present.Windowed)))
            {
                Present.BackBufferFormat = D3DFMT_A8R8G8B8;
            }
            else
            {
                Present.BackBufferFormat = D3DFMT_X8R8G8B8;
            }
        }
        else
        {
            Present.BackBufferFormat = adapterFormat;
        }
    }

    /* TRUE so depth/stencil buffer are managed for us */
    Present.EnableAutoDepthStencil = TRUE;

    /*
    * Find the Z-Buffer depth
    * Best perfomance results when using same bit depth than back buffer
    */
    _RwD3D9ZBufferDepth = D3D9FindDepth(Present.BackBufferFormat);

    if (_RwD3D9ZBufferDepth == 32)
    {
        if (D3D_OK ==
            IDirect3D9_CheckDeviceFormat(_RwDirect3DObject,
                _RwD3DAdapterIndex,
                _RwD3DAdapterType,
                adapterFormat,
                D3DUSAGE_DEPTHSTENCIL,
                D3DRTYPE_SURFACE, D3DFMT_D24S8)
            && D3D_OK ==
            IDirect3D9_CheckDepthStencilMatch(_RwDirect3DObject,
                _RwD3DAdapterIndex,
                _RwD3DAdapterType,
                adapterFormat,
                Present.BackBufferFormat,
                D3DFMT_D24S8))
        {
            Present.AutoDepthStencilFormat = D3DFMT_D24S8;

            _RwHasStencilBuffer = TRUE;
        }
        else
        {
            if (D3D_OK ==
                IDirect3D9_CheckDeviceFormat(_RwDirect3DObject,
                    _RwD3DAdapterIndex,
                    _RwD3DAdapterType,
                    adapterFormat,
                    D3DUSAGE_DEPTHSTENCIL,
                    D3DRTYPE_SURFACE,
                    D3DFMT_D24X4S4)
                && D3D_OK ==
                IDirect3D9_CheckDepthStencilMatch(_RwDirect3DObject,
                    _RwD3DAdapterIndex,
                    _RwD3DAdapterType,
                    adapterFormat,
                    Present.BackBufferFormat,
                    D3DFMT_D24X4S4))
            {
                Present.AutoDepthStencilFormat = D3DFMT_D24X4S4;

                _RwHasStencilBuffer = TRUE;
            }
            else
            {
                /* !!!! we don't have stencil buffer */
                if (D3D_OK ==
                    IDirect3D9_CheckDeviceFormat(_RwDirect3DObject,
                        _RwD3DAdapterIndex,
                        _RwD3DAdapterType,
                        adapterFormat,
                        D3DUSAGE_DEPTHSTENCIL,
                        D3DRTYPE_SURFACE,
                        D3DFMT_D32)
                    && D3D_OK ==
                    IDirect3D9_CheckDepthStencilMatch(_RwDirect3DObject,
                        _RwD3DAdapterIndex,
                        _RwD3DAdapterType,
                        adapterFormat,
                        Present.BackBufferFormat,
                        D3DFMT_D32))
                {
                    Present.AutoDepthStencilFormat = D3DFMT_D32;

                    _RwHasStencilBuffer = FALSE;
                }
                else
                {
                    if (D3D_OK ==
                        IDirect3D9_CheckDeviceFormat(_RwDirect3DObject,
                            _RwD3DAdapterIndex,
                            _RwD3DAdapterType,
                            adapterFormat,
                            D3DUSAGE_DEPTHSTENCIL,
                            D3DRTYPE_SURFACE,
                            D3DFMT_D24X8)
                        && D3D_OK ==
                        IDirect3D9_CheckDepthStencilMatch
                        (_RwDirect3DObject, _RwD3DAdapterIndex,
                            _RwD3DAdapterType, adapterFormat,
                            Present.BackBufferFormat,
                            D3DFMT_D24X8))
                    {
                        Present.AutoDepthStencilFormat = D3DFMT_D24X8;

                        _RwHasStencilBuffer = FALSE;
                    }
                    else
                    {
                        _RwD3D9ZBufferDepth = 16;
                    }
                }
            }
        }
    }

    if (_RwD3D9ZBufferDepth == 16)
    {
        if (D3D_OK ==
            IDirect3D9_CheckDeviceFormat(_RwDirect3DObject,
                _RwD3DAdapterIndex,
                _RwD3DAdapterType,
                adapterFormat,
                D3DUSAGE_DEPTHSTENCIL,
                D3DRTYPE_SURFACE,
                D3DFMT_D15S1)
            && D3D_OK ==
            IDirect3D9_CheckDepthStencilMatch(_RwDirect3DObject,
                _RwD3DAdapterIndex,
                _RwD3DAdapterType,
                adapterFormat,
                Present.BackBufferFormat,
                D3DFMT_D15S1))
        {
            Present.AutoDepthStencilFormat = D3DFMT_D15S1;

            _RwHasStencilBuffer = TRUE;
        }
        else
        {
            Present.AutoDepthStencilFormat = D3DFMT_D16;

            _RwHasStencilBuffer = FALSE;
        }
    }

    Present.MultiSampleType = D3DMULTISAMPLE_NONE;
    Present.MultiSampleQuality = 0;

    if ( EnableFullScreenDialogBoxMode == FALSE &&
        (SelectedMultisamplingLevels > 1 ||
            SelectedMultisamplingLevelsNonMask > 1) )
    {
        DWORD maxQualityLevels = 0;
        HRESULT hr;

        hr = IDirect3D9_CheckDeviceMultiSampleType(_RwDirect3DObject,
            _RwD3DAdapterIndex,
            _RwD3DAdapterType,
            Present.BackBufferFormat,
            Present.Windowed,
            (D3DMULTISAMPLE_TYPE)
            SelectedMultisamplingLevels,
            &maxQualityLevels);

        if (SUCCEEDED(hr) &&
            maxQualityLevels > 0)
        {
            Present.MultiSampleType = ((D3DMULTISAMPLE_TYPE)
                SelectedMultisamplingLevels);

            Present.MultiSampleQuality = maxQualityLevels - 1;
        }
        else
        {
            maxQualityLevels = 0;

            /* Check non-mask types */
            hr = IDirect3D9_CheckDeviceMultiSampleType(_RwDirect3DObject,
                _RwD3DAdapterIndex,
                _RwD3DAdapterType,
                Present.BackBufferFormat,
                Present.Windowed,
                D3DMULTISAMPLE_NONMASKABLE,
                &maxQualityLevels);

            if (SUCCEEDED(hr) &&
                maxQualityLevels > 0)
            {
                RwUInt32 qualityLevel;

                qualityLevel = (SelectedMultisamplingLevelsNonMask - 2);

                if (qualityLevel > (maxQualityLevels - 1))
                {
                    qualityLevel = (maxQualityLevels - 1);
                }

                Present.MultiSampleType = D3DMULTISAMPLE_NONMASKABLE;
                Present.MultiSampleQuality = qualityLevel;
            }
        }

        if (Present.MultiSampleType != D3DMULTISAMPLE_NONE)
        {
            Present.SwapEffect = D3DSWAPEFFECT_DISCARD;
        }
    }

    RWRETURNVOID();
}
};

void notsa::InjectWindowedModeHooks() {
    RH_ScopedNamespaceName("WindowedModeHooks");
    RH_ScopedCategory("RenderWare");

    //memset((void*)0x7481CD, 0x90, 16);

    if (GetD3D9Device()) {
        notsa::WindowedMode::D3D9ProxyDevice::Initialize(GetD3D9Device());
    }

    RH_ScopedGlobalInstall(notsa::WindowedMode::HookDirect3DDeviceReplacerSA, 0x7F6781);
    //RH_ScopedGlobalInstall(notsa::WindowedMode::HookDxCamFix, 0x7F7C41);


    RH_ScopedGlobalInstall(RenderWare::_rwD3D9CameraClear, 0x7F7730);
    //RH_ScopedGlobalInstall(RenderWare::D3D9SetPresentParameters, 0x7F6CB0);
}
