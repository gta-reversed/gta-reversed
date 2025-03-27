#pragma once

#include "TaskComplex.h"
#include "Vector.h"
class Vehicle;
class Entity;

class CVehicle;
class CEntity;

class NOTSA_EXPORT_VTABLE CTaskGangHassleVehicle : public CTaskComplex {
public:
    uint8     gapC[4];
    CVehicle* m_Vehicle;
    int32     m_nHasslePosId;
    uint8     byte18;
    float     dword1C;
    float     m_fOffsetX;
    CVector   m_vecPosn;
    bool      m_bRemoveAnim;
    bool      m_b31;
    CEntity*  m_pEntity;
    uint32    m_nTime;
    int32     dword3C;
    bool      byte40;
    uint8     byte41;

public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_GANG_HASSLE_VEHICLE;

    CTaskGangHassleVehicle(CVehicle* vehicle, int32 a3, uint8 a4, float a5, float a6);
    ~CTaskGangHassleVehicle() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskGangHassleVehicle(m_Vehicle, m_nHasslePosId, byte18, dword1C, m_fOffsetX); } // 0x65FC00;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    float GetTargetHeading(CPed* ped);
    void CalcTargetOffset();

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskGangHassleVehicle* Constructor(CVehicle* vehicle, int32 a3, uint8 a4, float a5, float a6) { this->CTaskGangHassleVehicle::CTaskGangHassleVehicle(vehicle, a3, a4, a5, a6); return this; }
    CTaskGangHassleVehicle* Destructor() { this->CTaskGangHassleVehicle::~CTaskGangHassleVehicle(); return this; }
};
VALIDATE_SIZE(CTaskGangHassleVehicle, 0x44);
