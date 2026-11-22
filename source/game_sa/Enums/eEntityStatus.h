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
    STATUS_PLAYER_PLAYBACK_FROM_BUFFER,
    STATUS_SIMPLE,
    STATUS_PHYSICS,
    STATUS_ABANDONED,
    STATUS_WRECKED,
    STATUS_TRAIN_MOVING,
    STATUS_TRAIN_NOT_MOVING,
    STATUS_REMOTE_CONTROLLED,
    STATUS_FORCED_STOP,
    STATUS_IS_TOWED,
    STATUS_IS_SIMPLE_TOWED,
    STATUS_GHOST
};
