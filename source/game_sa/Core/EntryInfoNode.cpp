#include "StdInc.h"

#include "EntryInfoNode.h"

void CEntryInfoNode::InjectHooks()
{
    RH_ScopedClass(CEntryInfoNode);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(operator new, 0x536DC0, {.locked = true}); // Crashes the game if unhooked
    RH_ScopedInstall(operator delete, 0x536DD0, {.locked = true});
}

void* CEntryInfoNode::operator new(unsigned size)
{
    return GetEntryInfoNodePool()->New();
}

void CEntryInfoNode::operator delete(void* ptr, size_t sz)
{
    GetEntryInfoNodePool()->Delete(reinterpret_cast<CEntryInfoNode*>(ptr));
}

void CEntryInfoNode::AddToList(CEntryInfoNode* next)
{
    m_previous = nullptr;
    m_next = next;
    if (next) {
        next->m_previous = this;
    }
}
