/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "TxdStore.h"

CTxdPool*& CTxdStore::ms_pTxdPool = *reinterpret_cast<CTxdPool**>(0xC8800C);
RwTexDictionary*& CTxdStore::ms_pStoredTxd = *reinterpret_cast<RwTexDictionary**>(0xC88010);
int32& CTxdStore::ms_lastSlotFound = *reinterpret_cast<int32*>(0xC88014);

int16 (&CTxdStore::defaultTxds)[4] = *reinterpret_cast<int16 (*)[4]>(0xC88004);

// variables list is not finished. Need to make CPools before.

void CTxdStore::InjectHooks() {
    RH_ScopedClass(CTxdStore);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(PushCurrentTxd, 0x7316A0);
    RH_ScopedInstall(PopCurrentTxd, 0x7316B0);
    RH_ScopedOverloadedInstall(FindTxdSlot, "name", 0x731850, int32 (*)(const char*));
    RH_ScopedOverloadedInstall(FindTxdSlot, "hash", 0x7318E0, int32 (*)(uint32));
    RH_ScopedInstall(StartLoadTxd, 0x731930);
    RH_ScopedInstall(Create, 0x731990);
    RH_ScopedInstall(SetCurrentTxd, 0x7319C0);
    RH_ScopedInstall(AddRef, 0x731A00);
    RH_ScopedInstall(RemoveRef, 0x731A30);
    RH_ScopedInstall(RemoveRefWithoutDelete, 0x731A70);
    RH_ScopedInstall(GetNumRefs, 0x731AA0);
    RH_ScopedInstall(AddTxdSlot, 0x731C80);
    RH_ScopedInstall(RemoveTxdSlot, 0x731CD0);
    RH_ScopedOverloadedInstall(LoadTxd, "stream", 0x731DD0, bool (*)(int32, RwStream*));
    RH_ScopedOverloadedInstall(LoadTxd, "filename", 0x7320B0, bool (*)(int32, const char*));
    RH_ScopedInstall(FinishLoadTxd, 0x731E40);
    RH_ScopedInstall(RemoveTxd, 0x731E90);
    RH_ScopedInstall(Initialise, 0x731F20);
    RH_ScopedInstall(Shutdown, 0x732000);
    RH_ScopedInstall(GameShutdown, 0x732060);
    RH_ScopedInstall(GetParentTxdSlot, 0x408370);
    RH_ScopedInstall(GetTxd, 0x408340);
    RH_ScopedInstall(TxdStoreFindCB, 0x731720);
    RH_ScopedInstall(TxdStoreLoadCB, 0x731710);
    RH_ScopedInstall(SetupTxdParent, 0x731D50);

    // global
    RH_ScopedGlobalInstall(RemoveIfRefCountIsGreaterThanOne, 0x731680);
}

// initialise txd store
// 0x731F20
void CTxdStore::Initialise() {
    if (!ms_pTxdPool)
        ms_pTxdPool = new CTxdPool(TOTAL_TXD_MODEL_IDS, "TexDictionary");

    for (auto& txd : defaultTxds)
        txd = static_cast<int16>(AddTxdSlot("*"));

    RwTextureSetFindCallBack(TxdStoreFindCB);
    RwTextureSetReadCallBack(TxdStoreLoadCB);
}

// shutdown txd store
// 0x732000
void CTxdStore::Shutdown() {
    delete ms_pTxdPool;
}

// 0x732060
void CTxdStore::GameShutdown() {
    for (int32 i = 0; i < TOTAL_TXD_MODEL_IDS; i++) {
        TxdDef* txd = ms_pTxdPool->GetAt(i);
        if (!txd || txd->m_wRefsCount)
            continue;
        RemoveTxdSlot(i);
    }
}

// 0x731930
bool CTxdStore::StartLoadTxd(int32 index, RwStream* stream) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!txd || !RwStreamFindChunk(stream, rwID_TEXDICTIONARY, nullptr, nullptr))
        return false;

    RwTexDictionary* texdic = RwTexDictionaryGtaStreamRead1(stream);
    txd->m_pRwDictionary = texdic;
    return texdic != nullptr;
}

// 0x731E40
bool CTxdStore::FinishLoadTxd(int32 index, RwStream* stream) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!txd)
        return false;

    txd->m_pRwDictionary = RwTexDictionaryGtaStreamRead2(stream, txd->m_pRwDictionary);
    if (txd->m_pRwDictionary)
        SetupTxdParent(index);
    return txd->m_pRwDictionary != nullptr;
}

// load txd from stream
// 0x731DD0
bool CTxdStore::LoadTxd(int32 index, RwStream* stream) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!txd || !RwStreamFindChunk(stream, rwID_TEXDICTIONARY, nullptr, nullptr))
        return false;
    txd->m_pRwDictionary = RwTexDictionaryGtaStreamRead(stream);
    if (txd->m_pRwDictionary)
        SetupTxdParent(index);
    return txd->m_pRwDictionary != nullptr;
}

// load txd from file
// 0x7320B0
bool CTxdStore::LoadTxd(int32 index, const char* filename) {
    ZoneScoped;

    char data[260];
    RwStream* stream;
    sprintf_s(data, "%s", filename);
    do {
        stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, data);
    } while (!stream);
    bool loaded = LoadTxd(index, stream);
    RwStreamClose(stream, nullptr);
    return loaded;
}

// store current txd
// 0x7316A0
void CTxdStore::PushCurrentTxd() {
    ms_pStoredTxd = RwTexDictionaryGetCurrent();
}

// restore txd
// 0x7316B0
void CTxdStore::PopCurrentTxd() {
    RwTexDictionarySetCurrent(ms_pStoredTxd);
    ms_pStoredTxd = nullptr;
}

// set the txd to work with
// 0x7319C0
void CTxdStore::SetCurrentTxd(int32 index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (txd)
        RwTexDictionarySetCurrent(txd->m_pRwDictionary);
}

// find txd by name. Returning value is txd index
// 0x731850
int32 CTxdStore::FindTxdSlot(const char* name) {
    uint32 key = CKeyGen::GetUppercaseKey(name);
    int32 last = ms_lastSlotFound;

    for (; last >= 0; last--) {
        TxdDef* txd = ms_pTxdPool->GetAt(last);
        if (txd && txd->m_hash == key)
            break;
    }

    if (last < 0) {
        last = ms_lastSlotFound;
        for (last++;; last++) {
            if (last >= 0 && static_cast<size_t>(last) >= ms_pTxdPool->GetSize())
                return -1;

            TxdDef* txd = ms_pTxdPool->GetAt(last);
            if (txd && txd->m_hash == key)
                break;
        }
    }

    ms_lastSlotFound = last;
    return last;
}

// find txd by name hash. Returning value is txd index
// 0x7318E0
int32 CTxdStore::FindTxdSlot(uint32 hash) {
    for (auto&& [i, txd] : ms_pTxdPool->GetAllValidWithIndex()) {
        if (txd.m_hash == hash)
            return i;
    }
    return -1;
}

// plugin-sdk has named it to getTexDictionary
// 0x408340
RwTexDictionary* CTxdStore::GetTxd(int32 index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    return txd ? txd->m_pRwDictionary : nullptr;
}

// plugin-sdk has named it to getTXDEntryParentIndex
// 0x408370
int32 CTxdStore::GetParentTxdSlot(int32 index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    return txd ? txd->m_wParentIndex : -1;
}

// create rw tex dictionary for txd slot
// 0x731990
void CTxdStore::Create(int32 index) {
    NOTSA_LOG_DEBUG("CTxdStore::Create({})", index);
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (txd)
        txd->m_pRwDictionary = RwTexDictionaryCreate();
}

// allocate new slot for this txd
// 0x731C80
int32 CTxdStore::AddTxdSlot(const char* name) {
    TxdDef* txd = ms_pTxdPool->New();
    txd->m_pRwDictionary = nullptr;
    txd->m_wRefsCount = 0;
    txd->m_wParentIndex = -1;
    txd->m_hash = CKeyGen::GetUppercaseKey(name);

    const auto i = ms_pTxdPool->GetIndex(txd);
    NOTSA_LOG_TRACE("CTxdStore::AddTxdSlot({:?}) -> {}", name, i);
    return i;
}

// remove txd slot
// 0x731CD0
void CTxdStore::RemoveTxdSlot(int32 index) {
    NOTSA_LOG_TRACE("CTxdStore::RemoveTxdSlot({})", index);
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!txd)
        return;

    if (txd->m_pRwDictionary)
        RwTexDictionaryDestroy(txd->m_pRwDictionary);
    ms_pTxdPool->Delete(txd);
}

// remove txd
// 0x731E90
void CTxdStore::RemoveTxd(int32 index) {
    NOTSA_LOG_DEBUG("CTxdStore::RemoveTxd({})", index);
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!txd)
        return;

    if (txd->m_pRwDictionary) {
        RwTexDictionaryForAllTextures(txd->m_pRwDictionary, RemoveIfRefCountIsGreaterThanOne, nullptr);
        RwTexDictionaryDestroy(txd->m_pRwDictionary);
    }
    if (txd->m_wParentIndex != -1)
        RemoveRef(txd->m_wParentIndex);

    txd->m_pRwDictionary = nullptr;
}

// add reference for txd
// 0x731A00
void CTxdStore::AddRef(int32 index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (txd)
        txd->m_wRefsCount++;
}

// remove reference, if references count is 0, then delete txd
// 0x731A30
void CTxdStore::RemoveRef(int32 index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!txd)
        return;

    if (--txd->m_wRefsCount <= 0)
        CStreaming::RemoveModel(TXDToModelId(index));
}

// remove reference without deleting
// 0x731A70
void CTxdStore::RemoveRefWithoutDelete(int32 index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (txd)
        txd->m_wRefsCount--;
}

// get references count
// 0x731AA0
int32 CTxdStore::GetNumRefs(int32 index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    return txd ? txd->m_wRefsCount : 0;
}

// 0x731D50
void CTxdStore::SetupTxdParent(int32 index) {
    TxdDef* txdcur = ms_pTxdPool->GetAt(index);
    if (!txdcur || txdcur->m_wParentIndex == -1)
        return;
    TxdDef* txdpar = ms_pTxdPool->GetAt(txdcur->m_wParentIndex);
    if (!txdpar)
        return;

    SetTxdParent(txdcur->m_pRwDictionary, txdpar->m_pRwDictionary);
    AddRef(txdcur->m_wParentIndex);
}

// 0x731720
RwTexture* CTxdStore::TxdStoreFindCB(const char* name) {
    RwTexDictionary* txd = RwTexDictionaryGetCurrent();
    for (; txd; txd = GetTxdParent(txd)) {
        RwTexture* tex = RwTexDictionaryFindNamedTexture(txd, name);
        if (tex)
            return tex;
    }
    return nullptr;
}

// 0x731710
RwTexture* CTxdStore::TxdStoreLoadCB(const char* name, const char* mask) {
    return nullptr;
}

// 0x731680
RwTexture* RemoveIfRefCountIsGreaterThanOne(RwTexture* texture, void* data) {
    if (texture->refCount > 1) {
        RwTextureDestroy(texture);
        RwTexDictionaryRemoveTexture(texture);
    }
    return texture;
}
