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

void CPatrolRoute::Extend(const CVector& position, const PointInfo& info) {
    if (m_Count >= MaxPoints) {
        return;
    }
    m_Positions[m_Count] = position;
    m_Infos[m_Count]     = info;
    ++m_Count;
}

void CPatrolRoute::Flush() {
    m_Count = 0;
}

// 0x66D550
void CPatrolRoute::Reverse() {
    // NOTE: The original swaps the names with `strcpy`, so only the bytes after the terminators can differ
    std::reverse(m_Infos, m_Infos + m_Count);
    std::reverse(m_Positions, m_Positions + m_Count);
}

void CPatrolRoute::InjectHooks() {
    RH_ScopedClass(CPatrolRoute);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(operator new, 0x41B810);
    RH_ScopedInstall(operator delete, 0x41B820);
    RH_ScopedInstall(Reverse, 0x66D550);
}
