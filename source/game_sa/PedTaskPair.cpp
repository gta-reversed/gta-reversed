#include "StdInc.h"

#include "PedTaskPair.h"

void CPedTaskPair::InjectHooks() {
    RH_ScopedClass(CPedTaskPair);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Flush, 0x5E95B0);
}

// 0x5E95B0
void CPedTaskPair::Flush() {
    m_pPed = nullptr;
    delete m_pTask;
    m_pTask = nullptr;
    m_taskSlot = -1;
}
