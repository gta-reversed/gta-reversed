#include "StdInc.h"

#include "TxdStore.h"

void* TxdParentConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
void* TxdParentDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
void* TxdParentCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);

// 0x731650
bool CTxdStore::PluginAttach() {
    ms_txdPluginOffset = RwTexDictionaryRegisterPlugin(
        sizeof(_TxdParent),
        rwID_TXDPARENTPLUGIN,
        TxdParentConstructor,
        TxdParentDestructor,
        TxdParentCopyConstructor
    );
    return ms_txdPluginOffset != -1;
}

// 0x731600
static void* TxdParentConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    RWPLUGINOFFSET(_TxdParent, object, offsetInObject)->parent = nullptr;
    return object;
}

// 0x731620
static void* TxdParentDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return object;
}

// 0x731630
static void* TxdParentCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    RWPLUGINOFFSET(_TxdParent, dstObject, ms_txdPluginOffset)->parent = RWPLUGINOFFSET(_TxdParent, srcObject, ms_txdPluginOffset)->parent;
    return dstObject;
}

RwTexDictionary* CTxdStore::GetTxdParent(RwTexDictionary* txd) {
    return RWPLUGINOFFSET(_TxdParent, txd, ms_txdPluginOffset)->parent;
}

void CTxdStore::SetTxdParent(RwTexDictionary* txd, RwTexDictionary* parent) {
    RWPLUGINOFFSET(_TxdParent, txd, ms_txdPluginOffset)->parent = parent;
}
