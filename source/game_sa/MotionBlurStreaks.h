/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class RegisteredMotionBlurStreak;

class CMotionBlurStreaks {
public:
    static constexpr uint32 MAX_NUM_MOTIONBLUR_STREAKS = 4; // default 4
    static inline auto (&aStreaks)[MAX_NUM_MOTIONBLUR_STREAKS] = StaticRef<CRegisteredMotionBlurStreak[MAX_NUM_MOTIONBLUR_STREAKS]>(0xC7CA08); // static CRegisteredMotionBlurStreak aStreaks[4]

    static constexpr uint32 NUM_STREAK_VERTICES = 4;
    static inline auto (&aStreakVertices)[NUM_STREAK_VERTICES] = StaticRef<RxObjSpace3DVertex[NUM_STREAK_VERTICES]>(0xC7C628);
    static constexpr uint32 NUM_STREAK_INDICES = 12;
    static inline auto (&aIndices)[NUM_STREAK_INDICES] = StaticRef<RxVertexIndex[NUM_STREAK_INDICES]>(0xC7C6B8);

public:
    static void InjectHooks();

    static void Init();
    static void Update();
    static void Render();
    static void RegisterStreak(uint32 id, uint8 red, uint8 green, uint8 blue, uint8 alpha, CVector leftPoint, CVector rightPoint);
};
