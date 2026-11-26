#pragma once

class CPed;

struct PlayerRelationship {
    const CPed* Ped;
    uint8       Relationship; // The original only ever reads/writes a byte (the 3 bytes after it are padding)

    PlayerRelationship() { Flush(); }

    void Flush() {
        Ped = nullptr;
        Relationship = 0;
    }
};

VALIDATE_SIZE(PlayerRelationship, 0x8);

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
