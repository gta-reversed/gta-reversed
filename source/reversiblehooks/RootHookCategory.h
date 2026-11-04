#pragma once
#include <string_view>

#include "HookCategory.h"
#include "HookCategoryItem.h"
#include "Utility.h"

namespace ReversibleHooks {

// For simplicity we have a `Root` category, which contains all other categories (even `Global`)
// There shouldn't be more than one root category
// Root category shouldn't contain any items (This is a small design issue tbh - For now: don't add items to it, just categories(to which you add items))
class RootHookCategory : public HookCategory {
public:
    static constexpr auto GetRootName() noexcept { return "Root"; }

public:
    RootHookCategory() :
        HookCategory{ GetRootName(), std::shared_ptr<HookCategory>{nullptr} } // Root has no parent
    {
    }
};

}; // namespace ReversibleHooks 
