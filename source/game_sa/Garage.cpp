#include "StdInc.h"

void CGarage::InjectHooks()
{
    using namespace ReversibleHooks;
    Install("CGarage", "BuildRotatedDoorMatrix", 0x4479F0, &CGarage::BuildRotatedDoorMatrix);
    Install("CGarage", "StoreAndRemoveCarsForThisHideOut", 0x449900, &CGarage::StoreAndRemoveCarsForThisHideOut);
}

// 0x4479F0
void CGarage::BuildRotatedDoorMatrix(CEntity* pEntity, float fDoorPosition)
{
    const auto fAngle = fDoorPosition * -HALF_PI;
    const auto fSin = sin(fAngle);
    const auto fCos = cos(fAngle);
    CMatrix& matrix = pEntity->GetMatrix();

    const auto& vecForward = matrix.GetForward();
    matrix.GetUp() = CVector(-fSin * vecForward.y, fSin * vecForward.x, fCos);
    matrix.GetRight() = CrossProduct(vecForward, matrix.GetUp());
}

// 0x449900
void CGarage::StoreAndRemoveCarsForThisHideOut(CStoredCar* storedCars, int32 maxSlot)
{
    maxSlot = std::min<int32>(maxSlot, NUM_GARAGE_STORED_CARS);

    for (auto i = 0; i < NUM_GARAGE_STORED_CARS; i++)
        storedCars[i].Clear();

    auto pool = CPools::GetVehiclePool();
    auto storedCarIdx{0u};
    for (auto i = pool->GetSize(); i; i--) {
        if (auto vehicle = pool->GetAt(i - 1)) {
            if (IsPointInsideGarage(vehicle->GetPosition()) && vehicle->m_nCreatedBy != MISSION_VEHICLE) {
                if (storedCarIdx < maxSlot && !EntityHasASphereWayOutsideGarage(vehicle, 1.0f)) {
                    storedCars[storedCarIdx++].StoreCar(vehicle);
                }

                FindPlayerInfo().CancelPlayerEnteringCars(vehicle);
                CWorld::Remove(vehicle);
                delete vehicle;
            }
        }
    }

    // Clear slots with no vehicles in it
    for (auto i = storedCarIdx; i < NUM_GARAGE_STORED_CARS; i++)
        storedCars[i].Clear();
}

// 0x449050
bool CGarage::EntityHasASphereWayOutsideGarage(CEntity* pEntity, float fRadius)
{
    return plugin::CallMethodAndReturn<bool, 0x449050, CGarage*, CEntity*, float>(this, pEntity, fRadius);
}

// 0x448740
bool CGarage::IsPointInsideGarage(CVector vecPoint)
{
    if (vecPoint.z < m_vPosn.z || vecPoint.z > m_fTopZ)
        return false;

    auto xDiff = vecPoint.x - m_vPosn.x;
    auto yDiff = vecPoint.y - m_vPosn.y;

    auto diffDirA = xDiff * m_vDirectionA.x + yDiff * m_vDirectionA.y;
    auto diffDirB = xDiff * m_vDirectionB.x + yDiff * m_vDirectionB.y;

    bool betweenA = diffDirA >= 0.0f && diffDirA <= m_fWidth;
    bool betweenB = diffDirB >= 0.0f && diffDirB <= m_fHeight;

    return betweenA && betweenB;
}

// todo move
// 0x449760
void CStoredCar::StoreCar(CVehicle* pVehicle)
{
    plugin::CallMethod<0x449760, CStoredCar*, CVehicle*>(this, pVehicle);
}
