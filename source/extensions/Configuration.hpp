#pragma once
#include <string>
#include <optional>
#include <unordered_map>
#include <fstream>

#include "app_debug.h"

static inline class {
    using IniContent = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;
    IniContent m_content{};

public:
    template <typename Ret = std::string>
    std::optional<Ret> GetIniValue(const std::string& section, const std::string& key) {
        if (m_content.empty() || !m_content.contains(section) || !m_content[section].contains(key))
            return {};

        const auto& str = m_content[section][key];
        if constexpr (std::is_same_v<Ret, std::string>) {
            return str;
        } else if constexpr (std::is_same_v<Ret, bool>) {
            return str == "true" || str == "TRUE" || str == "1";
        } else if constexpr (notsa::is_standard_integer<Ret> || std::is_floating_point_v<Ret>) {
            Ret rt{};

            auto [_, ec] = std::from_chars(str.data(), str.data() + str.size(), rt);
            if (ec != std::errc{})
                return {};

            return rt;
        } else {
            return {};
        }
    }

    void Load(const std::string& fileName) {
        std::wifstream in(fileName);
        assert(!in.bad());

        ParseIniFile(in, m_content);
    }

private:
    static void ParseIniFile(std::wifstream& file, IniContent& ini) {
        std::string sectionName{};
        std::wstring line{};
        while (std::getline(file, line)) {
            const auto StripText = [](std::wstring& s) {
                s.erase(0, s.find_first_not_of(L" \t\n\r\f\v"));
                s.erase(s.find_last_not_of(L" \t\n\r\f\v") + 1);
            };

            if (const auto cf = line.find(';'); cf != std::wstring::npos) {
                line.erase(cf);
            }

            StripText(line);

            if (line.empty())
                continue;

            if (line.front() == L'[' && line.back() == L']') {
                sectionName = UnicodeToUTF8(line.substr(1, line.size() - 2));
                continue;
            }

            if (const auto ef = line.find(L'='); ef != std::wstring::npos) {
                auto wkey = line.substr(0, ef);
                auto wvalue = line.substr(ef + 1);

                StripText(wkey);
                StripText(wvalue);

                ini[sectionName][UnicodeToUTF8(wkey)] = UnicodeToUTF8(wvalue);
                continue;
            }

            NOTSA_LOG_DEBUG("err: something wrong!");
            break;
        }
    }
} g_ConfigurationMgr;

namespace notsa::detail {
template<typename T>
struct unwrap_optional {
    using type = T;
};

template<typename T>
struct unwrap_optional<std::optional<T>> {
    using type = T;
};

template<typename T>
using unwrap_optional_t = unwrap_optional<T>::type;
};

#define INI_CONFIG_SECTION(name) \
    static constexpr auto IniSectionName = name

#define GET_INI_CONFIG_VALUE(key, _default) \
	g_ConfigurationMgr.GetIniValue<decltype(_default)>(IniSectionName, key).value_or(_default)

#define STORE_INI_CONFIG_VALUE(key_var, _default) \
	key_var = g_ConfigurationMgr.GetIniValue<decltype(key_var)>(IniSectionName, #key_var).value_or(_default)

#define GET_INI_CONFIG_VALUE_OPT(key, T) \
    g_ConfigurationMgr.GetIniValue<T>(IniSectionName, key)

#define STORE_INI_CONFIG_VALUE_OPT(key_var) \
    key_var = g_ConfigurationMgr.GetIniValue<notsa::detail::unwrap_optional_t<decltype(key_var)>>(IniSectionName, #key_var)
