#include "StdInc.h"

#include "StencilShadowObject.h"

void CStencilShadowObject::InjectHooks() {
    RH_ScopedClass(CStencilShadowObject);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x70F8D0);
    RH_ScopedInstall(Destructor, 0x70F8E0);
    RH_ScopedInstall(Destroy, 0x711310);
}

// 0x711310
void CStencilShadowObject::Destroy() {
    CEntity::SafeCleanUpRef(m_pOwner);
    m_pOwner                = nullptr;
    m_Type                  = eStencilShadowObjType::NONE;
    m_SizeOfShadowFacesData = 0;
    m_FaceID                = 0;

    if (m_ShadowFacesData) {
        CMemoryMgr::Free(std::exchange(m_ShadowFacesData, nullptr));
    }

    if (m_pPrev) {
        if (m_pNext) {
            m_pNext->m_pPrev = m_pPrev;
            m_pPrev->m_pNext = m_pNext;
        } else {
            m_pPrev->m_pNext = nullptr;
        }
    } else {
        CStencilShadows::pFirstActiveStencilShadowObject = m_pNext;
        if (CStencilShadows::pFirstActiveStencilShadowObject) {
            CStencilShadows::pFirstActiveStencilShadowObject->m_pPrev = nullptr;
        }
    }
    m_pNext = CStencilShadows::pFirstAvailableStencilShadowObject;
    m_pPrev = nullptr;
    CStencilShadows::pFirstAvailableStencilShadowObject = this;

    if (m_pNext) {
        m_pNext->m_pPrev = this;
    }
}
