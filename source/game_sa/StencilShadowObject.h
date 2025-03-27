#pragma once

enum class eStencilShadowObjType : uint8 {
    NONE,
    OBJECT,
    VEHICLE
};

class CStencilShadowObject {
public:
    CEntity*              m_pOwner;
    int16                 m_NumShadowFaces;
    eStencilShadowObjType m_Type;
    uint32                m_FaceID;
    uint32                m_SizeOfShadowFacesData;
    CVector*              m_ShadowFacesData;
    CStencilShadowObject* m_pNext;
    CStencilShadowObject* m_pPrev;

public:
    static void InjectHooks();

    CStencilShadowObject() = default; // 0x70F8D0
    ~CStencilShadowObject() = default; // 0x70F8E0

    void Destroy();

private:
    CStencilShadowObject* Constructor() {
        this->CStencilShadowObject::CStencilShadowObject();
        return this;
    }

    CStencilShadowObject* Destructor() {
        this->CStencilShadowObject::~CStencilShadowObject();
        return this;
    }
};

VALIDATE_SIZE(CStencilShadowObject, 0x1C);
