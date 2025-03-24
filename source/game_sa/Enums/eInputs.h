#pragma once

enum eControllerAction : int32 {
  CA_INVALID = -1,                              // -1
  PED_FIREWEAPON = 0,                          // 0x0
  PED_FIREWEAPON_ALT = 1,                      // 0x1
  PED_CYCLE_WEAPON_RIGHT = 2,                  // 0x2
  PED_CYCLE_WEAPON_LEFT = 3,                   // 0x3
  GO_FORWARD = 4,                              // 0x4
  GO_BACK = 5,                                 // 0x5
  GO_LEFT = 6,                                 // 0x6
  GO_RIGHT = 7,                                // 0x7
  PED_SNIPER_ZOOM_IN = 8,                      // 0x8
  PED_SNIPER_ZOOM_OUT = 9,                     // 0x9
  VEHICLE_ENTER_EXIT = 10,                     // 0xA
  CAMERA_CHANGE_VIEW_ALL_SITUATIONS = 11,      // 0xB
  PED_JUMPING = 12,                            // 0xC
  PED_SPRINT = 13,                             // 0xD
  PED_LOOKBEHIND = 14,                         // 0xE
  PED_DUCK = 15,                               // 0xF
  PED_ANSWER_PHONE = 16,                       // 0x10
  PED_WALK = 17,                               // 0x11
  VEHICLE_FIREWEAPON = 18,                     // 0x12
  VEHICLE_FIREWEAPON_ALT = 19,                 // 0x13
  VEHICLE_STEER_LEFT = 20,                     // 0x14
  VEHICLE_STEER_RIGHT = 21,                    // 0x15
  VEHICLE_STEER_UP = 22,                       // 0x16
  VEHICLE_STEER_DOWN = 23,                     // 0x17
  VEHICLE_ACCELERATE = 24,                     // 0x18
  VEHICLE_BRAKE = 25,                          // 0x19
  VEHICLE_RADIO_STATION_UP = 26,               // 0x1A
  VEHICLE_RADIO_STATION_DOWN = 27,             // 0x1B
  VEHICLE_RADIO_TRACK_SKIP = 28,               // 0x1C
  VEHICLE_HORN = 29,                           // 0x1D
  TOGGLE_SUBMISSIONS = 30,                     // 0x1E
  VEHICLE_HANDBRAKE = 31,                      // 0x1F
  PED_1RST_PERSON_LOOK_LEFT = 32,              // 0x20
  PED_1RST_PERSON_LOOK_RIGHT = 33,             // 0x21
  VEHICLE_LOOKLEFT = 34,                       // 0x22
  VEHICLE_LOOKRIGHT = 35,                      // 0x23
  VEHICLE_LOOKBEHIND = 36,                     // 0x24
  VEHICLE_MOUSELOOK = 37,                      // 0x25
  VEHICLE_TURRETLEFT = 38,                     // 0x26
  VEHICLE_TURRETRIGHT = 39,                    // 0x27
  VEHICLE_TURRETUP = 40,                       // 0x28
  VEHICLE_TURRETDOWN = 41,                     // 0x29
  PED_CYCLE_TARGET_LEFT = 42,                  // 0x2A
  PED_CYCLE_TARGET_RIGHT = 43,                 // 0x2B
  PED_CENTER_CAMERA_BEHIND_PLAYER = 44,        // 0x2C
  PED_LOCK_TARGET = 45,                        // 0x2D
  NETWORK_TALK = 46,                           // 0x2E
  CONVERSATION_YES = 47,                       // 0x2F
  CONVERSATION_NO = 48,                        // 0x30
  GROUP_CONTROL_FWD = 49,                      // 0x31
  GROUP_CONTROL_BWD = 50,                      // 0x32
  PED_1RST_PERSON_LOOK_UP = 51,                // 0x33
  PED_1RST_PERSON_LOOK_DOWN = 52,              // 0x34
  NUM_OF_1STPERSON_ACTIONS = 53,               // 0x35
  TOGGLE_DPAD = 54,                            // 0x36
  SWITCH_DEBUG_CAM_ON = 55,                    // 0x37
  TAKE_SCREEN_SHOT = 56,                       // 0x38
  SHOW_MOUSE_POINTER_TOGGLE = 57,              // 0x39
  SWITCH_CAM_DEBUG_MENU = 58,                  // 0x3A
  NUM_OF_CONTROLLER_ACTIONS                    // 59
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
