#include "StdInc.h"

#include "rpskin.h"

void RpSkinSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7C67F0)(blockSize, numBlocksToPrealloc);
}

RwBool RpSkinPluginAttach() {
    return ((RwBool(__cdecl *)(void))0x7C6820)();
}

RpAtomic* RpSkinAtomicSetHAnimHierarchy(RpAtomic* atomic, RpHAnimHierarchy* hierarchy) {
    return ((RpAtomic*(__cdecl *)(RpAtomic*, RpHAnimHierarchy*))0x7C7520)(atomic, hierarchy);
}

RpHAnimHierarchy* RpSkinAtomicGetHAnimHierarchy(const RpAtomic* atomic) {
    return ((RpHAnimHierarchy*(__cdecl *)(const RpAtomic*))0x7C7540)(atomic);
}

RpGeometry* RpSkinGeometrySetSkin(RpGeometry* geometry, RpSkin* skin) {
    return ((RpGeometry*(__cdecl *)(RpGeometry*, RpSkin*))0x7C7560)(geometry, skin);
}

RpSkin* RpSkinGeometryGetSkin(RpGeometry* geometry) {
    return ((RpSkin*(__cdecl *)(RpGeometry*))0x7C7550)(geometry);
}

RpSkin* RpSkinCreate(RwUInt32 numVertices, RwUInt32 numBones, RwMatrixWeights* vertexWeights, RwUInt32* vertexIndices, RwMatrix* inverseMatrices) {
    return ((RpSkin*(__cdecl *)(RwUInt32, RwUInt32, RwMatrixWeights*, RwUInt32*, RwMatrix*))0x7C75B0)(numVertices, numBones, vertexWeights, vertexIndices, inverseMatrices);
}

RpSkin* RpSkinDestroy(RpSkin* skin) {
    return ((RpSkin*(__cdecl *)(RpSkin*))0x7C77A0)(skin);
}

RwUInt32 RpSkinGetNumBones(RpSkin* skin) {
    return ((RwUInt32(__cdecl *)(RpSkin*))0x7C77E0)(skin);
}

/*const*/ RwMatrixWeights* RpSkinGetVertexBoneWeights(RpSkin* skin) {
    return ((RwMatrixWeights*(__cdecl *)(RpSkin*))0x7C77F0)(skin);
}

const RwUInt32* RpSkinGetVertexBoneIndices(RpSkin* skin) {
    return ((const RwUInt32*(__cdecl *)(RpSkin*))0x7C7800)(skin);
}

const RwMatrix* RpSkinGetSkinToBoneMatrices(RpSkin* skin) {
    return ((const RwMatrix*(__cdecl *)(RpSkin*))0x7C7810)(skin);
}

RwBool RpSkinIsSplit(RpSkin* skin) {
    return ((RwBool(__cdecl *)(RpSkin*))0x7C7820)(skin);
}

RpAtomic* RpSkinAtomicSetType(RpAtomic* atomic, RpSkinType type) {
    return ((RpAtomic*(__cdecl *)(RpAtomic*, RpSkinType))0x7C7830)(atomic, type);
}

RpSkinType RpSkinAtomicGetType(RpAtomic* atomic) {
    return ((RpSkinType(__cdecl *)(RpAtomic*))0x7C7880)(atomic);
}

RpGeometry* _rpSkinInitialize(RpGeometry* geometry) {
    return ((RpGeometry*(__cdecl *)(RpGeometry*))0x7C8740)(geometry);
}

RpGeometry* _rpSkinDeinitialize(RpGeometry* geometry) {
    return ((RpGeometry*(__cdecl *)(RpGeometry*))0x7C8820)(geometry);
}

RwUInt8* _rpSkinGetMeshBoneRemapIndices(RpSkin* skin) {
    return ((RwUInt8*(__cdecl *)(RpSkin*))0x7C8A40)(skin);
}

RwUInt8* _rpSkinGetMeshBoneRLECount(RpSkin* skin) {
    return ((RwUInt8*(__cdecl *)(RpSkin*))0x7C8A50)(skin);
}

RwUInt8* _rpSkinGetMeshBoneRLE(RpSkin* skin) {
    return ((RwUInt8*(__cdecl *)(RpSkin*))0x7C8A60)(skin);
}

RpSkin* _rpSkinSplitDataCreate(RpSkin* skin, RwUInt32 boneLimit, RwUInt32 numMatrices, RwUInt32 numMeshes, RwUInt32 numRLE) {
    return ((RpSkin*(__cdecl *)(RpSkin*, RwUInt32, RwUInt32, RwUInt32, RwUInt32))0x7C8A70)(skin, boneLimit, numMatrices, numMeshes, numRLE);
}

RwBool _rpSkinSplitDataDestroy(RpSkin* skin) {
    return ((RwBool(__cdecl *)(RpSkin*))0x7C8B10)(skin);
}

RxNodeDefinition* RxNodeDefinitionGetD3D9SkinAtomicAllInOne() {
    return ((RxNodeDefinition*(__cdecl *)(void))0x7CB2A0)();
}
