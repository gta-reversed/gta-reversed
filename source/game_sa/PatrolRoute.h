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
    static void InjectHooks() {
        RH_ScopedClass(CPatrolRoute);
        RH_ScopedCategoryGlobal();

        RH_ScopedInstall(operator new, 0x41B810);
        RH_ScopedInstall(operator delete, 0x41B820);
    }

    void Extend(const CVector& position, const PointInfo& info) {
        if (m_Count >= MaxPoints) {
            return;
        }
        m_Positions[m_Count] = position;
        m_Infos[m_Count]     = info;
        ++m_Count;
    }

    void Flush() {
        m_Count = 0;
    }

private:
    CVector   m_Positions[MaxPoints]{};
    PointInfo m_Infos[MaxPoints]{};
    size_t    m_Count{};
};
