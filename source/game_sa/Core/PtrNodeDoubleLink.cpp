#include "StdInc.h"

namespace details {
void* CPtrNodeDoubleLink__operator_new(size_t sz) {
    assert(sz == sizeof(CPtrNodeDoubleLink<void*>));
    return GetPtrNodeDoubleLinkPool()->New();
}

void CPtrNodeDoubleLink__operator_delete(void* data, size_t sz) {
    assert(sz == sizeof(CPtrNodeDoubleLink<void*>));
    GetPtrNodeDoubleLinkPool()->Delete(reinterpret_cast<CPtrNodeDoubleLink<void*>*>(data));
}
};
