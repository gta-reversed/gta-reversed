#pragma once

#include "Game.h"

// Type of menu entries
enum eMenuEntryType : int8 { // Originally it has no name.
    TI_STRING = 0,
    TI_SLOT1  = 1,
    TI_SLOT2,
    TI_SLOT3,
    TI_SLOT4,
    TI_SLOT5,
    TI_SLOT6,
    TI_SLOT7,
    TI_SLOT8,
    // TI_SLOTAUTO
    // TI_SLOTCP1
    // TI_SLOTCP2
    TI_MPACK = 9,
    TI_MOUSEJOYPAD,
    TI_ENTER,
    TI_OPTION,
};

enum eMenuScreen : int8 {
    SCREEN_GO_BACK                         = -2,
    SCREEN_NONE                            = -1,
    SCREEN_STATS                           =  0,
    SCREEN_NOP                             =  0,
    SCREEN_START_GAME,                     //  1  New Game, Load Game, Delete Game
    SCREEN_BRIEF,                          //  2
    SCREEN_AUDIO_SETTINGS,                 //  3
    SCREEN_DISPLAY_SETTINGS,               //  4
    SCREEN_MAP,                            //  5
    SCREEN_NEW_GAME_ASK,                   //  6  Are you sure you want to start a new game? All current game progress will be lost. Proceed?
    SCREEN_SELECT_GAME,                    //  7  Please select which new game you wish to start:
    SCREEN_MISSION_PACK_LOADING_ASK,       //  8  Are you sure you want to load a San Andreas Mission Pack? All current game progress will be lost. Proceed?
    SCREEN_LOAD_GAME,                      //  9  Select save file to load:
    SCREEN_DELETE_GAME,                    // 10  Select save file to delete:
    SCREEN_LOAD_GAME_ASK,                  // 11  All unsaved progress in your current game will be lost. Proceed with loading?
    SCREEN_DELETE_GAME_ASK,                // 12  Are you sure you wish to delete this save file?
    SCREEN_LOAD_FIRST_SAVE,                // 13
    SCREEN_DELETE_FINISHED,                // 14
    SCREEN_DELETE_SUCCESSFUL,              // 15  Delete Successful. Select OK to continue.
    SCREEN_GAME_SAVE,                      // 16  Select file you wish to save to:
    SCREEN_SAVE_WRITE_ASK,                 // 17  Are you sure you wish to save?
    SCREEN_SAVE_DONE_1,                    // 18
    SCREEN_SAVE_DONE_2,                    // 19  Save Successful. Select OK to continue.
    SCREEN_GAME_SAVED,                     // 20  OK
    SCREEN_GAME_LOADED,                    // 21  OK
    SCREEN_GAME_WARNING_DONT_SAVE,         // 22  Warning! One or more cheats have been activated. This may affect your save game. It is recommended that you do not save this game.
    SCREEN_ASK_DISPLAY_DEFAULT_SETS,       // 23  Are you sure you want to reset your current settings to default?
    SCREEN_ASK_AUDIO_DEFAULT_SETS,         // 24  Are you sure you want to reset your current settings to default?
    SCREEN_CONTROLS_RESET,                 // 25  Are you sure you want to reset your current settings to default?
    SCREEN_USER_TRACKS_OPTIONS,            // 26
    SCREEN_DISPLAY_ADVANCED,               // 27  DRAW DISTANCE, ...
    SCREEN_LANGUAGE,                       // 28  English, ...
    SCREEN_SAVE_GAME_DONE,                 // 29  O.K
    SCREEN_SAVE_GAME_FAILED,               // 30  Save Unsuccessful., O.K.
    SCREEN_SAVE_WRITE_FAILED,              // 31  Save Unsuccessful.
    SCREEN_SAVE_FAILED_FILE_ERROR,         // 32  Load Unsuccessful. File Corrupted, Please delete.
    SCREEN_OPTIONS,                        // 33  Controller Setup, Audio Setup ...
    SCREEN_MAIN_MENU,                      // 34  Start Game, Options, Quit Game
    SCREEN_QUIT_GAME_ASK,                  // 35  Are you sure you want to quit? All progress since the last save game will be lost. Proceed?
    SCREEN_CONTROLLER_SETUP,               // 36  CONFIGURATION, Redefine Controls ...
    SCREEN_REDEFINE_CONTROLS,              // 37  Foot Controls, Vehicle Controls
    SCREEN_CONTROLS_DEFINITION,            // 38  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    SCREEN_MOUSE_SETTINGS,                 // 39
    SCREEN_JOYPAD_SETTINGS,                // 40
    SCREEN_PAUSE_MENU,                     // 41  Resume, START NEW GAME ...
    SCREEN_INITIAL,                        // 42  SCREEN_QUIT_GAME_2
    SCREEN_EMPTY,                          // 43

    SCREEN_COUNT,                          // Screen count
};

// Menu entries action to perform
enum eMenuAction : int8 { // There's many actions @0x57702E and @0x57CD88
    MENU_ACTION_NA,
    MENU_ACTION_TEXT,                     //  1  Some static text at the top of the page (works only on first entry)
    MENU_ACTION_BACK,                     //  2  Back to previous menu

    MENU_ACTION_YES,                      //  3  Used as YES in menus (also as NO, weird?)
    MENU_ACTION_NO,                       //  4  Used as NO in menus  (also as YES, weird?)
    MENU_ACTION_MENU,                     //  5  Switch to target menu

    MENU_ACTION_USER_TRACKS_SCAN,         //  6
    MENU_ACTION_CTRLS_JOYPAD,             //  7  ??
    MENU_ACTION_CTRLS_FOOT,               //  8
    MENU_ACTION_CTRLS_CAR,                //  9
    MENU_ACTION_NEW_GAME,                 // 10  start a new game
    MENU_ACTION_MPACK,                    // 11
    MENU_ACTION_MPACKGAME,                // 12
    MENU_ACTION_SAVE_SLOT,                // 13
    MENU_ACTION_STANDARD_GAME,            // 14
    MENU_ACTION_15,                       // 15
    MENU_ACTION_16,                       // 16
    MENU_ACTION_17,                       // 17
    MENU_ACTION_SAVE_GAME,                // 18
    MENU_ACTION_19,                       // 19
    MENU_ACTION_SKIP,                     // 20  Skip this entry (unselectable)
    MENU_ACTION_STAT,                     // 21

    MENU_ACTION_INVERT_PAD,               // 22
    MENU_ACTION_23,                       // 23
    MENU_ACTION_FRAME_LIMITER,            // 24
    MENU_ACTION_SUBTITLES,                // 25
    MENU_ACTION_WIDESCREEN,               // 26
    MENU_ACTION_BRIGHTNESS,               // 27
    MENU_ACTION_RADIO_VOL,                // 28
    MENU_ACTION_SFX_VOL,                  // 29
    MENU_ACTION_RADIO_EQ,                 // 30 BASS EQ
    MENU_ACTION_RADIO_RETUNE,             // 31
    MENU_ACTION_RADIO_STATION,            // 32
    MENU_ACTION_SHOW_LEGEND,              // 33
    MENU_ACTION_RADAR_MODE,               // 34
    MENU_ACTION_HUD_MODE,                 // 35

    MENU_ACTION_LANGUAGE,                 // 36  Old way to switch language?
    MENU_ACTION_LANG_ENGLISH,              // 37
    MENU_ACTION_LANG_FRENCH,               // 38
    MENU_ACTION_LANG_GERMAN,               // 39
    MENU_ACTION_LANG_ITALIAN,              // 40
    MENU_ACTION_LANG_SPANISH,              // 41
# ifdef USE_LANG_RUSSIAN
    MENU_ACTION_LANG_RUSSIAN,              // 42
# endif
# ifdef USE_LANG_JAPANESE
    MENU_ACTION_LANG_JAPANESE,             // 43
# endif

    MENU_ACTION_FX_QUALITY,               // 42
    MENU_ACTION_MIP_MAPPING,              // 43
    MENU_ACTION_ANTIALIASING,             // 44
    MENU_ACTION_45,                       // 45

    MENU_ACTION_CONTROLS_MOUSE_INVERT_Y,  // 46
    MENU_ACTION_CONTROLS_JOY_INVERT_X,    // 47
    MENU_ACTION_CONTROLS_JOY_INVERT_Y,    // 48
    MENU_ACTION_CONTROLS_JOY_INVERT_X2,   // 49
    MENU_ACTION_CONTROLS_JOY_INVERT_Y2,   // 50
    MENU_ACTION_CONTROLS_JOY_SWAP_AXIS1,  // 51
    MENU_ACTION_CONTROLS_JOY_SWAP_AXIS2,  // 52

    MENU_ACTION_QUIT,                     // 53
    MENU_ACTION_MENU_CLOSE,               // 54  Deactivate menu
    MENU_ACTION_PAUSE,                    // 55  Same as BACK without an extra checking (?)

    MENU_ACTION_RESOLUTION,               // 56
    MENU_ACTION_RESET_CFG,                // 57  Set defaults settings depending on current menu page.

    MENU_ACTION_CONTROL_TYPE,             // 58
    MENU_ACTION_MOUSE_STEERING,           // 59
    MENU_ACTION_MOUSE_FLY,                // 60
    MENU_ACTION_DRAW_DIST,                // 61
    MENU_ACTION_MOUSE_SENS,               // 62

    MENU_ACTION_USER_TRACKS_PLAY_MODE,    // 63
    MENU_ACTION_USER_TRACKS_AUTO_SCAN,    // 64
    MENU_ACTION_STORE_PHOTOS,             // 65

# if defined(USE_GALLERY)
    MENU_ACTION_GALLERY, // (xbox #34)
# endif
};

enum eMenuAlign : int8 {
    MENU_ALIGN_DEFAULT = 0,
    MENU_ALIGN_LEFT    = 1,
    MENU_ALIGN_RIGHT   = 2,
    MENU_ALIGN_CENTER  = 3,
};

struct tMenuScreenItem {
    eMenuAction    m_nActionType;
    char           m_szName[8];
    eMenuEntryType m_nType;
    eMenuScreen    m_nTargetMenu; // Ignored for MENU_ACTION_BACK
    uint16         m_X;
    uint16         m_Y;
    eMenuAlign     m_nAlign;
};
VALIDATE_SIZE(tMenuScreenItem, 0x12);

struct tMenuScreen {
    char            m_szTitleName[8];
    eMenuScreen     m_nParentMenu;
    int8            m_nStartEntry;
    tMenuScreenItem m_aItems[12];
};
VALIDATE_SIZE(tMenuScreen, 0xE2);

enum {
    FRONTEND1_START       = 0,
    FRONTEND2_START       = 13,
    FRONTEND3_START       = 21,
    FRONTEND4_START       = 23, // PC
    FRONTEND_SPRITE_COUNT = 25,

    // FRONTEND 1 - Radio
    FRONTEND_SPRITE_ARROW        = FRONTEND1_START,
    FRONTEND_SPRITE_PLAYBACK,
    FRONTEND_SPRITE_KROSE,
    FRONTEND_SPRITE_KDST,
    FRONTEND_SPRITE_BOUNCE,
    FRONTEND_SPRITE_SFUR,
    FRONTEND_SPRITE_RLS,
    FRONTEND_SPRITE_RADIOX,
    FRONTEND_SPRITE_CSR,
    FRONTEND_SPRITE_KJAH,
    FRONTEND_SPRITE_MASTER_SOUNDS,
    FRONTEND_SPRITE_WCTR,
    FRONTEND_SPRITE_TPLAYER,

    // FRONTEND 2 - Background
    FRONTEND_SPRITE_BACK2        = FRONTEND2_START,
    FRONTEND_SPRITE_BACK3,
    FRONTEND_SPRITE_BACK4,
    FRONTEND_SPRITE_BACK5,
    FRONTEND_SPRITE_BACK6,
    FRONTEND_SPRITE_BACK7,
    FRONTEND_SPRITE_BACK8,
    FRONTEND_SPRITE_MAP,

    // FRONTEND 3 - AdditionalBackground
    FRONTEND_SPRITE_BACK8_TOP    = FRONTEND3_START,
    FRONTEND_SPRITE_BACK8_RIGHT,

    // FRONTEND PC - Mouse
    FRONTEND_SPRITE_MOUSE        = FRONTEND4_START,
    FRONTEND_SPRITE_CROSS_HAIR,
};

extern SpriteFileName FrontEndFilenames[];
/*
static inline tMenuScreen (&aScreens)[43] = *(tMenuScreen(*)[43])0x8CE008;
*/ 
extern tMenuScreen aScreens[];
