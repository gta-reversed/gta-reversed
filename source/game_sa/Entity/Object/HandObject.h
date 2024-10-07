/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Object.h"

class NOTSA_EXPORT_VTABLE CHandObject : public CObject {
public:
    class CPed* m_pPed;
    uint32      m_nBoneIndex;
    RwTexture*  m_pTexture;
    bool        m_bUpdatedMatricesArray;
    char        _pad[3];

public:
    static void InjectHooks();

    CHandObject(int32 handModelIndex, CPed* ped, bool bLeftHand);
    ~CHandObject() = default;

    void ProcessControl() override;
    void PreRender() override;
    void Render() override;

private:
};

VALIDATE_SIZE(CHandObject, 0x18C);
