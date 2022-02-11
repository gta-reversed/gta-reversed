#include "StdInc.h"

#include "EventDraggedOutCar.h"

void CEventDraggedOutCar::InjectHooks() {
    RH_ScopedClass(CEventDraggedOutCar);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AD250);
    RH_ScopedInstall(AffectsPed, 0x4AD3A0);
    RH_ScopedInstall(AffectsPedGroup, 0x4AD3C0);
    RH_ScopedInstall(CloneEditable_Reversed, 0x4B6DC0);
}

// // 0x4AD250
CEventDraggedOutCar::CEventDraggedOutCar(CVehicle* vehicle, CPed* carjacker, bool IsDriverSeat) {
    m_carjacker = carjacker;
    m_vehicle = vehicle;
    m_IsDriverSeat = IsDriverSeat;
    if (m_vehicle)
        m_vehicle->RegisterReference(reinterpret_cast<CEntity**>(&m_vehicle));
    if (m_carjacker)
        m_carjacker->RegisterReference(reinterpret_cast<CEntity**>(&m_carjacker));
}

CEventDraggedOutCar::~CEventDraggedOutCar() {
    if (m_vehicle)
        m_vehicle->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_vehicle));
    if (m_carjacker)
        m_carjacker->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_carjacker));
}

CEventDraggedOutCar* CEventDraggedOutCar::Constructor(CVehicle* vehicle, CPed* carjacker, bool IsDriverSeat) {
    this->CEventDraggedOutCar::CEventDraggedOutCar(vehicle, carjacker, IsDriverSeat);
    return this;
}

// 0x4AD3A0
bool CEventDraggedOutCar::AffectsPed(CPed* ped) {
    return ped->IsAlive() && m_carjacker;
}

// 0x4AD3C0
bool CEventDraggedOutCar::AffectsPedGroup(CPedGroup* pedGroup) {
    return FindPlayerPed() == pedGroup->m_groupMembership.GetLeader();
}

// 0x4B6DC0
CEventEditableResponse* CEventDraggedOutCar::CloneEditable() {
    return CEventDraggedOutCar::CloneEditable_Reversed();
}
CEventEditableResponse* CEventDraggedOutCar::CloneEditable_Reversed() {
    return new CEventDraggedOutCar(m_vehicle, m_carjacker, m_IsDriverSeat);
}
