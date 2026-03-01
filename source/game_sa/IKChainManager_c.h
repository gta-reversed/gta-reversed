#pragma once

#include "List_c.h"
#include "IKChain_c.h"
#include "Enums/eIKChainSlot.h"
#include "Enums/eIKArm.h"

class CPed;
class CEntity;

class IKChainManager_c {
public:
    static void InjectHooks();

    IKChainManager_c() = default;  // 0x617FC0
    ~IKChainManager_c() = default; // 0x8568E0

    bool Init();
    void Exit();
    void Reset();
    void Update(float a1);

    /*!
    * @addr 0x618750
    * @brief Tries initing a new chain from the free list.
    * @returns A new `IKChain_c` object, unless there are no more free chains or it's init failed.
    */
    IKChain_c* AddIKChain(
        const char*  name,
        eIKChainSlot slot,
        CPed*        ped,
        eBoneTag32   effectorBone,
        RwV3d        effectorOffset,
        eBoneTag32   pivotBone,
        CEntity*     entity,
        eBoneTag32   offsetBone,
        RwV3d        offset,
        float        speed,
        int32        priority = 3
    );
    void RemoveIKChain(IKChain_c* chain);

    bool CanAccept(CPed* ped, float) const;
    CEntity* GetLookAtEntity(CPed* ped);
    CVector GetLookAtOffset(CPed* ped);
    bool CanAcceptLookAt(CPed* ped);

    /*!
     * @addr 0x618970
     * @brief Make the ped towards another entity
     * @param purpose A string describing the caller
     * @param ped The ped that is looking
     * @param lookAtEntity The entity to look at
     * @param time Duration to look for [ms]
     * @param offsetBone Bone to look at
     * @param offset 
     * @param useTorso 
     * @param fSpeed 
     * @param blendTime 
     * @param priority 
     * @param bForceLooking 
    */
    void LookAt(
            const char* purpose,
            CPed*       ped,
            CEntity*    lookAtEntity,
            int32       time,
            eBoneTag    offsetBone = BONE_HEAD,
            CVector*    offset = nullptr,
            bool        useTorso = true,
            float       fSpeed = 0.25f,
            int32       blendTime = 500,
            int32       priority = 3,
            bool        bForceLooking = false
    );

    /*!
     * @addr 0x6181A0
     * @brief Check if the ped is looking
     * @param ped The ped
    */
    bool IsLooking(CPed* ped) const;


    /*!
    * @addr 0x618280
    * @brief Abort lookat of a ped
    */
    void AbortLookAt(CPed* ped, uint32 blendOutTime = 250u);

    /*!
    * @notsa
    * @brief Abort lookat of a ped if doing lookat
    */
    void AbortLookAtIfLooking(CPed* ped, uint32 blendOutTime = 250u);

    /*!
     * @addr 0x618B60
     * @brief Point the ped's arm towards an entity/position
     * @param purpose A string describing the usecase, not used
     * @param arm Arm to point (left/right)
     * @param ped Ped whose arm to point
     * @param lookAtEntity Entity to look at
     * @param offsetBoneTag ???
     * @param offset ???
     * @param speed IK animation speed (?)
     * @param blendTime When requested how fast the IK should stop (be blended out)
     * @param cullDist  If target/positionn is further than this the IK won't be played
    */
    void PointArm(const char* purpose, eIKArm pedArmId, CPed* ped, CEntity* lookAtEntity, eBoneTag offsetBoneTag, CVector* offset, float speed, int32 blendTime, float cullDist) const;

    /*!
     * @addr 0x6182B0
     * @brief Check if the ped's arm is pointing towards something
    */
    static bool __stdcall IsArmPointing(eIKArm arm, CPed* ped);

    /*!
     * @addr 0x6182F0
     * @brief Abort the arm pointing of a ped
    */
    static void __stdcall AbortPointArm(eIKArm arm, CPed* ped, int32 blendOutTime = 250);

    /*!
    * @notsa
    * @brief Abort arm pointing if it's pointed
    */
    static void AbortPointArmIfPointing(eIKArm arm, CPed* ped, int32 blendOutTime = 250);

    /*!
     * @addr 0x618330
     * @brief Check if the IK task at the given slot is facing the target
    */
    bool IsFacingTarget(CPed* ped, eIKChainSlot slot) const;

public:
    IKChain_c          m_IKChains[32]{};
    TList_c<IKChain_c> m_ActiveList{};
    TList_c<IKChain_c> m_FreeList{};
};
VALIDATE_SIZE(IKChainManager_c, 0xB18);

extern IKChainManager_c& g_ikChainMan;
