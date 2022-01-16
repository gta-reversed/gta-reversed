/*
    Plugin-SDK (Grand Theft Auto) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplex.h"
#include "TaskSimpleFacial.h"

class CTaskComplexFacial : public CTaskComplex {
public:
    bool              m_bNotPlayer;
    bool              m_bStarted;
    bool              m_bIsAborting;
    eFacialExpression m_nFacialExpression1;
    int32             m_nDuration1;
    eFacialExpression m_nFacialExpression2;
    int32             m_nDuration2;

    CTaskComplexFacial();
};

VALIDATE_SIZE(CTaskComplexFacial, 0x20);

