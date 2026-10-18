#include <windows.h>
#include <cstring>

#include "wm/WmFont.h"
#include "wm/WmRw.h"
#include "wm/WmSprite.h"
#include "wm/WmSprite2d.h"
#include "wm/WmTxdStore.h"
#include "wm/WmScriptTextDrawer.h"
#include "wm/WmCharTable.h"

#include "WmBridge.h"

namespace {

bool g_ready = false;

void InstallJmp(uintptr_t addr, void* fn) {
    DWORD old = 0;
    if (!VirtualProtect((void*)addr, 5, PAGE_EXECUTE_READWRITE, &old))
        return;
    *(uint8_t*)addr = 0xE9;
    *(int32_t*)(addr + 1) = (int32_t)((uintptr_t)fn - (addr + 5));
    VirtualProtect((void*)addr, 5, old, &old);
    FlushInstructionCache(GetCurrentProcess(), (void*)addr, 5);
}

void InstallCall(uintptr_t site, void* fn) {
    // redirect the rel32 of an existing `call` instruction (wm_sachs style)
    if (*(uint8_t*)site != 0xE8)
        return;
    DWORD old = 0;
    if (!VirtualProtect((void*)site, 5, PAGE_EXECUTE_READWRITE, &old))
        return;
    *(int32_t*)(site + 1) = (int32_t)((uintptr_t)fn - (site + 5));
    VirtualProtect((void*)site, 5, old, &old);
    FlushInstructionCache(GetCurrentProcess(), (void*)site, 5);
}


} // namespace

void wmbridge::InstallHooks() {
    static bool installed = false;
    if (installed) return;
    installed = true;

    // statics must be live before the game can reach the patched entry points
    WmRw::Init10U();
    WmSprite::Init10U();
    WmSprite2d::Init10U();
    WmTxdStore::Init10U();
    WmScriptTextDrawer::Init10U();
    WmFont::Init10U();
    WmCharTable::InitTable();

    // exact wm_sachs (1.0 US) patch list -- entry points stay original
    static const uintptr_t kWidthCalls[] = {
        0x47B73A, 0x57A49B, 0x57FB52, 0x57FE35, 0x5814A7, 0x581512, 0x58BCCC
    };
    static const uintptr_t kProcessCalls[] = {
        0x71A5F1, 0x71A611, 0x71A631, 0x71A802, 0x71A834
    };
    static const uintptr_t kRenderCalls[] = {
        0x57BF70, 0x719B5D, 0x719F43
    };

    for (uintptr_t a : kWidthCalls)   InstallCall(a, (void*)WmFont::GetStringWidth);

    // full coverage: the game still reaches these entries directly in many places
    InstallJmp(0x71A220, (void*)WmFont::ProcessCurrentString);
    InstallJmp(0x719840, (void*)WmFont::RenderFontBuffer);
    for (uintptr_t a : kProcessCalls) InstallCall(a, (void*)WmFont::ProcessCurrentString);
    for (uintptr_t a : kRenderCalls)  InstallCall(a, (void*)WmFont::RenderFontBuffer);

    InstallJmp(0x71A210, (void*)WmFont::RenderFontBuffer);
}

void wmbridge::Init() {
    InstallHooks();
}

float wmbridge::GetStringWidth(const char* text, bool getAll, bool script) {
    Init();
    return WmFont::GetStringWidth(text, getAll, script);
}

short wmbridge::ProcessCurrentString(bool print, float x, float y, const char* text) {
    Init();
    return WmFont::ProcessCurrentString(print, x, y, text);
}

void wmbridge::RenderFontBuffer() {
    Init();
    WmFont::RenderFontBuffer();
}
