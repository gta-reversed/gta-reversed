#pragma once

enum eReplayMode : int8 {
    MODE_RECORD   = 0,
    MODE_PLAYBACK = 1
};

enum eReplayCamMode : uint8 {
    REPLAY_CAM_MODE_AS_STORED = 0,
    REPLAY_CAM_MODE_TOPDOWN,
    REPLAY_CAM_MODE_FIXED
};

enum eReplayPacket : uint8 {
    REPLAY_PACKET_END           = 0,
    REPLAY_PACKET_VEHICLE       = 1,
    REPLAY_PACKET_BIKE          = 2,
    REPLAY_PACKET_PED_HEADER    = 3,
    REPLAY_PACKET_PED_UPDATE    = 4,
    REPLAY_PACKET_GENERAL       = 5, // also called 'camera'.
    REPLAY_PACKET_CLOCK         = 6,
    REPLAY_PACKET_WEATHER       = 7,
    REPLAY_PACKET_END_OF_FRAME  = 8,
    REPLAY_PACKET_TIMER         = 9,
    REPLAY_PACKET_BULLET_TRACES = 10,
    REPLAY_PACKET_PARTICLE      = 11,
    REPLAY_PACKET_MISC          = 12,

    REPLAY_PACKET_DELETED_VEH   = 13,
    REPLAY_PACKET_DELETED_PED   = 14,
    REPLAY_PACKET_BMX           = 15,
    REPLAY_PACKET_HELI          = 16,
    REPLAY_PACKET_PLANE         = 17,
    REPLAY_PACKET_TRAIN         = 18,
    REPLAY_PACKET_CLOTHES       = 19,

    NUM_REPLAY_PACKETS,
};
