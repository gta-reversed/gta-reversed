#pragma once

#include "KeyGen.h"
#include <string>
#include <string_view>

namespace notsa {
namespace details {
// Partially based on code from https://en.cppreference.com/w/cpp/string/basic_string
struct ci_char_traits : public std::char_traits<char> {
    static char to_upper(char ch) { return std::toupper((unsigned char)ch); }

    static bool eq(char c1, char c2) { return to_upper(c1) == to_upper(c2); }
    static bool lt(char c1, char c2) { return to_upper(c1) < to_upper(c2); }
    static const char* find(const char* s, std::size_t n, char a) {
        const auto ua = to_upper(a);
        for (; n; n--, s++) {
            if (to_upper(*s) == ua) {
                return s;
            }
        }
        return nullptr;
    }
    static int compare(const char* s1, const char* s2, std::size_t n) {
#ifdef WIN32
        return _strnicmp(s1, s2, n);
#else /* *nix */
        return strncasecmp(s1, s2, n)
#endif
    }
}; 
}; // namespace details
using ci_string = std::basic_string<char, details::ci_char_traits>;
using ci_string_view = std::basic_string_view<char, details::ci_char_traits>;
}; // namespace notsa

namespace std {
template<>
struct hash<notsa::ci_string> {
    std::size_t operator()(const notsa::ci_string& s) const noexcept {
        return CKeyGen::GetUppercaseKey(s.data(), s.data() + s.size());
    }
};

template<>
struct hash<notsa::ci_string_view> {
    std::size_t operator()(const notsa::ci_string_view& s) const noexcept {
        return CKeyGen::GetUppercaseKey(s.data(), s.data() + s.size());
    }
};
}
