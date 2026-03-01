#pragma once

#include "Base.h"
#include <rwplcore.h>
#include "TaskSimpleIKChain.h"
#include "Enums/eIKArm.h"

class CEntity;
class CPed;

/*!
 * @brief Class for controlling a ped's left/right arm IK
*/
class NOTSA_EXPORT_VTABLE CTaskSimpleIKPointArm final : public CTaskSimpleIKChain {
public:
    static constexpr auto Type = TASK_SIMPLE_IK_POINT_R_ARM; // Weird.. L_ARM never used?

    CTaskSimpleIKPointArm(const char* purpose, eIKArm arm, CEntity* targetEntity, eBoneTag offsetBoneTag, CVector offsetPos, float speed, int32 blendTime);
    ~CTaskSimpleIKPointArm() override = default; // 0x634240

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override;

    /*!
     * @addr 0x634370
     * @brief Update arm pointing info
     * @param purpose A string describing the caller
     * @param pointAtEntity Entity to point at
     * @param offsetBoneTag ??? - This is almost always a value from `eBoneTag`
     * @param offset ???
     * @param speed IK animation speed
     * @param blendDuration Blend out duration [ms]
    */
    void UpdatePointArmInfo(const char* purpose, CEntity* pointAtEntity, eBoneTag32 offsetBoneTag, CVector offset, float speed, int32 blendDuration);

private:
    bool CreateIKChain(CPed* ped) override;

    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleIKPointArm* Constructor(const char* purpose, eIKArm arm, CEntity* targetEntity, eBoneTag32 bone, RwV3d offsetPosn, float speed, int32 blendTime) { this->CTaskSimpleIKPointArm::CTaskSimpleIKPointArm(purpose, arm, targetEntity, bone, offsetPosn, speed, blendTime); return this; }
    CTaskSimpleIKPointArm* Destructor() { this->CTaskSimpleIKPointArm::~CTaskSimpleIKPointArm(); return this; }

private:
    eIKArm m_Arm{};
};
VALIDATE_SIZE(CTaskSimpleIKPointArm, 0x5C);
