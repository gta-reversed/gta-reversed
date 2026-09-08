#include "StdInc.h"

#include "Remote.h"

void CRemote::InjectHooks() {
    RH_ScopedClass(CRemote);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(TakeRemoteControlledCarFromPlayer, 0x45AE80, { .reversed = false });
    RH_ScopedInstall(GivePlayerRemoteControlledCar, 0x45AB10, { .reversed = false });
}

void CRemote::TakeRemoteControlledCarFromPlayer(bool bCreateRemoteVehicleExplosion)
{
    return plugin::Call<0x45AE80, bool>(bCreateRemoteVehicleExplosion);
}

void CRemote::GivePlayerRemoteControlledCar(CVector pos, float rotation, int16 modelId)
{
    return plugin::Call<0x45AB10, CVector, float, int16>(pos, rotation, modelId);
}
