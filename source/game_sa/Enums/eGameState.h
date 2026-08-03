#pragma once

enum eGameState : uint8 {
    GAME_STATE_INITIAL          = 0, // EAX / NVIDIA splashes
    GAME_STATE_LOGO             = 1, // Playing Logo.mpg (possibly incorrect)
    GAME_STATE_PLAYING_LOGO     = 2, // Init pads
    GAME_STATE_TITLE            = 3, // Playing GTAtitles.mpg
    GAME_STATE_PLAYING_INTRO    = 4, // Init pads (same state as 2)
    GAME_STATE_FRONTEND_LOADING = 5, // Start game loading (showing title splash)
    GAME_STATE_FRONTEND_LOADED  = 6, // End loading
    GAME_STATE_FRONTEND_IDLE    = 7, // Frontend menu (loop state)
    GAME_STATE_LOADING_STARTED  = 8, // Loading the game
    GAME_STATE_IDLE             = 9, // In-game (loop state)
};

inline std::optional<const char*> EnumToString(eGameState gs) {
    switch (gs) {
    case GAME_STATE_INITIAL:          return "INITIAL";
    case GAME_STATE_LOGO:             return "LOGO";
    case GAME_STATE_PLAYING_LOGO:     return "PLAYING_LOGO";
    case GAME_STATE_TITLE:            return "TITLE";
    case GAME_STATE_PLAYING_INTRO:    return "PLAYING_INTRO";
    case GAME_STATE_FRONTEND_LOADING: return "FRONTEND_LOADING";
    case GAME_STATE_FRONTEND_LOADED:  return "FRONTEND_LOADED";
    case GAME_STATE_FRONTEND_IDLE:    return "FRONTEND_IDLE";
    case GAME_STATE_LOADING_STARTED:  return "LOADING_STARTED";
    case GAME_STATE_IDLE:             return "IDLE";
    default:                          return std::nullopt;
    }
}
