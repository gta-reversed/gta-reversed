/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include <extensions/WEnum.hpp>

class CAnimBlendSequence;
class CAnimBlendHierarchy;

//! Stores static animation data loaded from an animation clump.
class NOTSA_EXPORT_VTABLE CAnimBlendStaticAssociation {
public:
    static void InjectHooks();

    CAnimBlendStaticAssociation() = default;
    CAnimBlendStaticAssociation(RpClump* clump, CAnimBlendHierarchy* hier);
    virtual ~CAnimBlendStaticAssociation();

    void Init(RpClump* clump, CAnimBlendHierarchy* hier);
    void AllocateSequenceArray(int32 count);
    void FreeSequenceArray();

    auto GetHashKey() const noexcept { return m_BlendHier->m_hashKey; }

    bool IsValid() const { return !!m_BlendSeqs; } // vanilla sa, inlined function
    auto GetAnimHierarchy() const { return m_BlendHier; }

public:
    uint16                        m_NumBlendNodes{};
    notsa::WEnumS16<AnimationId>  m_AnimId{ANIM_ID_UNDEFINED};
    notsa::WEnumS16<AssocGroupId> m_AnimGroupId{ANIM_GROUP_NONE};
    uint16                        m_Flags{};
    CAnimBlendSequence**          m_BlendSeqs{};
    CAnimBlendHierarchy*          m_BlendHier{};

private:
    // 0x4CE940
    auto Constructor1() {
        this->CAnimBlendStaticAssociation::CAnimBlendStaticAssociation();
        return this;
    }

    // 0x4CEF60
    auto Constructor2(RpClump* clump, CAnimBlendHierarchy* h) {
        this->CAnimBlendStaticAssociation::CAnimBlendStaticAssociation(clump, h);
        return this;
    }

    // 0x4CDF50
    auto Destructor() {
        this->~CAnimBlendStaticAssociation();
        return this;
    }
};
VALIDATE_SIZE(CAnimBlendStaticAssociation, 0x14);
