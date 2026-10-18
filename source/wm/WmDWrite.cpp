#include <windows.h>
#include <dwrite.h>
#include <cstdint>
#include <cstring>

#include "WmDWrite.h"

#pragma comment(lib, "dwrite.lib")

namespace {

const char* kFontName = "SimHei";   // wm_sachs atlas uses a heavy 榛戜綋 face

struct RwImageLayout {
    int32_t flags, width, height, depth, stride;
    uint8_t* cpPixels;
    void* palette;
};

using RwImageCreateFn    = RwImageLayout* (*)(int, int, int);
using RwImageAllocFn     = RwImageLayout* (*)(RwImageLayout*);
using RwImageDestroyFn   = int  (*)(RwImageLayout*);
using RwRasterCreateFn   = void* (*)(int, int, int, int);
using RwRasterSetImageFn = void* (*)(void*, RwImageLayout*);
using RwTextureCreateFn  = RwTexture* (*)(void*);

RwImageCreateFn    pImageCreate  = nullptr;
RwImageAllocFn     pImageAlloc   = nullptr;
RwImageDestroyFn   pImageDestroy = nullptr;
RwRasterCreateFn   pRasterCreate = nullptr;
RwRasterSetImageFn pRasterSet    = nullptr;
RwTextureCreateFn  pTextureCreate = nullptr;

IDWriteFactory*         g_factory = nullptr;
IDWriteGdiInterop*      g_gdi = nullptr;
IDWriteTextFormat*      g_fmt = nullptr;
IDWriteRenderingParams* g_params = nullptr;
int                     g_fmtPx = 0;

struct Entry {
    uint32_t  cp;
    int       px;
    RwTexture* tex;
};
Entry g_cache[512];
int   g_count = 0;

void InitRw() {
    if (pImageCreate) return;
    pImageCreate   = (RwImageCreateFn)0x8026E0;
    pImageAlloc    = (RwImageAllocFn)0x8027A0;
    pImageDestroy  = (RwImageDestroyFn)0x802740;
    pRasterCreate  = (RwRasterCreateFn)0x7FB230;
    pRasterSet     = (RwRasterSetImageFn)0x804290;
    pTextureCreate = (RwTextureCreateFn)0x7F37C0;
}

class Renderer final : public IDWriteTextRenderer {
public:
    IDWriteBitmapRenderTarget* m_target;
    LONG m_ref = 1;
    explicit Renderer(IDWriteBitmapRenderTarget* t) : m_target(t) {}

    ULONG STDMETHODCALLTYPE AddRef() override { return (ULONG)InterlockedIncrement(&m_ref); }
    ULONG STDMETHODCALLTYPE Release() override { return (ULONG)InterlockedDecrement(&m_ref); }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IDWritePixelSnapping) ||
            riid == __uuidof(IDWriteTextRenderer)) {
            *ppv = this; AddRef(); return S_OK;
        }
        *ppv = nullptr; return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE IsPixelSnappingDisabled(void*, BOOL* d) override { *d = FALSE; return S_OK; }
    HRESULT STDMETHODCALLTYPE GetCurrentTransform(void*, DWRITE_MATRIX* m) override {
        m->m11 = 1; m->m12 = 0; m->m21 = 0; m->m22 = 1; m->dx = 0; m->dy = 0; return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetPixelsPerDip(void*, FLOAT* p) override { *p = 1.0f; return S_OK; }
    HRESULT STDMETHODCALLTYPE DrawGlyphRun(void*, FLOAT x, FLOAT y, DWRITE_MEASURING_MODE mm,
                                           DWRITE_GLYPH_RUN const* run, DWRITE_GLYPH_RUN_DESCRIPTION const*,
                                           IUnknown*) override {
        return m_target->DrawGlyphRun(x, y, mm, run, g_params, RGB(255, 255, 255), nullptr);
    }
    HRESULT STDMETHODCALLTYPE DrawUnderline(void*, FLOAT, FLOAT, DWRITE_UNDERLINE const*, IUnknown*) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE DrawStrikethrough(void*, FLOAT, FLOAT, DWRITE_STRIKETHROUGH const*, IUnknown*) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE DrawInlineObject(void*, FLOAT, FLOAT, IDWriteInlineObject*, BOOL, BOOL, IUnknown*) override { return S_OK; }
};

bool EnsureFactory(int px) {
    if (!g_factory) {
        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&g_factory)))
            return false;
        g_factory->GetGdiInterop(&g_gdi);
        g_factory->CreateCustomRenderingParams(2.2f, 0.5f, 0.0f, DWRITE_PIXEL_GEOMETRY_FLAT,
                                               DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC, &g_params);
    }
    if (g_fmt && g_fmtPx == px) return true;
    if (g_fmt) { g_fmt->Release(); g_fmt = nullptr; }
    wchar_t name[128];
    MultiByteToWideChar(CP_UTF8, 0, kFontName, -1, name, 128);
    if (FAILED(g_factory->CreateTextFormat(name, nullptr, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
                                           DWRITE_FONT_STRETCH_NORMAL, (FLOAT)px, L"", &g_fmt)))
        return false;
    g_fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    g_fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    g_fmtPx = px;
    return true;
}

RwTexture* CreateGlyph(uint32_t cp, int px) {
    // wm_sachs atlas geometry: 4096x4096 with 64x80 texel cells (64 columns, 51.2 rows).
    // A CJK glyph is drawn square at cell width inside that cell, so it ends up filling
    // the full 32-unit game cell width after the quad maps 64:80 -> 32:20.
    const int ch = px;
    const int cw = (int)(px * 64.0f / 80.0f + 0.5f);
    // atlas glyph ink reaches ~95% of the 64px cell width; oversize the em so ours fills it too
    const int fontSize = (int)(cw * 1.06f + 0.5f);

    if (!EnsureFactory(fontSize) || !g_gdi) return nullptr;

    wchar_t wc[2] = { (wchar_t)cp, 0 };
    IDWriteTextLayout* layout = nullptr;
    if (FAILED(g_factory->CreateTextLayout(wc, 1, g_fmt, (FLOAT)cw, (FLOAT)ch, &layout)) || !layout)
        return nullptr;
    layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    IDWriteBitmapRenderTarget* target = nullptr;
    if (FAILED(g_gdi->CreateBitmapRenderTarget(nullptr, (UINT32)cw, (UINT32)ch, &target)) || !target) {
        layout->Release();
        return nullptr;
    }
    target->SetPixelsPerDip(1.0f);

    HDC dc = target->GetMemoryDC();
    RECT r = { 0, 0, cw, ch };
    FillRect(dc, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));

    Renderer renderer(target);
    // CJK ink is centred in the cell by default, which reads slightly lower than the
    // game latin glyphs (their baseline sits higher). Lift it by this fraction.
    const float kBaselineLift = 0.06f;
    layout->Draw(nullptr, &renderer, 0.0f, -(FLOAT)ch * kBaselineLift);

    HBITMAP bmp = (HBITMAP)GetCurrentObject(dc, OBJ_BITMAP);
    DIBSECTION ds;
    uint8_t* bgra = nullptr;
    if (bmp && GetObjectW(bmp, sizeof(ds), &ds) != 0 && ds.dsBm.bmBits) {
        uint8_t* src = (uint8_t*)ds.dsBm.bmBits;
        int stride = ds.dsBm.bmWidthBytes;
        if (stride < 0) stride = -stride;
        bgra = new uint8_t[(size_t)cw * ch * 4];
        for (int row = 0; row < ch; ++row)
            memcpy(bgra + (size_t)row * cw * 4, src + (size_t)row * stride, (size_t)cw * 4);
    }
    layout->Release();
    target->Release();
    if (!bgra) return nullptr;

    // white glyph, coverage in alpha (vertex colour modulates it)
    for (int i = 0, e = cw * ch; i < e; ++i) {
        uint8_t a = bgra[i * 4 + 2];
        bgra[i * 4 + 0] = 255;
        bgra[i * 4 + 1] = 255;
        bgra[i * 4 + 2] = 255;
        bgra[i * 4 + 3] = a;
    }

    InitRw();
    RwImageLayout* img = pImageCreate(cw, ch, 32);
    if (!img) { delete[] bgra; return nullptr; }
    if (!pImageAlloc(img)) { pImageDestroy(img); delete[] bgra; return nullptr; }
    img->stride = cw * 4;
    for (int row = 0; row < ch; ++row)
        memcpy(img->cpPixels + (size_t)row * (cw * 4),
               bgra + (size_t)(ch - 1 - row) * (cw * 4), (size_t)cw * 4);
    delete[] bgra;

    void* ras = pRasterCreate(cw, ch, 0, 0x04 | 0x0500);
    if (!ras) { pImageDestroy(img); return nullptr; }
    if (!pRasterSet(ras, img)) { pImageDestroy(img); return nullptr; }
    pImageDestroy(img);

    return pTextureCreate(ras);
}

} // namespace

RwTexture* WmDWrite::GetGlyph(uint32_t cp, int px) {
    if (px < 4) px = 4;
    if (px > 512) px = 512;

    for (int i = 0; i < g_count; ++i)
        if (g_cache[i].cp == cp && g_cache[i].px == px)
            return g_cache[i].tex;

    RwTexture* tex = CreateGlyph(cp, px);
    if (!tex) return nullptr;

    if (g_count >= 512) g_count = 0;
    Entry& e = g_cache[g_count++];
    e.cp = cp; e.px = px; e.tex = tex;
    return tex;
}
