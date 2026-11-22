#include "StdInc.h"

#include "FxPrim.h"

// 0x4A9F20
FxPrim_c::FxPrim_c() {
    m_PrimBP   = nullptr;
    m_System   = nullptr;
    m_bEnabled = true;
}

// 0x4A9F40
FxPrim_c::~FxPrim_c() {
    m_PrimBP = nullptr;
    m_System = nullptr;
}

// 0x4A9F50
void FxPrim_c::Enable(bool enabled) {
    m_bEnabled = enabled;
}
