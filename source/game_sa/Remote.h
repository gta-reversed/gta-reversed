#pragma once

class CRemote {
public:
    static void InjectHooks();

public:
    static void TakeRemoteControlledCarFromPlayer(bool bCreateRemoteVehicleExplosion);
    static void GivePlayerRemoteControlledCar(CVector pos, float rotation, int16 modelId);
};
