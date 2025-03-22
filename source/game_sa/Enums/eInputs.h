#pragma once

enum eControllerAction {
  CA_PED_FIREWEAPON = 0,                          // 0x0
  CA_PED_FIREWEAPON_ALT = 1,                      // 0x1
  CA_PED_CYCLE_WEAPON_RIGHT = 2,                  // 0x2
  CA_PED_CYCLE_WEAPON_LEFT = 3,                   // 0x3
  CA_GO_FORWARD = 4,                              // 0x4
  CA_GO_BACK = 5,                                 // 0x5
  CA_GO_LEFT = 6,                                 // 0x6
  CA_GO_RIGHT = 7,                                // 0x7
  CA_PED_SNIPER_ZOOM_IN = 8,                      // 0x8
  CA_PED_SNIPER_ZOOM_OUT = 9,                     // 0x9
  CA_VEHICLE_ENTER_EXIT = 10,                     // 0xA
  CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS = 11,      // 0xB
  CA_PED_JUMPING = 12,                            // 0xC
  CA_PED_SPRINT = 13,                             // 0xD
  CA_PED_LOOKBEHIND = 14,                         // 0xE
  CA_PED_DUCK = 15,                               // 0xF
  CA_PED_ANSWER_PHONE = 16,                       // 0x10
  CA_PED_WALK = 17,                               // 0x11
  CA_VEHICLE_FIREWEAPON = 18,                     // 0x12
  CA_VEHICLE_FIREWEAPON_ALT = 19,                 // 0x13
  CA_VEHICLE_STEER_LEFT = 20,                     // 0x14
  CA_VEHICLE_STEER_RIGHT = 21,                    // 0x15
  CA_VEHICLE_STEER_UP = 22,                       // 0x16
  CA_VEHICLE_STEER_DOWN = 23,                     // 0x17
  CA_VEHICLE_ACCELERATE = 24,                     // 0x18
  CA_VEHICLE_BRAKE = 25,                          // 0x19
  CA_VEHICLE_RADIO_STATION_UP = 26,               // 0x1A
  CA_VEHICLE_RADIO_STATION_DOWN = 27,             // 0x1B
  CA_VEHICLE_RADIO_TRACK_SKIP = 28,               // 0x1C
  CA_VEHICLE_HORN = 29,                           // 0x1D
  CA_TOGGLE_SUBMISSIONS = 30,                     // 0x1E
  CA_VEHICLE_HANDBRAKE = 31,                      // 0x1F
  CA_PED_1RST_PERSON_LOOK_LEFT = 32,              // 0x20
  CA_PED_1RST_PERSON_LOOK_RIGHT = 33,             // 0x21
  CA_VEHICLE_LOOKLEFT = 34,                       // 0x22
  CA_VEHICLE_LOOKRIGHT = 35,                      // 0x23
  CA_VEHICLE_LOOKBEHIND = 36,                     // 0x24
  CA_VEHICLE_MOUSELOOK = 37,                      // 0x25
  CA_VEHICLE_TURRETLEFT = 38,                     // 0x26
  CA_VEHICLE_TURRETRIGHT = 39,                    // 0x27
  CA_VEHICLE_TURRETUP = 40,                       // 0x28
  CA_VEHICLE_TURRETDOWN = 41,                     // 0x29
  CA_PED_CYCLE_TARGET_LEFT = 42,                  // 0x2A
  CA_PED_CYCLE_TARGET_RIGHT = 43,                 // 0x2B
  CA_PED_CENTER_CAMERA_BEHIND_PLAYER = 44,        // 0x2C
  CA_PED_LOCK_TARGET = 45,                        // 0x2D
  CA_NETWORK_TALK = 46,                           // 0x2E
  CA_CONVERSATION_YES = 47,                       // 0x2F
  CA_CONVERSATION_NO = 48,                        // 0x30
  CA_GROUP_CONTROL_FWD = 49,                      // 0x31
  CA_GROUP_CONTROL_BWD = 50,                      // 0x32
  CA_PED_1RST_PERSON_LOOK_UP = 51,                // 0x33
  CA_PED_1RST_PERSON_LOOK_DOWN = 52,              // 0x34
  CA_NUM_OF_1STPERSON_ACTIONS = 53,               // 0x35
  CA_TOGGLE_DPAD = 54,                            // 0x36
  CA_SWITCH_DEBUG_CAM_ON = 55,                    // 0x37
  CA_TAKE_SCREEN_SHOT = 56,                       // 0x38
  CA_SHOW_MOUSE_POINTER_TOGGLE = 57,              // 0x39
  CA_SWITCH_CAM_DEBUG_MENU = 58,                  // 0x3A
  CA_NUM_OF_CONTROLLER_ACTIONS                    // 59
};

enum eMouseButtons : int32_t {
    NO_BUTTON          = 0x0,
    LEFT_MS_BUTTON     = 0x1,
    MIDDLE_MS_BUTTON   = 0x2,
    RIGHT_MS_BUTTON    = 0x3,
    WHEEL_MS_UP        = 0x4,
    WHEEL_MS_DOWN      = 0x5,
    FIRST_MS_X_BUTTON  = 0x6,
    SECOND_MS_X_BUTTON = 0x7,
};

enum eJOY_BUTTONS : int32_t {
    NO_JOYBUTTONS       = 0x0,
    JOYBUTTON_ONE       = 0x1,
    JOYBUTTON_TWO       = 0x2,
    JOYBUTTON_THREE     = 0x3,
    JOYBUTTON_FOUR      = 0x4,
    JOYBUTTON_FIVE      = 0x5,
    JOYBUTTON_SIX       = 0x6,
    JOYBUTTON_SEVEN     = 0x7,
    JOYBUTTON_EIGHT     = 0x8,
    JOYBUTTON_NINE      = 0x9,
    JOYBUTTON_TEN       = 0xA,
    JOYBUTTON_ELEVEN    = 0xB,
    JOYBUTTON_TWELVE    = 0xC,
    JOYBUTTON_THIRTEEN  = 0xD,
    JOYBUTTON_FOURTEEN  = 0xE,
    JOYBUTTON_FIFTHTEEN = 0xF,
    JOYBUTTON_SIXTEEN   = 0x10,
};
