#include "StdInc.h"

namespace details {
void* CPtrNodeSingleLink__operator_new(size_t sz) {
    assert(sz == sizeof(CPtrNodeSingleLink<void*>));
    return GetPtrNodeSingleLinkPool()->New();
}

void CPtrNodeSingleLink__operator_delete(void* data, size_t sz) {
    assert(sz == sizeof(CPtrNodeSingleLink<void*>));
    GetPtrNodeSingleLinkPool()->Delete(reinterpret_cast<CPtrNodeSingleLink<void*>*>(data));
}
};
