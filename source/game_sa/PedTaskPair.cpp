#include "StdInc.h"

#include "PedTaskPair.h"

void CPedTaskPair::InjectHooks() {
    RH_ScopedClass(CPedTaskPair);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Flush, 0x5E95B0);
}

// 0x5E95B0
void CPedTaskPair::Flush() {
    m_Ped = nullptr;
    delete m_Task;
    m_Task = nullptr;
    m_Slot = -1;
}
