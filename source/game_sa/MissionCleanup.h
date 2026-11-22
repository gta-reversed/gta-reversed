/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

constexpr auto NUM_MISSION_CLEANUP_OBJECTS = 75;

enum MissionCleanUpEntityType : uint8 {
    MISSION_CLEANUP_ENTITY_TYPE_EMPTY          = 0, // free slot

    MISSION_CLEANUP_ENTITY_TYPE_VEHICLE        = 1,
    MISSION_CLEANUP_ENTITY_TYPE_PED            = 2,
    MISSION_CLEANUP_ENTITY_TYPE_OBJECT         = 3,
    MISSION_CLEANUP_ENTITY_TYPE_PARTICLE       = 4,
    MISSION_CLEANUP_ENTITY_TYPE_GROUP          = 5,
    MISSION_CLEANUP_ENTITY_TYPE_PED_QUEUE      = 7,
    MISSION_CLEANUP_ENTITY_TYPE_TASK_SEQUENCE  = 8,
    MISSION_CLEANUP_ENTITY_TYPE_DECISION_MAKER = 9,
    // type 10 does not exist
    MISSION_CLEANUP_ENTITY_TYPE_SEARCHLIGHT    = 11,
    MISSION_CLEANUP_ENTITY_TYPE_CHECKPOINT     = 12,
    MISSION_CLEANUP_ENTITY_TYPE_TXD            = 13
};

struct tMissionCleanupEntity {
    int8  type;
    int32 handle;

    tMissionCleanupEntity() { // 0x468DC0
        type   = 0;
        handle = 0;
    }
};
VALIDATE_SIZE(tMissionCleanupEntity, 0x8);

class CMissionCleanup {
public:
    std::array<tMissionCleanupEntity, NUM_MISSION_CLEANUP_OBJECTS> m_Objects;
    int8 m_Count;

public:
    // Default constructor
    CMissionCleanup();

    // Initializes data
    void Init();

    // Performs a clean-up
    void Process();

    // Finds a free entity, returns NULL if no free entity can be found.
    tMissionCleanupEntity* FindFree();

    // Adds entity to list
    void AddEntityToList(int32 handle, MissionCleanUpEntityType type);

    // Remotes entity from list
    void RemoveEntityFromList(int32 handle, MissionCleanUpEntityType type);

    // Checks if collision has loaded for mission objects
    void CheckIfCollisionHasLoadedForMissionObjects();


    // QoL (NOTSA) - TODO: Use `PooledType` + some traits shit to get the `MissionCleanUpEntityType` of `T`
    void AddEntityToList(CObject& obj);
    void AddEntityToList(CPed& ped);
    void AddEntityToList(CVehicle& veh);

    void RemoveEntityFromList(CObject& obj);
    void RemoveEntityFromList(CPed& ped);
    void RemoveEntityFromList(CVehicle& veh);
};

VALIDATE_SIZE(CMissionCleanup, 0x25C);
