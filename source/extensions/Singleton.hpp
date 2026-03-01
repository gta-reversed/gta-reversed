#pragma once

#include <memory>
namespace notsa {
//! Simple (not thread safe) singleton class. Instance created on first call to `GetSingleton()` or manually by `CreateInstance()`
template<typename T>
class Singleton {
    static inline std::unique_ptr<T> s_instance{};
public:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    //! Get current singleton instance (Create it if none)
    static T& GetSingleton() {
        if (!s_instance) {
            CreateInstance();
        }
        return *s_instance;
    }

    //! Destroy current instance and create new
    static void ResetSingleton() {
        DestroySingleton();
        CreateInstance();
    }

    //! Create the singleton instance now
    static void CreateInstance() {
        assert(!s_instance);
        s_instance = std::make_unique<T>();
    }

    static void DestroySingleton() {
        s_instance.reset();
    }

    static bool HasInstance() {
        return s_instance != nullptr;
    }
};
}; // namespace notsa
