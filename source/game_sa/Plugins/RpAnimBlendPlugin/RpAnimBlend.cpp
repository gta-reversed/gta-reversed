#include "StdInc.h"

#include "RpAnimBlend.h"

static uint32& ClumpOffset = *(uint32*)0xB5F878;

CAnimBlendClumpData*& RpAnimBlendClumpGetData(RpClump* clump) {
    return *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
}

// 0x4D5F40
void* ClumpAnimConstruct(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject);
    UNUSED(sizeInObject);

    const auto clump = static_cast<RpClump*>(object);

    RpAnimBlendClumpGetData(clump) = nullptr;

    return object;
}

// 0x4D6110
void* ClumpAnimDestruct(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject);
    UNUSED(sizeInObject);

    const auto clump = static_cast<RpClump*>(object);

    if (auto& bd = RpAnimBlendClumpGetData(clump)) {
        RpAnimBlendClumpRemoveAllAssociations(clump);
        delete std::exchange(bd, nullptr);
    }

    return object;
}

// 0x4D5F90
void* ClumpAnimCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(dstObject);
    UNUSED(srcObject);
    UNUSED(offsetInObject);
    UNUSED(sizeInObject);

    return nullptr;
}

// 0x4D5FA0
void RtAnimBlendKeyFrameApply(void* voidMat, void* voidFrame) { // Semantically same as the original `RpHAnimBlendKeyFrameApply`
    const auto mat   = static_cast<RwMatrix*>(voidMat);
    const auto frame = static_cast<RpHAnimBlendInterpFrame*>(voidFrame);

    RtQuatUnitConvertToMatrix(&frame->q, mat);
    RwV3dAssign(RwMatrixGetPos(mat), &frame->t);
}

// 0x4D60C0
void RpAnimBlendKeyFrameInterpolate(void* voidOut, void* voidIn1, void* voidIn2, float time, void* customData) {
    UNUSED(voidIn1);
    UNUSED(voidIn2);
    UNUSED(time);
    UNUSED(customData);

    const auto out = static_cast<RpHAnimBlendInterpFrame*>(voidOut);
    *out = {};
}

// 0x4D6150
bool RpAnimBlendPluginAttach() {
    ClumpOffset = RpClumpRegisterPlugin(
        sizeof(CAnimBlendClumpData*),
        rwID_RPANIMBLENDPLUGIN,
        &ClumpAnimConstruct,
        &ClumpAnimDestruct,
        &ClumpAnimCopy
    );

    if (ClumpOffset == -1) {
        return false;
    }

    RtAnimInterpolatorInfo rtInfo{
        .typeID                  = rwID_RPANIMBLENDPLUGIN,

        .interpKeyFrameSize      = sizeof(RpHAnimBlendInterpFrame),
        .animKeyFrameSize        = sizeof(RpHAnimKeyFrame),

        .keyFrameApplyCB         = RtAnimBlendKeyFrameApply,
        .keyFrameBlendCB         = RpHAnimKeyFrameBlend,
        .keyFrameInterpolateCB   = RpAnimBlendKeyFrameInterpolate,
        .keyFrameAddCB           = RpHAnimKeyFrameAdd,
        .keyFrameMulRecipCB      = RpHAnimKeyFrameMulRecip,
        .keyFrameStreamReadCB    = RpHAnimKeyFrameStreamRead,
        .keyFrameStreamWriteCB   = RpHAnimKeyFrameStreamWrite,
        .keyFrameStreamGetSizeCB = RpHAnimKeyFrameStreamGetSize,

        .customDataSize          = 0
    };
    RtAnimRegisterInterpolationScheme(&rtInfo);

    return true;
}

// 0x4D5F50
void RpAnimBlendAllocateData(RpClump* clump) {
    RpAnimBlendClumpGetData(clump) = new CAnimBlendClumpData;
}

// 0x4D6510
void RpAnimBlendClumpInitSkinned(RpClump* clump) { // Can't hook, `clump` passed in eax
    const auto bd = RpAnimBlendClumpGetData(clump);

    constexpr size_t MAX_NUM_BONES = 64;

    const auto skinGeo  = RpAtomicGetGeometry(GetFirstAtomic(clump));
    const auto skin     = RpSkinGeometryGetSkin(skinGeo);
    const auto nBones   = RpSkinGetNumBones(skin);
    const auto rpHAHier = GetAnimHierarchyFromSkinClump(clump);

    bd->SetNumberOfBones(nBones);

    // Get bone positions
    CVector bonePositions[MAX_NUM_BONES];
    { // 0x735360 (SkinGetBonePositionsToTable)
        assert(MAX_NUM_BONES >= nBones);

        bonePositions[0] = CVector{0.f, 0.f, 0.f}; // root bone

        uint32  nodeStk[MAX_NUM_BONES]{};
        uint32* nodeStkPtr{nodeStk};
        uint32  currNodeIdx{};

        for (uint32 i = 1; i < nBones; i++) { // Intentionally starting at 1
            // Calculate inverse matrix of this bone
            RwMatrix invBoneMat;
            RwMatrixInvert(&invBoneMat, &RpSkinGetSkinToBoneMatrices(skin)[i]); // Originally they did a copy here, not sure why

            // Calculate position of this bone
            RwV3dTransformPoint(
                &bonePositions[i],
                RwMatrixGetPos(&invBoneMat),
                &RpSkinGetSkinToBoneMatrices(skin)[currNodeIdx]
            );

            // Handle node stack now
            const auto nodeFlags = rpHAHier->pNodeInfo[i].flags;
            if (nodeFlags & rpHANIMPUSHPARENTMATRIX) {
                *++nodeStkPtr = currNodeIdx;
            }
            currNodeIdx = nodeFlags & rpHANIMPOPPARENTMATRIX
                ? *nodeStkPtr--
                : i;
        }
    }

    // Now, fill in the frame blend data from the positions we've just calculated
    for (size_t i = 0; i < nBones; i++) {
        const auto fd = &bd->m_FrameDatas[i]; // Frame blend data
            
        fd->KeyFrame = (RpHAnimBlendInterpFrame*)rtANIMGETINTERPFRAME(rpHAHier->currentAnim, i);
        fd->BoneTag  = rpHAHier->pNodeInfo[i].nodeID;
        fd->BonePos  = bonePositions[i];
    }

    // Initialize all frames now
    bd->ForAllFramesF([](AnimBlendFrameData* fd) { // 0x4D6500 (FrameInitCBskin)
        fd->Flags = 0;
    });
}

RwFrame* SetFrameDataFrameCB(RwFrame* f, void* data) {
    const auto fd = static_cast<AnimBlendFrameData**>(data);
    (*fd)->Frame = f;
    (*fd)++;
    RwFrameForAllChildren(f, SetFrameDataFrameCB, fd);
    return f;
}

// 0x
void RpAnimBlendClumpInitNonSkinned(RpClump* clump) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    // Number of bones is the count number of child frames (excluding root)
    bd->SetNumberOfBones(RwFrameCount(RpClumpGetFrame(clump)) - 1); // They didn't use RwFrameCount, but I will

    // Assign frames to our frame data recursively
    auto fd = bd->m_FrameDatas;
    RwFrameForAllChildren(RpClumpGetFrame(clump), SetFrameDataFrameCB, &fd);
    
    // Initialize all frames now (Could've done this in the above step too, oh well)
    bd->ForAllFramesF([](AnimBlendFrameData* fd) { // 0x4D6640 (FrameInitCBnonskin)
        fd->Flags    = 0;
        fd->FramePos = *RwMatrixGetPos(RwFrameGetMatrix(fd->Frame));
        fd->BoneTag  = BONE_UNKNOWN;
    });
}

// 0x4D6720
void RpAnimBlendClumpInit(RpClump* clump) {
    RpAnimBlendAllocateData(clump); // Moved this common piece out of the 2 functions
    if (IsClumpSkinned(clump)) {
        RpAnimBlendClumpInitSkinned(clump);
    } else {
        RpAnimBlendClumpInitNonSkinned(clump);
    }
    RpAnimBlendClumpGetData(clump)->GetRootFrameData().HasVelocity = true;
}

// 0x4D6790
CAnimBlendAssociation* RpAnimBlendClumpAddAssociation(RpClump* clump, CAnimBlendAssociation* association, uint32 playFlags, float startTime, float blendAmount) {
    NOTSA_UNREACHABLE("Unused function");
}

//! @notsa
CAnimBlendLink& RpAnimBlendClumpGetAssociations(RpClump* clump) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    return bd->m_AnimList;
}

// 0x4D58A0
eBoneTag32 ConvertPedNode2BoneTag(ePedNode pedNode) {
    switch (pedNode) {
    case PED_NODE_UPPER_TORSO:     return BONE_SPINE1;
    case PED_NODE_HEAD:            return BONE_HEAD;
    case PED_NODE_LEFT_ARM:        return BONE_L_UPPER_ARM;
    case PED_NODE_RIGHT_ARM:       return BONE_R_UPPER_ARM;
    case PED_NODE_LEFT_HAND:       return BONE_L_HAND;
    case PED_NODE_RIGHT_HAND:      return BONE_R_HAND;
    case PED_NODE_LEFT_LEG:        return BONE_L_THIGH;
    case PED_NODE_RIGHT_LEG:       return BONE_R_THIGH;
    case PED_NODE_LEFT_FOOT:       return BONE_L_FOOT;
    case PED_NODE_RIGHT_FOOT:      return BONE_R_FOOT;
    case PED_NODE_RIGHT_LOWER_LEG: return BONE_R_CALF;
    case PED_NODE_LEFT_LOWER_LEG:  return BONE_L_CALF;
    case PED_NODE_LEFT_LOWER_ARM:  return BONE_L_FORE_ARM;
    case PED_NODE_RIGHT_LOWER_ARM: return BONE_R_FORE_ARM;
    case PED_NODE_LEFT_CLAVICLE:   return BONE_L_CLAVICLE;
    case PED_NODE_RIGHT_CLAVICLE:  return BONE_R_CLAVICLE;
    case PED_NODE_NECK:            return BONE_NECK;
    case PED_NODE_JAW:             return BONE_JAW;
    default:                       return BONE_UNKNOWN;
    }
}

// 0x4D56F0
const char* ConvertBoneTag2BoneName(eBoneTag32 boneTag) {
    switch (boneTag) {
    case BONE_R_BREAST:    return "R Breast";
    case BONE_L_BREAST:    return "L Breast";
    case BONE_BELLY:       return "Belly";
    case BONE_ROOT:        return "Root";
    case BONE_PELVIS:      return "Pelvis";
    case BONE_SPINE:       return "Spine";
    case BONE_SPINE1:      return "Spine1";
    case BONE_NECK:        return "Neck";
    case BONE_HEAD:        return "Head";
    case BONE_L_BROW:      return "L Brow";
    case BONE_R_BROW:      return "R Brow";
    case BONE_JAW:         return "Jaw";
    case BONE_R_CLAVICLE:  return "Bip01 R Clavicle";
    case BONE_R_UPPER_ARM: return "R UpperArm";
    case BONE_R_FORE_ARM:  return "R Forearm";
    case BONE_R_HAND:      return "R Hand";
    case BONE_R_FINGER:    return "R Fingers";
    case BONE_R_FINGER_01: return "R Finger01";
    case BONE_L_CLAVICLE:  return "Bip01 L Clavicle";
    case BONE_L_UPPER_ARM: return "L UpperArm";
    case BONE_L_FORE_ARM:  return "L Forearm";
    case BONE_L_HAND:      return "L Hand";
    case BONE_L_FINGER:    return "L Fingers";
    case BONE_L_FINGER_01: return "L Finger01";
    case BONE_L_THIGH:     return "L Thigh";
    case BONE_L_CALF:      return "L Calf";
    case BONE_L_FOOT:      return "L Foot";
    case BONE_L_TOE_0:     return "L Toe";
    case BONE_R_THIGH:     return "R Thigh";
    case BONE_R_CALF:      return "R Calf";
    case BONE_R_FOOT:      return "R Foot";
    case BONE_R_TOE_0:     return "R Toe";
    default:               return nullptr;
    }
}

// 0x4D6BE0
CAnimBlendAssociation* RpAnimBlendClumpExtractAssociations(RpClump* clump) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    const auto head = std::exchange(bd->m_AnimList.next, nullptr);
    return CAnimBlendAssociation::FromLink(head);
}

// 0x4D6C30
void RpAnimBlendClumpGiveAssociations(RpClump* clump, CAnimBlendAssociation* associations) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    // Delete all animations of this clump
    RpAnimBlendClumpRemoveAllAssociations(clump);
    
    // Use new list of associations
    bd->m_AnimList.next = &associations->GetLink();
    associations->GetLink().prev = &bd->m_AnimList;
}

// 0x4D64A0
void RpAnimBlendClumpFillFrameArray(RpClump* clump, AnimBlendFrameData** outFrameData) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    if (IsClumpSkinned(clump)) { // 0x4D6450 (FillFrameArrayIndiciesSkinned)
        const auto ah = GetAnimHierarchyFromClump(clump);
        for (size_t i = PED_NODE_UPPER_TORSO; i < TOTAL_PED_NODES; i++) {
            outFrameData[i] = &bd->m_FrameDatas[RpHAnimIDGetIndex(ah, ConvertPedNode2BoneTag((ePedNode)i))];
        }
    } else {
        bd->ForAllFramesF([&](AnimBlendFrameData* fd) { // 0x4D6430 (FillFrameArrayIndiciesNonSkinned)
            outFrameData[CVisibilityPlugins::GetFrameHierarchyId(fd->Frame)] = fd;
        });
    }
}

// 0x4D6400
AnimBlendFrameData* RpAnimBlendClumpFindBone(RpClump* clump, eBoneTag32 boneTag) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    AnimBlendFrameData* ret{};
    bd->ForAllFramesF([&](AnimBlendFrameData* f) {
        if (f->BoneTag == boneTag) {
            ret = f;
        }
    });
    return ret;
}

// 0x4D62A0
AnimBlendFrameData* RpAnimBlendClumpFindFrame(RpClump* clump, const char* needle) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    const auto          needlesv = notsa::ci_string_view{ needle };
    AnimBlendFrameData* ret{};
    if (IsClumpSkinned(clump)) {
        bd->ForAllFramesF([&](AnimBlendFrameData* f) { // 0x4D6240
            const auto boneName = ConvertBoneTag2BoneName((eBoneTag)f->BoneTag);
            if (boneName && needlesv == boneName) {
                ret = f;
            }
        });
    } else {
        bd->ForAllFramesF([&](AnimBlendFrameData* f) { // 0x4D6240
            if (needlesv == GetFrameNodeName(f->Frame)) {
                ret = f;
            }
        });
    }
    return ret;
}

// 0x4D6370
AnimBlendFrameData* RpAnimBlendClumpFindFrameFromHashKey(RpClump* clump, uint32 boneNameKey) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    AnimBlendFrameData* ret{};
    if (IsClumpSkinned(clump)) {
        bd->ForAllFramesF([&](AnimBlendFrameData* f) { // 0x4D6310
            const auto boneName = ConvertBoneTag2BoneName((eBoneTag)f->BoneTag);
            if (boneName && boneNameKey == CKeyGen::GetUppercaseKey(boneName)) {
                ret = f;
            }
        });
    } else {
        bd->ForAllFramesF([&](AnimBlendFrameData* f) { // 0x4D6340
            if (boneNameKey == CKeyGen::GetUppercaseKey(GetFrameNodeName(f->Frame))) {
                ret = f;
            }
        });
    }
    return ret;
}

// notsa
template<typename Fn>
CAnimBlendAssociation* RpAnimBlendClumpFindAssociationIf_N(RpClump* clump, Fn&& Pred, size_t n) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    for (auto l = bd->m_AnimList.next; l;) {
        const auto a = CAnimBlendAssociation::FromLink(l);
        if (Pred(*a)) {
            if (n-- == 0) {
                return a;
            }
        }
        l = a->GetLink().next;
    }
    return nullptr;   
}

// 0x4D68E0
CAnimBlendAssociation* RpAnimBlendClumpGetAssociation(RpClump* clump, bool, CAnimBlendHierarchy* h) {
    return RpAnimBlendClumpFindAssociationIf_N(clump, [h](const CAnimBlendAssociation& a) {
        return a.GetHier() == h;
    }, 0);
}

// 0x4D6870
CAnimBlendAssociation* RpAnimBlendClumpGetAssociation(RpClump* clump, const char* name) {
    return RpAnimBlendClumpFindAssociationIf_N(clump, [nameKey = CKeyGen::GetUppercaseKey(name)](const CAnimBlendAssociation& a){
        return a.GetHashKey() == nameKey;
    }, 0);
}

// AnimationId animId
// 0x4D68B0
CAnimBlendAssociation* RpAnimBlendClumpGetAssociation(RpClump* clump, uint32 animId) {
    return RpAnimBlendClumpFindAssociationIf_N(clump, [animId](const CAnimBlendAssociation& a){
        return a.GetAnimId() == animId;
    }, 0);
}

// 0x4D15E0
CAnimBlendAssociation* RpAnimBlendClumpGetFirstAssociation(RpClump* clump) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    return RpAnimBlendClumpIsInitialized(clump) && bd->m_AnimList.next
        ? CAnimBlendAssociation::FromLink(bd->m_AnimList.next)
        : nullptr;
}

// 0x4D6A70
CAnimBlendAssociation* RpAnimBlendClumpGetFirstAssociation(RpClump* clump, uint32 flags) {
    return RpAnimBlendClumpFindAssociationIf_N(clump, [flags](const CAnimBlendAssociation& a) {
        return a.m_Flags & flags;
    }, 0);
}

// 0x4D6910
CAnimBlendAssociation* RpAnimBlendClumpGetMainAssociation(RpClump* clump, CAnimBlendAssociation** pp2ndAnim, float* pBlendVal2nd) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    CAnimBlendAssociation *aA{}, *aB{};
    float                  bA{}, bB{};
    RpAnimBlendClumpForEachAssociation(clump, [&](CAnimBlendAssociation* a) {
        if (a->IsPartial()) {
            return;
        }
        const auto blend = a->GetBlendAmount();
        if (blend > bA) { // Found a new main?
            aB = std::exchange(aA, a);
            bB = std::exchange(bA, blend);
        } else if (blend > bB) { // Found a new secondary?
            aB = a;
            bB = blend;
        }
    });

    if (pp2ndAnim) {
        *pp2ndAnim = aB;
    }
    if (pBlendVal2nd) {
        *pBlendVal2nd = bB;
    }

    return aA;

    /*
    * Code works, but i think it I overcomplicated it....
    // Array sorted descending by blend amount
    struct {
        CAnimBlendAssociation* anim{};
        float                  blendAmnt{};
    } sorted[2];

    // Fill array
    for (auto l = bd->m_AnimList.next; l;) {
        const auto a = CAnimBlendAssociation::FromLink(l);
        if (!a->IsPartial()) {
            const auto it = rng::upper_bound(
                sorted,
                a->GetBlendAmount(),
                [](float l, float r) { return l > r; }, // Descending
                [](auto& v) { return v.blendAmnt; }     // By blend amount
            );
            if (it != std::end(sorted)) {
                std::shift_right(it, std::end(sorted), 1); // Make space for insert
                *it = { a, a->GetBlendAmount() };
            }
        }
        l = a->GetLink().next;
    }

    if (pp2ndAnim) {
        *pp2ndAnim = sorted[1].anim;
    }
    if (pBlendVal2nd) {
        *pBlendVal2nd = sorted[1].blendAmnt;
    }

    return sorted[0].anim;
    */
}

// 0x4D6A30
CAnimBlendAssociation* RpAnimBlendClumpGetMainAssociation_N(RpClump* clump, uint32 n) {
    return RpAnimBlendClumpFindAssociationIf_N(clump, [](const CAnimBlendAssociation& a){
        return !a.IsPartial();
    }, n);
}

// 0x4D69A0
CAnimBlendAssociation* RpAnimBlendClumpGetMainPartialAssociation(RpClump* clump) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    CAnimBlendAssociation *mA{};
    float                  mB{};
    RpAnimBlendClumpForEachAssociation(clump, [&](CAnimBlendAssociation* a) {
        if (!a->IsPartial()) {
            return;
        }
        if (a->GetBlendAmount() > mB) {
            mA = a;
            mB = a->GetBlendAmount();
        }
    });
    return mA;
}

// 0x4D69F0
CAnimBlendAssociation* RpAnimBlendClumpGetMainPartialAssociation_N(RpClump* clump, int32 n) {
    return RpAnimBlendClumpFindAssociationIf_N(clump, [](const CAnimBlendAssociation& a){
        return a.IsPartial();
    }, n);
}

// notsa
template<typename Fn>
uint32 RpAnimBlendClumpCountAssociationsIf(RpClump* clump, Fn&& Pred) {
    return rng::count_if(RpAnimBlendClumpGetAssociations(clump), Pred);
}

// 0x4D6B60
uint32 RpAnimBlendClumpGetNumAssociations(RpClump* clump) {
    return RpAnimBlendClumpCountAssociationsIf(clump, [](const CAnimBlendAssociation& a){
        return true; // Count all
    });
}

// 0x4D6BB0
uint32 RpAnimBlendClumpGetNumNonPartialAssociations(RpClump* clump) {
    return RpAnimBlendClumpCountAssociationsIf(clump, [](const CAnimBlendAssociation& a){
        return !a.IsPartial();
    });
}

// 0x4D6B80
uint32 RpAnimBlendClumpGetNumPartialAssociations(RpClump* clump) {
    return RpAnimBlendClumpCountAssociationsIf(clump, [](const CAnimBlendAssociation& a){
        return a.IsPartial();
    });
}

// 0x4D6760
bool RpAnimBlendClumpIsInitialized(RpClump* clump) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    return bd && bd->m_NumFrameData;
}

// 0x4D6B00
void RpAnimBlendClumpPauseAllAnimations(RpClump* clump) {
    for (auto& a : RpAnimBlendClumpGetAssociations(clump)) {
        a.SetFlag(ANIMATION_IS_PLAYING, false);
    }
}

// 0x4D6B30
void RpAnimBlendClumpUnPauseAllAnimations(RpClump* clump) {
    for (auto& a : RpAnimBlendClumpGetAssociations(clump)) {
        a.SetFlag(ANIMATION_IS_PLAYING, true);
    }
}


// 0x4D6C00
void RpAnimBlendClumpRemoveAllAssociations(RpClump* clump) {
    RpAnimBlendClumpForEachAssociation(clump, [](CAnimBlendAssociation* a) { // Musn't use a for loop here, because it doesn't pre-cache `next`
        delete a;
    });
}

// 0x4D6820
void RpAnimBlendClumpRemoveAssociations(RpClump* clump, uint32 flags) {
    RpAnimBlendClumpForEachAssociation(clump, [=](CAnimBlendAssociation* a) { // Musn't use a for loop here, because it doesn't pre-cache `next`
        if (!flags || (a->m_Flags & flags)) {
            delete a;
        }
    });
}

// 0x4D67E0
void RpAnimBlendClumpSetBlendDeltas(RpClump* clump, uint32 flags, float delta) {
    RpAnimBlendClumpForEachAssociation(clump, [=](CAnimBlendAssociation* a) {
        if (!flags || (a->m_Flags & flags)) {
            a->SetBlendDelta(delta);
        }
    });
}

struct AnimBlendUpdateData { // OG name
    bool32          IncludePartial{};      //!< Has non-moving anim (Eg.: Anim with MOVEMENT flag NOT set)
    CAnimBlendNode* BlendNodeArrays[12]{}; //!< Null terminated array of arrays.
                                           //!< Each entry is incremented on each call of the appropriate `FrameUpdateCallBack`
                                           //!< So that on every call they point to the blend node of the given frame
                                           //!< (This works because the sequences are sorted the same way as the frames apprear in the clump)
};

static auto& gpAnimBlendClump = StaticRef<CAnimBlendClumpData*>(0xB4EA0C);

float CalculateTotalBlendOfPartial(AnimBlendUpdateData* c, AnimBlendFrameData* fd, bool checkDontAddToBlend) {
    float sum{};
    if (c->IncludePartial) {
        for (auto it = c->BlendNodeArrays; *it; it++) {
            const auto node = *it;
            if (!node->IsValid()) {
                continue;
            }

            const auto assoc = node->GetAnimAssoc();
            if (assoc->HasFlag(ANIMATION_IS_PARTIAL) && (!checkDontAddToBlend || !assoc->HasFlag(ANIMATION_DONT_ADD_TO_PARTIAL_BLEND))) {
                sum += assoc->GetBlendAmount();
            }
        }
    }
    return sum;
}

#define USE_COPY_PASTE_FRAME_UPDATE 0
#ifdef USE_COPY_PASTE_FRAME_UPDATE
    #define DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS 1
#endif

#ifdef USE_COPY_PASTE_FRAME_UPDATE
#pragma region "R* Frame Update Functions"
// 0x4D1DB0
void FrameUpdateCallBackWithVelocityExtractionCompressedSkinned(AnimBlendUpdateData* c, AnimBlendFrameData* fd) {
    assert(fd->HasVelocity);
    assert(fd->HasZVelocity);

#if DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS
    _asm {
        mov eax, c
        push fd
        mov ecx, 0x4D1DB0
        call ecx
    };
    return;
#endif

    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, false);

    CVector currV{}; // Velocity
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto node = *it;

        if (!node->IsValid() || !node->GetRootKF()->HasTranslation()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();
        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector t;
        node->GetCurrentTranslationCompressed(t, partialScale);
        currV += t;
    }

    CQuaternion nextQ{};
    CVector     nextT{}; // Translation
    CVector     deltaV{}, loopedDeltaV{}; // 3D Velocity
    bool        hasLoopedVelocity{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();

        const auto blendScale = partialScale;
        CVector t;
        CQuaternion q;
        const auto looped = node->UpdateCompressed(t, q, blendScale);

        nextQ = nextQ + q;

        if (!node->GetRootKF()->HasTranslation()) {
            continue;
        }

        nextT += t;

        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        deltaV += t;

        if (hasLoopedVelocity |= looped) {
            CVector t2;
            node->GetEndTranslationCompressed(t2, blendScale);
            loopedDeltaV += t;
        }
    }

    if (!fd->KeyFramesIgnoreNodeOrientation) {
        nextQ.Normalise();
        fd->KeyFrame->q = nextQ;
    }

    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Update world positions (This moves the ped around the world)
        const auto wsPos = gpAnimBlendClump->m_PedPosition;
        *wsPos += deltaV - currV;
        if (hasLoopedVelocity) {
            *wsPos += loopedDeltaV;
        }

        // Update key-frame translation
        fd->KeyFrame->t = nextT - deltaV + fd->BonePos;
    }
}

// 0x4D2E40
// FrameUpdateCallBackT<true, true, false, false>
void FrameUpdateCallBackCompressedSkinned(AnimBlendFrameData* fd, void* data) {
    const auto c = static_cast<AnimBlendUpdateData*>(data);

    if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
        FrameUpdateCallBackWithVelocityExtractionCompressedSkinned(c, fd);
        return;
    }

    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, false);

    // Calculate new key-frame translation, rotation and blend values
    CVector nextT{};
    CQuaternion nextQ{};
    float nextBlendT{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        CVector t;
        CQuaternion q;
        node->UpdateCompressed(t, q, partialScale);

        // Sum translation
        if (node->GetRootKF()->HasTranslation()) {
            nextT      += t;
            nextBlendT += node->GetAnimAssoc()->GetBlendAmount();
        }

        // Sum rotation
        nextQ = nextQ + (q.Dot(nextQ) >= 0.f ? q : -q);
    }

    // Apply rotation to kf
    if (!fd->KeyFramesIgnoreNodeOrientation) {
        nextQ.Normalise();
        fd->KeyFrame->q = nextQ;
    }

    // Apply translation to kf
    if (!fd->KeyFramesIgnoreNodeTranslation) {
        fd->KeyFrame->t = lerp<CVector>(fd->BonePos, nextT, nextBlendT);
    }
}

// 0x4D27F0
// FrameUpdateCallBackT<true, false, true, true>
void FrameUpdateCallBackWithVelocityExtractionCompressedNonSkinned(AnimBlendUpdateData* c, AnimBlendFrameData* fd) {
    assert(fd->HasVelocity);
    assert(fd->HasZVelocity);

#if DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS
    _asm {
        mov eax, c
        push fd
        mov ecx, 0x4D27F0
        call ecx
    };
    return;
#endif

    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, false);

    CVector currV{}; // 3D Velocity
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto node = *it;

        if (!node->IsValid() || !node->GetRootKF()->HasTranslation()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();
        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector t;
        node->GetCurrentTranslationCompressed(t, partialScale);
        currV += t;
    }

    CQuaternion nextQ{};
    CVector     nextT{}; // Translation
    CVector     deltaV{}, loopedDeltaV{}; // 3D Velocity
    bool        hasLoopedVelocity{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();

        const auto blendScale = partialScale;
        CVector t;
        CQuaternion q;
        const auto looped = node->UpdateCompressed(t, q, blendScale);

        nextQ = nextQ + q;

        if (!node->GetRootKF()->HasTranslation()) {
            continue;
        }

        nextT += t;

        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        deltaV += t;

        if (hasLoopedVelocity |= looped) {
            CVector t2;
            node->GetEndTranslationCompressed(t2, blendScale);
            loopedDeltaV += t;
        }
    }

    const auto fmat = RwFrameGetMatrix(fd->Frame);

    if (!fd->KeyFramesIgnoreNodeOrientation) {
        RwMatrixSetIdentity(fmat);
        nextQ.Normalise();
        nextQ.Get(fmat);
    }

    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Update world positions (This moves the ped around the world)
        const auto wsPos = gpAnimBlendClump->m_PedPosition;
        *wsPos += deltaV - currV;
        if (hasLoopedVelocity) {
            *wsPos += loopedDeltaV;
        }

        // Update key-frame translation
        CVector t = nextT - deltaV + fd->BonePos;
        RwV3dAssign(RwMatrixGetPos(fmat), &t);
    }

    RwMatrixUpdate(fmat);
}

// 0x4D32D0
// FrameUpdateCallBackT<true, false, false, false>
void FrameUpdateCallBackCompressedNonSkinned(AnimBlendFrameData* fd, void* data) {
    const auto c = static_cast<AnimBlendUpdateData*>(data);

    if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
        FrameUpdateCallBackWithVelocityExtractionCompressedNonSkinned(c, fd);
        return;
    }

    // 0x4D2C23
    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, false);

    // 0x4D2D69 - Calculate new key-frame translation, rotation and blend values
    CVector nextT{};
    CQuaternion nextQ{};
    float nextBlendT{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        CVector t;
        CQuaternion q;
        node->UpdateCompressed(t, q, partialScale);

        // Sum translation
        if (node->GetRootKF()->HasTranslation()) {
            nextT += t;
            nextBlendT += node->GetAnimAssoc()->GetBlendAmount();
        }

        // Sum rotation
        nextQ = nextQ + q;
    }
    const auto fmat = RwFrameGetMatrix(fd->Frame);

    // Apply rotation to frame
    if (!fd->KeyFramesIgnoreNodeOrientation) {
        RwMatrixSetIdentity(fmat);
        nextQ.Normalise();
        nextQ.Get(fmat);
    }

    // Apply translation to frame
    if (!fd->KeyFramesIgnoreNodeTranslation) {
        CVector t = lerp<CVector>(fd->FramePos, nextT, nextBlendT);
        RwV3dAssign(RwMatrixGetPos(fmat), &t);
    }

    RwMatrixUpdate(fmat);
}

// 0x4D1A50
// FrameUpdateCallBackT<false, true, true, true>
void FrameUpdateCallBackSkinnedWith3dVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) { // `c` is passed in `eax`, can't hook
    assert(fd->HasVelocity);
    assert(fd->HasZVelocity);

#if DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS
    _asm {
        mov eax, c
        push fd
        mov ecx, 0x4D1A50
        call ecx
    };
    return;
#endif

    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, true);

    // 0x4D1B32
    CVector currV{}; // 3D Velocity
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto node = *it;

        if (!node->IsValid() || !node->GetRootKF()->HasTranslation()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();
        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector t;
        node->GetCurrentTranslation(t, partialScale);
        currV += t;
    }

    // 0x4D1B97
    CQuaternion nextQ{};
    CVector     nextT{}; // Translation
    CVector     deltaV{}, loopedDeltaV{}; // 3D Velocity
    bool        hasLoopedVelocity{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();

        const auto blendScale = partialScale;
        CVector t;
        CQuaternion q;
        const auto looped = node->Update(t, q, blendScale);

        nextQ = nextQ + q;

        if (!node->GetRootKF()->HasTranslation()) {
            continue;
        }

        nextT += t;

        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        deltaV += t;

        if (hasLoopedVelocity |= looped) {
            CVector t2;
            node->GetEndTranslation(t2, blendScale);
            loopedDeltaV += t;
        }
    }

    // 0x4D1CB2
    if (!fd->KeyFramesIgnoreNodeOrientation) {
        nextQ.Normalise();
        fd->KeyFrame->q = nextQ;
    }

    // 0x4D1CD7
    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Update world positions (This moves the ped around the world)
        const auto wsPos = gpAnimBlendClump->m_PedPosition;
        *wsPos += deltaV - currV;
        if (hasLoopedVelocity) {
            *wsPos += loopedDeltaV;
        }

        // Update key-frame translation
        fd->KeyFrame->t = nextT - deltaV + fd->BonePos;
    }
}

// 0x4D1680
// FrameUpdateCallBackT<false, true, true, false>
void FrameUpdateCallBackSkinnedWithVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) { // `c` is passed in `eax`, can't hook
    assert(fd->HasVelocity);

#if DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS
    _asm {
        mov eax, c
        push fd
        mov ecx, 0x4D1680
        call ecx
    };
    return;
#endif

    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, true);

    // 0x4D174C
    CVector2D currV{}; // Velocity
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto node = *it;

        if (!node->IsValid() || !node->GetRootKF()->HasTranslation()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();
        if (assoc->HasFlag(ANIMATION_IGNORE_ROOT_TRANSLATION) || !assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector t;
        node->GetCurrentTranslation(t, partialScale);
        currV.y += t.y;
        if (assoc->HasFlag(ANIMATION_CAN_EXTRACT_X_VELOCITY)) {
            currV.x += t.x;
        }
    }

    // 0x4D17C4
    CQuaternion nextQ{}; // Rotation
    CVector     nextT{}; // Translation
    CVector2D   deltaV{}, loopedDeltaV{}; // 2D Velocity
    bool        hasLoopedVelocity{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();

        CVector t;
        CQuaternion q;
        const auto looped = node->Update(t, q, partialScale);

        nextQ = q.Dot(nextQ) >= 0.f
            ? nextQ + q
            : nextQ - q;

        if (!node->GetRootKF()->HasTranslation() || assoc->HasFlag(ANIMATION_IGNORE_ROOT_TRANSLATION)) {
            continue;
        }

        nextT += t;

        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        deltaV.y += t.y;
        if (assoc->HasFlag(ANIMATION_CAN_EXTRACT_X_VELOCITY)) {
            deltaV.x += t.x;
        }

        if (hasLoopedVelocity |= looped) {
            CVector t2;
            node->GetEndTranslation(t2, partialScale);
            loopedDeltaV.y += t2.y;
            if (assoc->HasFlag(ANIMATION_CAN_EXTRACT_X_VELOCITY)) {
                loopedDeltaV.x += t2.x;
            }
        }
    }

    // 0x4D195C
    if (!fd->KeyFramesIgnoreNodeOrientation) {
        nextQ.Normalise();
        fd->KeyFrame->q = nextQ;
    }

    // 0x4D1980
    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Update world positions (This moves the ped around the world)
        const auto wsPos = gpAnimBlendClump->m_PedPosition;
        wsPos->x = deltaV.x - currV.x;
        wsPos->y = deltaV.y - currV.y;
        if (hasLoopedVelocity) {
            wsPos->x += loopedDeltaV.x;
            wsPos->y += loopedDeltaV.y;
        }

        // Update key-frame translation
        const auto kfT = &fd->KeyFrame->t;
        *kfT = nextT - CVector{deltaV};
        kfT->x += fd->BonePos.x;
        kfT->y += fd->BonePos.y;
        if (kfT->z >= -0.8f) {
            kfT->z += kfT->z >= -0.4f
                ? fd->BonePos.z
                : (kfT->z * 2.5f + 2.f) * fd->BonePos.z;
        }
    }
}

// 0x4D2B90
// FrameUpdateCallBackT<false, true, false, false>
void FrameUpdateCallBackSkinned(AnimBlendFrameData* fd, void* data) {
    const auto c = static_cast<AnimBlendUpdateData*>(data);

    if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
        if (fd->HasZVelocity) {
            FrameUpdateCallBackSkinnedWith3dVelocityExtraction(c, fd);
        } else {
            FrameUpdateCallBackSkinnedWithVelocityExtraction(c, fd);
        }
        return;
    }

    // 0x4D2C23
    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, false);

    // 0x4D2D69 - Calculate new key-frame translation, rotation and blend values
    CVector nextT{};
    CQuaternion nextQ{};
    float nextBlendT{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        CVector t;
        CQuaternion q;
        node->Update(t, q, partialScale);

        // Sum translation
        if (node->GetRootKF()->HasTranslation()) {
            nextT += t;
            nextBlendT += node->GetAnimAssoc()->GetBlendAmount();
        }

        // Sum rotation
        nextQ = nextQ + (q.Dot(nextQ) >= 0.f ? q : -q);
    }

    // 0x4D2D73 - Apply rotation to kf
    if (!fd->KeyFramesIgnoreNodeOrientation) {
        nextQ.Normalise();
        fd->KeyFrame->q = nextQ;
    }

    // 0x4D2D9D - Apply translation to kf
    if (!fd->KeyFramesIgnoreNodeTranslation) {
        fd->KeyFrame->t = lerp<CVector>(fd->BonePos, nextT, nextBlendT);
    }
}

// 0x4D2450
// FrameUpdateCallBackT<false, false, true, true>
void FrameUpdateCallBackNonSkinnedWith3dVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) {
    assert(fd->HasVelocity);
    assert(fd->HasZVelocity);

#if DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS
    _asm {
        mov eax, c
        push fd
        mov ecx, 0x4D2450
        call ecx
    };
    return;
#endif

    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, false);

    CVector currV{}; // Velocity
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto node = *it;

        if (!node->IsValid() || !node->GetRootKF()->HasTranslation()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();
        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector t;
        node->GetCurrentTranslation(t, partialScale);
        currV += t;
    }

    CQuaternion nextQ{};
    CVector     nextT{}; // Translation
    CVector     deltaV{}, loopedDeltaV{}; // 3D Velocity
    bool        hasLoopedVelocity{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();

        const auto blendScale = partialScale;
        CVector t;
        CQuaternion q;
        const auto looped = node->Update(t, q, blendScale);

        nextQ = nextQ + q;

        if (!node->GetRootKF()->HasTranslation()) {
            continue;
        }

        nextT += t;

        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        deltaV += t;

        if (hasLoopedVelocity |= looped) {
            CVector t2;
            node->GetEndTranslation(t2, blendScale);
            loopedDeltaV += t;
        }
    }

    const auto fmat = RwFrameGetMatrix(fd->Frame);

    if (!fd->KeyFramesIgnoreNodeOrientation) {
        RwMatrixSetIdentity(fmat);
        nextQ.Normalise();
        nextQ.Get(fmat);
    }

    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Update world positions (This moves the ped around the world)
        const auto wsPos = gpAnimBlendClump->m_PedPosition;
        *wsPos += deltaV - currV;
        if (hasLoopedVelocity) {
            *wsPos += loopedDeltaV;
        }

        // Update key-frame translation
        CVector t = nextT - deltaV + fd->BonePos;
        RwV3dAssign(RwMatrixGetPos(fmat), &t);
    }

    RwMatrixUpdate(fmat);
}

// 0x4D2100
// FrameUpdateCallBackT<false, false, true, false>
void FrameUpdateCallBackNonSkinnedWithVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) {
    assert(fd->HasVelocity);

#if DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS
    _asm {
        mov eax, c
        push fd
        mov ecx, 0x4D2100
        call ecx
    };
    return;
#endif

    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, false);

    CVector2D currV{}; // Velocity
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto node = *it;

        if (!node->IsValid() || !node->GetRootKF()->HasTranslation()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();
        if (assoc->HasFlag(ANIMATION_IGNORE_ROOT_TRANSLATION) || !assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector t;
        node->GetCurrentTranslation(t, partialScale);
        currV.y += t.y;
        if (assoc->HasFlag(ANIMATION_CAN_EXTRACT_X_VELOCITY)) {
            currV.x += t.x;
        }
    }

    CQuaternion nextQ{}; // Rotation
    CVector     nextT{}; // Translation
    CVector2D   deltaV{}, loopedDeltaV{}; // 2D Velocity
    bool        hasLoopedVelocity{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();

        CVector t;
        CQuaternion q;
        const auto looped = node->Update(t, q, partialScale);

        nextQ = q.Dot(nextQ) >= 0.f
            ? nextQ + q
            : nextQ - q;

        if (!node->GetRootKF()->HasTranslation() || assoc->HasFlag(ANIMATION_IGNORE_ROOT_TRANSLATION)) {
            continue;
        }

        nextT += t;

        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        deltaV.y += t.y;
        if (assoc->HasFlag(ANIMATION_CAN_EXTRACT_X_VELOCITY)) {
            deltaV.x += t.x;
        }

        if (hasLoopedVelocity |= looped) {
            CVector t2;
            node->GetEndTranslation(t2, partialScale);
            loopedDeltaV.y += t2.y;
            if (assoc->HasFlag(ANIMATION_CAN_EXTRACT_X_VELOCITY)) {
                loopedDeltaV.x += t2.x;
            }
        }
    }

    const auto fmat = RwFrameGetMatrix(fd->Frame);

    if (!fd->KeyFramesIgnoreNodeOrientation) {
        RwMatrixSetIdentity(fmat);
        nextQ.Normalise();
        nextQ.Get(fmat);
    }

    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Update world positions (This moves the ped around the world)
        const auto wsPos = gpAnimBlendClump->m_PedPosition;
        wsPos->x = deltaV.x - currV.x;
        wsPos->y = deltaV.y - currV.y;
        if (hasLoopedVelocity) {
            wsPos->x += loopedDeltaV.x;
            wsPos->y += loopedDeltaV.y;
        }

        // Update key-frame translation
        CVector t = nextT - CVector{deltaV};
        t.x += fd->BonePos.x;
        t.y += fd->BonePos.y;
        RwV3dAssign(RwMatrixGetPos(fmat), &t);

        //if (kfT->z >= -0.8f) {
        //    kfT->z += kfT->z >= -0.4f
        //        ? fd->BonePos.z
        //        : (kfT->z * 2.5f + 2.f) * fd->BonePos.z;
        //}
    }

    RwMatrixUpdate(fmat);
}

// 0x4D30A0
// FrameUpdateCallBackT<false, false, false, false>
void FrameUpdateCallBackNonSkinned(AnimBlendFrameData* fd, void* data) {
    const auto c = static_cast<AnimBlendUpdateData*>(data);

    if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
        if (fd->HasZVelocity) {
            FrameUpdateCallBackNonSkinnedWith3dVelocityExtraction(c, fd);
        } else {
            FrameUpdateCallBackNonSkinnedWithVelocityExtraction(c, fd);
        }
        return;
    }

    // 0x4D3131
    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, false);

    // 0x4D3170 - Calculate new key-frame translation, rotation and blend values
    CVector nextT{};
    CQuaternion nextQ{};
    float nextBlendT{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        CVector t;
        CQuaternion q;
        node->Update(t, q, partialScale);

        // Sum translation
        if (node->GetRootKF()->HasTranslation()) {
            nextT += t;
            nextBlendT += node->GetAnimAssoc()->GetBlendAmount();
        }

        // Sum rotation
        nextQ = nextQ + q;
    }

    const auto fmat = RwFrameGetMatrix(fd->Frame);

    // 0x4D322F - Apply rotation to frame
    if (!fd->KeyFramesIgnoreNodeOrientation) {
        RwMatrixSetIdentity(fmat);
        nextQ.Normalise();
        nextQ.Get(fmat);
    }

    // 0x4D3278 - Apply translation to frame
    if (!fd->KeyFramesIgnoreNodeTranslation) {
        CVector t = lerp<CVector>(fd->FramePos, nextT, nextBlendT);
        RwV3dAssign(RwMatrixGetPos(fmat), &t);
    }

    // 0x4D32BE
    RwMatrixUpdate(fmat);
}

#pragma endregion
#endif

template<bool IsCompressed, bool IsSkinned, bool ExtractVelocity, bool Extract3DVelocity>
struct NeedsRemoveQuatFlips : std::false_type {};
template<>
struct NeedsRemoveQuatFlips<true, true, false, false> : std::true_type {}; // FrameUpdateCallBackCompressedSkinned
template<>
struct NeedsRemoveQuatFlips<false, true, false, false> : std::true_type {}; // FrameUpdateCallBackSkinned
template<>
struct NeedsRemoveQuatFlips<false, true, true, false> : std::true_type {}; // FrameUpdateCallBackSkinnedWithVelocityExtraction
template<>
struct NeedsRemoveQuatFlips<false, false, true, false> : std::true_type {}; // FrameUpdateCallBackNonSkinnedWithVelocityExtraction

/*!
 * @brief Copy-paste eliminated per-tick frame update
 * @tparam IsCompressed If the anim data is compressed
 * @tparam IsSkinned If the frame is skinned or not
 * @tparam ExtractVelocity Whenever to use the translation to update the ped's position
 * @tparam Extract3DVelocity Whenever to use Z translation too for updating the ped's position
 * @param fd Frame Data
 * @param c Context
*/
template<bool IsCompressed, bool IsSkinned, bool ExtractVelocity, bool Extract3DVelocity>
void FrameUpdateCallBackT(AnimBlendFrameData* fd, AnimBlendUpdateData* c) {
    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, true);
    const auto GetVelocityFromTranslation = [](CAnimBlendNode* node, CVector t) {
        return CVector{
            !notsa::IsFixBugs() && Extract3DVelocity || node->GetAnimAssoc()->HasFlag(ANIMATION_CAN_EXTRACT_X_VELOCITY) // BUGFIX: Check this flag for 3D too (Like they did for 2D)
                ? t.x
                : 0.f,
            t.y,
            Extract3DVelocity
                ? t.z
                : 0.f
        };
    };

    // (if `ExtractVelocity`): Extract current velocity
    CVector currV{}; // Current velocity (before update), Z only used if `ExtractZVelocity`
    if constexpr (ExtractVelocity) {
        for (auto it = c->BlendNodeArrays; *it; it++) {
            const auto node = *it;

            if (!node->IsValid() || !node->GetRootKF()->HasTranslation()) {
                continue;
            }

            const auto assoc = node->GetAnimAssoc();
            if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
                continue;
            }

            if (notsa::IsFixBugs() || !Extract3DVelocity) {
                if (assoc->HasFlag(ANIMATION_IGNORE_ROOT_TRANSLATION)) { // BUGFIX: Check this flag for 3D too (Like they did for 2D)
                    continue;
                }
            }

            CVector t;
            node->I_GetCurrentTranslation<IsCompressed>(t, partialScale);
            currV += GetVelocityFromTranslation(node, t);
        }
    }

    CQuaternion nextQ{}; // Rotation after update
    CVector     nextT{}; // Translation after update

    // if `ExtractVelocity`:
    CVector nextV{}, loopedNextV{}; // Velocity after update, Z only used if `ExtractZVelocity`
    bool    hasLoopedVelocity{};
    // else:
    float   nextBlendT{}; // Key-frame blend

    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();

        const auto blendScale = partialScale;
        CVector t;
        CQuaternion q;
        const auto looped = node->I_Update<IsCompressed>(t, q, blendScale);

        // NOTE: Not sure if doing this at all is necessary, but let's keep it the way it was.
        if constexpr (NeedsRemoveQuatFlips<IsCompressed, IsSkinned, ExtractVelocity, Extract3DVelocity>{}) {
            nextQ = nextQ + (q.Dot(nextQ) >= 0.f ? q : -q);
        } else {
            nextQ = nextQ + q;
        }

        if (!node->GetRootKF()->HasTranslation()) {
            continue;
        }

        if (notsa::IsFixBugs() || ExtractVelocity && !Extract3DVelocity) {
            if (assoc->HasFlag(ANIMATION_IGNORE_ROOT_TRANSLATION)) { // BUGFIX: Check this flag for 3D too (Like they did for 2D)
                continue;
            }
        }

        nextT += t;
        if constexpr (!ExtractVelocity) {
            nextBlendT += node->GetAnimAssoc()->GetBlendAmount();
        }

        if constexpr (!ExtractVelocity) {
            continue;
        }

        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector cT;
        node->I_GetCurrentTranslation<IsCompressed>(cT, partialScale);
        nextV += GetVelocityFromTranslation(node, cT);

        if (hasLoopedVelocity |= looped) {
            CVector eT;
            node->I_GetEndTranslation<IsCompressed>(eT, blendScale);
            loopedNextV += GetVelocityFromTranslation(node, eT);
        }
    }

    const auto fmat = IsSkinned
        ? nullptr
        : RwFrameGetMatrix(fd->Frame);

    if (!fd->KeyFramesIgnoreNodeOrientation) {
        nextQ.Normalise();

        if constexpr (IsSkinned) {
            fd->KeyFrame->q = nextQ;
        } else {
            RwMatrixSetIdentity(fmat);
            nextQ.Get(fmat); // Set delta rotation as the rotation of the frame
        }
    }

    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Apply velocity
        if constexpr (ExtractVelocity) {
            const auto wsPos = gpAnimBlendClump->m_PedPosition;

            wsPos->x = nextV.x - currV.x;
            wsPos->y = nextV.y - currV.y;
            if constexpr (Extract3DVelocity) {
                wsPos->z = nextV.z - currV.z;
            }

            if (hasLoopedVelocity) {
                wsPos->x += loopedNextV.x;
                wsPos->y += loopedNextV.y;
                if constexpr (Extract3DVelocity) {
                    wsPos->z += loopedNextV.z;
                }
            }
        }

        // Apply translation
        RwV3d* t = IsSkinned
            ? &fd->KeyFrame->t
            : RwMatrixGetPos(fmat);

        const CVector nodePos = IsSkinned
            ? fd->BonePos
            : fd->FramePos;

        if constexpr (ExtractVelocity) {
            t->x = nextT.x - nextV.x + nodePos.x;
            t->y = nextT.y - nextV.y + nodePos.y;

            if constexpr (Extract3DVelocity) {
                t->z = nextT.z - nextV.z + nodePos.z;
            } else if constexpr (IsSkinned && !IsCompressed) { // FrameUpdateCallBackSkinnedWithVelocityExtraction
                t->z = nextT.z;
                if (t->z >= -0.8f) {
                    t->z += t->z >= -0.4f
                        ? fd->BonePos.z
                        : (t->z * 2.5f + 2.f) * fd->BonePos.z;
                }
            } // otherwise `z` remains untouched
        } else {
            *t = lerp<CVector>(nodePos, nextT, nextBlendT);
        }
    }

    if constexpr (!IsSkinned) {
        RwMatrixUpdate(fmat);
    }
}

/*!
 * @brief Per-tick frame update wrapper
 * @tparam IsCompressed If the anim data is compressed
 * @tparam IsSkinned If the frame is skinned or not
 * @param fd Frame Data
 * @param c Context (Real type: `AnimBlendUpdateData*`)
*/
template<bool IsCompressed, bool IsSkinned>
void FrameUpdateCallBackW(AnimBlendFrameData* fd, void* data) {
    const auto c = static_cast<AnimBlendUpdateData*>(data);

#if USE_COPY_PASTE_FRAME_UPDATE // Templated function
    if constexpr (IsCompressed) {
        if constexpr (IsSkinned) {
            FrameUpdateCallBackCompressedSkinned(fd, c);
        } else {
            FrameUpdateCallBackCompressedNonSkinned(fd, c);
        }
    } else {
        if constexpr (IsSkinned) {
            FrameUpdateCallBackSkinned(fd, c);
        } else {
            FrameUpdateCallBackNonSkinned(fd, c);
        }
    }
#else
    if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
        if (fd->HasZVelocity) { // NOTE: Originally compressed anims immediately did 3D velocity, so this flag may not be set on compressed anims?
            FrameUpdateCallBackT<IsCompressed, IsSkinned, true, true>(fd, c);
        } else {
            FrameUpdateCallBackT<IsCompressed, IsSkinned, true, false>(fd, c);
        }
    } else {
        FrameUpdateCallBackT<IsCompressed, IsSkinned, false, false>(fd, c);
    }
#endif
}

// 0x4D2E10
void FrameUpdateCallBackOffscreen(AnimBlendFrameData* fd, void* data) {
    const auto c = static_cast<AnimBlendUpdateData*>(data);

    if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
#if USE_COPY_PASTE_FRAME_UPDATE
        FrameUpdateCallBackSkinnedWithVelocityExtraction(c, fd);
#else
        FrameUpdateCallBackT<false, true, true, false>(fd, c);
#endif
    }
}

// 0x4D1570
void RpAnimBlendNodeUpdateKeyFrames(AnimBlendUpdateData* c, AnimBlendFrameData* frames, uint32 nFrames) {
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto nodeArray = *it; // #nodes == nFrames

        const auto assoc = nodeArray[0].GetAnimAssoc();
        for (size_t frameN = 0; frameN < nFrames; frameN++) {
            const auto fd = &frames[frameN];

            if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
                continue;
            }

            const auto node = &nodeArray[frameN];
            if (!node->IsValid()) {
                continue;
            }

            node->FindKeyFrame(assoc->GetCurrentTime());
        }
    }
}

// 0x4D34F0
void RpAnimBlendClumpUpdateAnimations(RpClump* clump, float timeStep, bool isOnScreen) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    if (bd->m_AnimList.IsEmpty()) {
        return;
    }

    gpAnimBlendClump = bd;

    AnimBlendUpdateData ctx{};
    size_t             nodesCnt{};

    float totalTime{}, totalBlendAmnt{};

    // 0x4D351F
    RpAnimBlendClumpForEachAssociation(clump, [&](CAnimBlendAssociation* a) {
        if (!a->UpdateBlend(timeStep)) {
            return;
        }
        const auto ah = a->GetHier();
        if (ah->GetSequences().empty()) {
            return;
        }
        CAnimManager::UncompressAnimation(a->GetHier());
        if (nodesCnt + 1 <= std::size(ctx.BlendNodeArrays) - 1) { // - 1 for null terminator
            ctx.BlendNodeArrays[nodesCnt++] = a->GetNodesPtr();
        }
        if (a->IsSyncronised()) {
            totalTime      += ah->GetTotalTime() / a->GetSpeed() * a->GetBlendAmount();
            totalBlendAmnt += a->GetBlendAmount();
        } else {
            ctx.IncludePartial = true;        
        }
    });
    ctx.BlendNodeArrays[nodesCnt] = nullptr; // Null terminator

    // 0x4D35A2 - Update animation's timesteps
    const auto animTimeMult = totalTime == 0.f
        ? 1.f
        : 1.f / totalTime * totalBlendAmnt;
    RpAnimBlendClumpForEachAssociation(clump, [&](CAnimBlendAssociation* a) {
        a->UpdateTimeStep(timeStep, animTimeMult);
    });

    // 0x4D360E - Update all animations's frames
    const auto rootFD = &bd->GetRootFrameData();
    rootFD->IsUpdatingFrame = true;

    if (rootFD->IsCompressed) {
        bd->ForAllFrames(
            IsClumpSkinned(clump)
                ? &FrameUpdateCallBackW<true, true>   // FrameUpdateCallBackCompressedSkinned
                : &FrameUpdateCallBackW<true, false>, // FrameUpdateCallBackCompressedNonSkinned
            &ctx
        );
    } else if (isOnScreen) {
        if (rootFD->NeedsKeyFrameUpdate) {
            RpAnimBlendNodeUpdateKeyFrames(&ctx, bd->m_FrameDatas, bd->m_NumFrameData);
        }

        bd->ForAllFrames(
            IsClumpSkinned(clump)
                ? FrameUpdateCallBackW<false, true>   // FrameUpdateCallBackSkinned
                : FrameUpdateCallBackW<false, false>, // FrameUpdateCallBackNonSkinned
            &ctx
        );

        rootFD->NeedsKeyFrameUpdate = false;
    } else {
        bd->ForAllFrames(FrameUpdateCallBackOffscreen, &ctx);
        rootFD->NeedsKeyFrameUpdate = true;
    }

    // 0x4D3715 - Update all animation's times
    RpAnimBlendClumpForEachAssociation(clump, [&](CAnimBlendAssociation* a) {
        a->UpdateTime(timeStep, animTimeMult);
    });
    
    // 0x4D3764
    RwFrameUpdateObjects(RpClumpGetFrame(clump));
}

// 0x4D60E0
RtAnimAnimation* RpAnimBlendCreateAnimationForHierarchy(RpHAnimHierarchy* hierarchy) {
    if (!hierarchy) {
        return nullptr;
    }
    const auto rtA = RtAnimAnimationCreate(
        rwID_RPANIMBLENDPLUGIN,
        0,
        0,
        0.f
    );
    if (rtA) {
        rtA->numFrames = 2 * hierarchy->numNodes;
    }
    return rtA;
}

// 0x4D5EF0
const char* RpAnimBlendFrameGetName(RwFrame* frame) {
    return GetFrameNodeName(frame);
}

// 0x4D5F00
void RpAnimBlendFrameSetName(RwFrame* frame, const char* name) {
    SetFrameNodeName(frame, name);
}

// 0x4D6AB0
CAnimBlendAssociation* RpAnimBlendGetNextAssociation(CAnimBlendAssociation* association) {
    const auto next = association->GetLink().next;
    return next
        ? CAnimBlendAssociation::FromLink(next)
        : nullptr;
}

// 0x4D6AD0
CAnimBlendAssociation* RpAnimBlendGetNextAssociation(CAnimBlendAssociation* association, uint32 flags) {
    assert(flags);
    
    for (auto l = association->GetLink().next; l;) {
        const auto a = CAnimBlendAssociation::FromLink(l);
        l            = a->GetLink().next;
        if (a->m_Flags & flags) {
            return a;
        }
    }
    return nullptr;
}

void RpAnimBlendPlugin::InjectHooks() {
    RH_ScopedNamespaceName("RpAnimBlend");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(RpAnimBlendPluginAttach, 0x4D6150);
    RH_ScopedGlobalInstall(RpAnimBlendClumpInit, 0x4D6720);
    RH_ScopedGlobalInstall(ClumpAnimConstruct, 0x4D5F40);
    RH_ScopedGlobalInstall(ClumpAnimDestruct, 0x4D6110);
    RH_ScopedGlobalInstall(ClumpAnimCopy, 0x4D5F90);
    RH_ScopedGlobalInstall(RpAnimBlendAllocateData, 0x4D5F50);
    RH_ScopedGlobalInstall(RtAnimBlendKeyFrameApply, 0x4D5FA0);
    RH_ScopedGlobalInstall(RpAnimBlendKeyFrameInterpolate, 0x4D60C0);
    RH_ScopedGlobalInstall(RpAnimBlendClumpAddAssociation, 0x4D6790);
    RH_ScopedGlobalInstall(RpAnimBlendClumpExtractAssociations, 0x4D6BE0);
    RH_ScopedGlobalInstall(RpAnimBlendClumpGiveAssociations, 0x4D6C30);
    RH_ScopedGlobalInstall(RpAnimBlendClumpFillFrameArray, 0x4D64A0);
    RH_ScopedGlobalInstall(RpAnimBlendClumpFindBone, 0x4D6400);
    RH_ScopedGlobalInstall(RpAnimBlendClumpFindFrame, 0x4D62A0);
    RH_ScopedGlobalInstall(RpAnimBlendClumpFindFrameFromHashKey, 0x4D6370);
    RH_ScopedGlobalOverloadedInstall(RpAnimBlendClumpGetAssociation, "Hier", 0x4D68E0, CAnimBlendAssociation*(*)(RpClump*, bool, CAnimBlendHierarchy*));
    RH_ScopedGlobalOverloadedInstall(RpAnimBlendClumpGetAssociation, "AnimName", 0x4D6870, CAnimBlendAssociation*(*)(RpClump*, const char*));
    RH_ScopedGlobalOverloadedInstall(RpAnimBlendClumpGetAssociation, "AnimId", 0x4D68B0, CAnimBlendAssociation*(*)(RpClump*, uint32));
    RH_ScopedGlobalOverloadedInstall(RpAnimBlendClumpGetFirstAssociation, "", 0x4D15E0, CAnimBlendAssociation*(*)(RpClump*));
    RH_ScopedGlobalOverloadedInstall(RpAnimBlendClumpGetFirstAssociation, "Flags", 0x4D6A70, CAnimBlendAssociation*(*)(RpClump*, uint32));
    RH_ScopedGlobalInstall(RpAnimBlendClumpGetMainAssociation, 0x4D6910);
    RH_ScopedGlobalInstall(RpAnimBlendClumpGetMainAssociation_N, 0x4D6A30);
    RH_ScopedGlobalInstall(RpAnimBlendClumpGetMainPartialAssociation, 0x4D69A0);
    RH_ScopedGlobalInstall(RpAnimBlendClumpGetMainPartialAssociation_N, 0x4D69F0);
    RH_ScopedGlobalInstall(RpAnimBlendClumpGetNumAssociations, 0x4D6B60);
    RH_ScopedGlobalInstall(RpAnimBlendClumpGetNumNonPartialAssociations, 0x4D6BB0);
    RH_ScopedGlobalInstall(RpAnimBlendClumpGetNumPartialAssociations, 0x4D6B80);
    RH_ScopedGlobalInstall(RpAnimBlendClumpIsInitialized, 0x4D6760);
    RH_ScopedGlobalInstall(RpAnimBlendClumpPauseAllAnimations, 0x4D6B00);
    RH_ScopedGlobalInstall(RpAnimBlendClumpUnPauseAllAnimations, 0x4D6B30);
    RH_ScopedGlobalInstall(RpAnimBlendClumpRemoveAllAssociations, 0x4D6C00);
    RH_ScopedGlobalInstall(RpAnimBlendClumpRemoveAssociations, 0x4D6820);
    RH_ScopedGlobalInstall(RpAnimBlendClumpSetBlendDeltas, 0x4D67E0);
    RH_ScopedGlobalInstall(RpAnimBlendCreateAnimationForHierarchy, 0x4D60E0);
    RH_ScopedGlobalInstall(RpAnimBlendFrameGetName, 0x4D5EF0);
    RH_ScopedGlobalInstall(RpAnimBlendFrameSetName, 0x4D5F00);
    RH_ScopedGlobalOverloadedInstall(RpAnimBlendGetNextAssociation, "Any", 0x4D6AB0, CAnimBlendAssociation * (*)(CAnimBlendAssociation * association));
    RH_ScopedGlobalOverloadedInstall(RpAnimBlendGetNextAssociation, "Flags", 0x4D6AD0, CAnimBlendAssociation * (*)(CAnimBlendAssociation * association, uint32 flags));

    RH_ScopedGlobalInstall(RpAnimBlendClumpUpdateAnimations, 0x4D34F0, {.reversed = false}); // TODO: Hook this again... Unhooked for testing.
#ifdef USE_COPY_PASTE_FRAME_UPDATE
    // Most of these functions aren't hookable (without effort) they take args in <eax> and whatnot, they aren't regular `__cdecl` calls
    //RH_ScopedGlobalInstall(FrameUpdateCallBackWithVelocityExtractionCompressedSkinned, 0x4D1DB0);
    RH_ScopedGlobalInstall(FrameUpdateCallBackCompressedSkinned, 0x4D2E40, {.enabled = !DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS});
    //RH_ScopedGlobalInstall(FrameUpdateCallBackWithVelocityExtractionCompressedNonSkinned, 0x4D27F0);
    RH_ScopedGlobalInstall(FrameUpdateCallBackCompressedNonSkinned, 0x4D32D0, {.enabled = !DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS});
    //RH_ScopedGlobalInstall(FrameUpdateCallBackSkinnedWith3dVelocityExtraction, 0x4D1A50);
    //RH_ScopedGlobalInstall(FrameUpdateCallBackSkinnedWithVelocityExtraction, 0x4D1680);
    RH_ScopedGlobalInstall(FrameUpdateCallBackSkinned, 0x4D2B90, {.enabled = !DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS});
    //RH_ScopedGlobalInstall(FrameUpdateCallBackNonSkinnedWith3dVelocityExtraction, 0x4D2450);
    //RH_ScopedGlobalInstall(FrameUpdateCallBackNonSkinnedWithVelocityExtraction, 0x4D2100);
    RH_ScopedGlobalInstall(FrameUpdateCallBackNonSkinned, 0x4D30A0, {.enabled = !DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS});
#endif
}
