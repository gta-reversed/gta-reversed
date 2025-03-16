#pragma once

class CAnimBlendClumpData;
class AnimBlendFrameData;
class CAnimBlendAssociation;
struct RtAnimAnimation;

/*!
* @brief RpAnimBlend plugin unique rwID
*/
#define rwID_RPANIMBLENDPLUGIN MAKECHUNKID(rwVENDORID_DEVELOPER, 0xFB)

/*!
 * @brief Get RpAnimBlend data associated with this clump
 * @param clump 
 * @return 
*/
CAnimBlendClumpData*& RpAnimBlendClumpGetData(RpClump* clump);

namespace RpAnimBlendPlugin {
    void InjectHooks();
};

/*!
 * @brief Attach this plugin to RW
 * @return If the plugin was successfully attached
*/
bool RpAnimBlendPluginAttach();

/*!
 * @unused
 */
CAnimBlendAssociation* RpAnimBlendClumpAddAssociation(RpClump* clump, CAnimBlendAssociation* association, uint32 flags, float startTime, float blendAmount);

/*!
 * @notsa
 * @brief Get the head link for all animations associated with this clump
 * @param clump The clump
 * @return The linked list head for all animations
*/
CAnimBlendLink& RpAnimBlendClumpGetAssociations(RpClump* clump);

/*!
 * @notsa
 * @brief Iterate over all associations of a clump
 * @note The IterFn shouldn't modify the clump's association list
 * @param clump The clump containing the associations
 * @param IterFn The function to be called for iterator
*/
template<typename Fn>
void RpAnimBlendClumpForEachAssociation(RpClump* clump, Fn&& IterFn) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    for (auto l = bd->m_AnimList.next; l;) {
        const auto a = CAnimBlendAssociation::FromLink(l);
        l            = a->GetLink().next;
        std::invoke(IterFn, a);
    }
}

/*!
 * @addr 0x4D6BE0
 * @brief Take ownership of all associations of the clump
 * @brief It leaves this clump with no associations
 * @brief The opposite of this function is `RpAnimBlendClumpGiveAssociations`
 * @param clump The clump containing the associations
 * @return The first animation of the clump that has a link to all other animations
*/
CAnimBlendAssociation* RpAnimBlendClumpExtractAssociations(RpClump* clump);

/*!
* @addr 0x4D6BE0
* @brief Give ownership of a list of associations to this clump
* @brief The opposite of this function is `RpAnimBlendClumpExtractAssociations`
* @param clump The clump to give the ownership of associations to
*/
void RpAnimBlendClumpGiveAssociations(RpClump* clump, CAnimBlendAssociation* associations);

/*!
 * @addr 0x4D64A0
 * @brief Fill frame array
 * @param clump The clump with the frames
 * @param outFrameData Array to be filled
*/
void RpAnimBlendClumpFillFrameArray(RpClump* clump, AnimBlendFrameData** outFrameData);

/*!
 * @addr 0x4D6400
 * @brief Find FrameData of a bone
 * @param clump The clump the bone is part of
 * @param boneTag The BoneTag of the bone
 * @return FrameData of the specified bone
*/
AnimBlendFrameData* RpAnimBlendClumpFindBone(RpClump* clump, eBoneTag32 boneTag);

/*!
 * @addr 0x4D62A0
 * @brief Find FrameData of a bone
 * @param clump The clump the bone is part of
 * @param name The name of the bone. For skinned clumps this is the name of the bone tag, for non-skinned ones it's the frame node name.
 * @return FrameData of the specified bone
 */
AnimBlendFrameData* RpAnimBlendClumpFindFrame(RpClump* clump, const char* name);

/*!
 * @addr 0x4D6370
 * @brief Find FrameData of a bone
 * @param clump The clump the bone is part of
 * @param boneNameKey The hash (key) of the bone's name. For skinned clumps this is the name of the bone tag, for non-skinned ones it's the frame node name.
 * @return FrameData of the specified bone
 */
AnimBlendFrameData* RpAnimBlendClumpFindFrameFromHashKey(RpClump* clump, uint32 boneNameKey);

/*!
 * @addr 0x4D68E0
 * @brief Find animation of based on a hierarchy
 * @param clump The clump containing the associations
 * @param hierarchy The hierarchy to search for  
 * @return The first animation of the given hierarchy, or null
*/
CAnimBlendAssociation* RpAnimBlendClumpGetAssociation(RpClump* clump, bool, CAnimBlendHierarchy* hierarchy);

/*!
 * @addr 0x4D6870
 * @brief Find animation of the given name
 * @param clump The clump containing the associations
 * @param name Name of the animation to search for
 * @return The first animation with the given name, or null
*/
CAnimBlendAssociation* RpAnimBlendClumpGetAssociation(RpClump* clump, const char* name);

/*!
 * @addr 0x4D6870
 * @brief Find animation of the given id
 * @param clump The clump containing the associations
 * @param animId ID of the animation to search for
 * @return The first animation with the given id, or null
*/
CAnimBlendAssociation* RpAnimBlendClumpGetAssociation(RpClump* clump, uint32 animId);

/*!
 * @addr 0x4D15E0
 * @brief Get the first animation associated with the clump
 * @param clump The clump containing the associations
 * @return The first animation associated with the clump, or null
*/
CAnimBlendAssociation* RpAnimBlendClumpGetFirstAssociation(RpClump* clump);

/*!
 * @addr 0x4D6A70
 * @param clump The clump containing the associations
 * @param flags The flags of which at least one is expected to be set
 * @return The first anim with any of the flags set, or null if no animations are present on the clump/no animations have the specified flags set.
*/
CAnimBlendAssociation* RpAnimBlendClumpGetFirstAssociation(RpClump* clump, uint32 flags);

/*!
 * @addr 0x4D6910
 * @brief Get the 1st (and optionally 2nd) non-partial animations with the highest blend value
 * @param           clump The clump containing the associations
 * @param [opt,out] pp2ndAnim The 2nd animation
 * @param [opt,out] pBlendVal2nd The 2nd animation's blend amount
 * @return The 1st non-partial animation with the highest blend value, or null
*/
CAnimBlendAssociation* RpAnimBlendClumpGetMainAssociation(RpClump* clump, CAnimBlendAssociation** pp2ndAnim, float* pBlendVal2nd);

/*!
 * @addr 0x4D6A30
 * @brief Get the `n`th non-partial animation
 * @param clump The clump containing the associations
 * @param n The `n`
 * @return The `n`th non-partial animation
*/
CAnimBlendAssociation* RpAnimBlendClumpGetMainAssociation_N(RpClump* clump, uint32 n);

/*!
 * @brief 0x4D69A0
 * @param clump The clump containing the associations
 * @return 
*/
CAnimBlendAssociation* RpAnimBlendClumpGetMainPartialAssociation(RpClump* clump);

/*!
 * @brief 0x4D69F0
 * @brief Get the `n`th partial animation
 * @param clump The clump containing the associations
 * @param n The `n`
 * @return The `n`th partial animation
*/
CAnimBlendAssociation* RpAnimBlendClumpGetMainPartialAssociation_N(RpClump* clump, int32 n);

/*!
 * @brief 0x4D6B60
 * @param clump The clump containing the associations
 * @return The number of associations (animations) this clump has
*/
uint32 RpAnimBlendClumpGetNumAssociations(RpClump* clump);

/*!
* @brief 0x4D6BB0
* @param clump The clump containing the associations
* @return The number of non-partial associations (animations) this clump has
*/
uint32 RpAnimBlendClumpGetNumNonPartialAssociations(RpClump* clump);

/*!
* @brief 0x4D6B80
* @param clump The clump containing the associations
* @return The number of partial associations (animations) this clump has
*/
uint32 RpAnimBlendClumpGetNumPartialAssociations(RpClump* clump);

/*!
 * @addr 0x4D6720
 * @brief Initialize clump data for RpAnimBlend
 * @param clump The clump
*/
void RpAnimBlendClumpInit(RpClump* clump);

/*!
 * @addr 0x4D6760
 * @brief Check if the RpAnimBlend data of the clump is initialized
 * @param clump The clump to check
 * @return Whenever the RpAnimBlend data of the clump is initialized
*/
bool RpAnimBlendClumpIsInitialized(RpClump* clump);

/*!
 * @addr 0x4D6B00
 * @brief Pause all animations of a clump
 * @param clump The clump to pause the animations of
*/
void RpAnimBlendClumpPauseAllAnimations(RpClump* clump);

/*!
 * @addr 0x4D6B30
 * @brief Un-pause all animations of a clump
 * @param clump The clump to un-pause the animations of
 */
void RpAnimBlendClumpUnPauseAllAnimations(RpClump* clump);

/*!
 * @addr 0x4D6C00
 * @brief Remove all animations of a clump
 * @param clump The clump to remove all animations of
 */
void RpAnimBlendClumpRemoveAllAssociations(RpClump* clump);

/*!
 * @addr 0x4D6820
 * @brief Remove all animations of a clump
 * @param flags Flags to check for. If any of these flags is the animation will be removed. Ignored if `0`.
 * @param clump The clump to remove all animations of
 */
void RpAnimBlendClumpRemoveAssociations(RpClump* clump, uint32 flags = 0);

/*!
 * @addr 0x4D67E0
 * @brief Set the blend delta to the specified value for all animations of the clump
 * @param clump The clump to which the animations belong to
 * @param flags Flags to check for. If any of these flags is set, the anim is processed. Ignored if `0`.
 * @param delta The blend delta to set
*/
void RpAnimBlendClumpSetBlendDeltas(RpClump* clump, uint32 flags, float blendDelta);

/*!
 * @addr 0x4D34F0
 * @brief Update all animations associated with the clump
 * @param clump The clump
 * @param step Time step
 * @param onScreen If the clump is visible on the screen
*/
void RpAnimBlendClumpUpdateAnimations(RpClump* clump, float step, bool onScreen);

/*!
 * @addr 0x4D60E0
 * @brief R* hacking
 * @detail In III and VC R* wrote their own interpolated animation data into a dummy
 * @detail RtAnimAnimation and practically didn't use the RtAnimInterpolator.
 * @detail In SA they went a step further and now write into the interpolation frames of
 * @detail the RtAnimInterpolator instead, almost completely avoid RtAnimAnimation.
 * @detail Credit: https://gtaforums.com/topic/669045-silentpatch/page/154/#comment-1068971599
 * @param hierarchy The animation hierarchy
 * @return The animation
*/
RtAnimAnimation* RpAnimBlendCreateAnimationForHierarchy(RpHAnimHierarchy* hierarchy);

/*!
 * @addr 0x4D5EF0
 * @brief Get the name of a (bone?) frame
 * @param frame The frame to get the name of
 * @return Name of the frame
*/
const char* RpAnimBlendFrameGetName(RwFrame* frame);

/*!
 * @addr 0x4D5F00
 * @brief Set the name of a (bone?) frame
 * @param frame The frame to set the name of
 * @param name The new name
*/
void RpAnimBlendFrameSetName(RwFrame* frame, const char* name);

/*!
 * @addr 0x4D6AB0
 * @brief Get the next animation in the linked list
 * @param association The animation to get the next animation of
 * @return The animation after `association`
*/
CAnimBlendAssociation* RpAnimBlendGetNextAssociation(CAnimBlendAssociation* association);

/*!
 * @addr 0x4D6AD0
 * @brief Get the next animation in the linked list
 * @param association The animation to get the next animation of
 * @param flags The flags that need to be set (Any of them being set is enough)
 * @return The animation after `association`
 */
CAnimBlendAssociation* RpAnimBlendGetNextAssociation(CAnimBlendAssociation* association, uint32 flags);
