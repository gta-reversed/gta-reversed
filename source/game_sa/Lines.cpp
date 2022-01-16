#include "StdInc.h"

#include "Lines.h"

// 0x6FF460
void CLines::RenderLineNoClipping(float startX, float startY, float startZ, float endX, float endY, float endZ, uint32 startColor, uint32 endColor) {
    plugin::Call<0x6FF460, float, float, float, float, float, float, uint32, uint32>(startX, startY, startZ, endX, endY, endZ, startColor, endColor);
}

// 0x6FF4F0
void CLines::RenderLineWithClipping(float startX, float startY, float startZ, float endX, float endY, float endZ, uint32 startColor, uint32 endColor) {
    plugin::Call<0x6FF4F0, float, float, float, float, float, float, uint32, uint32>(startX, startY, startZ, endX, endY, endZ, startColor, endColor);
}

// 0x6FF790
void CLines::ImmediateLine2D(int32 startX, int32 startY, int32 endX, int32 endY, uint8 startR, uint8 startG, uint8 startB, uint8 startA, uint8 endR, uint8 endG, uint8 endB, uint8 endA) {
    plugin::Call<0x6FF790, int32, int32, int32, int32, uint8, uint8, uint8, uint8, uint8, uint8, uint8, uint8>(startX, startY, endX, endY, startR, startG, startB, startA, endR, endG, endB, endA);
}
