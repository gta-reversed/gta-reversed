#include "StdInc.h"

#pragma comment(lib, "Shlwapi.lib")
#include <Shlwapi.h>
#include <Wincodecsdk.h>

#include <wil/com.h>

// Vanilla used the old D3DX library, which requires old the DirectX SDK. Nasty stuff.
// Implemented with the new WIC API. No Windows XP support.

void JPegPlugin::InjectHooks() {
    RH_ScopedNamespace(JPegPlugin);
    RH_ScopedCategory("Plugins");

    // These functions are not ABI and API compatible with SA.
    // You'll be awarded a free crash if you leave one of them hooked yet others unhooked.
    RH_ScopedGlobalInstall(JPegCompressScreen, 0x5D0470, {.locked = true});
    RH_ScopedGlobalInstall(JPegCompressScreenToFile, 0x5D0820, {.locked = true});
    RH_ScopedGlobalInstall(JPegCompressScreenToBuffer, 0x5D0740, {.locked = true});

    RH_ScopedGlobalInstall(JPegDecompressToRaster, 0x5D05F0);
    RH_ScopedGlobalInstall(JPegDecompressToVramFromBuffer, 0x5D07A0);
}

// 0x5D0470
void JPegCompressScreen(RwCamera* camera, IStream* stream) {
    const auto scr = RsGrabScreen(camera);

    // Some parts are from: https://stackoverflow.com/a/30138664

    wil::com_ptr<IWICImagingFactory>    factory{};
    wil::com_ptr<IWICBitmapEncoder>     encoder{};
    wil::com_ptr<IWICBitmapFrameEncode> frame{};

#define HRCHK(hr) if (FAILED((hr))) { NOTSA_LOG_ERR("Couldn't encode screenshot. Your capture is not saved."); return; }

    HRCHK(CoInitialize(NULL));
    HRCHK(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)));

    HRCHK(factory->CreateEncoder(GUID_ContainerFormatJpeg, nullptr, &encoder));
    HRCHK(encoder->Initialize(stream, WICBitmapEncoderNoCache));
    HRCHK(encoder->CreateNewFrame(&frame, nullptr));
    HRCHK(frame->Initialize(nullptr));
    HRCHK(frame->SetSize(RwImageGetWidth(scr), RwImageGetHeight(scr)));

    GUID format{ GUID_WICPixelFormat24bppBGR };
    HRCHK(frame->SetPixelFormat(&format));

    {
        // Convert raw RGBA to JPEG friendly BGR.
        wil::com_ptr<IWICBitmap>          source{};
        wil::com_ptr<IWICBitmapSource>    converted{};
        wil::com_ptr<IWICFormatConverter> fmtConverter{};

        HRCHK(factory->CreateBitmapFromMemory(
            RwImageGetWidth(scr),
            RwImageGetHeight(scr),
            GUID_WICPixelFormat32bppRGBA,
            RwImageGetStride(scr),
            RwImageGetWidth(scr) * RwImageGetHeight(scr) * sizeof(RwRGBA),
            RwImageGetPixels(scr),
            &source
        ));
        HRCHK(factory->CreateFormatConverter(&fmtConverter));
        HRCHK(fmtConverter->Initialize(source.get(), format, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom));
        HRCHK(fmtConverter->QueryInterface(__uuidof(IWICBitmapSource), converted.put_void()));

        HRCHK(frame->WriteSource(converted.get(), NULL));
    }
    HRCHK(frame->Commit());
    HRCHK(encoder->Commit());

    CoUninitialize();

#undef HR_FAIL_RET
}

// 0x5D0820
void JPegCompressScreenToFile(RwCamera* camera, const char* path) {
    // Originally used CFileMgr::OpenFile with its handle saved to a static variable
    // for passing it to libjpeg callbacks.
    // 
    // I could've done the same thing for the IStream* but it seemed unnecessary.
    wil::com_ptr<IStream> stream{};
    if (SUCCEEDED(SHCreateStreamOnFileEx(
        UTF8ToUnicode(path).c_str(),
        STGM_CREATE | STGM_WRITE,
        FILE_ATTRIBUTE_NORMAL,
        true,
        NULL,
        stream.addressof())
    )) {
        JPegCompressScreen(camera, stream.get());
    }
}

// 0x5D0740 - unused
void JPegCompressScreenToBuffer(RwCamera* camera, uint8*& buffer, uint32& size) {
    // Originally same thing on JPegCompressScreenToFile but for in memory buffers.
    JPegCompressScreen(camera, SHCreateMemStream(buffer, size));
}

// 0x5D05F0 - unused, maybe remnant of PS2 where you can see the gallery in game.
void JPegDecompressToRaster(RwRaster* raster, IStream* stream) {
    // it can be implemented with wic as well but whatever
}

// 0x5D07A0 - unused
bool JPegDecompressToVramFromBuffer(RwRaster* raster, uint8** unk) {
    static auto s_ScreenshotFileBuf = StaticRef<std::array<uint8, 204'800>>(0xBD0B78);

    if (!unk) {
        return false;
    }

    std::memcpy(s_ScreenshotFileBuf.data(), FrontEndMenuManager.m_GalleryImageBuffer, s_ScreenshotFileBuf.size());
    JPegDecompressToRaster(raster, SHCreateMemStream(s_ScreenshotFileBuf.data(), s_ScreenshotFileBuf.size()));
    return true;
}
