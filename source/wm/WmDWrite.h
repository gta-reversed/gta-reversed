#pragma once

#include <cstdint>

struct RwTexture;

// Per-codepoint glyph textures generated with DirectWrite.
namespace WmDWrite {
    // px is the height in pixels of the rendered cell (supersampled by the caller);
    // the canvas keeps the game's 32:20 character cell aspect.
    RwTexture* GetGlyph(uint32_t codepoint, int px);
}
