#pragma once

// We can't include this header, because it creates a circular dependency with it.
// Wherever this is included has to include the entity they want to use the `Ref` of anyways...
#include <Entity.h>

namespace notsa {
//! Wrapper for entity references, avoids manual usage of `CleanupOldRef` and `RegisterRef`
template<typename T = CEntity>
struct EntityRef {
    // TODO/NOTE: Maybe make this explicit?
    EntityRef(T* ptr = nullptr) noexcept :
        m_Ptr{ ptr }
    {
        if (m_Ptr) {
            m_Ptr->RegisterReference(reinterpret_cast<CEntity**>(&m_Ptr));
        }
    }

    // Need to define this, as compiler generated one would just copy the pointer
    EntityRef(const EntityRef<T>& o) noexcept :
        EntityRef{ o.m_Ptr }
    {
    }

    // Can't really move this as the address registered is tied to `this`
    EntityRef(EntityRef<T>&&) = delete;

    ~EntityRef() {
        if (m_Ptr) {
            m_Ptr->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_Ptr));
        }
    }

    // Must have this defined, because compiler-generated op would just copy the pointers
    EntityRef<T>& operator=(const EntityRef<T>& o) noexcept {
        *this = o.m_Ptr;
        return *this;
    }

    // Assignments should be done without a (possibly) temporary `EntityRef` instance
    // (This way we avoid extra calls to `CleanUpOld/RegisterReference`...)
    EntityRef<T>& operator=(T* ptr) noexcept {
        if (m_Ptr) {
            m_Ptr->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_Ptr));
        }
        if (m_Ptr = ptr) {
            m_Ptr->RegisterReference(reinterpret_cast<CEntity**>(&m_Ptr));
        }
        return *this;
    }

    // Can't really move this as the address registered is tied to `this`
    EntityRef<T>& operator=(EntityRef<T>&&) = delete;

    decltype(auto) Get(this auto&& self) noexcept { return self.m_Ptr; }

    operator T*()   const noexcept { return m_Ptr;  }
    operator T*()         noexcept { return m_Ptr;  }

    T* operator->() const noexcept { return m_Ptr;  }
    T* operator->()       noexcept { return m_Ptr;  }

    T& operator*()  const noexcept { return *m_Ptr; }
    T& operator*()        noexcept { return *m_Ptr; }

    T* operator&()  const noexcept { return m_Ptr; }
    T* operator&()        noexcept { return m_Ptr; }

    //auto operator==(const EntityRef<T>& o) const noexcept { return m_Ptr == o.m_Ptr; }
    //auto operator!=(const EntityRef<T>& o) const noexcept { return !(*this == o); }
    //
    //auto operator==(const T* o) const noexcept { return m_Ptr == o; }
    //auto operator!=(const T* o) const noexcept { return !(*this == o); }

private:
    T* m_Ptr;
};
VALIDATE_SIZE(EntityRef<int>, sizeof(int*));
}; // namespace notsa
