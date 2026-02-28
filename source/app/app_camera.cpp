#include "StdInc.h"

#include "app.h"

// 0x5BA490
RwCamera* CameraCreate(RwInt32 width, RwInt32 height, RwBool zBuffer) {
    UNUSED(width);
    UNUSED(height);

    RwCamera* camera = RwCameraCreate();
    if (camera) {
        VERIFY(RwCameraSetFrame(camera, RwFrameCreate()));
        VERIFY(RwCameraSetRaster(camera, RwRasterCreate(width, height, 0, rwRASTERTYPECAMERA)));
        if (zBuffer) {
            VERIFY(RwCameraSetZRaster(camera, RwRasterCreate(width, height, 0, rwRASTERTYPEZBUFFER)));
        }

        return camera;
    }

    CameraDestroy(camera);
    return nullptr;
}

// 0x72FD90
void CameraDestroy(RwCamera* camera) {
    if (!camera) {
        return;
    }

    RwFrame* frame = RwCameraGetFrame(camera);
    if (frame) {
        RwCameraSetFrame(camera, nullptr);
        VERIFY(RwFrameDestroy(frame));
    }

    const auto RasterDestroy = [](RwRaster* raster) {
        if (raster) {
            RwRaster* parent = RwRasterGetParent(raster);
            VERIFY(RwRasterDestroy(raster));
            if (parent && parent != raster) {
                VERIFY(RwRasterDestroy(parent));
            }
        }
    };

    RasterDestroy(RwCameraGetRaster(camera));
    RasterDestroy(RwCameraGetZRaster(camera));

    RwCameraDestroy(camera);
}

// 0x72FC70
void CameraSize(RwCamera* camera, RwRect* rect, RwReal viewWindow, RwReal aspectRatio) {
    if (!camera) {
        return;
    }

#ifdef NOTSA_WINDOWED_MODE
    // WindowedMode
    if (notsa::IsFixBugs()) {
        RwCameraClear(Scene.m_pRwCamera, &gColourTop, 2);
    }
#endif

    static RwRect origSize = { 0, 0, 0, 0 };

    if (!origSize.w || !origSize.h) {
        origSize.x = origSize.y = 0;
        origSize.w = RwRasterGetWidth(RwCameraGetRaster(camera));
        origSize.h = RwRasterGetHeight(RwCameraGetRaster(camera));
    }

    if (!rect) {
        static RwRect r;

        rect = &r;

        rect->w = RwRasterGetWidth(RwCameraGetRaster(camera));
        rect->h = RwRasterGetHeight(RwCameraGetRaster(camera));
        rect->x = rect->y = 0;
    }

    RwVideoMode vmi;
    RwEngineGetVideoModeInfo(&vmi, RwEngineGetCurrentVideoMode());

    if (vmi.flags & rwVIDEOMODEEXCLUSIVE) {
        rect->x = rect->y = 0;
        rect->w = vmi.width;
        rect->h = vmi.height;
    }

    if (rect->w > 0 && rect->h > 0) {
        RwRaster* raster  = RwCameraGetRaster(camera);
        raster->width  = rect->w;
        raster->height = rect->h;

        RwRaster* zRaster = RwCameraGetZRaster(camera);
        zRaster->width  = rect->w;
        zRaster->height = rect->h;

        CVector2D vw;
        if (vmi.flags & rwVIDEOMODEEXCLUSIVE) {
            vw.x = viewWindow;
            vw.y = viewWindow / aspectRatio;
        } else {
            if (rect->w > rect->h) {
                vw.x = viewWindow;
                vw.y = ((float)rect->h * viewWindow) / (float)rect->w;
            } else {
                vw.x = ((float)rect->w * viewWindow) / (float)rect->h;
                vw.y = viewWindow;
            }
        }

        RwCameraSetViewWindow(camera, &vw);

        RsGlobal.maximumWidth  = rect->w;
        RsGlobal.maximumHeight = rect->h;
    }
}
