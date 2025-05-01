#pragma once

#include "eIKChainSlot.h"
#include "eBoneTag.h"

enum eIKArm {
    IK_ARM_RIGHT,
    IK_ARM_LEFT,
};

inline auto IKArmToIKSlot(eIKArm arm) {
    switch (arm) {
    case eIKArm::IK_ARM_RIGHT: return eIKChainSlot::RIGHT_ARM;
    case eIKArm::IK_ARM_LEFT: return eIKChainSlot::LEFT_ARM;
    default: NOTSA_UNREACHABLE("Unknown IKArm: {}", (int)arm);
    }
}

inline auto IKArmToBoneTag(eIKArm arm) {
    switch (arm) {
    case eIKArm::IK_ARM_RIGHT: return BONE_R_HAND;
    case eIKArm::IK_ARM_LEFT: return BONE_L_HAND;
    default: NOTSA_UNREACHABLE("Unknown IKArm: {}", (int)arm);
    }
}
