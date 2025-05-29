/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CEntity;
class CFire;
class FxSystem_c;

class CFire {
public:
    static void InjectHooks();

    CFire();
    ~CFire() = default;
    CFire* Constructor();

    void Initialise();
    void Start(CEntity* creator, CVector pos, uint32 nTimeToBurn, uint8 nGens);
    void Start(CEntity* creator, CEntity* target, uint32 nTimeToBurn, uint8 nGens);
    void Start(CVector pos, float fStrength, CEntity* target, uint8 nGens); /* For script */
    void CreateFxSysForStrength(const CVector& point, RwMatrix* matrix);
    void Extinguish();
    void ExtinguishWithWater(float fWaterStrength);
    void ProcessFire();

    // Inlined
    bool IsScript() const { return m_IsCreatedByScript; }
    void SetIsScript(bool b) { m_IsCreatedByScript = b; }

    bool IsFirstGen() const { return m_IsFirstGeneration; }
    void SetIsFirstGen(bool b) { m_IsFirstGeneration = b; }

    bool IsActive() const { return m_IsActive; }
    bool IsBeingExtinguished() const { return m_IsBeingExtinguished; }

    bool MakesNoise() const { return m_MakesNoise; }
    void SetMakesNoise(bool b) { m_MakesNoise = b; }

    auto GetStrength() const { return m_Strength; }

    // NOTSA funcs:
    auto GetFireParticleNameForStrength() const;
    void DestroyFx();

    auto GetEntityOnFire() const { return m_EntityOnFire; }
    void SetEntityOnFire(CEntity* target);

    auto GetEntityStartedFire() const { return m_EntityStartedFire; }
    void SetEntityStartedFire(CEntity* creator);

    bool HasTimeToBurn() const;
    bool IsNotInRemovalDistance() const;
    auto& GetPosition() const { return m_Position; }

    //! Script thing ID
    auto& GetId(this auto&& self) { return self.m_ScriptReferenceIndex; }

private:
    bool        m_IsActive : 1;
    bool        m_IsCreatedByScript : 1;
    bool        m_MakesNoise : 1;
    bool        m_IsBeingExtinguished : 1;
    bool        m_IsFirstGeneration : 1;
    int16       m_ScriptReferenceIndex;
    CVector     m_Position;
    CEntity*    m_EntityOnFire;
    CEntity*    m_EntityStartedFire;
    uint32      m_TimeToBurn;
    float       m_Strength;
    uint8       m_NumGenerationsAllowed;
    uint8       m_RemovalDist;
    FxSystem_c* m_FxSystem;
};
VALIDATE_SIZE(CFire, 0x28);
