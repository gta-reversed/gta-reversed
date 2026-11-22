#pragma once

#include <optional>

#include <boost/static_string.hpp>

#include <reversiblehooks/HookCategory.h>
#include <reversiblehooks/HookCategoryItem.h>

#include <ListItem_c.h>
#include <List_c.h>

namespace RHDebugModule {
using HookState     = ReversibleHooks::HookCategoryItem::HookState;
using CommonState   = std::optional<HookState>;

inline bool IsMatchingScoreOrNone(const std::optional<float>& score, float cutoff = 0.f) {
    return !score.has_value() || *score > cutoff;
}

struct RListCategoryItem : ListItem_c<RListCategoryItem> {
    std::optional<float>                               FilterScore{};  //!< Filter score of this item
    std::shared_ptr<ReversibleHooks::HookCategoryItem> Ptr{};          //!< Ptr to original item
    boost::static_string<256>                          DisplayTitle{}; //!< Rendered title of this item (Including any extra stuff like scores, etc if enabled)
};

struct RListCategory : ListItem_c<RListCategory> {
    std::shared_ptr<ReversibleHooks::HookCategory> Category{};     //!< Ptr to original category
    boost::static_string<256>                      DisplayTitle{}; //!< Rendered title of this item (Including any extra stuff like scores, etc if enabled)

    std::optional<float>                           FilterScore{};            //!< Filter score of this category
    std::optional<float>                           MaxFilterScoreSubCats{};  //!< Only score of categories, not including their items
    std::optional<float>                           MaxFilterScoreOwnItems{}; //!< Max score of our items
    std::optional<float>                           MaxFilterScoreSubItems{}; //!< Max score of sub items
    std::optional<float>                           MaxScoreAllItems{};       //!< Max score of our items and sub items
    std::optional<float>                           MaxFilterScore{};         //!< Max of all of the above

    bool                                           AnyUnhookedItems{};    //!< Any unhooked (sub-)items?
    bool                                           AnyUnhookedOwnItems{}; //!< Any unhooked own items?

    bool                                           AnyUnlockedItems{};    //!< Any unlocked (sub-)items?
    bool                                           AnyUnlockedOwnItems{}; //!< Any unlocked own items?

    std::optional<HookState>                       LastSetAllItemsState{};

    CommonState                                    CommonStateAllItems{}; //!< `nullopt` if there's no common state (There should be no categories with no items/sub-categories in this list)
    CommonState                                    CommonStateOwnItems{}; //!< `nullopt` if there's no common state, or no items
    CommonState                                    CommonStateSubItems{}; //!< `nullopt` if there's no common state, or no sub-categories

    TList_c<RListCategoryItem>                     Items{};

    TList_c<RListCategory>                         Categories{};
    size_t                                         NumCategoriesIgnored{}; //!< Number of categories ignored when then list was constructed because they were empty (no items and no sub-categories)
};
};
