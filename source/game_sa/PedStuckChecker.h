#pragma once

#include "Vector.h"

class CPed;
class CEventGroup;

enum ePedStuckState : uint16 {
    PED_STUCK_STATE_NONE = 0,
    PED_STUCK_STATE_STUCK = 1,
    PED_STUCK_STATE_WAS_STUCK = 2
};

class CPedStuckChecker {
public:
    CVector        m_lastNonStuckPoint;
    uint16         m_radius;
    ePedStuckState m_state;

public:
    CPedStuckChecker() { // inlined 0x607292
        m_radius = 0;
        m_state = PED_STUCK_STATE_NONE;
    }

    bool TestPedStuck(CPed* ped, CEventGroup* eventGroup);

    auto GetState() const { return m_state; }
};

VALIDATE_SIZE(CPedStuckChecker, 0x10);
