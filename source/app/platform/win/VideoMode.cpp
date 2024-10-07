#include "StdInc.h"

#include "VideoMode.h"
#include "platform.h"
#include "PostEffects.h"

// todo: "2014/12/07 [nick7]: is it related to streaming memory (45M default?)?"
#define GAME_FREE_VIDEO_MEM_REQUIRED (45 * 1024 * 1024)

void VideoModeInjectHooks() {
    RH_ScopedNamespaceName("VideoMode");
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(GetVideoModeList, 0x745AF0);
    RH_ScopedGlobalInstall(FreeVideoModeList, 0x745A80);
    RH_ScopedGlobalInstall(SetVideoMode, 0x745C70);
    RH_ScopedGlobalInstall(IsVideoModeExclusive, 0x745CA0);
}

// 0x745AF0
char** GetVideoModeList() {
    if (gVideoModes) {
        if (gCurrentGpu == RwEngineGetCurrentSubSystem()) {
            return gVideoModes;
        }
        FreeVideoModeList();
    }
    gCurrentGpu = RwEngineGetCurrentSubSystem();

    uint32 numVidModes = RwEngineGetNumVideoModes();
    assert(numVidModes != -1 && "Failed to get Video Modes");

    gVideoModes = (char**)CMemoryMgr::Calloc(numVidModes, sizeof(char*));

    for (auto modeId = 0u; modeId < numVidModes; modeId++) {
        const auto videoMode = RwEngineGetVideoModeInfo(modeId);

        gVideoModes[modeId] = nullptr;
        if ((videoMode.flags & rwVIDEOMODEEXCLUSIVE) == 0) {
#ifdef VIDEO_MODE_LOGS
            DEV_LOG("Unavailable video mode id={:02d}: {} X {} X {} [reason: video mode not exclusive]", modeId, videoMode.width, videoMode.height, videoMode.depth);
#endif
            continue;
        }

        if (videoMode.width < APP_MINIMAL_WIDTH || videoMode.height < APP_MINIMAL_HEIGHT) {
#ifdef VIDEO_MODE_LOGS
            DEV_LOG("Unavailable video mode id={:02d}: {} X {} X {} [reason: size]", modeId, videoMode.width, videoMode.height, videoMode.depth);
#endif
            continue;
        }

        float fRatio = float(videoMode.height) / float(videoMode.width);
        if (!IS_FULLSCREEN_RATIO(fRatio) && !IS_WIDESCREEN_RATIO(fRatio)) {
#ifdef VIDEO_MODE_LOGS
            DEV_LOG("Unavailable video mode id={:02d}: {} X {} X {} [reason: ratio {:0.2f}]", modeId, videoMode.width, videoMode.height, videoMode.depth, fRatio);
#endif
            continue;
        }

        if (videoMode.width != APP_MINIMAL_WIDTH || videoMode.height != APP_MINIMAL_HEIGHT) {
            if (s_OSStatus.VRAM.Avail - videoMode.height * videoMode.width * videoMode.depth / 8 <= GAME_FREE_VIDEO_MEM_REQUIRED) {
                continue;
            }
        }

        gVideoModes[modeId] = (char*)CMemoryMgr::Calloc(100, sizeof(char));                                  // 100 chars
        sprintf_s(gVideoModes[modeId], 100 * sizeof(char), "%lu X %lu X %lu", videoMode.width, videoMode.height, videoMode.depth); // rwsprintf

#ifdef VIDEO_MODE_LOGS
        DEV_LOG("Available video mode id={:02d}: {}", modeId, gVideoModes[modeId]);
#endif
    }

    return gVideoModes;
}

// 0x745A80
bool FreeVideoModeList() {
    auto numVidModes = RwEngineGetNumVideoModes();
    if (gVideoModes) {
        for (auto mode = 0; mode < numVidModes; mode++) {
            CMemoryMgr::Free(gVideoModes[mode]);
        }
        CMemoryMgr::Free(gVideoModes);
        gVideoModes = nullptr;
    }
    return true;
}

// 0x745C70
void SetVideoMode(int32 mode) {
    assert(mode < RwEngineGetNumVideoModes());

#ifdef VIDEO_MODE_LOGS
    DEV_LOG("Changing Video Mode to {} ({})", mode, gVideoModes ? gVideoModes[mode] : "unknown");
#endif

    RwD3D9ChangeVideoMode(mode);
    CPostEffects::SetupBackBufferVertex();
    RsGlobal.maximumWidth  = RwRasterGetWidth(RwCameraGetRaster(Scene.m_pRwCamera));
    RsGlobal.maximumHeight = RwRasterGetHeight(RwCameraGetRaster(Scene.m_pRwCamera));
}

// 0x745CA0
bool IsVideoModeExclusive() { // AKA isCurrentModeFullscreen
    return RwEngineGetVideoModeInfo(gCurrentVideoMode).flags & rwVIDEOMODEEXCLUSIVE;
}
