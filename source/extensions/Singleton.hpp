#pragma once

namespace notsa {
/*!
 * @brief Simple (not thread safe) singleton class. Instance created on first call to `GetInstance()` or manually by `CreateInstance()`
 * @tparam T Type of the singleton class. Must have a default constructor.
 * @note Must manually call `DestroyInstance()` before program exit to avoid memory leaks!
 */
template<typename T>
class Singleton {
public:
    Singleton() = default;
    ~Singleton() = default;

    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;

    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;


    //! Get current singleton instance (Create it if none)
    static T& GetInstance() {
        if (!HasInstance()) {
            CreateInstance();
        }
        return *s_Instance;
    }

    //! Destroy current instance and create new
    static void RecrateInstance() {
        DestroyInstance();
        CreateInstance();
    }

    //! Create the singleton instance now
    static void CreateInstance() {
        assert(!s_Instance);
        s_Instance = new T{};
    }

    //! Detroy current instance
    static void DestroyInstance() {
        delete std::exchange(s_Instance, nullptr);
    }

    //! Check if singleton instance exists
    static bool HasInstance() {
        return s_Instance != nullptr;
    }

private:
    static inline T* s_Instance{};
};
}; // namespace notsa
