/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Bike.h"
#include "AnimBlendAssociation.h"

enum eBmxNodes {
    BMX_NODE_NONE   = 0,
    BMX_CHASSIS     = 1,
    BMX_FORKS_FRONT = 2,
    BMX_FORKS_REAR  = 3,
    BMX_WHEEL_FRONT = 4,
    BMX_WHEEL_REAR  = 5,
    BMX_HANDLEBARS  = 6,
    BMX_CHAINSET    = 7,
    BMX_PEDAL_R     = 8,
    BMX_PEDAL_L     = 9,

    BMX_NUM_NODES
};

class CBmx : public CBike {
public:
    float m_fTimeStep;         // todo: change name; related to time step
    float m_fBaseAnimLean;     // always 0.0f
    float m_fRotPedalChainset; // rotX for BMX_CHAINSET
    float m_fRotPedalR;        // rotX BMX_PEDAL_R
    float m_fRotPedalL;        // rotX BMX_PEDAL_L
    float m_fDistanceBetweenWheels;
    float m_fWheelsBalance;
    float m_f830; // see CBmx::GetFrameOffset, ctor
    bool  m_b831;

public:
    CBmx(plugin::dummy_func_t) : CBike(plugin::dummy) { /* todo: remove NOTSA */ }
    CBmx(int32 modelIndex, eVehicleCreatedBy createdBy);
    ~CBmx() override;

    void BlowUpCar(CEntity* damager, uint8 bHideExplosion) override;
    bool SetUpWheelColModel(CColModel* wheelCol) override;
    bool BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) override;
    void ProcessControl() override;
    void ProcessDrivingAnims(CPed* driver, uint8 bBlend) override;
    float FindWheelWidth(bool bRear) override;
    void PreRender() override;
    bool ProcessAI(uint32& extraHandlingFlags) override;

    void GetFrameOffset(float& a1, float& a2);
    void ProcessBunnyHop();
    static void LaunchBunnyHopCB(CAnimBlendAssociation* blendAssoc, void* data); // data is a ptr to CBmx

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CBmx* Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) { this->CBmx::CBmx(modelIndex, createdBy); return this; }
    CBmx* Destructor() { this->CBmx::~CBmx(); return this; }

    void BlowUpCar_Reversed(CEntity* damager, uint8 bHideExplosion) { CBmx::BlowUpCar(damager, bHideExplosion); }
    bool SetUpWheelColModel_Reversed(CColModel* wheelCol) { return CBmx::SetUpWheelColModel(wheelCol); }
    bool BurstTyre_Reversed(uint8 tyreComponentId, bool bPhysicalEffect) { return CBmx::BurstTyre(tyreComponentId, bPhysicalEffect); }
    void ProcessControl_Reversed() { CBmx::ProcessControl(); }
    void ProcessDrivingAnims_Reversed(CPed* driver, uint8 bBlend) { CBmx::ProcessDrivingAnims(driver, bBlend); }
    float FindWheelWidth_Reversed(bool bRear) { return CBmx::FindWheelWidth(bRear); }
    void PreRender_Reversed() { CBmx::PreRender(); }
    bool ProcessAI_Reversed(uint32& extraHandlingFlags) { return CBmx::ProcessAI(extraHandlingFlags); }
};

VALIDATE_SIZE(CBmx, 0x838);
