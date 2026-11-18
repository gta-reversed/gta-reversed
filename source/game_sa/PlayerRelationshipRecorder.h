#pragma once

class CPed;

struct PlayerRelationship {
    const CPed* Ped;
    int32       Relationship;

    PlayerRelationship() { Flush(); }

    void Flush() {
        Ped = nullptr;
        Relationship = 0;
    }
};

// useless class (unfinished)
class CPlayerRelationshipRecorder {
public:
    std::array<PlayerRelationship, 16> m_Relationships;

public:
    static void InjectHooks();

public:
    CPlayerRelationshipRecorder();
    ~CPlayerRelationshipRecorder();

    void Flush();
    void ClearRelationshipWithPlayer(const CPed* ped);
    void AddRelationship(const CPed* ped, int32 value);
    int32 GetRelationshipWithPlayer(const CPed* ped);
    void RecordRelationshipWithPlayer(const CPed* ped);
};

CPlayerRelationshipRecorder& GetPlayerRelationshipRecorder();

VALIDATE_SIZE(CPlayerRelationshipRecorder, 0x80);
