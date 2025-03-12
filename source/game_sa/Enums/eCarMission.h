/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

enum eCarMission : int8 {
  MISSION_NONE = 0,
  MISSION_CRUISE = 1,
  MISSION_RAMPLAYER_FARAWAY = 2,
  MISSION_RAMPLAYER_CLOSE = 3,
  MISSION_BLOCKPLAYER_FARAWAY = 4,
  MISSION_BLOCKPLAYER_CLOSE = 5,
  MISSION_BLOCKPLAYER_HANDBRAKESTOP = 6,
  MISSION_WAITFORDELETION = 7,
  MISSION_GOTOCOORDINATES = 8,
  MISSION_GOTOCOORDINATES_STRAIGHTLINE = 9,
  MISSION_EMERGENCYVEHICLE_STOP = 10,
  MISSION_STOP_FOREVER = 11,
  MISSION_GOTOCOORDINATES_ACCURATE = 12,
  MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE = 13,
  MISSION_GOTOCOORDINATES_ASTHECROWSWIMS = 14,
  MISSION_RAMCAR_FARAWAY = 15,
  MISSION_RAMCAR_CLOSE = 16,
  MISSION_BLOCKCAR_FARAWAY = 17,
  MISSION_BLOCKCAR_CLOSE = 18,
  MISSION_BLOCKCAR_HANDBRAKESTOP = 19,
  MISSION_HELI_FLYTOCOORS = 20,
  MISSION_BOAT_ATTACKPLAYER = 21,
  MISSION_PLANE_FLYTOCOORS = 22,
  MISSION_HELI_ATTACK_PLAYER = 23,
  MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_1 = 24,
  MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_2 = 25,
  MISSION_BLOCKPLAYER_FORWARDANDBACK = 26,
  MISSION_PROTECTION_REAR = 27,
  MISSION_PROTECTION_FRONT = 28,
  MISSION_ESCORT_LEFT = 29,
  MISSION_ESCORT_RIGHT = 30,
  MISSION_ESCORT_REAR = 31,
  MISSION_ESCORT_FRONT = 32,
  MISSION_GOTOCOORDINATES_RACING = 33,
  MISSION_FOLLOW_RECORDED_PATH = 34,
  MISSION_PLANE_ATTACK_PLAYER = 35,
  MISSION_PLANE_FLYINDIRECTION = 36,
  MISSION_PLANE_FOLLOW_ENTITY = 37,
  MISSION_HELI_FLYINDIRECTION = 38,
  MISSION_HELI_FOLLOW_ENTITY = 39,
  MISSION_HELI_POLICE_BEHAVIOUR = 40,
  MISSION_HELI_NEWS_BEHAVIOUR = 41,
  MISSION_HELI_FLY_AWAY_FROM_PLAYER = 42,
  MISSION_APPROACHPLAYER_FARAWAY = 43,
  MISSION_APPROACHPLAYER_CLOSE = 44,
  MISSION_PARK_PERPENDICULAR = 45,
  MISSION_PARK_PARALLEL = 46,
  MISSION_HELI_LAND = 47,
  MISSION_HELI_LAND_TOUCHING_DOWN = 48,
  MISSION_PARK_PERPENDICULAR_2 = 49,
  MISSION_PARK_PARALLEL_2 = 50,
  MISSION_HELI_KEEP_ENTITY_IN_VIEW = 51,
  MISSION_FOLLOWCAR_FARAWAY = 52,
  MISSION_FOLLOWCAR_CLOSE = 53,
  MISSION_KILLPED_FARAWAY = 54,
  MISSION_KILLPED_CLOSE = 55,
  MISSION_HELI_ATTACK_PLAYER_FLY_AWAY = 56,
  MISSION_PLANE_CRASH_AND_BURN = 57,
  MISSION_HELI_CRASH_AND_BURN = 58,
  MISSION_PLANE_ATTACK_PLAYER_POLICE = 59,
  MISSION_DO_DRIVEBY_CLOSE = 60,
  MISSION_DO_DRIVEBY_FARAWAY = 61,
  MISSION_PLANE_DOG_FIGHT_ENTITY = 62,
  MISSION_PLANE_DOG_FIGHT_PLAYER = 63,
  MISSION_BOAT_CIRCLEPLAYER = 64,
  MISSION_ESCORT_LEFT_FARAWAY = 65,
  MISSION_ESCORT_RIGHT_FARAWAY = 66,
  MISSION_ESCORT_REAR_FARAWAY = 67,
  MISSION_ESCORT_FRONT_FARAWAY = 68,
};
