#pragma once

#include <string>

 namespace ReversibleHooks{

// TODO: Maybe this should go into `detail`
constexpr uint32 JUMP_OPCODE = 0xE9;
constexpr uint32 NOP_OPCODE  = 0x90;

constexpr uint32 x86JMPSize = 5U;
constexpr auto GetJMPLocation(uint32 dwFrom, uint32 dwTo) { return dwTo - dwFrom - x86JMPSize; }
constexpr auto GetFunctionLocationFromJMP(uint32 dwJmpLoc, uint32 dwJmpOffset) { return dwJmpOffset + dwJmpLoc + x86JMPSize; }

namespace ReversibleHook {
struct Base {
    enum class HookType { // Sadly can't use `Type` alone as it's some function..
        Simple,
        Virtual,
        ScriptCommand
    };

    Base(std::string fnName, HookType type, bool reversed = true) :
        m_Name{std::move(fnName)},
        m_Type{type},
        m_IsReversed{reversed}
    {
    }

    virtual ~Base() = default;

    virtual void Switch() = 0;
    virtual void Check() = 0;

    /*!
    * @brief Hook/unhook
    * 
    * @param hooked If this hook should be installed/uninstalled (true/false)
    *
    * @returns If the state has changed
    */
    bool State(bool hooked) {
        if (hooked == m_IsHooked) {
            return false; // No change
        }
        if (m_IsLocked) {
            return false; // Can't change
        }
        Switch();
        return true;
    }

    void LockState(bool locked) {
        m_IsLocked = locked;
    }

    /// Symbol in ImGui (On the left side of the checkbox)
    virtual const char* Symbol() const = 0;

    const auto& Name()     const { return m_Name; }
    const auto  Type()     const { return m_Type; }
    const auto  Hooked()   const { return m_IsHooked; }
    const auto  Locked()   const { return m_IsLocked; }
    const auto  Reversed() const { return m_IsReversed; }

public:
    // ImGui stuff
    bool m_isVisible{true};

protected:
    bool        m_IsReversed{}; // Is the function reversed - Only used when dumping hooks to csv
    bool        m_IsHooked{};   // Is hook installed (true => yes, gta calls are redirected to our code, false => our code is redirected to the gta function)
    bool        m_IsLocked{};   // Is hook locked, i.e.: the hooked state can't be changed.
    std::string m_Name{};       // Name of function, eg.: `Add` (Referring to CEntity::Add)
    HookType    m_Type{};
};
};
};
