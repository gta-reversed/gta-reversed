#include "StdInc.h"

#include "RListFilterer.h"

namespace RHDebugModule {
void RListFilterer::Process(RListCategory& root) const noexcept {
    ZoneScoped;

    // NB: Always run category filtering first, because item filtering depends on category scores to determine if it should filter items or not
    if (m_Filter.IsFilteringByCategoryPath()) {
        CalculateCategoryScoresByPath(root);
    } else if (m_Filter.IsFilteringByCategoryName()) {
        CalculateCategoryScoresByName(root);
    } else {
        SetCategoryUnfilteredCategoryScores(root);
    }

    // NB: Always run item filtering after category (See above why)
    if (m_Filter.IsHookFilterActive()) {
        CalculateCategoryItemsScore(root, m_Filter.IsFilteringByCategory());
    } else {
        SetCategoryUnfilteredItemsScore(root);
    }

    // After all this we can calculate the total scores
    CalculateCategoryMaxScores(root);
}

float RListFilterer::CalculateCategoryScoresByName(RListCategory& cat) const noexcept {
    cat.FilterScore = m_Filter.MatchCategoryByName(cat.Category->Name());
    if (cat.Categories.IsEmpty()) {
        cat.MaxFilterScoreSubCats = std::nullopt;
    } else {
        cat.MaxFilterScoreSubCats = 0.f;
        for (auto& sc : cat.Categories) {
            cat.MaxFilterScoreSubCats = std::max(*cat.MaxFilterScoreSubCats, CalculateCategoryScoresByName(sc));
        }
    }
    return std::max(cat.MaxFilterScoreSubCats.value_or(0.f), *cat.FilterScore);
}

float RListFilterer::CalculateCategoryScoresByPath(RListCategory& cat, const RListCategory* parent, HookFilter::CategoryPath& ns, size_t depth) const noexcept {
    cat.FilterScore           = m_Filter.MatchCategoryByPath(ns, depth);
    cat.MaxFilterScoreSubCats = std::nullopt;

    if (!m_Filter.IsRootRelativeCategoryPath() && parent) {
        // Filter just checks the end of the path as an optimization, but
        // but, logically, if our parent has matched, at worst our filter score
        // should be at least as good as our parent's filter score
        cat.FilterScore = std::max(cat.FilterScore, parent->FilterScore);
    }

    for (auto& sc : cat.Categories) {
        ns.push_back(sc.Category->Name());
        cat.MaxFilterScoreSubCats = std::max(cat.MaxFilterScoreSubCats.value_or(0.f), CalculateCategoryScoresByPath(sc, &cat, ns, depth + 1));
        ns.pop_back();
    }

    return std::max(cat.MaxFilterScoreSubCats.value_or(0.f), *cat.FilterScore);
}

float RListFilterer::CalculateCategoryScoresByPath(RListCategory& cat) const noexcept {
    HookFilter::CategoryPath ns = { cat.Category->Name() };
    return CalculateCategoryScoresByPath(cat, nullptr, ns, 0);
}

void RListFilterer::SetCategoryUnfilteredCategoryScores(RListCategory& cat) const noexcept {
    cat.FilterScore           = std::nullopt;
    cat.MaxFilterScoreSubCats = std::nullopt;
    for (auto& sc : cat.Categories) {
        SetCategoryUnfilteredCategoryScores(sc);
    }
}

std::optional<float> RListFilterer::CalculateCategoryItemsScore(RListCategory& cat, bool onlyIfCategoryHasScore) const noexcept {
    // NB: For this to work properly category scores must first be calculated!
    if (onlyIfCategoryHasScore != (std::max(cat.MaxFilterScoreSubCats, cat.FilterScore).value_or(0.f) > 0.f)) {
        SetCategoryUnfilteredItemsScore(cat);
        return std::nullopt;
    }

    if (!cat.Items.IsEmpty()) {
        cat.MaxFilterScoreOwnItems = 0.f;
        for (auto& i : cat.Items) {
            const auto score = m_Filter.MatchItem(
                i.Ptr->GetName(),
                i.Ptr->GetHookAddressGTA(),
                i.Ptr->GetHookAddressOur()
            );
            i.FilterScore = score;
            cat.MaxFilterScoreOwnItems = std::max(*cat.MaxFilterScoreOwnItems, score);
        }
    } else {
        cat.MaxFilterScoreOwnItems = std::nullopt;
    }

    cat.MaxFilterScoreSubItems = std::nullopt;
    for (auto& sc : cat.Categories) {
        cat.MaxFilterScoreSubItems = std::max(cat.MaxFilterScoreSubItems, CalculateCategoryItemsScore(sc, onlyIfCategoryHasScore));
    }

    cat.MaxScoreAllItems = std::max(cat.MaxFilterScoreSubItems, cat.MaxFilterScoreOwnItems);

    return cat.MaxScoreAllItems;
}

void RListFilterer::SetCategoryUnfilteredItemsScore(RListCategory& cat) const noexcept {
    cat.MaxFilterScoreOwnItems = std::nullopt;
    cat.MaxFilterScoreSubItems = std::nullopt;
    cat.MaxScoreAllItems       = std::nullopt;
    for (auto& i : cat.Items) {
        i.FilterScore = std::nullopt;
    }
    for (auto& sc : cat.Categories) {
        SetCategoryUnfilteredItemsScore(sc);
    }
}
void RListFilterer::CalculateCategoryMaxScores(RListCategory& cat) const noexcept {
    cat.MaxFilterScore = std::max({cat.MaxFilterScoreSubCats, cat.MaxScoreAllItems, cat.FilterScore});
    for (auto& sc : cat.Categories) {
        CalculateCategoryMaxScores(sc);
    }
}
}; // namespace RHDebugModule
