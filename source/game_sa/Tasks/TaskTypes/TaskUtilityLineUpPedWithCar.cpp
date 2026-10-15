#include "StdInc.h"

#include "TaskUtilityLineUpPedWithCar.h"

void CTaskUtilityLineUpPedWithCar::InjectHooks() {
    RH_ScopedClass(CTaskUtilityLineUpPedWithCar);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x64FBB0);
    RH_ScopedInstall(Destructor, 0x64FC00);

    RH_ScopedInstall(GetLocalPositionToOpenCarDoor, 0x64FC10, { .Reversed = false });
    RH_ScopedInstall(GetPositionToOpenCarDoor, 0x650A80, { .Reversed = false });
    RH_ScopedInstall(ProcessPed, 0x6513A0, { .Reversed = false });
}

// 0x64FBB0
CTaskUtilityLineUpPedWithCar::CTaskUtilityLineUpPedWithCar(const CVector& offset, int32 time, int32 doorOpenPosType, int32 doorIdx) {
    m_Offset = offset;
    m_fDoorOpenPosZ = -999.99f;
    m_fTime = time;
    m_nDoorOpenPosType = doorOpenPosType;
    m_nDoorIdx = doorIdx;
}

// The following 2 functions seem to have copy elision on the returned CVector, that the compiled functions
// took a vector ptr as their first arg. Now, hopefully our code will compile to the same stuff.
// If not, it might crash here, in that case a wrapper function should be used.

// 0x64FC10
CVector CTaskUtilityLineUpPedWithCar::GetLocalPositionToOpenCarDoor(CVehicle* vehicle, float animProgress, CAnimBlendAssociation* assoc) {
    CVector out;
    plugin::CallMethodAndReturn<CVector, 0x64FC10, CTaskUtilityLineUpPedWithCar*, CVector*, CVehicle*, float, CAnimBlendAssociation*>(this, &out, vehicle, animProgress, assoc);
    return out;
}

// 0x650A80
CVector CTaskUtilityLineUpPedWithCar::GetPositionToOpenCarDoor(CVehicle* vehicle, float animProgress, CAnimBlendAssociation* assoc) {
    CVector out;
    plugin::CallMethodAndReturn<CVector, 0x650A80, CTaskUtilityLineUpPedWithCar*, CVector*, CVehicle*, float, CAnimBlendAssociation*>(this, &out, vehicle, animProgress, assoc);
    return out;
}

// 0x6513A0
bool CTaskUtilityLineUpPedWithCar::ProcessPed(CPed* ped, CVehicle* vehicle, CAnimBlendAssociation* assoc) {
    return plugin::CallMethodAndReturn<bool, 0x6513A0, CTaskUtilityLineUpPedWithCar*, CPed*, CVehicle*, CAnimBlendAssociation*>(this, ped, vehicle, assoc);
}
