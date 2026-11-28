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

class NOTSA_EXPORT_VTABLE CBmx : public CBike {
public:
    static constexpr auto Type = VEHICLE_TYPE_BMX;

    CBmx(int32 modelIndex, eVehicleCreatedBy createdBy);
    ~CBmx() override;

    void  BlowUpCar(CEntity* damager, bool hideExplosion) override;
    bool  SetUpWheelColModel(CColModel* wheelCol) override;
    bool  BurstTyre(uint8 tyreComponentId, bool physicalEffect) override;
    void  ProcessControl() override;
    void  ProcessDrivingAnims(CPed* driver, bool blend) override;
    float FindWheelWidth(bool rear) override;
    void  PreRender() override;
    bool  ProcessAI(uint32& extraHandlingFlags) override;

    void GetFrameOffset(float& zOffset, float& angleOffset);
    void ProcessBunnyHop();

    float GetControlJump() const { return m_ControlJump; } // NOTSA

    static void InjectHooks();

private:
    static void LaunchBunnyHopCB(CAnimBlendAssociation* assoc, void* data);

    CBmx* Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) { this->CBmx::CBmx(modelIndex, createdBy); return this; }
    CBmx* Destructor() { this->CBmx::~CBmx(); return this; }

private:
    float m_ControlJump{};     //!< How long the bunny hop has been charged for (In time steps, up to 25)
    float m_ControlPedaling{}; //!< How hard the rider pedals (Above 5 they sprint)
    float m_SprintLeanAngle{}; //!< Sideways sway of the frame while sprinting
    float m_CrankAngle{};
    float m_PedalAngleL{};
    float m_PedalAngleR{};
    float m_MidWheelDistY{}; //!< Distance between the 2 wheels
    float m_MidWheelFracY{}; //!< Front wheel's Y position as a fraction of `m_MidWheelDistY`
    bool  m_IsFreewheeling{};
};
VALIDATE_SIZE(CBmx, 0x838);
