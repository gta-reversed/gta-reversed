#include "StdInc.h"

#include "InterestingEvents.h"

CInterestingEvents& g_InterestingEvents = *(CInterestingEvents*)0xC0B058;

/*
 * Commented hooks aren't tested.
 * */
void CInterestingEvents::InjectHooks() {
    RH_ScopedClass(CInterestingEvents);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x6023A0, { .reversed = false });
    RH_ScopedInstall(Destructor, 0x856880, { .reversed = false });
    RH_ScopedInstall(Add, 0x602590, { .reversed = false });
    RH_ScopedInstall(ScanForNearbyEntities, 0x605A30, { .reversed = false });
    RH_ScopedInstall(GetInterestingEvent, 0x6028A0, { .reversed = false });
    RH_ScopedInstall(InvalidateEvent, 0x602960);
    RH_ScopedInstall(InvalidateNonVisibleEvents, 0x6029C0);
}

// 0x6023A0
CInterestingEvents::CInterestingEvents() {
    m_nFlags = 0;
    m_b2 = true;
    m_b4 = true;
    m_b8 = true;

    m_fRadius = 30.f;
    m_nLastScanTimeUpdate = 0;
    m_nInterestingEvent = -1;
    m_nLastFrameUpdate = CTimer::GetFrameCounter() - 1;
    std::memset(m_Events, 0, sizeof(m_Events));

    const auto SetOptions = [=](auto index, auto priority, auto delay, uint32 end = 0) {
        m_nPriorities[index] = priority;
        m_nDelays[index]     = delay;
        m_nEndsOfTime[index] = end;
    };

    SetOptions(INTERESTING_EVENT_0,     5,  2000);
    SetOptions(PEDS_CHATTING,     1,  5000);
    SetOptions(INTERESTING_EVENT_2,     1,  5000);
    SetOptions(INTERESTING_EVENT_3,     1,  5000);
    SetOptions(INTERESTING_EVENT_4,     2,  3000);
    SetOptions(INTERESTING_EVENT_5,     2,  3000);
    SetOptions(INTERESTING_EVENT_6,     2,  3000);
    SetOptions(INTERESTING_EVENT_7,     2,  3000);
    SetOptions(INTERESTING_EVENT_8,     4,  3000);
    SetOptions(INTERESTING_EVENT_9,     4,  3000);
    SetOptions(INTERESTING_EVENT_10,    5,  6000);
    SetOptions(INTERESTING_EVENT_11,    6,  6000);
    SetOptions(INTERESTING_EVENT_12,    6,  8000);
    SetOptions(INTERESTING_EVENT_13,    6,  5000);
    SetOptions(INTERESTING_EVENT_14,    5,  6000);
    SetOptions(INTERESTING_EVENT_15,    9,  6000);
    SetOptions(INTERESTING_EVENT_16,    9,  6000);
    SetOptions(VEHICLE_DAMAGE,          8,  6000);
    SetOptions(INTERESTING_EVENT_18,    7,  6000);
    SetOptions(INTERESTING_EVENT_19,    6,  5000);
    SetOptions(INTERESTING_EVENT_20,    7,  6000);
    SetOptions(INTERESTING_EVENT_21,    8,  8000);
    SetOptions(INTERESTING_EVENT_22,    9,  5000);
    SetOptions(INTERESTING_EVENT_23,    9,  6000);
    SetOptions(INTERESTING_EVENT_24,    9,  6000);
    SetOptions(INTERESTING_EVENT_25,    9,  6000);
    SetOptions(ZELDICK_OCCUPATION,      9, 8000);
    SetOptions(EVENT_ATTRACTOR,         10, 4000);
    SetOptions(INTERESTING_EVENT_28,    10, 4000);
}

CInterestingEvents* CInterestingEvents::Constructor() {
    this->CInterestingEvents::CInterestingEvents();
    return this;
}

// 0x856880
CInterestingEvents::~CInterestingEvents() {
    for (auto& event : m_Events) {
        CEntity::ClearReference(event.entity);
    }
}

CInterestingEvents* CInterestingEvents::Destructor() {
    this->CInterestingEvents::~CInterestingEvents();
    return this;
}

// 0x602590
void CInterestingEvents::Add(CInterestingEvents::EType type, CEntity* entity) {
    return plugin::CallMethod<0x602590, CInterestingEvents*, CInterestingEvents::EType, CEntity*>(this, type, entity);

    if (!m_b1 || !entity)
        return;

    DEV_LOG("type={}, model={}", (int32)(type), entity->m_nModelIndex);

    const auto& camPos = CCamera::GetActiveCamera().m_vecSource;
    if (m_nLastFrameUpdate != CTimer::GetFrameCounter()) {
        m_nLastFrameUpdate = CTimer::GetFrameCounter();

        CPlayerPed* player = FindPlayerPed();
        const auto& playerPos = player->GetPosition();
        vec148 = playerPos - camPos;
        vec148.z = 0.f;
        if (vec148.NormaliseAndMag() == 0.f) {
            vec148 = player->GetMatrix().GetForward();
        }
        m_vecCenter = (vec148 * m_fRadius) + playerPos;
    }

    CVector2D distance = m_vecCenter - entity->GetPosition();
    if (distance.SquaredMagnitude() > m_fRadius * m_fRadius)
        return;

    CVector vec0 = vec148 * entity->GetPosition();
    CVector vec1 = vec148 * camPos;
    if (!m_b2 && vec0.ComponentwiseSum() - vec1.ComponentwiseSum() < 0.f)
        return;

    if (!CWorld::GetIsLineOfSightClear(camPos, entity->GetPosition(), true, false, false, false, false, true, false))
        return;

    uint32 time = CTimer::GetTimeInMS();
    for (auto index = 0; index < MAX_INTERESTING_EVENTS; index++) {
        TInterestingEvent& event = g_InterestingEvents.m_Events[index];
        if (event.entity) {
            if (m_nPriorities[type] < m_nPriorities[event.type] && CTimer::GetTimeInMS() <= event.time + static_cast<uint32>(m_nDelays[event.type]))
                continue;
            if (CTimer::GetTimeInMS() <= m_nEndsOfTime[type] || m_nInterestingEvent == index)
                continue;
        } else {
            event.type = 0;
        }

        CEntity::SafeCleanUpRef(event.entity);
        event.type = type;
        event.entity = entity;
        event.time = time;
        entity->RegisterReference(&event.entity);
        if (m_b8)
            m_nEndsOfTime[type] = time;
        else
            m_nEndsOfTime[type] = time + (m_nDelays[type] >> 1);
        break;
    }
}

// 0x605A30
void CInterestingEvents::ScanForNearbyEntities() {
    ZoneScoped;

    return plugin::CallMethod<0x605A30, CInterestingEvents*>(this);

    if (!m_b1)
        return;

    const auto UPDATE_INTERVAL = 500;
    if (CTimer::GetTimeInMS() - m_nLastScanTimeUpdate < UPDATE_INTERVAL) {
        return;
    }
    m_nLastScanTimeUpdate = CTimer::GetTimeInMS();

    CPlayerPed* player = FindPlayerPed();
    if (m_nLastFrameUpdate != CTimer::GetFrameCounter()) {
        m_nLastFrameUpdate = CTimer::GetFrameCounter();
        const auto& camPos = CCamera::GetActiveCamera().m_vecSource, playerPos = player->GetPosition();
        vec148 = playerPos - camPos;
        vec148.z = 0.f;
        if (vec148.NormaliseAndMag() == 0.f)
            vec148 = player->GetMatrix().GetForward();
        m_vecCenter = (vec148 * m_fRadius) + playerPos;
    }

    auto v0 = std::max(static_cast<int>(std::floor((m_vecCenter.x - m_fRadius) * 50.0f + 60.0f)), 0);
    auto v1 = std::max(static_cast<int>(std::floor((m_vecCenter.y - m_fRadius) * 50.0f + 60.0f)), 0);
    auto v2 = std::min(static_cast<int>(std::floor((m_vecCenter.x + m_fRadius) * 50.0f + 60.0f)), 119);
    auto v3 = std::min(static_cast<int>(std::floor((m_vecCenter.y + m_fRadius) * 50.0f + 60.0f)), 119);

    int32 startSectorX = CWorld::GetSectorX(m_vecCenter.x - m_fRadius);
    int32 startSectorY = CWorld::GetSectorY(m_vecCenter.y - m_fRadius);
    int32 endSectorX   = CWorld::GetSectorX(m_vecCenter.x + m_fRadius);
    int32 endSectorY   = CWorld::GetSectorY(m_vecCenter.y + m_fRadius);

    assert(v0 == startSectorX);
    assert(v1 == startSectorY);
    assert(v2 == endSectorX);
    assert(v3 == endSectorY);

    CWorld::IncrementCurrentScanCode();
    player->m_nScanCode = GetCurrentScanCode();

    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);
            CPtrListDoubleLink& list = repeatSector->Peds;

            for (CPtrListDoubleLink::NodeType *it = list.m_node, *next{}; it; it = next) {
                next = it->GetNext();

                auto* ped = static_cast<CPed*>(it->m_item);
                if (ped->IsScanCodeCurrent())
                    continue;

                ped->SetCurrentScanCode();

                if (ped->m_nPedState == PEDSTATE_DEAD)
                    continue;

                CEntity* entity;
                if (ped->bInVehicle) {
                    entity = ped->m_pVehicle;
                } else {
                    entity = ped;
                }

                switch (ped->m_nPedType) {
                case PED_TYPE_COP:
                    Add(INTERESTING_EVENT_5, entity);
                    break;
                case PED_TYPE_CRIMINAL:
                    Add(INTERESTING_EVENT_6, entity);
                    break;
                case PED_TYPE_PROSTITUTE:
                    Add(INTERESTING_EVENT_4, entity);
                    break;
                default:
                    if (IsPedTypeGang(ped->m_nPedType)) {
                        Add(INTERESTING_EVENT_7, entity);
                    }
                    break;
                }
            }

            for (CPtrListDoubleLink::NodeType *it = list.m_node, *next{}; it; it = next) {
                next = it->GetNext();
                auto* vehicle = static_cast<CVehicle*>(it->m_item);
                if (vehicle->m_nScanCode == GetCurrentScanCode())
                    continue;

                vehicle->m_nScanCode = GetCurrentScanCode();
                if (vehicle->physicalFlags.bRenderScorched != 0)
                    continue;

                if (!vehicle->m_pDriver)
                    continue;

                auto style = vehicle->m_autoPilot.m_nCarDrivingStyle;
                if (!style)
                    continue;

                if (style == (DRIVING_STYLE_STOP_FOR_CARS_IGNORE_LIGHTS | DRIVING_STYLE_AVOID_CARS))
                    continue;

                Add(INTERESTING_EVENT_14, vehicle);
            }
        }
    }
}

// 0x6028A0
TInterestingEvent* CInterestingEvents::GetInterestingEvent() {
    return plugin::CallMethodAndReturn<TInterestingEvent*, 0x6028A0, CInterestingEvents*>(this);

    uint32 start = CTimer::GetTimeInMS(), end = CTimer::GetTimeInMS();
    if (!m_b4 && m_nInterestingEvent != -1)
        return nullptr;

    TInterestingEvent* result = &m_Events[m_nInterestingEvent];
    if (result->entity && CTimer::GetTimeInMS() < result->time + static_cast<uint32>(m_nDelays[result->type])) {
        return result;
    }

    // update
    uint8 prevPriority = 0;
    int8 interesting = -1;
    for (auto i = 0; i < MAX_INTERESTING_EVENTS; i++, start = end) {
        TInterestingEvent& event = m_Events[i];
        if (!event.entity)
            continue;

        if (static_cast<uint16>(CGeneral::GetRandomNumber()) >= 128) {
            if (m_nPriorities[event.type] <= prevPriority)
                continue;

            if (start >= event.time + static_cast<uint32>(m_nDelays[result->type]))
                continue;
        }

        prevPriority = m_nPriorities[event.type];
        interesting = i;
    }
    m_nInterestingEvent = interesting;

    return interesting == -1 ? nullptr : &m_Events[m_nInterestingEvent];
}

// 0x602960
void CInterestingEvents::InvalidateEvent(const TInterestingEvent* event) {
    for (auto index = 0; index < MAX_INTERESTING_EVENTS; index++) {
        TInterestingEvent* tevent = &m_Events[index];
        if (tevent != event)
            continue;

        tevent->time = 0;
        CEntity::ClearReference(tevent->entity);
        if (m_nInterestingEvent == index) {
            m_nInterestingEvent = -1;
        }
    }
}

// 0x6029C0
void CInterestingEvents::InvalidateNonVisibleEvents() {
    const auto& camPos = CCamera::GetActiveCamera().m_vecSource;
    for (auto i = 0; i < MAX_INTERESTING_EVENTS; i++) {
        TInterestingEvent& event = m_Events[i];
        if (!event.entity)
            continue;

        CVector pos = event.entity->GetPosition();
        if (CWorld::GetIsLineOfSightClear(camPos, pos, true, false, false, false, false, true, false))
            continue;

        event.time = 0;
        CEntity::SafeCleanUpRef(event.entity);
        if (m_nInterestingEvent == i) {
            m_nInterestingEvent = -1;
        }
    }
}
