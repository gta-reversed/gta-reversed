/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
class CAnimBlendAssociation;

class NOTSA_EXPORT_VTABLE CTaskSimpleAnim : public CTaskSimple {
public:
    CAnimBlendAssociation *m_pAnim;
    union {
        struct {
            uint8 m_bIsFinished : 1;
            uint8 m_bDontInterrupt : 1;
            uint8 m_bHoldLastFrame : 1;

            // These flags are used in CTaskSimpleRunAnim only
            uint8 m_bDontBlendOut : 1;

            // These flags are used in 	CTaskSimpleRunNamedAnim only
            uint8 m_bRunInSequence : 1;
            uint8 m_bOffsetAtEnd : 1;
            uint8 m_bOffsetAvailable : 1;
        };
        uint8 m_nFlags;
    };

public:
    explicit CTaskSimpleAnim(bool bHoldLastFrame);
    ~CTaskSimpleAnim() override;

    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    static void FinishRunAnimCB(CAnimBlendAssociation* blendAssoc, void* data); //data is CTaskSimpleAnim

    static void InjectHooks();
    bool MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event);
};
VALIDATE_SIZE(CTaskSimpleAnim, 0x10);
