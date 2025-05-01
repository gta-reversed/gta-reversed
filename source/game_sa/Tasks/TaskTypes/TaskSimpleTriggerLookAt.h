/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "AnimBlendAssociation.h"
#include "Entity.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleTriggerLookAt : public CTaskSimple {
public:
    CEntity* m_pEntity;
    int32    m_time;
    int32    m_nOffsetBoneTag;
    RwV3d    m_vOffsetPos;
    bool     m_bUseTorso;
    float    m_fSpeed;
    int32    m_nBlendTime;
    bool     m_bEntityExist;
    int8     m_nPriority;

public:
    static constexpr auto Type = TASK_SIMPLE_TRIGGER_LOOK_AT;

    CTaskSimpleTriggerLookAt(CEntity* entity,
                             int32 time,
                             int32 offsetBoneTag,
                             RwV3d offsetPos,
                             bool bUseTorso = true,
                             float speed = 0.25f,
                             int32 blendTime = 1000,
                             int32 priority = 3);
};

VALIDATE_SIZE(CTaskSimpleTriggerLookAt, 0x30);
