# CJK text pipeline

This folder is a port of the **wm_sachs** text pipeline that ships with
[LC.VC.SA.Plugin](https://github.com/)，MIT License, Copyright (c) 2019 无名汉化组.

Only the glyph *source* was changed: instead of sampling the original 4096x4096 PNG
atlas, glyph bitmaps are generated at runtime with DirectWrite (`WmDWrite.cpp`) using
the same cell geometry as the atlas (64x80 texels mapped onto the game's 32x20 cell,
4 px per unit, SimHei bold).

| file | purpose |
| --- | --- |
| `WmFont.*` | UTF-8 aware `ProcessCurrentString`, per-glyph `RenderFontBuffer`, `GetStringWidth` |
| `WmCharTable.*` | codepoint -> atlas cell table (kept for the ASCII side) |
| `WmSprite2d.*`, `WmSprite.*` | `CSprite2d` / `CSprite` wrappers used by the renderer |
| `WmTxdStore.*`, `WmRw.*`, `WmScriptTextDrawer.*` | small engine wrappers |
| `WmDWrite.*` | DirectWrite glyph cache (white glyph + alpha coverage) |
| `WmCompat.h` | self-contained replacements for `injector` / `utf8cpp` |

Entry points are patched from `source/game_sa/WmBridge.cpp`:

* `0x71A220` ProcessCurrentString
* `0x719840` RenderFontBuffer
* `0x71A210` DrawFonts (jmp)
* the wm_sachs call-site list for `GetStringWidth` / `RenderFontBuffer`