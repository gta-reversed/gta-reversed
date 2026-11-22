#pragma once

enum class eIKChainSlot : int32 {
    UNKNOWN   = -1,
    LOOK_AT   = 0, //!< `CTaskSimpleIKLookAt`
    RIGHT_ARM = 1, //!< `CTaskSimpleIKPointArm` - Right arm
    LEFT_ARM  = 2, //!< `CTaskSimpleIKPointArm` - Left arm
    CUSTOM    = 3, //!< `CTaskSimpleIKChain` - A custom IK chain (?)

    // Add new above this
    COUNT,
};
