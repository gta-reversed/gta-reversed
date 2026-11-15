#include "StdInc.h"
#include "PatrolRoute.h"
#include "Pools/Pools.h"

/*!
* @addr 0x41B810
* @brief Allocate a route object from the point-route pool.
* @param size Requested allocation size.
* @return Allocated route object memory.
*/
void* CPatrolRoute::operator new(size_t sz) {
    return GetPatrolRoutePool()->New();
}

/*!
* @addr 0x41B820
* @brief Return a route object to the point-route pool.
* @param ptr Pointer to memory previously allocated by operator new.
* @param sz Allocation size (unused by implementation).
*/
void CPatrolRoute::operator delete(void* ptr, size_t sz) {
    GetPatrolRoutePool()->Delete(static_cast<CPatrolRoute*>(ptr));
}
