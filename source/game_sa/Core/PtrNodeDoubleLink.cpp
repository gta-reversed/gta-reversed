#include "StdInc.h"

void* CPtrNodeDoubleLink::operator new(unsigned size) {
    return GetPtrNodeDoubleLinkPool()->New();
}

void CPtrNodeDoubleLink::operator delete(void* ptr, size_t sz) {
    GetPtrNodeDoubleLinkPool()->Delete(static_cast<CPtrNodeDoubleLink*>(ptr));
}
