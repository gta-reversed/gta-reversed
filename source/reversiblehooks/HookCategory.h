#pragma once

#include <string>
#include <algorithm>
#include <memory>

#include "ReversibleHooks.h"
#include "HookCategoryItem.h"

namespace rng = std::ranges;

namespace ReversibleHooks {
class HookCategory : public std::enable_shared_from_this<HookCategory> {
public:
    using HookState        = ReversibleHook::TwoWayHookState;
    using CommonItemsState = std::optional<HookState>;

public:
    HookCategory(std::string name, std::shared_ptr<HookCategory> parent) :
        m_Name{ std::move(name) },
        m_Parent{ parent }
    {
    }

    HookCategory(const HookCategory&) = delete;
    HookCategory& operator=(const HookCategory&) = delete;

    const auto& Name()          const { return m_Name; }
                                
    const auto& SubCategories() const { return m_SubCategories; }
    auto&       SubCategories()       { return m_SubCategories; }
                                
    const auto& Items()         const { return m_Items; }
    auto&       Items()               { return m_Items; }
                                
    auto Parent()               const { return m_Parent; }

    bool IsEmpty() const noexcept { return m_Items.empty() && m_SubCategories.empty(); }

    //! Add one item to this category (Deal with possible state change)
    void AddItem(std::shared_ptr<HookCategoryItem> item) {
        assert(!FindItem(item->GetName()) && "Category already contains an item with such name");
        item->SetCategory(shared_from_this());
        m_Items.emplace_back(std::move(item));
    }

    //! Find item by name (function name)
    std::shared_ptr<HookCategoryItem> FindItem(std::string_view name) {
        const auto it = rng::find_if(m_Items, [&](const auto& c) { return c->GetName() == name; });
        return it == m_Items.end() 
            ? nullptr 
            : *it;
    }

    //! Find subcategory by name (Only checks for direct sub categories [No recursion])
    std::shared_ptr<HookCategory> FindCategoryByName(std::string_view name, bool create = false) {
        const auto it = rng::find_if(m_SubCategories, [&](const auto& c) { return c->Name() == name; });
        if (it != m_SubCategories.end()) {
            return *it;
        }
        if (create) {
            return m_SubCategories.emplace_back(
                std::make_shared<HookCategory>(std::string{ name }, shared_from_this())
            );
        }
        return nullptr;
    }

    /*!
     * @brief Find category by path, optionally creating all categories in the path that don't exist
     * @param path A `/` separated category list - Eg.: `Entity/Ped` (The hook should be added to the `Entity` category's `Ped` sub-category)
     * @param create If true, will create the category if it doesn't exist
     */
    std::shared_ptr<HookCategory> FindCategoryByPath(std::string_view path, bool create = false) {
        auto cat = shared_from_this();
        if (path == Name()) {
            return cat;
        }
        for (auto part : path | rngv::split('/')) {
            cat = cat->FindCategoryByName(std::string_view{ part }, create);
            if (!create && !cat) {
                return nullptr; // Not found
            }
        }
        return cat;
    }

    /*!
     * @brief Recursively iterate over all items in this category and all subcategories
     * @note Make sure the function doesn't add/remove items/subcategories, we don't want iterators being invalidated.
     */
    void ForEachItem(std::invocable<std::shared_ptr<HookCategoryItem>> auto&& fn) {
        for (auto item : m_Items) {
            std::invoke(fn, std::move(item));
        }
        for (const auto cat : m_SubCategories) {
            cat->ForEachItem(fn);
        }
    }

    /*!
    * @brief Iterate over all sub-categories recursively
    * @note Make sure the function doesn't add/remove items/subcategories, we don't want iterators being invalidated.
    */
    void ForEachCategory(std::invocable<std::shared_ptr<HookCategory>> auto&& fn) {
        std::invoke(fn, shared_from_this());
        for (const auto cat : m_SubCategories) {
            cat->ForEachCategory(fn);
        }
    }

    // Main ordering criteria is the no. of top-level sub categories
    // Secondary criteria is the name
    friend std::weak_ordering operator<=>(const HookCategory& lhs, const HookCategory& rhs) {
        const auto numSubCatOrder = rhs.SubCategories().size() <=> lhs.SubCategories().size();
        if (std::is_eq(numSubCatOrder)) 
            return rhs.Name() <=> lhs.Name(); // Same number of sub-categories, order by name
        return numSubCatOrder; // Order by no. of sub-categories
    }

private:
    std::weak_ptr<HookCategory>                    m_Parent{};        //!< Category we belong to - In case of `RootHookCategory` this is always `nullptr`.
    std::string                                    m_Name{};          //!< Name of this category (Eg.: `Root`, `Global`, `Entity`, etc...)
    std::vector<std::shared_ptr<HookCategory>>     m_SubCategories{}; //!< Subcategories
    std::vector<std::shared_ptr<HookCategoryItem>> m_Items{};         //!< Hooks in this category (`RootCategory` should have none)
};
}; // namespace ReversibleHooks
