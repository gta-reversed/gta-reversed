/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskTimer.h"

class C2dEffect;

// TODO: Refactoring
class CAttractorScanner {
public:
    bool m_Activated;

    CTaskTimer m_Timer;

    C2dEffect* m_PreviousEffect;
    CEntity*   m_PreviousEntity;

    CEntity*   m_Entities[10];
    C2dEffect* m_Effects[10];
    float      m_MinDistanceSq[10];

    void Clear();
};

VALIDATE_SIZE(CAttractorScanner, 0x90);
