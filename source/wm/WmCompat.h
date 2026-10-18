#pragma once

#include <cstdint>

// Minimal self-contained replacements for the two header-only helpers the original
// wm_sachs plugin used (injector + utf8cpp), so this module pulls in no third-party code.
namespace injector {

    struct addr_proxy {
        uintptr_t v;
        template <typename T>
        operator T() const { return reinterpret_cast<T>(v); }
    };

    struct addr_fn {
        uintptr_t v;
        addr_proxy get() const { return { v }; }
    };

    inline addr_fn raw_ptr(uintptr_t v) { return { v }; }
    inline addr_fn aslr_ptr(uintptr_t v) { return { v }; }

} // namespace injector

namespace utf8 {

    inline uint32_t decode(const char* s) {
        const auto* p = reinterpret_cast<const unsigned char*>(s);
        if (p[0] < 0x80) {
            return p[0];
        }
        if ((p[0] & 0xE0) == 0xC0) {
            return ((p[0] & 0x1Fu) << 6) | (p[1] & 0x3Fu);
        }
        if ((p[0] & 0xF0) == 0xE0) {
            return ((p[0] & 0x0Fu) << 12) | ((p[1] & 0x3Fu) << 6) | (p[2] & 0x3Fu);
        }
        return ((p[0] & 0x07u) << 18) | ((p[1] & 0x3Fu) << 12) | ((p[2] & 0x3Fu) << 6) | (p[3] & 0x3Fu);
    }

    inline int sequence_length(uint32_t c) {
        return c < 0x80 ? 1 : (c < 0x800 ? 2 : (c < 0x10000 ? 3 : 4));
    }

    namespace unchecked {

        inline uint32_t peek_next(const char* s) { return decode(s); }
        inline void next(char*& s) { s += sequence_length(decode(s)); }
        inline void next(const char*& s) { s += sequence_length(decode(s)); }

    } // namespace unchecked

} // namespace utf8
