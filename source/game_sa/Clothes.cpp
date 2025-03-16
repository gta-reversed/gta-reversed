/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "Clothes.h"
#include "ClothesBuilder.h"
#include "PedClothesDesc.h"

int32& CClothes::ms_clothesImageId = *(int32*)0xBC12F8;
uint32& CClothes::ms_numRuleTags = *(uint32*)0xBC12FC;
uint32 (&CClothes::ms_clothesRules)[600] = *(uint32(*)[600])0xBC1300;

CPedClothesDesc& PlayerClothes = *(CPedClothesDesc*)0xBC1C78;

void CClothes::InjectHooks() {
    RH_ScopedClass(CClothes);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x5A80D0);
    RH_ScopedInstall(LoadClothesFile, 0x5A7B30);
    RH_ScopedInstall(ConstructPedModel, 0x5A81E0);
    RH_ScopedInstall(RequestMotionGroupAnims, 0x5A8120);
    RH_ScopedInstall(RebuildPlayerIfNeeded, 0x5A8390);
    RH_ScopedInstall(RebuildPlayer, 0x5A82C0);
    RH_ScopedInstall(RebuildCutscenePlayer, 0x5A8270);
    RH_ScopedInstall(GetTextureDependency, 0x5A7EA0, { .reversed = false }); // Crashes when enabled - function looks simple and should be correct
    RH_ScopedInstall(GetDependentTexture, 0x5A7F30);
    RH_ScopedInstall(GetPlayerMotionGroupToLoad, 0x5A7FB0);
    RH_ScopedInstall(GetDefaultPlayerMotionGroup, 0x5A81B0);
}

// 0x5A80D0
void CClothes::Init() {
    for (int32 part = 0; part < CLOTHES_TEXTURE_TOTAL; part++) {
        eClothesModelPart modelPart = GetTextureDependency(static_cast<eClothesTexturePart>(part));
        PlayerClothes.m_anTextureKeys[part] = 0;
        if (modelPart != CLOTHES_MODEL_UNAVAILABLE)
            PlayerClothes.m_anModelKeys[modelPart] = 0;
    }
    ms_numRuleTags = 0;
    ms_clothesImageId = CStreaming::AddImageToList("MODELS\\PLAYER.IMG", false);
    LoadClothesFile();
}

// 0x5A7A20
int32 GetClothesModelFromName(const char* name) {
    if (!strcmp(name, "torso"))    return CLOTHES_MODEL_TORSO;
    if (!strcmp(name, "head"))     return CLOTHES_MODEL_HEAD;
    if (!strcmp(name, "hands"))    return CLOTHES_MODEL_HANDS;
    if (!strcmp(name, "legs"))     return CLOTHES_MODEL_LEGS;
    if (!strcmp(name, "feet"))     return CLOTHES_MODEL_SHOES;
    if (!strcmp(name, "necklace")) return CLOTHES_MODEL_NECKLACE;
    if (!strcmp(name, "watch"))    return CLOTHES_MODEL_BRACELET;
    if (!strcmp(name, "glasses"))  return CLOTHES_MODEL_GLASSES;
    if (!strcmp(name, "hat"))      return CLOTHES_MODEL_HATS;
    if (!strcmp(name, "extra1"))   return CLOTHES_MODEL_SPECIAL;

    return CLOTHES_MODEL_TORSO;
}

// 0x5A7B30
void CClothes::LoadClothesFile() {
    bool isRuleStarted = false;
    auto* file = CFileMgr::OpenFile("DATA\\CLOTHES.DAT", "r");

    for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        if (!isRuleStarted) {
            isRuleStarted = !strncmp("rule", line, 4);
            continue;
        }
        if (!strncmp("end", line, 3)) {
            isRuleStarted = false;
            continue;
        }

        char* nextToken{};
        char* strTag = strtok_s(line, " \t,", &nextToken);
        if (strTag == nullptr) {
            continue;
        }

        enum class eClothRule : uint8_t {
            TAG_CUTS,
            TAG_SETC,
            TAG_TEX,
            TAG_HIDE,
            TAG_END_IGNORE,
            TAG_IGNORE,
            TAG_END_EXCLUSIVE,
            TAG_EXCLUSIVE
        };
        const eClothRule ruleTag = [&](){
            constexpr struct {const char* name; eClothRule rule;} map[]{
                {"CUTS", eClothRule::TAG_CUTS},
                {"SETC", eClothRule::TAG_SETC},
                {"TEX", eClothRule::TAG_TEX},
                {"HIDE", eClothRule::TAG_HIDE},
                {"ENDIGNORE", eClothRule::TAG_END_IGNORE},
                {"IGNORE", eClothRule::TAG_IGNORE},
                {"ENDEXCLUSIVE", eClothRule::TAG_END_EXCLUSIVE},
                {"EXCLUSIVE", eClothRule::TAG_EXCLUSIVE}
            };
            for (const auto& [name, rule] : map) {
                if (!strcmp(strTag, name)) {
                    return rule;
                }
            }
            NOTSA_UNREACHABLE("Invalid rule tag: {}", strTag);
        }();
        AddRule(static_cast<uint32>(ruleTag));

        const auto GetNextArg = [&nextToken]{
            return strtok_s(NULL, " \t,", &nextToken);
        };
        switch (ruleTag) {
        case eClothRule::TAG_CUTS:
        case eClothRule::TAG_TEX: {
            for (auto i = 0u; i < 2u; i++) {
                AddRule(CKeyGen::GetUppercaseKey(GetNextArg()));
            }
            break;
        }
        case eClothRule::TAG_SETC: {
            AddRule(CKeyGen::GetUppercaseKey(GetNextArg()));
            AddRule(GetClothesModelFromName(GetNextArg()));
            for (auto i = 0u; i < 2u; i++) {
                const auto rule = GetNextArg();
                AddRule(!strcmp("-", rule) ? 0 : CKeyGen::GetUppercaseKey(rule));
            }
            break;
        }
        case eClothRule::TAG_HIDE: {
            AddRule(CKeyGen::GetUppercaseKey(GetNextArg()));
            AddRule(GetClothesModelFromName(GetNextArg()));
            break;
        }
        case eClothRule::TAG_END_IGNORE:
        case eClothRule::TAG_IGNORE:
        case eClothRule::TAG_END_EXCLUSIVE:
        case eClothRule::TAG_EXCLUSIVE: 
            AddRule(CKeyGen::GetUppercaseKey(GetNextArg()));
            break;
        }
    }

    CFileMgr::CloseFile(file);
}

// 0x5A81E0
void CClothes::ConstructPedModel(uint32 modelId, CPedClothesDesc& newClothes, const CPedClothesDesc* oldClothes, bool bCutscenePlayer) {
    CTimer::Suspend();

    auto modelInfo = CModelInfo::GetModelInfo(modelId)->AsPedModelInfoPtr();
    auto txd = CTxdStore::ms_pTxdPool->GetAt(modelInfo->m_nTxdIndex);
    auto skinnedClump = CClothesBuilder::CreateSkinnedClump(modelInfo->m_pRwClump, txd->m_pRwDictionary, newClothes, oldClothes, bCutscenePlayer);
    if (skinnedClump) {
        RequestMotionGroupAnims();
        modelInfo->AddTexDictionaryRef();
        modelInfo->DeleteRwObject();
        modelInfo->SetClump(skinnedClump);
        modelInfo->RemoveTexDictionaryRef();
        CStreaming::LoadAllRequestedModels(true);
    }

    CTimer::Resume();
}

// 0x5A8120
void CClothes::RequestMotionGroupAnims() {
    const auto group = GetPlayerMotionGroupToLoad();
    const auto fatIndex = CAnimManager::GetAnimationBlockIndex("fat");
    const auto muscularIndex = CAnimManager::GetAnimationBlockIndex("muscular");

    if (group == ANIM_GROUP_FAT) {
        CStreaming::RequestModel(IFPToModelId(fatIndex), STREAMING_GAME_REQUIRED | STREAMING_PRIORITY_REQUEST);
        CStreaming::SetModelIsDeletable(IFPToModelId(muscularIndex));
    } else {
        if (group == ANIM_GROUP_MUSCULAR) {
            CStreaming::RequestModel(IFPToModelId(muscularIndex), STREAMING_GAME_REQUIRED | STREAMING_PRIORITY_REQUEST);
            CStreaming::SetModelIsDeletable(IFPToModelId(fatIndex));
        } else {
            CStreaming::SetModelIsDeletable(IFPToModelId(muscularIndex));
            CStreaming::SetModelIsDeletable(IFPToModelId(fatIndex));
        }
    }
}

// 0x5A8390
void CClothes::RebuildPlayerIfNeeded(CPlayerPed* player) {
    const auto& fat = player->m_pPlayerData->m_pPedClothesDesc->m_fFatStat;
    const auto& muscle = player->m_pPlayerData->m_pPedClothesDesc->m_fMuscleStat;

    if (CStats::GetStatValue(STAT_FAT) != fat || CStats::GetStatValue(STAT_MUSCLE) != muscle) {
        RebuildPlayer(player, 0);
    }
}

// 0x5A82C0
void CClothes::RebuildPlayer(CPlayerPed* player, bool bIgnoreFatAndMuscle) {
    auto assoc = RpAnimBlendClumpExtractAssociations(player->m_pRwClump);
    auto task = player->m_pIntelligence->m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_IK);
    if (task)
        task->MakeAbortable(player, ABORT_PRIORITY_IMMEDIATE, nullptr);

    player->DeleteRwObject();
    CWorld::Remove(player);
    if (!bIgnoreFatAndMuscle) {
        player->m_pPlayerData->m_pPedClothesDesc->m_fFatStat = CStats::GetStatValue(STAT_FAT);
        player->m_pPlayerData->m_pPedClothesDesc->m_fMuscleStat = CStats::GetStatValue(STAT_MUSCLE);
    }

    ConstructPedModel(player->m_nModelIndex, *player->m_pPlayerData->m_pPedClothesDesc, &PlayerClothes, 0);
    player->Dress();
    RpAnimBlendClumpGiveAssociations(player->m_pRwClump, assoc);
    PlayerClothes = *player->m_pPlayerData->m_pPedClothesDesc;
}

// 0x5A8270
void CClothes::RebuildCutscenePlayer(CPlayerPed* player, int32 modelId) {
    const auto& clothesDesc    = player->m_pPlayerData->m_pPedClothesDesc;
    clothesDesc->m_fFatStat    = CStats::GetStatValue(STAT_FAT);
    clothesDesc->m_fMuscleStat = CStats::GetStatValue(STAT_MUSCLE);
    ConstructPedModel(modelId, *clothesDesc, nullptr, true);
}

// 0x5A7EA0
eClothesModelPart CClothes::GetTextureDependency(eClothesTexturePart texturePart) {
    switch (texturePart) {
    case CLOTHES_TEXTURE_TORSO:    return CLOTHES_MODEL_TORSO;
    case CLOTHES_TEXTURE_HEAD:     return CLOTHES_MODEL_HEAD;
    case CLOTHES_TEXTURE_LEGS:     return CLOTHES_MODEL_LEGS;
    case CLOTHES_TEXTURE_SHOES:    return CLOTHES_MODEL_SHOES;
    case CLOTHES_TEXTURE_NECKLACE: return CLOTHES_MODEL_NECKLACE;
    case CLOTHES_TEXTURE_BRACELET: return CLOTHES_MODEL_BRACELET;
    case CLOTHES_TEXTURE_GLASSES:  return CLOTHES_MODEL_GLASSES;
    case CLOTHES_TEXTURE_HATS:     return CLOTHES_MODEL_HATS;
    case CLOTHES_TEXTURE_SPECIAL:  return CLOTHES_MODEL_SPECIAL;
    default:                       return CLOTHES_MODEL_UNAVAILABLE;
    }
}

// 0x5A7F30
eClothesTexturePart CClothes::GetDependentTexture(eClothesModelPart modelPart) {
    switch (modelPart) {
    case CLOTHES_MODEL_TORSO:    return CLOTHES_TEXTURE_TORSO;
    case CLOTHES_MODEL_HEAD:     return CLOTHES_TEXTURE_HEAD;
    case CLOTHES_MODEL_LEGS:     return CLOTHES_TEXTURE_LEGS;
    case CLOTHES_MODEL_SHOES:    return CLOTHES_TEXTURE_SHOES;
    case CLOTHES_MODEL_NECKLACE: return CLOTHES_TEXTURE_NECKLACE;
    case CLOTHES_MODEL_BRACELET: return CLOTHES_TEXTURE_BRACELET;
    case CLOTHES_MODEL_GLASSES:  return CLOTHES_TEXTURE_GLASSES;
    case CLOTHES_MODEL_HATS:     return CLOTHES_TEXTURE_HATS;
    case CLOTHES_MODEL_SPECIAL:  return CLOTHES_TEXTURE_SPECIAL;
    default:                     return CLOTHES_TEXTURE_UNAVAILABLE;
    }
}

// 0x5A7FB0
AssocGroupId CClothes::GetPlayerMotionGroupToLoad() {
    const auto fat = CStats::GetStatValue(STAT_FAT);
    const auto muscle = CStats::GetStatValue(STAT_MUSCLE);

    if (fat > 500.0f && fat > muscle)
        return ANIM_GROUP_FAT;

    if (muscle <= 500.0f)
        return ANIM_GROUP_PLAYER;

    return ANIM_GROUP_MUSCULAR;
}

// 0x5A81B0
AssocGroupId CClothes::GetDefaultPlayerMotionGroup() {
    AssocGroupId group = GetPlayerMotionGroupToLoad();
    if (group == ANIM_GROUP_PLAYER)
        return ANIM_GROUP_PLAYER;

    CAnimBlock* animBlock = CAnimManager::GetAnimationBlock(group);
    if (!animBlock || !animBlock->IsLoaded)
        return ANIM_GROUP_PLAYER;

    return group;
}
