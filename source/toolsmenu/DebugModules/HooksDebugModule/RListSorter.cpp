#include "StdInc.h"

#include "RListSorter.hpp"
#include <concepts>

namespace RHDebugModule {
void RListSorter::Process(RListCategory& cat) noexcept {
    ZoneScoped;

    cat.Items.Sort([](const RListCategoryItem& a, const RListCategoryItem& b) {
        if (a.FilterScore.has_value() || b.FilterScore.has_value()) {
            return a.FilterScore < b.FilterScore;
        }
        return a.Ptr->GetName() < b.Ptr->GetName();
    });

    cat.Categories.Sort([](const RListCategory& a, const RListCategory& b) {
        if (a.MaxFilterScore.has_value() && b.MaxFilterScore.has_value()) {
            return a.MaxFilterScore > b.MaxFilterScore;
        }
        return a.Category->Name() > b.Category->Name();
    });

    for (auto& v : cat.Categories) {
        Process(v);
    }  
}
}; // namespace RHDebugModule
