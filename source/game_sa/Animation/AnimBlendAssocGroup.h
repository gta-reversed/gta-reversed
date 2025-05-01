/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <span>

#include "Base.h"

#include "AnimBlendStaticAssociation.h"
#include "Enums/AnimationEnums.h"

class CAnimBlock;
struct RpClump;

/*!
 * @brief Animation group (block)
 * 
 * Represents a group (block) (Such as `ped`, `int_house`, etc).
 * Is also used for mapping `AnimationId`'s and animation names to animation data.
*/
class CAnimBlendAssocGroup {
public:
    static void InjectHooks();

    CAnimBlendAssocGroup() = default;
    ~CAnimBlendAssocGroup();

    /*!
     * @addr 0x4CE0B0
     * 
     * @brief Instantiate an animation
     * 
     * @param name Name of the animation
     * 
     * @return The uncompressed instantiated animation
    */
    CAnimBlendAssociation* CopyAnimation(const char* name);

    //! @addr 0x4CE130
    CAnimBlendAssociation* CopyAnimation(AnimationId id);

    /*!
     * @addr 0x4CE220
     * 
     * @brief Create default associations
     * 
     * @param blockName Name of the anim block to load the anims from
    */
    void CreateAssociations(const char* blockName);

    /*!
     * @brief Create associations for the specified animations
     * 
     * @param blockName Name of the anim block to load the anims from
     * @param clump Fuck knows
     * @param names Animation names to load
     * @param cnt Num of anims to load (same as #names)
    */
    void CreateAssociations(const char* blockName, RpClump* clump, const char** names, uint32 cnt);

    /*!
     * @addr 0x4CE3B0
     * 
     * @brief Create animation associations with the given models
     * 
     * @param blockName Name of the anim block to load the anims from
     * @param animNames Animation name array
     * @param modelNames Model (Clump models, I guess animation clumps?) name array. This and `animNames` form a kv map (So, the `n`th animation is associated with the `n`th model)
     * @param strBufLen Size of each field in the above name arrays.
    */
    void CreateAssociations(const char* blockName, const char* animNames, const char* modelNames, uint32 strBufLen);

    //! @addr 0x4CDFF0
    void DestroyAssociations();

    //! @addr 0x4CE040
    CAnimBlendStaticAssociation* GetAnimation(const char* name);

    //! @addr 0x4CE090
    CAnimBlendStaticAssociation* GetAnimation(AnimationId id);

    //! 0x4CE1B0
    AnimationId GetAnimationId(const char* name);

    //! @addr 0x4CDFB0
    //! @unused
    void InitEmptyAssociations(RpClump* clump);

    //! @addr 0x4D37A0
    //! @unused
    auto IsCreated() const { return m_NumAnims != 0; }

    //! @addr 0x45B050
    //! @unused
    auto GetNumAnimations() const { return m_NumAnims; }

    //! @addr 0x45B060
    //! @unused
    auto GetAnimBlock() const { return m_AnimBlock; }

private:
    CAnimBlock* AllocateForBlock(const char* blockName, int32 numAnims);

    auto GetAssociations() const { return std::span{m_Anims, m_NumAnims}; }

    CAnimBlendAssociation* CopyAnimation(CAnimBlendStaticAssociation* def);
private:
    //! @notsa
    //! @brief Extract common code from `CreateAssociations`
    void CreateAssociation(CAnimBlendStaticAssociation* assoc, CAnimBlendHierarchy* anim, CBaseModelInfo* mi, size_t i);

private: // Wrappers for hooks
    //! @addr 0x4CDE70
    CAnimBlendAssocGroup* Constructor() {
        this->CAnimBlendAssocGroup::CAnimBlendAssocGroup();
        return this;
    }

    //! @addr 0x4CE1D0
    CAnimBlendAssocGroup* Destructor(uint32 flags) {
        std::destroy_at(this);
        return this;
    }

public:
    CAnimBlock*                  m_AnimBlock{};              ///< The anim block of this group
    CAnimBlendStaticAssociation* m_Anims{};
    uint32                       m_NumAnims{};               ///< Number of anims (in the `m_Anims` array)
    uint32                       m_IdOffset{};               ///< This group's first anim ID
    AssocGroupId                 m_GroupID{ANIM_GROUP_NONE}; ///< This group's ID
};
VALIDATE_SIZE(CAnimBlendAssocGroup, 0x14);
