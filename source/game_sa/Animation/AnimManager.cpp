/*
Plugin-SDK file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include <extensions/ci_string.hpp>

#include "AnimManager.h"
#include "AnimAssocDescriptions.h"

void CAnimManager::InjectHooks() {
    RH_ScopedClass(CAnimManager);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Initialise, 0x5BF6B0);
    RH_ScopedInstall(ReadAnimAssociationDefinitions, 0x5BC910);
    RH_ScopedInstall(Shutdown, 0x4D4130);
    RH_ScopedOverloadedInstall(GetAnimationBlock, "", 0x4D3940, CAnimBlock*(*)(const char*));
    RH_ScopedOverloadedInstall(GetAnimationBlockIndex, "by-name", 0x4D3990, int32(*)(const char*));
    RH_ScopedInstall(GetFirstAssocGroup, 0x4D39B0);
    RH_ScopedOverloadedInstall(GetAnimation, "0", 0x4D39F0, CAnimBlendHierarchy*(*)(uint32, const CAnimBlock*));
    RH_ScopedOverloadedInstall(GetAnimation, "1", 0x4D42F0, CAnimBlendHierarchy*(*)(const char*, const CAnimBlock*));
    RH_ScopedInstall(GetAnimGroupName, 0x4D3A20);
    RH_ScopedInstall(GetAnimBlockName, 0x4D3A30);
    RH_ScopedInstall(CreateAnimAssociation, 0x4D3A40);
    RH_ScopedOverloadedInstall(GetAnimAssociation, "by-id", 0x4D3A60, CAnimBlendStaticAssociation*(*)(AssocGroupId, AnimationId));
    RH_ScopedOverloadedInstall(GetAnimAssociation, "by-name", 0x4D3A80, CAnimBlendStaticAssociation*(*)(AssocGroupId, const char*));
    RH_ScopedOverloadedInstall(AddAnimation, "id", 0x4D3AA0, CAnimBlendAssociation*(*)(RpClump*, AssocGroupId, AnimationId));
    RH_ScopedOverloadedInstall(AddAnimation, "hier", 0x4D4330, CAnimBlendAssociation*(*)(RpClump*, CAnimBlendHierarchy*, int32));
    RH_ScopedInstall(AddAnimationAndSync, 0x4D3B30);
    RH_ScopedInstall(AddAnimAssocDefinition, 0x4D3BA0);
    RH_ScopedInstall(AddAnimToAssocDefinition, 0x4D3C80);
    RH_ScopedInstall(CreateAnimAssocGroups, 0x4D3CC0);
    RH_ScopedInstall(RegisterAnimBlock, 0x4D3E50);
    RH_ScopedInstall(RemoveLastAnimFile, 0x4D3ED0);
    RH_ScopedInstall(RemoveAnimBlock, 0x4D3F40);
    RH_ScopedInstall(AddAnimBlockRef, 0x4D3FB0);
    RH_ScopedInstall(RemoveAnimBlockRefWithoutDelete, 0x4D3FF0);
    RH_ScopedInstall(GetNumRefsToAnimBlock, 0x4D4010);
    RH_ScopedInstall(UncompressAnimation, 0x4D41C0);
    RH_ScopedInstall(RemoveFromUncompressedCache, 0x4D42A0);
    RH_ScopedOverloadedInstall(BlendAnimation, "id", 0x4D4610, CAnimBlendAssociation*(*)(RpClump*, AssocGroupId, AnimationId, float));
    RH_ScopedOverloadedInstall(BlendAnimation, "hier", 0x4D4410, CAnimBlendAssociation*(*)(RpClump*, CAnimBlendHierarchy*, int32, float));
    RH_ScopedInstall(LoadAnimFiles, 0x4D5620);
    RH_ScopedInstall(LoadAnimFile, 0x4D47F0);
}

// 0x5BF6B0
void CAnimManager::Initialise() {
    ZoneScoped;

    ms_numAnimations = 0;
    ms_numAnimBlocks = 0;
    ms_numAnimAssocDefinitions = 118; // ANIM_TOTAL_GROUPS aka NUM_ANIM_ASSOC_GROUPS
    ms_AnimCache.Init(50);
    ReadAnimAssociationDefinitions();
    RegisterAnimBlock("ped");
}

// 0x5BC910
void CAnimManager::ReadAnimAssociationDefinitions() {
    char groupName[32], blockName[32], type[32];
    bool                 isAnimSection = false;
    AnimAssocDefinition* def;
    uint32               animCount;

    CFileMgr::SetDir("");
    const auto f = CFileMgr::OpenFile("DATA\\ANIMGRP.DAT", "rb");
    for (;;) {
        const auto l = CFileLoader::LoadLine(f);
        if (!l) {
            break;
        }
        if (!*l || *l == '#') {
            continue;
        }
        if (isAnimSection) {
            if (sscanf_s(l, "%s", SCANF_S_STR(groupName)) == 1) {
                if (!memcmp(groupName, "end", 4)) {
                    isAnimSection = false;
                } else {
                    AddAnimToAssocDefinition(def, groupName);
                }
            }
        } else {
            VERIFY(sscanf_s(l, "%s %s %s %d", SCANF_S_STR(groupName), SCANF_S_STR(blockName), SCANF_S_STR(type), &animCount) == 4);
            def = AddAnimAssocDefinition(groupName, blockName, MODEL_MALE01, animCount, aStdAnimDescs);
            isAnimSection = true;
        }
    }
    CFileMgr::CloseFile(f);
}

// 0x4D4130
void CAnimManager::Shutdown() {
    for (auto i = 0; i < NUM_ANIM_BLOCKS; i++) {
        CStreaming::RemoveModel(IFPToModelId(i));
    }

    for (auto i = 0; i < ms_numAnimations; i++) {
        ms_aAnimations[i].Shutdown();
    }

    ms_AnimCache.Shutdown();
    delete[] ms_aAnimAssocGroups;
}

CAnimBlock* CAnimManager::GetAnimationBlock(AssocGroupId animGroup) {
    return GetAssocGroups()[animGroup].m_AnimBlock;
}

// 0x4D3940
CAnimBlock* CAnimManager::GetAnimationBlock(const char* name) {
    const auto namesv = notsa::ci_string_view{ name };
    for (auto& ab : GetAnimBlocks()) {
        if (namesv == ab.Name) {
            return &ab;
        }
    }
    return nullptr;
}

int32 CAnimManager::GetAnimationBlockIndex(AssocGroupId animGroup) {
    return GetAnimationBlock(animGroup) - ms_aAnimBlocks.data();
}

int32 CAnimManager::GetAnimationBlockIndex(CAnimBlock* animBlock) {
    return animBlock - ms_aAnimBlocks.data();
}

// 0x4D3990
int32 CAnimManager::GetAnimationBlockIndex(const char* name) {
    const auto b = GetAnimationBlock(name);
    return b
        ? (int32)(std::distance(ms_aAnimBlocks.data(), b))
        : -1;
}

// 0x4D39B0
AssocGroupId CAnimManager::GetFirstAssocGroup(const char* name) {
    const auto namesv = notsa::ci_string_view{ name };
    for (auto i = 0; i < ANIM_GROUP_MAN; i++) {
        if (ms_aAnimAssocDefinitions[i].BlockName == namesv) {
            return static_cast<AssocGroupId>(i);
        }
    }
    return ANIM_GROUP_MAN;
}

// 0x4D39F0
CAnimBlendHierarchy* CAnimManager::GetAnimation(uint32 hash, const CAnimBlock* animBlock) {
    auto h = &ms_aAnimations[animBlock->FirstAnimIdx];
    for (auto i = animBlock->NumAnims; i-- > 0; h++) {
        if (h->m_hashKey == hash) {
            return h;
        }
    }
    return nullptr;
}

// 0x4D42F0
CAnimBlendHierarchy* CAnimManager::GetAnimation(const char* animName, const CAnimBlock* animBlock) {
    return GetAnimation(CKeyGen::GetUppercaseKey(animName), animBlock);
}

// notsa
CAnimBlendHierarchy& CAnimManager::GetAnimation(AnimationId id) {
    return ms_aAnimations[(size_t)id];
}

// 0x4D3A20
const char* CAnimManager::GetAnimGroupName(AssocGroupId groupId) {
    return ms_aAnimAssocDefinitions[groupId].GroupName;
}

// 0x4D3A30
const char* CAnimManager::GetAnimBlockName(AssocGroupId groupId) {
    return ms_aAnimAssocDefinitions[groupId].BlockName;
}

// NOTSA
AssocGroupId CAnimManager::GetAnimationGroupIdByName(notsa::ci_string_view name) {
    for (const auto& [i, gd] : notsa::enumerate(GetAssocGroupDefs())) {
        if (gd.GroupName == name) {
            return (AssocGroupId)i;
        }
    }
    return AssocGroupId::ANIM_GROUP_DEFAULT;
}

// 0x4D3A40
CAnimBlendAssociation* CAnimManager::CreateAnimAssociation(AssocGroupId groupId, AnimationId animId) {
    return GetAssocGroups()[groupId].CopyAnimation(animId);
}

// 0x4D3A60
CAnimBlendStaticAssociation* CAnimManager::GetAnimAssociation(AssocGroupId groupId, AnimationId animId) {
    return GetAssocGroups()[groupId].GetAnimation(animId);
}

// 0x4D3A80
CAnimBlendStaticAssociation* CAnimManager::GetAnimAssociation(AssocGroupId groupId, const char* animName) {
    return GetAssocGroups()[groupId].GetAnimation(animName);
}

CAnimBlendAssociation* CAnimManager::AddAnimationToClump(RpClump* clump, CAnimBlendAssociation* anim) {
    const auto clumpAnims = &RpAnimBlendClumpGetData(clump)->m_AnimList;

    CAnimBlendAssociation* syncWith{};
    if (anim->IsSyncronised()) {
        for (auto l = clumpAnims->next; l; l = l->next) {
            const auto a = CAnimBlendAssociation::FromLink(l);
            if (a->IsSyncronised()) {
                syncWith = a;
                break;
            }
        }
    }

    if (syncWith) {
        anim->SyncAnimation(syncWith);
        anim->m_Flags |= ANIMATION_IS_PLAYING;
    } else {
        anim->Start(0.0f);
    }

    clumpAnims->Prepend(&anim->m_Link);

    return anim;
}

// 0x4D3AA0
CAnimBlendAssociation* CAnimManager::AddAnimation(RpClump* clump, AssocGroupId groupId, AnimationId animId) {
    return AddAnimationToClump(clump, CreateAnimAssociation(groupId, animId));
}

// 0x4D4330
CAnimBlendAssociation* CAnimManager::AddAnimation(RpClump* clump, CAnimBlendHierarchy* hier, int32 clumpAssocFlag) {
    const auto anim = new CAnimBlendAssociation(clump, hier);
    anim->m_Flags |= clumpAssocFlag;
    anim->ReferenceAnimBlock();
    UncompressAnimation(hier);
    return AddAnimationToClump(clump, anim);
}

// 0x4D3B30
CAnimBlendAssociation* CAnimManager::AddAnimationAndSync(RpClump* clump, CAnimBlendAssociation* syncWith, AssocGroupId groupId, AnimationId animId) {
    const auto a = CreateAnimAssociation(groupId, animId);
    if (a->IsSyncronised() && syncWith) {
        a->SyncAnimation(syncWith);
        a->m_Flags |= ANIMATION_IS_PLAYING;
    } else {
        a->Start(0.0f);
    }

    RpAnimBlendClumpGetData(clump)->m_AnimList.Prepend(&a->m_Link);

    return a;
}

// 0x4D3BA0
AnimAssocDefinition* CAnimManager::AddAnimAssocDefinition(const char* groupName, const char* blockName, uint32 modelIndex, uint32 animsCount, AnimDescriptor* descriptor) {
    const auto def = &ms_aAnimAssocDefinitions[ms_numAnimAssocDefinitions++];

    strcpy_s(def->GroupName, groupName);
    strcpy_s(def->BlockName, blockName);

    def->ModelIndex = modelIndex;
    def->NumAnims   = animsCount;
    def->AnimDescr  = descriptor;

    def->AnimNames   = new const char*[animsCount]; // This is so incredibly retarded there are no words for it....
    const auto bufsz = AnimAssocDefinition::ANIM_NAME_BUF_SZ * animsCount;
    const auto buf   = new char[bufsz];
    memset(buf, 0, bufsz);
    for (auto i = animsCount; i-->0;) {
        def->AnimNames[i] = buf + i * AnimAssocDefinition::ANIM_NAME_BUF_SZ;
    }

    return def;
}

// 0x4D3C80
void CAnimManager::AddAnimToAssocDefinition(AnimAssocDefinition* def, const char* animName) {
    int32 i = 0;
    for (; def->AnimNames[i][0]; i++) {
        assert(i < def->NumAnims);
    }
    // `const_cast` is fine here, because it's heap allocated [presumeably]
    strcpy_s(const_cast<char*>(def->AnimNames[i]), AnimAssocDefinition::ANIM_NAME_BUF_SZ, animName);
}

// 0x4D3CC0
void CAnimManager::CreateAnimAssocGroups() {
    for (auto&& [i, group] : notsa::enumerate(GetAssocGroups())) {
        const auto def   = &ms_aAnimAssocDefinitions[i];
        const auto block = GetAnimationBlock(def->BlockName);
        if (block == nullptr || !block->IsLoaded || group.m_Anims) {
            continue;
        }

        RpClump* clump = nullptr;
        if (def->ModelIndex != MODEL_INVALID) {
            clump = (RpClump*)CModelInfo::GetModelInfo(def->ModelIndex)->CreateInstance();
            RpAnimBlendClumpInit(clump);
        }

        group.m_GroupID = (AssocGroupId)i;
        group.m_IdOffset = def->AnimDescr->AnimId;
        group.CreateAssociations(def->BlockName, clump, def->AnimNames, def->NumAnims);
        for (auto j = 0u; j < group.m_NumAnims; j++) {
            group.GetAnimation(def->AnimDescr[j].AnimId)->m_Flags |= def->AnimDescr[j].Flags;
        }

        if (clump) {
#ifdef SA_SKINNED_PEDS
            if (IsClumpSkinned(clump)) {
                RpClumpForAllAtomics(clump, AtomicRemoveAnimFromSkinCB, nullptr);
            }
#endif
            RpClumpDestroy(clump);
        }
    }
}

// 0x4D3E50
int32 CAnimManager::RegisterAnimBlock(const char* name) {
    CAnimBlock* ab = GetAnimationBlock(name);
    if (ab == nullptr) { // Initialize a new anim block
        ab = &ms_aAnimBlocks[ms_numAnimBlocks++];
        strncpy_s(ab->Name, name, MAX_ANIM_BLOCK_NAME);
        ab->NumAnims = 0;
        ab->GroupId = GetFirstAssocGroup(name);
        assert(ab->RefCnt == 0);
    }

    return GetAnimationBlockIndex(ab);
}

// 0x4D3ED0
void CAnimManager::RemoveLastAnimFile() {
    const auto ab = &GetAnimBlocks()[--ms_numAnimBlocks];
    ms_numAnimations = ab->FirstAnimIdx;
    for (auto i = 0u; i < ab->NumAnims; i++) { // Remove related animations too
        ms_aAnimations[ab->FirstAnimIdx + i].Shutdown();
    }
    ab->IsLoaded = false;
}

// 0x4D3F40
void CAnimManager::RemoveAnimBlock(int32 index) {
    const auto ab = &GetAnimBlocks()[index];

    for (auto& g : GetAssocGroups()) {
        if (g.m_AnimBlock == ab) {
            g.DestroyAssociations();
        }
    }

    for (auto i = 0u; i < ab->NumAnims; i++) { // Remove related animations too
        ms_aAnimations[ab->FirstAnimIdx + i].Shutdown();
    }

    ab->IsLoaded = false;
    ab->RefCnt  = 0;
}

// 0x4D3FB0
void CAnimManager::AddAnimBlockRef(int32 index) {
    GetAnimBlocks()[index].RefCnt++;
}

// 0x4D3FD0
void CAnimManager::RemoveAnimBlockRef(int32 index) {
    GetAnimBlocks()[index].RefCnt--;
    /* see RemoveAnimBlockRefWithoutDelete, logically here should be called RemoveModel or something
    if (--ms_aAnimBlocks[index].usRefs == 0) {
    CStreaming::RemoveModel(IFPToModelId(index));
    }
    */
}

// 0x4D3FF0
void CAnimManager::RemoveAnimBlockRefWithoutDelete(int32 index) {
    ms_aAnimBlocks[index].RefCnt--;
}

// 0x4D4010
int32 CAnimManager::GetNumRefsToAnimBlock(int32 index) {
    return ms_aAnimBlocks[index].RefCnt;
}

// 0x4D41C0
void CAnimManager::UncompressAnimation(CAnimBlendHierarchy* h) {
    if (h->IsRunningCompressed()) { // Keep as compressed?
        if (h->GetTotalTime() == 0.f) {
            h->CalcTotalTimeCompressed();
        }
    } else if (!h->IsUncompressed()) { // Need to uncompress?
        assert(!h->m_Link); // Sanity check
        auto l = ms_AnimCache.Insert(h);
        if (!l) { // No more free links? (This is totally normal as animations aren't compressed back unless the cache is full)
            // Remove least recently used item
            const auto llr = ms_AnimCache.GetTail();
            llr->data->RemoveUncompressedData();

            // TODO: If the anim is still in use this will corrupt the animation data, and (hopefully) hit the assert in `GetKeyFrame`!
            //       There's currently no way for us to tell if the animation is in use, so there's no better solution for now.
            RemoveFromUncompressedCache(llr->data);

            // Now try again, this time it should succeed
            VERIFY(l = ms_AnimCache.Insert(h));
        }
        h->m_Link = l;
        h->Uncompress();
    } else if (h->m_Link) { // Already uncompressed, mark as recently-used in cache
        h->m_Link->Remove(); // Remove from current position
        ms_AnimCache.Insert(*h->m_Link); // Now re-insert at head
    }
}

// 0x4D42A0
void CAnimManager::RemoveFromUncompressedCache(CAnimBlendHierarchy* h) {
    if (const auto l = h->m_Link) {
        assert(l->data == h);
        ms_AnimCache.Remove(l);
        h->m_Link = nullptr;
    }
}

// 0x4D4410
CAnimBlendAssociation* CAnimManager::BlendAnimation(RpClump* clump, CAnimBlendHierarchy* toBlendHier, int32 toBlendFlags, float blendDelta) {
    const auto clumpAnimData = RpAnimBlendClumpGetData(clump); // Get running anim data

    CAnimBlendAssociation* running{}; // Running instance of this anim
    bool                   bFadeThisOut = false;
    for (auto l = clumpAnimData->m_AnimList.next; l; l = l->next) {
        const auto a = CAnimBlendAssociation::FromLink(l);
        assert(a->m_BlendHier);
        if (a->m_BlendHier && a->m_BlendHier == toBlendHier) { // Found an instance of this anim running
            running = a;
        } else if (((toBlendFlags & ANIMATION_IS_PARTIAL) != 0) == a->IsPartial()) {
            if (a->m_BlendAmount <= 0.f) {
                a->m_BlendDelta = -1.f;
            } else {
                const auto bd = a->GetBlendAmount() * -blendDelta;
                if ((toBlendFlags & ANIMATION_IS_PARTIAL) == 0 || bd <= a->GetBlendDelta() || (a->m_BlendHier->m_nAnimBlockId && a->m_BlendHier->m_nAnimBlockId == toBlendHier->m_nAnimBlockId)) {
                    a->m_BlendDelta = std::min(-0.05f, bd);
                }
            }
            
            a->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE);
            bFadeThisOut = true;
        }
    }

    // If already running just re-adjust blend delta (and re-start it if it has finished)
    if (running) {
        running->SetBlendDelta((1.f - running->m_BlendAmount) * blendDelta);
        if (running->HasFinished()) {
            running->Start();
        }
        UncompressAnimation(running->m_BlendHier); // Make sure anim doesn't get removed
        return running;
    }

    // Otherwise create new instance
    const auto a = new CAnimBlendAssociation{clump, toBlendHier};
    a->m_Flags = toBlendFlags;
    a->ReferenceAnimBlock();
    UncompressAnimation(a->m_BlendHier);
    clumpAnimData->m_AnimList.Prepend(&a->m_Link);
    a->Start();
    if (bFadeThisOut || (toBlendFlags & ANIMATION_IS_PARTIAL)) {
        a->SetBlend(0.f, blendDelta);
        UncompressAnimation(toBlendHier); // No need to call this again here, but doesn't hurt....
    } else {
        a->m_BlendAmount = 1.f;
    }
    return a;
}

// 0x4D4610
CAnimBlendAssociation* CAnimManager::BlendAnimation(RpClump* clump, AssocGroupId groupId, AnimationId animId, float blendDelta) {
    const auto clumpAnimData = RpAnimBlendClumpGetData(clump); // Get running anim data

    const auto toBlendAnim             = GetAssocGroups()[groupId].GetAnimation(animId);
    const bool toBlendIsMoving         = toBlendAnim->m_Flags & ANIMATION_IS_SYNCRONISED;
    const bool toBlendIsPartial        = toBlendAnim->m_Flags & ANIMATION_IS_PARTIAL;
    const bool toBlendIsIndestructible = toBlendAnim->m_Flags & ANIMATION_FACIAL;

    CAnimBlendAssociation *running{}, *movingAnim{}; // Running instance of this anim, and the last moving anim in the chain
    bool                   bFadeThisOut = false;
    for (auto l = clumpAnimData->m_AnimList.next; l; l = l->next) {
        const auto a = CAnimBlendAssociation::FromLink(l);

        if (toBlendIsMoving && a->IsSyncronised()) {
            movingAnim = a;
        }

        if (a->m_AnimId == animId && a->m_AnimGroupId == groupId) {
            running = a;
        } else if (toBlendIsPartial == a->IsPartial() && toBlendIsIndestructible == a->IsFacial()) {
            if (a->m_BlendAmount <= 0.f) {
                a->m_BlendDelta = -1.f;
            } else if (const auto bd = a->GetBlendAmount() * -blendDelta; bd <= a->GetBlendDelta() || !toBlendIsPartial) {
                a->m_BlendDelta = std::min(-0.05f, bd);
            }
            a->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE);
            bFadeThisOut = true;
        }
    }

    // If already running just re-adjust blend delta (and start it if it has finished)
    if (running) {
        running->SetBlendDelta((1.f - running->GetBlendAmount()) * blendDelta);
        if (running->HasFinished()) {
            running->Start();
        }
        UncompressAnimation(running->m_BlendHier);
        return running;
    }

    // Otherwise create new anim, and possibly sync it with the last moving anim of the clump
    const auto anim = AddAnimationAndSync(clump, movingAnim, groupId, animId);
    if (bFadeThisOut || toBlendIsPartial) {
        anim->SetBlend(0.f, blendDelta);
        UncompressAnimation(anim->m_BlendHier);
    } else {
        anim->m_BlendAmount = 1.f;
    }

    return anim;
}

//! @notsa
uint32 CAnimManager::GetAnimIndex(const CAnimBlendHierarchy* h) {
    const auto idx = h - ms_aAnimations.data();
    assert(idx >= 0 && idx <= ms_numAnimations);
    return (uint32)idx;
}

//! @notsa
bool CAnimManager::IsAnimInBlock(const CAnimBlendHierarchy* h, const CAnimBlock* b) {
    const auto animIdx = (int32)h->GetIndex();
    return animIdx >= b->FirstAnimIdx && animIdx <= b->FirstAnimIdx + (int32)b->NumAnims;
}

//! @notsa
//! @brief Function for the very common pattern found in many tasks. See xrefs to `CTaskComplexGangLeader::ShouldLoadGangAnims()`
//! @brief Usual usage would be: `StreamAnimBlock(m_animBlockName, CTaskComplexGangLeader::ShouldLoadGangAnims(), m_areAnimsReferenced)`
//! @return If the anims are now loaded or not
bool CAnimManager::StreamAnimBlock(const char* blck, bool shouldBeLoaded, bool isLoaded) {
    if (shouldBeLoaded && !isLoaded) {
        RemoveAnimBlockRef(GetAnimationBlockIndex(blck));
        return false;
    } else if (!shouldBeLoaded && isLoaded) {
        const auto blkIdx = GetAnimationBlockIndex(blck);
        if (GetAnimBlocks()[blkIdx].IsLoaded) {
            AddAnimBlockRef(blkIdx);
            return true;
        } else {
            CStreaming::RequestModel(IFPToModelId(blkIdx), STREAMING_KEEP_IN_MEMORY);
        }
    }
    return isLoaded; // Unchanged
}

// 0x4D5620
void CAnimManager::LoadAnimFiles() {
    RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, "ANIM\\PED.IFP");
    assert(stream);
    LoadAnimFile(stream, true);
    RwStreamClose(stream, nullptr);

    ms_aAnimAssocGroups = new CAnimBlendAssocGroup[ms_numAnimAssocDefinitions];
    CreateAnimAssocGroups();
}

struct IFPSectionHeader {
    union {
        uint32 ID;
        char   IDFourCC[4];
    };
    uint32 Size;
};

//! @notsa - Helper
auto CAnimManager::GetOrCreateAnimBlock(const char* name, uint32 numAnims) {
    CAnimBlock* ab;
    if (ab = GetAnimationBlock(name)) { // Block already exists, initialize it if necessary
        if (!ab->NumAnims) {
            ab->NumAnims     = numAnims;
            ab->FirstAnimIdx = ms_numAnimations;

            NOTSA_LOG_TRACE(
                "Initialized AnimBlock (Name: {}; NumAnims: {}; FirstAnimIdx: {}; GroupId: {})",
                ab->Name, ab->NumAnims, ab->FirstAnimIdx, (int32)ab->GroupId
            );
        }
    } else { // Create block
        ab = &ms_aAnimBlocks[ms_numAnimBlocks++];

        VERIFY(strncpy_s(ab->Name, name, MAX_ANIM_BLOCK_NAME) == 0);
        ab->NumAnims     = numAnims;
        ab->FirstAnimIdx = ms_numAnimations;
        ab->GroupId      = GetFirstAssocGroup(ab->Name);

        NOTSA_LOG_TRACE(
            "Created AnimBlock (Name: {}; NumAnims: {}; FirstAnimIdx: {}; GroupId: {})",
            ab->Name, ab->NumAnims, ab->FirstAnimIdx, (int32)ab->GroupId
        );
    }
    ab->IsLoaded = true;
    return std::make_tuple(ab, GetAnimationBlockIndex(ab));
}

// 0x4D47F0
void CAnimManager::LoadAnimFile(RwStream* stream, bool loadCompressed, char const(*uncompressedAnimations)[32]) {
    IFPSectionHeader h;
    RwStreamRead(stream, &h, sizeof(h));
    switch (h.ID) {
    case MakeFourCC("ANP3"):
    case MakeFourCC("ANP2"):
        return LoadAnimFile_ANP23(stream, h, loadCompressed, h.ID == MakeFourCC("ANP3"));
    case MakeFourCC("ANPK"):
        return LoadAnimFile_ANPK(stream, h, loadCompressed, uncompressedAnimations);
    default:
        NOTSA_UNREACHABLE("Unknown ANPK: {:x}", h.ID);
    }
}

namespace ANPK {
    template<size_t MaxNameLen>
    struct Info {
        uint32 Num;
        char   Name[MaxNameLen];
    };

    template<size_t MaxLen>
    struct TString {
        char Data[MaxLen];
    };

    //! Round number to 4 byte boundary
    auto RoundTo4(auto num) {
        return num % 4
            ? num + 4 - (num % 4)
            : num;
    }

    template<typename T>
    auto ReadSection(RwStream* stream) {
        auto h = RwStreamRead<IFPSectionHeader>(stream);
        h.Size = RoundTo4(h.Size);
        struct Ret { T v; IFPSectionHeader h; };
        return Ret{RwStreamRead<T>(stream, h.Size), h};
    }
};

// Code from 0x4D4863
// NOTE:
// This code was written based off the docs on gtamods
// I didn't manage to find any ANPK anims, thus this function is untested so far
inline void CAnimManager::LoadAnimFile_ANPK(RwStream* s, const IFPSectionHeader& h, bool bLoadCompressed, const char (*pLoadUncompressed)[32]) {
    using namespace ANPK;

    // Read ANPK section
    const auto hANPK    = RwStreamRead<IFPSectionHeader>(s);
    const auto infoAnim = RwStreamRead<Info<MAX_ANIM_BLOCK_NAME>>(s, RoundTo4(hANPK.Size)); // information : INFO<TAnimation>

    const auto [ablock, ablockIdx] = GetOrCreateAnimBlock(infoAnim.Name, infoAnim.Num);

    // Read TAnimation's for this block
    for (size_t animN = 0; animN < infoAnim.Num; animN++) {
        const auto hier = &ms_aAnimations[ablock->FirstAnimIdx + animN];

        /** TAnimation - Each entry represents an in-game animation
        * AnimName : NAME
        * AnimData : DGAN
        **/

        //
        // Read `TAnimation::AnimName`
        //
        {
            char name[64];
            const auto hAnimName = RwStreamRead<IFPSectionHeader>(s);
            assert(hAnimName.ID == MakeFourCC("NAME"));
            RwStreamRead(s, name, RoundTo4(hAnimName.Size));
            hier->SetName(name);
            NOTSA_LOG_DEBUG("Reading animation {}", name);
        }

        // Check if this animation is compressed
        bool isCompressed = bLoadCompressed;
        for (size_t k = 0; pLoadUncompressed[k][0]; k++) {
            if (CKeyGen::GetUppercaseKey(pLoadUncompressed[k]) == hier->GetHashKey()) {
                isCompressed = false;
                break; // fix_bugs
            }
        }
        hier->m_nAnimBlockId    = ablockIdx;
        hier->m_bIsCompressed   = isCompressed;
        hier->m_bKeepCompressed = false;

        //
        // Read `TAnimation::AnimData`
        //

        /** DGAN
        * AnimInfo : INFO<CPAN>
        **/

        const auto hAnimData = RwStreamRead<IFPSectionHeader>(s); // DGAN header
        assert(hAnimData.ID == MakeFourCC("DGAN"));

        //
        // Read `DGAN::AnimInfo`
        //
        const auto hAnimInfo = RwStreamRead<IFPSectionHeader>(s); // INFO header
        assert(hAnimInfo.ID == MakeFourCC("INFO"));
        const auto animInfo = RwStreamRead<Info<64>>(s, RoundTo4(hAnimInfo.Size)); // INFO data

        hier->m_nSeqCount  = animInfo.Num;
        hier->m_pSequences = new CAnimBlendSequence[hier->m_nSeqCount]; // Yes, they used `new`

        // Read `DGAN::AnimInfo` (CPAN) entries (The sequences of the animation)
        for (size_t seqN = 0; seqN < animInfo.Num; seqN++) {
            const auto seq = &hier->m_pSequences[seqN];

            /** CPAN
            * ObjectInfo : ANIM
            **/

            const auto hObjectInfo = RwStreamRead<IFPSectionHeader>(s); // CPAN header
            assert(hObjectInfo.ID == MakeFourCC("CPAN"));

            //
            // Read `CPAN::ObjectInfo`
            //

            /** ANIM
            * ObjectName : TString            // Also the name of the bone (Because of this fact that this string uses 28 bytes by default.)
            * Frames     : INT32              // Number of frames
            * Unknown    : INT32              // Usually 0
            * Next       : INT32              // Next sibling
            * Prev       : INT32              // Previous sibling
            * FrameData  : KRTS / KRT0 / KR00 // Key frames
            **/

            struct Anim {
                char       ObjName[28]; // 00 - Name of this sequence
                uint32     NumFrames;   // 28 - Number of (key)frames
                uint32     Next;        // 32 - Next sibling
                uint32     Prev;        // 36 - Previous sibling
                eBoneTag32 BoneTag;     // 40 - Only present if `Header.Size == 44`
            };
            const auto [objInfo, hObjInfo] = ReadSection<Anim>(s);
            assert(hObjInfo.ID == MakeFourCC("ANIM"));

            // Set sequence name
            seq->SetName(objInfo.ObjName);

            // Set bone tag if available
            if (hObjInfo.Size == sizeof(Anim)) {
                seq->SetBoneTag(objInfo.BoneTag);
            }

            //
            // Read `ANIM::FrameData` (If any)
            //

            if (!objInfo.NumFrames) {
                continue;
            }

            /** KR00
            * DeltaTime : FLOAT
            * Rot       : CQuaternion
            **/

            /** KRT0 : KR00
            * Pos : CVector
            **/
            
            /** KRTS : KRT0
            * Scale : CVector // Read, but ignored
            **/
            
            // Frame data header ("KFRM" -> Key Frame)
            const auto hKFRM = RwStreamRead<IFPSectionHeader>(s);
            assert(notsa::contains({MakeFourCC("KRTS"), MakeFourCC("KRT0"), MakeFourCC("KR00")}, hKFRM.ID));

            // Frame properties
            const auto hasRotation    = hKFRM.IDFourCC[1] == 'R';
            const auto hasTranslation = hKFRM.IDFourCC[2] == 'T';
            const auto hasScale       = hKFRM.IDFourCC[3] == 'S';
            
            assert(hasRotation); // Rotation must always be present

            // Allocate frame data
            seq->SetNumFrames(objInfo.NumFrames, hasTranslation, isCompressed, nullptr);

            // Read frame data from the stream
            for (size_t kfN = 0; kfN < objInfo.NumFrames; kfN++) {
                const auto SetKF = [&](auto* kf) {
                    kf->Rot = RwStreamRead<CQuaternion>(s).Conjugated();
                    if (hasTranslation) {
                        kf->Trans = RwStreamRead<CVector>(s);
                        if (hasScale) {
                            RwStreamSkip(s, sizeof(CVector)); // Scale ignored
                        }
                    }
                    kf->DeltaTime = RwStreamRead<float>(s);
                };
                if (isCompressed) {
                    SetKF(seq->GetCKeyFrame(kfN));
                } else {
                    SetKF(seq->GetUKeyFrame(kfN));
                }
            }

        }

        if (!hier->m_bIsCompressed) {
            hier->RemoveQuaternionFlips();
            hier->CalcTotalTime();
        }
    }
    ms_numAnimations = std::max<int32>(ablock->FirstAnimIdx + infoAnim.Num, ms_numAnimations);
}

// NOTE:
// This code was written based off the docs on gtamods, but it does seem to work
inline void CAnimManager::LoadAnimFile_ANP23(RwStream* s, const IFPSectionHeader& h, bool bLoadCompressed, bool isANP3) {
    char blockName[24];
    RwStreamRead(s, blockName, sizeof(blockName));
    const auto numAnims = RwStreamRead<uint32>(s);

    const auto [ablock, ablockId] = GetOrCreateAnimBlock(blockName, numAnims);

    NOTSA_LOG_DEBUG("Reading block ({})", blockName);

    for (size_t animN = 0; animN < numAnims; animN++) {
        const auto hier = &ms_aAnimations[ablock->FirstAnimIdx + animN];

        // Animation name
        char aname[24];
        RwStreamRead(s, aname, sizeof(aname));
        hier->SetName(aname);

        // Number of sequences
        const auto numSeq = RwStreamRead<uint32>(s);

        // In ANP3 a big chunk of memory is allocated for all frames
        // instead of allocating lots of small chunks
        char* frames = nullptr;
        if (isANP3) {
            const auto size  = RwStreamRead<uint32>(s);
            const auto flags = RwStreamRead<uint32>(s);

            hier->m_bIsCompressed = flags & 1;

            frames = (char*)CMemoryMgr::Malloc(size);
        }

        hier->m_nAnimBlockId    = ablockId;
        hier->m_bKeepCompressed = false;

        // Allocate sequences now
        hier->SetNumSequences(numSeq);

        // Read sequences
        for (size_t seqN = 0; seqN < numSeq; seqN++) {
            const auto seq = &hier->m_pSequences[seqN];

            char seqName[24];
            RwStreamRead(s, seqName, sizeof(seqName));
            const auto frameType = RwStreamRead<uint32>(s);
            const auto numFrames = RwStreamRead<uint32>(s);
            const auto boneTag   = RwStreamRead<eBoneTag32>(s);

            // Only 1 of these will be valid in the end
            // If BoneTag != -1 then it overwrites the name.
            seq->SetName(seqName);
            seq->SetBoneTag(boneTag);

            // Read frames
            const auto ReadFrames = [&](size_t kfSize, bool hasTranslation, bool compressed) {
                seq->SetNumFrames(numFrames, hasTranslation, compressed, frames);

                const auto memSz = kfSize * numFrames;
                RwStreamRead(s, seq->m_Frames, memSz);
                if (isANP3) {
                    frames += memSz;
                }
            };

            switch (frameType) {
            case 1:  ReadFrames(sizeof(KeyFrame), false, false); break;
            case 2:  ReadFrames(sizeof(KeyFrameTrans), true, false); break;
            case 3:  ReadFrames(sizeof(KeyFrameCompressed), false, true); break;
            case 4:  ReadFrames(sizeof(KeyFrameTransCompressed), true, true); break;
            default: NOTSA_UNREACHABLE("Invalid FrameType ({})", frameType);
            }

            if (isANP3) {
                seq->m_bUsingExternalMemory = true;
            }
        }

        if (!hier->m_bIsCompressed) {
            hier->RemoveQuaternionFlips();
            hier->CalcTotalTime();
        }
    }

    ms_numAnimations = std::max<int32>(ablock->FirstAnimIdx + numAnims, ms_numAnimations);
}

AnimationId CAnimManager::GetRandomGangTalkAnim() {
    constexpr AnimationId gangTalkAnims[]{
        ANIM_ID_PRTIAL_GNGTLKA,
        ANIM_ID_PRTIAL_GNGTLKB,
        ANIM_ID_PRTIAL_GNGTLKC,
        ANIM_ID_PRTIAL_GNGTLKD,

        ANIM_ID_PRTIAL_GNGTLKE,
        ANIM_ID_PRTIAL_GNGTLKF,
        ANIM_ID_PRTIAL_GNGTLKG,
        ANIM_ID_PRTIAL_GNGTLKH,
    };
    return CGeneral::RandomChoice(gangTalkAnims);
}
