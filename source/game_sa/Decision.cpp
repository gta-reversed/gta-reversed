#include "StdInc.h"

#include "Decision.h"

void CDecision::InjectHooks() {

}

// 0x6040C0
CDecision::CDecision() {
    SetDefault();
}

// 0x600530
void CDecision::SetDefault() {
    plugin::CallMethod<0x600530, CDecision*>(this);
}

/*
// 0x6040D0
void CDecision::MakeDecision(int32, bool, int32, int32, int32, int32, int16&, int16&) {

}

// 0x600710
bool CDecision::HasResponse() {

}

// 0x6006B0
void CDecision::From(const CDecision& rhs) {

}

// 0x600600
void CDecision::Add(int32, float*, int32*) {

}

// 0x600570
void CDecision::Set(int32*, float (*)[4], int32 (*)[2], float (*)[6]) {

}
*/