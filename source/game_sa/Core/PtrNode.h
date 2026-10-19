/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <Pools/Pools.h>

template<typename TItemType, typename TLinkType>
class CPtrNode {
public:
    using LinkType = TLinkType;

public:
    CPtrNode(TItemType item) :
        Item(item)
    { assert(item); }

    bool IsItemValid() const {
        if constexpr (std::is_pointer_v<TItemType>) {
            // In some cases `nullptr` is used as a valid value, so we need to allow it.
            // See `CCover::Update` - it uses entity refs (that is, it clears `Item` by setting it to `nullptr`)
            // `nullptr` errors will surface anyways, so it's not that important to catch them here.
            if (!this->Item) {
                return true;
            }
            using ItemTypeForPool = std::remove_cvref_t<std::remove_pointer_t<TItemType>>;
            if constexpr (requires { notsa::PoolOf<ItemTypeForPool>(); }) {
                if (!notsa::PoolOf<ItemTypeForPool>().IsObjectValid(this->Item)) {
                    return false;
                }
            }
        }

        //constexpr auto GetPtrForPattern = [](uint8 pattern) -> uintptr_t {
        //    uintptr_t out;
        //    memset(&out, pattern, sizeof(out));
        //    return out;
        //};
        //switch ((uintptr_t)(node->Item)) {
        //case GetPtrForPattern(CPtrNodeSingleLinkPool::DEADLAND_FILL):
        //case GetPtrForPattern(CPtrNodeSingleLinkPool::CLEANLAND_FILL):
        //case GetPtrForPattern(CPtrNodeSingleLinkPool::NOMANSLAND_FILL):
        //    return false;
        //}
        return true;
    }

    TItemType Item{};
    LinkType* Next{};
};
