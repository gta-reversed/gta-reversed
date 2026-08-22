#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace RHDebugModule {
class HookFilter {
    static constexpr const char*      WILDCARD_CHAR = "*";

    static constexpr std::string_view CATEGORY_PATH_SEPARATOR{ "/" };
    static constexpr std::string_view INVALID_CAT_PATH_CHARS{ "!\"#$%&'()+,-.:;<=>?@[\\]^`{|}~ " }; // Characters that may not be present in the namespace filter

    static constexpr std::string_view HOOK_FILTER_SEP{ "::" };
    static constexpr std::string_view DIGITS{ "0123456789" };
    static constexpr std::string_view INVALID_HOOK_FILTER_CHARS{ "!\"#$%&'()+,-./:;<=>?@[\\]^`{|}~ " }; // Characters that may not be present in the hook filter

public:
    using CategoryPath = std::vector<std::string_view>;

    struct Cutoffs {
        float CategoryInPath{ 0.35f }; //!< Used when filtering by category path to match segments
        float CategoryGlobal{ 0.5f };  //!< Used for by-name global category search
        float ItemGlobal{ 0.5f };      //!< Used for global item search (Eg.: When no category is specified)
        float ItemLocal{ 0.1f };       //!< Used when a category is specified
        float ItemAddress{ 0.1f };     //!< Used for matching hex address to filter string

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Cutoffs, CategoryInPath, CategoryGlobal, ItemGlobal, ItemLocal, ItemAddress)
    };

public:
    HookFilter() = default;
    HookFilter(std::string_view input, bool caseSensitive, Cutoffs cutoffs = {});

    /*!
     * @return The match score of the item, or `nullopt` if it doesn't match the filter
     */
    float MatchItem(
        std::string_view name,
        void*            addressA,
        void*            addressB
    ) const noexcept;

    /*!
     * @return If filter applies to categories
     */
    bool IsFilteringByCategoryPath() const noexcept;

    /*!
    * @brief Should the current filtered namespace be relative to the root namespace.
    * @brief This is the case when the user prepends the namespace tokens with a `/` (NAMESPACE_SEP).
    * @brief Eg.: `/Entity` should only show the `Entity` namespace under `Root` (But not, for example, `Audio/AEVehicleAudioEntity`)
    */
    bool IsRootRelativeCategoryPath() const noexcept { return m_CategoryPathTokens.size() >= 1 && m_CategoryPathTokens.front().empty(); }

    /*!
     * @return If filtering is applied to categories
     */
    bool IsFilteringByCategory() const noexcept { return !m_CategoryPathTokens.empty(); }

    /*!
     * @param cat Category to check
     * @param path Path of the category, including the root `/`, eg.: `/Audio/AEVehicleAudioEntity` => `{ "", "Audio", "AEVehicleAudioEntity" }`
     * @param depth Depth of the category in the namespace hierarchy
     * @return The match score of the category, or `nullopt` if it doesn't match the filter
     */
    float MatchCategoryByPath(const CategoryPath& path, size_t depth) const noexcept;

    /**
     * @brief Checks if the category name matches the filter
     */
    float MatchCategoryByName(std::string_view name) const noexcept;

    /*!
     * @return If there are active filters, if false, nothing is filtered out
     */
    bool HasActiveFilters() const noexcept { return IsFilteringByCategoryPath() || IsHookFilterActive(); }

    /*!
     * @return If hook filter by address is active
     */
    bool IsHookFilterByAddressActive() const noexcept { return m_HookAddressFilter.has_value() && !m_HookAddressFilter->empty(); }

    /*!
     * @return If hook filter by name is active
     */
    bool IsHookFilterByNameActive() const noexcept { return m_HookNameFilter.has_value() && !m_HookNameFilter->empty(); }

    /*!
     * @return If filter applies to items
     */
    bool IsHookFilterActive() const noexcept { return IsHookFilterByAddressActive() || IsHookFilterByNameActive(); }

    /*!
     * @brief Check if hook filter is present, even in case it's present it might be empty.
     * @brief Usually you want to use `IsHookFilterActive` which checks both.
     */
    bool IsHookFilterPresent() const noexcept { return m_HookNameFilter.has_value() || m_HookAddressFilter.has_value(); }

    /*!
     * @return Is filtering done by category name
     */
    bool IsFilteringByCategoryName() const noexcept { return m_CategoryPathTokens.size() == 1; }

    /*!
     * @return If what we're working with is just a simple filter string (So category by name and hook filter are active, no namespaces)
     */
    bool IsGlobalHookSearch() const noexcept { return m_IsGlobalHookSearch; }

    /*!
     * @brief Do string matching using Levenshtein distance against the 
     */

private:
    /*!
     * @brief Simple string matching algorithm.
     * @brief The closer `needle` to the beginning of `haystack` is the higher the score (proportionally to the size of haystack)
     * @brief There's only a match if `needle` is a substring of `haystack` (So no typos allowed)
     * @brief The wildcard character matches all with `1.f`.
     * @param haystack String to search for `needle` in
     * @param needle String to search for in `haystack`
     * @param cutoff Scores lower than this are considered no match (0.f)
     * @param caseSensitive Whether the match should be case sensitive
     * @param emptyHaystackMatchesAll Whether an empty `haystack` should match all `needle` values
     * @return The match score, or 0.f if no match, or score was less than cutoff
     */
    float MatchString(
        std::string_view haystack,
        std::string_view needle,
        float            cutoff = 1.f,
        bool             caseSensitive = false,
        bool             emptyHaystackMatchesAll = false
    ) const noexcept;

    /*!
     * @brief Convert number to hex string for filtering
     */
    std::string GetPtrAsHexString(uintptr_t ptr) const noexcept;

private:
    //! Contains all the tokens on the left side split by `NAMESPACE_SEP` of the input split by `HOOKNAME_SEP`
    //! Eg `m_input` => content:
    //! - `Name/Space/` => `Name`, `Space`, `` (<= empty string)
    //! - `Name/Space/::` => -||- (Same as the above example)
    //! - `/` - `` (empty string) - Indicates root namespace (See `IsRelativeToRootNamespace`)
    //! - `///` - 4 empty strings
    std::vector<std::string> m_CategoryPathTokens{};

    //! We're filtering by hook name and/or address, and no category path is present (So we're searching globally)
    bool m_IsGlobalHookSearch{};

    //! Filter of hook name
    //! Nullopt if no hook filtering is present OR the string entered is a hex number, in which case we treat it as an address filter
    std::optional<std::string> m_HookNameFilter{};

    //! Normalized hook address filter
    //! If user input can't be converted to an address, this remains nullopt
    std::optional<std::string> m_HookAddressFilter{};

    //! Is matching case sensitive or not
    bool m_IsCaseSensitive{};

    //! Filtering cutoffs
    Cutoffs m_Cutoffs{};
};
}; // namespace RHDebugModule
