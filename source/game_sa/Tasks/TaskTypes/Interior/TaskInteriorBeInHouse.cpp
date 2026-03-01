#include "StdInc.h"

#include "TaskInteriorBeInHouse.h"
#include "TaskInteriorUseInfo.h"

#include "Interior/InteriorGroup_c.h"
#include "Interior/InteriorInfo_t.h"
#include "Interior/Interior_c.h"

void CTaskInteriorBeInHouse::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorBeInHouse, 0x8701e8, 11);
    RH_ScopedCategory("Tasks/TaskTypes/Interior");

    RH_ScopedInstall(Constructor, 0x674FC0);
    RH_ScopedInstall(Destructor, 0x675000);

    RH_ScopedInstall(GetInfoForPedToUse, 0x6750A0);
    RH_ScopedVMTInstall(Clone, 0x675010);
    RH_ScopedVMTInstall(GetTaskType, 0x674FF0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x676200);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6762B0);
    RH_ScopedVMTInstall(ControlSubTask, 0x675090);
}

// 0x674FC0
CTaskInteriorBeInHouse::CTaskInteriorBeInHouse(InteriorGroup_c* intGrp) :
    m_intGrp{intGrp}
{
}

// NOTSA
CTaskInteriorBeInHouse::CTaskInteriorBeInHouse(const CTaskInteriorBeInHouse& o) :
    CTaskInteriorBeInHouse{o.m_intGrp}
{
}

// 0x6750A0
void CTaskInteriorBeInHouse::GetInfoForPedToUse(CPed* ped, int32* outDuration) {
    using enum eInteriorInfoType;

    const auto [chanceA, chanceB] = [&]() -> std::tuple<int32, int32> {
        if (ped->IsGangster()) {
            return { 0, 100 };
        }
        return { 90, 10 };
        }();

        const auto FindInterior = [this](std::initializer_list<eInteriorInfoType> types) {
            for (auto type : types) {
                if (m_intGrp->FindInteriorInfo(type, &m_intInfo, &m_int)) {
                    return true;
                }
            }
            return false;
            };

        *outDuration = -1;

        const auto rndChance0To100 = CGeneral::GetRandomNumberInRange(0, 100);
        if (rndChance0To100 < chanceA) {
            if (CGeneral::DoCoinFlip()) {
                FindInterior({ UNK_4, UNK_3 });
            } else {
                FindInterior({ UNK_3, UNK_4 });
            }
        }
        if (m_intInfo) {
            return;
        }
        if (rndChance0To100 < chanceA + chanceB) {
            FindInterior({ UNK_1 });
            if (m_intInfo) {
                return;
            }

            FindInterior({ UNK_5 });
            if (m_intInfo) {
                *outDuration = CGeneral::GetRandomNumberInRange(5'000, 30'000);
                return;
            }
        }
        if (!m_intInfo) {
            FindInterior({ UNK_1, UNK_2 });
        }
}

// 0x6762B0
CTask* CTaskInteriorBeInHouse::CreateFirstSubTask(CPed* ped) {
    int32 duration{};
    GetInfoForPedToUse(ped, &duration);
    return new CTaskInteriorUseInfo{ m_intInfo, m_int, duration, true };
}
