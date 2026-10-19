/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

// Genre-based names
enum eRadioID : int8 {
    RADIO_INVALID = -1,    // Used in CAudioEngine code.

    RADIO_EMERGENCY_AA,    // AA
    RADIO_CLASSIC_HIP_HOP, // Playback FM
    RADIO_COUNTRY,         // K-Rose
    RADIO_CLASSIC_ROCK,    // K-DST
    RADIO_DISCO_FUNK,      // Bounce FM
    RADIO_HOUSE_CLASSICS,  // San Fierro Underground Radio aka SF-UR
    RADIO_MODERN_HIP_HOP,  // Radio Los Santos
    RADIO_MODERN_ROCK,     // Radio X
    RADIO_NEW_JACK_SWING,  // CSR 103.9
    RADIO_REGGAE,          // K-Jah West
    RADIO_RARE_GROOVE,     // Master Sounds 98.3
    RADIO_TALK,            // WCTR
    RADIO_USER_TRACKS,
    RADIO_OFF,
    RADIO_COUNT,
};

inline std::optional<const char*> EnumToString(eRadioID v) {
    using namespace std::string_view_literals;
    switch (v) {
    case RADIO_INVALID: return "INVALID";
    case RADIO_EMERGENCY_AA: return "EMERGENCY_AA";
    case RADIO_CLASSIC_HIP_HOP: return "CLASSIC_HIP_HOP";
    case RADIO_COUNTRY: return "COUNTRY";
    case RADIO_CLASSIC_ROCK: return "CLASSIC_ROCK";
    case RADIO_DISCO_FUNK: return "DISCO_FUNK";
    case RADIO_HOUSE_CLASSICS: return "HOUSE_CLASSICS";
    case RADIO_MODERN_HIP_HOP: return "MODERN_HIP_HOP";
    case RADIO_MODERN_ROCK: return "MODERN_ROCK";
    case RADIO_NEW_JACK_SWING: return "NEW_JACK_SWING";
    case RADIO_REGGAE: return "REGGAE";
    case RADIO_RARE_GROOVE: return "RARE_GROOVE";
    case RADIO_TALK: return "TALK";
    case RADIO_USER_TRACKS: return "USER_TRACKS";
    case RADIO_OFF: return "OFF";
    default: return std::nullopt;
    }
}
