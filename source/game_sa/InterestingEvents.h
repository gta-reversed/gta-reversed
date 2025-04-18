#pragma once

constexpr int8 MAX_INTERESTING_EVENTS = 8;

struct TInterestingEvent {
    int32    type;
    uint32   time;
    CEntity* entity;
};

class CInterestingEvents {
public:
    enum EType {
        INTERESTING_EVENT_0     =  0, //
        PEDS_CHATTING           =  1, // CTaskComplexChat::CreateFirstSubTask
        INTERESTING_EVENT_2     =  2, // CTaskComplexSunbathe::CreateNextSubTask
        INTERESTING_EVENT_3     =  3, // CTaskComplexUseAttractor::CreateFirstSubTask CTaskComplexUseClosestFreeScriptedAttractor::CreateFirstSubTask
        INTERESTING_EVENT_4     =  4, //
        INTERESTING_EVENT_5     =  5, //
        INTERESTING_EVENT_6     =  6, //
        INTERESTING_EVENT_7     =  7, // CInterestingEvents::ScanForNearbyEntities
        INTERESTING_EVENT_8     =  8, // CEventHandler::ComputeSexyVehicleResponse
        INTERESTING_EVENT_9     =  9, // CSexyPedScanner::ScanForSexyPedEvents
        INTERESTING_EVENT_10    = 10, //
        INTERESTING_EVENT_11    = 11, // CTaskSimpleGiveCPR::ReviveDeadPed
        INTERESTING_EVENT_12    = 12, // CTaskComplexMedicTreatInjuredPed::CreateNextSubTask
        INTERESTING_EVENT_13    = 13, // CTaskComplexFleeEntity::CreateFirstSubTask CTaskComplexSmartFleeEntity::CreateFirstSubTask
        INTERESTING_EVENT_14    = 14, // CInterestingEvents::ScanForNearbyEntities
        INTERESTING_EVENT_15    = 15, // CEventHandler::ComputeDamageResponse
        INTERESTING_EVENT_16    = 16, // CEventHandler::ComputeKnockOffBikeResponse
        VEHICLE_DAMAGE          = 17, //
        INTERESTING_EVENT_18    = 18, // CTaskComplexRoadRage::CreateFirstSubTask
        INTERESTING_EVENT_19    = 19, // CTaskComplexKillPedOnFootMelee::CreateFirstSubTask
        INTERESTING_EVENT_20    = 20, // CEventHandler::ComputePedEnteredVehicleResponse
        INTERESTING_EVENT_21    = 21, // CHeli::ProcessControl
        INTERESTING_EVENT_22    = 22, // CWeapon::FireSniper CWeapon::FireInstantHitFromCar2 CWeapon::FireInstantHit CWeapon::FireProjectile
        INTERESTING_EVENT_23    = 23, // CTaskAllocatorKillThreatsDriveby::AllocateTasks CTaskAllocatorKillThreatsBasic::AllocateTasks ComputeKillThreatsBasicResponse
        INTERESTING_EVENT_24    = 24, //
        INTERESTING_EVENT_25    = 25, // CTaskComplexKillCriminal::CreateFirstSubTask
        ZELDICK_OCCUPATION      = 26, // CHeli::ProcessControl
        EVENT_ATTRACTOR         = 27, //
        INTERESTING_EVENT_28    = 28, // CEventHandler::ComputeDamageResponse

        MAX_INTERESTING_EVENT_TYPES,
    };

    TInterestingEvent m_Events[MAX_INTERESTING_EVENTS];
    uint8             m_nPriorities[MAX_INTERESTING_EVENT_TYPES]; // not sure
    uint16            m_nDelays[MAX_INTERESTING_EVENT_TYPES];     // delays for update (e.g. GetInterestingEvent)
    uint32            m_nEndsOfTime[MAX_INTERESTING_EVENT_TYPES];
    union {
        struct {
            uint8 m_b1 : 1;
            uint8 m_b2 : 1;
            uint8 m_b4 : 1;
            uint8 m_b8 : 1;
        };
        uint8         m_nFlags;
    };
    uint32            m_nLastFrameUpdate;
    uint32            m_nLastScanTimeUpdate;
    float             m_fRadius;
    CVector           m_vecCenter;
    CVector           vec148;
    int8              m_nInterestingEvent;

public:
    static void InjectHooks();

    CInterestingEvents();
    CInterestingEvents* Constructor();

    ~CInterestingEvents();
    CInterestingEvents* Destructor();

    void Add(CInterestingEvents::EType type, CEntity* entity);
    void ScanForNearbyEntities();
    TInterestingEvent* GetInterestingEvent();
    void InvalidateEvent(const TInterestingEvent* event);
    void InvalidateNonVisibleEvents();
};

extern CInterestingEvents& g_InterestingEvents;
