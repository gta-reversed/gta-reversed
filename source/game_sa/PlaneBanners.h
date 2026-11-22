#pragma once
#include "Base.h"
#include "Vector.h"

class CPlaneBanners {
public:
    static void Init();
    static void RegisterPoint(CVector point, uint32 bannerIndex);
    static void Render();
    static void Update();
};
