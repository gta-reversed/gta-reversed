#pragma once

#include "Vector.h"
#include "Route.hpp"

class CPatrolRoute {
private:
    constexpr static size_t MaxPoints = 8;

public:
    struct PointInfo {
        char AnimationName[24]{};
        char AnimationFile[16]{};
    };

public:
    /*!
     * @addr 0x41B810
     * @brief Allocate a route object from the point-route pool.
     * @param size Requested allocation size.
     * @return Allocated route object memory.
     */
    static void* operator new(uint32 size);

    /*!
     * @addr 0x41B820
     * @brief Return a route object to the point-route pool.
     * @param ptr Pointer to memory previously allocated by operator new.
     * @param sz Allocation size (unused by implementation).
     */
    static void operator delete(void* ptr, size_t sz);

public:
    void Extend(const CVector& position, const PointInfo& info);
    void Flush();
    void Reverse();

    auto             GetCount() const { return m_Count; }
    const CVector&   GetPosition(size_t i) const { return m_Positions[i]; }
    const PointInfo& GetInfo(size_t i) const { return m_Infos[i]; }

public:
    static void InjectHooks();

private:
    size_t    m_Count{};
    PointInfo m_Infos[MaxPoints]{};
    CVector   m_Positions[MaxPoints]{};
};
VALIDATE_SIZE(CPatrolRoute, 0x1A4);
