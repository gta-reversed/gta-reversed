#pragma once

#include <optional>

#include "HookFilter.h"
#include "RListDefs.h"

namespace RHDebugModule {
class RListFilterer {
public:
    RListFilterer(HookFilter filter) :
        m_Filter{ std::move(filter) } {
    }

    void Process(RListCategory& root) const noexcept;

private:
    float CalculateCategoryScoresByName(RListCategory& cat) const noexcept;

    float CalculateCategoryScoresByPath(RListCategory& cat) const noexcept;
    float CalculateCategoryScoresByPath(RListCategory& cat, const RListCategory* parent, HookFilter::CategoryPath& ns, size_t depth) const noexcept;
    void  SetCategoryUnfilteredCategoryScores(RListCategory& cat) const noexcept;

    std::optional<float> CalculateCategoryItemsScore(RListCategory& cat, bool onlyIfCategoryHasScore) const noexcept;
    void                 SetCategoryUnfilteredItemsScore(RListCategory& from) const noexcept;

    void CalculateCategoryMaxScores(RListCategory& cat) const noexcept;

private:
    HookFilter m_Filter;
};
}; // namespace RHDebugModule
