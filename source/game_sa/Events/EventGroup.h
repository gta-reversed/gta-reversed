/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Event.h"

class CPed;

const int32 TOTAL_EVENTS_PER_EVENTGROUP = 16;

class NOTSA_EXPORT_VTABLE CEventGroup {
public:
    static void InjectHooks();
   
    explicit CEventGroup(CPed* ped);
    virtual ~CEventGroup();

    CEvent* Add(CEvent* event, bool bValid = false);
    bool HasScriptCommandOfTaskType(eTaskType taskId);
    bool HasEventOfType(CEvent* event) const;
    bool HasEvent(CEvent* event);
    CEvent* GetHighestPriorityEvent();
    void TickEvents();
    void Remove(CEvent* event);
    void RemoveInvalidEvents(bool bRemoveNonScriptCommandEvents);

    /*!
     * @addr 0x4AB700
     * @brief Re-count the number of events and fill holes in the queue
    */
    void Reorganise();
    void Flush(bool bAvoidFlushingTaskComplexBeInGroup = false);
    CEvent* GetEventOfType(eEventType type) const noexcept;

    // NOTSA
    auto GetEvents()       { return m_events | rng::views::take(m_count); }
    auto GetEvents() const { return m_events | rng::views::take(m_count); }

    auto GetNumEventsInQueue() const { return m_count; }

    //! Helper so events can be directly passed in without having to put them into a variable
    template<std::derived_from<CEvent> T>
    CEvent* Add(T event, bool valid = false) {
        return Add(&event, valid);
    }

private:
    CEventGroup* Constructor(CPed* ped) {
        this->CEventGroup::CEventGroup(ped);
        return this;
    }

public:
    CPed*                                            m_pPed{};
    size_t                                           m_count{};
    std::array<CEvent*, TOTAL_EVENTS_PER_EVENTGROUP> m_events{};
};

VALIDATE_SIZE(CEventGroup, 0x4C);
