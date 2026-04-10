#include "StdInc.h"

#include "PipelinePlugin.h"

#include <rpworld.h>

auto& gPipelinePluginOffset = StaticRef<RwInt32>(0x8D6080);


void PipelinePlugin::InjectHooks() {
    RH_ScopedNamespace(PipelinePlugin);
    RH_ScopedCategory("Plugins");

    // TODO: Move this into the namespace PipelinePlugin
    RH_ScopedGlobalInstall(PipelinePluginAttach, 0x72FBD0);
    RH_ScopedGlobalInstall(GetPipelineID, 0x72FC40);
    RH_ScopedGlobalInstall(SetPipelineID, 0x72FC50);
}

// internal
typedef struct tPipelinePlugin {
    RwUInt32 pipelineId;
} PipelinePluginInstance;

VALIDATE_SIZE(tPipelinePlugin, 0x4);

// internal
// 0x72FB50
static void* PipelineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    if (gPipelinePluginOffset > 0) {
        RWPLUGINOFFSET(tPipelinePlugin, object, offsetInObject)->pipelineId = 0;
    }
    return object;
}

// internal
// 0x72FB70
static void* PipelineCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    RWPLUGINOFFSET(tPipelinePlugin, dstObject, offsetInObject)->pipelineId = RWPLUGINOFFSET(tPipelinePlugin, srcObject, offsetInObject)->pipelineId;
    return dstObject;
}

// internal
// 0x72FB90
static RwStream* PipelineStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    RwStreamRead(stream, RWPLUGINOFFSET(tPipelinePlugin, object, gPipelinePluginOffset), binaryLength);
    return stream;
}

// internal
// 0x72FBC0
static RwInt32 PipelineGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return sizeof(tPipelinePlugin);
}

// internal
// we need this shit as RW requires write callback
static RwStream* PipelineStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return stream;
}

// 0x72FBD0
RwBool PipelinePluginAttach() {
    gPipelinePluginOffset = RpAtomicRegisterPlugin(sizeof(tPipelinePlugin), rwID_PIPELINEPLUGIN, PipelineConstructor, nullptr, PipelineCopy);
    if (gPipelinePluginOffset == -1) {
        return FALSE;
    }
    if (RpAtomicRegisterPluginStream(rwID_PIPELINEPLUGIN, PipelineStreamRead, PipelineStreamWrite, PipelineGetSize) == -1) {
        gPipelinePluginOffset = -1;
        return FALSE;
    }
    return TRUE;
}

// 0x72FC40
RwUInt32 GetPipelineID(RpAtomic* atomic) {
    return RWPLUGINOFFSET(tPipelinePlugin, atomic, gPipelinePluginOffset)->pipelineId;
}

// 0x72FC50
void SetPipelineID(RpAtomic* atomic, RwUInt32 pipelineId) {
    RWPLUGINOFFSET(tPipelinePlugin, atomic, gPipelinePluginOffset)->pipelineId = pipelineId;
}
