#include "StdInc.h"

#include "EventSource.h"

int32 CEventSource::ComputeEventSourceType(const CEvent& event, const CPed& ped) {
    return plugin::CallAndReturn<int32, 0x4ABAC0, const CEvent&, const CPed&>(event, ped);
}

void CEventSource::InjectHooks() {
    RH_ScopedClass(CEventSource);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(ComputeEventSourceType, 0x4ABAC0, { .Reversed = false });
}
