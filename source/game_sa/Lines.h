#pragma once

class CLines {
public:
    static void RenderLineNoClipping(float startX, float startY, float startZ, float endX, float endY, float endZ, uint32 startColor, uint32 endColor);
    static void RenderLineWithClipping(float startX, float startY, float startZ, float endX, float endY, float endZ, uint32 startColor, uint32 endColor);
    static void ImmediateLine2D(int32 startX, int32 startY, int32 endX, int32 endY, uint8 startR, uint8 startG, uint8 startB, uint8 startA, uint8 endR, uint8 endG, uint8 endB, uint8 endA);

public:
    // NOTSA wrappers
    static void RenderLineNoClipping(const CVector& start, const CVector& end, uint32 startColor, uint32 endColor) {
        RenderLineNoClipping(start.x, start.y, start.z, end.x, end.y, end.z, startColor, endColor);
    }

    static void RenderLineWithClipping(const CVector& start, const CVector& end, uint32 startColor, uint32 endColor) {
        RenderLineWithClipping(start.x, start.y, start.z, end.x, end.y, end.z, startColor, endColor);
    }

    static void ImmediateLine2D(const CVector2D& start, const CVector2D& end, const CRGBA& startColor, const CRGBA& endColor) {
        ImmediateLine2D(
            static_cast<int32>(start.x), static_cast<int32>(start.y),
            static_cast<int32>(end.x), static_cast<int32>(end.y),
            startColor.r, startColor.g, startColor.b, startColor.a,
            endColor.r, endColor.g, endColor.b, endColor.a
        );
    }

    static void RenderLineCircleNoClipping(const CVector& centerPoint, float radius, uint32 numLines, uint32 color) {
        assert(numLines > 0);
        auto angleDiff = TWO_PI / numLines;
        for (float angle = 0; angle <= TWO_PI; angle += angleDiff) { //You are using radians so you will have to increase by a very small amount
            auto vecStart = centerPoint + CVector{ radius * cos(angle), radius * sin(angle), 0.f };
            auto vecEnd   = centerPoint + CVector{ radius * cos(angle + angleDiff), radius * sin(angle + angleDiff), 0.f };
            RenderLineNoClipping(vecStart, vecEnd, color, color);
        }
    }
};
