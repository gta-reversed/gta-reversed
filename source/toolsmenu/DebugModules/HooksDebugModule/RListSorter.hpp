#pragma once

#include "RListDefs.h"

namespace RHDebugModule {
class RListSorter {
public:
    /*!
     * @brief Sort the given category and its sub-categories and items
     */
    void Process(RListCategory& cat) noexcept;
};
};
