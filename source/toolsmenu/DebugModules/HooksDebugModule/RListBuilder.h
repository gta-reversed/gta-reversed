#pragma once

#include <reversiblehooks/HookCategory.h>
#include <Pool.h>

#include "RListDefs.h"

namespace RHDebugModule {
class RListBuilder {
public:
    struct Options {
        bool DisplayTitleWithFilterScores{};
        bool DisplayTitleWithItemAddress{};

        auto operator<=>(const Options&) const = default;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Options, DisplayTitleWithFilterScores, DisplayTitleWithItemAddress)
    };

public:
    RListBuilder(
        size_t maxItems      = 16'384,
        size_t maxCategories = 4'096
    );

    /*!
     * @brief Construct render list starting at the given `cat` (Should be the `RootCategory`)
     * @note Should be ran on the main thread!
     * @return Pointer to the first cateogry, owned by this class
     */
    RListCategory* ConstructList(std::shared_ptr<ReversibleHooks::HookCategory> cat, const Options& opts) noexcept;

    /*!
     * @brief Recursively update all categories
     */
    void UpdateList(RListCategory& cat, const Options& opts) const noexcept {
        UpdateCategory(cat, opts);
        for (auto& sc : cat.Categories) {
            UpdateList(sc, opts);
        }
    }

    /*!
     * @brief Re-calculate the common state of the category and its items
     * @note Should be ran on the main thread!
     * @param cat 
     * @return true if any of the common states or flags have changed, false otherwise
     */
    bool UpdateCategory(RListCategory& cat, const Options& opts) const noexcept;

private:
    CPool<RListCategoryItem> m_PoolItem;
    CPool<RListCategory>     m_PoolCategory;
};
}; // namespace RHDebugModule
