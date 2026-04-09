#pragma once

#include "Game.h"

// Type of menu entries
enum eMenuEntryType : int8 { // Originally it has no name.
    TI_STRING       = 0,
    TI_SLOT1        = 1,
    FIRST_SAVE_SLOT = TI_SLOT1,
    TI_SLOT2,
    TI_SLOT3,
    TI_SLOT4,
    TI_SLOT5,
    TI_SLOT6,
    TI_SLOT7,
    TI_SLOT8,
    MAX_SAVE_SLOT = TI_SLOT8,

    /*
    TI_SLOTAUTO
    TI_SLOTCP1
    TI_SLOTCP2
    */

    TI_MPACK = 9,
    TI_MOUSEJOYPAD = 10,
    TI_ENTER = 11,
    TI_OPTION = 12,
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

    MENU_ACTION_LANGUAGE,                  // 36  Old way to switch language?
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
    MENU_ACTION_COUNT
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
    tMenuScreenItem m_aItems[12]; // eMenuEntryType::TI_OPTION ?
};
VALIDATE_SIZE(tMenuScreen, 0xE2);

enum eFrontend : int8 {
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

// NOTSA
inline bool IsSaveSlot(eMenuEntryType slot) {
    return slot >= eMenuEntryType::FIRST_SAVE_SLOT && slot <= eMenuEntryType::MAX_SAVE_SLOT;
}

/// Screens data (From 0x8CE008)
constexpr tMenuScreen aScreens[SCREEN_COUNT] = {
    // [0]: SCREEN_STATS
    {
        "FEP_STA",
        SCREEN_INITIAL,
        3,
        {
            { MENU_ACTION_STAT, "FES_PLA", TI_OPTION, SCREEN_STATS, 57, 120, MENU_ALIGN_LEFT },
            { MENU_ACTION_STAT, "FES_MON", TI_OPTION, SCREEN_STATS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_STAT, "FES_WEA", TI_OPTION, SCREEN_STATS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_STAT, "FES_CRI", TI_OPTION, SCREEN_STATS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_STAT, "FES_GAN", TI_OPTION, SCREEN_STATS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_STAT, "FES_ACH", TI_OPTION, SCREEN_STATS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_STAT, "FES_MIS", TI_OPTION, SCREEN_STATS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_STAT, "FES_MSC", TI_OPTION, SCREEN_STATS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_INITIAL, 320, 380, MENU_ALIGN_CENTER },
        },
    },
    // [1]: SCREEN_START_GAME
    {
        "FEH_LOA",
        SCREEN_INITIAL,
        1,
        {
            { MENU_ACTION_NEW_GAME, "FES_NGA", TI_ENTER, SCREEN_NEW_GAME_ASK, 80, 150, MENU_ALIGN_LEFT },
            { MENU_ACTION_MENU, "FES_LOA", TI_ENTER, SCREEN_LOAD_GAME, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_MENU, "FES_DEL", TI_ENTER, SCREEN_DELETE_GAME, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_STATS, 490, 380, MENU_ALIGN_LEFT },
        },
    },
    // [2]: SCREEN_BRIEF
    {
        "FEH_BRI",
        SCREEN_INITIAL,
        4,
        {
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_INITIAL, 57, 410, MENU_ALIGN_LEFT },
        },
    },
    // [3]: SCREEN_AUDIO_SETTINGS
    {
        "FEH_AUD",
        SCREEN_OPTIONS,
        1,
        {
            { MENU_ACTION_RADIO_VOL, "FEA_MUS", TI_OPTION, SCREEN_AUDIO_SETTINGS, 57, 100, MENU_ALIGN_LEFT },
            { MENU_ACTION_SFX_VOL, "FEA_SFX", TI_OPTION, SCREEN_AUDIO_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_RADIO_EQ, "FEA_BAS", TI_OPTION, SCREEN_AUDIO_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_RADIO_RETUNE, "FEA_ART", TI_OPTION, SCREEN_AUDIO_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_MENU, "FEA_TIT", TI_ENTER, SCREEN_USER_TRACKS_OPTIONS, 320, 225, MENU_ALIGN_CENTER },
            { MENU_ACTION_RADIO_STATION, "FEA_RSS", TI_OPTION, SCREEN_AUDIO_SETTINGS, 57, 260, MENU_ALIGN_LEFT },
            { MENU_ACTION_MENU, "FET_DEF", TI_ENTER, SCREEN_ASK_AUDIO_DEFAULT_SETS, 320, 367, MENU_ALIGN_CENTER },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_INITIAL, 0, 0, MENU_ALIGN_CENTER },
        },
    },
    // [4]: SCREEN_DISPLAY_SETTINGS
    {
        "FEH_DIS",
        SCREEN_OPTIONS,
        2,
        {
            { MENU_ACTION_BRIGHTNESS, "FED_BRI", TI_OPTION, SCREEN_DISPLAY_SETTINGS, 57, 127, MENU_ALIGN_LEFT },
            { MENU_ACTION_SHOW_LEGEND, "MAP_LEG", TI_OPTION, SCREEN_DISPLAY_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_RADAR_MODE, "FED_RDR", TI_OPTION, SCREEN_DISPLAY_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_HUD_MODE, "FED_HUD", TI_OPTION, SCREEN_DISPLAY_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_SUBTITLES, "FED_SUB", TI_OPTION, SCREEN_DISPLAY_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_STORE_PHOTOS, "FED_GLS", TI_OPTION, SCREEN_DISPLAY_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_MENU, "FED_ADV", TI_ENTER, SCREEN_DISPLAY_ADVANCED, 320, 320, MENU_ALIGN_CENTER },
            { MENU_ACTION_MENU, "FET_DEF", TI_ENTER, SCREEN_ASK_DISPLAY_DEFAULT_SETS, 320, 365, MENU_ALIGN_CENTER },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_OPTIONS, 0, 0, MENU_ALIGN_CENTER },
        },
    },
    // [5]: SCREEN_MAP
    {
        "FEH_MAP",
        SCREEN_INITIAL,
        2,
        {
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_INITIAL, 57, 410, MENU_ALIGN_LEFT },
        },
    },
    // [6]: SCREEN_NEW_GAME_ASK
    {
        "FES_NGA",
        SCREEN_START_GAME,
        0,
        {
            { MENU_ACTION_TEXT, "FESZ_QR", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_NO, "FEM_NO", TI_ENTER, SCREEN_START_GAME, 320, 215, MENU_ALIGN_CENTER },
            { MENU_ACTION_STANDARD_GAME, "FEM_YES", TI_ENTER, SCREEN_NEW_GAME_ASK, 320, 240, MENU_ALIGN_CENTER },
        },
    },
    // [7]: SCREEN_SELECT_GAME
    {
        "FES_NGA",
        SCREEN_START_GAME,
        0,
        {
            { MENU_ACTION_TEXT, "FEN_NGS", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_MENU, "FEN_NGX", TI_ENTER, SCREEN_NEW_GAME_ASK, 320, 150, MENU_ALIGN_CENTER },
            { MENU_ACTION_MPACK, "FEN_MPX", TI_MPACK, SCREEN_STATS, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MPACK, "FEN_MPX", TI_MPACK, SCREEN_STATS, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MPACK, "FEN_MPX", TI_MPACK, SCREEN_STATS, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MPACK, "FEN_MPX", TI_MPACK, SCREEN_STATS, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MPACK, "FEN_MPX", TI_MPACK, SCREEN_STATS, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MPACK, "FEN_MPX", TI_MPACK, SCREEN_STATS, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MPACK, "FEN_MPX", TI_MPACK, SCREEN_STATS, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MPACK, "FEN_MPX", TI_MPACK, SCREEN_STATS, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MPACK, "FEN_MPX", TI_MPACK, SCREEN_STATS, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MPACK, "FEN_MPX", TI_MPACK, SCREEN_STATS, 0, 0, MENU_ALIGN_CENTER },
        },
    },
    // [8]: SCREEN_MISSION_PACK_LOADING_ASK
    {
        "FES_LMI",
        SCREEN_START_GAME,
        0,
        {
            { MENU_ACTION_TEXT, "FESZ_QM", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_NO, "FEM_NO", TI_ENTER, SCREEN_START_GAME, 320, 215, MENU_ALIGN_CENTER },
            { MENU_ACTION_MPACKGAME, "FEM_YES", TI_ENTER, SCREEN_STATS, 320, 240, MENU_ALIGN_CENTER },
        },
    },
    // [9]: SCREEN_LOAD_GAME
    {
        "FET_LG",
        SCREEN_START_GAME,
        1,
        {
            { MENU_ACTION_TEXT, "FES_SEL", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL1", TI_SLOT1, SCREEN_STATS, 80, 150, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL2", TI_SLOT2, SCREEN_STATS, 80, 170, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL3", TI_SLOT3, SCREEN_STATS, 80, 190, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL4", TI_SLOT4, SCREEN_STATS, 80, 210, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL5", TI_SLOT5, SCREEN_STATS, 80, 230, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL6", TI_SLOT6, SCREEN_STATS, 80, 250, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL7", TI_SLOT7, SCREEN_STATS, 80, 270, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL8", TI_SLOT8, SCREEN_STATS, 80, 290, MENU_ALIGN_LEFT },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_STATS, 490, 380, MENU_ALIGN_CENTER },
        },
    },
    // [10]: SCREEN_DELETE_GAME
    {
        "FES_DEL",
        SCREEN_START_GAME,
        2,
        {
            { MENU_ACTION_TEXT, "FES_SED", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL1", TI_SLOT1, SCREEN_STATS, 80, 150, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL2", TI_SLOT2, SCREEN_STATS, 80, 170, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL3", TI_SLOT3, SCREEN_STATS, 80, 190, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL4", TI_SLOT4, SCREEN_STATS, 80, 210, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL5", TI_SLOT5, SCREEN_STATS, 80, 230, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL6", TI_SLOT6, SCREEN_STATS, 80, 250, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL7", TI_SLOT7, SCREEN_STATS, 80, 270, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_SLOT, "FEM_SL8", TI_SLOT8, SCREEN_STATS, 80, 290, MENU_ALIGN_LEFT },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_STATS, 490, 380, MENU_ALIGN_CENTER },
        },
    },
    // [11]: SCREEN_LOAD_GAME_ASK
    {
        "FET_LG",
        SCREEN_LOAD_GAME,
        0,
        {
            { MENU_ACTION_TEXT, "FESZ_QL", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_NO, "FEM_NO", TI_ENTER, SCREEN_LOAD_GAME, 320, 215, MENU_ALIGN_CENTER },
            { MENU_ACTION_YES, "FEM_YES", TI_ENTER, SCREEN_LOAD_FIRST_SAVE, 320, 240, MENU_ALIGN_CENTER },
        },
    },
    // [12]: SCREEN_DELETE_GAME_ASK
    {
        "FES_DEL",
        SCREEN_DELETE_GAME,
        0,
        {
            { MENU_ACTION_TEXT, "FESZ_QD", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_NO, "FEM_NO", TI_ENTER, SCREEN_DELETE_GAME, 320, 215, MENU_ALIGN_CENTER },
            { MENU_ACTION_YES, "FEM_YES", TI_ENTER, SCREEN_DELETE_FINISHED, 320, 240, MENU_ALIGN_CENTER },
        },
    },
    // [13]: SCREEN_LOAD_FIRST_SAVE
    {
        "FET_LG",
        SCREEN_LOAD_GAME,
        0,
        {
        },
    },
    // [14]: SCREEN_DELETE_FINISHED
    {
        "FES_DEL",
        SCREEN_DELETE_GAME,
        0,
        {
        },
    },
    // [15]: SCREEN_DELETE_SUCCESSFUL
    {
        "FES_DEL",
        SCREEN_START_GAME,
        0,
        {
            { MENU_ACTION_TEXT, "FES_DSC", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_MENU, "FEM_OK", TI_ENTER, SCREEN_START_GAME, 320, 240, MENU_ALIGN_CENTER },
        },
    },
    // [16]: SCREEN_GAME_SAVE
    {
        "FET_SG",
        SCREEN_NONE,
        0,
        {
            { MENU_ACTION_TEXT, "FES_SES", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_GAME, "FEM_SL1", TI_SLOT1, SCREEN_SAVE_WRITE_ASK, 80, 150, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_GAME, "FEM_SL2", TI_SLOT2, SCREEN_SAVE_WRITE_ASK, 80, 170, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_GAME, "FEM_SL3", TI_SLOT3, SCREEN_SAVE_WRITE_ASK, 80, 190, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_GAME, "FEM_SL4", TI_SLOT4, SCREEN_SAVE_WRITE_ASK, 80, 210, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_GAME, "FEM_SL5", TI_SLOT5, SCREEN_SAVE_WRITE_ASK, 80, 230, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_GAME, "FEM_SL6", TI_SLOT6, SCREEN_SAVE_WRITE_ASK, 80, 250, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_GAME, "FEM_SL7", TI_SLOT7, SCREEN_SAVE_WRITE_ASK, 80, 270, MENU_ALIGN_LEFT },
            { MENU_ACTION_SAVE_GAME, "FEM_SL8", TI_SLOT8, SCREEN_SAVE_WRITE_ASK, 80, 290, MENU_ALIGN_LEFT },
            { MENU_ACTION_15, "FESZ_CA", TI_ENTER, SCREEN_STATS, 470, 380, MENU_ALIGN_CENTER },
        },
    },
    // [17]: SCREEN_SAVE_WRITE_ASK
    {
        "FET_SG",
        SCREEN_GAME_SAVE,
        0,
        {
            { MENU_ACTION_TEXT, "FESZ_QZ", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_NO, "FEM_NO", TI_ENTER, SCREEN_GAME_SAVE, 320, 215, MENU_ALIGN_CENTER },
            { MENU_ACTION_YES, "FEM_YES", TI_ENTER, SCREEN_SAVE_DONE_1, 320, 240, MENU_ALIGN_CENTER },
        },
    },
    // [18]: SCREEN_SAVE_DONE_1
    {
        "FET_SG",
        SCREEN_GAME_SAVE,
        0,
        {
        },
    },
    // [19]: SCREEN_SAVE_DONE_2
    {
        "FET_SG",
        SCREEN_GAME_SAVE,
        0,
        {
            { MENU_ACTION_TEXT, "FES_SSC", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_MENU_CLOSE, "FEM_OK", TI_ENTER, SCREEN_STATS, 320, 240, MENU_ALIGN_CENTER },
        },
    },
    // [20]: SCREEN_GAME_SAVED
    {
        "FET_SG",
        SCREEN_INITIAL,
        0,
        {
            { MENU_ACTION_TEXT, "", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_MENU, "FEM_OK", TI_ENTER, SCREEN_STATS, 320, 270, MENU_ALIGN_CENTER },
        },
    },
    // [21]: SCREEN_GAME_LOADED
    {
        "FET_LG",
        SCREEN_INITIAL,
        0,
        {
            { MENU_ACTION_TEXT, "", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_MENU, "FEM_OK", TI_ENTER, SCREEN_STATS, 320, 270, MENU_ALIGN_CENTER },
        },
    },
    // [22]: SCREEN_GAME_WARNING_DONT_SAVE
    {
        "FET_SG",
        SCREEN_START_GAME,
        0,
        {
            { MENU_ACTION_TEXT, "FES_CHE", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_MENU, "FEM_OK", TI_ENTER, SCREEN_GAME_SAVE, 320, 240, MENU_ALIGN_CENTER },
        },
    },
    // [23]: SCREEN_ASK_DISPLAY_DEFAULT_SETS
    {
        "FEH_DIS",
        SCREEN_DISPLAY_SETTINGS,
        7,
        {
            { MENU_ACTION_TEXT, "FED_RDP", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_MENU, "FEM_NO", TI_ENTER, SCREEN_DISPLAY_SETTINGS, 320, 215, MENU_ALIGN_CENTER },
            { MENU_ACTION_RESET_CFG, "FEM_YES", TI_ENTER, SCREEN_DISPLAY_SETTINGS, 320, 240, MENU_ALIGN_CENTER },
        },
    },
    // [24]: SCREEN_ASK_AUDIO_DEFAULT_SETS
    {
        "FEH_AUD",
        SCREEN_AUDIO_SETTINGS,
        6,
        {
            { MENU_ACTION_TEXT, "FED_RDP", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_MENU, "FEM_NO", TI_ENTER, SCREEN_AUDIO_SETTINGS, 320, 215, MENU_ALIGN_CENTER },
            { MENU_ACTION_RESET_CFG, "FEM_YES", TI_ENTER, SCREEN_AUDIO_SETTINGS, 320, 240, MENU_ALIGN_CENTER },
        },
    },
    // [25]: SCREEN_CONTROLS_RESET
    {
        "FET_CTL",
        SCREEN_CONTROLLER_SETUP,
        3,
        {
            { MENU_ACTION_TEXT, "FED_RDP", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_MENU, "FEM_NO", TI_ENTER, SCREEN_CONTROLLER_SETUP, 320, 215, MENU_ALIGN_CENTER },
            { MENU_ACTION_RESET_CFG, "FEM_YES", TI_ENTER, SCREEN_CONTROLLER_SETUP, 320, 240, MENU_ALIGN_CENTER },
        },
    },
    // [26]: SCREEN_USER_TRACKS_OPTIONS
    {
        "FEA_TIT",
        SCREEN_AUDIO_SETTINGS,
        4,
        {
            { MENU_ACTION_TEXT, "FEA_SUB", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_USER_TRACKS_PLAY_MODE, "FEA_MPM", TI_OPTION, SCREEN_USER_TRACKS_OPTIONS, 57, 190, MENU_ALIGN_LEFT },
            { MENU_ACTION_USER_TRACKS_AUTO_SCAN, "FEA_AMS", TI_OPTION, SCREEN_USER_TRACKS_OPTIONS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_USER_TRACKS_SCAN, "FEA_SCN", TI_ENTER, SCREEN_USER_TRACKS_OPTIONS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_AUDIO_SETTINGS, 320, 360, MENU_ALIGN_CENTER },
        },
    },
    // [27]: SCREEN_DISPLAY_ADVANCED
    {
        "FEH_DIS",
        SCREEN_DISPLAY_SETTINGS,
        3,
        {
            { MENU_ACTION_DRAW_DIST, "FEM_LOD", TI_OPTION, SCREEN_DISPLAY_ADVANCED, 57, 127, MENU_ALIGN_LEFT },
            { MENU_ACTION_FRAME_LIMITER, "FEM_FRM", TI_OPTION, SCREEN_DISPLAY_ADVANCED, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_WIDESCREEN, "FED_WIS", TI_OPTION, SCREEN_DISPLAY_ADVANCED, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_FX_QUALITY, "FED_FXQ", TI_OPTION, SCREEN_DISPLAY_ADVANCED, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_MIP_MAPPING, "FED_MIP", TI_OPTION, SCREEN_DISPLAY_ADVANCED, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_ANTIALIASING, "FED_AAS", TI_OPTION, SCREEN_DISPLAY_ADVANCED, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_RESOLUTION, "FED_RES", TI_OPTION, SCREEN_DISPLAY_ADVANCED, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_DISPLAY_SETTINGS, 320, 360, MENU_ALIGN_CENTER },
        },
    },
    // [28]: SCREEN_LANGUAGE
    {
        "FEH_LAN",
        SCREEN_OPTIONS,
        3,
        {
            { MENU_ACTION_LANG_ENGLISH, "FEL_ENG", TI_ENTER, SCREEN_LANGUAGE, 320, 132, MENU_ALIGN_CENTER },
            { MENU_ACTION_LANG_SPANISH, "FEL_SPA", TI_ENTER, SCREEN_LANGUAGE, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_DISPLAY_SETTINGS, 490, 380, MENU_ALIGN_LEFT },
        },
    },
    // [29]: SCREEN_SAVE_GAME_DONE
    {
        "FET_SG",
        SCREEN_START_GAME,
        0,
        {
            { MENU_ACTION_TEXT, "FED_LWR", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_MENU, "FEC_OKK", TI_ENTER, SCREEN_GAME_SAVE, 0, 0, MENU_ALIGN_DEFAULT },
        },
    },
    // [30]: SCREEN_SAVE_GAME_FAILED
    {
        "FET_SG",
        SCREEN_GAME_SAVE,
        0,
        {
            { MENU_ACTION_TEXT, "FEC_SVU", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_MENU, "FEC_OKK", TI_ENTER, SCREEN_GAME_SAVE, 0, 0, MENU_ALIGN_DEFAULT },
        },
    },
    // [31]: SCREEN_SAVE_WRITE_FAILED
    {
        "FET_LG",
        SCREEN_GAME_SAVE,
        0,
        {
            { MENU_ACTION_TEXT, "FEC_SVU", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
        },
    },
    // [32]: SCREEN_SAVE_FAILED_FILE_ERROR
    {
        "FET_LG",
        SCREEN_START_GAME,
        0,
        {
            { MENU_ACTION_TEXT, "FEC_LUN", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_START_GAME, 0, 0, MENU_ALIGN_DEFAULT },
        },
    },
    // [33]: SCREEN_OPTIONS
    {
        "FET_OPT",
        SCREEN_INITIAL,
        5,
        {
            { MENU_ACTION_MENU, "FEO_CON", TI_ENTER, SCREEN_CONTROLLER_SETUP, 210, 152, MENU_ALIGN_CENTER },
            { MENU_ACTION_MENU, "FEO_AUD", TI_ENTER, SCREEN_AUDIO_SETTINGS, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MENU, "FEO_DIS", TI_ENTER, SCREEN_DISPLAY_SETTINGS, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MENU, "FEH_LAN", TI_ENTER, SCREEN_LANGUAGE, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_STATS, 490, 380, MENU_ALIGN_LEFT },
        },
    },
    // [34]: SCREEN_MAIN_MENU
    {
        "FEM_MM",
        SCREEN_NONE,
        0,
        {
            { MENU_ACTION_MENU, "FEP_STG", TI_ENTER, SCREEN_START_GAME, 320, 170, MENU_ALIGN_CENTER },
            { MENU_ACTION_MENU, "FEP_OPT", TI_ENTER, SCREEN_OPTIONS, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MENU, "FEP_QUI", TI_ENTER, SCREEN_QUIT_GAME_ASK, 0, 0, MENU_ALIGN_CENTER },
        },
    },
    // [35]: SCREEN_QUIT_GAME_ASK
    {
        "FET_QG",
        SCREEN_INITIAL,
        6,
        {
            { MENU_ACTION_TEXT, "FEQ_SRE", TI_STRING, SCREEN_STATS, 0, 0, MENU_ALIGN_DEFAULT },
            { MENU_ACTION_PAUSE, "FEM_NO", TI_ENTER, SCREEN_INITIAL, 320, 215, MENU_ALIGN_CENTER },
            { MENU_ACTION_QUIT, "FEM_YES", TI_ENTER, SCREEN_INITIAL, 320, 240, MENU_ALIGN_CENTER },
        },
    },
    // [36]: SCREEN_CONTROLLER_SETUP
    {
        "FET_CTL",
        SCREEN_OPTIONS,
        0,
        {
            { MENU_ACTION_CONTROL_TYPE, "FEC_CFG", TI_OPTION, SCREEN_CONTROLLER_SETUP, 57, 150, MENU_ALIGN_LEFT },
            { MENU_ACTION_MENU, "FEC_RED", TI_ENTER, SCREEN_REDEFINE_CONTROLS, 320, 195, MENU_ALIGN_CENTER },
            { MENU_ACTION_CTRLS_JOYPAD, "FEC_CFG", TI_MOUSEJOYPAD, SCREEN_CONTROLLER_SETUP, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MENU, "FET_DEF", TI_ENTER, SCREEN_CONTROLS_RESET, 320, 275, MENU_ALIGN_CENTER },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_STATS, 0, 0, MENU_ALIGN_CENTER },
        },
    },
    // [37]: SCREEN_REDEFINE_CONTROLS
    {
        "FET_CTL",
        SCREEN_CONTROLLER_SETUP,
        1,
        {
            { MENU_ACTION_CTRLS_FOOT, "FET_CFT", TI_ENTER, SCREEN_CONTROLS_DEFINITION, 320, 190, MENU_ALIGN_CENTER },
            { MENU_ACTION_CTRLS_CAR, "FET_CCR", TI_ENTER, SCREEN_CONTROLS_DEFINITION, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_CONTROLLER_SETUP, 320, 270, MENU_ALIGN_CENTER },
        },
    },
    // [38]: SCREEN_CONTROLS_DEFINITION
    {
        "FET_CTL",
        SCREEN_REDEFINE_CONTROLS,
        0,
        {
        },
    },
    // [39]: SCREEN_MOUSE_SETTINGS
    {
        "FEC_MOU",
        SCREEN_CONTROLLER_SETUP,
        2,
        {
            { MENU_ACTION_MOUSE_SENS, "FEC_MSH", TI_OPTION, SCREEN_MOUSE_SETTINGS, 40, 130, MENU_ALIGN_LEFT },
            { MENU_ACTION_CONTROLS_MOUSE_INVERT_Y, "FEC_IVV", TI_OPTION, SCREEN_MOUSE_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_MOUSE_STEERING, "FET_MST", TI_OPTION, SCREEN_MOUSE_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_MOUSE_FLY, "FET_MFL", TI_OPTION, SCREEN_MOUSE_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_STATS, 320, 290, MENU_ALIGN_CENTER },
        },
    },
    // [40]: SCREEN_JOYPAD_SETTINGS
    {
        "FEJ_TIT",
        SCREEN_CONTROLLER_SETUP,
        2,
        {
            { MENU_ACTION_CONTROLS_JOY_INVERT_X, "FEJ_INX", TI_OPTION, SCREEN_JOYPAD_SETTINGS, 40, 130, MENU_ALIGN_LEFT },
            { MENU_ACTION_CONTROLS_JOY_INVERT_Y, "FEJ_INY", TI_OPTION, SCREEN_JOYPAD_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_CONTROLS_JOY_SWAP_AXIS1, "FEJ_INA", TI_OPTION, SCREEN_JOYPAD_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_CONTROLS_JOY_INVERT_X2, "FEJ_RNX", TI_OPTION, SCREEN_JOYPAD_SETTINGS, 40, 245, MENU_ALIGN_LEFT },
            { MENU_ACTION_CONTROLS_JOY_INVERT_Y2, "FEJ_RNY", TI_OPTION, SCREEN_JOYPAD_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_CONTROLS_JOY_SWAP_AXIS2, "FEJ_RNA", TI_OPTION, SCREEN_JOYPAD_SETTINGS, 0, 0, MENU_ALIGN_LEFT },
            { MENU_ACTION_BACK, "FEDS_TB", TI_ENTER, SCREEN_STATS, 320, 370, MENU_ALIGN_CENTER },
        },
    },
    // [41]: SCREEN_PAUSE_MENU
    {
        "FET_PAU",
        SCREEN_NONE,
        0,
        {
            { MENU_ACTION_MENU_CLOSE, "FEP_RES", TI_ENTER, SCREEN_STATS, 320, 140, MENU_ALIGN_CENTER },
            { MENU_ACTION_MENU, "FEH_SGA", TI_ENTER, SCREEN_START_GAME, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MENU, "FEH_MAP", TI_ENTER, SCREEN_MAP, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MENU, "FEP_STA", TI_ENTER, SCREEN_STATS, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MENU, "FEH_BRI", TI_ENTER, SCREEN_BRIEF, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MENU, "FEP_OPT", TI_ENTER, SCREEN_OPTIONS, 0, 0, MENU_ALIGN_CENTER },
            { MENU_ACTION_MENU, "FEP_QUI", TI_ENTER, SCREEN_QUIT_GAME_ASK, 0, 0, MENU_ALIGN_CENTER },
        },
    },
    // [42]: SCREEN_INITIAL
    {
        "",
        SCREEN_STATS,
        0,
        {
        },
    },
    // [43]: SCREEN_EMPTY
    {
        "",
        SCREEN_STATS,
        0,
        {
        },
    },
};
extern SpriteFileName FrontEndFilenames[];
extern tMenuScreen aScreensX[];
