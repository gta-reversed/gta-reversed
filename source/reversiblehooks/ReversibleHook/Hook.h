#pragma once

#include <Base.h>
#include <nlohmann/json.hpp>
#include <string>

#include "Enums/HookMode.h"
#include "Enums/HookType.h"

namespace ReversibleHooks {
namespace ReversibleHook {
class Hook {
public:
    Hook(std::string name) :
        m_Name{ std::move(name) }
    {
    }

    virtual ~Hook() = default;

    virtual HookType Type() const noexcept = 0;
    virtual HookMode Mode() const noexcept = 0;
    virtual void     Check()               = 0;
    virtual void     Serialize(json& j) const { to_json(j, *this); }

    const auto& Name() const noexcept { return m_Name; }

public:
    friend void to_json(json& j, const Hook& hook) {
        j["Name"] = hook.Name();
        j["Type"] = hook.Type();
        j["Mode"] = hook.Mode();
    }

protected:
    const std::string m_Name{}; //!< Name of hook, ex.: `Add` (Referring to CEntity::Add)
};
}; // namespace ReversibleHook
}; // namespace ReversibleHooks

