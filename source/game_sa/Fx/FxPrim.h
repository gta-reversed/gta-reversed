#pragma once

class FxPrimBP_c;
class FxSystem_c;

class NOTSA_EXPORT_VTABLE FxPrim_c {
public:
    FxPrimBP_c* m_PrimBP;
    FxSystem_c* m_System;
    bool        m_bEnabled;

public:
    FxPrim_c();
    virtual ~FxPrim_c() = 0;

    virtual bool Init(FxPrimBP_c* prim, FxSystem_c* system) = 0;
    virtual void Update(float a2, float a3) = 0;
    virtual void Reset() = 0;
    virtual void AddParticle(RwMatrix* mat, CVector* vel, float timeSince, FxPrtMult_c* fxMults, float rotZ, float brightness, bool createLocal) = 0;
    virtual void AddParticle(CVector* pos, CVector* vel, float timeSince, FxPrtMult_c* fxMults, float rotZ, float brightness, bool createLocal) = 0;

    void Enable(bool enabled);
};
