/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"

class CAnimBlendNode;
class CAnimBlendHierarchy;

class AnimAssociationData {
public:
    RwLLLink             m_link;
    uint16               m_nNumBlendNodes;
    uint16               m_nAnimGroup;
    CAnimBlendNode*      m_pNodeArray;
    CAnimBlendHierarchy* m_pHierarchy;
    float                m_fBlendAmount;
    float                m_fBlendDelta;
    float                m_fCurrentTime;
    float                m_Speed;
    float                fTimeStep;
    int16                m_nAnimId;
    uint16               m_nFlags;
};

VALIDATE_SIZE(AnimAssociationData, 0x2C);
