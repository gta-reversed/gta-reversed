#pragma once

#include "Base.h"

#include "TaskTimer.h"

class CMentalState {
public:
    static void InjectHooks();

    CMentalState() = default;
    ~CMentalState() = default;

    void IncrementAngerAtPlayer(uint8 anger);
    void Process(const CPed& ped);

private:
    uint8      m_AngerAtPlayer;
    uint8      m_LastAngerAtPlayer;

    CTaskTimer m_AngerTimer;

    uint8      m_PedHealth;
    uint8      m_PrevPedHealth;

    uint8      m_VehHealth;
    uint8      m_PrevVehHealth;
};
VALIDATE_SIZE(CMentalState, 0x14);
