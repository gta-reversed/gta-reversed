#include "StdInc.h"
#include "jpeglib.h"

static std::array<uint8, 204'800> g_ScreenshotFileBuf    = StaticRef<std::array<uint8, 204'800>>(0xBD0B78);

// NOTSA. For debugging purposes, without using this callback,
// game terminates without notice in case of any libjpeg failure.
static void JPegErrorExit(j_common_ptr cinfo) {
    static char message[JMSG_LENGTH_MAX]{};

    const auto* einfo = cinfo->err;
    einfo->format_message(cinfo, message);
    NOTSA_UNREACHABLE("libjpeg error!\nCode:\n{}\nMessage:\n{}", einfo->msg_code, message);
}

void JPegPlugin::InjectHooks() {
    RH_ScopedNamespace(JPegPlugin);
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(JPegCompressScreen, 0x5D0470);
    RH_ScopedGlobalInstall(JPegCompressScreenToFile, 0x5D0820);
    RH_ScopedGlobalInstall(JPegCompressScreenToBuffer, 0x5D0740, {.reversed=false});
    RH_ScopedGlobalInstall(JPegDecompressToRaster, 0x5D05F0, {.reversed=false});
    RH_ScopedGlobalInstall(JPegDecompressToVramFromBuffer, 0x5D0320, {.reversed=false});
}

// 0x5D0470
void JPegCompressScreen(RwCamera* camera, jpeg_destination_mgr& dst) {
    jpeg_error_mgr       jerr{};
    jpeg_compress_struct cinfo{
        .err = jpeg_std_error(&jerr)
    };
    jerr.error_exit = JPegErrorExit;

    const auto image = RsGrabScreen(camera);
    jpeg_CreateCompress(&cinfo, JPEG_LIB_VERSION, sizeof(cinfo));

    // It was JCS_RGB originally, but libjpeg errors with 'bogus colorspace'.
    cinfo.in_color_space = JCS_EXT_RGBA;
    jpeg_set_defaults(&cinfo);

    cinfo.image_width      = RwRasterGetWidth(image);
    cinfo.image_height     = RwRasterGetHeight(image);
    cinfo.dest             = &dst;
    cinfo.dct_method       = JDCT_FLOAT;
    cinfo.input_components = 4;
    jpeg_start_compress(&cinfo, true);

    for (auto i = 0; i < cinfo.image_height; i++) {
        const auto* line = &RwImageGetPixels(image)[sizeof(RwRGBA) * i * cinfo.image_width];
        if (jpeg_write_scanlines(&cinfo, const_cast<uint8**>(&line), 1) != 1) {
            break;
        }
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    RwImageDestroy(image);
}

// 0x5D0820
void JPegCompressScreenToFile(RwCamera* camera, const char* path) {
    static FILESTREAM g_ScreenshotFileHandle{};

    if (g_ScreenshotFileHandle = CFileMgr::OpenFile(path, "wb")) {
        jpeg_destination_mgr dst{};

        // InitDestination (0x5D03E0)
        dst.init_destination = [](j_compress_ptr c) {
            c->dest->next_output_byte = g_ScreenshotFileBuf.data();
            c->dest->free_in_buffer   = g_ScreenshotFileBuf.size();
        };

        // HDEmptyOutputBuffer (0x5D0400)
        dst.empty_output_buffer = [](j_compress_ptr c) -> boolean {
            CFileMgr::Write(g_ScreenshotFileHandle, g_ScreenshotFileBuf.data(), g_ScreenshotFileBuf.size());
            c->dest->next_output_byte = g_ScreenshotFileBuf.data();
            c->dest->free_in_buffer   = g_ScreenshotFileBuf.size();
            return true;
        };

        // HDTermDestination (0x5D0440)
        dst.term_destination = [](j_compress_ptr c) {
            // NOTSA: signed -> unsigned
            const auto totalProcessed = (uintptr)c->dest->next_output_byte - (uintptr)g_ScreenshotFileBuf.data();
            if (totalProcessed > 0) {
                CFileMgr::Write(g_ScreenshotFileHandle, g_ScreenshotFileBuf.data(), totalProcessed);
            }
        };

        JPegCompressScreen(camera, dst);
        CFileMgr::CloseFile(g_ScreenshotFileHandle);
    }
}

// 0x5D0740
void JPegCompressScreenToBuffer(char** buffer, uint32* size) {
}

// 0x5D05F0 -- PS2 leftover
void JPegDecompressToRaster(RwRaster* raster, jpeg_source_mgr& src) {
}

// 0x5D0320 -- PS2 leftover
void JPegDecompressToVramFromBuffer(RwRaster* raster, RwInt8** unk) {
}
