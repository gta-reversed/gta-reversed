/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

// https://github.com/multitheftauto/mtasa-blue/blob/master/Client/sdk/game/CEntity.h
enum eEntityStatus : uint8 {
    STATUS_PLAYER = 0,
    STATUS_PLAYER_PLAYBACK_FROM_BUFFER = 1,
    STATUS_SIMPLE = 2,
    STATUS_PHYSICS = 3,
    STATUS_ABANDONED = 4,
    STATUS_WRECKED = 5,
    STATUS_TRAIN_MOVING = 6,
    STATUS_TRAIN_NOT_MOVING = 7,
    STATUS_REMOTE_CONTROLLED = 8,
    STATUS_FORCED_STOP = 9,
    STATUS_IS_TOWED = 10,
    STATUS_IS_SIMPLE_TOWED = 11,
    STATUS_GHOST = 12,
};
