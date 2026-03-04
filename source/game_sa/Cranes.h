#pragma once

// straight from III/VC
class CCranes {
public:
    static void InitCranes();
    static void UpdateCranes();
    static bool IsThisCarPickedUp(float x, float y, CVehicle* vehicle);
    static bool IsThisCarBeingCarriedByAnyCrane(CVehicle* vehicle);
    static bool IsThisCarBeingTargettedByAnyCrane(CVehicle* vehicle);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};
