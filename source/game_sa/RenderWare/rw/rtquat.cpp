#include "StdInc.h"

#include "rtquat.h"

RwBool RtQuatConvertFromMatrix(RtQuat* qpQuat, const RwMatrix* const mpMatrix) {
    return ((RwBool(__cdecl*)(RtQuat*, const RwMatrix* const))0x7EB5C0)(qpQuat, mpMatrix);
}

RtQuat* RtQuatRotate(RtQuat* quat, const RwV3d* axis, RwReal angle, RwOpCombineType combineOp) {
    return ((RtQuat*(__cdecl *)(RtQuat*, const RwV3d*, RwReal, RwOpCombineType))0x7EB7C0)(quat, axis, angle, combineOp);
}

const RtQuat* RtQuatQueryRotate(const RtQuat* quat, RwV3d* unitAxis, RwReal* angle) {
    return ((const RtQuat*(__cdecl *)(const RtQuat*, RwV3d*, RwReal*))0x7EBA80)(quat, unitAxis, angle);
}

RwV3d* RtQuatTransformVectors(RwV3d* vectorsOut, const RwV3d* vectorsIn, const RwInt32 numPoints, const RtQuat* quat) {
    return ((RwV3d*(__cdecl *)(RwV3d*, const RwV3d*, const RwInt32, const RtQuat*))0x7EBBB0)(vectorsOut, vectorsIn, numPoints, quat);
}

RwReal RtQuatModulus(RtQuat* q) {
    return ((RwReal(__cdecl *)(RtQuat*))0x7EBD10)(q);
}
