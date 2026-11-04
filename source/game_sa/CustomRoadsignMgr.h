#pragma once

#include "Vector.h"

#define SIGN_CHAR_WIDTH 8
#define SIGN_CHAR_HEIGHT 16
#define SIGN_PIXEL_BYTES 4

class CCustomRoadsignMgr {
public:
    static inline auto& pCharsetTex = StaticRef<RwTexture*>(0xC3EF84);
    static inline auto& pCharsetLockedRaster = StaticRef<RwUInt8*>(0xC3EF88);
    static inline auto& pCharsetLockedPallete = StaticRef<RwUInt8*>(0xC3EF8C); // unused, never really locked
public:
    static void InjectHooks();

    static bool Initialise();
    static void Shutdown();
    static bool RenderTest(const CVector& vecUnused) { return true; };
    static void DebugDrawInternalTexture() {}
    static RwTexture* CreateRoadsignTexture(char* name, int32 numOfChars);
    static RwTexture* SetupRoadsignAtomic(RpAtomic* atomic, char* name, int32 numOfChars);
    static RpAtomic* SetAtomicAlpha(RpAtomic* atomic, uint8 alpha);
    static RpAtomic* CreateRoadsignAtomicA(float fWidth, float fHeight, int32 numLines, char* line1, char* line2, char* line3, char* line4, int32 lettersPerLine, uint8 ucPallete);
    static RpAtomic* CreateRoadsignAtomic(float xScale, float yScale, int32 numLines, char* line1, char* line2, char* line3, char* line4, int32 lettersPerLine, uint8 ucPallete);
    static RpAtomic* RenderRoadsignAtomic(RpAtomic* atomic, const CVector& vecPos);
};

RpMaterial* RoadsignSetMaterialAlphaCB(RpMaterial* material, void* data); //data is uint8 alpha
RpMaterial* RoadsignSetMaterialTextureCB(RpMaterial* material, void* data); //data is RwTexture*
bool RoadsignGenerateTextRaster(char* roadName, int32 numLetters, RwRaster* charsetRaster, int32 unused, RwRaster* signRaster);
void RoadsignGetLineAndRow(char cLetter, int32* line, int32* row); // Weird calling convention in SA, passing argument in <al>, line is column
