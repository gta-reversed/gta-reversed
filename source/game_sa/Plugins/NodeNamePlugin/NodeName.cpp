#include "StdInc.h"

#include "NodeName.h"

#define NAME_LENGTH 23

auto& gNodeNamePluginOffset = StaticRef<RwInt32>(0xC87C5C);

/**
 * NodeName plugin structure
 *
 * @size 24
 */
typedef struct tNodeNamePlugin {
    RwChar name[NAME_LENGTH + 1];
} NodeNamePluginInstance;

void NodeNamePlugin::InjectHooks() {
    RH_ScopedNamespace(NodeNamePlugin);
    RH_ScopedCategory("Plugins");

    // TODO: These should be moved into the namespace instead of being global...
    RH_ScopedGlobalInstall(NodeNamePluginAttach, 0x72FAB0);
    RH_ScopedGlobalInstall(GetFrameNodeName, 0x72FB30);
    RH_ScopedGlobalInstall(SetFrameNodeName, 0x72FB00);
}

// internal
// 0x72F9C0
static void* NodeNameConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    if (gNodeNamePluginOffset > 0)
        RWPLUGINOFFSET(NodeNamePluginInstance, object, gNodeNamePluginOffset)->name[0] = '\0';

    return object;
}

// internal
// 0x72F9E0
static void* NodeNameDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return object;
}

// internal
// 0x72F9F0
static void* NodeNameCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    strncpy_s(RWPLUGINOFFSET(NodeNamePluginInstance, dstObject, gNodeNamePluginOffset)->name, RWPLUGINOFFSET(NodeNamePluginInstance, srcObject, gNodeNamePluginOffset)->name, NAME_LENGTH);
    return dstObject;
}

// internal
// 0x72FA50
static RwStream* NodeNameStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    RwStreamRead(stream, RWPLUGINOFFSET(NodeNamePluginInstance, object, gNodeNamePluginOffset), binaryLength);
    RWPLUGINOFFSET(NodeNamePluginInstance, object, gNodeNamePluginOffset)->name[binaryLength] = 0;
    return stream;
}

// internal
// 0x72FA20
static RwStream* NodeNameStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    RwStreamWrite(stream, RWPLUGINOFFSET(NodeNamePluginInstance, object, gNodeNamePluginOffset), binaryLength);
    return stream;
}

// internal
// 0x72FA80
static RwInt32 NodeNameStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    if (RWPLUGINOFFSET(NodeNamePluginInstance, object, gNodeNamePluginOffset))
        return strlen(RWPLUGINOFFSET(NodeNamePluginInstance, object, gNodeNamePluginOffset)->name);

    return 0;
}

RwBool NodeNamePluginAttach() {
    // register plugin
    gNodeNamePluginOffset = RwFrameRegisterPlugin(sizeof(NodeNamePluginInstance), rwID_NODENAMEPLUGIN, NodeNameConstructor, NodeNameDestructor, NodeNameCopy);

    // register stream operations
    RwFrameRegisterPluginStream(rwID_NODENAMEPLUGIN, NodeNameStreamRead, NodeNameStreamWrite, NodeNameStreamGetSize);

    return gNodeNamePluginOffset != -1;
}

const RwChar* GetFrameNodeName(RwFrame* frame) {
    if (gNodeNamePluginOffset > 0)
        return RWPLUGINOFFSET(NodeNamePluginInstance, frame, gNodeNamePluginOffset)->name;

    return nullptr;
}

void SetFrameNodeName(RwFrame* frame, const RwChar* name) {
    if (gNodeNamePluginOffset > 0) {
        strncpy_s(RWPLUGINOFFSET(NodeNamePluginInstance, frame, gNodeNamePluginOffset)->name, name, NAME_LENGTH);
        RWPLUGINOFFSET(NodeNamePluginInstance, frame, gNodeNamePluginOffset)->name[NAME_LENGTH] = '\0';
    }
}
