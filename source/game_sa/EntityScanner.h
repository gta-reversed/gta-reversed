/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RepeatSector.h"
#include "TickCounter.h"
#include <extensions/utility.hpp>

class CEntity;
class CPed;

static constexpr size_t MAX_NUM_ENTITIES = 16; // Mobile

// Just a theory..
// Since this is originally a virtual class..
// Maybe it was templated? Like: `template<class Entity_t, size_t MaxCount>` ?

class CEntityScanner {
protected:
    CTickCounter m_timer;
    std::array<CEntity*, MAX_NUM_ENTITIES> m_apEntities; /// SEEMINGLY: The array might have "holes" in it, also it's sorted by distance (closer to further)
    CEntity* m_pClosestEntityInRange;

public:
    static const int32 ms_iScanPeriod = 10; // Mobile, unused

    friend class CPedIntelligence; // NOTE: There's no other way at all

public:
    CEntityScanner();
    ~CEntityScanner();

    std::span<CEntity*> GetEntitiesInRange() { // NOTSA modificated
        return { m_apEntities.data(), m_apEntities.size() };
    }

    // unused
    CTickCounter* GetTimer() {
        return &m_timer;
    }

protected:
    virtual void ScanForEntitiesInRange(const eRepeatSectorList sectorList, const CPed& ped);
    CEntity* GetClosestEntity() const {
        return m_pClosestEntityInRange;
    }

    void Clear();

private: // NOTSA:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEntityScanner* Constructor() {
        this->CEntityScanner::CEntityScanner();
        return this;
    }

    CEntityScanner* Destructor() {
        this->CEntityScanner::~CEntityScanner();
        return this;
    }

public:
    /// View of all non-null entities as a view of `T&`
    template<typename T = CEntity>
    auto GetEntities() const {
        using namespace rng::views;
        using namespace notsa;

        return m_apEntities
             | filter([](auto&& e) { return e != nullptr; })  // Filter all null
             | transform([](CEntity* e) -> T& { return static_cast<T&>(*e); }); // Cast to required type and dereference
    }
};

VALIDATE_SIZE(CEntityScanner, 0x50);
