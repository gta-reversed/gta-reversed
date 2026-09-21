#include "StdInc.h"

#include "TaskComplexGoToPointShooting.h"

void CTaskComplexGoToPointShooting::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointShooting, 0x86fe2c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x668C70);

    RH_ScopedVMTInstall(Clone, 0x66CE10);
    RH_ScopedVMTInstall(GetTaskType, 0x668CC0);
}

// 0x668C70
CTaskComplexGoToPointShooting::CTaskComplexGoToPointShooting(
    eMoveState moveState,
    const CVector& movePos,
    CEntity* aimAtEntity,
    CVector aimPos,
    const float targetRadius,
    const float slowDownDist
) :
    CTaskComplexGoToPointAiming{
        moveState,
        movePos,
        aimAtEntity,
        aimPos,
        targetRadius,
        slowDownDist
    }
{
    // NOP
}

CTaskComplexGoToPointShooting::CTaskComplexGoToPointShooting(const CTaskComplexGoToPointShooting& o) :
    CTaskComplexGoToPointAiming{*this}
{
}

