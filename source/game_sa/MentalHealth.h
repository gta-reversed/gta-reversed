#pragma once

#include "Base.h"

#include "TaskTimer.h"

class CMentalState {
public:
    uint8      m_anger;
    uint8      m_oldAnger;

    CTaskTimer m_timer;

    uint8      m_pedHealth;
    uint8      m_oldPedHealth;

    uint8      m_vehicleHealth;
    uint8      m_oldVehicleHealth;

public:
    CMentalState() = default;
    ~CMentalState() = default;

    void IncrementAnger(int32 anger);
    void Process();
};
VALIDATE_SIZE(CMentalState, 0x14);
