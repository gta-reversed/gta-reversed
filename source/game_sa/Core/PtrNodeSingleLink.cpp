#include "StdInc.h"

void* CPtrNodeSingleLink::operator new(unsigned size) {
    return GetPtrNodeSingleLinkPool()->New();
}

void CPtrNodeSingleLink::operator delete(void* data) {
    GetPtrNodeSingleLinkPool()->Delete(static_cast<CPtrNodeSingleLink*>(data));
}
