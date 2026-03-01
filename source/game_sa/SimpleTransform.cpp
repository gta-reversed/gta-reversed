/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "SimpleTransform.h"

// 0x54EF40
void CSimpleTransform::UpdateRwMatrix(RwMatrix* out)
{
    const float sinHeading = std::sin(m_fHeading);
    const float cosHeading = std::cos(m_fHeading);

    out->right = { cosHeading, sinHeading, 0.0f };
    out->up = { -sinHeading, cosHeading, 0.0f };
    out->at = { 0.0f, 0.0f, 1.0f };
    out->pos = m_vPosn;

    RwMatrixUpdate(out);

}

// 0x54EF90
void CSimpleTransform::Invert(const CSimpleTransform& base)
{
    const float cosHeading = cosf(base.m_fHeading);
    const float sinHeading = sinf(base.m_fHeading);

    m_vPosn.x = -(cosHeading * base.m_vPosn.x) - (sinHeading * base.m_vPosn.y);
    m_vPosn.y = (sinHeading * base.m_vPosn.x) - (cosHeading * base.m_vPosn.y);
    m_vPosn.z = -base.m_vPosn.z;
    m_fHeading = -base.m_fHeading;
}

// 0x54F1B0
void CSimpleTransform::UpdateMatrix(CMatrix* out)
{
    out->SetTranslate(m_vPosn);
    out->SetRotateZOnly(m_fHeading);
}
