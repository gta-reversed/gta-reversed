#include "StdInc.h"
#include "Base.h"
#include <iterator>
#include "HookFilter.h"

namespace RHDebugModule {
HookFilter::HookFilter(std::string_view untrimmedInput, bool caseSensitive, Cutoffs cutoffs) :
    m_IsCaseSensitive{ caseSensitive },
    m_Cutoffs{ cutoffs }
{
    const auto input = notsa::trim_string(untrimmedInput);

    if (input.empty()) {
        return;
    }

    const auto TrySetAddressFilter = [this](std::string_view str) {
        if (str.starts_with("0x") || str.starts_with("0X")) {
            str = str.substr(2);
        }
        const char* end;
        const auto  address = notsa::try_ston<uintptr>(str, 16, &end);
        const auto  matched = address.has_value() && end == str.data() + str.size();
        if (matched) {
            m_HookAddressFilter = GetPtrAsHexString(*address);
        }
        return matched;
    };

    // If the first character is a digit, we assume the user wants to filter by address
    // as namespace or function names can't start with a digit
    if (DIGITS.contains(input.front())) {
        TrySetAddressFilter(input);
    } else {
        const auto namespaceSepPos = input.rfind(HOOK_FILTER_SEP);
        const auto hasNamespaceSep = namespaceSepPos != std::string_view::npos;

        // First half (if any), or the whole input is the category (path) filter (if there's a `/`)
        // Eg.:
        // - `Category/` - By path
        // - `Category/Subcategory`  - By path
        // - `Category/Subcategory::Function` - By path
        // - `Category::Function` - Simple by-name category filter
        // What isn't a category filter:
        // - `::Function` - No category filter
        // - `Function` - No category filter
        const auto categoryFilterStr = hasNamespaceSep
            ? input.substr(0, namespaceSepPos) // No need to trim here, it'll be trimmed when emplacing
            : input;
        m_IsGlobalHookSearch = hasNamespaceSep
            ? categoryFilterStr == WILDCARD_CHAR                    // Eg.: `*::` would still match search globally
            : !categoryFilterStr.contains(CATEGORY_PATH_SEPARATOR); // Eg.: `Function`
        if (!m_IsGlobalHookSearch && !categoryFilterStr.empty() && categoryFilterStr.find_first_of(INVALID_CAT_PATH_CHARS) == std::string_view::npos) {
            for (auto t : SplitStringView(categoryFilterStr, CATEGORY_PATH_SEPARATOR)) {
                m_CategoryPathTokens.emplace_back(notsa::trim_string(t));
            }
            if (m_CategoryPathTokens.size() == 1 && m_CategoryPathTokens.back().empty()) {
                m_CategoryPathTokens.clear(); // An empty string would match all categories, so there's no point in keeping it
            }
        }

        // Whole string (if no namespace separator or string has no `/`) or second half is the whole input is the hook filter
        // Eg.:
        // - `Function`
        // - `::Function`
        // - `Category::Function`
        // - `category/subcategory::` (Hook filter present, but not active)
        // - `category/subcategory::Function` (Hook filter present and active)
        // What isn't a hook filter:
        // - `category/`
        // - `category/subcategory`
        const auto hookFilterStr = hasNamespaceSep
            ? notsa::trim_string(input.substr(namespaceSepPos + HOOK_FILTER_SEP.size()))
            : input;
        if (!hookFilterStr.empty() && hookFilterStr.find_first_of(INVALID_HOOK_FILTER_CHARS) == std::string_view::npos) {
            if (!TrySetAddressFilter(hookFilterStr)) {
                m_HookNameFilter = hookFilterStr;
            }
        }       
    }
}

float HookFilter::MatchItem(
    std::string_view name,
    void*            addressA,
    void*            addressB
) const noexcept {
    float max = 0.0;
    const auto DoTest = [&](std::string_view haystack, std::string_view needle, float cutoff) {
        max = std::max(max, MatchString(haystack, needle, cutoff, m_IsCaseSensitive));
    };
    if (IsHookFilterByNameActive()) {
        DoTest(name, *m_HookNameFilter, m_IsGlobalHookSearch ? m_Cutoffs.ItemGlobal : m_Cutoffs.ItemLocal);
    }
    if (IsHookFilterByAddressActive()) {
        for (auto address : { addressA, addressB }) {
            if (!address) {
                continue;
            }
            //char buf[64]{ "0x" };
            //const auto [end, ec] = std::to_chars(buf + 2, buf + sizeof(buf) - 2, (uintptr_t)(address), 16);
            //if (ec != std::errc{}) {
            //    continue;
            //}
            DoTest(GetPtrAsHexString(std::bit_cast<uintptr_t>(address)), *m_HookAddressFilter, m_Cutoffs.ItemAddress);
        }
    }
    return max;
}

bool HookFilter::IsFilteringByCategoryPath() const noexcept {
    if (m_CategoryPathTokens.empty()) {
        return false; // No tokens
    }
    if (IsFilteringByCategoryName()) {
        return false; // Only filtering by category name, not path
    }
    return true;
}

float HookFilter::MatchCategoryByPath(const CategoryPath& path, size_t depth) const noexcept {
    assert(path.size() > depth);

    if (m_CategoryPathTokens.size() > depth + 1) {
        return 0.f; // Can't match whole, so just stop now
    }

    const auto MatchPath = [&] (size_t off = 0) {
        float total = 0.f;
        for (size_t i = 0; i < m_CategoryPathTokens.size(); i++) {
            const auto match = MatchString(path[off + i], m_CategoryPathTokens[i], m_Cutoffs.CategoryInPath, m_IsCaseSensitive, true);
            if (match <= 0.f) {
                return 0.f;
            }
            total += match;
        }
        return total;
    };

    if (IsRootRelativeCategoryPath()) {
        return MatchPath(0); // Compare whole from the begining of `path`, whole filter must match (it's ok to have excess tokens in the input path)
    } else { // This should match at the end
        if (m_CategoryPathTokens.size() > depth + 1) {
            return 0.f; // Can't match whole, so just stop now
        }
        return MatchPath(1 + depth - m_CategoryPathTokens.size()); // Compare `m_CategoryPathTokens` with the end of `path`, it's ok if we have exess tokens in the input `path`
    }
}

float HookFilter::MatchCategoryByName(std::string_view name) const noexcept {
    assert(m_CategoryPathTokens.size() == 1);
    return MatchString(name, m_CategoryPathTokens.front(), m_Cutoffs.CategoryGlobal, m_IsCaseSensitive);
}

float HookFilter::MatchString(std::string_view haystack, std::string_view needle, float cutoff, bool caseSensitive, bool emptyNeedleMatchesAll) const noexcept {
    if (needle.empty()) {
        return emptyNeedleMatchesAll ? 1.f : 0.f; // This way if user didn't type anything stuff will still show up
    } else if (needle == WILDCARD_CHAR) {
        return 1.f;
    }
    if (haystack.empty()) {
        return 0.f;
    }
    const auto CalculateScore = [&] (size_t pos) {
        if (pos == std::string_view::npos) {
            return 0.f;
        }
        if (haystack.size() == needle.size()) {
            return 1.f;
        }
        const auto score = (float)(needle.size()) / (float)(haystack.size() + pos);
        return score >= cutoff ? score : 0.f;
        };
    if (caseSensitive) {
        return CalculateScore(haystack.find(needle));
    }
    const auto ToUpper = [](auto&& c) {
        return (char)(std::toupper((unsigned char)(c)));
        };
    const auto range = rng::search(haystack, needle, {}, ToUpper, ToUpper);
    if (range.empty()) {
        return 0.f;
    }
    return CalculateScore(rng::distance(haystack.begin(), range.begin()));
}

std::string HookFilter::GetPtrAsHexString(uintptr_t ptr) const noexcept {
    // `to_chars` doesn't use the `0x` prefix, so we don't need to account for it here
    // by limiting the size of the string we avoid heap allocation
    // because std::string has SSO of around 10-15 chars on x32
    constexpr auto MAX_PTR_HEX_SIZE = sizeof(uintptr_t) * 2;

    std::string hex;
    hex.resize(MAX_PTR_HEX_SIZE);
    const auto [end, ec] = std::to_chars(hex.data(), hex.data() + hex.size(), ptr, 16);
    if (ec != std::errc{}) {
        return {};
    }
    hex.resize(rng::distance(hex.data(), end));
    return hex;
}
}; // namespace RHDebugModule
