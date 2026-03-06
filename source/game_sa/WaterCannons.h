#pragma once

class CVector;
class CWaterCannon;

class CWaterCannons {
public:
    static constexpr uint32 MAX_WATER_CANNONS = 3;
    static inline auto (&aCannons)[3] = StaticRef<CWaterCannon[3]>(0xC80740);

public:
    static void InjectHooks();

    static void Init();
    static void UpdateOne(uint32 vehicle, CVector* start, CVector* end);
    static void Update();
    static void Render();
};
