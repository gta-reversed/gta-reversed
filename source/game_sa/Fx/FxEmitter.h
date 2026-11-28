#pragma once

#include "FxPrim.h"

struct EmissionInfo_t;
class FxEmitterPrt_c;

class NOTSA_EXPORT_VTABLE FxEmitter_c : public FxPrim_c {
public:
    FxEmitter_c()           = default; // 0x4A1920
    ~FxEmitter_c() override = default; // 0x4A2BB0

    bool Init(FxPrimBP_c* primBP, FxSystem_c* system) override;
    void Update(float currentTime, float deltaTime) override;
    void Reset() override;
    void AddParticle(const RwMatrix& mat, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal) override;
    void AddParticle(const CVector& pos, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal) override;

    void            CreateParticles(float currentTime, float deltaTime);
    FxEmitterPrt_c* CreateParticle(const EmissionInfo_t& emissionInfo, RwMatrix& wldMat, const CVector* velOverride, float timeSince, const FxPrtMult_c& fxMults, float brightness, bool createLocal);

    static void InjectHooks();

private:
    void GetEmitterMatrix(RwMatrix& out, const RwMatrix& local) const;
    void EmitParticle(const RwMatrix& local, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal);

private:
    float m_EmissionIntensity{}; //!< Particles owed to the next `CreateParticles` call, the fraction is carried over
};
VALIDATE_SIZE(FxEmitter_c, 0x14);
