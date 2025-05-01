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
