#include "StdInc.h"

#include "TaskComplexPartnerGreet.h"
#include "TaskComplexGangLeader.h"

void CTaskComplexPartnerGreet::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPartnerGreet, 0x87078c, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x684210);
    RH_ScopedInstall(Destructor, 0x684280);

    RH_ScopedVMTInstall(Clone, 0x684DA0);
    RH_ScopedVMTInstall(GetTaskType, 0x681E00);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6825A0);
    RH_ScopedVMTInstall(StreamRequiredAnims, 0x6825B0);
    RH_ScopedVMTInstall(GetPartnerSequence, 0x682630);
}

CTaskComplexPartnerGreet::CTaskComplexPartnerGreet(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int32 handShakeType, CVector point) :
    CTaskComplexPartner(commandName, partner, leadSpeaker, distanceMultiplier, true, 1, point)
{
    m_handShakeType = handShakeType;
    m_taskId = TASK_COMPLEX_PARTNER_GREET;
    strcpy_s(m_animBlockName, "gangs");
}

CTaskComplexPartnerGreet::CTaskComplexPartnerGreet(const CTaskComplexPartnerGreet& o) :
    CTaskComplexPartnerGreet{
        o.m_commandName,
        o.m_partner,
        o.m_leadSpeaker,
        o.m_distanceMultiplier,
        o.m_handShakeType,
        o.m_point
    }
{
}

void CTaskComplexPartnerGreet::StreamRequiredAnims() {
    CAnimManager::StreamAnimBlock(m_animBlockName, CTaskComplexGangLeader::ShouldLoadGangAnims(), m_requiredAnimsStreamedIn);
}

CTaskComplexSequence* CTaskComplexPartnerGreet::GetPartnerSequence() {
    NOTSA_UNREACHABLE(); // In theory this is unused
}
