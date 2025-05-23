#include "StdInc.h"

#include "DecisionMaker.h"

// 0x4650A0
CDecisionMaker::CDecisionMaker() {
    for (auto& decision : m_aDecisions) {
        decision.SetDefault();
    }
}
