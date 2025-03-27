#include "StdInc.h"

#include "PtrListDoubleLink.h"

void CPtrListDoubleLink::InjectHooks() {
    RH_ScopedClass(CPtrListDoubleLink);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(Flush, 0x552470);
    RH_ScopedInstall(AddItem, 0x533670);
    RH_ScopedInstall(DeleteItem, 0x5336B0);
}

void CPtrListDoubleLink::Flush() {
    if (!m_node)
        return;

    CPtrNodeDoubleLink* curNode;
    while ((curNode = GetNode()))
        CPtrListDoubleLink::DeleteNode(curNode);
}

CPtrNodeDoubleLink* CPtrListDoubleLink::AddItem(void* item) {
    auto link = new CPtrNodeDoubleLink{ item };
    link->AddToList(this);
    return link;
}

void CPtrListDoubleLink::DeleteItem(void* item) {
    if (!m_node)
        return;

    auto* node = GetNode();
    while (node->m_item != item) {
        node = reinterpret_cast<CPtrNodeDoubleLink*>(node->GetNext());
        if (!node) {
            return;
        }
    }
    CPtrListDoubleLink::DeleteNode(node);
}

void CPtrListDoubleLink::DeleteNode(CPtrNodeDoubleLink* node) {
    if (GetNode() == node) {
        m_node = m_node->m_next;
    }

    if (node->m_prev) {
        node->m_prev->m_next = node->m_next;
    }

    if (node->m_next) {
        node->m_next->m_prev = node->m_prev;
    }

    delete node;
}
