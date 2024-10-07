#include "StdInc.h"

#include "rwcore.h"

RxHeap* RxHeapCreate(RwUInt32 size) {
    return ((RxHeap*(__cdecl *)(RwUInt32))0x809F90)(size);
}

void RxHeapDestroy(RxHeap* heap) {
    ((void(__cdecl *)(RxHeap*))0x809F30)(heap);
}

RwBool _rxHeapReset(RxHeap* heap) {
    return ((RwBool(__cdecl *)(RxHeap*))0x809EC0)(heap);
}

void* RxHeapAlloc(RxHeap* heap, RwUInt32 size) {
    return ((void*(__cdecl *)(RxHeap*, RwUInt32))0x809AA0)(heap, size);
}

void RxHeapFree(RxHeap* heap, void* block) {
    ((void(__cdecl *)(RxHeap*, void*))0x8098B0)(heap, block);
}

void* RxHeapRealloc(RxHeap* heap, void* block, RwUInt32 newSize, RwBool allowCopy) {
    return ((void*(__cdecl *)(RxHeap*, void*, RwUInt32, RwBool))0x809D10)(heap, block, newSize, allowCopy);
}

void RxPipelineSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x804FC0)(blockSize, numBlocksToPrealloc);
}

RwBool _rxPipelineOpen() {
    return ((RwBool(__cdecl *)(void))0x804FE0)();
}

RwBool _rxPipelineClose() {
    return ((RwBool(__cdecl *)(void))0x804F60)();
}

RxPipeline* RxPipelineCreate() {
    return ((RxPipeline*(__cdecl *)(void))0x8057B0)();
}

void _rxPipelineDestroy(RxPipeline* Pipeline) {
    ((void(__cdecl *)(RxPipeline*))0x805820)(Pipeline);
}

RxHeap* RxHeapGetGlobalHeap() {
    return ((RxHeap*(__cdecl *)(void))0x8052F0)();
}

RxPipeline* RxPipelineExecute(RxPipeline* pipeline, void* data, RwBool heapReset) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*, void*, RwBool))0x805710)(pipeline, data, heapReset);
}

RxPacket* RxPacketCreate(RxPipelineNode* node) {
    return ((RxPacket*(__cdecl *)(RxPipelineNode*))0x805300)(node);
}

RxCluster* RxClusterSetStride(RxCluster* cluster, RwInt32 stride) {
    return ((RxCluster*(__cdecl *)(RxCluster*, RwInt32))0x805330)(cluster, stride);
}

RxCluster* RxClusterSetExternalData(RxCluster* cluster, void* data, RwInt32 stride, RwInt32 numElements) {
    return ((RxCluster*(__cdecl *)(RxCluster*, void*, RwInt32, RwInt32))0x805340)(cluster, data, stride, numElements);
}

RxCluster* RxClusterSetData(RxCluster* cluster, void* data, RwInt32 stride, RwInt32 numElements) {
    return ((RxCluster*(__cdecl *)(RxCluster*, void*, RwInt32, RwInt32))0x8053A0)(cluster, data, stride, numElements);
}

void _rxPacketDestroy(RxPacket* Packet) {
    ((void(__cdecl *)(RxPacket*))0x80F070)(Packet);
}

RxCluster* RxClusterInitializeData(RxCluster* cluster, RwUInt32 numElements, RwUInt16 stride) {
    return ((RxCluster*(__cdecl *)(RxCluster*, RwUInt32, RwUInt16))0x805400)(cluster, numElements, stride);
}

RxCluster* RxClusterResizeData(RxCluster* CurrentCluster, RwUInt32 NumElements) {
    return ((RxCluster*(__cdecl *)(RxCluster*, RwUInt32))0x805470)(CurrentCluster, NumElements);
}

RxCluster* RxClusterDestroyData(RxCluster* CurrentCluster) {
    return ((RxCluster*(__cdecl *)(RxCluster*))0x8054C0)(CurrentCluster);
}

RxCluster* RxClusterLockWrite(RxPacket* packet, RwUInt32 clusterIndex, RxPipelineNode* node) {
    return ((RxCluster*(__cdecl *)(RxPacket*, RwUInt32, RxPipelineNode*))0x8054F0)(packet, clusterIndex, node);
}

void RxClusterUnlock(RxCluster* cluster) {
    ((void(__cdecl *)(RxCluster*))0x8055C0)(cluster);
}

RwUInt32 RxPipelineNodeSendConfigMsg(RxPipelineNode* dest, RwUInt32 msg, RwUInt32 intparam, void* ptrparam) {
    return ((RwUInt32(__cdecl *)(RxPipelineNode*, RwUInt32, RwUInt32, void*))0x8055D0)(dest, msg, intparam, ptrparam);
}

RxPipelineNode* RxPipelineNodeForAllConnectedOutputs(RxPipelineNode* node, RxPipeline* pipeline, RxPipelineNodeOutputCallBack callbackfn, void* callbackdata) {
    return ((RxPipelineNode*(__cdecl *)(RxPipelineNode*, RxPipeline*, RxPipelineNodeOutputCallBack, void*))0x8055F0)(node, pipeline, callbackfn, callbackdata);
}

RxPipelineCluster* RxPipelineNodeGetPipelineCluster(RxPipelineNode* node, RwUInt32 clustersOfInterestIndex) {
    return ((RxPipelineCluster*(__cdecl *)(RxPipelineNode*, RwUInt32))0x805680)(node, clustersOfInterestIndex);
}

RwUInt32 RxPipelineClusterGetCreationAttributes(RxPipelineCluster* cluster) {
    return ((RwUInt32(__cdecl *)(RxPipelineCluster*))0x8056B0)(cluster);
}

RxPipelineCluster* RxPipelineClusterSetCreationAttributes(RxPipelineCluster* cluster, RwUInt32 creationAttributes) {
    return ((RxPipelineCluster*(__cdecl *)(RxPipelineCluster*, RwUInt32))0x8056C0)(cluster, creationAttributes);
}

RwUInt32 RxClusterGetAttributes(RxCluster* cluster) {
    return ((RwUInt32(__cdecl *)(RxCluster*))0x8056E0)(cluster);
}

RxCluster* RxClusterSetAttributes(RxCluster* cluster, RwUInt32 attributes) {
    return ((RxCluster*(__cdecl *)(RxCluster*, RwUInt32))0x8056F0)(cluster, attributes);
}

RxNodeOutput RxPipelineNodeFindOutputByName(RxPipelineNode* node, const RwChar* outputname) {
    return ((RxNodeOutput(__cdecl *)(RxPipelineNode*, const RwChar*))0x805D10)(node, outputname);
}

RxNodeOutput RxPipelineNodeFindOutputByIndex(RxPipelineNode* node, RwUInt32 outputindex) {
    return ((RxNodeOutput(__cdecl *)(RxPipelineNode*, RwUInt32))0x805D70)(node, outputindex);
}

RxNodeInput RxPipelineNodeFindInput(RxPipelineNode* node) {
    return ((RxNodeInput(__cdecl *)(RxPipelineNode*))0x805DA0)(node);
}

RxNodeDefinition* RxPipelineNodeCloneDefinition(RxPipelineNode* node, RxClusterDefinition* cluster2add) {
    return ((RxNodeDefinition*(__cdecl *)(RxPipelineNode*, RxClusterDefinition*))0x8059C0)(node, cluster2add);
}

RxPipeline* RxPipelineNodeRequestCluster(RxPipeline* pipeline, RxPipelineNode* node, RxClusterDefinition* clusterDef) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*, RxPipelineNode*, RxClusterDefinition*))0x805DB0)(pipeline, node, clusterDef);
}

RxPipeline* RxPipelineNodeReplaceCluster(RxPipeline* pipeline, RxPipelineNode* node, RxClusterDefinition* oldClusterDef, RxClusterDefinition* newClusterDef) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*, RxPipelineNode*, RxClusterDefinition*, RxClusterDefinition*))0x805E20)(pipeline, node, oldClusterDef, newClusterDef);
}

void* RxPipelineNodeGetInitData(RxPipelineNode* node) {
    return ((void*(__cdecl *)(RxPipelineNode*))0x805F30)(node);
}

void* RxPipelineNodeCreateInitData(RxPipelineNode* node, RwUInt32 size) {
    return ((void*(__cdecl *)(RxPipelineNode*, RwUInt32))0x805EA0)(node, size);
}

RxPipeline* RxPipelineClone(RxPipeline* pipeline) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*))0x806AC0)(pipeline);
}

RxPipelineNode* RxPipelineFindNodeByName(RxPipeline* pipeline, const RwChar* name, RxPipelineNode* start, RwInt32* nodeIndex) {
    return ((RxPipelineNode*(__cdecl *)(RxPipeline*, const RwChar*, RxPipelineNode*, RwInt32*))0x806B30)(pipeline, name, start, nodeIndex);
}

RxPipelineNode* RxPipelineFindNodeByIndex(RxPipeline* pipeline, RwUInt32 nodeindex) {
    return ((RxPipelineNode*(__cdecl *)(RxPipeline*, RwUInt32))0x806BC0)(pipeline, nodeindex);
}

RxLockedPipe* RxPipelineLock(RxPipeline* pipeline) {
    return ((RxLockedPipe*(__cdecl *)(RxPipeline*))0x806990)(pipeline);
}

RxPipeline* RxLockedPipeUnlock(RxLockedPipe* pipeline) {
    return ((RxPipeline*(__cdecl *)(RxLockedPipe*))0x805F40)(pipeline);
}

RxPipeline* RxLockedPipeReplaceNode(RxLockedPipe* pipeline, RxPipelineNode* node, RxNodeDefinition* nodeDef) {
    return ((RxPipeline*(__cdecl *)(RxLockedPipe*, RxPipelineNode*, RxNodeDefinition*))0x806F20)(pipeline, node, nodeDef);
}

RxPipeline* RxLockedPipeDeleteNode(RxLockedPipe* pipeline, RxPipelineNode* node) {
    return ((RxPipeline*(__cdecl *)(RxLockedPipe*, RxPipelineNode*))0x807040)(pipeline, node);
}

RxPipeline* RxLockedPipeSetEntryPoint(RxLockedPipe* pipeline, RxNodeInput in) {
    return ((RxPipeline*(__cdecl *)(RxLockedPipe*, RxNodeInput))0x807070)(pipeline, in);
}

RxPipeline* RxLockedPipeAddFragment(RxLockedPipe* pipeline, RwUInt32 firstIndex, RxNodeDefinition* nodeDef0, RwUInt32 nodeUnk) {
    return plugin::CallAndReturn<RxPipeline*, 0x806BE0, RxLockedPipe*, RwUInt32, RxNodeDefinition*, RwUInt32>(pipeline, firstIndex, nodeDef0, nodeUnk);
}

RxPipelineNode* RxLockedPipeGetEntryPoint(RxLockedPipe* pipeline) {
    return ((RxPipelineNode*(__cdecl *)(RxLockedPipe*))0x8070D0)(pipeline);
}

RxPipeline* RxLockedPipeAddPath(RxLockedPipe* pipeline, RxNodeOutput out, RxNodeInput in) {
    return ((RxPipeline*(__cdecl *)(RxLockedPipe*, RxNodeOutput, RxNodeInput))0x807100)(pipeline, out, in);
}

RxPipeline* RxLockedPipeDeletePath(RxLockedPipe* pipeline, RxNodeOutput out, RxNodeInput in) {
    return ((RxPipeline*(__cdecl *)(RxLockedPipe*, RxNodeOutput, RxNodeInput))0x807170)(pipeline, out, in);
}

RxPipeline* RxPipelineInsertDebugNode(RxPipeline* pipeline, RxPipelineNode* before, RxPipelineNode* after, RxNodeDefinition* debugNode) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*, RxPipelineNode*, RxPipelineNode*, RxNodeDefinition*))0x8071B0)(pipeline, before, after, debugNode);
}

void RwD3D9VertexBufferManagerChangeDefaultSize(RwUInt32 defaultSize) {
    ((void(__cdecl *)(RwUInt32))0x7F57F0)(defaultSize);
}

RwBool RwD3D9CreateVertexBuffer(RwUInt32 stride, RwUInt32 size, void* vertexBuffer, RwUInt32* offset) {
    return ((RwBool(__cdecl *)(RwUInt32, RwUInt32, void*, RwUInt32*))0x7F5500)(stride, size, vertexBuffer, offset);
}

void RwD3D9DestroyVertexBuffer(RwUInt32 stride, RwUInt32 size, void* vertexBuffer, RwUInt32 offset) {
    ((void(__cdecl *)(RwUInt32, RwUInt32, void*, RwUInt32))0x7F56A0)(stride, size, vertexBuffer, offset);
}

RwBool RwD3D9DynamicVertexBufferCreate(RwUInt32 size, void* vertexBuffer) {
    return ((RwBool(__cdecl *)(RwUInt32, void*))0x7F5A00)(size, vertexBuffer);
}

void RwD3D9DynamicVertexBufferDestroy() {
    ((void(__cdecl *)(void))0x7F5AE0)();
}

RwBool RwD3D9DynamicVertexBufferLock(RwUInt32 vertexSize, RwUInt32 numVertex, void* vertexBufferOut, void* vertexDataOut, RwUInt32* baseIndexOut) {
    return ((RwBool(__cdecl *)(RwUInt32, RwUInt32, void*, void*, RwUInt32*))0x7F5B10)(vertexSize, numVertex, vertexBufferOut, vertexDataOut, baseIndexOut);
}

RwBool RwD3D9DynamicVertexBufferUnlock() {
    return ((RwBool(__cdecl *)(void))0x7F5C90)();
}

void RwRasterSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7FB350)(blockSize, numBlocksToPrealloc);
}

RwRaster* RwRasterCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags) {
    return ((RwRaster*(__cdecl *)(RwInt32, RwInt32, RwInt32, RwInt32))0x7FB230)(width, height, depth, flags);
}

RwBool RwRasterDestroy(RwRaster* raster) {
    return ((RwBool(__cdecl *)(RwRaster*))0x7FB020)(raster);
}

RwRaster* RwRasterGetOffset(RwRaster* raster, RwInt16* xOffset, RwInt16* yOffset) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwInt16*, RwInt16*))0x7FAEA0)(raster, xOffset, yOffset);
}

RwInt32 RwRasterGetNumLevels(RwRaster* raster) {
    return ((RwInt32(__cdecl *)(RwRaster*))0x7FB160)(raster);
}

RwRaster* RwRasterSubRaster(RwRaster* subRaster, RwRaster* raster, RwRect* rect) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwRaster*, RwRect*))0x7FB1D0)(subRaster, raster, rect);
}

RwRaster* RwRasterRenderFast(RwRaster* raster, RwInt32 x, RwInt32 y) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwInt32, RwInt32))0x7FAF50)(raster, x, y);
}

RwRaster* RwRasterRender(RwRaster* raster, RwInt32 x, RwInt32 y) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwInt32, RwInt32))0x7FAFB0)(raster, x, y);
}

RwRaster* RwRasterRenderScaled(RwRaster* raster, RwRect* rect) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwRect*))0x7FAE80)(raster, rect);
}

RwRaster* RwRasterPushContext(RwRaster* raster) {
    return ((RwRaster*(__cdecl *)(RwRaster*))0x7FB060)(raster);
}

RwRaster* RwRasterPopContext() {
    return ((RwRaster*(__cdecl *)(void))0x7FB110)();
}

RwRaster* RwRasterGetCurrentContext() {
    return ((RwRaster*(__cdecl *)(void))0x7FAE60)();
}

RwBool RwRasterClear(RwInt32 pixelValue) {
    return ((RwBool(__cdecl *)(RwInt32))0x7FAEE0)(pixelValue);
}

RwBool RwRasterClearRect(RwRect* rpRect, RwInt32 pixelValue) {
    return ((RwBool(__cdecl *)(RwRect*, RwInt32))0x7FAF90)(rpRect, pixelValue);
}

RwRaster* RwRasterShowRaster(RwRaster* raster, void* dev, RwUInt32 flags) {
    return ((RwRaster*(__cdecl *)(RwRaster*, void*, RwUInt32))0x7FB1A0)(raster, dev, flags);
}

//! NOTE: This function is responsible for calculating the `stride` in the `raster`
RwUInt8* RwRasterLock(RwRaster* raster, RwUInt8 level, RwInt32 lockMode) {
    return ((RwUInt8*(__cdecl *)(RwRaster*, RwUInt8, RwInt32))0x7FB2D0)(raster, level, lockMode);
}

RwRaster* RwRasterUnlock(RwRaster* raster) {
    return ((RwRaster*(__cdecl *)(RwRaster*))0x7FAEC0)(raster);
}

RwUInt8* RwRasterLockPalette(RwRaster* raster, RwInt32 lockMode) {
    return ((RwUInt8*(__cdecl *)(RwRaster*, RwInt32))0x7FB0E0)(raster, lockMode);
}

RwRaster* RwRasterUnlockPalette(RwRaster* raster) {
    return ((RwRaster*(__cdecl *)(RwRaster*))0x7FAFF0)(raster);
}

RwInt32 RwRasterRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return ((RwInt32(__cdecl *)(RwInt32, RwUInt32, RwPluginObjectConstructor, RwPluginObjectDestructor, RwPluginObjectCopy))0x7FB0B0)(size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RwRasterGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x7FAE40)(pluginID);
}

RxRenderStateVector* RxRenderStateVectorSetDefaultRenderStateVector(RxRenderStateVector* rsvp) {
    return ((RxRenderStateVector*(__cdecl *)(RxRenderStateVector*))0x80EA40)(rsvp);
}

RwImage* RwImageCreate(RwInt32 width, RwInt32 height, RwInt32 depth) {
    return ((RwImage*(__cdecl *)(RwInt32, RwInt32, RwInt32))0x8026E0)(width, height, depth);
}

RwBool RwImageDestroy(RwImage* image) {
    return ((RwBool(__cdecl *)(RwImage*))0x802740)(image);
}

RwImage* RwImageAllocatePixels(RwImage* image) {
    return ((RwImage*(__cdecl *)(RwImage*))0x8027A0)(image);
}

RwImage* RwImageFreePixels(RwImage* image) {
    return ((RwImage*(__cdecl *)(RwImage*))0x802860)(image);
}

RwImage* RwImageApplyMask(RwImage* image, const RwImage* mask) {
    return ((RwImage*(__cdecl *)(RwImage*, const RwImage*))0x802AF0)(image, mask);
}

RwImage* RwImageMakeMask(RwImage* image) {
    return ((RwImage*(__cdecl *)(RwImage*))0x802A20)(image);
}

RwImage* RwImageReadMaskedImage(const RwChar* imageName, const RwChar* maskname) {
    return ((RwImage*(__cdecl *)(const RwChar*, const RwChar*))0x8035C0)(imageName, maskname);
}

RwImage* RwImageRead(const RwChar* imageName) {
    return ((RwImage*(__cdecl *)(const RwChar*))0x802FD0)(imageName);
}

const RwChar* RwImageSetPath(const RwChar* path) {
    return ((const RwChar*(__cdecl *)(const RwChar*))0x802EA0)(path);
}

RwBool RwImageSetGamma(RwReal gammaValue) {
    return ((RwBool(__cdecl *)(RwReal))0x803FE0)(gammaValue);
}

RwImage* RwImageGammaCorrect(RwImage* image) {
    return ((RwImage*(__cdecl *)(RwImage*))0x803E30)(image);
}

RwBool RwPalQuantInit(RwPalQuant* pq) {
    return ((RwBool(__cdecl *)(RwPalQuant*))0x80C470)(pq);
}

void RwPalQuantTerm(RwPalQuant* pq) {
    ((void(__cdecl *)(RwPalQuant*))0x80C520)(pq);
}

void RwPalQuantAddImage(RwPalQuant* pq, RwImage* img, RwReal weight) {
    ((void(__cdecl *)(RwPalQuant*, RwImage*, RwReal))0x80AA80)(pq, img, weight);
}

RwInt32 RwPalQuantResolvePalette(RwRGBA* palette, RwInt32 maxcols, RwPalQuant* pq) {
    return ((RwInt32(__cdecl *)(RwRGBA*, RwInt32, RwPalQuant*))0x80AF60)(palette, maxcols, pq);
}

void RwPalQuantMatchImage(RwUInt8* dstpixels, RwInt32 dststride, RwInt32 dstdepth, RwBool dstPacked, RwPalQuant* pq, RwImage* img) {
    ((void(__cdecl *)(RwUInt8*, RwInt32, RwInt32, RwBool, RwPalQuant*, RwImage*))0x80BF20)(dstpixels, dststride, dstdepth, dstPacked, pq, img);
}

RwBool RwTextureSetMipmapping(RwBool enable) {
    return ((RwBool(__cdecl *)(RwBool))0x7F3530)(enable);
}

RwBool RwTextureGetMipmapping() {
    return ((RwBool(__cdecl *)(void))0x7F3550)();
}

RwBool RwTextureSetAutoMipmapping(RwBool enable) {
    return ((RwBool(__cdecl *)(RwBool))0x7F3560)(enable);
}

RwBool RwTextureGetAutoMipmapping() {
    return ((RwBool(__cdecl *)(void))0x7F3580)();
}

RwBool RwTextureSetMipmapGenerationCallBack(RwTextureCallBackMipmapGeneration callback) {
    return ((RwBool(__cdecl *)(RwTextureCallBackMipmapGeneration))0x7F3C70)(callback);
}

RwTextureCallBackMipmapGeneration RwTextureGetMipmapGenerationCallBack() {
    return ((RwTextureCallBackMipmapGeneration(__cdecl *)(void))0x7F3C90)();
}

RwBool RwTextureSetMipmapNameCallBack(RwTextureCallBackMipmapName callback) {
    return ((RwBool(__cdecl *)(RwTextureCallBackMipmapName))0x7F3CA0)(callback);
}

RwTextureCallBackMipmapName RwTextureGetMipmapNameCallBack() {
    return ((RwTextureCallBackMipmapName(__cdecl *)(void))0x7F3CC0)();
}

RwBool RwTextureGenerateMipmapName(RwChar* name, RwChar* maskName, RwUInt8 mipLevel, RwInt32 format) {
    return ((RwBool(__cdecl *)(RwChar*, RwChar*, RwUInt8, RwInt32))0x7F3AA0)(name, maskName, mipLevel, format);
}

RwBool RwTextureRasterGenerateMipmaps(RwRaster* raster, RwImage* image) {
    return ((RwBool(__cdecl *)(RwRaster*, RwImage*))0x7F3CD0)(raster, image);
}

RwBool _rwTextureSetAutoMipMapState(RwBool enable) {
    return ((RwBool(__cdecl *)(RwBool))0x7F3590)(enable);
}

RwBool _rwTextureGetAutoMipMapState() {
    return ((RwBool(__cdecl *)(void))0x7F35C0)();
}

RwTextureCallBackRead RwTextureGetReadCallBack() {
    return ((RwTextureCallBackRead(__cdecl *)(void))0x7F3520)();
}

RwBool RwTextureSetReadCallBack(RwTextureCallBackRead callBack) {
    return ((RwBool(__cdecl *)(RwTextureCallBackRead))0x7F3500)(callBack);
}

RwTextureCallBackFind RwTextureGetFindCallBack() {
    return ((RwTextureCallBackFind(__cdecl *)(void))0x7F34F0)();
}

RwBool RwTextureSetFindCallBack(RwTextureCallBackFind callBack) {
    return ((RwBool(__cdecl *)(RwTextureCallBackFind))0x7F34D0)(callBack);
}

RwTexture* RwTextureSetName(RwTexture* texture, const RwChar* name) {
    return ((RwTexture*(__cdecl *)(RwTexture*, const RwChar*))0x7F38A0)(texture, name);
}

RwTexture* RwTextureSetMaskName(RwTexture* texture, const RwChar* maskName) {
    return ((RwTexture*(__cdecl *)(RwTexture*, const RwChar*))0x7F3910)(texture, maskName);
}

void RwTexDictionarySetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7F3E80)(blockSize, numBlocksToPrealloc);
}

RwTexDictionary* RwTexDictionaryCreate() {
    return ((RwTexDictionary*(__cdecl *)(void))0x7F3600)();
}

RwBool RwTexDictionaryDestroy(RwTexDictionary* dict) {
    return ((RwBool(__cdecl *)(RwTexDictionary*))0x7F36A0)(dict);
}

RwTexture* RwTextureCreate(RwRaster* raster) {
    return ((RwTexture*(__cdecl *)(RwRaster*))0x7F37C0)(raster);
}

RwTexture* RwTextureRead(const char* name, const char* maskName) {
    return plugin::CallAndReturn<RwTexture*, 0x7F3AC0, const char*, const char*>(name, maskName);
}

RwBool RwTextureDestroy(RwTexture* texture) {
    return ((RwBool(__cdecl *)(RwTexture*))0x7F3820)(texture);
}

RwTexture* RwTextureSetRaster(RwTexture* texture, RwRaster* raster) {
    return ((RwTexture*(__cdecl *)(RwTexture*, RwRaster*))0x7F35D0)(texture, raster);
}

RwTexture* RwTexDictionaryAddTexture(RwTexDictionary* dict, RwTexture* texture) {
    return ((RwTexture*(__cdecl *)(RwTexDictionary*, RwTexture*))0x7F3980)(dict, texture);
}

RwTexture* RwTexDictionaryRemoveTexture(RwTexture* texture) {
    return ((RwTexture*(__cdecl *)(RwTexture*))0x7F39C0)(texture);
}

RwTexture* RwTexDictionaryFindNamedTexture(RwTexDictionary* dict, const RwChar* name) {
    return ((RwTexture*(__cdecl *)(RwTexDictionary*, const RwChar*))0x7F39F0)(dict, name);
}

RwTexDictionary* RwTexDictionaryGetCurrent() {
    return ((RwTexDictionary*(__cdecl *)(void))0x7F3A90)();
}

RwTexDictionary* RwTexDictionarySetCurrent(RwTexDictionary* dict) {
    return ((RwTexDictionary*(__cdecl *)(RwTexDictionary*))0x7F3A70)(dict);
}

const RwTexDictionary* RwTexDictionaryForAllTextures(const RwTexDictionary* dict, RwTextureCallBack fpCallBack, void* data) {
    return ((const RwTexDictionary*(__cdecl *)(const RwTexDictionary*, RwTextureCallBack, void*))0x7F3730)(dict, fpCallBack, data);
}

RwBool RwTexDictionaryForAllTexDictionaries(RwTexDictionaryCallBack fpCallBack, void* data) {
    return ((RwBool(__cdecl *)(RwTexDictionaryCallBack, void*))0x7F3770)(fpCallBack, data);
}

RwInt32 RwTextureRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return ((RwInt32(__cdecl *)(RwInt32, RwUInt32, RwPluginObjectConstructor, RwPluginObjectDestructor, RwPluginObjectCopy))0x7F3BB0)(size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RwTexDictionaryRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return ((RwInt32(__cdecl *)(RwInt32, RwUInt32, RwPluginObjectConstructor, RwPluginObjectDestructor, RwPluginObjectCopy))0x7F3C10)(size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RwTextureGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x7F3BE0)(pluginID);
}

RwInt32 RwTexDictionaryGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x7F3C40)(pluginID);
}

RwBool RwTextureValidatePlugins(const RwTexture* texture) {
    return ((RwBool(__cdecl *)(const RwTexture*))0x7F3C00)(texture);
}

RwBool RwTexDictionaryValidatePlugins(const RwTexDictionary* dict) {
    return ((RwBool(__cdecl *)(const RwTexDictionary*))0x7F3C60)(dict);
}

rwIm3DPool* _rwIm3DGetPool() {
    return ((rwIm3DPool*(__cdecl *)(void))0x7EFDD0)();
}

void* RwIm3DTransform(RwIm3DVertex* pVerts, RwUInt32 numVerts, RwMatrix* ltm, RwUInt32 flags) {
    return ((void*(__cdecl *)(RwIm3DVertex*, RwUInt32, RwMatrix*, RwUInt32))0x7EF450)(pVerts, numVerts, ltm, flags);
}

RwBool RwIm3DEnd() {
    return ((RwBool(__cdecl *)(void))0x7EF520)();
}

RwBool RwIm3DRenderLine(RwInt32 vert1, RwInt32 vert2) {
    return ((RwBool(__cdecl *)(RwInt32, RwInt32))0x7EF900)(vert1, vert2);
}

RwBool RwIm3DRenderTriangle(RwInt32 vert1, RwInt32 vert2, RwInt32 vert3) {
    return ((RwBool(__cdecl *)(RwInt32, RwInt32, RwInt32))0x7EF810)(vert1, vert2, vert3);
}

RwBool RwIm3DRenderIndexedPrimitive(RwPrimitiveType primType, RwImVertexIndex* indices, RwInt32 numIndices) {
    return ((RwBool(__cdecl *)(RwPrimitiveType, RwImVertexIndex*, RwInt32))0x7EF550)(primType, indices, numIndices);
}

RwBool RwIm3DRenderPrimitive(RwPrimitiveType primType) {
    return ((RwBool(__cdecl *)(RwPrimitiveType))0x7EF6B0)(primType);
}

RxPipeline* RwIm3DGetTransformPipeline() {
    return ((RxPipeline*(__cdecl *)(void))0x7EF9D0)();
}

RxPipeline* RwIm3DGetRenderPipeline(RwPrimitiveType primType) {
    return ((RxPipeline*(__cdecl *)(RwPrimitiveType))0x7EF9E0)(primType);
}

RxPipeline* RwIm3DSetTransformPipeline(RxPipeline* pipeline) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*))0x7EFAC0)(pipeline);
}

RxPipeline* RwIm3DSetRenderPipeline(RxPipeline* pipeline, RwPrimitiveType primType) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*, RwPrimitiveType))0x7EFB10)(pipeline, primType);
}

RwRaster* RwD3D9RasterStreamReadDDS(RwStream* stream) {
    return ((RwRaster*(__cdecl *)(RwStream*))0x81F360)(stream);
}

RwTexture* RwD3D9DDSTextureRead(const RwChar* name, const RwChar* maskname) {
    return ((RwTexture*(__cdecl *)(const RwChar*, const RwChar*))0x820A10)(name, maskname);
}

RwBool RwD3D9RasterIsCompressed(const RwRaster* raster) {
    return ((RwBool(__cdecl *)(const RwRaster*))0x820C90)(raster);
}

RwBool RwD3D9DeviceSupportsDXTTexture() {
    return ((RwBool(__cdecl *)(void))0x7F9C30)();
}

IDirect3DDevice9* RwD3D9GetCurrentD3DDevice() {
    return ((IDirect3DDevice9*(__cdecl *)(void))0x7F9D50)();
}

RwUInt32 RwD3D9EngineGetMaxMultiSamplingLevels() {
    return ((RwUInt32(__cdecl *)(void))0x7F84E0)();
}

void RwD3D9EngineSetMultiSamplingLevels(RwUInt32 numLevels) {
    ((void(__cdecl *)(RwUInt32))0x7F84F0)(numLevels);
}

void RwD3D9EngineSetRefreshRate(RwUInt32 refreshRate) {
    ((void(__cdecl *)(RwUInt32))0x7F8580)(refreshRate);
}

void RwD3D9EngineSetMultiThreadSafe(RwBool enable) {
    ((void(__cdecl *)(RwBool))0x7F8620)(enable);
}

void RwD3D9EngineSetSoftwareVertexProcessing(RwBool enable) {
    ((void(__cdecl *)(RwBool))0x7F8630)(enable);
}

void* RwD3D9GetCurrentD3DRenderTarget(RwUInt32 index) {
    return ((void*(__cdecl *)(RwUInt32))0x7F9E80)(index);
}

RwBool RwD3D9SetRenderTarget(RwUInt32 index, RwRaster* raster) {
    return ((RwBool(__cdecl *)(RwUInt32, RwRaster*))0x7F9E90)(index, raster);
}

RwBool RwD3D9ChangeVideoMode(RwInt32 modeIndex) {
    return ((RwBool(__cdecl *)(RwInt32))0x7F8640)(modeIndex);
}

RwBool RwD3D9ChangeMultiSamplingLevels(RwUInt32 numLevels) {
    return ((RwBool(__cdecl *)(RwUInt32))0x7F8A90)(numLevels);
}

RwBool RwD3D9CameraAttachWindow(void* camera, void* hwnd) {
    return ((RwBool(__cdecl *)(void*, void*))0x7F8D70)(camera, hwnd);
}

void RwD3D9SetStreamSource(RwUInt32 streamNumber, void* streamData, RwUInt32 offset, RwUInt32 stride) {
    ((void(__cdecl *)(RwUInt32, void*, RwUInt32, RwUInt32))0x7FA030)(streamNumber, streamData, offset, stride);
}

void _rwD3D9RenderStateFlushCache() {
    ((void(__cdecl *)(void))0x7FC200)();
}

void _rwD3D9DrawIndexedPrimitiveUP(RwUInt32 primitiveType, RwUInt32 minIndex, RwUInt32 numVertices, RwUInt32 primitiveCount, const void* indexData, const void* vertexStreamZeroData, RwUInt32 VertexStreamZeroStride) {
    ((void(__cdecl *)(RwUInt32, RwUInt32, RwUInt32, RwUInt32, const void*, const void*, RwUInt32))0x7FA1F0)(primitiveType, minIndex, numVertices, primitiveCount, indexData, vertexStreamZeroData, VertexStreamZeroStride);
}

void _rwD3D9DrawPrimitiveUP(RwUInt32 primitiveType, RwUInt32 primitiveCount, const void* vertexStreamZeroData, RwUInt32 VertexStreamZeroStride) {
    ((void(__cdecl *)(RwUInt32, RwUInt32, const void*, RwUInt32))0x7FA290)(primitiveType, primitiveCount, vertexStreamZeroData, VertexStreamZeroStride);
}

void _rwD3D9DrawIndexedPrimitive(RwUInt32 primitiveType, RwInt32 baseVertexIndex, RwUInt32 minIndex, RwUInt32 numVertices, RwUInt32 startIndex, RwUInt32 primitiveCount) {
    ((void(__cdecl *)(RwUInt32, RwInt32, RwUInt32, RwUInt32, RwUInt32, RwUInt32))0x7FA320)(primitiveType, baseVertexIndex, minIndex, numVertices, startIndex, primitiveCount);
}

void _rwD3D9SetVertexShaderConstant(RwUInt32 registerAddress, const void* constantData, RwUInt32 constantCount) {
    ((void(__cdecl *)(RwUInt32, const void*, RwUInt32))0x7FACA0)(registerAddress, constantData, constantCount);
}

void _rwD3D9SetPixelShaderConstant(RwUInt32 registerAddress, const void* constantData, RwUInt32 constantCount) {
    ((void(__cdecl *)(RwUInt32, const void*, RwUInt32))0x7FAD00)(registerAddress, constantData, constantCount);
}

void _rwD3D9SetFVF(RwUInt32 fvf) {
    ((void(__cdecl *)(RwUInt32))0x7F9F30)(fvf);
}

void _rwD3D9SetVertexShader(void *shader) {
    ((void(__cdecl *)(void *))0x7F9FB0)(shader);
}

void _rwD3D9SetPixelShader(void *shader) {
    ((void(__cdecl *)(void *))0x7F9FF0)(shader);
}

void RwD3D9SetRenderState(RwUInt32 state, RwUInt32 value) {
    ((void(__cdecl *)(RwUInt32, RwUInt32))0x7FC2D0)(state, value);
}

void RwD3D9GetRenderState(RwUInt32 state, void* value) {
    ((void(__cdecl *)(RwUInt32, void*))0x7FC320)(state, value);
}

void RwD3D9SetTextureStageState(RwUInt32 stage, RwUInt32 type, RwUInt32 value) {
    ((void(__cdecl *)(RwUInt32, RwUInt32, RwUInt32))0x7FC340)(stage, type, value);
}

void RwD3D9GetTextureStageState(RwUInt32 stage, RwUInt32 type, void* value) {
    ((void(__cdecl *)(RwUInt32, RwUInt32, void*))0x7FC3A0)(stage, type, value);
}

void RwD3D9SetSamplerState(RwUInt32 stage, RwUInt32 type, RwUInt32 value) {
    ((void(__cdecl *)(RwUInt32, RwUInt32, RwUInt32))0x7FC3C0)(stage, type, value);
}

void RwD3D9GetSamplerState(RwUInt32 stage, RwUInt32 type, void* value) {
    ((void(__cdecl *)(RwUInt32, RwUInt32, void*))0x7FC400)(stage, type, value);
}

void RwD3D9SetStencilClear(RwUInt32 stencilClear) {
    ((void(__cdecl *)(RwUInt32))0x7F9D30)(stencilClear);
}

RwUInt32 RwD3D9GetStencilClear() {
    return ((RwUInt32(__cdecl *)(void))0x7F9D40)();
}

RwBool RwD3D9SetTexture(RwTexture* texture, RwUInt32 stage) {
    return ((RwBool(__cdecl *)(RwTexture*, RwUInt32))0x7FDE70)(texture, stage);
}

RwBool RwD3D9SetTransform(RwUInt32 state, const void* matrix) {
    return ((RwBool(__cdecl *)(RwUInt32, const void*))0x7FA390)(state, matrix);
}

void RwD3D9GetTransform(RwUInt32 state, void* matrix) {
    ((void(__cdecl *)(RwUInt32, void*))0x7FA4F0)(state, matrix);
}

RwBool RwD3D9SetMaterial(const void* material) {
    return ((RwBool(__cdecl *)(const void*))0x7FC430)(material);
}

RwBool RwD3D9SetClipPlane(RwUInt32 index, const RwV4d* plane) {
    return ((RwBool(__cdecl *)(RwUInt32, const RwV4d*))0x7FC4A0)(index, plane);
}

RwBool RwD3D9SetTransformWorld(const RwMatrix* matrix) {
    return ((RwBool(__cdecl *)(const RwMatrix*))0x7FA520)(matrix);
}

RwBool RwD3D9SetSurfaceProperties(const RwSurfaceProperties* surfaceProps, const RwRGBA* color, RwUInt32 flags) {
    return ((RwBool(__cdecl *)(const RwSurfaceProperties*, const RwRGBA*, RwUInt32))0x7FC4D0)(surfaceProps, color, flags);
}

RwBool RwD3D9SetLight(RwInt32 index, const void* light) {
    return ((RwBool(__cdecl *)(RwInt32, const void*))0x7FA660)(index, light);
}

void RwD3D9GetLight(RwInt32 index, void* light) {
    ((void(__cdecl *)(RwInt32, void*))0x7FA820)(index, light);
}

RwBool RwD3D9EnableLight(RwInt32 index, RwBool enable) {
    return ((RwBool(__cdecl *)(RwInt32, RwBool))0x7FA860)(index, enable);
}

RwBool RwD3D9IndexBufferCreate(RwUInt32 numIndices, void* indexBuffer) {
    return ((RwBool(__cdecl *)(RwUInt32, void*))0x4C9970)(numIndices, indexBuffer);
}

RwBool RwD3D9CreateVertexDeclaration(const void* elements, void* vertexdeclaration) {
    return ((RwBool(__cdecl *)(const void*, void*))0x7FAA30)(elements, vertexdeclaration);
}

void RwD3D9DeleteVertexDeclaration() {
    ((void(__cdecl *)(void))0x7FAC10)();
}

void RwD3D9DeleteVertexShader() {
    ((void(__cdecl *)(void))0x7FAC90)();
}

RwBool RwD3D9CreatePixelShader(const RwUInt32* function, void* shader) {
    return ((RwBool(__cdecl *)(const RwUInt32*, void*))0x7FACC0)(function, shader);
}

void RwD3D9DeletePixelShader() {
    ((void(__cdecl *)(void))0x7FACF0)();
}

D3DCAPS9* RwD3D9GetCaps() {
    return ((D3DCAPS9*(__cdecl *)(void))0x7FAD20)();
}

RwBool RwD3D9CameraIsSphereFullyInsideFrustum(const void* camera, const void* sphere) {
    return ((RwBool(__cdecl *)(const void*, const void*))0x7FAD30)(camera, sphere);
}

RwBool RwD3D9CameraIsBBoxFullyInsideFrustum(const void* camera, const void* boundingBox) {
    return ((RwBool(__cdecl *)(const void*, const void*))0x7FAD90)(camera, boundingBox);
}

void _rwD3D9RasterConvertToNonPalettized(RwRaster* raster) {
    ((void(__cdecl *)(RwRaster*))0x4CD250)(raster);
}

RwBool _rwDeviceRegisterPlugin() {
    return ((RwBool(__cdecl *)(void))0x7F5F60)();
}

void _rwD3D9DeviceSetRestoreCallback(rwD3D9DeviceRestoreCallBack callback) {
    ((void(__cdecl *)(rwD3D9DeviceRestoreCallBack))0x7FAE20)(callback);
}

rwD3D9DeviceRestoreCallBack _rwD3D9DeviceGetRestoreCallback() {
    return ((rwD3D9DeviceRestoreCallBack(__cdecl *)(void))0x7FAE30)();
}

RwImage* RwImageResample(RwImage* dstImage, const RwImage* srcImage) {
    return ((RwImage*(__cdecl *)(RwImage*, const RwImage*))0x80C600)(dstImage, srcImage);
}

RwImage* RwImageCreateResample(const RwImage* srcImage, RwInt32 width, RwInt32 height) {
    return ((RwImage*(__cdecl *)(const RwImage*, RwInt32, RwInt32))0x80CD10)(srcImage, width, height);
}

RwImage* RwImageSetFromRaster(RwImage* image, RwRaster* raster) {
    return ((RwImage*(__cdecl *)(RwImage*, RwRaster*))0x804250)(image, raster);
}

RwRaster* RwRasterSetFromImage(RwRaster* raster, RwImage* image) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwImage*))0x804290)(raster, image);
}

RwRaster* RwRasterRead(const RwChar* filename) {
    return ((RwRaster*(__cdecl *)(const RwChar*))0x8043F0)(filename);
}

RwRaster* RwRasterReadMaskedRaster(const RwChar* filename, const RwChar* maskname) {
    return ((RwRaster*(__cdecl *)(const RwChar*, const RwChar*))0x8044E0)(filename, maskname);
}

RwImage* RwImageFindRasterFormat(RwImage* ipImage, RwInt32 nRasterType, RwInt32* npWidth, RwInt32* npHeight, RwInt32* npDepth, RwInt32* npFormat) {
    return ((RwImage*(__cdecl *)(RwImage*, RwInt32, RwInt32*, RwInt32*, RwInt32*, RwInt32*))0x8042C0)(ipImage, nRasterType, npWidth, npHeight, npDepth, npFormat);
}

RwFrame* RwFrameForAllObjects(RwFrame* frame, RwObjectCallBack callBack, void* data) {
    return ((RwFrame*(__cdecl *)(RwFrame*, RwObjectCallBack, void*))0x7F1200)(frame, callBack, data);
}

RwFrame* RwFrameTranslate(RwFrame* frame, const RwV3d* v, RwOpCombineType combine) {
    return ((RwFrame*(__cdecl *)(RwFrame*, const RwV3d*, RwOpCombineType))0x7F0E30)(frame, v, combine);
}

RwFrame* RwFrameRotate(RwFrame* frame, const RwV3d* axis, RwReal angle, RwOpCombineType combine) {
    return ((RwFrame*(__cdecl *)(RwFrame*, const RwV3d*, RwReal, RwOpCombineType))0x7F1010)(frame, axis, angle, combine);
}

RwFrame* RwFrameScale(RwFrame* frame, const RwV3d* v, RwOpCombineType combine) {
    return ((RwFrame*(__cdecl *)(RwFrame*, const RwV3d*, RwOpCombineType))0x7F0ED0)(frame, v, combine);
}

RwFrame* RwFrameTransform(RwFrame* frame, const RwMatrix* m, RwOpCombineType combine) {
    return ((RwFrame*(__cdecl *)(RwFrame*, const RwMatrix*, RwOpCombineType))0x7F0F70)(frame, m, combine);
}

RwFrame* RwFrameOrthoNormalize(RwFrame* frame) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7F1170)(frame);
}

RwFrame* RwFrameSetIdentity(RwFrame* frame) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7F10B0)(frame);
}

RwFrame* RwFrameCloneHierarchy(RwFrame* root) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7F0250)(root);
}

RwBool RwFrameDestroyHierarchy(RwFrame* frame) {
    return ((RwBool(__cdecl *)(RwFrame*))0x7F08A0)(frame);
}

RwFrame* RwFrameForAllChildren(RwFrame* frame, RwFrameCallBack callBack, void* data) {
    return ((RwFrame*(__cdecl *)(RwFrame*, RwFrameCallBack, void*))0x7F0DC0)(frame, callBack, data);
}

RwFrame* RwFrameRemoveChild(RwFrame* child) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7F0CD0)(child);
}

RwFrame* RwFrameAddChild(RwFrame* parent, RwFrame* child) {
    return ((RwFrame*(__cdecl *)(RwFrame*, RwFrame*))0x7F0B00)(parent, child);
}

RwFrame* RwFrameAddChildNoUpdate(RwFrame* parent, RwFrame* child) {
    return ((RwFrame*(__cdecl *)(RwFrame*, RwFrame*))0x7F09C0)(parent, child);
}

RwFrame* RwFrameGetRoot(const RwFrame* frame) {
    return ((RwFrame*(__cdecl *)(const RwFrame*))0x7F09B0)(frame);
}

RwMatrix* RwFrameGetLTM(RwFrame* frame) {
    return ((RwMatrix*(__cdecl *)(RwFrame*))0x7F0990)(frame);
}

RwFrame* RwFrameUpdateObjects(RwFrame* frame) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7F0910)(frame);
}

void RwFrameSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7EFED0)(blockSize, numBlocksToPrealloc);
}

RwFrame* RwFrameCreate() {
    return ((RwFrame*(__cdecl *)(void))0x7F0410)();
}

RwBool RwFrameDestroy(RwFrame* frame) {
    assert(frame);
    return ((RwBool(__cdecl *)(RwFrame*))0x7F05A0)(frame);
}

void _rwFrameInit(RwFrame* frame) {
    ((void(__cdecl *)(RwFrame*))0x7F0450)(frame);
}

void _rwFrameDeInit(RwFrame* frame) {
    ((void(__cdecl *)(RwFrame*))0x7F06F0)(frame);
}

RwBool RwFrameDirty(const RwFrame* frame) {
    return ((RwBool(__cdecl *)(const RwFrame*))0x7F0340)(frame);
}

RwInt32 RwFrameCount(RwFrame* frame) {
    return ((RwInt32(__cdecl *)(RwFrame*))0x7F0E00)(frame);
}

RwInt32 RwFrameRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return ((RwInt32(__cdecl *)(RwInt32, RwUInt32, RwPluginObjectConstructor, RwPluginObjectDestructor, RwPluginObjectCopy))0x7F1260)(size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RwFrameGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x7F1290)(pluginID);
}

RwBool RwFrameValidatePlugins(const RwFrame* frame) {
    return ((RwBool(__cdecl *)(const RwFrame*))0x7F12B0)(frame);
}

RwFrame* _rwFrameCloneAndLinkClones(RwFrame* root) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7EFFB0)(root);
}

RwFrame* _rwFramePurgeClone(RwFrame* root) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7F01A0)(root);
}

void _rwObjectHasFrameReleaseFrame() {
    ((void(__cdecl *)(void))0x804F40)();
}

RwBool _rwFrameSyncDirty() {
    return ((RwBool(__cdecl *)(void))0x809550)();
}

void _rwFrameSyncHierarchyLTM(RwFrame* frame) {
    ((void(__cdecl *)(RwFrame*))0x8097D0)(frame);
}

RwInt32 RwTextureRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x804550)(pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 RwTextureSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkAlwaysCallBack))0x804580)(pluginID, alwaysCB);
}

RwUInt32 RwTextureStreamGetSize(const RwTexture* texture) {
    return ((RwUInt32(__cdecl *)(const RwTexture*))0x8045A0)(texture);
}

RwTexture* RwTextureStreamRead(RwStream* stream) {
    return ((RwTexture*(__cdecl *)(RwStream*))0x8046E0)(stream);
}

const RwTexture* RwTextureStreamWrite(const RwTexture* texture, RwStream* stream) {
    return ((const RwTexture*(__cdecl *)(const RwTexture*, RwStream*))0x8045E0)(texture, stream);
}

RwInt32 RwTexDictionaryRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x8048E0)(pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 RwTexDictionarySetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkAlwaysCallBack))0x804910)(pluginID, alwaysCB);
}

RwUInt32 RwTexDictionaryStreamGetSize(const RwTexDictionary* texDict) {
    return ((RwUInt32(__cdecl *)(const RwTexDictionary*))0x804930)(texDict);
}

RwTexDictionary* RwTexDictionaryStreamRead(RwStream* stream) {
    return ((RwTexDictionary*(__cdecl *)(RwStream*))0x804C30)(stream);
}

const RwTexDictionary* RwTexDictionaryStreamWrite(const RwTexDictionary* texDict, RwStream* stream) {
    return ((const RwTexDictionary*(__cdecl *)(const RwTexDictionary*, RwStream*))0x8049F0)(texDict, stream);
}

RwTextureChunkInfo* _rwTextureChunkInfoRead(RwStream* stream, RwTextureChunkInfo* textureChunkInfo, RwInt32* bytesRead) {
    return ((RwTextureChunkInfo*(__cdecl *)(RwStream*, RwTextureChunkInfo*, RwInt32*))0x804E60)(stream, textureChunkInfo, bytesRead);
}

void RwFrameListSetAutoUpdate(RwBool flag) {
    ((void(__cdecl *)(RwBool))0x807570)(flag);
}

RwInt32 RwFrameRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x807580)(pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 RwFrameSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkAlwaysCallBack))0x8075B0)(pluginID, alwaysCB);
}

rwFrameList* _rwFrameListInitialize(rwFrameList* frameList, RwFrame* frame) {
    return ((rwFrameList*(__cdecl *)(rwFrameList*, RwFrame*))0x8075D0)(frameList, frame);
}

RwBool _rwFrameListFindFrame(const rwFrameList* frameList, const RwFrame* frame, RwInt32* npIndex) {
    return ((RwBool(__cdecl *)(const rwFrameList*, const RwFrame*, RwInt32*))0x8076E0)(frameList, frame, npIndex);
}

rwFrameList* _rwFrameListDeinitialize(rwFrameList* frameList) {
    return ((rwFrameList*(__cdecl *)(rwFrameList*))0x807720)(frameList);
}

RwUInt32 _rwFrameListStreamGetSize(const rwFrameList* frameList) {
    return ((RwUInt32(__cdecl *)(const rwFrameList*))0x807750)(frameList);
}

rwFrameList* _rwFrameListStreamRead(RwStream* stream, rwFrameList* fl) {
    return ((rwFrameList*(__cdecl *)(RwStream*, rwFrameList*))0x807970)(stream, fl);
}

const rwFrameList* _rwFrameListStreamWrite(const rwFrameList* frameList, RwStream* stream) {
    return ((const rwFrameList*(__cdecl *)(const rwFrameList*, RwStream*))0x8077A0)(frameList, stream);
}

RwBBox* RwBBoxCalculate(RwBBox* boundBox, const RwV3d* verts, RwInt32 numVerts) {
    return ((RwBBox*(__cdecl *)(RwBBox*, const RwV3d*, RwInt32))0x808F60)(boundBox, verts, numVerts);
}

RwBBox* RwBBoxInitialize(RwBBox* boundBox, const RwV3d* vertex) {
    return ((RwBBox*(__cdecl *)(RwBBox*, const RwV3d*))0x809020)(boundBox, vertex);
}

RwBBox* RwBBoxAddPoint(RwBBox* boundBox, const RwV3d* vertex) {
    return ((RwBBox*(__cdecl *)(RwBBox*, const RwV3d*))0x809060)(boundBox, vertex);
}

RwBool RwBBoxContainsPoint(const RwBBox* boundBox, const RwV3d* vertex) {
    return ((RwBool(__cdecl *)(const RwBBox*, const RwV3d*))0x8090E0)(boundBox, vertex);
}

RwCamera* RwCameraBeginUpdate(RwCamera* camera) {
    ZoneScoped;

    return ((RwCamera*(__cdecl *)(RwCamera*))0x7EE190)(camera);
}

RwCamera* RwCameraEndUpdate(RwCamera* camera) {
    ZoneScoped;

    return ((RwCamera*(__cdecl *)(RwCamera*))0x7EE180)(camera);
}

RwCamera* RwCameraClear(RwCamera* camera, RwRGBA* colour, RwInt32 clearMode) {
    return ((RwCamera*(__cdecl *)(RwCamera*, RwRGBA*, RwInt32))0x7EE340)(camera, colour, clearMode);
}

RwCamera* RwCameraShowRaster(RwCamera* camera, void* pDev, RwUInt32 flags) {
    return ((RwCamera*(__cdecl *)(RwCamera*, void*, RwUInt32))0x7EE370)(camera, pDev, flags);
}

void RwCameraSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7EE0F0)(blockSize, numBlocksToPrealloc);
}

RwBool RwCameraDestroy(RwCamera* camera) {
    return ((RwBool(__cdecl *)(RwCamera*))0x7EE4B0)(camera);
}

RwCamera* RwCameraCreate() {
    return ((RwCamera*(__cdecl *)(void))0x7EE4F0)();
}

RwCamera* RwCameraClone(RwCamera* camera) {
    return ((RwCamera*(__cdecl *)(RwCamera*))0x7EF3B0)(camera);
}

RwCamera* RwCameraSetViewOffset(RwCamera* camera, const RwV2d* offset) {
    return ((RwCamera*(__cdecl *)(RwCamera*, const RwV2d*))0x7EE1A0)(camera, offset);
}

RwCamera* RwCameraSetViewWindow(RwCamera* camera, const RwV2d* viewWindow) {
    return ((RwCamera*(__cdecl *)(RwCamera*, const RwV2d*))0x7EE410)(camera, viewWindow);
}

RwCamera* RwCameraSetProjection(RwCamera* camera, RwCameraProjection projection) {
    return ((RwCamera*(__cdecl *)(RwCamera*, RwCameraProjection))0x7EE3A0)(camera, projection);
}

RwCamera* RwCameraSetNearClipPlane(RwCamera* camera, RwReal nearClip) {
    return ((RwCamera*(__cdecl *)(RwCamera*, RwReal))0x7EE1D0)(camera, nearClip);
}

RwCamera* RwCameraSetFarClipPlane(RwCamera* camera, RwReal farClip) {
    return ((RwCamera*(__cdecl *)(RwCamera*, RwReal))0x7EE2A0)(camera, farClip);
}

RwInt32 RwCameraRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return ((RwInt32(__cdecl *)(RwInt32, RwUInt32, RwPluginObjectConstructor, RwPluginObjectDestructor, RwPluginObjectCopy))0x7EE450)(size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RwCameraGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x7EE480)(pluginID);
}

RwBool RwCameraValidatePlugins(const RwCamera* camera) {
    return ((RwBool(__cdecl *)(const RwCamera*))0x7EE4A0)(camera);
}

RwFrustumTestResult RwCameraFrustumTestSphere(const RwCamera* camera, const RwSphere* sphere) {
    return ((RwFrustumTestResult(__cdecl *)(const RwCamera*, const RwSphere*))0x7EE2D0)(camera, sphere);
}

RwInt32 RwCameraRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x808C90)(pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 RwCameraSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkAlwaysCallBack))0x808CC0)(pluginID, alwaysCB);
}

RwUInt32 RwCameraStreamGetSize(const RwCamera* camera) {
    return ((RwUInt32(__cdecl *)(const RwCamera*))0x808CE0)(camera);
}

RwCamera* RwCameraStreamRead(RwStream* stream) {
    return ((RwCamera*(__cdecl *)(RwStream*))0x808DE0)(stream);
}

const RwCamera* RwCameraStreamWrite(const RwCamera* camera, RwStream* stream) {
    return ((const RwCamera*(__cdecl *)(const RwCamera*, RwStream*))0x808D00)(camera, stream);
}

RwCameraChunkInfo* RwCameraChunkInfoRead(RwStream* stream, RwCameraChunkInfo* cameraChunkInfo, RwInt32* bytesRead) {
    return ((RwCameraChunkInfo*(__cdecl *)(RwStream*, RwCameraChunkInfo*, RwInt32*))0x808EF0)(stream, cameraChunkInfo, bytesRead);
}

void _rwObjectHasFrameSetFrame(void *object, RwFrame *frame) {
    plugin::Call<0x804EF0, void*, RwFrame*>(object, frame);
}

void _rwObjectHasFrameReleaseFrame(void *object) {
    plugin::Call<0x804F40, void*>(object);
}
