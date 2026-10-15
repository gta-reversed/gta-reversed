#pragma once

#include <format>
#include <optional>

/**
 * @brief Custom formatter for std::optional<T> to handle formatting of optional values.
 */
template <typename T>
struct std::formatter<std::optional<T>> : std::formatter<T> {
    using std::formatter<T>::parse; // Inherit the parse method from the underlying type T to handle specifiers like {:x} or {:.2f}
    auto format(const std::optional<T>& opt, std::format_context& ctx) const {
        if (opt.has_value()) {
            return std::formatter<T>::format(*opt, ctx); // Forward the value and formatting context to the base formatter
        }
        return std::format_to(ctx.out(), "nullopt"); // Fallback string if the optional is empty
    }
};

