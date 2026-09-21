#include "StdInc.h"

#include "Decision.h"

void CDecision::InjectHooks() {
    RH_ScopedClass(CDecision);
    RH_ScopedCategory("DecisionMakers");

    RH_ScopedInstall(SetDefault, 0x600530);
    RH_ScopedInstall(Set, 0x600570);
    RH_ScopedInstall(Add, 0x600600);
    RH_ScopedInstall(From, 0x6006B0);
    RH_ScopedInstall(HasResponse, 0x600710);
    RH_ScopedInstall(MakeDecision, 0x6040D0);
}

// 0x6040C0
CDecision::CDecision() {
    SetDefault();
}

// 0x600530
void CDecision::SetDefault() {
    for (auto i = 0; i < MAX_NUM_CHOICES; i++) {
        m_Tasks[i] = (eTaskType)-1;
        m_Probs[i].fill(0);
        m_Bools[i].fill(false);
    }
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
    for (auto i = 0; i < MAX_NUM_CHOICES; i++) {
        m_Tasks[i] = (eTaskType)tasks[i];
        for (auto j = 0; j < 4; j++) {
            m_Probs[i][j] = (uint8)probs[i][j];
        }
        m_Bools[i][0] = bools[i][0] != 0;
        m_Bools[i][1] = bools[i][1] != 0;
    }
}

// 0x600710
bool CDecision::HasResponse() {
    for (const auto& task : m_Tasks) {
        if (task != (eTaskType)-1) {
            return true;
        }
    }
    return false;
}

// 0x600600
void CDecision::Add(int32 task, float* probs, int32* bools) {
    for (auto i = 0; i < MAX_NUM_CHOICES; i++) {
        if (m_Tasks[i] != (eTaskType)-1) {
            continue;
        }
        bool probsEmpty = true;
        for (auto j = 0; j < 4; j++) {
            if (m_Probs[i][j] != 0) {
                probsEmpty = false;
                break;
            }
        }
        if (!probsEmpty) {
            continue;
        }
        if (m_Bools[i][0] || m_Bools[i][1]) {
            continue;
        }
        m_Tasks[i] = (eTaskType)task;
        for (auto j = 0; j < 4; j++) {
            m_Probs[i][j] = (uint8)probs[j];
        }
        m_Bools[i][0] = bools[0] != 0;
        m_Bools[i][1] = bools[1] != 0;
        return;
    }
}

// 0x6007A0 - NOTSA: Unnamed helper selecting a task by seek-match or weighted chance
static void SelectDecision(const int32* tasks, const float* chances, int32 count, int32 taskToSeek, int16& outTask, int32& outIndex) {
    outTask = (int16)TASK_NONE;
    outIndex = -1;
    if (taskToSeek != -1) {
        for (auto i = 0; i < count; i++) {
            if (tasks[i] == taskToSeek) {
                outTask = (int16)tasks[i];
                outIndex = i;
                break;
            }
        }
    }
    if (outTask == (int16)TASK_NONE) {
        const auto rnd = (float)CGeneral::GetRandomNumber() * (1.f / (float)RAND_MAX);
        for (auto i = 0; i < count; i++) {
            if (rnd <= chances[i]) { // Original advances while `rnd > chance` (ordered compare), so equal selects
                outTask = (int16)tasks[i];
                outIndex = i;
                break;
            }
        }
    }
}

// 0x6040D0
void CDecision::MakeDecision(int32 eventType, bool bIsPedInVehicle, int32 taskToAvoid1, int32 taskToAvoid2, int32 taskToAvoid3, int32 taskToSeek, int16& outTask, int16& outFacialTask) {
    // NOTSA: `eventType` selects the probability column (0..3, relationship/source type),
    //        `bIsPedInVehicle` selects the bools column. See callers 0x606E70/0x606F80.
    int32 tasks[MAX_NUM_CHOICES];
    uint8 weights[MAX_NUM_CHOICES]{};
    float cumulative[MAX_NUM_CHOICES]{};
    for (auto i = 0; i < MAX_NUM_CHOICES; i++) {
        tasks[i] = (int32)TASK_NONE;
    }
    int32 numValid = 0;
    for (auto i = 0; i < MAX_NUM_CHOICES; i++) {
        if (!m_Bools[i][bIsPedInVehicle]) {
            continue;
        }
        const auto task = (int32)m_Tasks[i]; // Original compares as int (disasm CMP EDX,-1); -1 = empty slot
        if (task == -1 || task == taskToAvoid1 || task == taskToAvoid2 || task == taskToAvoid3) {
            continue;
        }
        if (m_Probs[i][eventType] == 0) {
            continue;
        }
        tasks[numValid] = task;
        weights[numValid] = m_Probs[i][eventType];
        numValid++;
    }
    outTask = (int16)TASK_NONE;
    if (numValid <= 0) {
        outFacialTask = -1;
        return;
    }
    // Original always normalizes all 6 slots (padding weights are 0, so padding cumulative == total)
    float total = 0.f;
    for (auto i = 0; i < MAX_NUM_CHOICES; i++) {
        total += (float)weights[i];
        cumulative[i] = total;
    }
    const auto scale = 1.f / total;
    for (auto i = 0; i < MAX_NUM_CHOICES; i++) {
        cumulative[i] *= scale;
    }
    int32 selectedIndex = -1; // 0x6007A0 output, discarded (facial output is always -1)
    SelectDecision(tasks, cumulative, MAX_NUM_CHOICES, taskToSeek, outTask, selectedIndex);
    outFacialTask = -1;
}
