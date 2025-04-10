#pragma once

tMenuScreen aScreens[] = {
  // 00 Stats
  {
    "FEP_STA",
    SCREEN_INITIAL,
    3,
    {
      { MENU_ACTION_STAT, "FES_PLA", eMenuEntryType::TI_OPTION, SCREEN_NOP,      57, 120,   MENU_ALIGN_LEFT   }, // Player
      { MENU_ACTION_STAT, "FES_MON", eMenuEntryType::TI_OPTION, SCREEN_NOP,       0,   0,   MENU_ALIGN_LEFT   }, // Money
      { MENU_ACTION_STAT, "FES_WEA", eMenuEntryType::TI_OPTION, SCREEN_NOP,       0,   0,   MENU_ALIGN_LEFT   }, // Weapons
      { MENU_ACTION_STAT, "FES_CRI", eMenuEntryType::TI_OPTION, SCREEN_NOP,       0,   0,   MENU_ALIGN_LEFT   }, // Crimes
      { MENU_ACTION_STAT, "FES_GAN", eMenuEntryType::TI_OPTION, SCREEN_NOP,       0,   0,   MENU_ALIGN_LEFT   }, // Gangs
      { MENU_ACTION_STAT, "FES_ACH", eMenuEntryType::TI_OPTION, SCREEN_NOP,       0,   0,   MENU_ALIGN_LEFT   }, // Achievements
      { MENU_ACTION_STAT, "FES_MIS", eMenuEntryType::TI_OPTION, SCREEN_NOP,       0,   0,   MENU_ALIGN_LEFT   }, // Mission
      { MENU_ACTION_STAT, "FES_MSC", eMenuEntryType::TI_OPTION, SCREEN_NOP,       0,   0,   MENU_ALIGN_LEFT   }, // Misc
      { MENU_ACTION_BACK, "FEDS_TB", eMenuEntryType::TI_ENTER, SCREEN_INITIAL, 320, 380,   MENU_ALIGN_CENTER }, // Back
    }
  },

  // 01 Game
  {
    "FEH_LOA",
    SCREEN_INITIAL,
    1,
    {
      { MENU_ACTION_NEW_GAME, "FES_NGA", eMenuEntryType::TI_ENTER, SCREEN_NEW_GAME_ASK,  80, 150,   MENU_ALIGN_LEFT }, // New Game
      { MENU_ACTION_MENU,     "FES_LOA", eMenuEntryType::TI_ENTER, SCREEN_LOAD_GAME,      0,   0,   MENU_ALIGN_LEFT }, // Load Game
      { MENU_ACTION_MENU,     "FES_DEL", eMenuEntryType::TI_ENTER, SCREEN_DELETE_GAME,    0,   0,   MENU_ALIGN_LEFT }, // Delete Game
      { MENU_ACTION_BACK,     "FEDS_TB", eMenuEntryType::TI_ENTER, SCREEN_NOP,          490, 380,   MENU_ALIGN_LEFT }, // Back
    }
  },

  // 02 Brief
  {
    "FEH_BRI",
    SCREEN_INITIAL,
    4,
    {
      { MENU_ACTION_BACK, "FEDS_TB", eMenuEntryType::TI_ENTER, SCREEN_INITIAL, 57, 410,   MENU_ALIGN_LEFT }, // Back
    }
  },

  // 03 Audio
  {
    "FEH_AUD",
    SCREEN_OPTIONS,
    1,
    {
      { MENU_ACTION_RADIO_VOL,     "FEA_MUS", eMenuEntryType::TI_OPTION, SCREEN_AUDIO_SETTINGS,             57, 100,   MENU_ALIGN_LEFT   }, // Radio
      { MENU_ACTION_SFX_VOL,       "FEA_SFX", eMenuEntryType::TI_OPTION, SCREEN_AUDIO_SETTINGS,              0,   0,   MENU_ALIGN_LEFT   }, // SFX
      { MENU_ACTION_RADIO_EQ,      "FEA_BAS", eMenuEntryType::TI_OPTION, SCREEN_AUDIO_SETTINGS,              0,   0,   MENU_ALIGN_LEFT   }, // Radio EQ
      { MENU_ACTION_RADIO_RETUNE,  "FEA_ART", eMenuEntryType::TI_OPTION, SCREEN_AUDIO_SETTINGS,              0,   0,   MENU_ALIGN_LEFT   }, // Radio Auto-Tune
      { MENU_ACTION_MENU,          "FEA_TIT", eMenuEntryType::TI_ENTER, SCREEN_USER_TRACKS_OPTIONS,       320, 225,   MENU_ALIGN_CENTER }, // User Radio Options
      { MENU_ACTION_RADIO_STATION, "FEA_RSS", eMenuEntryType::TI_OPTION, SCREEN_AUDIO_SETTINGS,             57, 260,   MENU_ALIGN_LEFT   }, // Radio station
      { MENU_ACTION_MENU,          "FET_DEF", eMenuEntryType::TI_ENTER, SCREEN_ASK_AUDIO_DEFAULT_SETS,    320, 367,   MENU_ALIGN_CENTER }, // Restore Defaults
      { MENU_ACTION_BACK,          "FEDS_TB", eMenuEntryType::TI_ENTER, SCREEN_INITIAL,                     0,   0,   MENU_ALIGN_CENTER }, // Back
    }
  },

  // 04 Display
  {
    "FEH_DIS",
    SCREEN_OPTIONS,
    2,
    {
      { MENU_ACTION_BRIGHTNESS,   "FED_BRI", eMenuEntryType::TI_OPTION, SCREEN_DISPLAY_SETTINGS,           57, 127,   MENU_ALIGN_LEFT   }, // Brightness
      { MENU_ACTION_SHOW_LEGEND,  "MAP_LEG", eMenuEntryType::TI_OPTION, SCREEN_DISPLAY_SETTINGS,            0,   0,   MENU_ALIGN_LEFT   }, // Legend
      { MENU_ACTION_RADAR_MODE,   "FED_RDR", eMenuEntryType::TI_OPTION, SCREEN_DISPLAY_SETTINGS,            0,   0,   MENU_ALIGN_LEFT   }, // Radar Mode
      { MENU_ACTION_HUD_MODE,     "FED_HUD", eMenuEntryType::TI_OPTION, SCREEN_DISPLAY_SETTINGS,            0,   0,   MENU_ALIGN_LEFT   }, // HUD Mode
      { MENU_ACTION_SUBTITLES,    "FED_SUB", eMenuEntryType::TI_OPTION, SCREEN_DISPLAY_SETTINGS,            0,   0,   MENU_ALIGN_LEFT   }, // Subtitles
      { MENU_ACTION_STORE_PHOTOS, "FED_GLS", eMenuEntryType::TI_OPTION, SCREEN_DISPLAY_SETTINGS,            0,   0,   MENU_ALIGN_LEFT   }, // Store Gallery Photos
      { MENU_ACTION_MENU,         "FED_ADV", eMenuEntryType::TI_ENTER, SCREEN_DISPLAY_ADVANCED,          320, 320,   MENU_ALIGN_CENTER }, // Advanced
      { MENU_ACTION_MENU,         "FET_DEF", eMenuEntryType::TI_ENTER, SCREEN_ASK_DISPLAY_DEFAULT_SETS,  320, 365,   MENU_ALIGN_CENTER }, // Restore Defaults
      { MENU_ACTION_BACK,         "FEDS_TB", eMenuEntryType::TI_ENTER, SCREEN_OPTIONS,                     0,   0,   MENU_ALIGN_CENTER }, // Back
    }
  },

  // 05 Map
  {
    "FEH_MAP",
    SCREEN_INITIAL,
    2,
    {
      { MENU_ACTION_BACK, "FEDS_TB", eMenuEntryType::TI_ENTER,  SCREEN_INITIAL, 57, 410,   MENU_ALIGN_LEFT }, // Back
    }
  },

  // 06 New Game
  {
    "FES_NGA",
    SCREEN_START_GAME,
    0,
    {
      { MENU_ACTION_TEXT,          "FESZ_QR"  }, // Are you sure you want to start a new game? All current game progress will be lost. Proceed?
      { MENU_ACTION_NO,            "FEM_NO",  eMenuEntryType::TI_ENTER,  SCREEN_START_GAME,   320, 215,   MENU_ALIGN_CENTER }, // No
      { MENU_ACTION_STANDARD_GAME, "FEM_YES", eMenuEntryType::TI_ENTER,  SCREEN_NEW_GAME_ASK, 320, 240,   MENU_ALIGN_CENTER }, // Yes
    }
  },

  // 07 New Game
  {
    "FES_NGA",
    SCREEN_START_GAME,
    0,
    {
      { MENU_ACTION_TEXT,  "FEN_NGS", eMenuEntryType::TI_STRING,   SCREEN_NOP,            0,   0,   MENU_ALIGN_LEFT },   // Please select which new game you wish to start:
      { MENU_ACTION_MENU,  "FEN_NGX", eMenuEntryType::TI_ENTER, SCREEN_NEW_GAME_ASK, 320, 150,   MENU_ALIGN_CENTER }, // San Andreas (standard game)
      { MENU_ACTION_MPACK, "FEN_MPX", eMenuEntryType::TI_MPACK,  SCREEN_NOP,            0,   0,   MENU_ALIGN_CENTER }, // Mission Pack
      { MENU_ACTION_MPACK, "FEN_MPX", eMenuEntryType::TI_MPACK,  SCREEN_NOP,            0,   0,   MENU_ALIGN_CENTER }, // Mission Pack
      { MENU_ACTION_MPACK, "FEN_MPX", eMenuEntryType::TI_MPACK,  SCREEN_NOP,            0,   0,   MENU_ALIGN_CENTER }, // Mission Pack
      { MENU_ACTION_MPACK, "FEN_MPX", eMenuEntryType::TI_MPACK,  SCREEN_NOP,            0,   0,   MENU_ALIGN_CENTER }, // Mission Pack
      { MENU_ACTION_MPACK, "FEN_MPX", eMenuEntryType::TI_MPACK,  SCREEN_NOP,            0,   0,   MENU_ALIGN_CENTER }, // Mission Pack
      { MENU_ACTION_MPACK, "FEN_MPX", eMenuEntryType::TI_MPACK,  SCREEN_NOP,            0,   0,   MENU_ALIGN_CENTER }, // Mission Pack
      { MENU_ACTION_MPACK, "FEN_MPX", eMenuEntryType::TI_MPACK,  SCREEN_NOP,            0,   0,   MENU_ALIGN_CENTER }, // Mission Pack
      { MENU_ACTION_MPACK, "FEN_MPX", eMenuEntryType::TI_MPACK,  SCREEN_NOP,            0,   0,   MENU_ALIGN_CENTER }, // Mission Pack
      { MENU_ACTION_MPACK, "FEN_MPX", eMenuEntryType::TI_MPACK,  SCREEN_NOP,            0,   0,   MENU_ALIGN_CENTER }, // Mission Pack
      { MENU_ACTION_MPACK, "FEN_MPX", eMenuEntryType::TI_MPACK,  SCREEN_NOP,            0,   0,   MENU_ALIGN_CENTER }, // Mission Pack
    }
  },

  // 08 Load Mission
  {
    "FES_LMI",
    SCREEN_START_GAME,
    0,
    {
      { MENU_ACTION_TEXT,      "FESZ_QM"  }, // Are you sure you want to load a San Andreas Mission Pack? All current game progress will be lost. Proceed?
      { MENU_ACTION_NO,        "FEM_NO",  eMenuEntryType::TI_ENTER, SCREEN_START_GAME,         320, 215,   MENU_ALIGN_CENTER },
      { MENU_ACTION_MPACKGAME, "FEM_YES", eMenuEntryType::TI_ENTER, SCREEN_NOP,                320, 240,   MENU_ALIGN_CENTER },
    }
  },

  // 09 Load Game
  {
    "FET_LG",
    SCREEN_START_GAME,
    1,
    {
      { MENU_ACTION_TEXT,      "FES_SEL", eMenuEntryType::TI_STRING,   SCREEN_NOP,     0,   0,   MENU_ALIGN_LEFT   }, // Select save file to load:
      { MENU_ACTION_SAVE_SLOT, "FEM_SL1", eMenuEntryType::TI_SLOT1, SCREEN_NOP,    80, 150,   MENU_ALIGN_LEFT   }, // Save File 1 Not Present
      { MENU_ACTION_SAVE_SLOT, "FEM_SL2", eMenuEntryType::TI_SLOT2, SCREEN_NOP,    80, 170,   MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_SLOT, "FEM_SL3", eMenuEntryType::TI_SLOT3, SCREEN_NOP,    80, 190,   MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_SLOT, "FEM_SL4", eMenuEntryType::TI_SLOT4, SCREEN_NOP,    80, 210,   MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_SLOT, "FEM_SL5", eMenuEntryType::TI_SLOT5, SCREEN_NOP,    80, 230,   MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_SLOT, "FEM_SL6", eMenuEntryType::TI_SLOT6, SCREEN_NOP,    80, 250,   MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_SLOT, "FEM_SL7", eMenuEntryType::TI_SLOT7, SCREEN_NOP,    80, 270,   MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_SLOT, "FEM_SL8", eMenuEntryType::TI_SLOT8, SCREEN_NOP,    80, 290,   MENU_ALIGN_LEFT   },
      { MENU_ACTION_BACK,      "FEDS_TB", eMenuEntryType::TI_ENTER, SCREEN_NOP,   490, 380,   MENU_ALIGN_CENTER }, // Back
    }
  },

  // 10 Delete Game
  {
    "FES_DEL",
    SCREEN_START_GAME,
    2,
    {
      { MENU_ACTION_TEXT,      "FES_SED", eMenuEntryType::TI_STRING,   SCREEN_NOP,   0,   0, MENU_ALIGN_LEFT   }, // Select save file to delete:
      { MENU_ACTION_SAVE_SLOT, "FEM_SL1", eMenuEntryType::TI_SLOT1, SCREEN_NOP,  80, 150, MENU_ALIGN_LEFT   }, // Save File 1 Not Present
      { MENU_ACTION_SAVE_SLOT, "FEM_SL2", eMenuEntryType::TI_SLOT2, SCREEN_NOP,  80, 170, MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_SLOT, "FEM_SL3", eMenuEntryType::TI_SLOT3, SCREEN_NOP,  80, 190, MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_SLOT, "FEM_SL4", eMenuEntryType::TI_SLOT4, SCREEN_NOP,  80, 210, MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_SLOT, "FEM_SL5", eMenuEntryType::TI_SLOT5, SCREEN_NOP,  80, 230, MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_SLOT, "FEM_SL6", eMenuEntryType::TI_SLOT6, SCREEN_NOP,  80, 250, MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_SLOT, "FEM_SL7", eMenuEntryType::TI_SLOT7, SCREEN_NOP,  80, 270, MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_SLOT, "FEM_SL8", eMenuEntryType::TI_SLOT8, SCREEN_NOP,  80, 290, MENU_ALIGN_LEFT   },
      { MENU_ACTION_BACK,      "FEDS_TB", eMenuEntryType::TI_ENTER, SCREEN_NOP, 490, 380, MENU_ALIGN_CENTER }, // Back
    }
  },

  // 11 Load Game
  {
    "FET_LG",
    SCREEN_LOAD_GAME,
    0,
    {
      { MENU_ACTION_TEXT, "FESZ_QL"  }, // All unsaved progress in your current game will be lost. Proceed with loading?
      { MENU_ACTION_NO,   "FEM_NO",  eMenuEntryType::TI_ENTER, SCREEN_LOAD_GAME,       320, 215, MENU_ALIGN_CENTER }, // No
      { MENU_ACTION_YES,  "FEM_YES", eMenuEntryType::TI_ENTER, SCREEN_LOAD_FIRST_SAVE, 320, 240, MENU_ALIGN_CENTER }, // Yes
    }
  },

  // 12 Delete Game
  {
    "FES_DEL",
    SCREEN_DELETE_GAME,
    0,
    {
      { MENU_ACTION_TEXT, "FESZ_QD"  }, // Are you sure you wish to delete this save file?
      { MENU_ACTION_NO,   "FEM_NO",  eMenuEntryType::TI_ENTER, SCREEN_DELETE_GAME,     320, 215, MENU_ALIGN_CENTER }, // No
      { MENU_ACTION_YES,  "FEM_YES", eMenuEntryType::TI_ENTER, SCREEN_DELETE_FINISHED, 320, 240, MENU_ALIGN_CENTER }, // Yes
    }
  },

  // 13 Load Game
  {
    "FET_LG",
    SCREEN_LOAD_GAME,
    0,
  },

  // 14 Delete Game
  {
    "FES_DEL",
    SCREEN_DELETE_GAME,
    0,
  },

  // 15 Delete Game
  {
    "FES_DEL",
    SCREEN_START_GAME,
    0,
    {
      { MENU_ACTION_TEXT, "FES_DSC" }, // Delete Successful. Select OK to continue.
      { MENU_ACTION_MENU, "FEM_OK", eMenuEntryType::TI_ENTER, SCREEN_START_GAME, 320, 240, MENU_ALIGN_CENTER }, // OK
    }
  },

  // 16 Save Game
  {
    "FET_SG",
    SCREEN_NONE,
    0,
    {
      { MENU_ACTION_TEXT,      "FES_SES", eMenuEntryType::TI_STRING,   SCREEN_NOP,              0,   0,   MENU_ALIGN_LEFT   }, // Select file you wish to save to:
      { MENU_ACTION_SAVE_GAME, "FEM_SL1", eMenuEntryType::TI_SLOT1, SCREEN_SAVE_WRITE_ASK,  80, 150,   MENU_ALIGN_LEFT   }, // Save File 1 Not Present
      { MENU_ACTION_SAVE_GAME, "FEM_SL2", eMenuEntryType::TI_SLOT2, SCREEN_SAVE_WRITE_ASK,  80, 170,   MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_GAME, "FEM_SL3", eMenuEntryType::TI_SLOT3, SCREEN_SAVE_WRITE_ASK,  80, 190,   MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_GAME, "FEM_SL4", eMenuEntryType::TI_SLOT4, SCREEN_SAVE_WRITE_ASK,  80, 210,   MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_GAME, "FEM_SL5", eMenuEntryType::TI_SLOT5, SCREEN_SAVE_WRITE_ASK,  80, 230,   MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_GAME, "FEM_SL6", eMenuEntryType::TI_SLOT6, SCREEN_SAVE_WRITE_ASK,  80, 250,   MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_GAME, "FEM_SL7", eMenuEntryType::TI_SLOT7, SCREEN_SAVE_WRITE_ASK,  80, 270,   MENU_ALIGN_LEFT   },
      { MENU_ACTION_SAVE_GAME, "FEM_SL8", eMenuEntryType::TI_SLOT8, SCREEN_SAVE_WRITE_ASK,  80, 290,   MENU_ALIGN_LEFT   }, // Save File 8 Not Present
      { MENU_ACTION_15,        "FESZ_CA", eMenuEntryType::TI_ENTER, SCREEN_NOP,            470, 380,   MENU_ALIGN_CENTER }, // Cancel
    }
  },

  // 17 Save Game
  {
    "FET_SG",
    SCREEN_GAME_SAVE,
    0,
    {
      { MENU_ACTION_TEXT, "FESZ_QZ"                                                                     }, // Are you sure you wish to save?
      { MENU_ACTION_NO,   "FEM_NO",  eMenuEntryType::TI_ENTER, SCREEN_GAME_SAVE,   320, 215, MENU_ALIGN_CENTER }, // No
      { MENU_ACTION_YES,  "FEM_YES", eMenuEntryType::TI_ENTER, SCREEN_SAVE_DONE_1, 320, 240, MENU_ALIGN_CENTER }, // Yes
    }
  },

  // 18 Save Game
  {
    "FET_SG",
    SCREEN_GAME_SAVE,
    0,
  },

  // 19 Save Game
  {
    "FET_SG",
    SCREEN_GAME_SAVE,
    0,
    {
      { MENU_ACTION_TEXT,       "FES_SSC"                                                            }, // Save Successful. Select OK to continue.
      { MENU_ACTION_MENU_CLOSE, "FEM_OK", eMenuEntryType::TI_ENTER, SCREEN_NOP, 320, 240, MENU_ALIGN_CENTER }, // OK
    }
  },

  // 20 Save Game
  {
    "FET_SG",
    SCREEN_INITIAL,
    0,
    {
      { MENU_ACTION_TEXT                                                                         },
      { MENU_ACTION_MENU, "FEM_OK", eMenuEntryType::TI_ENTER, SCREEN_NOP,   320, 270, MENU_ALIGN_CENTER }, // OK
    }
  },

  // 21 Load Game
  {
    "FET_LG",
    SCREEN_INITIAL,
    0,
    {
      { MENU_ACTION_TEXT                                                                         },
      { MENU_ACTION_MENU, "FEM_OK", eMenuEntryType::TI_ENTER, SCREEN_NOP,   320, 270, MENU_ALIGN_CENTER }, // OK
    }
  },

  // 22 Save Game
  {
    "FET_SG",
    SCREEN_START_GAME,
    0,
    {
      { MENU_ACTION_TEXT, "FES_CHE" }, // Warning! One or more cheats have been activated. This may affect your save game. It is recommended that you do not save this game.
      { MENU_ACTION_MENU, "FEM_OK", eMenuEntryType::TI_ENTER, SCREEN_GAME_SAVE, 320, 240, MENU_ALIGN_CENTER }, // OK
    }
  },

  // 23 Display
  {
    "FEH_DIS",
    SCREEN_DISPLAY_SETTINGS,
    7,
    {
      { MENU_ACTION_TEXT,        "FED_RDP", }, // Are you sure you want to reset your current settings to default?
      { MENU_ACTION_MENU,        "FEM_NO",  eMenuEntryType::TI_ENTER,   SCREEN_DISPLAY_SETTINGS, 320, 215,   MENU_ALIGN_CENTER }, // No
      { MENU_ACTION_RESET_CFG,   "FEM_YES", eMenuEntryType::TI_ENTER,   SCREEN_DISPLAY_SETTINGS, 320, 240,   MENU_ALIGN_CENTER }, // Yes
    }
  },

  // 24 Audio
  {
    "FEH_AUD",
    SCREEN_AUDIO_SETTINGS,
    6,
    {
      { MENU_ACTION_TEXT,      "FED_RDP"  }, // Are you sure you want to reset your current settings to default?
      { MENU_ACTION_MENU,      "FEM_NO",  eMenuEntryType::TI_ENTER, SCREEN_AUDIO_SETTINGS, 320, 215, MENU_ALIGN_CENTER }, // No
      { MENU_ACTION_RESET_CFG, "FEM_YES", eMenuEntryType::TI_ENTER, SCREEN_AUDIO_SETTINGS, 320, 240, MENU_ALIGN_CENTER }, // Yes
    }
  },

  // 25 Controller Setup
  {
    "FET_CTL",
    SCREEN_CONTROLLER_SETUP,
    3,
    {
      { MENU_ACTION_TEXT,      "FED_RDP"  }, // Are you sure you want to reset your current settings to default?
      { MENU_ACTION_MENU,      "FEM_NO",  eMenuEntryType::TI_ENTER, SCREEN_CONTROLLER_SETUP, 320, 215, MENU_ALIGN_CENTER }, // No
      { MENU_ACTION_RESET_CFG, "FEM_YES", eMenuEntryType::TI_ENTER, SCREEN_CONTROLLER_SETUP, 320, 240, MENU_ALIGN_CENTER }, // Yes
    }
  },

  // 26 User Track Options
  {
    "FEA_TIT",
    SCREEN_AUDIO_SETTINGS,
    4,
    {
      { MENU_ACTION_TEXT,                  "FEA_SUB"   }, // To play your own music in-game, create Short-Cuts to your music tracks in the 'User Tracks' folder.
      { MENU_ACTION_USER_TRACKS_PLAY_MODE, "FEA_MPM",  eMenuEntryType::TI_OPTION,  SCREEN_USER_TRACKS_OPTIONS,  57, 190,   MENU_ALIGN_LEFT   }, // Play Mode
      { MENU_ACTION_USER_TRACKS_AUTO_SCAN, "FEA_AMS",  eMenuEntryType::TI_OPTION,  SCREEN_USER_TRACKS_OPTIONS,   0,   0,   MENU_ALIGN_LEFT   }, // Automatic Media Scan
      { MENU_ACTION_USER_TRACKS_SCAN,      "FEA_SCN",  eMenuEntryType::TI_ENTER,  SCREEN_USER_TRACKS_OPTIONS,   0,   0,   MENU_ALIGN_LEFT   }, // Scan User Tracks
      { MENU_ACTION_BACK,                  "FEDS_TB",  eMenuEntryType::TI_ENTER,  SCREEN_AUDIO_SETTINGS,      320, 360,   MENU_ALIGN_CENTER }, // Back
    }
  },

  // 27 Display
  {
    "FEH_DIS",
    SCREEN_DISPLAY_SETTINGS,
    3,
    {
      { MENU_ACTION_DRAW_DIST,     "FEM_LOD", eMenuEntryType::TI_OPTION,  SCREEN_DISPLAY_ADVANCED,  57, 127,   MENU_ALIGN_LEFT   }, // Draw Distance
      { MENU_ACTION_FRAME_LIMITER, "FEM_FRM", eMenuEntryType::TI_OPTION,  SCREEN_DISPLAY_ADVANCED,   0,   0,   MENU_ALIGN_LEFT   }, // Frame Limiter
      { MENU_ACTION_WIDESCREEN,    "FED_WIS", eMenuEntryType::TI_OPTION,  SCREEN_DISPLAY_ADVANCED,   0,   0,   MENU_ALIGN_LEFT   }, // Widescreen
      { MENU_ACTION_FX_QUALITY,    "FED_FXQ", eMenuEntryType::TI_OPTION,  SCREEN_DISPLAY_ADVANCED,   0,   0,   MENU_ALIGN_LEFT   }, // Visual FX Quality
      { MENU_ACTION_MIP_MAPPING,   "FED_MIP", eMenuEntryType::TI_OPTION,  SCREEN_DISPLAY_ADVANCED,   0,   0,   MENU_ALIGN_LEFT   }, // Mip Mapping
      { MENU_ACTION_ANTIALIASING,  "FED_AAS", eMenuEntryType::TI_OPTION,  SCREEN_DISPLAY_ADVANCED,   0,   0,   MENU_ALIGN_LEFT   }, // Anti Aliasing
      { MENU_ACTION_RESOLUTION,    "FED_RES", eMenuEntryType::TI_OPTION,  SCREEN_DISPLAY_ADVANCED,   0,   0,   MENU_ALIGN_LEFT   }, // Resolution
      { MENU_ACTION_BACK,          "FEDS_TB", eMenuEntryType::TI_ENTER,  SCREEN_DISPLAY_SETTINGS, 320, 360,   MENU_ALIGN_CENTER }, // Back
    }
  },

  // 28 Language
  {
    "FEH_LAN",
    SCREEN_OPTIONS,
    3,
    {
      { MENU_ACTION_LANG_ENGLISH,  "FEL_ENG", eMenuEntryType::TI_ENTER,  SCREEN_LANGUAGE,           320, 132,   MENU_ALIGN_CENTER }, // English
#ifdef USE_EU_STUFF
      { MENU_ACTION_LANG_FRENCH,   "FEL_FRE", eMenuEntryType::TI_ENTER,  SCREEN_LANGUAGE,             0,   0,   MENU_ALIGN_CENTER }, // French
      { MENU_ACTION_LANG_GERMAN,   "FEL_GER", eMenuEntryType::TI_ENTER,  SCREEN_LANGUAGE,             0,   0,   MENU_ALIGN_CENTER }, // German
      { MENU_ACTION_LANG_ITALIAN,  "FEL_ITA", eMenuEntryType::TI_ENTER,  SCREEN_LANGUAGE,             0,   0,   MENU_ALIGN_CENTER }, // Italian
#endif
      { MENU_ACTION_LANG_SPANISH,  "FEL_SPA", eMenuEntryType::TI_ENTER,  SCREEN_LANGUAGE,             0,   0,   MENU_ALIGN_CENTER }, // Spanish
#ifdef USE_LANG_RUSSIAN
      { MENU_ACTION_LANG_RUSSIAN,  "FEL_RUS", eMenuEntryType::TI_ENTER,  SCREEN_LANGUAGE,             0,   0,   MENU_ALIGN_CENTER }, // Russian
#endif
#ifdef USE_LANG_JAPANESE
      { MENU_ACTION_LANG_JAPANESE, "FEL_JPN", eMenuEntryType::TI_ENTER,  SCREEN_LANGUAGE,             0,   0,   MENU_ALIGN_CENTER }, // Japanese
#endif
      { MENU_ACTION_BACK,          "FEDS_TB", eMenuEntryType::TI_ENTER,  SCREEN_DISPLAY_SETTINGS,   490, 380,    MENU_ALIGN_LEFT  }, // Back
    }
  },

  // 29 Save Game
  {
    "FET_SG",
    SCREEN_START_GAME,
    0,
    {
      { MENU_ACTION_TEXT, "FED_LWR"                                                                 }, // (none, maybe "Deleting data, please wait...")
      { MENU_ACTION_MENU, "FEC_OKK", eMenuEntryType::TI_ENTER,  SCREEN_GAME_SAVE, 0, 0, MENU_ALIGN_DEFAULT }, // O.K.
    }
  },

  // 30 Save Game
  {
    "FET_SG",
    SCREEN_GAME_SAVE,
    0,
    {
      { MENU_ACTION_TEXT, "FEC_SVU"                                                                }, // Save Unsuccessful.
      { MENU_ACTION_MENU, "FEC_OKK", eMenuEntryType::TI_ENTER, SCREEN_GAME_SAVE, 0, 0, MENU_ALIGN_DEFAULT }, // O.K.
    }
  },

  // 31 Load game
  {
    "FET_LG",
    SCREEN_GAME_SAVE,
    0,
    {
      { MENU_ACTION_TEXT,  "FEC_SVU"                                                            }, // Save Unsuccessful.
      { MENU_ACTION_NA,    "",        eMenuEntryType::TI_ENTER, SCREEN_NOP,   0, 0, MENU_ALIGN_DEFAULT },
    }
  },

  // 32 Load Game
  {
    "FET_LG",
    SCREEN_START_GAME,
    0,
    {
      { MENU_ACTION_TEXT, "FEC_LUN"                                                                 }, // Load Unsuccessful. File Corrupted, Please delete.
      { MENU_ACTION_BACK, "FEDS_TB", eMenuEntryType::TI_ENTER, SCREEN_START_GAME, 0, 0, MENU_ALIGN_DEFAULT }, // Back
    }
  },

  // 33 Options
  {
    "FET_OPT",
    SCREEN_INITIAL,
    5,
    {
#ifdef GAME_ADVANCED_OPTIONS
      { MENU_ACTION_MENU,        "FEO_GAM",  eMenuEntryType::TI_ENTER,  SCREEN_GAME_OPTIONS,               210, 125,   MENU_ALIGN_CENTER }, // Game
#endif
      { MENU_ACTION_MENU, "FEO_CON", eMenuEntryType::TI_ENTER, SCREEN_CONTROLLER_SETUP, 210, 152,   MENU_ALIGN_CENTER }, // Controller Setup
      { MENU_ACTION_MENU, "FEO_AUD", eMenuEntryType::TI_ENTER, SCREEN_AUDIO_SETTINGS,     0,   0,   MENU_ALIGN_CENTER }, // Audio Setup
      { MENU_ACTION_MENU, "FEO_DIS", eMenuEntryType::TI_ENTER, SCREEN_DISPLAY_SETTINGS,   0,   0,   MENU_ALIGN_CENTER }, // Display Setup
      { MENU_ACTION_MENU, "FEH_LAN", eMenuEntryType::TI_ENTER, SCREEN_LANGUAGE,           0,   0,   MENU_ALIGN_CENTER }, // Language
      { MENU_ACTION_BACK, "FEDS_TB", eMenuEntryType::TI_ENTER, SCREEN_NOP,              490, 380,   MENU_ALIGN_LEFT   }, // Back
    }
  },

  // 34 Main Menu
  {
    "FEM_MM",
    SCREEN_NONE,
    0,
    {
      { MENU_ACTION_MENU, "FEP_STG",  eMenuEntryType::TI_ENTER, SCREEN_START_GAME,        320, 170,   MENU_ALIGN_CENTER }, // Start Game
#if USE_GALLERY
      { MENU_ACTION_MENU, "FEH_GAL",  eMenuEntryType::TI_ENTER, SCREEN_GALLERY,             0,   0,   MENU_ALIGN_CENTER }, // Gallery
#endif
      { MENU_ACTION_MENU, "FEP_OPT",  eMenuEntryType::TI_ENTER, SCREEN_OPTIONS,             0,   0,   MENU_ALIGN_CENTER }, // Options
      { MENU_ACTION_MENU, "FEP_QUI",  eMenuEntryType::TI_ENTER, SCREEN_QUIT_GAME_ASK,       0,   0,   MENU_ALIGN_CENTER }, // Quit Game
    }
  },

  // 35 Quit Game
  {
    "FET_QG",
    SCREEN_INITIAL,
    6,
    {
      { MENU_ACTION_TEXT,  "FEQ_SRE"                                                                 }, // Are you sure you want to quit? All progress since the last save game will be lost. Proceed?
      { MENU_ACTION_PAUSE, "FEM_NO",  eMenuEntryType::TI_ENTER, SCREEN_INITIAL, 320, 215, MENU_ALIGN_CENTER }, // No
      { MENU_ACTION_QUIT,  "FEM_YES", eMenuEntryType::TI_ENTER, SCREEN_INITIAL, 320, 240, MENU_ALIGN_CENTER }, // Yes
    }
  },

#ifdef USE_ADVANCED_OPTIONS_MENU
  // 36
  {
    "FEO_GAM",
    SCREEN_OPTIONS,
    0,
    {
      { 75,                       "MOB_TRA",  eMenuEntryType::TI_OPTION,  SCREEN_NOP,  57, 140,   MENU_ALIGN_LEFT   }, // Traffic Mode
      { 82,                       "MOB_CAM",  eMenuEntryType::TI_OPTION,  SCREEN_NOP,  57, 175,   MENU_ALIGN_LEFT   }, // Camera Height
      { 77,                       "MOB_LAN",  eMenuEntryType::TI_OPTION,  SCREEN_NOP,  57, 210,   MENU_ALIGN_LEFT   }, // Lane Correction
      { 78,                       "MOB_TAR",  eMenuEntryType::TI_OPTION,  SCREEN_NOP,  57, 245,   MENU_ALIGN_LEFT   }, // Targeting Mode
      { 80,                       "MOB_ACC",  eMenuEntryType::TI_OPTION,  SCREEN_NOP,  57, 280,   MENU_ALIGN_LEFT   }, // Accelerometer
      { MENU_ACTION_BACK,         "FEDS_TB",  eMenuEntryType::TI_ENTER,  SCREEN_NOP, 320, 315,   MENU_ALIGN_CENTER }, // Back
    }
  },
#endif

  // 36 Controller Setup
  {
    "FET_CTL",
    SCREEN_OPTIONS,
    0,
    {
#ifdef USE_ADVANCED_OPTIONS_MENU
      { 61,                       "FEC_CFG",  eMenuEntryType::TI_OPTION,    SCREEN_CONTROLLER_SETUP,       57, 140,   MENU_ALIGN_LEFT   }, // Configuration
      { 81,                       "FEC_TOU",  eMenuEntryType::TI_OPTION,    SCREEN_NOP,                    57, 175,   MENU_ALIGN_LEFT   }, // Touch Layout
      { 74,                       "FEC_STE",  eMenuEntryType::TI_OPTION,    SCREEN_NOP,                    57, 210,   MENU_ALIGN_LEFT   }, // Touch Steering
      { MENU_ACTION_MENU,         "FEC_RED",  eMenuEntryType::TI_ENTER,    SCREEN_CONTROLS_DEFINITION,   320, 245,   MENU_ALIGN_CENTER }, // Redefine Controls
      { 69,                       "FEC_ADJ",  eMenuEntryType::TI_ENTER,    SCREEN_CONTROLS_DEFINITION,   320, 280,   MENU_ALIGN_CENTER }, // Change Button Position
      { MENU_ACTION_CTRLS_JOYPAD, "FEC_CFG",  eMenuEntryType::TI_MOUSEJOYPAD, SCREEN_CONTROLLER_SETUP,      320, 315,   MENU_ALIGN_CENTER }, // Configuration
      { MENU_ACTION_MENU,         "FET_DEF",  eMenuEntryType::TI_ENTER,    SCREEN_CONTROLS_RESET,        320, 350,   MENU_ALIGN_CENTER }, // Restore Defaults
#else
      { MENU_ACTION_CONTROL_TYPE, "FEC_CFG", eMenuEntryType::TI_OPTION,    SCREEN_CONTROLLER_SETUP,       57, 150,   MENU_ALIGN_LEFT    }, // Configuration
      { MENU_ACTION_MENU,         "FEC_RED", eMenuEntryType::TI_ENTER,    SCREEN_REDEFINE_CONTROLS,     320, 195,   MENU_ALIGN_CENTER  }, // Redefine Controls
      { MENU_ACTION_CTRLS_JOYPAD, "FEC_CFG", eMenuEntryType::TI_MOUSEJOYPAD, SCREEN_CONTROLLER_SETUP,        0,   0,   MENU_ALIGN_CENTER  }, // Configuration
      { MENU_ACTION_MENU,         "FET_DEF", eMenuEntryType::TI_ENTER,    SCREEN_CONTROLS_RESET,        320, 275,   MENU_ALIGN_CENTER  }, // Restore Defaults
#endif
      { MENU_ACTION_BACK,         "FEDS_TB", eMenuEntryType::TI_ENTER,    SCREEN_NOP,                     0,   0,   MENU_ALIGN_CENTER  }, // Back
    }
  },

  // 37 Controller Setup
  {
    "FET_CTL",
    SCREEN_CONTROLLER_SETUP,
    1,
    {
      { MENU_ACTION_CTRLS_FOOT, "FET_CFT", eMenuEntryType::TI_ENTER, SCREEN_CONTROLS_DEFINITION,     320, 190,   MENU_ALIGN_CENTER }, // Foot Controls
      { MENU_ACTION_CTRLS_CAR,  "FET_CCR", eMenuEntryType::TI_ENTER, SCREEN_CONTROLS_DEFINITION,       0,   0,   MENU_ALIGN_CENTER }, // Vehicle Controls
      { MENU_ACTION_BACK,       "FEDS_TB", eMenuEntryType::TI_ENTER, SCREEN_CONTROLLER_SETUP,        320, 270,   MENU_ALIGN_CENTER }, // Back
    }
  },

  // 38 Controller Setup
  {
    "FET_CTL",
    SCREEN_REDEFINE_CONTROLS,
    0,
  },

  // 39 Mouse Settings
  {
    "FEC_MOU",
    SCREEN_CONTROLLER_SETUP,
    2,
    {
      { MENU_ACTION_MOUSE_SENS,              "FEC_MSH",  eMenuEntryType::TI_OPTION,  SCREEN_MOUSE_SETTINGS,      40, 130,   MENU_ALIGN_LEFT   }, // Mouse Sensitivity
      { MENU_ACTION_CONTROLS_MOUSE_INVERT_Y, "FEC_IVV",  eMenuEntryType::TI_OPTION,  SCREEN_MOUSE_SETTINGS,       0,   0,   MENU_ALIGN_LEFT   }, // Invert Mouse Vertically
      { MENU_ACTION_MOUSE_STEERING,          "FET_MST",  eMenuEntryType::TI_OPTION,  SCREEN_MOUSE_SETTINGS,       0,   0,   MENU_ALIGN_LEFT   }, // Steer With Mouse
      { MENU_ACTION_MOUSE_FLY,               "FET_MFL",  eMenuEntryType::TI_OPTION,  SCREEN_MOUSE_SETTINGS,       0,   0,   MENU_ALIGN_LEFT   }, // Fly With Mouse
      { MENU_ACTION_BACK,                    "FEDS_TB",  eMenuEntryType::TI_ENTER,  SCREEN_NOP,                320, 290,   MENU_ALIGN_CENTER }, // Back
    }
  },

  // 40 Joypad Settings
  {
    "FEJ_TIT",
    SCREEN_CONTROLLER_SETUP,
    2,
    {
      { MENU_ACTION_CONTROLS_JOY_INVERT_X,   "FEJ_INX",  eMenuEntryType::TI_OPTION,  SCREEN_JOYPAD_SETTINGS,      40, 130,   MENU_ALIGN_LEFT   }, // Invert left stick x
      { MENU_ACTION_CONTROLS_JOY_INVERT_Y,   "FEJ_INY",  eMenuEntryType::TI_OPTION,  SCREEN_JOYPAD_SETTINGS,       0,   0,   MENU_ALIGN_LEFT   }, // Invert left stick y
      { MENU_ACTION_CONTROLS_JOY_SWAP_AXIS1, "FEJ_INA",  eMenuEntryType::TI_OPTION,  SCREEN_JOYPAD_SETTINGS,       0,   0,   MENU_ALIGN_LEFT   }, // Invert left axis
      { MENU_ACTION_CONTROLS_JOY_INVERT_X2,  "FEJ_RNX",  eMenuEntryType::TI_OPTION,  SCREEN_JOYPAD_SETTINGS,      40, 245,   MENU_ALIGN_LEFT   }, // Invert right stick x
      { MENU_ACTION_CONTROLS_JOY_INVERT_Y2,  "FEJ_RNY",  eMenuEntryType::TI_OPTION,  SCREEN_JOYPAD_SETTINGS,       0,   0,   MENU_ALIGN_LEFT   }, // Invert right stick y
      { MENU_ACTION_CONTROLS_JOY_SWAP_AXIS2, "FEJ_RNA",  eMenuEntryType::TI_OPTION,  SCREEN_JOYPAD_SETTINGS,       0,   0,   MENU_ALIGN_LEFT   }, // Invert right axis
      { MENU_ACTION_BACK,                    "FEDS_TB",  eMenuEntryType::TI_ENTER,  SCREEN_NOP,                 320, 370,   MENU_ALIGN_CENTER }, // Back
    }
  },

  // 41 Pause Menu
  {
    "FET_PAU",
    SCREEN_NONE,
    0,
    {
      { MENU_ACTION_MENU_CLOSE,  "FEP_RES", eMenuEntryType::TI_ENTER, SCREEN_NOP,           320, 140,   MENU_ALIGN_CENTER  }, // Resume
      { MENU_ACTION_MENU,        "FEH_SGA", eMenuEntryType::TI_ENTER, SCREEN_START_GAME,      0,   0,   MENU_ALIGN_CENTER  }, // START NEW GAME
      { MENU_ACTION_MENU,        "FEH_MAP", eMenuEntryType::TI_ENTER, SCREEN_MAP,             0,   0,   MENU_ALIGN_CENTER  }, // Map
      { MENU_ACTION_MENU,        "FEP_STA", eMenuEntryType::TI_ENTER, SCREEN_NOP,             0,   0,   MENU_ALIGN_CENTER  }, // Stats
      { MENU_ACTION_MENU,        "FEH_BRI", eMenuEntryType::TI_ENTER, SCREEN_BRIEF,           0,   0,   MENU_ALIGN_CENTER  }, // Brief
      { MENU_ACTION_MENU,        "FEP_OPT", eMenuEntryType::TI_ENTER, SCREEN_OPTIONS,         0,   0,   MENU_ALIGN_CENTER  }, // Options
      { MENU_ACTION_MENU,        "FEP_QUI", eMenuEntryType::TI_ENTER, SCREEN_QUIT_GAME_ASK,   0,   0,   MENU_ALIGN_CENTER  }, // Quit Game
      /*
      { MENU_ACTION_NA,          "",        eMenuEntryType::TI_STRING,   SCREEN_QUIT_GAME_ASK, 320, 350,   MENU_ALIGN_DEFAULT },
      { MENU_ACTION_NA,          "",        eMenuEntryType::TI_STRING,   SCREEN_QUIT_GAME_ASK, 320, 380,   MENU_ALIGN_DEFAULT },
      { MENU_ACTION_NA,          "",        eMenuEntryType::TI_STRING,   SCREEN_QUIT_GAME_ASK, 320, 410,   MENU_ALIGN_DEFAULT },
      { MENU_ACTION_NA,          "",        eMenuEntryType::TI_STRING,   SCREEN_QUIT_GAME_ASK, 320, 440,   MENU_ALIGN_DEFAULT },
      { MENU_ACTION_NA,          "",        eMenuEntryType::TI_STRING,   SCREEN_QUIT_GAME_ASK, 320, 470,   MENU_ALIGN_DEFAULT },
      */
    }
  },

  // 42
  {
    "",
    SCREEN_NOP,  
    0,
    {
      { MENU_ACTION_NA,      "",         eMenuEntryType::TI_ENTER },
    }
  },

  // 43
  /*
  {
    "",
    SCREEN_NOP,  
    0,
    {
      { MENU_ACTION_NA,      "",         eMenuEntryType::TI_ENTER },
    }
  },
  */

#if USE_GALLERY
  // 44 [xbox #23] Gallery
  {
    "FEH_GAL",
    SCREEN_INITIAL,
    0,
  },

  // 45 [xbox #24] Gallery
  {
    "FEH_GAL",
    SCREEN_GALLERY,
    0,
    {
      { MENU_ACTION_TEXT,        "",         eMenuEntryType::TI_STRING,   SCREEN_NOP,                          0,   0,   MENU_ALIGN_DEFAULT },
      { MENU_ACTION_MENU,        "FEM_OK",   eMenuEntryType::TI_ENTER, SCREEN_NOP,                        320, 270,   MENU_ALIGN_CENTER  },
    }
  },

  // 46 [xbox #25] Gallery
  {
    "FEH_GAL",
    SCREEN_NONE,
    0,
  },

  // 47 [xbox #26] Gallery
  {
    "FEH_GAL",
    0,
    0,
  },

  // 48 [xbox #27] Gallery
  {
    "FEH_GAL",
    0,
    0,
  },

  // 49 [xbox #28] Gallery
  {
    "FEH_GAL",
    0,
    0,
  },

  // 50 [xbox #29] Gallery
  {
    "FEH_GAL",
    SCREEN_GALLERY,
    0,
    {
      { MENU_ACTION_TEXT,        "FEG_DL3",  eMenuEntryType::TI_STRING,   SCREEN_NOP,               0,   0,   MENU_ALIGN_DEFAULT }, // Do you wish to delete just this one photo or all photos from your Gallery?
      { MENU_ACTION_MENU,        "FEG_DL4",  eMenuEntryType::TI_ENTER, SCREEN_GALLERY_DEL_ONE, 320, 200,   MENU_ALIGN_CENTER  }, // One photo
      { MENU_ACTION_MENU,        "FEG_DL5",  eMenuEntryType::TI_ENTER, SCREEN_GALLERY_DEL_ALL,   0,   0,   MENU_ALIGN_CENTER  }, // All photos
      { MENU_ACTION_GALLERY,     "FESZ_CA",  eMenuEntryType::TI_ENTER, SCREEN_GALLERY,           0,   0,   MENU_ALIGN_CENTER  }, // Cancel
    }
  },

  // 51 [xbox #30] Gallery
  {
    "FEH_GAL",
    SCREEN_GALLERY,
    0,
    {
      { MENU_ACTION_TEXT,        "FEG_DL2",  eMenuEntryType::TI_STRING,    SCREEN_NOP,               0,   0,   MENU_ALIGN_DEFAULT }, // Are you sure you wish to delete this photograph?
      { MENU_ACTION_GALLERY,     "FEM_NO",   eMenuEntryType::TI_ENTER,  SCREEN_GALLERY,         320, 215,   MENU_ALIGN_CENTER  }, // No
      { MENU_ACTION_YES,         "FEM_YES",  eMenuEntryType::TI_ENTER,  SCREEN_GAL_DEL_ONE_PROC,320, 240,   MENU_ALIGN_CENTER  }, // Yes
    }
  },

  // 52 [xbox #31] Gallery
  {
    "FEH_GAL",
    SCREEN_GALLERY,
    0,
    {
      { MENU_ACTION_TEXT,        "FEG_DL1",  eMenuEntryType::TI_STRING,    SCREEN_NOP,               0,   0,   MENU_ALIGN_DEFAULT }, // Are you sure you wish to delete all photographs in your Gallery?
      { MENU_ACTION_GALLERY,     "FEM_NO",   eMenuEntryType::TI_ENTER,  SCREEN_GALLERY,         320, 215,   MENU_ALIGN_CENTER  }, // No
      { MENU_ACTION_YES,         "FEM_YES",  eMenuEntryType::TI_ENTER,  SCREEN_GAL_DEL_ALL_PROC,320, 240,   MENU_ALIGN_CENTER  }, // Yes
    }
  },
#endif
};
