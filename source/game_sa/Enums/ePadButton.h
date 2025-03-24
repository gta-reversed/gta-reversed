#pragma once

enum ePadButton : uint32 {
    LEFTSTICKX,     // Left / Right	GO_LEFT / GO_RIGHT
    LEFTSTICKY,     // Forward / Backward	GO_FORWARD / GO_BACK
    RIGHTSTICKX,    // Special CTRL Left / Special CTRL Right	PED_1RST_PERSON_LOOK_LEFT / PED_1RST_PERSON_LOOK_RIGHT
    RIGHTSTICKY,    // Special CTRL Up / Special CTRL Down	PED_1RST_PERSON_LOOK_UP / PED_1RST_PERSON_LOOK_DOWN
    LEFTSHOULDER1,  // Action, Secondary Fire	PED_ANSWER_PHONE, PED_FIREWEAPON_ALT
    LEFTSHOULDER2,  // Previous Weapon / Zoom In	PED_CYCLE_WEAPON_LEFT / PED_SNIPER_ZOOM_IN
    RIGHTSHOULDER1, // Aim Weapon	PED_LOCK_TARGET
    RIGHTSHOULDER2, // Next Weapon / Zoom Out	PED_CYCLE_WEAPON_RIGHT / PED_SNIPER_ZOOM_OUT
    DPADUP,         // Group Control Forward	GROUP_CONTROL_FWD
    DPADDOWN,       // Group Control Backward	GROUP_CONTROL_BWD
    DPADLEFT,       // Conversation - No	CONVERSATION_NO
    DPADRIGHT,      // Conversation - Yes	CONVERSATION_YES
    START,          // Pause Menu
    SELECT,         // Change Camera	CAMERA_CHANGE_VIEW_ALL_SITUATIONS
    SQUARE,         // Jump	PED_JUMPING
    TRIANGLE,       // Enter Vehicle	VEHICLE_ENTER_EXIT
    CROSS,          // Sprint	PED_SPRINT
    CIRCLE,         // Fire Weapon	PED_FIREWEAPON
    LEFTSHOCK,      // Crouch	PED_DUCK
    RIGHTSHOCK,     // Look Behind	PED_LOOKBEHIND
    // --		Sneak	SNEAK_ABOUT
    // --		Cycle Target Left	PED_CYCLE_TARGET_LEFT
    // --		Cycle Target Right	PED_CYCLE_TARGET_RIGHT
    // --		Center Camera	PED_CENTER_CAMERA_BEHIND_PLAYER
};
