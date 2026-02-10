#include "StdInc.h"

#include "Decision.h"

void CDecision::InjectHooks() {    
    RH_ScopedClass(CDecision);
    RH_ScopedCategory("DecisionMakers");

    RH_ScopedInstall(SetDefault, 0x600530, { .reversed = false });
    RH_ScopedInstall(Set, 0x600570);
    RH_ScopedInstall(Add, 0x600600, { .reversed = false });
    RH_ScopedInstall(From, 0x6006B0);
    //RH_ScopedInstall(HasResponse, 0x600710, { .reversed = false });
    RH_ScopedInstall(MakeDecision, 0x6040D0, { .reversed = false });
}

// 0x6040C0
CDecision::CDecision() {
    SetDefault();
}

// 0x600530
void CDecision::SetDefault() {
    plugin::CallMethod<0x600530, CDecision*>(this);
}

// 0x6006B0
void CDecision::From(const CDecision& rhs) {
    *this = rhs;
}

// 0x600570
void CDecision::Set(
    notsa::mdarray<int32, MAX_NUM_CHOICES>&    tasks,
    notsa::mdarray<float, MAX_NUM_CHOICES, 4>& probs,
    notsa::mdarray<int32, MAX_NUM_CHOICES, 2>& bools,
    notsa::mdarray<float, MAX_NUM_CHOICES, 6>& facialProbs
) {
    plugin::CallMethod<0x600570>(this, &tasks, &probs, &bools, &facialProbs);
}

// 0x6040D0
void CDecision::MakeDecision(
    int32 eventSourceType,
    bool isInVehicle,
    eTaskType taskTypeToReject1,
    eTaskType taskTypeToReject2,
    eTaskType taskTypeToReject3,
    eTaskType taskTypeToSeek,
    int16& outTaskType
) {
    plugin::CallMethod<0x6040D0, CDecision*, int32, bool, eTaskType, eTaskType, eTaskType, eTaskType, int16*>(
        this,
        eventSourceType,
        isInVehicle,
        taskTypeToReject1,
        taskTypeToReject2,
        taskTypeToReject3,
        taskTypeToSeek,
        &outTaskType
    );
}

// 0x600600
void CDecision::Add(eTaskType taskType, float* pProbs, int32* pBools) {
    plugin::CallMethod<0x600600, CDecision*, eTaskType, float*, int32*>(this, taskType, pProbs, pBools);
}

/*
// 0x600710
bool CDecision::HasResponse() {

}
*/
