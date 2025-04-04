/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

enum eCamMode : uint16 {
    MODE_NONE = 0,                              // 0x00
    MODE_TOPDOWN = 1,                           // 0x01
    MODE_GTACLASSIC = 2,                        // 0x02
    MODE_BEHINDCAR = 3,                         // 0x03
    MODE_FOLLOWPED = 4,                         // 0x04
    MODE_AIMING = 5,                            // 0x05
    MODE_DEBUG = 6,                             // 0x06
    MODE_SNIPER = 7,                            // 0x07
    MODE_ROCKETLAUNCHER = 8,                    // 0x08
    MODE_MODELVIEW = 9,                         // 0x09
    MODE_BILL = 10,                             // 0x0A
    MODE_SYPHON = 11,                           // 0x0B
    MODE_CIRCLE = 12,                           // 0x0C
    MODE_CHEESYZOOM = 13,                       // 0x0D
    MODE_WHEELCAM = 14,                         // 0x0E
    MODE_FIXED = 15,                            // 0x0F
    MODE_1STPERSON = 16,                        // 0x10
    MODE_FLYBY = 17,                            // 0x11
    MODE_CAM_ON_A_STRING = 18,                  // 0x12
    MODE_REACTION = 19,                         // 0x13
    MODE_FOLLOW_PED_WITH_BIND = 20,             // 0x14
    MODE_CHRIS = 21,                            // 0x15
    MODE_BEHINDBOAT = 22,                       // 0x16
    MODE_PLAYER_FALLEN_WATER = 23,              // 0x17
    MODE_CAM_ON_TRAIN_ROOF = 24,                // 0x18
    MODE_CAM_RUNNING_SIDE_TRAIN = 25,           // 0x19
    MODE_BLOOD_ON_THE_TRACKS = 26,              // 0x1A
    MODE_IM_THE_PASSENGER_WOOWOO = 27,          // 0x1B
    MODE_SYPHON_CRIM_IN_FRONT = 28,             // 0x1C
    MODE_PED_DEAD_BABY = 29,                    // 0x1D
    MODE_PILLOWS_PAPS = 30,                     // 0x1E
    MODE_LOOK_AT_CARS = 31,                     // 0x1F
    MODE_ARRESTCAM_ONE = 32,                    // 0x20
    MODE_ARRESTCAM_TWO = 33,                    // 0x21
    MODE_M16_1STPERSON = 34,                    // 0x22
    MODE_SPECIAL_FIXED_FOR_SYPHON = 35,         // 0x23
    MODE_FIGHT_CAM = 36,                        // 0x24
    MODE_TOP_DOWN_PED = 37,                     // 0x25
    MODE_LIGHTHOUSE = 38,                       // 0x26
    MODE_SNIPER_RUNABOUT = 39,                  // 0x27
    MODE_ROCKETLAUNCHER_RUNABOUT = 40,          // 0x28
    MODE_1STPERSON_RUNABOUT = 41,               // 0x29
    MODE_M16_1STPERSON_RUNABOUT = 42,           // 0x2A
    MODE_FIGHT_CAM_RUNABOUT = 43,               // 0x2B
    MODE_EDITOR = 44,                           // 0x2C
    MODE_HELICANNON_1STPERSON = 45,             // 0x2D
    MODE_CAMERA = 46,                           // 0x2E
    MODE_ATTACHCAM = 47,                        // 0x2F
    MODE_TWOPLAYER = 48,                        // 0x30
    MODE_TWOPLAYER_IN_CAR_AND_SHOOTING = 49,    // 0x31
    MODE_TWOPLAYER_SEPARATE_CARS = 50,          // 0x32
    MODE_ROCKETLAUNCHER_HS = 51,                // 0x33
    MODE_ROCKETLAUNCHER_RUNABOUT_HS = 52,       // 0x34
    MODE_AIMWEAPON = 53,                        // 0x35
    MODE_TWOPLAYER_SEPARATE_CARS_TOPDOWN = 54,  // 0x36
    MODE_AIMWEAPON_FROMCAR = 55,                // 0x37
    MODE_DW_HELI_CHASE = 56,                    // 0x38
    MODE_DW_CAM_MAN = 57,                       // 0x39
    MODE_DW_BIRDY = 58,                         // 0x3A
    MODE_DW_PLANE_SPOTTER = 59,                 // 0x3B
    MODE_DW_DOG_FIGHT = 60,                     // 0x3C
    MODE_DW_FISH = 61,                          // 0x3D
    MODE_DW_PLANECAM1 = 62,                     // 0x3E
    MODE_DW_PLANECAM2 = 63,                     // 0x3F
    MODE_DW_PLANECAM3 = 64,                     // 0x40
    MODE_AIMWEAPON_ATTACHED = 65                // 0x41
};
