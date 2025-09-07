/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Physical.h"
#include "CarCtrl.h"
#include "Glass.h"
#include "TaskSimpleClimb.h"
#include "RealTimeShadowManager.h"

float HIGHSPEED_ELASTICITY_MULT_COPCAR = 2.0f; // 0x8CD784

float PHYSICAL_SHIFT_SPEED_DAMP = 0.707f; // 0x8CD788

float SOFTCOL_DEPTH_MIN = 0.5f; // 0x8CD78C
float SOFTCOL_DEPTH_MULT = 2.0f; // 0x8CD790
float SOFTCOL_SPEED_MULT = 0.95f; // 0x8CD794
float SOFTCOL_SPEED_MULT2 = 1.05f; // 0x8CD798
float SOFTCOL_CARLINE_SPEED_MULT = 0.4f; // 0x8CD79C

float DAMPING_LIMIT_IN_FRAME = 0.25f; // 0x8CD7A0
float DAMPING_LIMIT_OF_SPRING_FORCE = 0.999f; // 0x8CD7A4

float PHYSICAL_CAR_FRICTION_MULT = 150.0f; // 0x8CD7A8
float PHYSICAL_CARCAR_FRICTION_MULT = 1.0f; // 0x8CD7AC
float PHYSICAL_CAR_FRICTION_SPEED_LIMSQR = 0.02f; // 0x8CD7B0
float PHYSICAL_CAR_FRICTION_TURN_LIMSQR = 0.01f; // 0x8CD7B4

float TEST_ADD_AMBIENT_LIGHT_FRAC = 0.5f; // 0x8CD7B8

float TEST_AI_LIGHTING_DAY_MULT = 1.0f; // 0x8CD7BC
float TEST_AI_LIGHTING_DAY_ADD = 0.1f; // 0x8CD7C0

CVector& fxDirection = *(CVector*)0xB73720;

void CPhysical::InjectHooks() {
    RH_ScopedVirtualClass(CPhysical, 0x863BA0, 23);
    RH_ScopedCategory("Entity");

    RH_ScopedInstall(Constructor, 0x542260);
    RH_ScopedInstall(Destructor, 0x542450);

    RH_ScopedInstall(RemoveAndAdd, 0x542560, {.enabled=false, .locked=true});
    RH_ScopedOverloadedInstall(ApplyTurnForce, "xyz", 0x65DA70, void(CPhysical::*)(float, float, float, float, float, float));
    RH_ScopedOverloadedInstall(ApplyTurnForce, "vec", 0x542A50, void(CPhysical::*)(CVector, CVector));
    RH_ScopedOverloadedInstall(ApplyForce, "vec", 0x542B50, void(CPhysical::*)(CVector, CVector, bool));
    RH_ScopedOverloadedInstall(GetSpeed, "vec", 0x542CE0, CVector(CPhysical::*)(CVector));
    RH_ScopedInstall(ApplyMoveSpeed, 0x542DD0);
    RH_ScopedInstall(ApplyTurnSpeed, 0x542E20);
    RH_ScopedOverloadedInstall(ApplyMoveForce, "vec", 0x5429F0, void(CPhysical::*)(CVector force));
    RH_ScopedInstall(SetDamagedPieceRecord, 0x5428C0);
    RH_ScopedInstall(RemoveFromMovingList, 0x542860);
    RH_ScopedInstall(AddToMovingList, 0x542800);
    RH_ScopedVMTInstall(Add, 0x544A30, {.enabled=false, .locked=true});
    RH_ScopedVMTInstall(Remove, 0x5424C0);
    RH_ScopedVMTInstall(GetBoundRect, 0x5449B0);
    RH_ScopedVMTInstall(ProcessControl, 0x5485E0);
    RH_ScopedVMTInstall(ProcessCollision, 0x54DFB0);
    RH_ScopedVMTInstall(ProcessShift, 0x54DB10);
    RH_ScopedVMTInstall(TestCollision, 0x54DEC0);
    RH_ScopedVMTInstall(ProcessEntityCollision, 0x546D00);
    RH_ScopedInstall(ApplyGravity, 0x542FE0);
    RH_ScopedOverloadedInstall(ApplyFrictionTurnForce, "vec", 0x543100, void(CPhysical::*)(CVector, CVector));
    RH_ScopedOverloadedInstall(ApplyFrictionMoveForce, "vec", 0x5430A0, void(CPhysical::*)(CVector));
    RH_ScopedOverloadedInstall(ApplyFrictionForce, "vec", 0x543220, void(CPhysical::*)(CVector, CVector));
    RH_ScopedInstall(SkipPhysics, 0x5433B0);
    RH_ScopedInstall(AddCollisionRecord, 0x543490);
    RH_ScopedInstall(GetHasCollidedWith, 0x543540);
    RH_ScopedInstall(GetHasCollidedWithAnyObject, 0x543580);
    RH_ScopedOverloadedInstall(ApplyCollision, "1", 0x5435C0, bool(CPhysical::*)(CEntity*, const CColPoint&, float&));
    RH_ScopedOverloadedInstall(ApplySoftCollision, "1", 0x543890, bool(CPhysical::*)(CEntity*, const CColPoint&, float&));
    RH_ScopedInstall(ApplySpringCollision, 0x543C90);
    RH_ScopedInstall(ApplySpringCollisionAlt, 0x543D60);
    RH_ScopedInstall(ApplySpringDampening, 0x543E90);
    RH_ScopedInstall(RemoveRefsToEntity, 0x544280);
    RH_ScopedInstall(DettachEntityFromEntity, 0x5442F0);
    RH_ScopedInstall(DettachAutoAttachedEntity, 0x5446A0);
    RH_ScopedInstall(GetLightingFromCol, 0x5447B0);
    RH_ScopedInstall(GetLightingTotal, 0x544850);
    RH_ScopedInstall(CanPhysicalBeDamaged, 0x5448B0);
    RH_ScopedInstall(ApplyAirResistance, 0x544C40);
    RH_ScopedInstall(ApplyCollisionAlt, 0x544D50);
    RH_ScopedOverloadedInstall(ApplyFriction, "self", 0x5454C0, bool(CPhysical::*)(float, CColPoint&));
    RH_ScopedOverloadedInstall(ApplyFriction, "other", 0x545980, bool(CPhysical::*)(CPhysical*, float, CColPoint&));
    RH_ScopedInstall(ProcessShiftSectorList, 0x546670);
    RH_ScopedInstall(nullsub_496, 0x546CEC);
    RH_ScopedInstall(ApplySpeed, 0x547B80);
    RH_ScopedInstall(UnsetIsInSafePosition, 0x548320);
    RH_ScopedOverloadedInstall(ApplyFriction, "void", 0x5483D0, void(CPhysical::*)());
    RH_ScopedOverloadedInstall(ApplyCollision, "2", 0x548680, bool(CPhysical::*)(CEntity*, CColPoint&, float&, float&));
    RH_ScopedOverloadedInstall(ApplySoftCollision, "2", 0x54A2C0, bool(CPhysical::*)(CPhysical*, CColPoint&, float&, float&));
    RH_ScopedInstall(ProcessCollisionSectorList, 0x54BA60);
    RH_ScopedInstall(ProcessCollisionSectorList_SimpleCar, 0x54CFF0);
    RH_ScopedOverloadedInstall(AttachEntityToEntity, "vec", 0x54D570, void(CPhysical::*)(CPhysical*, CVector, CVector));
    RH_ScopedOverloadedInstall(AttachEntityToEntity, "quat", 0x54D690, void(CPhysical::*)(CPhysical*, CVector*, CQuaternion*));
    RH_ScopedInstall(CheckCollision, 0x54D920);
    RH_ScopedInstall(CheckCollision_SimpleCar, 0x54DAB0);
    RH_ScopedInstall(PlacePhysicalRelativeToOtherPhysical, 0x546DB0);
    RH_ScopedInstall(PositionAttachedEntity, 0x546FF0);
}

// 0x542260
CPhysical::CPhysical() : CEntity() {
    m_pCollisionList.m_node = nullptr;

    CPlaceable::AllocateStaticMatrix();
    m_matrix->SetUnity();

    ResetMoveSpeed();
    ResetTurnSpeed();
    m_vecMoveFriction.Reset();
    m_vecTurnFriction.Reset();
    m_vecForce.Reset();
    m_vecTorque.Reset();

    m_fMass = 1.0f;
    m_fTurnMass = 1.0f;
    m_fVelocityFrequency = 1.0f;
    m_fAirResistance = 0.1f;
    m_pMovingList = nullptr;
    m_nFakePhysics = 0;
    m_nNumEntitiesCollided = 0;
    std::ranges::fill(m_apCollidedEntities, nullptr);

    m_nPieceType = 0;

    m_fDamageIntensity = 0.0f;
    m_pDamageEntity = nullptr;

    m_vecLastCollisionImpactVelocity.Reset();
    m_vecLastCollisionPosn.Reset();

    m_bUsesCollision = true;

    m_vecCentreOfMass.Reset();

    m_fMovingSpeed = 0.0f;
    m_pAttachedTo = nullptr;
    m_pEntityIgnoredCollision = nullptr;

    m_qAttachedEntityRotation = {};

    m_fDynamicLighting = 0.0f;
    m_pShadowData = nullptr;
    m_prevDistFromCam = 100.0f;

    m_nPhysicalFlags = 0;
    physicalFlags.bApplyGravity = true;

    m_nContactSurface = SURFACE_DEFAULT; // NOTSA; If not initialize, it will go out of bounds in the SurfaceInfos_c
    m_fContactSurfaceBrightness = 1.0f;
}

// 0x542450
CPhysical::~CPhysical() {
    if (m_pShadowData) {
        g_realTimeShadowMan.ReturnRealTimeShadow(m_pShadowData);
    }

    m_pCollisionList.Flush();
}

// 0x544A30
void CPhysical::Add() {
    // TODO: Refactor `CEntryInfoNode` to be templated then use it here
#if 0
    if (m_bIsBIGBuilding) {
        CEntity::Add();
        return;
    }

    const auto boundRect = GetBoundRect();
    CWorld::IterateSectorsOverlappedByRect(
        boundRect,
        [this](int32 sectorX, int32 sectorY) {
            CPtrListDoubleLink* list = nullptr;
            CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);
            switch (m_nType) {
            case ENTITY_TYPE_VEHICLE:
                list = &repeatSector->Vehicles;
                break;
            case ENTITY_TYPE_PED:
                list = &repeatSector->Peds;
                break;
            case ENTITY_TYPE_OBJECT:
                list = &repeatSector->Objects;
                break;
            }

            auto newEntityInfoNode = new CEntryInfoNode();
            if (newEntityInfoNode) {
                newEntityInfoNode->m_doubleLink = list->AddItem(this);
                newEntityInfoNode->m_repeatSector = repeatSector;
                newEntityInfoNode->m_doubleLinkList = list;
            }
            newEntityInfoNode->AddToList(m_pCollisionList.m_node);
            m_pCollisionList.m_node = newEntityInfoNode;
            return true;
        }
    );
#endif
}

// 0x5424C0
void CPhysical::Remove() {
    if (m_bIsBIGBuilding) {
        CEntity::Remove();
    } else {
        for (CEntryInfoNode* node = m_pCollisionList.m_node; node;) {
            CEntryInfoNode* next = node->m_next;
            node->m_doubleLinkList->DeleteNode(node->m_doubleLink);
            m_pCollisionList.DeleteNode(node);
            node = next;
        }
    }
}

// 0x5449B0
CRect CPhysical::GetBoundRect() {
    CVector boundCentre;
    CEntity::GetBoundCentre(&boundCentre);
    const float fRadius = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundRadius();
    return CRect(
        boundCentre.x - fRadius,
        boundCentre.y - fRadius,
        boundCentre.x + fRadius,
        boundCentre.y + fRadius
    );
}

// 0x5485E0
void CPhysical::ProcessControl() {
    if (!IsPed()) {
        physicalFlags.bSubmergedInWater = false;
    }

    m_bHasHitWall = false;
    m_bWasPostponed = false;
    m_bIsInSafePosition = false;
    m_bHasContacted = false;

    if (m_nStatus != STATUS_SIMPLE) {
        physicalFlags.b31 = false;
        physicalFlags.bOnSolidSurface = false;
        m_nNumEntitiesCollided = 0;
        m_nPieceType = 0;
        m_fDamageIntensity = 0.0f;

        CEntity::SafeCleanUpRef(m_pDamageEntity);
        m_pDamageEntity = nullptr;

        ApplyFriction();

        if (!m_pAttachedTo || physicalFlags.bInfiniteMass) {
            ApplyGravity();
            ApplyAirResistance();
        }
    }
}

// 0x54DFB0
void CPhysical::ProcessCollision() {
    ZoneScoped;

    auto* vehicle = AsVehicle();
    auto* automobile = AsAutomobile();
    auto* bike = AsBike();

    m_fMovingSpeed = 0.0f;
    physicalFlags.bProcessingShift = false;
    physicalFlags.bSkipLineCol = false;

    // Invert
    if (!m_bUsesCollision || physicalFlags.bDisableSimpleCollision) {
        // 0x54E87E
        m_bIsStuck = false;
        m_bIsInSafePosition = true;
        RemoveAndAdd();
        return;
    }

    switch (m_nStatus) {
        case STATUS_SIMPLE: {
            // 0x54E00A - Review
            if (CheckCollision_SimpleCar() && m_nStatus == STATUS_SIMPLE) {
                m_nStatus = STATUS_PHYSICS;
                if (IsVehicle()) {
                    CCarCtrl::SwitchVehicleToRealPhysics(vehicle);
                }
            }
            m_bIsStuck = false;
            m_bIsInSafePosition = true;
            RemoveAndAdd();
            return;
        }
        case STATUS_GHOST: {
            CColPoint* wheelsColPoints = nullptr;
            float* pfWheelsSuspensionCompression = nullptr;
            CVector*   wheelsCollisionPositions = nullptr;
            if (vehicle->m_nVehicleSubType) {
                rng::fill(bike->m_aGroundPhysicalPtrs, nullptr);
                wheelsColPoints = bike->m_aWheelColPoints;
                pfWheelsSuspensionCompression = bike->m_aWheelRatios;
                wheelsCollisionPositions = bike->m_aGroundOffsets;
            } else {
                rng::fill(automobile->m_apWheelCollisionEntity, nullptr);

                // TODO: Use array<> for these local variables
                wheelsColPoints = automobile->m_wheelColPoint.data();
                pfWheelsSuspensionCompression = automobile->m_fWheelsSuspensionCompression.data();
                wheelsCollisionPositions = automobile->m_vWheelCollisionPos.data();
            }
            CCollisionData* colData = GetColModel()->m_pColData;
            uint8 collisionIndex = colData->bUsesDisks ? 0u : colData->m_nNumLines;
            for (auto wheelID = 0; wheelID < collisionIndex; ++wheelID) {
                CColLine* line = &colData->m_pLines[wheelID];

                CVector startPosWorld = m_matrix->TransformPoint(line->m_vecStart);
                CVector endPosWorld = m_matrix->TransformPoint(line->m_vecEnd);

                CColPoint& wheelCP = wheelsColPoints[wheelID];
                wheelCP.m_vecNormal = CVector(0.0f, 0.0f, 1.0f);
                wheelCP.m_nSurfaceTypeA = SURFACE_WHEELBASE;
                wheelCP.m_nSurfaceTypeB = SURFACE_TARMAC;
                wheelCP.m_fDepth = 0.0f;

                float ghostRoadZ = CCarCtrl::FindGhostRoadHeight(vehicle);

                if (ghostRoadZ <= startPosWorld.z && ghostRoadZ > endPosWorld.z) {
                    float ratio = (startPosWorld.z - ghostRoadZ) / (startPosWorld.z - endPosWorld.z);
                    pfWheelsSuspensionCompression[wheelID] = ratio;
                    wheelCP.m_vecPoint = startPosWorld + (endPosWorld - startPosWorld) * ratio;
                } else if (ghostRoadZ > startPosWorld.z) {
                    pfWheelsSuspensionCompression[wheelID] = 0.0f;
                    wheelCP.m_vecPoint = startPosWorld;
                } else { // ghostRoadZ <= endPosWorld.z
                    pfWheelsSuspensionCompression[wheelID] = 1.0f;
                    wheelCP.m_vecPoint = endPosWorld;
                }

                wheelsCollisionPositions[wheelID] = wheelCP.m_vecPoint - GetPosition();
            }

            m_bIsStuck = false;
            m_bIsInSafePosition = true;
            m_vecMoveSpeed.z = 0.0f;
            m_vecTurnSpeed.Reset();

            m_matrix->ForceUpVector({ 0.0f, 0.0f, 1.0f });
            GetPosition().z = CCarCtrl::FindGhostRoadHeight(vehicle) + vehicle->GetHeightAboveRoad();

            ApplySpeed();
            m_matrix->Reorthogonalise();
            RemoveAndAdd();
            vehicle->vehicleFlags.bVehicleColProcessed = true;
            return;
        }
    }

    const CVector originalMoveSpeed = m_vecMoveSpeed;
    const float originalTimeStep = CTimer::GetTimeStep();
    const float originalElasticity = m_fElasticity;
    const CVector originalPos = m_matrix->GetPosition();

    CMatrix oldEntityMatrix(*m_matrix);

    bool bProcessCollisionBeforeSettingTimeStep = false;
    bool bFalse = false;

    uint8 numCollisionSteps = this->SpecialEntityCalcCollisionSteps(&bProcessCollisionBeforeSettingTimeStep, &bFalse);
    float fStep = originalTimeStep / numCollisionSteps;

    if (bProcessCollisionBeforeSettingTimeStep) {
        ApplySpeed();
        m_matrix->Reorthogonalise();
        physicalFlags.bProcessingShift = false;
        physicalFlags.bSkipLineCol = false;
        physicalFlags.b17 = true;
        bool bOldUsesCollision = m_bUsesCollision;
        m_bUsesCollision = false;
        if (CheckCollision()) {
            physicalFlags.b17 = false;
            m_bUsesCollision = bOldUsesCollision;
            if (IsVehicle()) {
                vehicle->vehicleFlags.bVehicleColProcessed = true;
            }
            m_bIsStuck = false;
            m_bIsInSafePosition = true;
            physicalFlags.bProcessCollisionEvenIfStationary = false;
            physicalFlags.bSkipLineCol = false;
            m_fElasticity = originalElasticity;
            m_fMovingSpeed = DistanceBetweenPoints(oldEntityMatrix.GetPosition(), GetPosition());
            RemoveAndAdd();
            return;
        }
        m_bUsesCollision = bOldUsesCollision;
        physicalFlags.b17 = false;
        *static_cast<CMatrix*>(m_matrix) = oldEntityMatrix;
        m_vecMoveSpeed = originalMoveSpeed;
        if (IsVehicle() && vehicle->vehicleFlags.bIsLawEnforcer) {
            m_fElasticity *= HIGHSPEED_ELASTICITY_MULT_COPCAR;
        }
    }

    CPed* ped = AsPed();
    if (numCollisionSteps > 1u) {
        for (uint8 stepIndex = 1; stepIndex < numCollisionSteps; stepIndex++) {
            CTimer::UpdateTimeStep(stepIndex * fStep);
            ApplySpeed();
            bool bCheckCollision = CheckCollision();
            if (IsPed() && m_vecMoveSpeed.z == 0.0f && !ped->bWasStanding && ped->bIsStanding) {
                oldEntityMatrix.GetPosition().z = GetPosition().z;
            }
            *static_cast<CMatrix*>(m_matrix) = oldEntityMatrix;
            CTimer::UpdateTimeStep(originalTimeStep);
            if (bCheckCollision) {
                m_fElasticity = originalElasticity;
                return;
            }
            if (IsVehicle()) {
                switch (vehicle->m_nVehicleType) {
                case VEHICLE_TYPE_BIKE:
                    rng::fill(bike->m_aWheelRatios, 1.0f);
                    break;
                case VEHICLE_TYPE_TRAILER:
                    // BUG: maybe there should be a 4 wheel here?
                    automobile->m_fWheelsSuspensionCompression[CAR_WHEEL_FRONT_LEFT]  = 1.0f;
                    automobile->m_fWheelsSuspensionCompression[CAR_WHEEL_REAR_LEFT]   = 1.0f;
                    automobile->m_fWheelsSuspensionCompression[CAR_WHEEL_FRONT_RIGHT] = 1.0f;
                    break;
                case VEHICLE_TYPE_AUTOMOBILE:
                    rng::fill(automobile->m_fWheelsSuspensionCompression, 1.0f);
                    break;
                }
            }
        }
    }

    ApplySpeed();
    m_matrix->Reorthogonalise();
    physicalFlags.bProcessingShift = false;
    physicalFlags.bSkipLineCol = false;
    if (!m_vecMoveSpeed.IsZero()
        || !m_vecTurnSpeed.IsZero()
        || physicalFlags.bProcessCollisionEvenIfStationary
        || m_nStatus == STATUS_PLAYER
        || IsVehicle() && vehicle->vehicleFlags.bIsCarParkVehicle
        || IsPed() && (ped->IsPlayer() || ped->bTestForBlockedPositions || !ped->bIsStanding)) {
        if (IsVehicle()) {
            vehicle->vehicleFlags.bVehicleColProcessed = true;
        }
        if (CheckCollision()) {
            *static_cast<CMatrix*>(m_matrix) = oldEntityMatrix;
            m_fElasticity = originalElasticity;
            return;
        }
    } else if (IsPed()) {
        ped->bIsStanding = true;
    }
    m_bIsStuck = false;
    m_bIsInSafePosition = true;
    physicalFlags.bProcessCollisionEvenIfStationary = false;
    physicalFlags.bSkipLineCol = false;
    m_fElasticity = originalElasticity;
    m_fMovingSpeed = DistanceBetweenPoints(oldEntityMatrix.GetPosition(), GetPosition());
    RemoveAndAdd();
}

// 0x54DB10
void CPhysical::ProcessShift() {
    ZoneScoped;

    CRect boundingBox = GetBoundRect();
    m_fMovingSpeed = 0.0f;

    bool bPhysicalFlagsSet = m_nPhysicalFlags & (PHYSICAL_DISABLE_MOVE_FORCE | PHYSICAL_INFINITE_MASS | PHYSICAL_DISABLE_Z);
    if (m_nStatus == STATUS_SIMPLE || bPhysicalFlagsSet) {
        if (bPhysicalFlagsSet) {
            ResetTurnSpeed();
        }
        m_bIsStuck = false;
        m_bIsInSafePosition = true;
        RemoveAndAdd();
    } else {
        if (m_bHasHitWall) {
            CPed* ped = AsPed();
            bool bSomeSpecificFlagsSet = false;
            if (IsPed() && ped->m_standingOnEntity) {
                if (!ped->m_standingOnEntity->m_bIsStatic || ped->m_standingOnEntity->m_bHasContacted) {
                    bSomeSpecificFlagsSet = true;
                }
            }
            if ((IsPed() && bSomeSpecificFlagsSet) || CWorld::bSecondShift) {
                float fMoveSpeedShift = pow(PHYSICAL_SHIFT_SPEED_DAMP, CTimer::GetTimeStep());
                m_vecMoveSpeed *= fMoveSpeedShift;
                float fTurnSpeedShift = pow(PHYSICAL_SHIFT_SPEED_DAMP, CTimer::GetTimeStep());
                m_vecTurnSpeed *= fTurnSpeedShift;
            }
        }

        CMatrix oldEntityMatrix(*m_matrix);
        ApplySpeed();
        m_matrix->Reorthogonalise();
        CWorld::IncrementCurrentScanCode();

        bool bShifted = false;
        if (IsVehicle()) {
            physicalFlags.bProcessingShift = true;
        }

        CWorld::IterateSectorsOverlappedByRect(
            boundingBox,
            [this, &bShifted](int32 sectorX, int32 sectorY) {
                if (ProcessShiftSectorList(sectorX, sectorY)) {
                    bShifted = true;
                    return false;
                }
                return true;
            }
        );

        physicalFlags.bProcessingShift = false;

        if (bShifted || IsVehicle()) {
            CWorld::IncrementCurrentScanCode();
            bool bShifted2 = false;
            CWorld::IterateSectorsOverlappedByRect(
                boundingBox,
                [this, &bShifted2, &oldEntityMatrix](int32 sectorX, int32 sectorY) {
                    if (ProcessCollisionSectorList(sectorX, sectorY)) {
                        if (!CWorld::bSecondShift) {
                            *(CMatrix*)m_matrix = oldEntityMatrix;
                            return false;
                        }
                        bShifted2 = true;
                    }
                    return true;
                }
            );
            if (bShifted2) {
                *(CMatrix*)m_matrix = oldEntityMatrix;
                return;
            }
        }
        m_bIsStuck = false;
        m_bIsInSafePosition = true;
        m_fMovingSpeed = (GetPosition() - oldEntityMatrix.GetPosition()).Magnitude();
        RemoveAndAdd();
    }
}

// 0x54DEC0
bool CPhysical::TestCollision(bool bApplySpeed) {
    CMatrix entityMatrix(*m_matrix);
    physicalFlags.b17 = true;
    physicalFlags.bSkipLineCol = true;
    bool bOldUsesCollision = m_bUsesCollision;
    m_bUsesCollision = false;
    bool bTestForBlockedPositions = false;

    CPed* ped = AsPed();
    if (IsPed() && ped->bTestForBlockedPositions) {
        bTestForBlockedPositions = true;
        ped->bTestForBlockedPositions = false;
    }

    if (bApplySpeed) {
        ApplySpeed();
    }

    bool bCheckCollision = CheckCollision();
    m_bUsesCollision = bOldUsesCollision;
    physicalFlags.b17 = false;
    physicalFlags.bSkipLineCol = false;
    *(CMatrix*)m_matrix = entityMatrix;
    if (bTestForBlockedPositions) {
        ped->bTestForBlockedPositions = true;
    }

    return bCheckCollision;
}

// 0x546D00
int32 CPhysical::ProcessEntityCollision(CEntity* entity, CColPoint* colPoint) {
    CColModel* colModel = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel();
    int32 numSpheres = CCollision::ProcessColModels(
        *m_matrix, *colModel,
        entity->GetMatrix(), *entity->GetColModel(),
        colPoint,
        nullptr, nullptr, false);
    if (numSpheres > 0) {
        AddCollisionRecord(entity);
        if (!entity->IsBuilding()) {
            entity->AsPhysical()->AddCollisionRecord(this);
        }

        if (entity->IsBuilding() || entity->IsStatic()) {
            m_bHasHitWall = true;
        }
    }
    return numSpheres;
}

// 0x542560
void CPhysical::RemoveAndAdd() {
// TODO: Refactor `CEntryInfoNode` to be templated, otherwise this function will be a mess
#if 0
    if (m_bIsBIGBuilding) {
        CEntity::Remove();
        CEntity::Add();
        return;
    }

    CEntryInfoNode* entryInfoNode = m_pCollisionList.m_node;
    CRect boundRect = GetBoundRect();
    int32 startSectorX = CWorld::GetSectorX(boundRect.left);
    int32 startSectorY = CWorld::GetSectorY(boundRect.bottom);
    int32 endSectorX = CWorld::GetSectorX(boundRect.right);
    int32 endSectorY = CWorld::GetSectorY(boundRect.top);
    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            CRepeatSector* const rs = GetRepeatSector(sectorX, sectorY);
            const auto ProcessSectorList = [&]<typename PtrListType>(PtrListType& list) {
                if (entryInfoNode) {
                    auto* doubleLink = reinterpret_cast<typename PtrListType::NodeType*>(entryInfoNode->m_doubleLink);

                    entryInfoNode->m_doubleLinkList->UnlinkNode(doubleLink);
                    list.AddNode(doubleLink);
   
                    entryInfoNode->m_repeatSector = rs;
                    entryInfoNode->m_doubleLinkList = &list;
                    entryInfoNode = entryInfoNode->m_next;
                } else {
                    auto newEntityInfoNode = new CEntryInfoNode();
                    if (newEntityInfoNode) {
                        newEntityInfoNode->m_doubleLink = list.AddItem(this);
                        newEntityInfoNode->m_repeatSector = rs;
                        newEntityInfoNode->m_doubleLinkList = &list;
                    }
                    newEntityInfoNode->AddToList(m_pCollisionList.m_node);
                    m_pCollisionList.m_node = newEntityInfoNode;
                }
            };
            switch (m_nType) {
            case ENTITY_TYPE_VEHICLE: ProcessSectorList(rs->Vehicles); break;
            case ENTITY_TYPE_PED:     ProcessSectorList(rs->Peds);     break;
            case ENTITY_TYPE_OBJECT:  ProcessSectorList(rs->Objects);  break;
            }
        }
    }

    while (entryInfoNode) {
        CEntryInfoNode* nextEntryInfoNode = entryInfoNode->m_next;

        entryInfoNode->m_doubleLinkList->DeleteNode(entryInfoNode->m_doubleLink);
        m_pCollisionList.DeleteNode(entryInfoNode);

        entryInfoNode = nextEntryInfoNode;
    }
#endif
}

// 0x542800
void CPhysical::AddToMovingList() {
    if (!m_pMovingList && !m_bIsStaticWaitingForCollision) {
        m_pMovingList = CWorld::ms_listMovingEntityPtrs.AddItem(this);
    }
}

// 0x542860
void CPhysical::RemoveFromMovingList() {
    if (m_pMovingList) {
        CWorld::ms_listMovingEntityPtrs.DeleteNode(m_pMovingList);
        m_pMovingList = nullptr;
    }
}

// 0x5428C0
void CPhysical::SetDamagedPieceRecord(float fDamageIntensity, CEntity* entity, const CColPoint& colPoint, float fDistanceMult) {
    auto* object = AsObject();
    if (fDamageIntensity > m_fDamageIntensity) {
        m_fDamageIntensity = fDamageIntensity;
        m_nPieceType = colPoint.m_nPieceTypeA;

        // CEntity::ChangeEntityReference?
        if (m_pDamageEntity) {
            m_pDamageEntity->CleanUpOldReference(&m_pDamageEntity);
        }
        m_pDamageEntity = entity;
        entity->RegisterReference(&m_pDamageEntity);

        m_vecLastCollisionPosn = colPoint.m_vecPoint;
        m_vecLastCollisionImpactVelocity = fDistanceMult * colPoint.m_vecNormal;
        if (IsObject() && colPoint.m_nSurfaceTypeB == SURFACE_CAR_MOVINGCOMPONENT) {
            object->objectFlags.bDamaged = 1;
        } else if (entity->IsObject() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT) {
            entity->AsObject()->objectFlags.bDamaged = 1;
        }
    }

    if (physicalFlags.bDisableZ) {
        if (entity->m_nModelIndex == ModelIndices::MI_POOL_CUE_BALL && IsObject()) {
            object->m_nLastWeaponDamage = object->m_nLastWeaponDamage != 255 ? WEAPON_RUNOVERBYCAR : WEAPON_DROWNING;
        }
    }
}

// 0x4ABBA0
void CPhysical::ApplyMoveForce(float x, float y, float z) {
    return ApplyMoveForce(CVector(x, y, z));
}

// 0x5429F0
void CPhysical::ApplyMoveForce(CVector force) {
    if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce) {
        if (physicalFlags.bDisableZ) {
            force.z = 0.0f;
        }
        m_vecMoveSpeed += force / m_fMass;
    }
}

// 0x65DA70
void CPhysical::ApplyTurnForce(float forceX, float forceY, float forceZ, float pointX, float pointY, float pointZ) {
    ApplyTurnForce(CVector(forceX, forceY, forceZ), CVector(pointX, pointY, pointZ));
}

// 0x542A50
void CPhysical::ApplyTurnForce(CVector force, CVector point) {
    if (!physicalFlags.bDisableTurnForce) {
        if (physicalFlags.bDisableMoveForce) {
            point.z = 0.0f;
            force.z = 0.0f;
        }

        if (!physicalFlags.bInfiniteMass) {
            point -= GetMatrix().TransformVector(m_vecCentreOfMass);
        }

        m_vecTurnSpeed += point.Cross(force) / m_fTurnMass;
    }
}

// 0x542B50
void CPhysical::ApplyForce(CVector force, CVector point, bool bUpdateTurnSpeed) {
    CVector vecMoveSpeedForce = force;
    if (physicalFlags.bDisableZ) {
        vecMoveSpeedForce.z = 0.0f;
    }

    if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce) {
        m_vecMoveSpeed += vecMoveSpeedForce / m_fMass;
    }

    if (!physicalFlags.bDisableTurnForce && bUpdateTurnSpeed) {
        const auto worldCOM = physicalFlags.bInfiniteMass
              ? CVector{}
              : GetMatrix().TransformVector(m_vecCentreOfMass);

        const float fTurnMass = physicalFlags.bInfiniteMass
            ? m_fTurnMass + m_vecCentreOfMass.z * m_fMass * m_vecCentreOfMass.z * 0.5f
            : m_fTurnMass;

        if (physicalFlags.bDisableMoveForce) {
            point.z = 0.0f;
            force.z = 0.0f;
        }

        m_vecTurnSpeed += (point - worldCOM).Cross(force) / fTurnMass;
    }
}

// 0x542CE0
CVector CPhysical::GetSpeed(CVector point) {
    CVector worldCOM = physicalFlags.bInfiniteMass
        ? CVector{}
        : GetMatrix().TransformVector(m_vecCentreOfMass);

    CVector distance = point - worldCOM;
    CVector vecTurnSpeed = m_vecTurnSpeed + m_vecTurnFriction;
    return vecTurnSpeed.Cross(distance) + m_vecMoveSpeed + m_vecMoveFriction;
}

// 0x542DD0
void CPhysical::ApplyMoveSpeed() {
    if (physicalFlags.bDontApplySpeed || physicalFlags.bDisableMoveForce) {
        ResetMoveSpeed();
    } else {
        GetPosition() += CTimer::GetTimeStep() * m_vecMoveSpeed;
    }
}

// 0x542E20
void CPhysical::ApplyTurnSpeed() {
    if (physicalFlags.bDontApplySpeed) {
        ResetTurnSpeed();
    } else {
        const auto turn = CTimer::GetTimeStep() * m_vecTurnSpeed;
        GetRight() += turn.Cross(GetRight());
        GetForward() += turn.Cross(GetForward());
        GetUp() += turn.Cross(GetUp());
        if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce) {
            GetPosition() += turn.Cross(GetMatrix().TransformVector(-m_vecCentreOfMass));
        }
    }
}

// 0x542FE0
void CPhysical::ApplyGravity() {
    if (physicalFlags.bApplyGravity && !physicalFlags.bDisableMoveForce) {
        if (physicalFlags.bInfiniteMass) {
            float fMassTimeStep = CTimer::GetTimeStep() * m_fMass;
            CVector point = GetMatrix().TransformVector(m_vecCentreOfMass);
            CVector force(0.0f, 0.0f, fMassTimeStep * HEDATIVE_GAME_GRAVITY);
            ApplyForce(force, point, true);
        } else if (m_bUsesCollision) {
            m_vecMoveSpeed.z -= CTimer::GetTimeStep() * GAME_GRAVITY;
        }
    }
}

// 0x5430A0
void CPhysical::ApplyFrictionMoveForce(CVector force) {
    if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce) {
        if (physicalFlags.bDisableZ) {
            force.z = 0.0f;
        }
        m_vecMoveFriction += force / m_fMass;
    }
}

// Unused
// 0x543100
void CPhysical::ApplyFrictionTurnForce(CVector force, CVector offset) {
    if (!physicalFlags.bDisableTurnForce) {
        CVector worldCOM{};
        float mass = m_fTurnMass;
        if (physicalFlags.bInfiniteMass) {
            mass = m_vecCentreOfMass.z * m_fMass * m_vecCentreOfMass.z * 0.5f + m_fTurnMass;
        } else {
            worldCOM = GetMatrix().TransformVector(m_vecCentreOfMass);
        }
        if (physicalFlags.bDisableMoveForce) {
            offset.z = 0.0f;
            force.z  = 0.0f;
        }
        m_vecTurnFriction += (1.0f / mass) * (offset - worldCOM).Cross(force);
    }
}

// 0x543220
void CPhysical::ApplyFrictionForce(CVector force, CVector point) {
    CVector vecTheMoveForce = force;

    if (physicalFlags.bDisableZ) {
        vecTheMoveForce.z = 0.0f;
    }

    if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce) {
        m_vecMoveFriction += vecTheMoveForce / m_fMass;
    }

    if (!physicalFlags.bDisableTurnForce) {
        const auto worldCOM = physicalFlags.bInfiniteMass
              ? CVector{}
              : GetMatrix().TransformVector(m_vecCentreOfMass);

        const float fTurnMass = physicalFlags.bInfiniteMass
            ? m_fTurnMass + m_vecCentreOfMass.z * m_fMass * m_vecCentreOfMass.z * 0.5f
            : m_fTurnMass;

        if (physicalFlags.bDisableMoveForce) {
            point.z = 0.0f;
            force.z = 0.0f;
        }

        m_vecTurnFriction += (point - worldCOM).Cross(force) / fTurnMass;
    }
}

// 0x5433B0
void CPhysical::SkipPhysics() {
    if (!IsPed() && !IsVehicle()) {
        physicalFlags.bSubmergedInWater = false;
    }

    m_bHasHitWall = false;
    m_bWasPostponed = false;
    m_bIsInSafePosition = false;
    m_bHasContacted = false;

    if (m_nStatus != STATUS_SIMPLE) {
        physicalFlags.bOnSolidSurface = false;
        m_nNumEntitiesCollided = 0;
        m_nPieceType = 0;
        m_fDamageIntensity = 0.0f;
        CEntity::ClearReference(m_pDamageEntity);
        ResetFrictionTurnSpeed();
        ResetFrictionMoveSpeed();
    }
}

// 0x543490
void CPhysical::AddCollisionRecord(CEntity* collidedEntity) {
    physicalFlags.bOnSolidSurface = true;
    m_nLastCollisionTime = CTimer::GetTimeInMS();
    if (IsVehicle()) {
        auto* vehicle = AsVehicle();
        if (collidedEntity->IsVehicle()) {
            auto* collidedVehicle = collidedEntity->AsVehicle();
            if (vehicle->m_nAlarmState == -1) {
                vehicle->m_nAlarmState = 15'000;
            }
            if (collidedVehicle->m_nAlarmState == -1) {
                collidedVehicle->m_nAlarmState = 15'000;
            }
        }
    }

    if (physicalFlags.bCanBeCollidedWith) {
        for (auto i = 0; i < m_nNumEntitiesCollided; i++) {
            if (m_apCollidedEntities[i] == collidedEntity) {
                return;
            }
        }

        if (m_nNumEntitiesCollided < std::size(m_apCollidedEntities)) {
            m_apCollidedEntities[m_nNumEntitiesCollided] = collidedEntity;
            m_nNumEntitiesCollided++;
        }
    }
}

// 0x543540
bool CPhysical::GetHasCollidedWith(CEntity* entity) {
    if (!physicalFlags.bCanBeCollidedWith || m_nNumEntitiesCollided <= 0) {
        return false;
    }

    return std::ranges::any_of(GetCollidingEntities(), [entity](const CEntity* ent) {
        return ent == entity;
    });
}

// 0x543580
bool CPhysical::GetHasCollidedWithAnyObject() {
    if (!physicalFlags.bCanBeCollidedWith || m_nNumEntitiesCollided <= 0) {
        return false;
    }

    return std::ranges::any_of(GetCollidingEntities(), [](const CEntity* ent) {
        return ent && ent->IsObject();
    });
}

// 0x5435C0
bool CPhysical::ApplyCollision(CEntity* entity, const CColPoint& colPoint, float& damageIntensity) {
    CVector vecNormal = colPoint.m_vecNormal;
    if (physicalFlags.bDisableTurnForce) {
        float speed = m_vecMoveSpeed.Dot(vecNormal);
        if (speed < 0.0f) {
            damageIntensity = -(speed * m_fMass);
            ApplyMoveForce(damageIntensity * vecNormal);
            ReportCollision(this, entity, true, colPoint, damageIntensity / m_fMass);
            return true;
        }
    } else {
        CVector vecDistanceToPoint = colPoint.m_vecPoint - GetPosition();
        float speed = vecNormal.Dot(GetSpeed(vecDistanceToPoint));
        if (speed < 0.0f) {
            CVector vecDifference = GetMatrix().TransformVector(m_vecCentreOfMass) - vecDistanceToPoint;
            float fCollisionMass = GetTurnTorque(vecDifference, vecNormal);
            
            damageIntensity = -((m_fElasticity + 1.0f) * fCollisionMass * speed);

            CVector vecMoveSpeed = vecNormal * damageIntensity;
            if (IsVehicle() && vecNormal.z < 0.7f) {
                vecMoveSpeed.z *= 0.3f;
            }

            if (!physicalFlags.bDisableCollisionForce) {
                bool bUpdateTurnSpeed = !IsVehicle() || !CWorld::bNoMoreCollisionTorque;
                ApplyForce(vecMoveSpeed, vecDistanceToPoint, bUpdateTurnSpeed);
            }

            ReportCollision(this, entity, true, colPoint, damageIntensity / fCollisionMass);
            return true;
        }
    }
    return false;
}

// 0x543890
bool CPhysical::ApplySoftCollision(CEntity* entity, const CColPoint& colPoint, float& outDamageIntensity) {
    if (physicalFlags.bDisableTurnForce) {
        ApplyCollision(entity, colPoint, outDamageIntensity);
    }

    CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
    CVector vecMoveDirection = colPoint.m_vecNormal;
    float fSoftColSpeedMult = SOFTCOL_SPEED_MULT;

    CVehicle* vehicle = AsVehicle();
    if (IsVehicle() && vehicle->IsSubMonsterTruck()) {
        float fForwardsDotProduct = vecMoveDirection.Dot(GetUp());
        if (fForwardsDotProduct < -0.9f) {
            return false;
        }

        if (fForwardsDotProduct < 0.0f) {
            vecMoveDirection -= fForwardsDotProduct * GetUp();
            vecMoveDirection.Normalise();
        } else if (fForwardsDotProduct > 0.5f) {
            fSoftColSpeedMult = SOFTCOL_SPEED_MULT2;
        }
    }

    const float fSpeedDotProduct = GetSpeed(vecDistanceToPointFromThis).Dot(vecMoveDirection);
    const CVector worldCOM = physicalFlags.bInfiniteMass
        ? CVector{}
        : GetMatrix().TransformVector(m_vecCentreOfMass);

    const CVector vecDifference = vecDistanceToPointFromThis - worldCOM;
    const float fCollisionMass = GetTurnTorque(vecDifference, vecMoveDirection);

    if (!IsVehicle() || vehicle->m_nVehicleSubType
        || notsa::contains(eCarPiece_WheelPieces, (eCarPiece)colPoint.m_nPieceTypeA)) {
        // 0x543C26
        const auto fDepth = SOFTCOL_DEPTH_MIN >= colPoint.m_fDepth ? colPoint.m_fDepth : SOFTCOL_DEPTH_MIN;

        outDamageIntensity = fDepth * CTimer::GetTimeStep() * SOFTCOL_DEPTH_MULT * fCollisionMass * GAME_GRAVITY;
        if (fSpeedDotProduct >= 0.0f) {
            outDamageIntensity = 0.0f;
            return false;
        }

        outDamageIntensity -= fCollisionMass * fSpeedDotProduct * fSoftColSpeedMult;
    } else {
        // 0x543AFF
        outDamageIntensity = colPoint.m_fDepth * CTimer::GetTimeStep() * SOFTCOL_DEPTH_MULT * fCollisionMass * GAME_GRAVITY;
        if (fSpeedDotProduct < 0.0f) {
            outDamageIntensity -= SOFTCOL_CARLINE_SPEED_MULT * fCollisionMass * fSpeedDotProduct;
        }

        vecMoveDirection -= 0.9f * vecMoveDirection.Dot(GetRight()) * GetRight();
    }

    if (outDamageIntensity == 0.0f) {
        return false;
    }

    ApplyForce(vecMoveDirection * outDamageIntensity, vecDistanceToPointFromThis, true);
    if (outDamageIntensity < 0.0f) {
        outDamageIntensity *= -1.0f;
    }
    return true;
}

// 0x543C90
bool CPhysical::ApplySpringCollision(float fSuspensionForceLevel, CVector& direction, CVector& collisionPoint, float fSpringLength, float fSuspensionBias, float& fSpringForceDampingLimit) {
    float compression = 1.0f - fSpringLength;
    if (compression > 0.0f) {
        float step = std::min(CTimer::GetTimeStep(), 3.0f);
        fSpringForceDampingLimit = compression * m_fMass * fSuspensionForceLevel * TWO_GAME_GRAVITY * step * fSuspensionBias;
        ApplyForce(-fSpringForceDampingLimit * direction, collisionPoint, true);   
    }
    return true;
}

// 0x543D60
bool CPhysical::ApplySpringCollisionAlt(float fSuspensionForceLevel, CVector& direction, CVector& collisionPoint, float fSpringLength, float fSuspensionBias, CVector& normal, float& fSpringForceDampingLimit) {
    float compression = 1.0f - fSpringLength;
    if (compression > 0.0f) {
        if (direction.Dot(normal) > 0.0f) {
            normal *= -1.0f;
        }

        float step = std::min(CTimer::GetTimeStep(), 3.0f);
        fSpringForceDampingLimit = compression * (step * m_fMass) * fSuspensionForceLevel * fSuspensionBias * TWO_GAME_GRAVITY;

        if (physicalFlags.bMakeMassTwiceAsBig) {
            fSpringForceDampingLimit *= 0.75f;
        }

        ApplyForce(fSpringForceDampingLimit * normal, collisionPoint, true);
    }
    return true;
}

// 0x543E90
bool CPhysical::ApplySpringDampening(float fDampingForce, float fSpringForceDampingLimit, CVector& direction, CVector& collisionPoint, CVector& collisionPos) {
    const float speedA = collisionPos.Dot(direction);
    const float speedB = GetSpeed(collisionPoint).Dot(direction);

    if (notsa::IsFixBugs()) {
        if (speedB == 0.0f) {
            return true;
        }
    }

    const float step = std::min(CTimer::GetTimeStep(), 3.0f);

    float fDampingForceTimeStep = step * fDampingForce;
    if (physicalFlags.bMakeMassTwiceAsBig) {
        fDampingForceTimeStep *= 2.0f;
    }

    fDampingForceTimeStep = std::clamp(fDampingForceTimeStep, -DAMPING_LIMIT_IN_FRAME, DAMPING_LIMIT_IN_FRAME);
    float fDampingSpeed = -(fDampingForceTimeStep * speedA);
    if (fDampingSpeed > 0.0f && fDampingSpeed + speedB > 0.0f) {
        if (speedB >= 0.0f) {
            fDampingSpeed = 0.0f;
        } else {
            fDampingSpeed = -speedB;
        }
    } else if (fDampingSpeed < 0.0f && fDampingSpeed + speedB < 0.0f) {
        if (speedB <= 0.0f) {
            fDampingSpeed = 0.0f;
        } else {
            fDampingSpeed = -speedB;
        }
    }

    CVector center = GetMatrix().TransformVector(m_vecCentreOfMass);
    CVector distance = collisionPoint - center;
    float fSpringForceDamping = GetTurnTorque(distance, direction) * fDampingSpeed;
    fSpringForceDampingLimit = fabs(fSpringForceDampingLimit) * DAMPING_LIMIT_OF_SPRING_FORCE;
    if (fSpringForceDamping > fSpringForceDampingLimit) {
        fSpringForceDamping = fSpringForceDampingLimit;
    }
    ApplyForce(fSpringForceDamping * direction, collisionPoint, true);
    return true;
}

// Unused
// 0x544100
bool CPhysical::ApplySpringDampeningOld(float fDampeningScalar, float fSpringForce, CVector& vecDirection, CVector& vecOffset, CVector& vecSpeed) {
    const auto speedA = vecSpeed.Dot(vecDirection);
    const auto speedB = GetSpeed(vecOffset).Dot(vecDirection);
    if (notsa::IsFixBugs()) {
        if (speedB == 0.0f) {
            return true;
        }
    }
    const float step = std::min(CTimer::GetTimeStep(), 3.0f);
    float impulse = -fDampeningScalar * (speedA + speedB) / 2.0f * m_fMass * step * 0.53f;
    if (physicalFlags.bMakeMassTwiceAsBig) {
        impulse *= 2.f;
    }

    auto a = m_fTurnMass / ((vecOffset.Magnitude() + 1.0f) * m_fMass * 2.f);
    a = std::min(a, 1.0f);
    float b = abs(impulse / (speedB * m_fMass));
    if (a < b) {
        impulse *= a / b;
    }

    ApplyForce(impulse * vecDirection, vecOffset, true);
    return true;
}

// 0x544280
void CPhysical::RemoveRefsToEntity(CEntity* entity) {
    uint8 collidedEntityIndex = 0;
    if (m_nNumEntitiesCollided) {
        while (collidedEntityIndex < m_nNumEntitiesCollided) {
            if (m_apCollidedEntities[collidedEntityIndex] == entity) {
                for (uint16 i = collidedEntityIndex; i < m_nNumEntitiesCollided - 1; ++i) {
                    m_apCollidedEntities[i] = m_apCollidedEntities[i + 1];
                }
                m_nNumEntitiesCollided--;
            } else {
                collidedEntityIndex++;
            }
        }
    }
}

// 0x5442F0
void CPhysical::DettachEntityFromEntity(float height, float direction, float strength, bool bMakeSpin) {
    CMatrix vecDetachOffsetMatrix{};
    if (IsVehicle() && m_pAttachedTo && m_pAttachedTo->IsVehicle()) {
        CColModel* colModel = CEntity::GetColModel();
        CCollisionData* colData = colModel->m_pColData;
        if (colData) {
            uint8 nNumLines = colData->m_nNumLines;
            colData->m_nNumLines = 0;
            CColModel* attachedToColModel = m_pAttachedTo->GetColModel();
            CMatrix& attachedToMatrix = m_pAttachedTo->GetMatrix();
            if (CCollision::ProcessColModels(*m_matrix, *colModel, attachedToMatrix, *attachedToColModel, CWorld::m_aTempColPts, nullptr, nullptr, false)) {
                m_pEntityIgnoredCollision = m_pAttachedTo;
            } else if (m_pEntityIgnoredCollision == m_pAttachedTo) {
                m_pEntityIgnoredCollision = nullptr;
            }
            colData->m_nNumLines = nNumLines;
        } else if (m_pEntityIgnoredCollision == m_pAttachedTo) {
            m_pEntityIgnoredCollision = nullptr;
        }
    } else {
        m_pEntityIgnoredCollision = m_pAttachedTo;
    }

    vecDetachOffsetMatrix.ResetOrientation();
    vecDetachOffsetMatrix.RotateZ(direction);
    vecDetachOffsetMatrix.RotateX(height);

    vecDetachOffsetMatrix *= m_pAttachedTo->GetMatrix();
    CVector force = vecDetachOffsetMatrix.GetForward() * strength;
    CWorld::Remove(this);
    CEntity::SetIsStatic(false);
    physicalFlags.bAttachedToEntity = false;
    CWorld::Add(this);
    if (physicalFlags.bDisableCollisionForce) {
        CObjectData* pThisObjectInfo = AsObject()->m_pObjectInfo;
        if (!IsObject() || pThisObjectInfo->m_fMass >= 99998.0f) {
            physicalFlags.bCollidable = true;
            ResetTurnSpeed();
            ResetMoveSpeed();
            bMakeSpin = false;
        } else {
            physicalFlags.bDisableCollisionForce = false;
            m_fMass = pThisObjectInfo->m_fMass;
            m_fTurnMass = pThisObjectInfo->m_fTurnMass;
        }
    }

    if (!physicalFlags.bDisableCollisionForce) {
        if (m_pAttachedTo->IsPhysical()) {
            m_vecMoveSpeed = m_pAttachedTo->m_vecMoveSpeed;
            m_vecMoveSpeed += force;
        }
    }

    if (bMakeSpin) {
        ApplyTurnForce(force, force / 2);
    }

    m_pAttachedTo = nullptr;
    m_qAttachedEntityRotation = {};
    m_vecAttachOffset.Reset();
}

// 0x5446A0
void CPhysical::DettachAutoAttachedEntity() {
    CEntity::SetIsStatic(false);
    physicalFlags.bAttachedToEntity = false;
    m_nFakePhysics = 0;
    if (!physicalFlags.bDisableCollisionForce) {
        if (m_pAttachedTo->IsVehicle()) {
            m_vecMoveSpeed = m_pAttachedTo->m_vecMoveSpeed;
            m_vecTurnSpeed = m_pAttachedTo->m_vecTurnSpeed;
        }
    } else {
        physicalFlags.bCollidable = true;
        ResetTurnSpeed();
        ResetMoveSpeed();
    }
    m_vecAttachOffset.Reset();
    m_pEntityIgnoredCollision = nullptr;
    m_pAttachedTo = nullptr;
    m_qAttachedEntityRotation = {};
    if (IsObject()) {
        m_fElasticity = AsObject()->m_pObjectInfo->m_fElasticity;
    }
}

// 0x5447B0
float CPhysical::GetLightingFromCol(bool bLightForRender) const {
    static auto rgbNormalizationFactor = 1.f / (255 * 3); // 0x863C0C
    const float fAmbientRedBlue = CTimeCycle::GetAmbientRed_BeforeBrightness() + CTimeCycle::GetAmbientBlue_BeforeBrightness();
    float fLighting = (CTimeCycle::GetAmbientGreen_BeforeBrightness() + fAmbientRedBlue) * (1.f / 3) + m_fContactSurfaceBrightness;
    if (!bLightForRender) {
        fLighting *= (CTimeCycle::SumOfCurrentRGB1() * rgbNormalizationFactor * TEST_ADD_AMBIENT_LIGHT_FRAC + 1.0f - TEST_ADD_AMBIENT_LIGHT_FRAC)
            + CTimeCycle::SumOfCurrentRGB2() * rgbNormalizationFactor * TEST_ADD_AMBIENT_LIGHT_FRAC;
    }
    return fLighting;
}

// 0x544850
float CPhysical::GetLightingTotal() {
    bool bInteriorLighting = m_nAreaCode;
    if (IsPed()) {
        CPed* ped = AsPed();
        if (ped->m_pPlayerData && (CGame::currArea || ped->m_pPlayerData->m_bForceInteriorLighting)) {
            bInteriorLighting = true;
        }
    }
    return GetLightingFromCol(bInteriorLighting) + m_fDynamicLighting;
}

// 0x5448B0
bool CPhysical::CanPhysicalBeDamaged(eWeaponType weapon, bool* bDamagedDueToFireOrExplosionOrBullet) {
    if (bDamagedDueToFireOrExplosionOrBullet) {
        *bDamagedDueToFireOrExplosionOrBullet = false;
    }

    // NOTSA: for a clearer principle of logic
    bool canDamage = false;
    bool isSpecialDamage = false;

    switch (weapon) {
    // Melee weapons (not special damage)
    case WEAPON_UNARMED:
    case WEAPON_BRASSKNUCKLE:
    case WEAPON_GOLFCLUB:
    case WEAPON_NIGHTSTICK:
    case WEAPON_KNIFE:
    case WEAPON_BASEBALLBAT:
    case WEAPON_SHOVEL:
    case WEAPON_POOL_CUE:
    case WEAPON_KATANA:
    case WEAPON_CHAINSAW:
    case WEAPON_DILDO1:
    case WEAPON_DILDO2:
    case WEAPON_VIBE1:
    case WEAPON_VIBE2:
    case WEAPON_FLOWERS:
    case WEAPON_CANE:
        canDamage = !physicalFlags.bMeleeProof;
        isSpecialDamage = false;
        break;
    // Explosive weapons (special damage)
    case WEAPON_GRENADE:
    case WEAPON_ROCKET:
    case WEAPON_ROCKET_HS:
    case WEAPON_FREEFALL_BOMB:
    case WEAPON_RLAUNCHER:
    case WEAPON_RLAUNCHER_HS:
    case WEAPON_REMOTE_SATCHEL_CHARGE:
    case WEAPON_DETONATOR:
    case WEAPON_ARMOUR | WEAPON_NIGHTSTICK:
        canDamage = !physicalFlags.bExplosionProof;
        isSpecialDamage = true;
        break;
    // Always damage (not special damage, unconditional)
    case WEAPON_TEARGAS:
    case WEAPON_SPRAYCAN:
    case WEAPON_EXTINGUISHER:
    case WEAPON_CAMERA:
    case WEAPON_NIGHTVISION:
    case WEAPON_INFRARED:
    case WEAPON_PARACHUTE:
    case WEAPON_LAST_WEAPON:
    case WEAPON_ARMOUR:
    case WEAPON_ARMOUR | WEAPON_BASEBALLBAT:
        canDamage = true;
        isSpecialDamage = false;
        break;
    // Molotov (special damage)
    case WEAPON_MOLOTOV:
        canDamage = !physicalFlags.bFireProof;
        isSpecialDamage = true;
        break;
    // Bullet weapons (special damage)
    case WEAPON_PISTOL:
    case WEAPON_PISTOL_SILENCED:
    case WEAPON_DESERT_EAGLE:
    case WEAPON_SHOTGUN:
    case WEAPON_SAWNOFF_SHOTGUN:
    case WEAPON_SPAS12_SHOTGUN:
    case WEAPON_MICRO_UZI:
    case WEAPON_MP5:
    case WEAPON_AK47:
    case WEAPON_M4:
    case WEAPON_TEC9:
    case WEAPON_COUNTRYRIFLE:
    case WEAPON_SNIPERRIFLE:
    case WEAPON_MINIGUN:
    case WEAPON_ARMOUR | WEAPON_KNIFE:
        canDamage = !physicalFlags.bBulletProof;
        isSpecialDamage = true;
        break;
    // Flamethrower (not special damage)
    case WEAPON_FLAMETHROWER:
        canDamage = !physicalFlags.bFireProof;
        isSpecialDamage = false;
        break;
    // Armored collision weapons (not special damage)
    case WEAPON_ARMOUR | WEAPON_BRASSKNUCKLE:
    case WEAPON_ARMOUR | WEAPON_GOLFCLUB:
    case WEAPON_ARMOUR | WEAPON_SHOVEL:
        canDamage = !physicalFlags.bCollisionProof;
        isSpecialDamage = false;
        break;
    // Default case for unknown weapons (special damage)
    default:
        canDamage = true;
        isSpecialDamage = true;
        break;
    }
    if (!canDamage) {
        return false;
    }

    if (bDamagedDueToFireOrExplosionOrBullet) {
        *bDamagedDueToFireOrExplosionOrBullet = isSpecialDamage;
    }

    return true;
}

// 0x544C40
void CPhysical::ApplyAirResistance() {
    if (m_fAirResistance <= 0.1f || IsVehicle()) {
        float fSpeedMagnitude = m_vecMoveSpeed.Magnitude() * m_fAirResistance;
        if (CCullZones::DoExtraAirResistanceForPlayer()) {
            if (IsVehicle()) {
                CVehicle* vehicle = this->AsVehicle();
                if (vehicle->IsSubAutomobile() || vehicle->IsSubBike()) {
                    fSpeedMagnitude = CVehicle::m_fAirResistanceMult * fSpeedMagnitude;
                }
            }
        }

        m_vecMoveSpeed *= pow(1.0f - fSpeedMagnitude, CTimer::GetTimeStep());
        m_vecTurnSpeed *= 0.99f;    
    } else {
        float fAirResistanceTimeStep = pow(m_fAirResistance, CTimer::GetTimeStep());
        m_vecMoveSpeed *= fAirResistanceTimeStep;
        m_vecTurnSpeed *= fAirResistanceTimeStep;
    }
}

// 0x544D50
bool CPhysical::ApplyCollisionAlt(CPhysical* entity, CColPoint& colPoint, float& damageIntensity, CVector& outVecMoveSpeed, CVector& outVecTurnSpeed) {
    constexpr float OBJECT_SPEED_LIMIT      = 1.3f;   // 0x863B70
    constexpr float OBJECT_IMPULSE_MULT     = -0.98f; // 0x863B74
    constexpr float UPSIDEDOWN_SPEED_LIMIT  = 1.4f;   // 0x863B78
    constexpr float UPSIDEDOWN_IMPULSE_MULT = -0.95f; // 0x863B7C
    constexpr float BIKE_SPEED_LIMIT_MULT   = 1.7f;   // 0x863B80
    constexpr float BIKE_IMPULSE_MULT       = -0.95f; // 0x863B84
    constexpr float BOAT_SPEED_LIMIT_MULT   = 1.5f;   // 0x863B88
    constexpr float BOAT_IMPULSE_MULT       = -0.95f; // 0x863B8C

    if (m_pAttachedTo && m_pAttachedTo->IsPhysical() && !m_pAttachedTo->IsPed()) {
        float attachedDamageIntensity = 0.0f;
        m_pAttachedTo->ApplySoftCollision(entity, colPoint, attachedDamageIntensity);
    }

    if (physicalFlags.bDisableTurnForce) {
        const float normalSpeed = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
        if (normalSpeed < 0.0f) {
            damageIntensity = -(normalSpeed * m_fMass);
            ApplyMoveForce(colPoint.m_vecNormal * damageIntensity);
            ReportCollision(this, entity, true, colPoint, damageIntensity / m_fMass);
            return true;
        }
        return false;
    }

    auto* vehicle = AsVehicle();
    CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
    CVector vecSpeedAtPoint = GetSpeed(vecDistanceToPointFromThis);

    if (physicalFlags.bAddMovingCollisionSpeed && IsVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT) {
        vecSpeedAtPoint += vehicle->AddMovingCollisionSpeed(vecDistanceToPointFromThis);
    }

    const CVector vecMoveDirection = colPoint.m_vecNormal;
    const float speed = vecMoveDirection.Dot(vecSpeedAtPoint);
    if (speed >= 0.0f) {
        return false;
    }

    CVector worldCOM = GetMatrix().TransformVector(m_vecCentreOfMass);
    if (physicalFlags.bInfiniteMass) {
        worldCOM.Reset();
    }

    const float collisionMass = GetTurnTorque(vecDistanceToPointFromThis - worldCOM, vecMoveDirection);
    uint16 entityAltCol = ALT_ENITY_COL_DEFAULT;
    float moveSpeedLimit = CTimer::GetTimeStep() * GAME_GRAVITY;

    // 0x545023
    if (IsObject()) {
        moveSpeedLimit *= OBJECT_SPEED_LIMIT;
        entityAltCol = ALT_ENITY_COL_OBJECT;
    } else if (IsVehicle() && !physicalFlags.bSubmergedInWater) {
        float moveSpeedLimitMultiplier = 0.0f;
        if (!vehicle->IsBike() || (m_nStatus != STATUS_ABANDONED && m_nStatus != STATUS_WRECKED)) {
            if (vehicle->IsBoat()) {
                moveSpeedLimitMultiplier = BOAT_SPEED_LIMIT_MULT;
                entityAltCol = ALT_ENITY_COL_BOAT;
            } else if (GetUp().z < -0.3f) {
                moveSpeedLimitMultiplier = UPSIDEDOWN_SPEED_LIMIT;
                entityAltCol = ALT_ENITY_COL_VEHICLE;
            }
        } else {
            moveSpeedLimitMultiplier = BIKE_SPEED_LIMIT_MULT;
            entityAltCol = ALT_ENITY_COL_BIKE_WRECKED;
        }
        moveSpeedLimit *= moveSpeedLimitMultiplier;
    }

    // 0x5450A1
    float collisionImpact2 = 1.0f;
    bool useElasticity = false;

    const bool isLowSpeed = !m_bHasContacted
        && abs(m_vecMoveSpeed.x) < moveSpeedLimit
        && abs(m_vecMoveSpeed.y) < moveSpeedLimit
        && abs(m_vecMoveSpeed.z) < (moveSpeedLimit * 2.f);

    switch (entityAltCol) {
    case ALT_ENITY_COL_OBJECT:
        if (isLowSpeed) {
            collisionImpact2 = 0.0f;
            damageIntensity = OBJECT_IMPULSE_MULT * collisionMass * speed;
        }
        useElasticity = true;
        break;
    case ALT_ENITY_COL_BIKE_WRECKED:
        if (isLowSpeed) {
            collisionImpact2 = 0.0f;
            damageIntensity = BIKE_IMPULSE_MULT * collisionMass * speed;
        } else {
            useElasticity = true;
        }
        break;
    case ALT_ENITY_COL_VEHICLE:
        if (isLowSpeed) {
            collisionImpact2 = 0.0f;
            damageIntensity = UPSIDEDOWN_IMPULSE_MULT * collisionMass * speed;
        } else {
            useElasticity = true;
        }
        break;
    case ALT_ENITY_COL_BOAT:
        if (isLowSpeed) {
            collisionImpact2 = 0.0f;
            damageIntensity = BOAT_IMPULSE_MULT * collisionMass * speed;
        } else {
            useElasticity = true;
        }
        break;
    default:
        useElasticity = true;
        break;
    }

    if (useElasticity) {
        // 0x54514A
        float elasticity = m_fElasticity + m_fElasticity;
        if (!IsVehicle() || !vehicle->IsBoat() || colPoint.m_nSurfaceTypeB != SURFACE_WOOD_SOLID && vecMoveDirection.z >= 0.5f) {
            elasticity = m_fElasticity;
        }
        damageIntensity = -((elasticity + 1.0f) * collisionMass * speed);
    }

    CVector vecImpulse = vecMoveDirection * damageIntensity;

    // 0x5452E4
    if (physicalFlags.bDisableZ || physicalFlags.bInfiniteMass || physicalFlags.bDisableMoveForce) {
        ApplyForce(vecImpulse, vecDistanceToPointFromThis, true);
    } else {
        const CVector vecSpeedChange = vecImpulse / m_fMass;
        if (IsVehicle()) {
            if (!m_bHasHitWall || m_vecMoveSpeed.SquaredMagnitude() <= 0.1f && (entity->IsBuilding() || entity->physicalFlags.bDisableCollisionForce)) {
                outVecMoveSpeed += vecSpeedChange * 1.2f;
            } else {
                outVecMoveSpeed += vecSpeedChange;
            }
            vecImpulse *= 0.8f;
        } else {
            outVecMoveSpeed += vecSpeedChange;
        }

        worldCOM = GetMatrix().TransformVector(m_vecCentreOfMass);
        outVecTurnSpeed += (vecDistanceToPointFromThis - worldCOM).Cross(vecImpulse) / m_fTurnMass;
    }

    ReportCollision(this, entity, true, colPoint, damageIntensity / collisionMass, collisionImpact2);
    return true;
}

// 0x5454C0
bool CPhysical::ApplyFriction(float adhesiveLimit, CColPoint& colPoint) {
    if (!physicalFlags.bDisableCollisionForce) {
        if (physicalFlags.bDisableTurnForce) {
            // 0x5454F4
            float normalSpeed = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
            CVector vOtherSpeed = m_vecMoveSpeed - (normalSpeed * colPoint.m_vecNormal);
            float fOtherSpeed = vOtherSpeed.Magnitude();
            if (fOtherSpeed > 0.0f) {
                // 0x54554B
                CVector frictionDir;
                if (notsa::IsFixBugs()) { // division by 0
                    frictionDir = vOtherSpeed;
                    frictionDir.Normalise();
                } else {
                    frictionDir = vOtherSpeed * (1.0f / fOtherSpeed);
                }
                float fImpulse = -fOtherSpeed;
                float impulseLimit = adhesiveLimit * CTimer::GetTimeStep() / m_fMass;
                fImpulse = std::max(fImpulse, -impulseLimit);
                CVector vImpulse = frictionDir * fImpulse;
                m_vecMoveFriction += CVector(vImpulse.x, vImpulse.y, 0.f);
                return true;
            }
        } else {
            // 0x5455C4
            CVector pointpos = colPoint.m_vecPoint - GetPosition();
            CVector speed = GetSpeed(pointpos);

            float normalSpeed = speed.Dot(colPoint.m_vecNormal);
            CVector vOtherSpeed = speed - (normalSpeed * colPoint.m_vecNormal);

            float fOtherSpeed = vOtherSpeed.Magnitude();
            if (fOtherSpeed > 0.0f) {
                // 0x545686
                CVector frictionDir;
                if (notsa::IsFixBugs()) { // division by 0
                    frictionDir = vOtherSpeed;
                    frictionDir.Normalise();
                } else {
                    frictionDir = vOtherSpeed * (1.0f / fOtherSpeed);
                }

                CVector worldCOM = GetMatrix().TransformVector(m_vecCentreOfMass);
                CVector vecDifference = pointpos - worldCOM;
                float fImpulse = std::max(-GetTurnTorque(vecDifference, frictionDir), -adhesiveLimit);

                ApplyFrictionForce(frictionDir * fImpulse, pointpos);

                auto* vehicle = AsVehicle();
                if (fOtherSpeed > 0.1f) {
                    if (g_surfaceInfos.GetFrictionEffect(colPoint.m_nSurfaceTypeB)
                        && (g_surfaceInfos.GetFrictionEffect(colPoint.m_nSurfaceTypeA) == FRICTION_EFFECT_SPARKS || IsVehicle())
                        && (!IsVehicle() || !vehicle->IsSubBMX() || !vehicle->m_pDriver
                            || fabs(colPoint.m_vecNormal.Dot(GetRight())) >= 0.8667)) {
                        // 0x545845
                        CVector v = frictionDir * (fOtherSpeed * 0.25f);
                        CVector direction = frictionDir + (colPoint.m_vecNormal / 10);
                        CVector vecSpeedCrossProduct = colPoint.m_vecNormal.Cross(m_vecMoveSpeed); // todo: shadow var
                        vecSpeedCrossProduct.Normalise();

                        for (int32 i = 0; i < 8; i++) {
                            float random = CGeneral::GetRandomNumberInRange(-0.2f, 0.2f);
                            CVector origin = colPoint.m_vecPoint + (vecSpeedCrossProduct * random);
                            float force = fOtherSpeed * 12.5f;
                            g_fx.AddSparks(origin, direction, force, 1, v, SPARK_PARTICLE_SPARK2, 0.1f, 1.0f);
                        }
                    }
                }
                return true;
            }
        }
    }
    return false;
}

// 0x545980
bool CPhysical::ApplyFriction(CPhysical* entity, float adhesiveLimit, CColPoint& colPoint) {
    const auto normalSpeed = [&](const CVector& v) {
        return v.Dot(colPoint.m_vecNormal);
    };

    const auto otherSpeed = [&](const CVector& v, float nSpeed) {
        return v - (nSpeed * colPoint.m_vecNormal);
    };

    const auto getFrictionDir = [&](const CVector& vOtherSpeed) -> CVector {
        if (notsa::IsFixBugs()) {
            auto dir = vOtherSpeed;
            dir.Normalise();
            return dir;
        }
        return vOtherSpeed * (1.0f / vOtherSpeed.Magnitude()); // may still be unsafe if Magnitude == 0
    };

    const auto calcImpulse = [&](const float fSpeedA, const float fSpeedB, const float massA, const float massB, const float limit) {
        const float speedSum = (fSpeedB * massB + fSpeedA * massA) / (massB + massA);
        return std::tuple{
            speedSum,
            std::clamp((speedSum - fSpeedA) * massA, -limit, limit),
            std::clamp((speedSum - fSpeedB) * massB, -limit, limit)
        };
    };

    if (physicalFlags.bDisableTurnForce && entity->physicalFlags.bDisableTurnForce) {
        // 0x545AE8
        const float nSpeedA = normalSpeed(m_vecMoveSpeed);
        const float nSpeedB = normalSpeed(entity->m_vecMoveSpeed);

        const auto vOtherSpeedA = otherSpeed(m_vecMoveSpeed, nSpeedA);
        const auto vOtherSpeedB = otherSpeed(entity->m_vecMoveSpeed, nSpeedB);

        const float fOtherSpeedA = vOtherSpeedA.Magnitude();
        const float fOtherSpeedB = vOtherSpeedB.Magnitude();

        const auto frictionDir = getFrictionDir(vOtherSpeedA);

        // 0x545AC3
        const float speedSum = (entity->m_fMass * fOtherSpeedB + m_fMass * fOtherSpeedA) / (entity->m_fMass + m_fMass);
        if (fOtherSpeedA > speedSum) {
            const float impulseLimit = adhesiveLimit * CTimer::GetTimeStep();

            const float impulseA = std::clamp((speedSum - fOtherSpeedA) * m_fMass, -impulseLimit, 0.0f);
            const float impulseB = notsa::IsFixBugs()
                ? std::clamp((speedSum - fOtherSpeedB) * entity->m_fMass, 0.0f, impulseLimit)
                : impulseA; // original fallback

            ApplyFrictionMoveForce(frictionDir * impulseA);
            entity->ApplyFrictionMoveForce(frictionDir * impulseB);
            return true;
        }
    } else if (physicalFlags.bDisableTurnForce && !entity->IsVehicle()) {
        // 0x545BD0
        const auto pointPosB = colPoint.m_vecPoint - entity->GetPosition();
        const auto speedB = entity->GetSpeed(pointPosB);

        const float nSpeedA = normalSpeed(m_vecMoveSpeed);
        const float nSpeedB = normalSpeed(speedB);

        const auto vOtherSpeedA = otherSpeed(m_vecMoveSpeed, nSpeedA);
        const auto vOtherSpeedB = otherSpeed(speedB, nSpeedB);

        const float fOtherSpeedA = vOtherSpeedA.Magnitude();
        const float fOtherSpeedB = vOtherSpeedB.Magnitude();

        const auto frictionDir = getFrictionDir(vOtherSpeedA);
        const auto worldCOM_B = entity->GetMatrix().TransformVector(entity->m_vecCentreOfMass);
        const float massB = entity->GetTurnTorque(pointPosB - worldCOM_B, frictionDir);

        // 0x545DC2
        const auto [speedSum, impulseA, impulseB] = calcImpulse(fOtherSpeedA, fOtherSpeedB, m_fMass, massB, adhesiveLimit * CTimer::GetTimeStep());
        if (fOtherSpeedA > speedSum) {
            ApplyFrictionMoveForce(frictionDir * impulseA);
            if (!entity->physicalFlags.bDisableCollisionForce) {
                entity->ApplyFrictionForce(frictionDir * impulseB, pointPosB);
            }
            return true;
        }
    } else if (!entity->physicalFlags.bDisableTurnForce) {
        // 0x54621E
        const auto pointPosA = colPoint.m_vecPoint - GetPosition();
        const auto speedA = GetSpeed(pointPosA);

        const auto pointPosB = colPoint.m_vecPoint - entity->GetPosition();
        const auto speedB = entity->GetSpeed(pointPosB);

        const float nSpeedA = normalSpeed(speedA);
        const float nSpeedB = normalSpeed(speedB);


        float TEST_FRICTION_SIDE_LIMIT_MULT    = 0.2f;   // 0x8CD7CC
        float TEST_FRICTION_SIDE_LIMIT_DOTPROD = 0.707f; // 0x8CD7C8
        float TEST_FRICTION_SIDE_LIMIT_SPEED   = 0.05f;  // 0x8CD7C4

        if (TEST_FRICTION_SIDE_LIMIT_MULT * TEST_FRICTION_SIDE_LIMIT_DOTPROD > abs(nSpeedA)) {
            adhesiveLimit *= TEST_FRICTION_SIDE_LIMIT_SPEED;
        }   

        const auto vOtherSpeedA = otherSpeed(speedA, nSpeedA);
        const auto vOtherSpeedB = otherSpeed(speedB, nSpeedB);

        const float fOtherSpeedA = vOtherSpeedA.Magnitude();
        const float fOtherSpeedB = vOtherSpeedB.Magnitude();

        const auto frictionDir = getFrictionDir(vOtherSpeedA);

        const auto worldCOM_A = GetMatrix().TransformVector(m_vecCentreOfMass);
        const auto worldCOM_B = entity->GetMatrix().TransformVector(entity->m_vecCentreOfMass);

        const float massA = GetTurnTorque(pointPosA - worldCOM_A, frictionDir);
        const float massB = entity->GetTurnTorque(pointPosB - worldCOM_B, frictionDir);

        // 0x546529
        const auto [speedSum, impulseA, impulseB] = calcImpulse(fOtherSpeedA, fOtherSpeedB, massA, massB, adhesiveLimit);
        if (fOtherSpeedA > speedSum) {
            if (!physicalFlags.bDisableCollisionForce) {
                ApplyFrictionForce(frictionDir * impulseA, pointPosA);
            }
            if (!entity->physicalFlags.bDisableCollisionForce) {
                entity->ApplyFrictionForce(frictionDir * impulseB, pointPosB);
            }
            return true;
        }
    } else if (!IsVehicle()) {
        // 0x545EF6
        const auto pointPosA = colPoint.m_vecPoint - GetPosition();
        const auto speedA = GetSpeed(pointPosA);

        const float nSpeedA = normalSpeed(speedA);
        const float nSpeedB = normalSpeed(entity->m_vecMoveSpeed);

        const auto vOtherSpeedA = otherSpeed(speedA, nSpeedA);
        const auto vOtherSpeedB = otherSpeed(entity->m_vecMoveSpeed, nSpeedB);

        const float fOtherSpeedA = vOtherSpeedA.Magnitude();
        const float fOtherSpeedB = vOtherSpeedB.Magnitude();

        const auto frictionDir = getFrictionDir(vOtherSpeedA);

        const auto worldCOM_A = GetMatrix().TransformVector(m_vecCentreOfMass);
        const float massA = GetTurnTorque(pointPosA - worldCOM_A, frictionDir);

        // 0x5460F0
        const auto [speedSum, impulseA, impulseB] = calcImpulse(fOtherSpeedA, fOtherSpeedB, massA, entity->m_fMass, adhesiveLimit * CTimer::GetTimeStep());
        if (fOtherSpeedA > speedSum) {
            if (!physicalFlags.bDisableCollisionForce) {
                ApplyFrictionForce(frictionDir * impulseA, pointPosA);
            }
            entity->ApplyFrictionMoveForce(frictionDir * impulseB);
            return true;
        }
    }
    return false;
}

// 0x546670
bool CPhysical::ProcessShiftSectorList(int32 sectorX, int32 sectorY) {
    CBaseModelInfo* mi = CModelInfo::GetModelInfo(m_nModelIndex);
    float           fBoundingSphereRadius = mi->GetColModel()->GetBoundRadius();
    float           fMaxColPointDepth = 0.0f;
    CVector         vecShift{};
    CColPoint       colPoints[32];
    
    CVector vecBoundCentre = GetBoundCentre();

    int32      totalAcceptableColPoints = 0;
    const auto ProcessSectorList = [&]<typename PtrListType>(PtrListType& list) {
        bool bProcessEntityCollision = true;
        for (CEntity* const entity : list) {
            if (!entity->IsBuilding() && (!entity->IsObject() || !entity->AsPhysical()->physicalFlags.bDisableCollisionForce)) {
                if (!IsPed() || !entity->IsObject()
                    || !entity->IsStatic()
                    || entity->AsObject()->objectFlags.bIsExploded) {
                    bProcessEntityCollision = false;
                }
            }
            if (entity != this
                && !entity->IsScanCodeCurrent()
                && entity->m_bUsesCollision && (!m_bHasHitWall || bProcessEntityCollision)) {
                if (entity->GetIsTouching(vecBoundCentre, fBoundingSphereRadius)) {
                    bool bCollisionDisabled = false;
                    bool bCollidedEntityCollisionIgnored = false;
                    bool bCollidedEntityUnableToMove = false;
                    bool bThisOrCollidedEntityStuck = false;
                    if (entity->IsBuilding()) {
                        if (physicalFlags.bDisableCollisionForce
                            && (!IsVehicle() || entity->AsVehicle()->IsSubTrain())) {
                            bCollisionDisabled = true;
                        } else {
                            if (m_pAttachedTo) {
                                if (m_pAttachedTo->IsPhysical() && m_pAttachedTo->physicalFlags.bDisableCollisionForce) {
                                    bCollisionDisabled = true;
                                }
                            } else if (m_pEntityIgnoredCollision == entity) {
                                bCollisionDisabled = true;
                            }

                            else if (!physicalFlags.bDisableZ || physicalFlags.bApplyGravity) {
                                if (physicalFlags.bDontCollideWithFlyers
                                    && m_nStatus // todo: == STATUS_PLAYER_PLAYBACK_FROM_BUFFER
                                    && m_nStatus != STATUS_REMOTE_CONTROLLED
                                    && entity->DoesNotCollideWithFlyers()) {
                                    bCollisionDisabled = true;
                                }
                            } else {
                                bCollisionDisabled = true;
                            }
                        }
                    } else {
                        SpecialEntityPreCollisionStuff(
                            entity->AsPhysical(),
                            true,
                            bCollisionDisabled,
                            bCollidedEntityCollisionIgnored,
                            bCollidedEntityUnableToMove,
                            bThisOrCollidedEntityStuck
                        );
                    }

                    if (IsPed()) {
                        physicalFlags.bSkipLineCol = true;
                    }

                    if (bCollidedEntityCollisionIgnored || bCollisionDisabled) {
                        continue;
                    }

                    entity->SetCurrentScanCode();
                    int32 totalColPointsToProcess = ProcessEntityCollision(entity, colPoints);
                    if (totalColPointsToProcess <= 0) {
                        continue;
                    }

                    for (int32 colpointIndex = 0; colpointIndex < totalColPointsToProcess; colpointIndex++) {
                        CColPoint* colPoint = &colPoints[colpointIndex];
                        if (colPoint->m_fDepth > 0.0f) {
                            uint8 pieceTypeB = colPoint->m_nPieceTypeB;
                            if (pieceTypeB < 13 || pieceTypeB > 16) {
                                totalAcceptableColPoints++;
                                if (IsVehicle() && entity->IsPed() && colPoint->m_vecNormal.z < 0.0f) {
                                    vecShift.x += colPoint->m_vecNormal.x;
                                    vecShift.y += colPoint->m_vecNormal.y;
                                    vecShift.z += colPoint->m_vecNormal.z * 0.0f;
                                    fMaxColPointDepth = std::max(fMaxColPointDepth, colPoint->m_fDepth);
                                } else {
                                    if (!IsPed() || !entity->IsObject() || !entity->AsPhysical()->physicalFlags.bDisableMoveForce || fabs(colPoint->m_vecNormal.z) <= 0.1f) {
                                        vecShift += colPoint->m_vecNormal;
                                        fMaxColPointDepth = std::max(fMaxColPointDepth, colPoint->m_fDepth);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    };

    CSector*       s = GetSector(sectorX, sectorY);
    CRepeatSector* rs = GetRepeatSector(sectorX, sectorY);
    ProcessSectorList(s->m_buildings);
    ProcessSectorList(rs->Vehicles);
    ProcessSectorList(rs->Peds);
    ProcessSectorList(rs->Objects);

    if (totalAcceptableColPoints == 0) {
        nullsub_496(sectorX, sectorY);
        return false;
    }

    float shiftMagnitude = vecShift.Magnitude();
    if (shiftMagnitude > 1.0f) { // normalize the shift boi
        float shiftMultiplier = 1.0f / shiftMagnitude;
        vecShift *= shiftMultiplier;
    }

    CVector& vecEntityPosition = GetPosition();
    if (vecShift.z >= -0.5f) {
        if (!IsPed()) {
            vecEntityPosition += vecShift * fMaxColPointDepth * 1.5f;
        } else {
            float fDepthMultiplied = 1.5f * fMaxColPointDepth;
            if (fDepthMultiplied >= 0.005f) {
                if (fDepthMultiplied > 0.3f) {
                    vecEntityPosition += vecShift * fMaxColPointDepth * 0.3f;
                }
            } else {
                vecEntityPosition += vecShift * fMaxColPointDepth * 0.005f;
            }

            vecEntityPosition += vecShift * fDepthMultiplied;
        }
    } else {
        vecEntityPosition += vecShift * fMaxColPointDepth * 0.75f;
    }

    if (!IsVehicle() || 1.5f <= 0.0f) { // R* nice check
        return true;
    }

    if (vecShift.z < 0.0f) {
        vecShift.z = 0.0f;
    }

    m_vecMoveSpeed += vecShift * GAME_GRAVITY * CTimer::GetTimeStep();
    return true;
}

// 0x546CEC
void CPhysical::nullsub_496(int32 sectorX, int32 sectorY) {
    // NOP
}

// Used in driving school mission
// 0x546DB0
void CPhysical::PlacePhysicalRelativeToOtherPhysical(CPhysical* other, CPhysical* phys, CVector offset) {
    CVector worldPos = other->GetMatrix().TransformPoint(offset);
    float step = CTimer::GetTimeStep() * 0.9f;
    CVector pos = other->m_vecMoveSpeed * step + worldPos;

    CWorld::Remove(phys);
    phys->GetMatrix() = other->GetMatrix();
    phys->SetPosn(pos);
    phys->SetVelocity(other->m_vecMoveSpeed);
    phys->UpdateRW();
    phys->UpdateRwFrame();
    CWorld::Add(phys);
}

// Unused
// 0x546ED0
float CPhysical::ApplyScriptCollision(CVector vecColNormal, float fElasticity, float fAdhesiveLimit, CVector* pVecColPos) {
    CColPoint tempColPt{};
    float fImpulseMagnitude = 0.0f;
    float originalElasticity = m_fElasticity;
    tempColPt.m_vecNormal = vecColNormal;
    if (pVecColPos) {
        tempColPt.m_vecPoint = *pVecColPos;
    } else {
        CSimpleTransform transform = m_matrix ? (CSimpleTransform&)m_matrix->GetPosition() : m_placement;
        float radius = GetColModel()->GetBoundRadius();
        tempColPt.m_vecPoint = transform.m_vPosn - vecColNormal * radius;
    }
    m_fElasticity = fElasticity;
    if (ApplyCollision(this, tempColPt, fImpulseMagnitude)) {
        if (fAdhesiveLimit > 0.0f) {
            ApplyFriction(fAdhesiveLimit, tempColPt);
        }
    }
    m_fElasticity = originalElasticity;
    return fImpulseMagnitude;
}

constexpr float DUMPTRUCK_DETATCH_IMPULSE = 500.0f; // 0x863B90
constexpr float DUMPTRUCK_DETATCH_MAXSPEED = 0.3f;   // 0x863B94
constexpr float FORKLIFT_DETACH_OBJECT_IMPULSE = 0.1f;   // 0x863B98

// 0x546FF0
void CPhysical::PositionAttachedEntity() {
    if (!m_pAttachedTo) {
        return;
    }

    auto* attachedToVehicle = m_pAttachedTo->AsVehicle();
    auto* attachedToAuto = m_pAttachedTo->AsAutomobile();
    auto* attachedToBike = m_pAttachedTo->AsBike();

    CMatrix attachedEntityMatrix;
    CMatrix attachedEntityRotationMatrix;
    CMatrix attachedToEntityMatrix(m_pAttachedTo->GetMatrix());
    if (m_pAttachedTo->IsVehicle() && attachedToVehicle->IsBike()) {
        attachedToBike->CalculateLeanMatrix();
        attachedToEntityMatrix = attachedToBike->m_mLeanMatrix;
    } else if (IsObject() && ModelIndices::IsForklift(m_pAttachedTo->m_nModelIndex)) {
        RwFrame* carPart = attachedToAuto->m_aCarNodes[CAR_MISC_A];
        if (carPart) {
            attachedToEntityMatrix.UpdateMatrix(RwFrameGetLTM(carPart));
        }
    }

    // 0x547126
    if (m_qAttachedEntityRotation.imag.IsZero()) {
        if (physicalFlags.bInfiniteMass) {
            attachedEntityMatrix = *m_matrix;
        } else {
            attachedEntityRotationMatrix.ResetOrientation();
            attachedEntityRotationMatrix.Rotate(m_vecAttachedEntityRotation);
            attachedEntityMatrix = attachedToEntityMatrix * attachedEntityRotationMatrix;
        }
        attachedEntityMatrix.GetPosition() = attachedToEntityMatrix.TransformPoint(m_vecAttachOffset);
    } else {
        RwMatrix rotationMatrix;
        RtQuatConvertToMatrix((RtQuat*)&m_qAttachedEntityRotation, &rotationMatrix);
        attachedEntityMatrix.UpdateMatrix(&rotationMatrix);
        attachedEntityMatrix = attachedToEntityMatrix * attachedEntityMatrix;
        CVector vecTranslation = attachedToEntityMatrix.TransformPoint(m_vecAttachOffset);
        attachedEntityMatrix.SetTranslateOnly(vecTranslation);
    }
    SetMatrix(attachedEntityMatrix);

    // 0x5473E3
    if (IsObject()) {
        if (IsStatic()) {
            SetIsStatic(false);
        }
        physicalFlags.bAttachedToEntity = true;
        m_nFakePhysics = 0;
    }

    bool bUpdateSpeed = false;
    if (!m_pAttachedTo->IsVehicle() || !IsObject() && !IsVehicle()) {
        if (m_nModelIndex == MODEL_SATCHEL && m_pAttachedTo->IsObject() && !m_pAttachedTo->m_bUsesCollision) {
            DettachAutoAttachedEntity();
            m_bUsesCollision = true;
            return;
        }
        bUpdateSpeed = true;
    }

    bool bDettachEntity = false;
    if (!bUpdateSpeed) {
        // 0x54742F
        if (ModelIndices::IsDumper(m_pAttachedTo->m_nModelIndex)) {
            int16 wMiscComponentAngle = attachedToAuto->m_wMiscComponentAngle;
            if (wMiscComponentAngle && wMiscComponentAngle != attachedToAuto->m_wMiscComponentAnglePrev) {
                bDettachEntity = true;
            }
            if (m_fDamageIntensity > 0.0f) {
                if (m_pDamageEntity) {
                    if (m_pDamageEntity->IsBuilding()) {
                        bDettachEntity = true;
                    }
                }
            }
        } else {
            if (!ModelIndices::IsForklift(m_pAttachedTo->m_nModelIndex)) {
                if (IsVehicle()) {
                    CMatrix attachedToEntityMatrix = m_pAttachedTo->GetMatrix();
                    if (fabs(attachedToEntityMatrix.GetRight().z) > sin(DegreesToRadians(45)) || fabs(attachedToEntityMatrix.GetForward().z) > sin(DegreesToRadians(45))) {
                        DettachEntityFromEntity(0.0f, 0.0f, 1.0f, false);
                        return;
                    }
                }
                bUpdateSpeed = true;
            } else if (!attachedToAuto->m_wMiscComponentAngle && attachedToAuto->m_wMiscComponentAnglePrev
                       || m_fDamageIntensity > 0.1f * m_fMass && m_pDamageEntity
                       && m_pDamageEntity->IsBuilding()) {
                bDettachEntity = true;
            }
        }
        if (!bUpdateSpeed && !bDettachEntity) {
            // 0x5475E0
            float fDamagedIntensity = m_pAttachedTo->m_fDamageIntensity;
            CMatrix* matrix = m_pAttachedTo->m_matrix;
            if (fDamagedIntensity <= 0.0f || m_pAttachedTo->GetUp().z >= 0.1f
                && fDamagedIntensity <= 1.0f || m_pAttachedTo->GetForward().z <= sin(DegreesToRadians(60))) {
                float fDotProduct = m_pAttachedTo->m_vecLastCollisionImpactVelocity.Dot(matrix->GetUp());
                if ((fDamagedIntensity <= DUMPTRUCK_DETATCH_IMPULSE || fDotProduct >= 0.7f
                    || DUMPTRUCK_DETATCH_MAXSPEED * DUMPTRUCK_DETATCH_MAXSPEED <= m_pAttachedTo->m_vecMoveSpeed.SquaredMagnitude())) {
                    bUpdateSpeed = true;
                }
            }
        }
    }

    if (!bUpdateSpeed) {
        // 0x5475E6
        auto* attachedTo = m_pAttachedTo;
        DettachAutoAttachedEntity();
        if (!physicalFlags.bDisableCollisionForce) {
            float randomNumber = CGeneral::GetRandomNumberInRange(-1.0f, 1.0f);
            CMatrix& attachedToEntityMatrix = attachedTo->GetMatrix(); // todo: shadow var
            CVector randomRight = attachedToEntityMatrix.GetRight() * randomNumber;
            CVector randomForward = attachedToEntityMatrix.GetForward() * randomNumber;
            CVector force = (randomRight + randomForward) * (m_fMass / 50.0f);
            ApplyMoveForce(force);
            if (attachedToAuto->m_wMiscComponentAngle > attachedToAuto->m_wMiscComponentAnglePrev) {
                ApplyMoveForce(attachedTo->GetMatrix().GetUp() * m_fMass / 50.0f);
            }
        }
        return;
    }

    constexpr float HANGING_ATTACH_APPLY_TURNSPEED_CAP  = 0.5f; // 0x8CD7E0
    constexpr float HANGING_ATTACH_APPLY_TURNSPEED_MULT = 1.0f / 100000; // 0x8CD7DC

    // 0x547744
    if (physicalFlags.bInfiniteMass) {
        CVector vecMoveSpeed = GetPosition() - m_vecAttachedEntityRotation;
        float fSquaredMagnitude = vecMoveSpeed.SquaredMagnitude();
        m_vecAttachedEntityRotation = GetPosition();
        if (fSquaredMagnitude > 4.0f) {
            vecMoveSpeed *= 2.0f / sqrt(fSquaredMagnitude);
        }

        float fTimeStep = std::max(1.0f, CTimer::GetTimeStep());

        vecMoveSpeed *= 1.0f / fTimeStep;
        CVector vecMoveSpeedDifference = vecMoveSpeed - m_vecMoveSpeed;
        m_vecMoveSpeed = vecMoveSpeed;
        CVector force = vecMoveSpeedDifference * m_fMass * -1.0f;
        CVector vecCenterOfMassMultiplied = GetMatrix().TransformVector(m_vecCentreOfMass);
        ApplyForce(force, vecCenterOfMassMultiplied, true);
        if (m_pAttachedTo->IsVehicle() || m_pAttachedTo->IsObject()) {
            if (m_pAttachedTo->m_bUsesCollision && !m_pAttachedTo->physicalFlags.bDisableCollisionForce) {
                CVector vecDistance = (vecCenterOfMassMultiplied + GetPosition()) - m_pAttachedTo->GetPosition();
                m_pAttachedTo->ApplyForce(force * -1.0f, vecDistance, true);
            }

            float fRotationInRadians = fmod(m_pAttachedTo->GetHeading() - GetHeading() + PI, 2 * PI) - PI;
            fRotationInRadians = std::clamp(fRotationInRadians, -HANGING_ATTACH_APPLY_TURNSPEED_CAP, HANGING_ATTACH_APPLY_TURNSPEED_CAP);
            m_vecTurnSpeed.z += fRotationInRadians * HANGING_ATTACH_APPLY_TURNSPEED_MULT * m_fMass;
        }
    } else {
        if (m_pAttachedTo->IsVehicle() || m_pAttachedTo->IsObject()) {
            m_vecMoveSpeed = m_pAttachedTo->GetSpeed(GetPosition() - m_pAttachedTo->GetPosition());
            m_vecTurnSpeed = m_pAttachedTo->m_vecTurnSpeed;
        }
    }
}

// 0x547B80
void CPhysical::ApplySpeed() {
    static float OneR = 0.03f; // 0x863B6C
    static float TwoR = OneR * 2.f; // 0x863B64

    auto* object = AsObject();
    const float oldTimeStep = CTimer::GetTimeStep();

    auto applyMoveAndTurn = [&]() {
        ApplyMoveSpeed();
        ApplyTurnSpeed();
    };

    // 0x547B94
    if (physicalFlags.bDisableZ) {
        if (physicalFlags.bApplyGravity) {
            if (CTimer::GetTimeStep() * m_vecMoveSpeed.z + GetPosition().z < CWorld::SnookerTableMin.z) {
                GetPosition().z = CWorld::SnookerTableMin.z;
                ResetMoveSpeed();
                ResetTurnSpeed();
            }
        } else {
            // 0x547C31
            auto calcHitTime = [&](float pos, float speed, float min, float max) -> float {
                const float newPos = CTimer::GetTimeStep() * speed + pos;
                if (newPos > max && speed > 0.0f) {
                    return (max - pos) / speed;
                }
                if (newPos < min && speed < 0.0f) {
                    return (min - pos) / speed;
                }
                return 1000.0f;
            };

            const CVector2D tableMin = CWorld::SnookerTableMin;
            const CVector2D tableMax = CWorld::SnookerTableMax;

            const float timeX = calcHitTime(GetPosition().x, m_vecMoveSpeed.x, tableMin.x, tableMax.x);
            const float timeY = calcHitTime(GetPosition().y, m_vecMoveSpeed.y, tableMin.y, tableMax.y);

            // Determine which hit happens first (matches original: prefer Y if times equal)
            float hitTime = 1000.0f;
            bool isHitX = false;
            if (timeX < timeY && timeX < 1000.0f) {
                hitTime = timeX;
                isHitX = true;
            } else if (timeY < 1000.0f) {
                hitTime = timeY;
            }

            // Common vars
            const bool isLongerInY = tableMax.y - tableMin.y > tableMax.x - tableMin.x;
            const float centerX = (tableMin.x + tableMax.x) / 2;
            const float centerY = (tableMin.y + tableMax.y) / 2;

            if (hitTime < 1000.0f) { // Invert
                // 0x547D61, 0x547EA8
                auto& speed = isHitX ? m_vecMoveSpeed.x : m_vecMoveSpeed.y;
                const float normalSign = speed <= 0.0f ? 1.0f : -1.0f; // Matches original: direction opposite to speed

                // Apply movement up to hit time
                CTimer::UpdateTimeStep(hitTime);
                applyMoveAndTurn();

                // Determine axis-specific vars
                const float absMoveSpeed = fabs(speed);

                const float posCheck = isHitX ? GetPosition().y : GetPosition().x;
                const float center = isHitX ? centerY : centerX;
                const float pMin = isHitX ? tableMin.y : tableMin.x;
                const float pMax = isHitX ? tableMax.y : tableMax.x;
                const bool hasMiddlePocket = isHitX ? isLongerInY : !isLongerInY;

                const bool isPocket = (posCheck < pMin + TwoR || posCheck > pMax - TwoR) || (hasMiddlePocket && fabs(posCheck - center) < TwoR);

                if (!isPocket) {
                    speed = -speed;
                } else {
                    physicalFlags.bApplyGravity = true;
                    const float delta = oldTimeStep * speed;
                    if (delta > OneR) {
                        speed = OneR / oldTimeStep;
                    } else if (delta < -OneR) {
                        speed = -OneR / oldTimeStep;
                    }
                    // Else: leave speed as is (matches original)
                }

                // Set remaining time and apply friction if needed
                const float remaining = oldTimeStep - hitTime;

                // 0x547FBA
                CTimer::UpdateTimeStep(remaining);
                if (absMoveSpeed > 0.0f) {
                    const float fRadius = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundRadius();
                    CVector normal{};
                    if (isHitX) {
                        normal.x = normalSign;
                    } else {
                        normal.y = normalSign;
                    }
                    const CVector thePosition = normal * fRadius;

                    CColPoint colPoint{};
                    colPoint.m_vecPoint = GetPosition() - thePosition;
                    colPoint.m_vecNormal = normal;

                    ApplyFriction(10.0f * absMoveSpeed, colPoint);

                    if (IsObject()) {
                        AudioEngine.ReportMissionAudioEvent(AE_SCRIPT_POOL_HIT_CUSHION, object);
                        object->m_nLastWeaponDamage = 4 * (object->m_nLastWeaponDamage == 0xFF) + WEAPON_RUNOVERBYCAR;
                    }
                }
            }
        }
        applyMoveAndTurn();
        CTimer::UpdateTimeStep(oldTimeStep);
        return;
    }

    // 0x5480E2
    if (!physicalFlags.bDisableMoveForce || !IsObject() || object->m_fDoorStartAngle <= -1000.0f) {
        applyMoveAndTurn();
        CTimer::UpdateTimeStep(oldTimeStep);
        return;
    }

    const float fDoorStartAngle = object->m_fDoorStartAngle;
    const auto normalizeToDoor = [&](float angle) -> float {
        if (fDoorStartAngle + PI >= angle) {
            if (fDoorStartAngle - PI > angle) {
                angle += 2 * PI;
            }
        } else {
            angle -= 2 * PI;
        }
        return angle;
    };

    float fHeading = normalizeToDoor(GetHeading());

    constexpr float DOOR_LIMIT_ANGLE       = 1.8849558f; // 0x8CD7E8, DegreesToRadians(108.0f)
    constexpr float DOOR_LIMIT_BOUNCE_MULT = -0.2f;      // 0x8CD7E4

    // 0x548171
    float hitTime = -1000.0f;
    const float fHeadingWithStep = oldTimeStep * m_vecTurnSpeed.z + fHeading;
    if (m_vecTurnSpeed.z > 0.0f) {
        const float fTheDoorAngle = fDoorStartAngle + DOOR_LIMIT_ANGLE;
        if (fHeadingWithStep > fTheDoorAngle) {
            hitTime = (fTheDoorAngle - fHeading) / m_vecTurnSpeed.z;
        }
    } else if (m_vecTurnSpeed.z < 0.0f) {
        const float fTheDoorAngle = fDoorStartAngle - DOOR_LIMIT_ANGLE;
        if (fHeadingWithStep < fTheDoorAngle) {
            hitTime = (fTheDoorAngle - fHeading) / m_vecTurnSpeed.z;
        }
    }

    // 0x548205
    if (-oldTimeStep <= hitTime) {
        CTimer::UpdateTimeStep(hitTime);
        ApplyTurnSpeed();
        m_vecTurnSpeed.z *= DOOR_LIMIT_BOUNCE_MULT;
        const float remaining = oldTimeStep - hitTime;
        CTimer::UpdateTimeStep(remaining);
        physicalFlags.b31 = true;
    }

    applyMoveAndTurn();
    CTimer::UpdateTimeStep(oldTimeStep);

    // 0x548268
    if (object->objectFlags.bIsDoorMoving) {
        float fNewHeading = normalizeToDoor(GetHeading());

        fHeading -= fDoorStartAngle;
        fNewHeading -= fDoorStartAngle;
        if (fabs(fHeading) < 0.001f) {
            fHeading = 0.0f;
        }
        if (fabs(fNewHeading) < 0.001f) {
            fNewHeading = 0.0f;
        }
        if (fHeading * fNewHeading < 0.0f) {
            m_vecTurnSpeed.z = 0.0f;
        }
    }
}

// 0x548320
void CPhysical::UnsetIsInSafePosition() {
    m_vecMoveSpeed = -m_vecMoveSpeed;
    m_vecTurnSpeed = -m_vecTurnSpeed;
    ApplySpeed();
    m_vecMoveSpeed = -m_vecMoveSpeed;
    m_vecTurnSpeed = -m_vecTurnSpeed;
    m_bIsInSafePosition = false;
}

// 0x5483D0
void CPhysical::ApplyFriction() {
    if (physicalFlags.bDisableZ) {
        CColPoint cp;

        const CVector& pos = GetPosition();
        float radius = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundRadius();

        const auto zero = 0.f * radius; // Okay
        cp.m_vecPoint.x = pos.x - zero;
        cp.m_vecPoint.y = pos.y - zero;
        cp.m_vecPoint.z = pos.z - radius;
        cp.m_vecNormal.Set(0.0f, 0.0f, 1.0f);

        ApplyFriction(CTimer::GetTimeStep() * 0.001f, cp);
        m_vecTurnSpeed.z = pow(0.98f, CTimer::GetTimeStep()) * m_vecTurnSpeed.z;
    }

    m_vecMoveSpeed += m_vecMoveFriction;
    m_vecTurnSpeed += m_vecTurnFriction;
    ResetFrictionMoveSpeed();
    ResetFrictionTurnSpeed();

    static float TEST_BIKE_FRICTION_SPEED_LIMIT = 0.05f;  // 0x8CD7F4
    static float TEST_BIKE_FRICTION_TURN_LIMIT  = 0.01f;  // 0x8CD7F0
    static float TEST_BIKE_FRICTION_SPEED_MULT  = 0.707f; // 0x863C1C

    auto* vehicle = AsVehicle();
    if (IsVehicle() && vehicle->IsBike()
        && !physicalFlags.b32 && m_nStatus == STATUS_ABANDONED
        && TEST_BIKE_FRICTION_SPEED_MULT > fabs(GetUp().z)
        && sq(TEST_BIKE_FRICTION_SPEED_LIMIT) > m_vecMoveSpeed.SquaredMagnitude()
        && sq(TEST_BIKE_FRICTION_TURN_LIMIT) > m_vecTurnSpeed.SquaredMagnitude()) {
        m_vecMoveSpeed *= pow(0.5f, CTimer::GetTimeStep());
    }
}

// 0x548680
bool CPhysical::ApplyCollision(CEntity* ent, CColPoint& colPoint, float& impulseMagnitudeA, float& impulseMagnitudeB) {
    auto* entity = ent->AsPhysical();
    auto* entityObject = entity->AsObject();
    auto* entityVehicle = entity->AsVehicle();

    auto* thisObject = AsObject();
    auto* thisPed = AsPed();
    auto* thisVehicle = AsVehicle();

    bool bThisPedIsStandingOnEntity = false;
    bool bEntityPedIsStandingOnThis = false;
    bool bEntityCollisionForceDisabled = false;

    float fThisMassFactor = 0.0f;
    float fEntityMassFactor = 1.0f;

    // 0x5486B7
    if (!entity->physicalFlags.bDisableTurnForce || physicalFlags.bDisableMoveForce) {
        fThisMassFactor = physicalFlags.bMakeMassTwiceAsBig ? 2.0f : 1.0f;
    } else {
        fThisMassFactor = 10.0f;
        if (entity->IsPed() && entity->AsPed()->m_standingOnEntity == this) {
            bEntityPedIsStandingOnThis = true;
        }
    }

    // 0x5486F6
    if (physicalFlags.bDisableTurnForce) {
        if (IsPed() && thisPed->IsPlayer() && entity->IsVehicle()
            && (entity->m_nStatus == STATUS_ABANDONED
                || entity->m_nStatus == STATUS_WRECKED || m_bIsStuck)) {
            float fTheEntityMass = std::max(entity->m_fMass - 2000.0f, 0.0f);
            fEntityMassFactor = 1.0f / (fTheEntityMass / 5000.0f + 1.0f);
        } else if (!entity->physicalFlags.bDisableMoveForce) {
            fEntityMassFactor = 10.0f;
        }
        if (IsPed() && thisPed->m_standingOnEntity == entity) {
            bThisPedIsStandingOnEntity = true;
            fEntityMassFactor = 10.0f;
        }
    } else if (IsVehicle() && thisVehicle->m_pVehicleBeingTowed) {
        fEntityMassFactor = (thisVehicle->m_pVehicleBeingTowed->m_fMass + m_fMass) / m_fMass;
    } else {
        fEntityMassFactor = entity->physicalFlags.bMakeMassTwiceAsBig ? 2.0f : 1.0f;
    }

    // 0x5487F5
    if (entity->physicalFlags.bDisableCollisionForce && !entity->physicalFlags.bCollidable
        || entity->m_pAttachedTo && !entity->physicalFlags.bInfiniteMass) {
        bEntityCollisionForceDisabled = true;
        bThisPedIsStandingOnEntity = false;
    }

    const CVector worldCOM_A = physicalFlags.bInfiniteMass
        ? CVector{}
        : GetMatrix().TransformVector(m_vecCentreOfMass);

    const CVector worldCOM_B = entity->physicalFlags.bInfiniteMass
        ? CVector{}
        : entity->GetMatrix().TransformVector(entity->m_vecCentreOfMass);

    // 0x5488EF
    if (IsVehicle() && entity->IsObject() && entityObject->objectFlags.bIsLampPost
        || entity->IsVehicle() && IsObject() && thisObject->objectFlags.bIsLampPost) {
        colPoint.m_vecNormal.z = 0.0f;
    }

    // 0x548917 - Invert
    if (entity->IsStatic() && !bEntityCollisionForceDisabled) {
        // 0x54891D
        if (!physicalFlags.bDisableTurnForce) {
            CVector pointPosA = colPoint.m_vecPoint - GetPosition();
            CVector speedA = GetSpeed(pointPosA);

            if (physicalFlags.bAddMovingCollisionSpeed && IsVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT) {
                speedA += thisVehicle->AddMovingCollisionSpeed(pointPosA);
            }

            float normalSpeed = speedA.Dot(colPoint.m_vecNormal);
            if (normalSpeed < 0.0f) {
                if (!entity->IsObject()) {
                    if (!entity->physicalFlags.bDisableCollisionForce) {
                        entity->SetIsStatic(false);
                        CWorld::Players[CWorld::PlayerInFocus].m_nHavocCaused += 2;
                        CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, static_cast<float>(CGeneral::GetRandomNumberInRange(30, 60)));
                    }
                    return ApplyCollision(entity, colPoint, impulseMagnitudeA);
                } else {
                    auto massA = GetTurnTorque(pointPosA - worldCOM_A, colPoint.m_vecNormal);
                    // 0x548D31
                    if (!m_bHasHitWall) {
                        impulseMagnitudeA = -((m_fElasticity + 1.0f) * massA * normalSpeed);
                    } else {
                        impulseMagnitudeA = massA * normalSpeed * -1.0f;
                    }

                    impulseMagnitudeB = impulseMagnitudeA;

                    CObjectData* objInfo = entityObject->m_pObjectInfo;

                    const float fObjectDamageMultiplier = IsVehicle() && thisVehicle->IsSubBike() ? 3.0f : 1.0f;

                    // 0x548D91
                    if (entityObject->m_nColDamageEffect) {
                        float fObjectDamage = fObjectDamageMultiplier * impulseMagnitudeA;
                        if (fObjectDamage > 20.0f) {
                            entityObject->ObjectDamage(fObjectDamage, &colPoint.m_vecPoint, &colPoint.m_vecNormal, this, WEAPON_UNIDENTIFIED);
                            if (!entity->m_bUsesCollision) {
                                if (!physicalFlags.bDisableCollisionForce) {
                                    float fColDamageMultiplier = objInfo->m_fColDamageMultiplier;
                                    float fCollisionDamage = fColDamageMultiplier + fColDamageMultiplier;
                                    CVector vecMoveForce = (colPoint.m_vecNormal * impulseMagnitudeA) / fCollisionDamage;
                                    ApplyForce(vecMoveForce, pointPosA, true);
                                }

                                float fDamageIntensityMultiplier = objInfo->m_fColDamageMultiplier / massA;
                                ReportCollision(this, entity, true, colPoint, fDamageIntensityMultiplier * impulseMagnitudeA * 2.f);
                                return false;
                            }
                        }
                    }

                    // 0x548EE5
                    if (entity->physicalFlags.bDisableCollisionForce
                        || objInfo->m_fUprootLimit >= 9999.0f
                        || (impulseMagnitudeA <= objInfo->m_fUprootLimit && ((m_nFlags & 0x10) == 0 || (m_nFlags & 0x1000) == 0))) {
                        if (IsGlassModel(entity)) {
                            CGlass::WindowRespondsToSoftCollision(entityObject, impulseMagnitudeA);
                        }
                        return ApplyCollision(entity, colPoint, impulseMagnitudeA);
                    }

                    // 0x548EEC
                    if (IsGlassModel(entity)) {
                        CGlass::WindowRespondsToCollision(entity, impulseMagnitudeA, m_vecMoveSpeed, colPoint.m_vecPoint, false);
                    } else {
                        entity->SetIsStatic(false);
                    }

                    // 0x548F45
                    int32 entityModelIndex = entity->m_nModelIndex;
                    if (entityModelIndex == ModelIndices::MI_FIRE_HYDRANT && !entityObject->objectFlags.bIsExploded) {
                        g_fx.TriggerWaterHydrant(entity->GetPosition());
                        entityObject->objectFlags.bIsExploded = true;
                    } else if ((entityModelIndex == ModelIndices::MI_PARKINGMETER || entityModelIndex == ModelIndices::MI_PARKINGMETER2) && !entityObject->objectFlags.bIsExploded) {
                        CPickups::CreateSomeMoney(entity->GetPosition(), CGeneral::GetRandomNumber() % 100);
                        entityObject->objectFlags.bIsExploded = true;
                    } else if (entity->IsObject() && !objInfo->m_bCausesExplosion) {
                        entityObject->objectFlags.bIsExploded = true;
                    }

                    // 0x54901D
                    if (!physicalFlags.bDisableCollisionForce && objInfo->m_fUprootLimit > 200.0f) {
                        CVector vecMoveForce = (colPoint.m_vecNormal * 0.2f) * impulseMagnitudeA;
                        ApplyForce(vecMoveForce, pointPosA, true);
                    }
                }
            }
        } else {
            // 0x54893B
            float normalSpeed = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
            if (normalSpeed < 0.0f) {
                if (!entity->IsObject()) {
                    if (entity->physicalFlags.bDisableCollisionForce) {
                        return ApplyCollision(entity, colPoint, impulseMagnitudeA);
                    }
                    entity->SetIsStatic(false);
                } else {
                    impulseMagnitudeB = impulseMagnitudeA = -(normalSpeed * m_fMass);

                    if (entity->physicalFlags.bDisableCollisionForce) {
                        return ApplyCollision(entity, colPoint, impulseMagnitudeA);
                    }

                    CObjectData* objInfo = entityObject->m_pObjectInfo;
                    if (objInfo->m_fUprootLimit >= 9999.0f || impulseMagnitudeA <= objInfo->m_fUprootLimit) {
                        return ApplyCollision(entity, colPoint, impulseMagnitudeA);
                    }

                    // 0x5489DA
                    if (IsGlassModel(entity)) {
                        CGlass::WindowRespondsToCollision(entity, impulseMagnitudeA, m_vecMoveSpeed, colPoint.m_vecPoint, false);
                    } else if (!entity->physicalFlags.bDisableCollisionForce) {
                        entity->SetIsStatic(false);
                        CWorld::Players[CWorld::PlayerInFocus].m_nHavocCaused += 2;
                        CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, static_cast<float>(CGeneral::GetRandomNumberInRange(30, 60)));
                    }
                }
            }
        }

        if (entity->IsStatic()) {
            return false;
        }
        if (!entity->physicalFlags.bDisableCollisionForce) {
            entity->AddToMovingList();
        }
    }

    // 0x548AA3
    if (physicalFlags.bDisableTurnForce && entity->physicalFlags.bDisableTurnForce) {
        const float massA = m_fMass;
        const float massB = entity->m_fMass;
        const float normalSpeedA = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
        const float normalSpeedB = entity->m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
        float speed = 0.0f;

        // 0x548B0A
        if (physicalFlags.bDisableCollisionForce || physicalFlags.bDontApplySpeed) {
            speed = normalSpeedA;
        } else if (entity->physicalFlags.bDisableCollisionForce || entity->physicalFlags.bDontApplySpeed) {
            speed = normalSpeedB;
        } else if (!thisPed->bPushOtherPeds) {
            speed = normalSpeedB >= 0.0f ? 0.0f : normalSpeedB;
        } else {
            speed = (massA * normalSpeedA * 4.0f + massB * normalSpeedB) / (massA * 4.0f + massB);
        }

        bThisPedIsStandingOnEntity = true;

        const float diff = normalSpeedA - speed;
        if (diff >= 0.0f) {
            return false;
        }

        // 0x549134
        const float fTheElasticity = (entity->m_fElasticity + m_fElasticity) / 2;
        float fThisMoveSpeedElasticity = m_bHasHitWall
            ? speed
            : speed - fTheElasticity * diff;

        impulseMagnitudeA = (fThisMoveSpeedElasticity - normalSpeedA) * massA;

        CVector vecThisMoveForce = impulseMagnitudeA * colPoint.m_vecNormal;
        if (!physicalFlags.bDisableCollisionForce && !physicalFlags.bDontApplySpeed) {
            ApplyMoveForce(vecThisMoveForce);
            ReportCollision(this, entity, true, colPoint, impulseMagnitudeA / massA);
        }

        if (bThisPedIsStandingOnEntity) {
            const float fEntityMoveSpeedElasticity = entity->m_bHasHitWall
                ? speed
                : speed - (normalSpeedB - speed) * fTheElasticity;
            impulseMagnitudeB = -(fEntityMoveSpeedElasticity - normalSpeedB) * massB;

            CVector vecEntityMoveForce = impulseMagnitudeB * colPoint.m_vecNormal * -1.0f;
            if (!entity->physicalFlags.bDisableCollisionForce && !entity->physicalFlags.bDontApplySpeed) {
                if (entity->m_bIsInSafePosition) {
                    entity->UnsetIsInSafePosition();
                }
                entity->ApplyMoveForce(vecEntityMoveForce);
                ReportCollision(entity, this, false, colPoint, impulseMagnitudeB / massB);
            }
        }
        return true;
    }

    // 0x5492D1
    if (physicalFlags.bDisableTurnForce) {
        CVector pointPosB = colPoint.m_vecPoint - entity->GetPosition();
        CVector speedB = entity->GetSpeed(pointPosB);

        if (entity->physicalFlags.bAddMovingCollisionSpeed && entity->IsVehicle()
            && colPoint.m_nSurfaceTypeB == SURFACE_CAR_MOVINGCOMPONENT) {
            speedB += entityVehicle->AddMovingCollisionSpeed(pointPosB);
        }

        float normalSpeedA = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
        float normalSpeedB = speedB.Dot(colPoint.m_vecNormal);

        float massA = fThisMassFactor * m_fMass;

        CVector cross = (pointPosB - worldCOM_B).Cross(colPoint.m_vecNormal);
        float sqMag = cross.SquaredMagnitude();
        float massB = sqMag / (fEntityMassFactor * entity->m_fTurnMass);
        if (!entity->physicalFlags.bDisableMoveForce) {
            massB += 1.0f / (fEntityMassFactor * entity->m_fMass);
        }
        massB = 1.0f / massB;

        float speed = bEntityCollisionForceDisabled
            ? normalSpeedB
            : (massB * normalSpeedB + massA * normalSpeedA) / (massB + massA);

        float diff = normalSpeedA - speed;
        if (diff >= 0.0f) {
            return false;
        }

        float fTheElasticity = (entity->m_fElasticity + m_fElasticity) / 2;
        float fThisMoveSpeedElasticity = m_bHasHitWall
            ? speed
            : speed - fTheElasticity * diff;
        float fEntityMoveSpeedElasticity = entity->m_bHasHitWall
            ? speed
            : speed - (normalSpeedB - speed) * fTheElasticity;

        impulseMagnitudeA = (fThisMoveSpeedElasticity - normalSpeedA) * massA;
        impulseMagnitudeB = -((fEntityMoveSpeedElasticity - normalSpeedB) * massB);

        CVector vecThisMoveForce = (impulseMagnitudeA / fThisMassFactor) * colPoint.m_vecNormal;
        if (!physicalFlags.bDisableCollisionForce) {
            if (vecThisMoveForce.z < 0.0f) {
                vecThisMoveForce.z = 0.0f;
            }
            if (bThisPedIsStandingOnEntity) {
                vecThisMoveForce.x *= 2.0f;
                vecThisMoveForce.y *= 2.0f;
            }
            ApplyMoveForce(vecThisMoveForce);
        }

        CVector vecEntityMoveForce = (impulseMagnitudeB / fEntityMassFactor) * colPoint.m_vecNormal * -1.0f;
        if (!entity->physicalFlags.bDisableCollisionForce && !bThisPedIsStandingOnEntity) {
            if (entity->m_bIsInSafePosition) {
                entity->UnsetIsInSafePosition();
            }
            entity->ApplyForce(vecEntityMoveForce, pointPosB, true);
        }

        ReportCollision(this, entity, true, colPoint, impulseMagnitudeA / massA);
        ReportCollision(entity, this, false, colPoint, impulseMagnitudeB / massB);
        return true;
    }

    // 0x5496F0
    if (entity->physicalFlags.bDisableTurnForce) {
        CVector pointPosA = colPoint.m_vecPoint - GetPosition();
        CVector speedA = GetSpeed(pointPosA);

        if (physicalFlags.bAddMovingCollisionSpeed && IsVehicle()
            && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT) {
            speedA += thisVehicle->AddMovingCollisionSpeed(pointPosA);
        }

        float normalSpeedA = speedA.Dot(colPoint.m_vecNormal);
        float normalSpeedB = entity->m_vecMoveSpeed.Dot(colPoint.m_vecNormal);

        CVector cross = (pointPosA - worldCOM_A).Cross(colPoint.m_vecNormal);
        float sqMag = cross.SquaredMagnitude();
        float massA = sqMag / (fThisMassFactor * m_fTurnMass);
        if (!physicalFlags.bDisableMoveForce) {
            massA += 1.0f / (fThisMassFactor * m_fMass);
        }
        massA = 1.0f / massA;

        float massB = fEntityMassFactor * entity->m_fMass;

        float speed = (massB * normalSpeedB + massA * normalSpeedA) / (massB + massA);

        float diff = normalSpeedA - speed;
        if (diff >= 0.0f) {
            return false;
        }

        float fTheElasticity = (entity->m_fElasticity + m_fElasticity) / 2;
        float fThisMoveSpeedElasticity = m_bHasHitWall ? speed : speed - fTheElasticity * diff;
        float fEntityMoveSpeedElasticity = entity->m_bHasHitWall ? speed : speed - (normalSpeedB - speed) * fTheElasticity;

        impulseMagnitudeA = (fThisMoveSpeedElasticity - normalSpeedA) * massA;
        impulseMagnitudeB = -((fEntityMoveSpeedElasticity - normalSpeedB) * massB);

        CVector vecThisMoveForce = (impulseMagnitudeA / fThisMassFactor) * colPoint.m_vecNormal;
        if (!physicalFlags.bDisableCollisionForce && !bEntityPedIsStandingOnThis) {
            if (vecThisMoveForce.z < 0.0f) {
                vecThisMoveForce.z = 0.0f;
            }
            ApplyForce(vecThisMoveForce, pointPosA, true);
        }

        CVector vecEntityMoveForce = (impulseMagnitudeB / fEntityMassFactor) * colPoint.m_vecNormal * -1.0f;
        if (!entity->physicalFlags.bDisableCollisionForce) {
            if (vecEntityMoveForce.z < 0.0f) {
                vecEntityMoveForce.z = 0.0f;
                if (fabs(normalSpeedA) < 0.01f) {
                    vecEntityMoveForce.x /= 2;
                    vecEntityMoveForce.y /= 2;
                }
            }
            if (bEntityPedIsStandingOnThis) {
                vecEntityMoveForce.x *= 2;
                vecEntityMoveForce.y *= 2;
            }
            if (entity->m_bIsInSafePosition) {
                entity->UnsetIsInSafePosition();
            }
            entity->ApplyMoveForce(vecEntityMoveForce);
        }

        ReportCollision(this, entity, true, colPoint, impulseMagnitudeA / massA);
        ReportCollision(entity, this, false, colPoint, impulseMagnitudeB / massB);
        return true;
    } else {
        CVector pointPosA = colPoint.m_vecPoint - GetPosition();
        CVector speedA = GetSpeed(pointPosA);

        if (physicalFlags.bAddMovingCollisionSpeed && IsVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT) {
            speedA += thisVehicle->AddMovingCollisionSpeed(pointPosA);
        }

        CVector pointPosB = colPoint.m_vecPoint - entity->GetPosition();
        CVector speedB = entity->GetSpeed(pointPosB);

        if (entity->physicalFlags.bAddMovingCollisionSpeed && entity->IsVehicle() && colPoint.m_nSurfaceTypeB == SURFACE_CAR_MOVINGCOMPONENT) {
            speedB += entityVehicle->AddMovingCollisionSpeed(pointPosB);
        }

        float normalSpeedA = speedA.Dot(colPoint.m_vecNormal);
        float normalSpeedB = speedB.Dot(colPoint.m_vecNormal);

        CVector crossA = (pointPosA - worldCOM_A).Cross(colPoint.m_vecNormal);
        float sqMagA = crossA.SquaredMagnitude();
        float massA = sqMagA / (fThisMassFactor * m_fTurnMass);
        if (!physicalFlags.bDisableMoveForce) {
            massA += 1.0f / (fThisMassFactor * m_fMass);
        }
        massA = 1.0f / massA;

        CVector crossB = (pointPosB - worldCOM_B).Cross(colPoint.m_vecNormal);
        float sqMagB = crossB.SquaredMagnitude();
        float massB = sqMagB / (fEntityMassFactor * entity->m_fTurnMass);
        if (!entity->physicalFlags.bDisableMoveForce) {
            massB += 1.0f / (fEntityMassFactor * entity->m_fMass);
        }
        massB = 1.0f / massB;

        float speed = (massB * normalSpeedB + massA * normalSpeedA) / (massB + massA);

        float diff = normalSpeedA - speed;
        if (diff >= 0.0f) {
            return false;
        }

        float fTheElasticity = (entity->m_fElasticity + m_fElasticity) / 2.0f;
        float fThisMoveSpeedElasticity = m_bHasHitWall ? speed : speed - fTheElasticity * diff;
        float fEntityMoveSpeedElasticity = entity->m_bHasHitWall ? speed : speed - (normalSpeedB - speed) * fTheElasticity;

        impulseMagnitudeA = (fThisMoveSpeedElasticity - normalSpeedA) * massA;
        impulseMagnitudeB = -((fEntityMoveSpeedElasticity - normalSpeedB) * massB);

        CVector vecThisMoveForce = (impulseMagnitudeA / fThisMassFactor) * colPoint.m_vecNormal;
        CVector vecEntityMoveForce = (impulseMagnitudeB / fEntityMassFactor) * colPoint.m_vecNormal * -1.0f;

        if (IsVehicle() && !m_bHasHitWall && !physicalFlags.bDisableCollisionForce) {
            if (colPoint.m_vecNormal.z < 0.7f) {
                vecThisMoveForce.z *= 0.3f;
            }
            if (!m_nStatus) {
                pointPosA *= 0.8f;
            }
            if (CWorld::bNoMoreCollisionTorque) {
                CVector vecFrictionForce = vecThisMoveForce * -0.3f;
                ApplyFrictionForce(vecFrictionForce, pointPosA);
            }
        }

        if (entity->IsVehicle() && !entity->m_bHasHitWall && !entity->physicalFlags.bDisableCollisionForce) {
            if (-colPoint.m_vecNormal.z < 0.7f) {
                vecEntityMoveForce.z *= 0.3f;
            }
            if (!entity->m_nStatus) {
                pointPosB *= 0.8f;
            }
            if (CWorld::bNoMoreCollisionTorque) {
                CVector vecFrictionForce = vecEntityMoveForce * -0.3f;
                entity->ApplyFrictionForce(vecFrictionForce, pointPosB);
            }
        }

        if (CCheat::IsActive(CHEAT_CARS_FLOAT_AWAY_WHEN_HIT)) {
            if (FindPlayerVehicle() == thisVehicle && entity->IsVehicle()
                && entityVehicle->m_nCreatedBy != MISSION_VEHICLE) {
                entity->physicalFlags.bApplyGravity = false;
            }
            if (FindPlayerVehicle() == entityVehicle && IsVehicle()
                && thisVehicle->m_nCreatedBy != MISSION_VEHICLE) {
                physicalFlags.bApplyGravity = false;
            }
        }

        if (!physicalFlags.bDisableCollisionForce) {
            ApplyForce(vecThisMoveForce, pointPosA, true);
        }

        if (!entity->physicalFlags.bDisableCollisionForce) {
            if (entity->m_bIsInSafePosition) {
                entity->UnsetIsInSafePosition();
            }
            entity->ApplyForce(vecEntityMoveForce, pointPosB, true);
        }

        ReportCollision(this, entity, true, colPoint, impulseMagnitudeA / massA);
        ReportCollision(entity, this, false, colPoint, impulseMagnitudeB / massB);
        return true;
    }
    return false;
}

// 0x54A2C0
bool CPhysical::ApplySoftCollision(CPhysical* physical, CColPoint& colPoint, float& ipulseMagnitudeA, float& impulseMagnitudeB) {
    auto* entityObject = physical->AsObject();
    auto* entityPed = physical->AsPed();
    auto* entityVehicle = physical->AsVehicle();

    auto* thisObject = AsObject();
    auto* thisPed = AsPed();
    auto* thisVehicle = AsVehicle();

    bool bEntityPedIsStandingOnThis = false;
    bool bThisPedIsStandingOnEntity = false;
    bool bEntityCollisionForceDisabled = false;

    float massFactorA = 0.0f;
    float massFactorB = 0.0f;

    // 0x54A2EF
    if (!physical->physicalFlags.bDisableTurnForce || physicalFlags.bDisableMoveForce || physicalFlags.bInfiniteMass) {
        massFactorA = physicalFlags.bMakeMassTwiceAsBig ? 2.0f : 1.0f;
    } else {
        massFactorA = 10.0f;
        if (entityPed->m_standingOnEntity == this) { // BUG: Game should be checking if entity is ped or not.
            bEntityPedIsStandingOnThis = true;
        }
    }
    if (!physicalFlags.bDisableTurnForce || physical->physicalFlags.bDisableMoveForce || physical->physicalFlags.bInfiniteMass) {
        massFactorB = physical->physicalFlags.bMakeMassTwiceAsBig ? 2.0f : 1.0f;
    } else {
        if (IsPed() && thisPed->IsPlayer() && physical->IsVehicle()
            && (physical->m_nStatus == STATUS_ABANDONED
                || physical->m_nStatus == STATUS_WRECKED
                || m_bIsStuck)) {
            const float fTheEntityMass = std::max(0.0f, physical->m_fMass - 2000.0f);
            massFactorB = 1.0f / (fTheEntityMass * 0.0002f + 1.0f);
        } else {
            massFactorB = 10.0f;
        }
        if (IsPed() && thisPed->m_standingOnEntity == physical) {
            bThisPedIsStandingOnEntity = true;
        }
    }

    // 0x54A3D6
    if (physical->physicalFlags.bDisableCollisionForce && !physical->physicalFlags.bCollidable) {
        bEntityCollisionForceDisabled = true;
        bThisPedIsStandingOnEntity = false;
    }

    CVector worldCOM_A = GetMatrix().TransformVector(m_vecCentreOfMass);
    CVector worldCOM_B = physical->GetMatrix().TransformVector(physical->m_vecCentreOfMass);

    if (physicalFlags.bInfiniteMass) {
        worldCOM_A.Reset();
    }
    if (physical->physicalFlags.bInfiniteMass) {
        worldCOM_B.Reset();
    }

    bool bApplyCollisionWithElasticity = !physical->IsStatic() || bEntityCollisionForceDisabled;
    if (!bApplyCollisionWithElasticity) {
        if (physicalFlags.bDisableTurnForce) {
            return ApplySoftCollision(physical, colPoint, ipulseMagnitudeA);
        }
    }

    CVector pointPosA = colPoint.m_vecPoint - GetPosition();
    const CVector speedA = GetSpeed(pointPosA);

    float normalSpeedA = speedA.Dot(colPoint.m_vecNormal);
    if (!bApplyCollisionWithElasticity && normalSpeedA < 0.0f) {
        if (!physical->IsObject()) {
            if (!physical->physicalFlags.bDisableCollisionForce) {
                physical->SetIsStatic(false);
                CWorld::Players[CWorld::PlayerInFocus].m_nHavocCaused += 2;
                CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, static_cast<float>(CGeneral::GetRandomNumberInRange(30, 60)));
            }
        } else {
            const CVector diff = pointPosA - worldCOM_A;
            const float massA = GetTurnTorque(diff, colPoint.m_vecNormal);
            impulseMagnitudeB = ipulseMagnitudeA = massA * normalSpeedA * (m_bHasHitWall ? -1.0f : -(m_fElasticity + 1.0f));

            if (entityObject->m_nColDamageEffect != COL_DAMAGE_EFFECT_NONE && ipulseMagnitudeA > 20.0f) {
                entityObject->ObjectDamage(ipulseMagnitudeA, &colPoint.m_vecPoint, &colPoint.m_vecNormal, this, WEAPON_UNIDENTIFIED);
                if (!physical->m_bUsesCollision) {
                    if (!physicalFlags.bDisableCollisionForce) {
                        const CVector vecMoveSpeed = (colPoint.m_vecNormal * 0.2f) * ipulseMagnitudeA;
                        ApplyForce(vecMoveSpeed, pointPosA, true);
                    }

                    ReportCollision(this, physical, true, colPoint, ipulseMagnitudeA / massA * 0.2f, 1.0f);
                    return false;
                }
            }

            const float fObjectUprootLimit = entityObject->m_pObjectInfo->m_fUprootLimit;
            if ((ipulseMagnitudeA > fObjectUprootLimit || m_bIsStuck) && !physical->physicalFlags.bDisableCollisionForce) {
                if (IsGlassModel(physical)) {
                    CGlass::WindowRespondsToCollision(physical, ipulseMagnitudeA, m_vecMoveSpeed, colPoint.m_vecPoint, false);
                } else if (!physical->physicalFlags.bDisableCollisionForce) {
                    physical->SetIsStatic(false);
                }

                const int32 entityModelIndex = physical->m_nModelIndex;
                if (entityModelIndex == MODEL_FIRE_HYDRANT && !entityObject->objectFlags.bIsExploded) {
                    g_fx.TriggerWaterHydrant(physical->GetPosition());
                    entityObject->objectFlags.bIsExploded = true;
                } else if ((entityModelIndex == ModelIndices::MI_PARKINGMETER || entityModelIndex == ModelIndices::MI_PARKINGMETER2) && !entityObject->objectFlags.bIsExploded) {
                    CPickups::CreateSomeMoney(physical->GetPosition(), CGeneral::GetRandomNumber() % 100);
                    entityObject->objectFlags.bIsExploded = true;
                } else if (physical->IsObject() && !CModelInfo::GetModelInfo(entityModelIndex)->AsAtomicModelInfoPtr()) {
                    entityObject->objectFlags.bIsExploded = true;
                }

                if (physical->IsStatic()) {
                    return false;
                }

                if (!physical->physicalFlags.bDisableCollisionForce) {
                    physical->AddToMovingList();
                }

                bApplyCollisionWithElasticity = true;
            } else {
                if (IsGlassModel(physical)) {
                    CGlass::WindowRespondsToSoftCollision(entityObject, ipulseMagnitudeA);
                }

                CVector vecThisMoveForce = colPoint.m_vecNormal * ipulseMagnitudeA;
                if (IsVehicle() && colPoint.m_vecNormal.z < 0.7f) {
                    vecThisMoveForce.z *= 0.3f;
                }

                if (!physicalFlags.bDisableCollisionForce) {
                    const bool bUpdateTorque = !IsVehicle() || !CWorld::bNoMoreCollisionTorque;
                    ApplyForce(vecThisMoveForce, pointPosA, bUpdateTorque);
                }

                ReportCollision(this, physical, true, colPoint, ipulseMagnitudeA / massA, 1.0f);
            }
        }
    }

    if (!bApplyCollisionWithElasticity) {
        return true;
    }

    // Lambda to compute speed with optional moving collision adjustment
    const auto getAdjustedSpeed = [](CPhysical* physical, CVector& dist, eSurfaceType surfaceType, bool isThis) -> CVector {
        CVector speed = physical->GetSpeed(dist);
        if (physical->physicalFlags.bAddMovingCollisionSpeed && physical->IsVehicle()
            && ((isThis && surfaceType == SURFACE_CAR_MOVINGCOMPONENT)
                || (!isThis && surfaceType == SURFACE_CAR_MOVINGCOMPONENT))) {
            speed += physical->AsVehicle()->AddMovingCollisionSpeed(dist);
        }
        return speed;
    };

    const float fTheElasticity = (physical->m_fElasticity + m_fElasticity) * 0.5f;

    if (physicalFlags.bDisableTurnForce && physical->physicalFlags.bDisableTurnForce) {
        normalSpeedA = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
        const float normalSpeedB = physical->m_vecMoveSpeed.Dot(colPoint.m_vecNormal);

        const float massA = m_fMass;
        const float massB = physical->m_fMass;

        float speed = normalSpeedB;
        bool bPedFollowPath = thisPed->m_nPedState == PEDSTATE_FOLLOW_PATH;
        if (bPedFollowPath) {
            speed = (massB * normalSpeedB + massA * normalSpeedA + massA * normalSpeedA) / (massA + massA + massB);
        } else if (normalSpeedB >= 0.0f) {
            speed = 0.0f;
        }

        const float diff = normalSpeedA - speed;
        if (diff >= 0.0f) {
            return false;
        }

        const float fThisMoveSpeedElasticity = m_bHasHitWall
            ? speed
            : speed - diff * fTheElasticity;
        ipulseMagnitudeA = (fThisMoveSpeedElasticity - normalSpeedA) * massA;
        const CVector vecThisMoveForce = colPoint.m_vecNormal * ipulseMagnitudeA;

        if (!physicalFlags.bDisableCollisionForce) {
            ApplyMoveForce(vecThisMoveForce);
        }

        ReportCollision(this, physical, false, colPoint, ipulseMagnitudeA / massA, 1.0f);
        if (bPedFollowPath) {
            const float fEntityMoveSpeedDifference = normalSpeedB - speed;
            if (fEntityMoveSpeedDifference < 0.0f) {
                const float fEntityMoveSpeedElasticity = physical->m_bHasHitWall ? speed : speed - fEntityMoveSpeedDifference * fTheElasticity;
                impulseMagnitudeB = -((fEntityMoveSpeedElasticity - normalSpeedB) * massB);
                const CVector vecEntityMoveForce = colPoint.m_vecNormal * -impulseMagnitudeB;

                if (!physical->physicalFlags.bDisableCollisionForce) {
                    physical->ApplyMoveForce(vecEntityMoveForce);
                }

                ReportCollision(physical, this, false, colPoint, impulseMagnitudeB / massB, 1.0f);
            }
        }
        return true;
    }

    if (physicalFlags.bDisableTurnForce) {
        CVector vecDistanceToPoint = colPoint.m_vecPoint - physical->GetPosition();
        const CVector vecEntitySpeed = getAdjustedSpeed(physical, vecDistanceToPoint, colPoint.m_nSurfaceTypeB, false);

        normalSpeedA = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
        const float fEntitySpeedDotProduct = vecEntitySpeed.Dot(colPoint.m_vecNormal);

        const float massA = massFactorA * m_fMass;

        const CVector vecEntityDifference = vecDistanceToPoint - worldCOM_B;
        const float massB = physical->GetTurnTorque(vecEntityDifference, colPoint.m_vecNormal);

        float speed = bEntityCollisionForceDisabled
            ? fEntitySpeedDotProduct
            : (massB * fEntitySpeedDotProduct + massA * normalSpeedA) / (massB + massA);

        const float diff = normalSpeedA - speed;
        if (diff >= 0.0f) {
            return false;
        }

        const float fThisMoveSpeedElasticity = m_bHasHitWall
            ? speed
            : speed - diff * fTheElasticity;
        const float fEntityMoveSpeedElasticity = physical->m_bHasHitWall
            ? speed
            : speed - (fEntitySpeedDotProduct - speed) * fTheElasticity;

        ipulseMagnitudeA = (fThisMoveSpeedElasticity - normalSpeedA) * massA;
        impulseMagnitudeB = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * massB);

        CVector vecThisMoveForce = colPoint.m_vecNormal * (ipulseMagnitudeA / massFactorA);
        CVector vecEntityMoveForce = colPoint.m_vecNormal * -(impulseMagnitudeB / massFactorB);

        if (!physicalFlags.bDisableCollisionForce) {
            if (vecThisMoveForce.z < 0.0f) {
                vecThisMoveForce.z = 0.0f;
            }
            if (bThisPedIsStandingOnEntity) {
                vecThisMoveForce.x *= 2.0f;
                vecThisMoveForce.y *= 2.0f;
            }
            ApplyMoveForce(vecThisMoveForce);
        }

        if (!physical->physicalFlags.bDisableCollisionForce && !bThisPedIsStandingOnEntity) {
            ApplyForce(vecEntityMoveForce, vecDistanceToPoint, true);
        }

        ReportCollision(this, physical, true, colPoint, ipulseMagnitudeA / massA, 1.0f);
        ReportCollision(physical, this, false, colPoint, impulseMagnitudeB / massB, 1.0f);
        return true;
    } else if (physical->physicalFlags.bDisableTurnForce) {
        const CVector vecThisSpeed = getAdjustedSpeed(this, pointPosA, colPoint.m_nSurfaceTypeA, true);

        normalSpeedA = vecThisSpeed.Dot(colPoint.m_vecNormal);
        const float normalSpeedB = physical->m_vecMoveSpeed.Dot(colPoint.m_vecNormal);

        const CVector vecThisDifference = pointPosA - worldCOM_A;
        const float massA = GetTurnTorque(vecThisDifference, colPoint.m_vecNormal);
        const float massB = massFactorB * physical->m_fMass;

        const float speed = (massB * normalSpeedB + massA * normalSpeedA) / (massB + massA);

        const float diff = normalSpeedA - speed;
        if (diff >= 0.0f) {
            return false;
        }

        const float fThisMoveSpeedElasticity = m_bHasHitWall ? speed : speed - diff * fTheElasticity;
        const float fEntityMoveSpeedElasticity = physical->m_bHasHitWall ? speed : speed - (normalSpeedB - speed) * fTheElasticity;

        ipulseMagnitudeA = (fThisMoveSpeedElasticity - normalSpeedA) * massA;
        impulseMagnitudeB = -((fEntityMoveSpeedElasticity - normalSpeedB) * massB);

        CVector vecThisMoveForce = colPoint.m_vecNormal * (ipulseMagnitudeA / massFactorA);
        CVector vecEntityMoveForce = colPoint.m_vecNormal * -(impulseMagnitudeB / massFactorB);

        if (!physicalFlags.bDisableCollisionForce && !bEntityPedIsStandingOnThis) {
            if (vecThisMoveForce.z < 0.0f) {
                vecThisMoveForce.z = 0.0f;
            }
            ApplyForce(vecThisMoveForce, pointPosA, true);
        }

        if (!physical->physicalFlags.bDisableCollisionForce) {
            if (vecEntityMoveForce.z < 0.0f) {
                vecEntityMoveForce.z = 0.0f;
                if (std::abs(normalSpeedA) < 0.01f) {
                    vecEntityMoveForce.x /= 2;
                    vecEntityMoveForce.y /= 2;
                }
            }
            if (bEntityPedIsStandingOnThis) {
                vecEntityMoveForce.x *= 2;
                vecEntityMoveForce.y *= 2;
            }
            physical->ApplyMoveForce(vecEntityMoveForce);
        }

        ReportCollision(this, physical, true, colPoint, ipulseMagnitudeA / massA, 1.0f);
        ReportCollision(physical, this, false, colPoint, impulseMagnitudeB / massB, 1.0f);
        return true;
    } else {
        CVector pointPosB = colPoint.m_vecPoint - physical->GetPosition();
        const CVector vecThisSpeed = getAdjustedSpeed(this, pointPosB, colPoint.m_nSurfaceTypeA, true);
        const CVector vecEntitySpeed = getAdjustedSpeed(physical, pointPosB, colPoint.m_nSurfaceTypeB, false);

        normalSpeedA = vecThisSpeed.Dot(colPoint.m_vecNormal);
        const float normalSpeedB = vecEntitySpeed.Dot(colPoint.m_vecNormal);

        const CVector vecThisDifference = pointPosB - worldCOM_A;
        const float massA = GetTurnTorque(vecThisDifference, colPoint.m_vecNormal);

        const CVector vecEntityDifference = pointPosB - worldCOM_B;
        const float massB = GetTurnTorque(vecEntityDifference, colPoint.m_vecNormal);

        const float speed = (massB * normalSpeedB + massA * normalSpeedA) / (massB + massA);

        const float diff = normalSpeedA - speed;
        if (diff >= 0.0f) {
            return false;
        }

        const float fThisMoveSpeedElasticity = m_bHasHitWall
            ? speed
            : speed - diff * fTheElasticity;
        const float fEntityMoveSpeedElasticity = physical->m_bHasHitWall
            ? speed
            : speed - (normalSpeedB - speed) * fTheElasticity;

        ipulseMagnitudeA = (fThisMoveSpeedElasticity - normalSpeedA) * massA;
        impulseMagnitudeB = -((fEntityMoveSpeedElasticity - normalSpeedB) * massB);

        CVector moveForceA = colPoint.m_vecNormal * (ipulseMagnitudeA / massFactorA);
        CVector moveForceB = colPoint.m_vecNormal * -(impulseMagnitudeB / massFactorB);

        if (IsVehicle() && !m_bHasHitWall) {
            if (colPoint.m_vecNormal.z < 0.7f) {
                moveForceA.z *= 0.3f;
            }
            if (!m_nStatus) {
                pointPosB *= 0.8f;
            }
            if (CWorld::bNoMoreCollisionTorque) {
                const CVector vecFrictionForce = moveForceA * -0.3f;
                ApplyFrictionForce(vecFrictionForce, pointPosB);
            }
        }

        if (physical->IsVehicle() && !physical->m_bHasHitWall) {
            if (-colPoint.m_vecNormal.z < 0.7f) {
                moveForceB.z *= 0.3f;
            }
            if (!physical->m_nStatus) {
                pointPosB *= 0.8f;
            }
            if (CWorld::bNoMoreCollisionTorque) {
                const CVector vecFrictionForce = moveForceB * -0.3f;
                physical->ApplyFrictionForce(vecFrictionForce, pointPosB);
            }
        }

        if (!physicalFlags.bDisableCollisionForce) {
            ApplyForce(moveForceA, pointPosB, true);
        }

        if (!physical->physicalFlags.bDisableCollisionForce) {
            if (physical->m_bIsInSafePosition) {
                physical->UnsetIsInSafePosition();
            }
            physical->ApplyForce(moveForceB, pointPosB, true);
        }

        ReportCollision(this, physical, true, colPoint, ipulseMagnitudeA / massA, 1.0f);
        ReportCollision(physical, this, false, colPoint, impulseMagnitudeB / massB, 1.0f);
        return true;
    }
    return false;
}

// 0x54BA60
bool CPhysical::ProcessCollisionSectorList(int32 sectorX, int32 sectorY) {
    static CColPoint(&colPoints)[32] = *(CColPoint(*)[32])0xB73710; // TODO | STATICREF

    bool bResult = false;

    bool bCollisionDisabled = false;
    bool bCollidedEntityCollisionIgnored = false;
    bool bCollidedEntityUnableToMove = false;
    bool bThisOrCollidedEntityStuck = false;

    float fThisDamageIntensity = -1.0f;
    float fEntityDamageIntensity = -1.0f;
    float fThisMaxDamageIntensity = 0.0f;
    float fEntityMaxDamageIntensity = 0.0f;

    CBaseModelInfo* mi = CModelInfo::GetModelInfo(m_nModelIndex);
    float radius = mi->GetColModel()->GetBoundRadius();
    CVector vecBoundCentre = GetBoundCentre();

    const auto ProcessSectorList = [&]<typename PtrListType>(PtrListType& list) {
        auto* const thisObject = AsObject();
        auto* const thisPed = AsPed();
        auto* const thisVehicle = AsVehicle();
        for (auto* const entity : list) {
            auto* const physicalEntity = entity->AsPhysical();
            auto* const entityObject = entity->AsObject();
            auto* const entityPed = entity->AsPed();
            auto* const entityVehicle = entity->AsVehicle();

            if (!entity->m_bUsesCollision || physicalEntity == this || entity->IsScanCodeCurrent()) {
                continue;
            }

            if (!entity->GetIsTouching(vecBoundCentre, radius)) {
                if (m_pEntityIgnoredCollision == entity && static_cast<CEntity*>(m_pAttachedTo) != entity) {
                    m_pEntityIgnoredCollision = nullptr;
                }

                if (entity->IsPhysical() && physicalEntity->m_pEntityIgnoredCollision == this && physicalEntity->m_pAttachedTo != this) {
                    physicalEntity->m_pEntityIgnoredCollision = nullptr;
                }
                continue;
            }

            bCollisionDisabled = false;
            bCollidedEntityCollisionIgnored = false;
            bCollidedEntityUnableToMove = false;
            bThisOrCollidedEntityStuck = false;

            physicalFlags.bSkipLineCol = false;

            if (entity->IsBuilding()) {
                bCollidedEntityCollisionIgnored = false;
                if (physicalFlags.bInfiniteMass && m_bIsStuck) {
                    bThisOrCollidedEntityStuck = true;
                }

                if (physicalFlags.bDisableCollisionForce && (!IsVehicle() || thisVehicle->IsSubTrain())) {
                    bCollisionDisabled = true;
                } else {
                    if (m_pAttachedTo && m_pAttachedTo->IsPhysical() && m_pAttachedTo->physicalFlags.bDisableCollisionForce) {
                        bCollisionDisabled = true;
                    } else if (m_pEntityIgnoredCollision == entity) {
                        bCollisionDisabled = true;
                    } else if (!physicalFlags.bDisableZ || physicalFlags.bApplyGravity) {
                        if (physicalFlags.bDontCollideWithFlyers) {
                            if (m_nStatus) {
                                if (m_nStatus != STATUS_REMOTE_CONTROLLED && entity->DoesNotCollideWithFlyers()) {
                                    bCollisionDisabled = true;
                                }
                            }
                        }
                    } else {
                        bCollisionDisabled = true;
                    }
                }
            } else {
                SpecialEntityPreCollisionStuff(
                    entity->AsPhysical(),
                    false,
                    bCollisionDisabled,
                    bCollidedEntityCollisionIgnored,
                    bCollidedEntityUnableToMove,
                    bThisOrCollidedEntityStuck
                );
            }

            if (!m_bUsesCollision || bCollidedEntityCollisionIgnored || bCollisionDisabled) {
                entity->SetCurrentScanCode();
                if (!bCollisionDisabled) // if collision is enabled then
                {
                    int32 totalColPointsToProcess = ProcessEntityCollision(physicalEntity, &colPoints[0]);
                    if (physicalFlags.b17 && !bCollidedEntityCollisionIgnored && totalColPointsToProcess > 0) {
                        return true;
                    }
                    if (!totalColPointsToProcess && m_pEntityIgnoredCollision == entity && this == FindPlayerPed()) {
                        m_pEntityIgnoredCollision = nullptr;
                    }
                }
                continue;
            }

            if (entity->IsBuilding() || physicalEntity->physicalFlags.bCollidable || bCollidedEntityUnableToMove) {
                entity->SetCurrentScanCode();

                int32   totalAcceptableColPoints = 0;
                float   fThisMaxDamageIntensity = 0.0f;
                CVector vecMoveSpeed{};
                CVector vecTurnSpeed{};

                int32 totalColPointsToProcess = ProcessEntityCollision(physicalEntity, &colPoints[0]);
                if (totalColPointsToProcess > 0) {
                    if (m_bHasContacted) {
                        if (totalColPointsToProcess > 0) {
                            for (int32 colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++) {
                                CColPoint* colPoint = &colPoints[colPointIndex];
                                if (bThisOrCollidedEntityStuck || (colPoint->m_nPieceTypeA >= 13 && colPoint->m_nPieceTypeA <= 16)) {
                                    ApplySoftCollision(entity, *colPoint, fThisDamageIntensity);
                                } else if (ApplyCollisionAlt(physicalEntity, *colPoint, fThisDamageIntensity, vecMoveSpeed, vecTurnSpeed)) {
                                    ++totalAcceptableColPoints;
                                    if (fThisDamageIntensity > fThisMaxDamageIntensity) {
                                        fThisMaxDamageIntensity = fThisDamageIntensity;
                                    }

                                    if (IsVehicle()) {
                                        if (!thisVehicle->IsBoat() || colPoint->m_nSurfaceTypeB != SURFACE_WOOD_SOLID) {
                                            SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint, 1.0f);
                                        }
                                    } else {
                                        SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint, 1.0f);
                                    }
                                }
                            }
                        }
                    } else {
                        for (int32 colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++) {
                            CColPoint* colPoint = &colPoints[colPointIndex];
                            if (bThisOrCollidedEntityStuck || (colPoint->m_nPieceTypeA >= 13 && colPoint->m_nPieceTypeA <= 16)) {
                                if (ApplySoftCollision(entity, *colPoint, fThisDamageIntensity) && (colPoint->m_nSurfaceTypeA != SURFACE_WHEELBASE || colPoint->m_nSurfaceTypeB != SURFACE_WHEELBASE)) {
                                    float fSurfaceFriction = g_surfaceInfos.GetAdhesiveLimit(colPoint);
                                    if (ApplyFriction(fSurfaceFriction, *colPoint)) {
                                        m_bHasContacted = true;
                                    }
                                    continue;
                                }
                            } else if (ApplyCollisionAlt(physicalEntity, *colPoint, fThisDamageIntensity, vecMoveSpeed, vecTurnSpeed)) {
                                ++totalAcceptableColPoints;
                                if (fThisDamageIntensity > fThisMaxDamageIntensity) {
                                    fThisMaxDamageIntensity = fThisDamageIntensity;
                                }

                                float fSurfaceFriction = g_surfaceInfos.GetAdhesiveLimit(colPoint);
                                float fFriction = fSurfaceFriction / totalColPointsToProcess;
                                if (!IsVehicle()) {
                                    fFriction *= PHYSICAL_CAR_FRICTION_MULT * fThisDamageIntensity;
                                    SetDamagedPieceRecord(fThisDamageIntensity, entity, *colPoint, 1.0f);
                                    if (ApplyFriction(fFriction, *colPoint)) {
                                        m_bHasContacted = true;
                                    }
                                    continue;
                                }
                                if (!thisVehicle->IsBoat() || colPoint->m_nSurfaceTypeB != SURFACE_WOOD_SOLID) {
                                    SetDamagedPieceRecord(fThisDamageIntensity, entity, *colPoint, 1.0f);
                                } else {
                                    fFriction = 0.0f;
                                }

                                if (m_nModelIndex == MODEL_RCBANDIT) {
                                    fFriction *= 0.2f;
                                } else {
                                    if (thisVehicle->IsBoat()) {
                                        if (colPoint->m_vecNormal.z > 0.6f) {
                                            if (g_surfaceInfos.GetAdhesionGroup(colPoint->m_nSurfaceTypeB) == ADHESION_GROUP_LOOSE || g_surfaceInfos.GetAdhesionGroup(colPoint->m_nSurfaceTypeB) == ADHESION_GROUP_SAND) {
                                                fFriction *= 3.0f;
                                            }
                                        } else {
                                            fFriction = 0.0f;
                                        }
                                    }

                                    if (!thisVehicle->IsTrain()) {
                                        if (m_nStatus == STATUS_WRECKED) {
                                            fFriction *= 3.0f;
                                        } else {
                                            if (GetUp().z > 0.3f && m_vecMoveSpeed.SquaredMagnitude() < PHYSICAL_CAR_FRICTION_SPEED_LIMSQR && m_vecTurnSpeed.SquaredMagnitude() < PHYSICAL_CAR_FRICTION_TURN_LIMSQR) {
                                                fFriction = 0.0f;
                                            } else {
                                                if (m_nStatus != STATUS_ABANDONED && colPoint->m_vecNormal.Dot(GetUp()) >= 0.707f) {
                                                } else {
                                                    fFriction = PHYSICAL_CAR_FRICTION_MULT / m_fMass * fFriction * fThisDamageIntensity;
                                                }
                                            }
                                        }
                                    }
                                }

                                if (IsVehicle() && thisVehicle->IsSubTrain()) {
                                    fFriction = fFriction + fFriction;
                                }
                                if (ApplyFriction(fFriction, *colPoint)) {
                                    m_bHasContacted = true;
                                }
                            }
                        }
                    }
                }

                if (totalAcceptableColPoints) {
                    float fSpeedFactor = 1.0f / totalAcceptableColPoints;
                    m_vecMoveSpeed += vecMoveSpeed * fSpeedFactor;
                    m_vecTurnSpeed += vecTurnSpeed * fSpeedFactor;
                    if (!CWorld::bNoMoreCollisionTorque) {
                        if (!m_nStatus && IsVehicle()) {
                            float fThisMoveSpeedX = m_vecMoveSpeed.x;
                            if (m_vecMoveSpeed.x < 0.0f) {
                                fThisMoveSpeedX = -fThisMoveSpeedX;
                            }

                            if (fThisMoveSpeedX > 0.2f) {
                                float fThisMoveSpeedY = m_vecMoveSpeed.y;
                                if (m_vecMoveSpeed.y < 0.0f) {
                                    fThisMoveSpeedY = -fThisMoveSpeedY;
                                }
                                if (fThisMoveSpeedY > 0.2f) {
                                    if (!physicalFlags.bSubmergedInWater) {
                                        m_vecMoveFriction.x -= vecMoveSpeed.x * 0.3f / (float)totalColPointsToProcess;
                                        m_vecMoveFriction.y -= vecMoveSpeed.y * 0.3f / (float)totalColPointsToProcess;
                                        m_vecTurnFriction += (vecTurnSpeed * -0.3f) / (float)totalColPointsToProcess;
                                    }
                                }
                            }
                        }
                    }

                    if (entity->IsObject() && entityObject->m_nColDamageEffect && fThisMaxDamageIntensity > 20.0f) {
                        entityObject->ObjectDamage(fThisMaxDamageIntensity, &colPoints[0].m_vecPoint, &fxDirection, this, WEAPON_UNIDENTIFIED);
                    }
                    if (!CWorld::bSecondShift) {
                        return true;
                    }

                    bResult = true;
                }
            } else {
                entity->SetCurrentScanCode();

                int32 totalAcceptableColPoints = 0;
                int32 totalColPointsToProcess = ProcessEntityCollision(physicalEntity, &colPoints[0]);
                if (totalColPointsToProcess <= 0) {
                    continue;
                }

                fThisMaxDamageIntensity = 0.0f;
                fEntityMaxDamageIntensity = 0.0f;
                if (m_bHasContacted && entity->m_bHasContacted) {
                    if (totalColPointsToProcess > 0) {
                        for (int32 colPointIndex4 = 0; colPointIndex4 < totalColPointsToProcess; colPointIndex4++) {
                            CColPoint* colPoint2 = &colPoints[colPointIndex4];
                            if (bThisOrCollidedEntityStuck || (colPoint2->m_nPieceTypeA >= 13 && colPoint2->m_nPieceTypeA <= 16) || (colPoint2->m_nPieceTypeB >= 13 && colPoint2->m_nPieceTypeB <= 16)) {
                                ++totalAcceptableColPoints;
                                ApplySoftCollision(physicalEntity, *colPoint2, fThisDamageIntensity, fEntityDamageIntensity);
                            } else {
                                if (ApplyCollision(entity, *colPoint2, fThisDamageIntensity, fEntityDamageIntensity)) {
                                    if (fThisDamageIntensity > fThisMaxDamageIntensity) {
                                        fThisMaxDamageIntensity = fThisDamageIntensity;
                                    }
                                    if (fEntityDamageIntensity > fEntityMaxDamageIntensity) {
                                        fEntityMaxDamageIntensity = fEntityDamageIntensity;
                                    }

                                    SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint2, 1.0f);
                                    physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint2, -1.0f);
                                }
                            }
                        }
                    }
                } else if (m_bHasContacted) {
                    m_bHasContacted = false;
                    CVector vecThisFrictionMoveSpeed = m_vecMoveFriction;
                    CVector vecThisFrictionTurnSpeed = m_vecTurnFriction;
                    ResetFrictionMoveSpeed();
                    ResetFrictionTurnSpeed();
                    if (totalColPointsToProcess > 0) {
                        for (int32 colPointIndex1 = 0; colPointIndex1 < totalColPointsToProcess; colPointIndex1++) {
                            CColPoint* colPoint1 = &colPoints[colPointIndex1];
                            if (bThisOrCollidedEntityStuck || (colPoint1->m_nPieceTypeA >= 13 && colPoint1->m_nPieceTypeA <= 16) || (colPoint1->m_nPieceTypeB >= 13 && colPoint1->m_nPieceTypeB <= 16)) {
                                ++totalAcceptableColPoints;
                                ApplySoftCollision(physicalEntity, *colPoint1, fThisDamageIntensity, fEntityDamageIntensity);
                                if (colPoint1->m_nPieceTypeB >= 13 && colPoint1->m_nPieceTypeB <= 16) {
                                    physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint1, -1.0f);
                                }
                            } else if (ApplyCollision(physicalEntity, *colPoint1, fThisDamageIntensity, fEntityDamageIntensity)) {
                                if (fThisDamageIntensity > fThisMaxDamageIntensity) {
                                    fThisMaxDamageIntensity = fThisDamageIntensity;
                                }
                                if (fEntityDamageIntensity > fEntityMaxDamageIntensity) {
                                    fEntityMaxDamageIntensity = fEntityDamageIntensity;
                                }

                                SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint1, 1.0f);
                                physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint1, -1.0f);

                                float fSurfaceFriction1 = g_surfaceInfos.GetAdhesiveLimit(colPoint1);
                                float fFriction3 = fSurfaceFriction1 / totalColPointsToProcess;
                                if (IsVehicle() && entity->IsVehicle() && (m_vecMoveSpeed.SquaredMagnitude() > PHYSICAL_CAR_FRICTION_SPEED_LIMSQR || m_vecTurnSpeed.SquaredMagnitude() > PHYSICAL_CAR_FRICTION_TURN_LIMSQR)) {
                                    fFriction3 *= PHYSICAL_CARCAR_FRICTION_MULT * fThisDamageIntensity;
                                }

                                if (entity->IsStatic()) {
                                    if (ApplyFriction(fFriction3, *colPoint1)) {
                                        m_bHasContacted = true;
                                    }
                                } else if (ApplyFriction(physicalEntity, fFriction3, *colPoint1)) {
                                    m_bHasContacted = true;
                                    entity->m_bHasContacted = true;
                                }
                            }
                        }
                    }

                    if (!m_bHasContacted) {
                        m_bHasContacted = true;
                        m_vecMoveFriction = vecThisFrictionMoveSpeed;
                        m_vecTurnFriction = vecThisFrictionTurnSpeed;
                    }
                } else {
                    if (entity->m_bHasContacted) {
                        entity->m_bHasContacted = false;
                        CVector vecEntityMoveSpeed = physicalEntity->m_vecMoveFriction;
                        CVector vecEntityFrictionTurnSpeed = physicalEntity->m_vecTurnFriction;
                        physicalEntity->ResetFrictionMoveSpeed();
                        physicalEntity->ResetFrictionTurnSpeed();

                        if (totalColPointsToProcess > 0) {
                            for (int32 colPointIndex3 = 0; colPointIndex3 < totalColPointsToProcess; colPointIndex3++) {
                                CColPoint* colPoint4 = &colPoints[colPointIndex3];
                                if (bThisOrCollidedEntityStuck || (colPoint4->m_nPieceTypeA >= 13 && colPoint4->m_nPieceTypeA <= 16) || (colPoint4->m_nPieceTypeB >= 13 && colPoint4->m_nPieceTypeB <= 16)) {
                                    ++totalAcceptableColPoints;
                                    ApplySoftCollision(physicalEntity, *colPoint4, fThisDamageIntensity, fEntityDamageIntensity);
                                    if (colPoint4->m_nPieceTypeB >= 13 && colPoint4->m_nPieceTypeB <= 16) {
                                        physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint4, -1.0f);
                                    }
                                } else if (ApplyCollision(physicalEntity, *colPoint4, fThisDamageIntensity, fEntityDamageIntensity)) {
                                    if (fThisDamageIntensity > fThisMaxDamageIntensity) {
                                        fThisMaxDamageIntensity = fThisDamageIntensity;
                                    }

                                    if (fEntityDamageIntensity > fEntityMaxDamageIntensity) {
                                        fEntityMaxDamageIntensity = fEntityDamageIntensity;
                                    }

                                    SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint4, 1.0f);
                                    physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint4, -1.0f);

                                    float fSurfaceFriction1 = g_surfaceInfos.GetAdhesiveLimit(colPoint4);

                                    float fFriction2 = fSurfaceFriction1 / totalColPointsToProcess;
                                    if (IsVehicle() && entity->IsVehicle() && (m_vecMoveSpeed.SquaredMagnitude() > PHYSICAL_CAR_FRICTION_SPEED_LIMSQR || m_vecTurnSpeed.SquaredMagnitude() > PHYSICAL_CAR_FRICTION_TURN_LIMSQR)) {
                                        fFriction2 *= PHYSICAL_CARCAR_FRICTION_MULT * fThisDamageIntensity;
                                    }

                                    if (entity->IsStatic()) {
                                        if (ApplyFriction(fFriction2, *colPoint4)) {
                                            m_bHasContacted = true;
                                        }
                                    } else if (ApplyFriction(physicalEntity, fFriction2, *colPoint4)) {
                                        m_bHasContacted = true;
                                        entity->m_bHasContacted = true;
                                    }
                                }
                            }
                        }

                        if (!entity->m_bHasContacted) {
                            entity->m_bHasContacted = true;
                            physicalEntity->m_vecMoveFriction = vecEntityMoveSpeed;
                            physicalEntity->m_vecTurnFriction = vecEntityFrictionTurnSpeed;
                        }
                    } else if (totalColPointsToProcess > 0) {
                        for (int32 colPointIndex2 = 0; colPointIndex2 < totalColPointsToProcess; colPointIndex2++) {
                            CColPoint* colPoint3 = &colPoints[colPointIndex2];
                            if (bThisOrCollidedEntityStuck || (colPoint3->m_nPieceTypeA >= 13 && colPoint3->m_nPieceTypeA <= 16)
                                // || (colPoint3->m_nPieceTypeA >= 13 && colPoint3->m_nPieceTypeA <= 16) // BUG: I think it should be m_nPieceTypeB
                            ) {
                                ++totalAcceptableColPoints;
                                ApplySoftCollision(physicalEntity, *colPoint3, fThisDamageIntensity, fEntityDamageIntensity);
                                if (colPoint3->m_nPieceTypeB >= 13 && colPoint3->m_nPieceTypeB <= 16) {
                                    physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint3, -1.0f);
                                }
                            } else if (ApplyCollision(physicalEntity, *colPoint3, fThisDamageIntensity, fEntityDamageIntensity)) {
                                if (fThisDamageIntensity > fThisMaxDamageIntensity) {
                                    fThisMaxDamageIntensity = fThisDamageIntensity;
                                }

                                if (fEntityDamageIntensity > fEntityMaxDamageIntensity) {
                                    fEntityMaxDamageIntensity = fEntityDamageIntensity;
                                }

                                SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint3, 1.0f);
                                physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint3, -1.0f);

                                float fSurfaceFirction2 = g_surfaceInfos.GetAdhesiveLimit(colPoint3);
                                float fFriction1 = fSurfaceFirction2 / totalColPointsToProcess;
                                if (IsVehicle() && entity->IsVehicle() && (m_vecMoveSpeed.SquaredMagnitude() > PHYSICAL_CAR_FRICTION_SPEED_LIMSQR || m_vecTurnSpeed.SquaredMagnitude() > PHYSICAL_CAR_FRICTION_TURN_LIMSQR)) {
                                    fFriction1 *= PHYSICAL_CARCAR_FRICTION_MULT * fThisDamageIntensity;
                                }

                                if (entity->IsStatic()) {
                                    if (ApplyFriction(fFriction1, *colPoint3)) {
                                        m_bHasContacted = true;
                                    }
                                } else if (ApplyFriction(physicalEntity, fFriction1, *colPoint3)) {
                                    m_bHasContacted = true;
                                    entity->m_bHasContacted = true;
                                }
                            }
                        }
                    }
                }

                if (entity->IsPed() && IsVehicle()) {
                    float fThisMoveSpeedDot = m_vecMoveSpeed.SquaredMagnitude();
                    if (!entityPed->IsPlayer() || entity->m_bIsStuck && m_vecMoveSpeed.SquaredMagnitude() > 0.0025f) {
                        entityPed->KillPedWithCar(AsVehicle(), fEntityMaxDamageIntensity, false);
                    }
                } else if (IsPed() && entity->IsVehicle() && entityVehicle->IsSubTrain() && (entityVehicle->m_vecMoveSpeed.Dot(m_vecLastCollisionImpactVelocity) > 0.2f || thisPed->bFallenDown && entityVehicle->m_vecMoveSpeed.SquaredMagnitude() > 0.0005f)) {
                    float fDamageIntensity = fThisMaxDamageIntensity + fThisMaxDamageIntensity;
                    thisPed->KillPedWithCar(entityVehicle, fDamageIntensity, false);
                } else if (entity->IsObject() && IsVehicle() && entity->m_bUsesCollision) {
                    if (entityObject->m_nColDamageEffect && fEntityMaxDamageIntensity > 20.0f) {
                        entityObject->ObjectDamage(fEntityMaxDamageIntensity, &colPoints[0].m_vecPoint, &fxDirection, this, WEAPON_RUNOVERBYCAR);
                    } else {
                        if (entityObject->m_nColDamageEffect >= COL_DAMAGE_EFFECT_SMASH_COMPLETELY) {
                            CBaseModelInfo* pEntityModelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
                            CColModel*      colModel = pEntityModelInfo->GetColModel();
                            CVector         boundBoxPos = entity->GetMatrix().TransformPoint(colModel->m_boundBox.GetSize());

                            bool bObjectDamage = false;
                            if (GetPosition().z > boundBoxPos.z) {
                                bObjectDamage = true;
                            } else {
                                CMatrix invertedMatrix;
                                invertedMatrix = Invert(*m_matrix, invertedMatrix);
                                if ((invertedMatrix.TransformPoint(boundBoxPos)).z < 0.0f) {
                                    bObjectDamage = true;
                                }
                            }
                            if (bObjectDamage) {
                                entityObject->ObjectDamage(50.0f, &colPoints[0].m_vecPoint, &fxDirection, this, WEAPON_RUNOVERBYCAR);
                            }
                        }
                    }
                } else if (IsObject() && entity->IsVehicle() && m_bUsesCollision) {
                    if (thisObject->m_nColDamageEffect && fEntityMaxDamageIntensity > 20.0f) {
                        thisObject->ObjectDamage(fEntityMaxDamageIntensity, &colPoints[0].m_vecPoint, &fxDirection, entity, WEAPON_RUNOVERBYCAR);
                    } else {
                        // BUG: entity is a vehicle here, but we are treating it as an object?
                        if (entityObject->m_nColDamageEffect >= COL_DAMAGE_EFFECT_SMASH_COMPLETELY) {
                            CColModel* colModel = mi->GetColModel();
                            CVector    boundBoxPos = (*m_matrix).TransformPoint(colModel->m_boundBox.GetSize());

                            bool bObjectDamage = false;
                            if (boundBoxPos.z < entity->GetPosition().z) {
                                bObjectDamage = true;
                            } else {
                                CMatrix invertedMatrix;
                                invertedMatrix = Invert(entity->GetMatrix(), invertedMatrix);
                                if ((invertedMatrix.TransformPoint(boundBoxPos)).z < 0.0f) {
                                    bObjectDamage = true;
                                }
                            }

                            if (bObjectDamage) {
                                thisObject->ObjectDamage(50.0f, &colPoints[0].m_vecPoint, &fxDirection, entity, WEAPON_RUNOVERBYCAR);
                            }
                        }
                    }
                }

                if (entity->m_nStatus == STATUS_SIMPLE) {
                    entity->m_nStatus = STATUS_PHYSICS;
                    if (entity->IsVehicle()) {
                        CCarCtrl::SwitchVehicleToRealPhysics(entityVehicle);
                    }
                }
                if (CWorld::bSecondShift) {
                    bResult = true;
                } else if (totalColPointsToProcess > totalAcceptableColPoints) {
                    return true;
                }
            }
        }
        return false;
    };
    CSector* s = GetSector(sectorX, sectorY);
    CRepeatSector* rs = GetRepeatSector(sectorX, sectorY);
    if (ProcessSectorList(s->m_buildings)
        || ProcessSectorList(rs->Vehicles)
        || ProcessSectorList(rs->Peds)
        || ProcessSectorList(rs->Objects)) {
        return true;
    }
    return bResult;
}

// 0x54CFF0
bool CPhysical::ProcessCollisionSectorList_SimpleCar(CRepeatSector* repeatSector) {
    static CColPoint(&colPoints)[32] = *(CColPoint(*)[32])0xB73C98; // TODO | STATICREF
    float fThisDamageIntensity = -1.0f;
    float fEntityDamageIntensity = -1.0f;

    if (!m_bUsesCollision) {
        return false;
    }

    CVector vecBoundingCentre;
    GetBoundCentre(&vecBoundingCentre);

    float fBoundingRadius = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundRadius();

    int32      totalColPointsToProcess = 0;
    const auto ProcessSectorList = [&]<typename PtrListType>(PtrListType& list) -> CPhysical* {
        for (auto* const entity : list) {
            bool isLampTouchingGround = false;
            if (entity->IsObject() && entity->AsObject()->IsFallenLampPost()) {
                isLampTouchingGround = true;
            }

            if (entity != this
                && !isLampTouchingGround
                && !entity->IsScanCodeCurrent()) {
                if (entity->m_bUsesCollision && entity->GetIsTouching(vecBoundingCentre, fBoundingRadius)) {
                    entity->SetCurrentScanCode();
                    totalColPointsToProcess = ProcessEntityCollision(entity, &colPoints[0]);
                    if (totalColPointsToProcess > 0) {
                        return entity;
                    }
                }
            }
        }
        return nullptr;
    };

    // Find entity we're colliding with
    CPhysical* entity;
    if (!(entity = ProcessSectorList(repeatSector->Vehicles))) {
        if (!(entity = ProcessSectorList(repeatSector->Objects))) {
            return false;
        }
    }

    assert(entity);
    if (m_bHasContacted && entity->m_bHasContacted) {
        if (totalColPointsToProcess > 0) {
            for (int32 colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++) {
                CColPoint* colPoint = &colPoints[colPointIndex];
                if (ApplyCollision(entity, *colPoint, fThisDamageIntensity, fEntityDamageIntensity)) {
                    SetDamagedPieceRecord(fThisDamageIntensity, entity, *colPoint, 1.0f);
                    entity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint, -1.0f);
                }
            }
        }
    } else if (m_bHasContacted) {
        CVector vecOldFrictionMoveSpeed = m_vecMoveFriction;
        CVector vecOldFrictionTurnSpeed = m_vecTurnFriction;
        ResetFrictionTurnSpeed();
        ResetFrictionMoveSpeed();
        m_bHasContacted = false;

        if (totalColPointsToProcess > 0) {
            for (int32 colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++) {
                CColPoint* colPoint = &colPoints[colPointIndex];
                if (ApplyCollision(entity, *colPoint, fThisDamageIntensity, fEntityDamageIntensity)) {
                    SetDamagedPieceRecord(fThisDamageIntensity, entity, *colPoint, 1.0f);
                    entity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint, -1.0f);
                    float fSurfaceFriction = g_surfaceInfos.GetAdhesiveLimit(colPoint);
                    float fFriction = fSurfaceFriction / totalColPointsToProcess;
                    if (ApplyFriction(entity, fFriction, *colPoint)) {
                        m_bHasContacted = true;
                        entity->m_bHasContacted = true;
                    }
                }
            }
        }

        if (!m_bHasContacted) {
            m_vecMoveFriction = vecOldFrictionMoveSpeed;
            m_vecTurnFriction = vecOldFrictionTurnSpeed;
            m_bHasContacted = true;
        }
    } else {
        if (entity->m_bHasContacted) {
            assert(entity);
            CVector vecOldFrictionMoveSpeed = entity->m_vecMoveFriction;
            CVector vecOldFrictionTurnSpeed = entity->m_vecTurnFriction;
            entity->ResetFrictionTurnSpeed();
            entity->ResetFrictionMoveSpeed();
            entity->m_bHasContacted = false;

            if (totalColPointsToProcess > 0) {
                for (int32 colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++) {
                    CColPoint* colPoint = &colPoints[colPointIndex];
                    if (ApplyCollision(entity, *colPoint, fThisDamageIntensity, fEntityDamageIntensity)) {
                        SetDamagedPieceRecord(fThisDamageIntensity, entity, *colPoint, 1.0f);
                        entity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint, -1.0f);
                        float fSurfaceFriction = g_surfaceInfos.GetAdhesiveLimit(colPoint);
                        float fFriction = fSurfaceFriction / totalColPointsToProcess;
                        if (ApplyFriction(entity, fFriction, *colPoint)) {
                            m_bHasContacted = true;
                            entity->m_bHasContacted = true;
                        }
                    }
                }
            }
            if (!entity->m_bHasContacted) {
                entity->m_vecMoveFriction = vecOldFrictionMoveSpeed;
                entity->m_vecTurnFriction = vecOldFrictionTurnSpeed;
                entity->m_bHasContacted = true;
            }
        } else if (totalColPointsToProcess > 0) {
            for (int32 colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++) {
                CColPoint* colPoint = &colPoints[colPointIndex];
                if (ApplyCollision(entity, *colPoint, fThisDamageIntensity, fEntityDamageIntensity)) {
                    SetDamagedPieceRecord(fThisDamageIntensity, entity, *colPoint, 1.0f);
                    entity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint, -1.0f);
                    float fSurfaceFriction = g_surfaceInfos.GetAdhesiveLimit(colPoint);
                    float fFriction = fSurfaceFriction / totalColPointsToProcess;
                    if (ApplyFriction(entity, fFriction, *colPoint)) {
                        m_bHasContacted = true;
                        entity->m_bHasContacted = true;
                    }
                }
            }
        }
    }

    if (entity->m_nStatus == STATUS_SIMPLE) {
        entity->m_nStatus = STATUS_PHYSICS;
        if (entity->IsVehicle()) {
            CCarCtrl::SwitchVehicleToRealPhysics(entity->AsVehicle());
        }
    }
    return true;
}

// 0x54D570
void CPhysical::AttachEntityToEntity(CPhysical* entityAttachTo, CVector vecAttachOffset, CVector vecAttachRotation) {
    if (!entityAttachTo) {
        return;
    }

    CEntity* oldEntityAttachedTo = m_pAttachedTo;
    m_pAttachedTo = entityAttachTo;
    assert(m_pAttachedTo);
    m_pAttachedTo->RegisterReference(reinterpret_cast<CEntity**>(&m_pAttachedTo));
    m_vecAttachOffset = vecAttachOffset;
    if (physicalFlags.bInfiniteMass) {
        m_vecAttachedEntityRotation = GetPosition();
    } else {
        m_vecAttachedEntityRotation = vecAttachRotation;
    }
    m_qAttachedEntityRotation = {};
    m_pEntityIgnoredCollision = oldEntityAttachedTo;
    if (physicalFlags.bDisableCollisionForce) {
        physicalFlags.bCollidable = false;
        PositionAttachedEntity();
    } else {
        if (m_pAttachedTo->IsPhysical()
            && m_pAttachedTo->physicalFlags.bDisableCollisionForce
            && IsObject() && !physicalFlags.bInfiniteMass) {
            physicalFlags.bDisableCollisionForce = true;
            m_fMass = 99999.9f;
            m_fTurnMass = 99999.9f;
        }
        PositionAttachedEntity();
    }
    
}

// 0x54D690
void CPhysical::AttachEntityToEntity(CPhysical* entityAttachTo, CVector* vecAttachOffset, CQuaternion* attachRotation) {
    if (!entityAttachTo) {
        return;
    }

    CPhysical* oldEntityAttachedTo = m_pAttachedTo;
    m_pAttachedTo = entityAttachTo;
    assert(m_pAttachedTo);
    m_pAttachedTo->RegisterReference(reinterpret_cast<CEntity**>(&m_pAttachedTo));
    CMatrix entityAttachedtoMatrix(m_pAttachedTo->GetMatrix());
    auto*   attachedToAutoMobile = m_pAttachedTo->AsAutomobile();
    if (IsObject() && m_pAttachedTo->m_nModelIndex == MODEL_FORKLIFT) {
        if (RwFrame* carMiscAFrame = attachedToAutoMobile->m_aCarNodes[CAR_MISC_A]) {
            entityAttachedtoMatrix.UpdateMatrix(RwFrameGetLTM(carMiscAFrame));
        }
    }

    if (vecAttachOffset) {
        m_vecAttachOffset = *vecAttachOffset;
    } else {
        CVector distance = GetPosition() - entityAttachedtoMatrix.GetPosition();
        m_vecAttachOffset.x = entityAttachedtoMatrix.GetRight().Dot(distance);
        m_vecAttachOffset.y = entityAttachedtoMatrix.GetForward().Dot(distance);
        m_vecAttachOffset.z = entityAttachedtoMatrix.GetUp().Dot(distance);
    }

    if (attachRotation) {
        m_qAttachedEntityRotation = *attachRotation;
    } else {
        CMatrix entityAttachedToMatrixInverted;
        Invert(entityAttachedtoMatrix, entityAttachedToMatrixInverted);
        entityAttachedToMatrixInverted *= *m_matrix;
        RwMatrixTag rwMatrix{};
        entityAttachedToMatrixInverted.CopyToRwMatrix(&rwMatrix);
        RtQuatConvertFromMatrix((RtQuat*)&m_qAttachedEntityRotation, &rwMatrix);
    }

    if (physicalFlags.bDisableCollisionForce) {
        physicalFlags.bCollidable = false;
    } else {
        if (m_pAttachedTo->IsPhysical()
            && m_pAttachedTo->physicalFlags.bDisableCollisionForce
            && IsObject() && !physicalFlags.bInfiniteMass) {
            physicalFlags.bDisableCollisionForce = true;
            m_fMass = 99999.9f;
            m_fTurnMass = 99999.9f;
        }
    }

    if (physicalFlags.bInfiniteMass) {
        m_vecAttachedEntityRotation = GetPosition();
    }

    m_pEntityIgnoredCollision = oldEntityAttachedTo;
    PositionAttachedEntity();
}

// 0x54D920
bool CPhysical::CheckCollision() {
    m_bCollisionProcessed = false;
    if (IsPed()) {
        CPed* ped = AsPed();
        if (!m_pAttachedTo && !physicalFlags.b17 && !physicalFlags.bProcessingShift && !physicalFlags.bSkipLineCol) {
            ped->m_standingOnEntity = nullptr;
            if (ped->bIsStanding) {
                ped->bIsStanding = false;
                ped->bWasStanding = true;
            }
        }

        if (ped->IsPlayer()) {
            CTaskSimpleClimb* taskClimb = ped->m_pIntelligence->GetTaskClimb();
            if (taskClimb) {
                switch (taskClimb->GetHeightForPos()) {
                case CLIMB_GRAB:
                case CLIMB_PULLUP:
                case CLIMB_STANDUP:
                case CLIMB_VAULT:   physicalFlags.bSkipLineCol = true; break;
                }
            }
        }
    }

    CWorld::IncrementCurrentScanCode();

    CRect boundRect = GetBoundRect();

    bool state = false;
    CWorld::IterateSectorsOverlappedByRect(
        boundRect,
        [this, &state](int32 sectorX, int32 sectorY) {
            if (ProcessCollisionSectorList(sectorX, sectorY)) {
                state = true;
                return true;
            }
            return false;
        }
    );
    return state;
}

// 0x54DAB0
bool CPhysical::CheckCollision_SimpleCar() {
    m_bCollisionProcessed = false;
    CWorld::IncrementCurrentScanCode();
    CEntryInfoNode* entryInfoNode = m_pCollisionList.m_node;
    if (!entryInfoNode) {
        return false;
    }

    while (!ProcessCollisionSectorList_SimpleCar(entryInfoNode->m_repeatSector)) {
        entryInfoNode = entryInfoNode->m_next;
        if (!entryInfoNode) {
            return false;
        }
    }
    return true;
}

// NOTSA
void CPhysical::ReportCollision(CPhysical* entity1, CEntity* entity2, bool isFirstA, const CColPoint& colPoint, float collisionImpact1, float collisionImpact2) {
    const auto surf1 = isFirstA ? colPoint.m_nSurfaceTypeA : colPoint.m_nSurfaceTypeB;
    const auto surf2 = isFirstA ? colPoint.m_nSurfaceTypeB : colPoint.m_nSurfaceTypeA;
    AudioEngine.ReportCollision(entity1, entity2, surf1, surf2, colPoint.m_vecPoint, &colPoint.m_vecNormal, collisionImpact1, collisionImpact2, false, false);
}
