#pragma once

namespace wmbridge {
    void  Init();
    void  InstallHooks();
    float GetStringWidth(const char* text, bool getAll, bool script);
    short ProcessCurrentString(bool print, float x, float y, const char* text);
    void  RenderFontBuffer();
}
