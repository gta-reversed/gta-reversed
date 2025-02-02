#pragma once

#include <Entity.h>
namespace notsa {
//! Wrapper for entity references, avoids manual usage of `CleanupOldRef` and `RegisterRef`
template<typename T>
struct EntityRef {
    EntityRef(T* ptr = nullptr) noexcept :
        m_Ptr{ ptr }
    {
        if (m_Ptr) {
            m_Ptr->RegisterReference(reinterpret_cast<CEntity**>(&m_Ptr));
        }
    }

    EntityRef(const EntityRef<T>& o) noexcept : // We only define a copy constructor, as this class isn't moveable
        EntityRef{ o.m_Ptr }
    {
    }

    ~EntityRef() {
        if (m_Ptr) {
            m_Ptr->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_Ptr));
        }
    }

    // Assignments should be done without a (possibly) temporary `EntityRef` instance
    // (This way we avoid extra calls to `CleanUpOld/RegisterReference`...)
    EntityRef<T>& operator=(T* ptr) noexcept {
        if (m_Ptr) {
            m_Ptr->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_Ptr));
        }
        m_Ptr = ptr;
        if (m_Ptr) {
            m_Ptr->RegisterReference(reinterpret_cast<CEntity**>(&m_Ptr));
        }
        return *this;
    }

    decltype(auto) Get(this auto&& self) noexcept { return self.m_Ptr; }

    operator T*()   const noexcept { return m_Ptr;  }
    operator T*()         noexcept { return m_Ptr;  }

    T* operator->() const noexcept { return m_Ptr;  }
    T* operator->()       noexcept { return m_Ptr;  }

    T& operator*()  const noexcept { return *m_Ptr; }
    T& operator*()        noexcept { return *m_Ptr; }

    T* operator&()  const noexcept { return m_Ptr; }
    T* operator&()        noexcept { return m_Ptr; }

private:
    T* m_Ptr;
};
VALIDATE_SIZE(EntityRef<int>, sizeof(int*));
}; // namespace notsa
