// Extracted from AnimBlendNode.h
/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Base.h"

class CAnimBlendAssociation;
class CAnimBlendSequence;

// NOTSA
class CAnimBlendNodeImpl {
public:
    // 0x4CFB70
    // Reference: Original CAnimBlendNode::Init()
    inline void Init() {
        *this = CAnimBlendNodeImpl{};
    }

public:
    float                  m_Theta{};           //!< Angle between current and previous key-frames
    float                  m_InvSinTheta{};     //!< 1 / sin(m_Theta), used in slerp calculation
    int16                  m_KFCurr{ -1 };      //!< Current key frame (A)
    int16                  m_KFPrev{ -1 };      //!< Previous key frame (B)
    float                  m_KFRemainingTime{}; //!< Time until frames have to advance (So, basically time left from current key-frame, so <frame progress> = m_KFRemainingTime / KeyFrameCurr->DeltaTime)
    CAnimBlendSequence*    m_Seq{};             //!< Node animation key-frames
    CAnimBlendAssociation* m_BlendAssoc{};      //!< Parent anim to which this node belongs to
};
VALIDATE_SIZE(CAnimBlendNodeImpl, 0x18);
