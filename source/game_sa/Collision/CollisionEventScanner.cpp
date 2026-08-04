#include "StdInc.h"

#include "CollisionEventScanner.h"

// There's an open PR with code for this function, but it is not merged yet

void CCollisionEventScanner::ScanForCollisionEvents(CPed* victim, CEventGroup* eventGroup)
{
    plugin::CallMethod<0x604500, CCollisionEventScanner*, CPed*, CEventGroup*>(this, victim, eventGroup);
}
