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

float& CPhysical::DAMPING_LIMIT_IN_FRAME = *(float*)0x8CD7A0;
float& CPhysical::DAMPING_LIMIT_OF_SPRING_FORCE = *(float*)0x8CD7A4;
float& CPhysical::PHYSICAL_SHIFT_SPEED_DAMP = *(float*)0x8CD788;
float& CPhysical::SOFTCOL_SPEED_MULT = *(float*)0x8CD794;
float& CPhysical::SOFTCOL_SPEED_MULT2 = *(float*)0x8CD798;
float& CPhysical::SOFTCOL_DEPTH_MIN = *(float*)0x8CD78C;
float& CPhysical::SOFTCOL_DEPTH_MULT = *(float*)0x8CD790;
float& CPhysical::SOFTCOL_CARLINE_SPEED_MULT = *(float*)0x8CD79C;
float& CPhysical::TEST_ADD_AMBIENT_LIGHT_FRAC = *(float*)0x8CD7B8;
float& CPhysical::HIGHSPEED_ELASTICITY_MULT_COPCAR = *(float*)0x8CD784;
CVector& CPhysical::fxDirection = *(CVector*)0xB73720;

void CPhysical::InjectHooks()
{
    RH_ScopedVirtualClass(CPhysical, 0x863BA0, 23);
    RH_ScopedCategory("Entity");

    RH_ScopedInstall(Constructor, 0x542260);
    RH_ScopedInstall(Destructor, 0x542450);
    RH_ScopedInstall(RemoveAndAdd, 0x542560);
    RH_ScopedInstall(ApplyTurnForce, 0x542A50);
    RH_ScopedInstall(ApplyForce, 0x542B50);
    RH_ScopedInstall(GetSpeed, 0x542CE0);
    RH_ScopedInstall(ApplyMoveSpeed, 0x542DD0);
    RH_ScopedInstall(ApplyTurnSpeed, 0x542E20);
    RH_ScopedOverloadedInstall(ApplyMoveForce, "vec", 0x5429F0, void(CPhysical::*)(CVector force));
    RH_ScopedInstall(SetDamagedPieceRecord, 0x5428C0);
    RH_ScopedInstall(RemoveFromMovingList, 0x542860);
    RH_ScopedInstall(AddToMovingList, 0x542800);
    RH_ScopedVMTInstall(Add, 0x544A30);
    RH_ScopedVMTInstall(Remove, 0x5424C0);
    RH_ScopedVMTInstall(GetBoundRect, 0x5449B0);
    RH_ScopedVMTInstall(ProcessControl, 0x5485E0);
    RH_ScopedVMTInstall(ProcessCollision, 0x54DFB0);
    RH_ScopedVMTInstall(ProcessShift, 0x54DB10);
    RH_ScopedVMTInstall(TestCollision, 0x54DEC0);
    RH_ScopedVMTInstall(ProcessEntityCollision, 0x546D00);
    RH_ScopedInstall(ApplyGravity, 0x542FE0);
    RH_ScopedInstall(ApplyFrictionMoveForce, 0x5430A0);
    RH_ScopedInstall(ApplyFrictionForce, 0x543220);
    RH_ScopedInstall(SkipPhysics, 0x5433B0);
    RH_ScopedInstall(AddCollisionRecord, 0x543490);
    RH_ScopedInstall(GetHasCollidedWith, 0x543540);
    RH_ScopedInstall(GetHasCollidedWithAnyObject, 0x543580);
    RH_ScopedOverloadedInstall(ApplyCollision, "1", 0x5435C0, bool(CPhysical::*)(CEntity*, CColPoint&, float&));
    RH_ScopedOverloadedInstall(ApplySoftCollision, "1", 0x543890, bool(CPhysical::*)(CEntity*, CColPoint&, float&));
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
CPhysical::CPhysical() : CEntity()
{
    m_pCollisionList.m_node = nullptr;

    CPlaceable::AllocateStaticMatrix();
    m_matrix->SetUnity();

    ResetMoveSpeed();
    ResetTurnSpeed();
    m_vecFrictionMoveSpeed.Reset();
    m_vecFrictionTurnSpeed.Reset();
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

    m_qAttachedEntityRotation = CQuaternion(0.0f, 0.0f, 0.0f, 0.0f);

    m_fDynamicLighting = 0.0f;
    m_pShadowData = nullptr;
    m_fPrevDistFromCam = 100.0f;

    m_nPhysicalFlags = 0;
    physicalFlags.bApplyGravity = true;

    m_nContactSurface = SURFACE_DEFAULT; // NOTSA; If not initialize, it will go out of bounds in the SurfaceInfos_c
    m_fContactSurfaceBrightness = 1.0f;
}

// 0x542450
CPhysical::~CPhysical()
{
    if (m_pShadowData)
        g_realTimeShadowMan.ReturnRealTimeShadow(m_pShadowData);

    m_pCollisionList.Flush();
}

// 0x544A30
void CPhysical::Add()
{
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
                list = &repeatSector->GetList(REPEATSECTOR_VEHICLES);
                break;
            case ENTITY_TYPE_PED:
                list = &repeatSector->GetList(REPEATSECTOR_PEDS);
                break;
            case ENTITY_TYPE_OBJECT:
                list = &repeatSector->GetList(REPEATSECTOR_OBJECTS);
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
}

// 0x5424C0
void CPhysical::Remove()
{
    if (m_bIsBIGBuilding) {
        CEntity::Remove();
        return;
    }

    CEntryInfoNode *node{}, *next{};
    for (node = m_pCollisionList.m_node; node; node = next) {
        next = node->m_next;
        node->m_doubleLinkList->DeleteNode(node->m_doubleLink);
        m_pCollisionList.DeleteNode(node);
    }
}

// 0x5449B0
CRect CPhysical::GetBoundRect()
{
    return CRect(GetBoundCentre(), GetColModel()->GetBoundRadius());
}

// 0x5485E0
void CPhysical::ProcessControl()
{
    if (!IsPed())
        physicalFlags.bSubmergedInWater = false;

    m_bHasHitWall = false;
    m_bWasPostponed = false;
    m_bIsInSafePosition = false;
    m_bHasContacted = false;

    if (m_nStatus == STATUS_SIMPLE)
        return;

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

// 0x54DFB0
void CPhysical::ProcessCollision() {
    ZoneScoped;

    auto* vehicle    = AsVehicle();
    auto* automobile = AsAutomobile();
    auto* bike       = AsBike();

    m_fMovingSpeed = 0.0f;
    physicalFlags.bProcessingShift = false;
    physicalFlags.bSkipLineCol = false;
    if (!m_bUsesCollision || physicalFlags.bDisableSimpleCollision) {
        UnstuckAndReposition();
        return;
    }

    if (m_nStatus == STATUS_SIMPLE) {
        if (CheckCollision_SimpleCar() && m_nStatus == STATUS_SIMPLE) {
            m_nStatus = STATUS_PHYSICS;
            if (IsVehicle()) {
                CCarCtrl::SwitchVehicleToRealPhysics(vehicle);
            }
        }
        UnstuckAndReposition();
        return;
    }

    if (m_nStatus == STATUS_GHOST) {
        CColPoint* wheelsColPoints = nullptr;
        float* pfWheelsSuspensionCompression = nullptr;
        CVector* wheelsCollisionPositions = nullptr;
        switch (vehicle->m_nVehicleSubType) {
        case VEHICLE_TYPE_AUTOMOBILE:
            rng::fill(automobile->m_apWheelCollisionEntity, nullptr);

            // TODO: Use std::array<> for these local variables
            wheelsColPoints = automobile->m_wheelColPoint.data();
            pfWheelsSuspensionCompression = automobile->m_fWheelsSuspensionCompression.data();
            wheelsCollisionPositions = automobile->m_vWheelCollisionPos.data();
            break;
        default:
            rng::fill(bike->m_aGroundPhysicalPtrs, nullptr);
            wheelsColPoints = bike->m_aWheelColPoints;
            pfWheelsSuspensionCompression = bike->m_aWheelRatios;
            wheelsCollisionPositions = bike->m_aGroundOffsets;
            break;
        }

        CCollisionData* cd = GetColModel()->m_pColData;
        for (int32 collisionIndex = 0;; collisionIndex++) {
            int32 colLinesCount = cd->m_nNumLines;
            if (cd->bUsesDisks)
                colLinesCount = 0;

            if (collisionIndex >= colLinesCount) {
                m_bIsStuck = false;
                m_bIsInSafePosition = true;
                m_vecMoveSpeed.Reset();
                m_matrix->ForceUpVector(CVector(0.0f, 0.0f, 1.0f));
                GetPosition().z = CCarCtrl::FindGhostRoadHeight(vehicle) + vehicle->GetHeightAboveRoad();
                ApplySpeed();
                m_matrix->Reorthogonalise();
                RemoveAndAdd();
                vehicle->vehicleFlags.bVehicleColProcessed = true;
                return;
            }

            CColPoint* wheelColPoint = &wheelsColPoints[collisionIndex];
            CColLine* colLine = &cd->m_pLines[collisionIndex];
            CVector vecColLinePosStart = m_matrix->TransformPoint(colLine->m_vecStart);
            CVector vecColLinePosEnd = m_matrix->TransformPoint(colLine->m_vecEnd);
            wheelColPoint->m_vecNormal = CVector(0.0f, 0.0f, 1.0f);
            wheelColPoint->m_nSurfaceTypeA = SURFACE_WHEELBASE;
            wheelColPoint->m_nSurfaceTypeB = SURFACE_TARMAC;
            wheelColPoint->m_fDepth = 0.0f;
            float fGhostRoadHeight = CCarCtrl::FindGhostRoadHeight(vehicle);
            if (fGhostRoadHeight <= vecColLinePosStart.z) {
                if (fGhostRoadHeight > vecColLinePosEnd.z) {
                    float fWheelSuspensionCompression = (vecColLinePosStart.z - fGhostRoadHeight) / (vecColLinePosStart.z - vecColLinePosEnd.z);
                    pfWheelsSuspensionCompression[collisionIndex] = fWheelSuspensionCompression;
                    wheelColPoint->m_vecPoint = (vecColLinePosEnd - vecColLinePosStart) * fWheelSuspensionCompression + vecColLinePosStart;
                } else {
                    pfWheelsSuspensionCompression[collisionIndex] = 1.0f;
                    wheelColPoint->m_vecPoint = vecColLinePosEnd;
                }
            } else {
                pfWheelsSuspensionCompression[collisionIndex] = 0.0f;
                wheelColPoint->m_vecPoint = vecColLinePosStart;
            }
            wheelsCollisionPositions[collisionIndex] = wheelColPoint->m_vecPoint - GetPosition();
        }
    }
    CVector vecOldMoveSpeed = m_vecMoveSpeed;
    float fOldTimeStep = CTimer::GetTimeStep();
    float fOldElasticity = m_fElasticity;
    CMatrix oldEntityMatrix(*m_matrix);
    bool bProcessCollisionBeforeSettingTimeStep = false;
    bool bUnknown = false;
    uint8 collisionSteps = SpecialEntityCalcCollisionSteps(bProcessCollisionBeforeSettingTimeStep, bUnknown);
    float fStep = fOldTimeStep / collisionSteps;
    if (bProcessCollisionBeforeSettingTimeStep)
    {
        ApplySpeed();
        m_matrix->Reorthogonalise();
        physicalFlags.bProcessingShift = false;
        physicalFlags.bSkipLineCol = false;
        physicalFlags.b17 = true;
        bool bOldUsesCollision = m_bUsesCollision;
        m_bUsesCollision = false;
        if (!CheckCollision())
        {
            physicalFlags.b17 = false;
            m_bUsesCollision = bOldUsesCollision;
            if (IsVehicle())
                vehicle->vehicleFlags.bVehicleColProcessed = true;

            physicalFlags.bProcessCollisionEvenIfStationary = false;
            physicalFlags.bSkipLineCol = false;
            m_fElasticity = fOldElasticity;
            m_fMovingSpeed = DistanceBetweenPoints(oldEntityMatrix.GetPosition(), GetPosition());
            UnstuckAndReposition();
            return;
        }
        m_bUsesCollision = bOldUsesCollision;
        physicalFlags.b17 = false;
        *static_cast<CMatrix*>(m_matrix) = oldEntityMatrix;
        m_vecMoveSpeed = vecOldMoveSpeed;
        if (IsVehicle() && vehicle->vehicleFlags.bIsLawEnforcer) {
            m_fElasticity *= HIGHSPEED_ELASTICITY_MULT_COPCAR;
        }
    }

    CPed* ped = AsPed();
    if (collisionSteps > 1u) {
        for (uint8 stepIndex = 1; stepIndex < collisionSteps; stepIndex++) {
            CTimer::UpdateTimeStep(stepIndex * fStep);
            ApplySpeed();
            bool bCheckCollision = CheckCollision();
            if (IsPed() && m_vecMoveSpeed.z == 0.0f && !ped->bWasStanding && ped->bIsStanding) {
                oldEntityMatrix.GetPosition().z = GetPosition().z;
            }
            *static_cast<CMatrix*>(m_matrix) = oldEntityMatrix;
            CTimer::UpdateTimeStep(fOldTimeStep);
            if (bCheckCollision) {
                m_fElasticity = fOldElasticity;
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
    if (!m_vecMoveSpeed.IsZero() || !m_vecTurnSpeed.IsZero()
        || physicalFlags.bProcessCollisionEvenIfStationary
        || m_nStatus == STATUS_PLAYER
        || IsVehicle() && vehicle->vehicleFlags.bIsCarParkVehicle
        || IsPed() && (ped->IsPlayer() || ped->bTestForBlockedPositions || !ped->bIsStanding))
    {
        if (IsVehicle())
            vehicle->vehicleFlags.bVehicleColProcessed = true;

        if (CheckCollision()) {
            *static_cast<CMatrix*>(m_matrix) = oldEntityMatrix;
            m_fElasticity = fOldElasticity;
            return;
        }
    }
    else if (IsPed()) {
        ped->bIsStanding = true;
    }
    physicalFlags.bProcessCollisionEvenIfStationary = false;
    physicalFlags.bSkipLineCol = false;
    m_fElasticity = fOldElasticity;
    m_fMovingSpeed = DistanceBetweenPoints(oldEntityMatrix.GetPosition(), GetPosition());
    UnstuckAndReposition();
    return;
}

// 0x54DB10
void CPhysical::ProcessShift() {
    ZoneScoped;

    CRect boundingBox = GetBoundRect();
    m_fMovingSpeed = 0.0f;

    bool bPhysicalFlagsSet = m_nPhysicalFlags & (PHYSICAL_DISABLE_MOVE_FORCE | PHYSICAL_INFINITE_MASS | PHYSICAL_DISABLE_Z);
    if (m_nStatus == STATUS_SIMPLE || bPhysicalFlagsSet)
    {
        if (bPhysicalFlagsSet)
            ResetTurnSpeed();

        UnstuckAndReposition();
    }
    else
    {
        if (m_bHasHitWall)
        {
            CPed* ped = AsPed();
            bool bSomeSpecificFlagsSet = false;
            if (IsPed() && ped->m_standingOnEntity) {
                if (!ped->m_standingOnEntity->m_bIsStatic || ped->m_standingOnEntity->m_bHasContacted) {
                    bSomeSpecificFlagsSet = true;
                }
            }
            if ((IsPed() && bSomeSpecificFlagsSet) || CWorld::bSecondShift)
            {
                const float fMoveSpeedShift = pow(PHYSICAL_SHIFT_SPEED_DAMP, CTimer::GetTimeStep());
                m_vecMoveSpeed *= fMoveSpeedShift;
                const float fTurnSpeedShift = pow(PHYSICAL_SHIFT_SPEED_DAMP, CTimer::GetTimeStep());
                m_vecTurnSpeed *= fTurnSpeedShift;
            }
        }

        CMatrix oldEntityMatrix(*m_matrix);
        ApplySpeed();
        m_matrix->Reorthogonalise();
        CWorld::IncrementCurrentScanCode();

        bool bShifted = false;
        if (IsVehicle())
            physicalFlags.bProcessingShift = true;

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
            if (bShifted2)
            {
                *(CMatrix*)m_matrix = oldEntityMatrix;
                return;
            }
        }
        m_fMovingSpeed = (GetPosition() - oldEntityMatrix.GetPosition()).Magnitude();
        UnstuckAndReposition();
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

    if (bApplySpeed)
        ApplySpeed();

    bool bCheckCollision = CheckCollision();
    m_bUsesCollision = bOldUsesCollision;
    physicalFlags.b17 = false;
    physicalFlags.bSkipLineCol = false;
    *(CMatrix*)m_matrix = entityMatrix;
    if (bTestForBlockedPositions)
        ped->bTestForBlockedPositions = true;

    return bCheckCollision;
}

// 0x546D00
int32 CPhysical::ProcessEntityCollision(CEntity* entity, CColPoint* colPoint) {
    CColModel* colModel = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel();
    int32 totalColPointsToProcess = CCollision::ProcessColModels(*m_matrix, *colModel, entity->GetMatrix(), *entity->GetColModel(), *(std::array<CColPoint, 32>*)colPoint/*should be okay for now*/, nullptr, nullptr, false);
    if (totalColPointsToProcess > 0) {
        AddCollisionRecord(entity);
        if (!entity->IsBuilding())
            entity->AsPhysical()->AddCollisionRecord(this);

        if (entity->IsBuilding() || entity->IsStatic())
            m_bHasHitWall = true;
    }
    return totalColPointsToProcess;
}

// 0x542560
void CPhysical::RemoveAndAdd()
{
    if (m_bIsBIGBuilding) {
        CEntity::Remove();
        CEntity::Add();
        return;
    }

    CEntryInfoNode* entryInfoNode = m_pCollisionList.m_node;
    CRect boundRect = GetBoundRect();
    CWorld::IterateSectorsOverlappedByRect(
        boundRect,
        [this, &entryInfoNode](int32 sectorX, int32 sectorY) {
            CPtrListDoubleLink* list = nullptr;
            CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);
            switch (m_nType) {
            case ENTITY_TYPE_VEHICLE:
                list = &repeatSector->GetList(REPEATSECTOR_VEHICLES);
                break;
            case ENTITY_TYPE_PED:
                list = &repeatSector->GetList(REPEATSECTOR_PEDS);
                break;
            case ENTITY_TYPE_OBJECT:
                list = &repeatSector->GetList(REPEATSECTOR_OBJECTS);
                break;
            }

            if (entryInfoNode) {
                CPtrNodeDoubleLink* doubleLink = entryInfoNode->m_doubleLink;
                if (entryInfoNode->m_doubleLinkList->m_node == (CPtrNode*)doubleLink)
                    entryInfoNode->m_doubleLinkList->m_node = (CPtrNode*)doubleLink->m_next;

                CPtrNodeDoubleLink* doubleLinkPrevious = doubleLink->m_prev;
                if (doubleLinkPrevious)
                    doubleLinkPrevious->m_next = doubleLink->m_next;

                CPtrNodeDoubleLink* doubleLinkNext = doubleLink->m_next;
                if (doubleLinkNext)
                    doubleLinkNext->m_prev = doubleLink->m_prev;

                doubleLink->AddToList(list);

                entryInfoNode->m_repeatSector = repeatSector;
                entryInfoNode->m_doubleLinkList = list;
                entryInfoNode = entryInfoNode->m_next;
            } else {
                auto newEntityInfoNode = new CEntryInfoNode();
                if (newEntityInfoNode) {
                    newEntityInfoNode->m_doubleLink = list->AddItem(this);
                    newEntityInfoNode->m_repeatSector = repeatSector;
                    newEntityInfoNode->m_doubleLinkList = list;
                }
                newEntityInfoNode->AddToList(m_pCollisionList.m_node);
                m_pCollisionList.m_node = newEntityInfoNode;
            }
            return true;
        }
    );

    while (entryInfoNode) {
        CEntryInfoNode* nextEntryInfoNode = entryInfoNode->m_next;
        entryInfoNode->m_doubleLinkList->DeleteNode(entryInfoNode->m_doubleLink);
        m_pCollisionList.DeleteNode(entryInfoNode);

        entryInfoNode = nextEntryInfoNode;
    }
}

// 0x542800
void CPhysical::AddToMovingList()
{
    if (!m_pMovingList && !m_bIsStaticWaitingForCollision)
    {
        auto pLink = CWorld::ms_listMovingEntityPtrs.AddItem(this);
        m_pMovingList = pLink;
    }
}

// 0x542860
void CPhysical::RemoveFromMovingList()
{
    if (m_pMovingList) {
        CWorld::ms_listMovingEntityPtrs.DeleteNode(m_pMovingList);
        m_pMovingList = nullptr;
    }
}

// 0x5428C0
void CPhysical::SetDamagedPieceRecord(float fDamageIntensity, CEntity* entity, CColPoint& colPoint, float fDistanceMult)
{
    auto* object = AsObject();
    if (fDamageIntensity > m_fDamageIntensity) {
        m_fDamageIntensity = fDamageIntensity;
        m_nPieceType = colPoint.m_nPieceTypeA;

        // CEntity::ChangeEntityReference?
        if (m_pDamageEntity)
            m_pDamageEntity->CleanUpOldReference(&m_pDamageEntity);

        m_pDamageEntity = entity;
        entity->RegisterReference(&m_pDamageEntity);

        m_vecLastCollisionPosn = colPoint.m_vecPoint;
        m_vecLastCollisionImpactVelocity = fDistanceMult * colPoint.m_vecNormal;
        if (IsObject() && colPoint.m_nSurfaceTypeB == SURFACE_CAR_MOVINGCOMPONENT)
            object->objectFlags.bDamaged = 1;
        else if (entity->IsObject() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
            entity->AsObject()->objectFlags.bDamaged = 1;
    }

    if (physicalFlags.bDisableZ) {
        if (entity->m_nModelIndex == ModelIndices::MI_POOL_CUE_BALL && IsObject()) {
            object->m_nLastWeaponDamage = object->m_nLastWeaponDamage != 255 ? WEAPON_RUNOVERBYCAR : WEAPON_DROWNING;
        }
    }
}

// 0x4ABBA0
void CPhysical::ApplyMoveForce(float x, float y, float z)
{
    return ApplyMoveForce(CVector(x, y ,z));
}

// 0x5429F0
void CPhysical::ApplyMoveForce(CVector force)
{
    if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce) {
        if (physicalFlags.bDisableZ)
            force.z = 0.0f;

        m_vecMoveSpeed += force / m_fMass;
    }
}

// 0x542A50
void CPhysical::ApplyTurnForce(CVector force, CVector point)
{
    if (!physicalFlags.bDisableTurnForce)
    {
        CVector worldCOM{};
        if (!physicalFlags.bInfiniteMass)
            worldCOM = GetMatrix().TransformVector(m_vecCentreOfMass);

        if (physicalFlags.bDisableMoveForce) {
            point.z = 0.0f;
            force.z = 0.0f;
        }
        m_vecTurnSpeed += CrossProduct(point - worldCOM, force) / m_fTurnMass;
    }
}

// 0x542B50
void CPhysical::ApplyForce(CVector vecForce, CVector point, bool bUpdateTurnSpeed)
{
    CVector vecMoveSpeedForce = vecForce;
    if (physicalFlags.bDisableZ)
        vecMoveSpeedForce.z = 0.0f;

    if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce)
        m_vecMoveSpeed += vecMoveSpeedForce / m_fMass;

    if (!physicalFlags.bDisableTurnForce && bUpdateTurnSpeed) {
        CVector worldCOM{};
        float fTurnMass = m_fTurnMass;
        if (physicalFlags.bInfiniteMass)
            fTurnMass += m_vecCentreOfMass.z * m_fMass * m_vecCentreOfMass.z * 0.5f;
        else
            worldCOM = GetMatrix().TransformVector(m_vecCentreOfMass);

        if (physicalFlags.bDisableMoveForce) {
            point.z = 0.0f;
            vecForce.z = 0.0f;
        }

        m_vecTurnSpeed += CrossProduct(point - worldCOM, vecForce) / fTurnMass;
    }
}

// 0x542CE0
CVector CPhysical::GetSpeed(CVector point)
{
    CVector worldCOM{};
    if (!physicalFlags.bInfiniteMass)
        worldCOM = GetMatrix().TransformVector(m_vecCentreOfMass);

    CVector distance = point - worldCOM;
    CVector speed = CrossProduct(m_vecTurnSpeed + m_vecFrictionTurnSpeed, distance);
    speed += m_vecMoveSpeed + m_vecFrictionMoveSpeed;
    return speed;
}

// 0x542DD0
void CPhysical::ApplyMoveSpeed()
{
    if (physicalFlags.bDontApplySpeed || physicalFlags.bDisableMoveForce)
        ResetMoveSpeed();
    else
        GetPosition() += CTimer::GetTimeStep() * m_vecMoveSpeed;
}

// 0x542E20
void CPhysical::ApplyTurnSpeed()
{
    if (physicalFlags.bDontApplySpeed) {
        ResetTurnSpeed();
    }
    else
    {
        CVector vecTurnSpeedTimeStep = CTimer::GetTimeStep() * m_vecTurnSpeed;
        CVector vecCrossProduct{};
        vecCrossProduct = vecTurnSpeedTimeStep.Cross(GetRight());
        GetRight() += vecCrossProduct;
        vecCrossProduct = vecTurnSpeedTimeStep.Cross(GetForward());
        GetForward() += vecCrossProduct;
        vecCrossProduct = vecTurnSpeedTimeStep.Cross(GetUp());
        GetUp() += vecCrossProduct;
        if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce) {
            CVector worldCOM = GetMatrix().TransformVector(-m_vecCentreOfMass);
            GetPosition() += vecTurnSpeedTimeStep.Cross(worldCOM);
        }
    }
}

// 0x542FE0
void CPhysical::ApplyGravity()
{
    if (physicalFlags.bApplyGravity && !physicalFlags.bDisableMoveForce) {
        if (physicalFlags.bInfiniteMass) {
            float fMassTimeStep = CTimer::GetTimeStep() * m_fMass;
            CVector point = GetMatrix().TransformVector(m_vecCentreOfMass);
            CVector force (0.0f, 0.0f, fMassTimeStep * -0.008f);
            ApplyForce(force, point, true);
        }
        else if (m_bUsesCollision) {
            m_vecMoveSpeed.z -= CTimer::GetTimeStep() * 0.008f;
        }
    }
}

// 0x5430A0
void CPhysical::ApplyFrictionMoveForce(CVector moveForce)
{
    if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce) {
        if (physicalFlags.bDisableZ)
            moveForce.z = 0.0f;

        m_vecFrictionMoveSpeed += moveForce / m_fMass;
    }
}

// Unused
// 0x543100
void CPhysical::ApplyFrictionTurnForce(CVector posn, CVector velocity)
{
    ((void(__thiscall*)(CPhysical*, CVector, CVector))0x543100)(this, posn, velocity);
}

// 0x543220
void CPhysical::ApplyFrictionForce(CVector vecMoveForce, CVector point)
{
    CVector vecTheMoveForce = vecMoveForce;

    if (physicalFlags.bDisableZ)
        vecTheMoveForce.z = 0.0f;

    if (!physicalFlags.bInfiniteMass && !physicalFlags.bDisableMoveForce)
        m_vecFrictionMoveSpeed += vecTheMoveForce / m_fMass;

    CVector worldCOM{};
    if (!physicalFlags.bDisableTurnForce)
    {
        float fTurnMass = m_fTurnMass;
        if (physicalFlags.bInfiniteMass)
            fTurnMass += m_vecCentreOfMass.z * m_fMass * m_vecCentreOfMass.z * 0.5f;
        else
            worldCOM = GetMatrix().TransformVector(m_vecCentreOfMass);

        if (physicalFlags.bDisableMoveForce)
        {
            point.z = 0.0f;
            vecMoveForce.z = 0.0f;
        }
        CVector vecMoveForceCrossProduct = CrossProduct(point - worldCOM, vecMoveForce);
        m_vecFrictionTurnSpeed += vecMoveForceCrossProduct / fTurnMass;
    }
}

// 0x5433B0
void CPhysical::SkipPhysics()
{
    if (!IsPed() && !IsVehicle())
        physicalFlags.bSubmergedInWater = false;

    m_bHasHitWall = false;
    m_bWasPostponed = false;
    m_bIsInSafePosition = false;
    m_bHasContacted = false;

    if (m_nStatus != STATUS_SIMPLE)
    {
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
void CPhysical::AddCollisionRecord(CEntity* collidedEntity)
{
    physicalFlags.bOnSolidSurface = true;
    m_nLastCollisionTime = CTimer::GetTimeInMS();
    if (IsVehicle())
    {
        auto* vehicle = AsVehicle();
        if (collidedEntity->IsVehicle())
        {
            auto* collidedVehicle = collidedEntity->AsVehicle();
            if (vehicle->m_nAlarmState == -1)
                vehicle->m_nAlarmState = 15000;

            if (collidedVehicle->m_nAlarmState == -1)
                collidedVehicle->m_nAlarmState = 15000;
        }
    }

    if (physicalFlags.bCanBeCollidedWith)
    {
        for (uint32 i = 0; i < m_nNumEntitiesCollided; i++) {
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
bool CPhysical::GetHasCollidedWith(CEntity* entity)
{
    if (!physicalFlags.bCanBeCollidedWith || m_nNumEntitiesCollided <= 0)
        return false;

    for (uint32 i = 0; i < m_nNumEntitiesCollided; i++) {
        if (m_apCollidedEntities[i] == entity) {
            return true;
        }
    }

    return false;
}

// 0x543580
bool CPhysical::GetHasCollidedWithAnyObject()
{
    if (!physicalFlags.bCanBeCollidedWith || m_nNumEntitiesCollided <= 0)
        return false;

    for (uint32 i = 0; i < m_nNumEntitiesCollided; i++)
    {
        CEntity* pCollidedEntity = m_apCollidedEntities[i];
        if (pCollidedEntity && pCollidedEntity->IsObject())
            return true;
    }
    return false;
}

// 0x5435C0
bool CPhysical::ApplyCollision(CEntity* entity, CColPoint& colPoint, float& damageIntensity)
{
    if (physicalFlags.bDisableTurnForce) {
        float fSpeedDotProduct = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
        if (fSpeedDotProduct < 0.0f) {
            damageIntensity = -(fSpeedDotProduct * m_fMass);
            CVector vecMoveSpeed = damageIntensity * colPoint.m_vecNormal;
            ApplyMoveForce(vecMoveSpeed.x, vecMoveSpeed.y, vecMoveSpeed.z);
            ReportCollision(this, entity, true, colPoint, damageIntensity / m_fMass);
            return true;
        }
    } else {
        CVector vecDistanceToPoint = colPoint.m_vecPoint - GetPosition();
        CVector vecSpeed = GetSpeed(vecDistanceToPoint);

        CVector vecMoveDirection = colPoint.m_vecNormal;
        float fSpeedDotProduct = vecMoveDirection.Dot(vecSpeed);
        if (fSpeedDotProduct < 0.0f) {
            CVector worldCOM = GetMatrix().TransformVector(m_vecCentreOfMass);
            float fCollisionMass = GetTurnTorque(vecDistanceToPoint - worldCOM, vecMoveDirection);

            damageIntensity = -((m_fElasticity + 1.0f) * fCollisionMass * fSpeedDotProduct);

            CVector vecMoveSpeed = vecMoveDirection * damageIntensity;
            if (IsVehicle() && vecMoveDirection.z < 0.7f)
                vecMoveSpeed.z *= 0.3f;

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
bool CPhysical::ApplySoftCollision(CEntity* entity, CColPoint& colPoint, float& outDamageIntensity)
{
    if (physicalFlags.bDisableTurnForce)
        ApplyCollision(entity, colPoint, outDamageIntensity);

    CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
    CVector vecSpeed = GetSpeed(vecDistanceToPointFromThis);

    CVector vecMoveDirection = colPoint.m_vecNormal;

    float fSoftColSpeedMult = SOFTCOL_SPEED_MULT;

    CVehicle* vehicle = AsVehicle();
    if (IsVehicle() && vehicle->IsSubMonsterTruck()) {
        float fForwardsDotProduct = vecMoveDirection.Dot(GetUp());
        if (fForwardsDotProduct < -0.9f)
            return false;

        if (fForwardsDotProduct < 0.0f) {
            vecMoveDirection -= fForwardsDotProduct * GetUp();
            vecMoveDirection.Normalise();
        } else if (fForwardsDotProduct > 0.5f) {
            fSoftColSpeedMult = SOFTCOL_SPEED_MULT2;
        }
    }

    float fSpeedDotProduct = vecSpeed.Dot(vecMoveDirection);
    CVector worldCOM = GetMatrix().TransformVector(m_vecCentreOfMass);

    if (physicalFlags.bInfiniteMass)
        worldCOM = CVector(0.0f, 0.0f, 0.0f);

    float fCollisionMass = GetTurnTorque(vecDistanceToPointFromThis - worldCOM, vecMoveDirection);

    if (!IsVehicle() || vehicle->m_nVehicleSubType // todo: m_nVehicleSubType
        || colPoint.m_nPieceTypeA < 13u || colPoint.m_nPieceTypeA > 16u)
    {
        float fDepth = SOFTCOL_DEPTH_MIN;
        if (SOFTCOL_DEPTH_MIN >= colPoint.m_fDepth)
            fDepth = colPoint.m_fDepth;

        outDamageIntensity = fDepth * CTimer::GetTimeStep() * SOFTCOL_DEPTH_MULT * fCollisionMass * 0.008f;
        if (fSpeedDotProduct >= 0.0f)
        {
            outDamageIntensity = 0.0f;
            return false;
        }
        outDamageIntensity -= fCollisionMass * fSpeedDotProduct * fSoftColSpeedMult;
    }
    else
    {
        outDamageIntensity = colPoint.m_fDepth * CTimer::GetTimeStep() * SOFTCOL_DEPTH_MULT * fCollisionMass * 0.008f;
        if (fSpeedDotProduct < 0.0f)
            outDamageIntensity -= SOFTCOL_CARLINE_SPEED_MULT * fCollisionMass * fSpeedDotProduct;

        float fRightDotProduct = vecMoveDirection.Dot(GetRight());
        vecMoveDirection -= 0.9f * fRightDotProduct * GetRight();
    }

    if (outDamageIntensity == 0.0f)
        return false;

    ApplyForce(vecMoveDirection * outDamageIntensity, vecDistanceToPointFromThis, true);
    if (outDamageIntensity < 0.0f)
        outDamageIntensity *= -1.0f;

    return true;
}

// 0x543C90
bool CPhysical::ApplySpringCollision(float fSuspensionForceLevel, CVector& direction, CVector& collisionPoint, float fSpringLength, float fSuspensionBias, float& fSpringForceDampingLimit) {
    float fSpringStress = 1.0f - fSpringLength;
    if (fSpringStress <= 0.0f)
        return true;

    float fTimeStep = CTimer::GetTimeStep();
    if (CTimer::GetTimeStep() >= 3.0f)
        fTimeStep = 3.0f;

    fSpringForceDampingLimit = fSpringStress * m_fMass * fSuspensionForceLevel * 0.016f * fTimeStep * fSuspensionBias;
    ApplyForce(-fSpringForceDampingLimit * direction, collisionPoint, true);
    return true;
}

// 0x543D60
bool CPhysical::ApplySpringCollisionAlt(float fSuspensionForceLevel, CVector& direction, CVector& collisionPoint, float fSpringLength, float fSuspensionBias, CVector& normal, float& fSpringForceDampingLimit) {
    float fSpringStress = 1.0f - fSpringLength;
    if (fSpringStress <= 0.0f)
        return true;

    if (direction.Dot(normal) > 0.0f)
        normal *= -1.0f;

    float fTimeStep = CTimer::GetTimeStep();
    if (CTimer::GetTimeStep() >= 3.0f)
        fTimeStep = 3.0f;

    fSpringForceDampingLimit = fSpringStress * (fTimeStep * m_fMass) * fSuspensionForceLevel * fSuspensionBias * 0.016f;
    if (physicalFlags.bMakeMassTwiceAsBig)
        fSpringForceDampingLimit *= 0.75f;

    ApplyForce(fSpringForceDampingLimit * normal, collisionPoint, true);
    return true;
}

// 0x543E90
bool CPhysical::ApplySpringDampening(float fDampingForce, float fSpringForceDampingLimit, CVector& direction, CVector& collisionPoint, CVector& collisionPos) {
    float fCollisionPosDotProduct = collisionPos.Dot(direction);
    CVector vecCollisionPointSpeed = GetSpeed(collisionPoint);
    float fCollisionPointSpeedDotProduct = vecCollisionPointSpeed.Dot(direction);
    float fTimeStep = CTimer::GetTimeStep();
    if (CTimer::GetTimeStep() >= 3.0f)
        fTimeStep = 3.0f;

    float fDampingForceTimeStep = fTimeStep * fDampingForce;
    if (physicalFlags.bMakeMassTwiceAsBig)
        fDampingForceTimeStep *= 2.0f;

    fDampingForceTimeStep = std::clamp(fDampingForceTimeStep, -DAMPING_LIMIT_IN_FRAME, DAMPING_LIMIT_IN_FRAME);
    float fDampingSpeed = -(fDampingForceTimeStep * fCollisionPosDotProduct);
    if (fDampingSpeed > 0.0f && fDampingSpeed + fCollisionPointSpeedDotProduct > 0.0f) {
        if (fCollisionPointSpeedDotProduct >= 0.0f)
            fDampingSpeed = 0.0f;
        else
            fDampingSpeed = -fCollisionPointSpeedDotProduct;
    }
    else if (fDampingSpeed < 0.0f && fDampingSpeed + fCollisionPointSpeedDotProduct < 0.0f) {
        if (fCollisionPointSpeedDotProduct <= 0.0f)
            fDampingSpeed = 0.0f;
        else
            fDampingSpeed = -fCollisionPointSpeedDotProduct;
    }

    CVector center = GetMatrix().TransformVector(m_vecCentreOfMass);
    CVector distance = collisionPoint - center;
    float fSpringForceDamping = GetTurnTorque(distance, direction) * fDampingSpeed;
    fSpringForceDampingLimit = fabs(fSpringForceDampingLimit) * DAMPING_LIMIT_OF_SPRING_FORCE;
    fSpringForceDamping = std::min(fSpringForceDamping, fSpringForceDampingLimit);
    ApplyForce(fSpringForceDamping * direction, collisionPoint, true);
    return true;
}

// Unused
bool CPhysical::ApplySpringDampeningOld(float arg0, float arg1, CVector& arg2, CVector& arg3, CVector& arg4)
{
    return ((bool(__thiscall*)(CPhysical*, float, float, CVector&, CVector&, CVector&))0x544100)(this, arg0, arg1, arg2, arg3, arg4);
}

// 0x544280
void CPhysical::RemoveRefsToEntity(CEntity* entity)
{
    for (uint8 collidedEntityIndex = 0; collidedEntityIndex < m_nNumEntitiesCollided;) {
        if (m_apCollidedEntities[collidedEntityIndex] == entity) {
            for (uint16 i = collidedEntityIndex; i < m_nNumEntitiesCollided - 1; ++i) {
                m_apCollidedEntities[i] = m_apCollidedEntities[i + 1];
            }
            m_nNumEntitiesCollided--;
        }
        else {
            collidedEntityIndex++;
        }
    }
}

// 0x5442F0
void CPhysical::DettachEntityFromEntity(float x, float y, float z, bool bApplyTurnForce)
{
    CMatrix vecDetachOffsetMatrix{};
    if (IsVehicle() && m_pAttachedTo && m_pAttachedTo->IsVehicle()) {
        CColModel* colModel = CEntity::GetColModel();
        CCollisionData* colData = colModel->m_pColData;
        if (colData) {
            uint8 nNumLines = colData->m_nNumLines;
            colData->m_nNumLines = 0;
            CColModel* attachedToColModel = m_pAttachedTo->GetColModel();
            CMatrix& attachedToMatrix = m_pAttachedTo->GetMatrix();
            if (CCollision::ProcessColModels(*m_matrix, *colModel, attachedToMatrix, *attachedToColModel, CWorld::m_aTempColPts, nullptr, nullptr, false))
                m_pEntityIgnoredCollision = m_pAttachedTo;
            else if (m_pEntityIgnoredCollision == m_pAttachedTo)
                m_pEntityIgnoredCollision = nullptr;

            colData->m_nNumLines = nNumLines;
        }
        else if (m_pEntityIgnoredCollision == m_pAttachedTo) {
            m_pEntityIgnoredCollision = nullptr;
        }
    }
    else {
        m_pEntityIgnoredCollision = m_pAttachedTo;
    }

    vecDetachOffsetMatrix.ResetOrientation();
    vecDetachOffsetMatrix.RotateZ(y);
    vecDetachOffsetMatrix.RotateX(x);

    vecDetachOffsetMatrix *= m_pAttachedTo->GetMatrix();
    CVector vecForce = vecDetachOffsetMatrix.GetForward() * z;
    CWorld::Remove(this);
    SetIsStatic(false);
    physicalFlags.bAttachedToEntity = false;
    CWorld::Add(this);
    if (physicalFlags.bDisableCollisionForce) {
        CObjectData* pThisObjectInfo = AsObject()->m_pObjectInfo;
        if (!IsObject() || pThisObjectInfo->m_fMass >= 99998.0f)
        {
            physicalFlags.bCollidable = true;
            ResetTurnSpeed();
            ResetMoveSpeed();
            bApplyTurnForce = false;
        }
        else
        {
            physicalFlags.bDisableCollisionForce = false;
            m_fMass = pThisObjectInfo->m_fMass;
            m_fTurnMass = pThisObjectInfo->m_fTurnMass;
        }
    }

    if (!physicalFlags.bDisableCollisionForce) {
        if (m_pAttachedTo->IsPhysical()) {
            m_vecMoveSpeed = m_pAttachedTo->m_vecMoveSpeed;
            m_vecMoveSpeed += vecForce;
        }
    }

    if (bApplyTurnForce)
        ApplyTurnForce(vecForce, vecForce * 0.5f);

    m_pAttachedTo = nullptr;
    m_qAttachedEntityRotation = CQuaternion(0.0f, 0.0f, 0.0f, 0.0f);
    m_vecAttachOffset = CVector(0.0f, 0.0f, 0.0f);
}

// 0x5446A0
void CPhysical::DettachAutoAttachedEntity()
{
    SetIsStatic(false);
    physicalFlags.bAttachedToEntity = false;
    m_nFakePhysics = 0;
    if (!physicalFlags.bDisableCollisionForce) {
        if (m_pAttachedTo->IsVehicle()) {
            m_vecMoveSpeed = m_pAttachedTo->m_vecMoveSpeed;
            m_vecTurnSpeed = m_pAttachedTo->m_vecTurnSpeed;
        }
    }
    else
    {
        physicalFlags.bCollidable = true;
        ResetTurnSpeed();
        ResetMoveSpeed();
    }
    m_vecAttachOffset = CVector(0.0f, 0.0f, 0.0f);
    m_pEntityIgnoredCollision = nullptr;
    m_pAttachedTo = nullptr;
    m_qAttachedEntityRotation = CQuaternion(0.0f, 0.0f, 0.0f, 0.0f);
    if (IsObject()) {
        m_fElasticity = AsObject()->m_pObjectInfo->m_fElasticity;
    }
}

// 0x5447B0
float CPhysical::GetLightingFromCol(bool bInteriorLighting)
{
    float fAmbientRedBlue = CTimeCycle::GetAmbientRed_BeforeBrightness() + CTimeCycle::GetAmbientBlue_BeforeBrightness();
    float fLighting = (CTimeCycle::GetAmbientGreen_BeforeBrightness() + fAmbientRedBlue) * (1.0f / 3.0f) + m_fContactSurfaceBrightness;
    if (!bInteriorLighting) {
        fLighting *= (CTimeCycle::SumOfCurrentRGB1() * (1.0f / 765.0f) * TEST_ADD_AMBIENT_LIGHT_FRAC + 1.0f - TEST_ADD_AMBIENT_LIGHT_FRAC)
                    + CTimeCycle::SumOfCurrentRGB2() * (1.0f / 765.0f) * TEST_ADD_AMBIENT_LIGHT_FRAC;
    }
    return fLighting;
}

// 0x544850
float CPhysical::GetLightingTotal()
{
    bool bInteriorLighting = false;
    if (m_nAreaCode)
        bInteriorLighting = true;

    if (IsPed()) {
        CPed* ped = AsPed();
        if (ped->m_pPlayerData && (CGame::currArea || ped->m_pPlayerData->m_bForceInteriorLighting))
            bInteriorLighting = true;
    }
    return GetLightingFromCol(bInteriorLighting) + m_fDynamicLighting;
}

// 0x5448B0
bool CPhysical::CanPhysicalBeDamaged(eWeaponType weapon, bool* bDamagedDueToFireOrExplosionOrBullet)
{
    if (bDamagedDueToFireOrExplosionOrBullet)
        *bDamagedDueToFireOrExplosionOrBullet = false;

    switch (weapon)
    {
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
        if (!physicalFlags.bMeleeProof)
            return true;
        return false;
    case WEAPON_GRENADE:
    case WEAPON_ROCKET:
    case WEAPON_ROCKET_HS:
    case WEAPON_FREEFALL_BOMB:
    case WEAPON_RLAUNCHER:
    case WEAPON_RLAUNCHER_HS:
    case WEAPON_REMOTE_SATCHEL_CHARGE:
    case WEAPON_DETONATOR:
    case WEAPON_ARMOUR | WEAPON_NIGHTSTICK:
        if (!physicalFlags.bExplosionProof)
            break;
        return false;
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
        return true;
    case WEAPON_MOLOTOV:
        if (!physicalFlags.bFireProof)
            break;
        return false;
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
        if (physicalFlags.bBulletProof)
            return false;
        break;
    case WEAPON_FLAMETHROWER:
        if (!physicalFlags.bFireProof)
            return true;
        return false;
    case WEAPON_ARMOUR | WEAPON_BRASSKNUCKLE:
    case WEAPON_ARMOUR | WEAPON_GOLFCLUB:
    case WEAPON_ARMOUR | WEAPON_SHOVEL:
        if (physicalFlags.bCollisionProof)
            return false;
        return true;
    }

    if (!bDamagedDueToFireOrExplosionOrBullet)
        return true;

    *bDamagedDueToFireOrExplosionOrBullet = true;
    return true;
}

// 0x544C40
void CPhysical::ApplyAirResistance()
{
    if (m_fAirResistance <= 0.1f || IsVehicle())
    {
        float fSpeedMagnitude = m_vecMoveSpeed.Magnitude() * m_fAirResistance;
        if (CCullZones::DoExtraAirResistanceForPlayer())
        {
            if (IsVehicle())
            {
                CVehicle* vehicle = AsVehicle();
                if (vehicle->IsAutomobile() || vehicle->IsSubBike())
                    fSpeedMagnitude = CVehicle::m_fAirResistanceMult * fSpeedMagnitude;
            }
        }

        m_vecMoveSpeed *= pow(1.0f - fSpeedMagnitude, CTimer::GetTimeStep());
        m_vecTurnSpeed *= 0.99f;
    }
    else
    {
        float fAirResistanceTimeStep = pow(m_fAirResistance, CTimer::GetTimeStep());
        m_vecMoveSpeed *= fAirResistanceTimeStep;
        m_vecTurnSpeed *= fAirResistanceTimeStep;
    }
}

// 0x544D50
bool CPhysical::ApplyCollisionAlt(CPhysical* entity, CColPoint& colPoint, float& damageIntensity, CVector& outVecMoveSpeed, CVector& outVecTurnSpeed)
{
    if (m_pAttachedTo)
    {
        if (m_pAttachedTo->IsPhysical() && !m_pAttachedTo->IsPed())
        {
            float fDamageIntensity = 0.0f;
            m_pAttachedTo->ApplySoftCollision(entity, colPoint, fDamageIntensity);
        }
    }

    if (physicalFlags.bDisableTurnForce)
    {
        float fSpeedDotProduct = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
        if (fSpeedDotProduct < 0.0f)
        {
            damageIntensity = -(fSpeedDotProduct * m_fMass);
            ApplyMoveForce(colPoint.m_vecNormal * damageIntensity);
            ReportCollision(this, entity, true, colPoint, damageIntensity / m_fMass);
            return true;
        }
        return false;
    }

    auto* vehicle = AsVehicle();
    CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
    CVector vecSpeed = GetSpeed(vecDistanceToPointFromThis);

    if (physicalFlags.bAddMovingCollisionSpeed && IsVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
        vecSpeed += vehicle->AddMovingCollisionSpeed(vecDistanceToPointFromThis);

    CVector vecMoveDirection = colPoint.m_vecNormal;
    float fSpeedDotProduct = vecMoveDirection.Dot(vecSpeed);
    if (fSpeedDotProduct >= 0.0f)
        return false;

    CVector worldCOM = GetMatrix().TransformVector(m_vecCentreOfMass);
    if (physicalFlags.bInfiniteMass)
        worldCOM = CVector(0.0f, 0.0f, 0.0f);

    CVector vecDifference = vecDistanceToPointFromThis - worldCOM;
    float fCollisionMass = GetTurnTorque(vecDifference, vecMoveDirection);

    uint16 entityAltCol = ALT_ENITY_COL_DEFAULT;
    float fMoveSpeedLimit = CTimer::GetTimeStep() * 0.008f;
    if (IsObject())
    {
        entityAltCol = ALT_ENITY_COL_OBJECT;
        fMoveSpeedLimit *= 1.3f;
    }
    else
    {
        if (IsVehicle() && !physicalFlags.bSubmergedInWater) {
            float fMoveSpeedLimitMultiplier = 0.0f;
            if (!vehicle->IsBike() || (m_nStatus != STATUS_ABANDONED) && m_nStatus != STATUS_WRECKED)
            {
                if (vehicle->IsBoat())
                {
                    fMoveSpeedLimitMultiplier = 1.5f;
                    entityAltCol = ALT_ENITY_COL_BOAT;
                }
                else if (GetUp().z < -0.3f) {
                    fMoveSpeedLimitMultiplier = 1.4f;
                    entityAltCol = ALT_ENITY_COL_VEHICLE;
                }
            }
            else
            {
                fMoveSpeedLimitMultiplier = 1.7f;
                entityAltCol = ALT_ENITY_COL_BIKE_WRECKED;
            }
            fMoveSpeedLimit *= fMoveSpeedLimitMultiplier;
        }
    }

    // 0x5450A1
    float fCollisionImpact2 = 1.0f;
    bool bUseElasticity = false;
    const bool isLowSpeed = fabs(m_vecMoveSpeed.x) < fMoveSpeedLimit
        && fabs(m_vecMoveSpeed.y) < fMoveSpeedLimit
        && fabs(m_vecMoveSpeed.z) < fMoveSpeedLimit * 2.f;

    switch (entityAltCol) {
    case ALT_ENITY_COL_OBJECT:
        if (!m_bHasContacted && isLowSpeed) {
            fCollisionImpact2 = 0.0f;
            damageIntensity = -0.98f * fCollisionMass * fSpeedDotProduct;
        }
        bUseElasticity = true;
        break;
    case ALT_ENITY_COL_VEHICLE:
    case ALT_ENITY_COL_BIKE_WRECKED:
    case ALT_ENITY_COL_BOAT:
        if (isLowSpeed) {
            fCollisionImpact2 = 0.0f;
            damageIntensity = -0.95f * fCollisionMass * fSpeedDotProduct;
        } else {
            bUseElasticity = true;
        }
        break;
    default:
        bUseElasticity = true;
        break;
    }

    if (bUseElasticity)
    {
        float fElasticity = m_fElasticity * 2.f;
        if (!IsVehicle() || !vehicle->IsBoat()
            || colPoint.m_nSurfaceTypeB != SURFACE_WOOD_SOLID
            && vecMoveDirection.z >= 0.5f)
        {
            fElasticity = m_fElasticity;
        }

        damageIntensity = -((fElasticity + 1.0f) * fCollisionMass * fSpeedDotProduct);
    }

    CVector momentum = vecMoveDirection * damageIntensity;

    if (physicalFlags.bDisableZ || physicalFlags.bInfiniteMass || physicalFlags.bDisableMoveForce) {
        ApplyForce(momentum, vecDistanceToPointFromThis, true);
    } else {
        const auto velocity = momentum / m_fMass;
        if (IsVehicle()) {
            if (!m_bHasHitWall || m_vecMoveSpeed.SquaredMagnitude() <= 0.1f
                && (entity->IsBuilding() || entity->physicalFlags.bDisableCollisionForce))
            {
                outVecMoveSpeed += velocity * 1.2f;
            } else {
                outVecMoveSpeed += velocity;
            }
            momentum *= 0.8f;
        } else {
            outVecMoveSpeed += velocity;
        }

        worldCOM = GetMatrix().TransformVector(m_vecCentreOfMass);
        float fTurnMass = m_fTurnMass;
        CVector vecDifference = vecDistanceToPointFromThis - worldCOM; // todo: shadow var
        outVecTurnSpeed += vecDifference.Cross(momentum) / fTurnMass;
    }
    ReportCollision(this, entity, true, colPoint, damageIntensity / fCollisionMass, fCollisionImpact2);
    return true;
}

// 0x5454C0
bool CPhysical::ApplyFriction(float fFriction, CColPoint& colPoint)
{
    if (physicalFlags.bDisableCollisionForce)
        return false;

    if (physicalFlags.bDisableTurnForce)
    {
        float fMoveSpeedDotProduct = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
        CVector vecSpeedDifference = m_vecMoveSpeed - (fMoveSpeedDotProduct * colPoint.m_vecNormal);
        float fMoveSpeedMagnitude = vecSpeedDifference.Magnitude();
        if (fMoveSpeedMagnitude > 0.0f)
        {
            CVector vecMoveDirection = vecSpeedDifference / fMoveSpeedMagnitude;

            float fSpeed = -fMoveSpeedMagnitude;
            float fForce = -(CTimer::GetTimeStep() / m_fMass * fFriction);
            fSpeed = std::max(fSpeed, fForce);

            m_vecFrictionMoveSpeed.x += vecMoveDirection.x * fSpeed;
            m_vecFrictionMoveSpeed.y += vecMoveDirection.y * fSpeed;
            return true;
        }
        return false;
    }

    CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
    CVector vecSpeed = GetSpeed(vecDistanceToPointFromThis);

    float fMoveSpeedDotProduct = vecSpeed.Dot(colPoint.m_vecNormal);
    CVector vecSpeedDifference = vecSpeed - (fMoveSpeedDotProduct * colPoint.m_vecNormal);

    float fMoveSpeedMagnitude = vecSpeedDifference.Magnitude();
    if (fMoveSpeedMagnitude <= 0.0f)
        return false;

    CVector vecMoveDirection = vecSpeedDifference / fMoveSpeedMagnitude;

    CVector worldCOM = GetMatrix().TransformVector(m_vecCentreOfMass);
    CVector vecDifference = vecDistanceToPointFromThis - worldCOM;
    float fCollisionMass = -GetTurnTorque(vecDifference, vecMoveDirection) * fMoveSpeedMagnitude;
    fCollisionMass = std::max(fCollisionMass, -fFriction);

    ApplyFrictionForce(vecMoveDirection * fCollisionMass, vecDistanceToPointFromThis);

    auto* vehicle = AsVehicle();
    if (fMoveSpeedMagnitude > 0.1f
        && g_surfaceInfos.GetFrictionEffect(colPoint.m_nSurfaceTypeB)
        && (g_surfaceInfos.GetFrictionEffect(colPoint.m_nSurfaceTypeA) == FRICTION_EFFECT_SPARKS || IsVehicle())
        && (!IsVehicle() || !vehicle->IsSubBMX() || !vehicle->m_pDriver
            || fabs(colPoint.m_vecNormal.Dot(GetRight())) >= 0.87f))
    {

        CVector across = vecMoveDirection * (fMoveSpeedMagnitude * 0.25f);
        CVector direction = vecMoveDirection + (colPoint.m_vecNormal * 0.1f);
        CVector vecSpeedCrossProduct = colPoint.m_vecNormal.Cross(m_vecMoveSpeed); // todo: shadow var
        vecSpeedCrossProduct.Normalise();

        for (int32 i = 0; i < 8; i++)
        {
            float fRandom = CGeneral::GetRandomNumberInRange(-0.2f, 0.2f);
            CVector origin = colPoint.m_vecPoint + (vecSpeedCrossProduct * fRandom);
            float force = fMoveSpeedMagnitude * 12.5f;
            g_fx.AddSparks(origin, direction, force, 1, across, SPARK_PARTICLE_SPARK2, 0.1f, 1.0f);
        }
    }
    return true;
}

// 0x545980
bool CPhysical::ApplyFriction(CPhysical* entity, float fFriction, CColPoint& colPoint) {
    const auto DoApplyFriction = [&](bool isThisVehicle, bool isEntityVehicle) {
        const auto GetMoveForce = [&colPoint](CPhysical* p, bool isVehicle) {
            const auto velocity = isVehicle
                ? p->GetSpeed(colPoint.m_vecPoint - p->GetPosition())
                : p->m_vecMoveSpeed;
            return velocity - velocity.ProjectOnToNormal(colPoint.m_vecNormal);
        };

        const auto moveForceA    = GetMoveForce(this, isThisVehicle);
        const auto moveForceMagA = moveForceA.Magnitude();

        const auto moveForceB = GetMoveForce(entity, isEntityVehicle);
        const auto moveForceMagB = moveForceB.Magnitude();

        const auto GetMoveTorque = [&colPoint](CPhysical* p, bool isVehicle, const CVector& turnForce) {
            return isVehicle
                ? p->GetTurnTorque(colPoint.m_vecPoint - p->GetMatrix().TransformVector(p->m_vecCentreOfMass), turnForce)
                : p->m_fMass;
        };
        const auto turnTorqueA = GetMoveTorque(this, isThisVehicle, moveForceA);
        const auto turnTorqueB = GetMoveTorque(entity, isEntityVehicle, moveForceB);

        const auto avgTurnTorque = (turnTorqueA * moveForceMagA + turnTorqueB * moveForceMagB) / (turnTorqueA + turnTorqueB);
        if (moveForceMagA <= avgTurnTorque) {
            return false;
        }

        auto speed1 = turnTorqueA * (avgTurnTorque - moveForceMagA);
        auto speed2 = turnTorqueB * (avgTurnTorque - moveForceMagB);

        const auto frictionStep = CTimer::GetTimeStep() * fFriction;
        speed1 = std::max(speed1, -frictionStep);
        speed2 = std::min(speed2, frictionStep);

        CVector moveDirection = moveForceA / moveForceMagA;
        auto ApplyFrictionToObject = [&](CPhysical* obj, float speed) {
            if (!obj->physicalFlags.bDisableCollisionForce) {
                obj->ApplyFrictionForce(moveDirection * speed, colPoint.m_vecPoint - obj->GetPosition());
            }
        };

        ApplyFrictionToObject(this, speed1);
        ApplyFrictionToObject(entity, speed2);
        return true;
    };

    if (physicalFlags.bDisableTurnForce && entity->physicalFlags.bDisableTurnForce)
        return DoApplyFriction(false, false);

    if (physicalFlags.bDisableTurnForce) {
        if (entity->IsVehicle())
            return false;

        return DoApplyFriction(false, true);
    }
    if (!entity->physicalFlags.bDisableTurnForce)
        return DoApplyFriction(true, true);

    if (IsVehicle())
        return false;

    return DoApplyFriction(true, false);
}

// 0x546670
bool CPhysical::ProcessShiftSectorList(int32 sectorX, int32 sectorY)
{
    CBaseModelInfo* mi = CModelInfo::GetModelInfo(m_nModelIndex);
    float fBoundingSphereRadius = mi->GetColModel()->GetBoundRadius();
    float fMaxColPointDepth = 0.0f;
    CVector vecShift{};
    CColPoint colPoints[32];
    CVector vecBoundCentre{};

    GetBoundCentre(&vecBoundCentre);

    CSector* sector = GetSector(sectorX, sectorY);
    CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);

    int32 totalAcceptableColPoints = 0;
    int32 scanListIndex = 4;
    do
    {
        CPtrListDoubleLink* list = nullptr;
        switch (--scanListIndex)
        {
        case 0:
            list = &sector->m_buildings;
            break;
        case 1:
            list = &repeatSector->GetList(REPEATSECTOR_VEHICLES);
            break;
        case 2:
            list = &repeatSector->GetList(REPEATSECTOR_PEDS);
            break;
        case 3:
            list = &repeatSector->GetList(REPEATSECTOR_OBJECTS);
            break;
        }
        CPtrNodeDoubleLink* node = list->GetNode();
        if (list->GetNode())
        {
            do
            {
                auto* entity = reinterpret_cast<CPhysical*>(node->m_item);
                node = node->m_next;

                bool bProcessEntityCollision = true;
                if (!entity->IsBuilding() && (!entity->IsObject()
                    || !entity->physicalFlags.bDisableCollisionForce))
                {
                    if (!IsPed() || !entity->IsObject() || !entity->IsStatic()
                        || entity->AsObject()->objectFlags.bIsExploded)
                    {
                        bProcessEntityCollision = false;
                    }
                }
                if (entity != this
                    && !entity->IsScanCodeCurrent()
                    && entity->m_bUsesCollision && (!m_bHasHitWall || bProcessEntityCollision))
                {
                    if (entity->GetIsTouching(vecBoundCentre, fBoundingSphereRadius))
                    {
                        bool bCollisionDisabled = false;
                        bool bCollidedEntityCollisionIgnored = false;
                        bool bCollidedEntityUnableToMove = false;
                        bool bThisOrCollidedEntityStuck = false;
                        if (entity->IsBuilding()) {
                            if (physicalFlags.bDisableCollisionForce
                                && (!IsVehicle() || entity->AsVehicle()->IsSubTrain()))
                            {
                                bCollisionDisabled = true;
                            } else {
                                if (m_pAttachedTo) {
                                    if (m_pAttachedTo->IsPhysical() && m_pAttachedTo->physicalFlags.bDisableCollisionForce) {
                                        bCollisionDisabled = true;
                                    }
                                } else if (m_pEntityIgnoredCollision == entity) {
                                    bCollisionDisabled = true;
                                } else if (!physicalFlags.bDisableZ || physicalFlags.bApplyGravity) {
                                    if (physicalFlags.bDontCollideWithFlyers
                                        && m_nStatus == STATUS_PLAYER_PLAYBACK_FROM_BUFFER
                                        && m_nStatus != STATUS_REMOTE_CONTROLLED
                                        && entity->DoesNotCollideWithFlyers())
                                    {
                                        bCollisionDisabled = true;
                                    }
                                } else {
                                    bCollisionDisabled = true;
                                }
                            }
                        } else {
                            SpecialEntityPreCollisionStuff(entity, true, bCollisionDisabled, bCollidedEntityCollisionIgnored, bCollidedEntityUnableToMove, bThisOrCollidedEntityStuck);
                        }

                        if (IsPed())
                            physicalFlags.bSkipLineCol = true;

                        if (bCollidedEntityCollisionIgnored || bCollisionDisabled)
                            continue;

                        entity->SetCurrentScanCode();
                        int32 totalColPointsToProcess = ProcessEntityCollision(entity, colPoints);
                        if (totalColPointsToProcess <= 0)
                            continue;

                        for (int32 colpointIndex = 0; colpointIndex < totalColPointsToProcess; colpointIndex++) {
                            CColPoint* colPoint = &colPoints[colpointIndex];
                            if (colPoint->m_fDepth > 0.0f) {
                                uint8 pieceTypeB = colPoint->m_nPieceTypeB;
                                if (pieceTypeB < 13 || pieceTypeB > 16) {
                                    totalAcceptableColPoints++;
                                    if (IsVehicle() && entity->IsPed()
                                        && colPoint->m_vecNormal.z < 0.0f)
                                    {
                                        vecShift.x += colPoint->m_vecNormal.x;
                                        vecShift.y += colPoint->m_vecNormal.y;
                                        fMaxColPointDepth = std::max(fMaxColPointDepth, colPoint->m_fDepth);
                                    } else {
                                        if (!IsPed() || !entity->IsObject()
                                            || !entity->physicalFlags.bDisableMoveForce
                                            || fabs(colPoint->m_vecNormal.z) <= 0.1f)
                                        {
                                            vecShift += colPoint->m_vecNormal;
                                            fMaxColPointDepth = std::max(fMaxColPointDepth, colPoint->m_fDepth);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } while (node);
        }
    } while (scanListIndex);

    if (totalAcceptableColPoints == 0)
        return false;

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

    if (!IsVehicle() || 1.5f <= 0.0f) // R* nice check
        return true;

    vecShift.z = std::max(vecShift.z, 0.0f);
    m_vecMoveSpeed += vecShift * 0.008f * CTimer::GetTimeStep();
    return true;
}

// Used in driving school mission
// 0x546DB0
void CPhysical::PlacePhysicalRelativeToOtherPhysical(CPhysical* relativeToPhysical, CPhysical* physicalToPlace, CVector offset)
{
    CVector vecRelativePosition = relativeToPhysical->m_matrix->TransformPoint(offset);
    vecRelativePosition += CTimer::GetTimeStep() * 0.9f * relativeToPhysical->m_vecMoveSpeed;
    CWorld::Remove(physicalToPlace);
    *(CMatrix*)physicalToPlace->m_matrix = *relativeToPhysical->m_matrix;
    physicalToPlace->GetPosition() = vecRelativePosition;
    physicalToPlace->m_vecMoveSpeed = relativeToPhysical->m_vecMoveSpeed;
    physicalToPlace->UpdateRW();
    physicalToPlace->UpdateRwFrame();
    CWorld::Add(physicalToPlace);
}

// Unused
// 0x546ED0
float CPhysical::ApplyScriptCollision(CVector vecColNormal, float fElasticity, float fAdhesiveLimit, CVector* pVecColPos)
{
    CColPoint colPos{};
    float fImpulseMagnitude = 0.0f;
    float originalElasticity = m_fElasticity;
    colPos.m_vecNormal = vecColNormal;
    if (pVecColPos) {
        colPos.m_vecPoint = pVecColPos;
    } else {
        const CSimpleTransform transform = m_matrix ? (CSimpleTransform&)m_matrix->GetPosition() : m_placement;
        const float radius = GetColModel()->GetBoundRadius();
        colPos.m_vecPoint = transform.m_vPosn - vecColNormal * radius;
    }
    m_fElasticity = fElasticity;
    if (ApplyCollision(this, colPos, fImpulseMagnitude)) {
        if (fAdhesiveLimit > 0.0f) {
            ApplyFriction(fAdhesiveLimit, colPos);
        }
    }
    m_fElasticity = originalElasticity;
    return fImpulseMagnitude;
}

// 0x546FF0
void CPhysical::PositionAttachedEntity()
{
    if (!m_pAttachedTo)
        return;

    auto* attachedToVehicle = m_pAttachedTo->AsVehicle();
    auto* attachedToAuto = m_pAttachedTo->AsAutomobile();
    auto* attachedToBike = m_pAttachedTo->AsBike();

    CMatrix attachedEntityMatrix;
    CMatrix attachedEntityRotationMatrix;
    CMatrix attachedToEntityMatrix (m_pAttachedTo->GetMatrix());
    if (m_pAttachedTo->IsVehicle() && attachedToVehicle->IsBike()) {
        attachedToBike->CalculateLeanMatrix();
        attachedToEntityMatrix = attachedToBike->m_mLeanMatrix;
    } else {
        if (IsObject() && m_pAttachedTo->m_nModelIndex == MODEL_FORKLIFT) {
            RwFrame* carPart = attachedToAuto->m_aCarNodes[CAR_MISC_A];
            if (carPart)
                attachedToEntityMatrix.UpdateMatrix(RwFrameGetLTM(carPart));
        }
    }

    if (m_qAttachedEntityRotation.imag != 0.0f) {
        RwMatrix rotationMatrix;
        RtQuatConvertToMatrix((RtQuat*)&m_qAttachedEntityRotation, &rotationMatrix);
        attachedEntityMatrix.UpdateMatrix(&rotationMatrix);
        attachedEntityMatrix = attachedToEntityMatrix * attachedEntityMatrix;
        CVector vecTranslation = attachedToEntityMatrix.TransformPoint(m_vecAttachOffset);
        attachedEntityMatrix.SetTranslateOnly(vecTranslation);
    } else {
        if (physicalFlags.bInfiniteMass) {
            attachedEntityMatrix = *m_matrix;
        }
        else {
            attachedEntityRotationMatrix.ResetOrientation();
            attachedEntityRotationMatrix.Rotate(m_vecAttachedEntityRotation);
            attachedEntityMatrix = attachedToEntityMatrix;
            attachedEntityMatrix *= attachedEntityRotationMatrix;
        }
        attachedEntityMatrix.GetPosition() = attachedToEntityMatrix.TransformPoint(m_vecAttachOffset);
    }
    SetMatrix(attachedEntityMatrix);

    if (IsObject()) {
        if (IsStatic())
            SetIsStatic(false);

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
    if (!bUpdateSpeed)
    {
        if (m_pAttachedTo->m_nModelIndex == MODEL_DUMPER) {
            int16 wMiscComponentAngle = attachedToAuto->m_wMiscComponentAngle;
            if (wMiscComponentAngle && wMiscComponentAngle != attachedToAuto->m_wMiscComponentAnglePrev) {
                bDettachEntity = true;
            } else if (m_fDamageIntensity > 0.0f && m_pDamageEntity && m_pDamageEntity->IsBuilding()) {
                bDettachEntity = true;
            }
        } else {
            if (m_pAttachedTo->m_nModelIndex != MODEL_FORKLIFT) {
                if (IsVehicle()) {
                    CMatrix attachedToEntityMatrix = m_pAttachedTo->GetMatrix();
                    if (fabs(attachedToEntityMatrix.GetRight().z) > 0.707f || fabs(attachedToEntityMatrix.GetForward().z) > 0.707f) { // 0.707 == sin(DegreesToRadians(45))
                        DettachEntityFromEntity(0.0f, 0.0f, 1.0f, false);
                        return;
                    }
                }
                bUpdateSpeed = true;
            } else if (!attachedToAuto->m_wMiscComponentAngle && attachedToAuto->m_wMiscComponentAnglePrev
                || m_fDamageIntensity > 0.1f * m_fMass && m_pDamageEntity && m_pDamageEntity->IsBuilding())
            {
                bDettachEntity = true;
            }
        }
        if (!bUpdateSpeed && !bDettachEntity) {
            float fDamagedIntensity = m_pAttachedTo->m_fDamageIntensity;
            CMatrix* matrix = m_pAttachedTo->m_matrix;
            if ((fDamagedIntensity <= 0.0f || m_pAttachedTo->GetUp().z >= 0.1f) && (fDamagedIntensity <= 1.0f || m_pAttachedTo->GetForward().z <= 0.87f)) {
                float fDotProduct = m_pAttachedTo->m_vecLastCollisionImpactVelocity.Dot(matrix->GetUp());
                if (fDamagedIntensity <= 500.0f || fDotProduct >= 0.7f || sq(0.3f) <= m_pAttachedTo->m_vecMoveSpeed.SquaredMagnitude())
                    bUpdateSpeed = true;
            }
        }
    }

    if (!bUpdateSpeed) {
        auto* attachedTo = m_pAttachedTo;
        DettachAutoAttachedEntity();
        if (!physicalFlags.bDisableCollisionForce) {
            float randomNumber = CGeneral::GetRandomNumberInRange(-1.0f, 1.0f);
            CMatrix& attachedToEntityMatrix = attachedTo->GetMatrix(); // todo: shadow var
            CVector randomRight = attachedToEntityMatrix.GetRight() * randomNumber;
            CVector randomForward = attachedToEntityMatrix.GetForward() * randomNumber;
            CVector force = (randomRight + randomForward) * (m_fMass / 50.0f);
            ApplyMoveForce(force);
            if (attachedToAuto->m_wMiscComponentAngle > attachedToAuto->m_wMiscComponentAnglePrev)
                ApplyMoveForce(attachedTo->GetMatrix().GetUp() * m_fMass / 50.0f);
        }
        return;
    }

    if (physicalFlags.bInfiniteMass) {
        CVector vecMoveSpeed = GetPosition() - m_vecAttachedEntityRotation;
        float fSquaredMagnitude = vecMoveSpeed.SquaredMagnitude();
        m_vecAttachedEntityRotation = GetPosition();
        if (fSquaredMagnitude > 4.0f)
            vecMoveSpeed *= 2.0f / sqrt(fSquaredMagnitude);

        float fTimeStep = 1.0f;
        if (CTimer::GetTimeStep() >= 1.0f)
            fTimeStep = CTimer::GetTimeStep();

        vecMoveSpeed *= 1.0f / fTimeStep;
        CVector vecMoveSpeedDifference = vecMoveSpeed - m_vecMoveSpeed;
        m_vecMoveSpeed = vecMoveSpeed;
        CVector vecForce = -(vecMoveSpeedDifference * m_fMass);
        CVector vecCenterOfMassMultiplied = GetMatrix().TransformVector(m_vecCentreOfMass);
        ApplyForce(vecForce, vecCenterOfMassMultiplied, true);
        if (m_pAttachedTo->IsVehicle() || m_pAttachedTo->IsObject()) {
            if (m_pAttachedTo->m_bUsesCollision && !m_pAttachedTo->physicalFlags.bDisableCollisionForce) {
                CVector vecDistance = (vecCenterOfMassMultiplied + GetPosition()) - m_pAttachedTo->GetPosition();
                m_pAttachedTo->ApplyForce(-vecForce, vecDistance, true);
            }
            float fRotationInRadians = m_pAttachedTo->GetHeading() - GetHeading();
            if (fRotationInRadians > PI)
                fRotationInRadians -= PI * 2;
            else if (fRotationInRadians < -PI)
                fRotationInRadians += PI * 2;

            fRotationInRadians = std::clamp(fRotationInRadians, -0.5f, 0.5f);
            m_vecTurnSpeed.z += fRotationInRadians / 100'000.0f * m_fMass;
        }
    }
    else {
        if (m_pAttachedTo->IsVehicle() || m_pAttachedTo->IsObject()) {
            m_vecMoveSpeed = m_pAttachedTo->GetSpeed(GetPosition() - m_pAttachedTo->GetPosition());
            m_vecTurnSpeed = m_pAttachedTo->m_vecTurnSpeed;
        }
    }
}

// 0x547B80
void CPhysical::ApplySpeed()
{
    auto* object = AsObject();
    float fOldTimeStep = CTimer::GetTimeStep();
    if (physicalFlags.bDisableZ) {
        if (physicalFlags.bApplyGravity) {
            if (fOldTimeStep * m_vecMoveSpeed.z + GetPosition().z < CWorld::SnookerTableMin.z) {
                GetPosition().z = CWorld::SnookerTableMin.z;
                ResetMoveSpeed();
                ResetTurnSpeed();
            }
            ApplyMoveSpeed();
            ApplyTurnSpeed();
            CTimer::UpdateTimeStep(fOldTimeStep);
            return;
        }

        float fTimeStepX = 1000.0f;
        float fTimeStepY = 1000.0f;
        const float fNewPositionX = fOldTimeStep * m_vecMoveSpeed.x + GetPosition().x;
        if (m_vecMoveSpeed.x > 0.0f) {
            if (fNewPositionX > CWorld::SnookerTableMax.x) {
                fTimeStepX = (CWorld::SnookerTableMax.x - GetPosition().x) / m_vecMoveSpeed.x;
            }
        } else if (m_vecMoveSpeed.x < 0.0f) {
            if (fNewPositionX < CWorld::SnookerTableMin.x) {
                fTimeStepX = (CWorld::SnookerTableMin.x - GetPosition().x) / m_vecMoveSpeed.x;
            }
        }
        float fNewPositionY = CTimer::GetTimeStep() * m_vecMoveSpeed.y + GetPosition().y;
        if (m_vecMoveSpeed.y > 0.0f) {
            if (fNewPositionY > CWorld::SnookerTableMax.y) {
                fTimeStepY = (CWorld::SnookerTableMax.y - GetPosition().y) / m_vecMoveSpeed.y;
            }
        } else if (m_vecMoveSpeed.y < 0.0f) {
            if (fNewPositionY < CWorld::SnookerTableMin.y) {
                fTimeStepY = (CWorld::SnookerTableMin.y - GetPosition().y) / m_vecMoveSpeed.y;
            }
        }

        bool bTableWidthIsLessThanHeight = true;
        if (CWorld::SnookerTableMax.x - CWorld::SnookerTableMin.x < CWorld::SnookerTableMax.y - CWorld::SnookerTableMin.y)
            bTableWidthIsLessThanHeight = false;

        bool bApplyFriction = false;
        float fNormalX = 0.0f;
        float fNormalY = 0.0f;
        float fAbsoluteMoveSpeed = 0.0f;
        float fNewTimeStep = 0.0f;
        if (fTimeStepX < fTimeStepY && fTimeStepX < 1000.0f) {
            fNormalX = m_vecMoveSpeed.x <= 0.0f ? 1.0f : -1.0f;
            fAbsoluteMoveSpeed = fabs(m_vecMoveSpeed.x);

            CTimer::UpdateTimeStep(fTimeStepX);
            ApplyMoveSpeed();
            ApplyTurnSpeed();
            float fTableY = (CWorld::SnookerTableMin.y + CWorld::SnookerTableMax.y) * 0.5f;
            if (CWorld::SnookerTableMax.y - 0.06f >= GetPosition().y && CWorld::SnookerTableMin.y + 0.06f <= GetPosition().y
                && (bTableWidthIsLessThanHeight || fabs(GetPosition().y - fTableY) >= 0.06f))
            {
                m_vecMoveSpeed.x = -m_vecMoveSpeed.x;
            } else {
                physicalFlags.bApplyGravity = true;
                float fTimeStepMoveSpeedX = fOldTimeStep * m_vecMoveSpeed.x;
                m_vecMoveSpeed.x = std::clamp(m_vecMoveSpeed.x, -0.03f / fOldTimeStep, 0.03f / fOldTimeStep);
            }
            fNewTimeStep = fOldTimeStep - fTimeStepX;
            bApplyFriction = true;
        }
        if (!bApplyFriction && fTimeStepY < 1000.0f) {
            fNormalY = m_vecMoveSpeed.y <= 0.0f ? 1.0f : -1.0f;
            fAbsoluteMoveSpeed = fabs(m_vecMoveSpeed.y);

            CTimer::UpdateTimeStep(fTimeStepY);
            ApplyMoveSpeed();
            ApplyTurnSpeed();
            float fTableX = (CWorld::SnookerTableMin.x + CWorld::SnookerTableMax.x) * 0.5f;
            if (CWorld::SnookerTableMax.x - 0.06f < GetPosition().x
                || CWorld::SnookerTableMin.x + 0.06f > GetPosition().x
                || bTableWidthIsLessThanHeight
                && fabs(GetPosition().x - fTableX) < 0.06f)
            {
                physicalFlags.bApplyGravity = true;
                float fTimeStepMoveSpeedY = fOldTimeStep * m_vecMoveSpeed.y;
                if (fTimeStepMoveSpeedY <= 0.03f && fTimeStepMoveSpeedY >= -0.03f) {
                    m_vecMoveSpeed.y = std::clamp(m_vecMoveSpeed.y, -0.03f / fOldTimeStep, 0.03f / fOldTimeStep);
                }
            } else {
                m_vecMoveSpeed.y = -m_vecMoveSpeed.y;
            }
            fNewTimeStep = fOldTimeStep - fTimeStepY;
            bApplyFriction = true;
        }

        if (bApplyFriction) {
            CTimer::UpdateTimeStep(fNewTimeStep);
            if (fAbsoluteMoveSpeed > 0.0f) {
                float fRadius = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundRadius();
                CVector thePosition = CVector(fNormalX * fRadius, fNormalY * fRadius, 0.0f);
                CColPoint colPoint{};
                colPoint.m_vecPoint = GetPosition() - thePosition;
                colPoint.m_vecNormal = CVector(fNormalX, fNormalY, 0.0f);
                ApplyFriction(10.0f * fAbsoluteMoveSpeed, colPoint);
                if (IsObject()) {
                    AudioEngine.ReportMissionAudioEvent(AE_CAS4_FE, object);
                    object->m_nLastWeaponDamage = 4 * (object->m_nLastWeaponDamage == 0xFF) + WEAPON_RUNOVERBYCAR;
                }
            }
        }
        ApplyMoveSpeed();
        ApplyTurnSpeed();
        CTimer::UpdateTimeStep(fOldTimeStep);
        return;
    }

    if (!physicalFlags.bDisableMoveForce || !IsObject() || object->m_fDoorStartAngle <= -1000.0f) {
        ApplyMoveSpeed();
        ApplyTurnSpeed();
        CTimer::UpdateTimeStep(fOldTimeStep);
        return;
    }

    float fDoorStartAngle = object->m_fDoorStartAngle;
    float fHeading = GetHeading();
    if (fDoorStartAngle + PI >= fHeading) {
        if (fDoorStartAngle - PI > fHeading) {
            fHeading += PI * 2;
        } else {
            fHeading -= PI * 2;
        }
    }

    float fNewTimeStep = -1000.0f;
    float fTheDoorAngle = DegreesToRadians(108.0f) + fDoorStartAngle;
    float fHeadingTimeStep = CTimer::GetTimeStep() * m_vecTurnSpeed.z + fHeading;
    if (m_vecTurnSpeed.z <= 0.0f || fHeadingTimeStep <= DegreesToRadians(108.0f) + fDoorStartAngle) {
        if (m_vecTurnSpeed.z < 0.0f) {
            float fTheDoorAngle = fDoorStartAngle - DegreesToRadians(108.0f); // todo: shadow var
            if (fHeadingTimeStep < fTheDoorAngle) {
                fNewTimeStep = (fTheDoorAngle - fHeading) / m_vecTurnSpeed.z;
            }
        }
    } else {
        fNewTimeStep = (fTheDoorAngle - fHeading) / m_vecTurnSpeed.z;
    }

    if (-CTimer::GetTimeStep() <= fNewTimeStep) {
        CTimer::UpdateTimeStep(fNewTimeStep);
        ApplyTurnSpeed();
        m_vecTurnSpeed.z = -0.2f * m_vecTurnSpeed.z;
        CTimer::UpdateTimeStep(fOldTimeStep - fNewTimeStep);
        physicalFlags.b31 = true;
    }
    ApplyMoveSpeed();
    ApplyTurnSpeed();
    CTimer::UpdateTimeStep(fOldTimeStep);
    if (object->objectFlags.bIsDoorMoving) {
        float fNewHeading = GetHeading();
        if (fNewHeading + PI >= fDoorStartAngle) {
            if (fNewHeading - PI > fDoorStartAngle) {
                fNewHeading -= PI * 2;
            }
        } else {
            fNewHeading += PI * 2;
        }
        fHeading -= fDoorStartAngle;
        fNewHeading -= fDoorStartAngle;
        if (fabs(fHeading) < 0.001f)
            fHeading = 0.0f;

        if (fabs(fNewHeading) < 0.001f)
            fNewHeading = 0.0f;

        if (fHeading * fNewHeading < 0.0f)
            m_vecTurnSpeed.z = 0.0f;
    }
}

// 0x548320
void CPhysical::UnsetIsInSafePosition()
{
    m_vecMoveSpeed *= -1.0f;
    m_vecTurnSpeed *= -1.0f;
    ApplySpeed();
    m_vecMoveSpeed *= -1.0f;
    m_vecTurnSpeed *= -1.0f;
    m_bIsInSafePosition = false;
}

// 0x5483D0
void CPhysical::ApplyFriction()
{
    if (physicalFlags.bDisableZ)
    {
        const CVector& vecPosition = GetPosition();
        float fSphereRadius = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundRadius();
        CColPoint colPoint(CVector(vecPosition.x, vecPosition.y, vecPosition.z - fSphereRadius));
        colPoint.m_vecNormal = CVector(0.0f, 0.0f, 1.0f);

        ApplyFriction(CTimer::GetTimeStep() * 0.001f, colPoint);
        m_vecTurnSpeed.z = pow(0.98f, CTimer::GetTimeStep()) * m_vecTurnSpeed.z;
    }

    m_vecMoveSpeed += m_vecFrictionMoveSpeed;
    m_vecTurnSpeed += m_vecFrictionTurnSpeed;
    ResetFrictionMoveSpeed();
    ResetFrictionTurnSpeed();

    auto* vehicle = AsVehicle();
    if (IsVehicle() && vehicle->IsBike()
        && !physicalFlags.b32 && m_nStatus == STATUS_ABANDONED
        && fabs(GetUp().z) < 0.707f
        && m_vecMoveSpeed.SquaredMagnitude() < sq(0.05f)
        && m_vecTurnSpeed.SquaredMagnitude() < sq(0.01f))
    {
        m_vecMoveSpeed *= pow(0.5f, CTimer::GetTimeStep());
    }
}

// 0x548680
bool CPhysical::ApplyCollision(CEntity* theEntity, CColPoint& colPoint, float& thisDamageIntensity, float& entityDamageIntensity)
{
    auto* entity = theEntity->AsPhysical();
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

    if (!entity->physicalFlags.bDisableTurnForce || physicalFlags.bDisableMoveForce) {
        fThisMassFactor = physicalFlags.bMakeMassTwiceAsBig ? 2.0f : 1.0f;
    } else {
        fThisMassFactor = 10.0f;
        if (entity->IsPed() && entity->AsPed()->m_standingOnEntity == this)
            bEntityPedIsStandingOnThis = true;
    }

    if (physicalFlags.bDisableTurnForce) {
        if (IsPed() && thisPed->IsPlayer() && entity->IsVehicle()
            && (entity->m_nStatus == STATUS_ABANDONED
                || entity->m_nStatus == STATUS_WRECKED || m_bIsStuck))
        {
            float fTheEntityMass = entity->m_fMass - 2000.0f;
            fTheEntityMass = std::max(0.0f, fTheEntityMass);
            fEntityMassFactor = 1.0f / (fTheEntityMass / 5000.0f + 1.0f);
        }
        else if (!entity->physicalFlags.bDisableMoveForce) {
            fEntityMassFactor = 10.0f;
        }
        if (IsPed() && thisPed->m_standingOnEntity == entity) {
            bThisPedIsStandingOnEntity = true;
            fEntityMassFactor = 10.0f;
        }
    } else if (IsVehicle() && thisVehicle->m_pTrailer) {
        fEntityMassFactor = (thisVehicle->m_pTrailer->m_fMass + m_fMass) / m_fMass;
    } else {
        fEntityMassFactor = entity->physicalFlags.bMakeMassTwiceAsBig ? 2.0f : 1.0f;
    }

    if (entity->physicalFlags.bDisableCollisionForce && !entity->physicalFlags.bCollidable
        || entity->m_pAttachedTo && !entity->physicalFlags.bInfiniteMass)
    {
        bEntityCollisionForceDisabled = true;
        bThisPedIsStandingOnEntity = false;
    }
    CVector vecThisCentreOfMassMultiplied = GetMatrix().TransformVector(m_vecCentreOfMass);
    CVector vecEntityCentreOfMassMultiplied = entity->GetMatrix().TransformVector(entity->m_vecCentreOfMass);
    if (physicalFlags.bInfiniteMass)
        vecThisCentreOfMassMultiplied = CVector(0.0f, 0.0f, 0.0f);

    if (entity->physicalFlags.bInfiniteMass)
        vecEntityCentreOfMassMultiplied = CVector(0.0f, 0.0f, 0.0f);

    if (IsVehicle() && entity->IsObject()
        && entityObject->objectFlags.bIsLampPost
        || entity->IsVehicle() && IsObject()
        && thisObject->objectFlags.bIsLampPost)
    {
        colPoint.m_vecNormal.z = 0.0f;
    }
    if (entity->IsStatic() && !bEntityCollisionForceDisabled) {
        if (physicalFlags.bDisableTurnForce) {
            float fThisSpeedDotProduct = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
            if (fThisSpeedDotProduct < 0.0f) {
                if (!entity->IsObject()) {
                    if (entity->physicalFlags.bDisableCollisionForce)
                        return ApplyCollision(entity, colPoint, thisDamageIntensity);

                    entity->SetIsStatic(false);
                } else {
                    thisDamageIntensity = -(fThisSpeedDotProduct * m_fMass);
                    entityDamageIntensity = thisDamageIntensity;

                    if (entity->physicalFlags.bDisableCollisionForce
                        || entityObject->m_pObjectInfo->m_fUprootLimit >= 9999.0f
                        || thisDamageIntensity <= entityObject->m_pObjectInfo->m_fUprootLimit)
                    {
                        return ApplyCollision(entity, colPoint, thisDamageIntensity);
                    }

                    if (IsGlassModel(entity)) {
                        CGlass::WindowRespondsToCollision(entity, thisDamageIntensity, m_vecMoveSpeed, colPoint.m_vecPoint, false);
                    } else if (!entity->physicalFlags.bDisableCollisionForce) {
                        entity->SetIsStatic(false);
                        CWorld::Players[CWorld::PlayerInFocus].m_nHavocCaused += 2;
                        CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, static_cast<float>(CGeneral::GetRandomNumberInRange(30, 60)));
                    }
                }
            }
        } else {
            CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
            CVector vecThisSpeed = GetSpeed(vecDistanceToPointFromThis);
            if (physicalFlags.bAddMovingCollisionSpeed && IsVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
                vecThisSpeed += thisVehicle->AddMovingCollisionSpeed(vecDistanceToPointFromThis);

            float fThisSpeedDotProduct = vecThisSpeed.Dot(colPoint.m_vecNormal);
            if (fThisSpeedDotProduct < 0.0f) {
                if (!entity->IsObject()) {
                    if (entity->physicalFlags.bDisableCollisionForce)
                        return ApplyCollision(entity, colPoint, thisDamageIntensity);

                    entity->SetIsStatic(false);
                    CWorld::Players[CWorld::PlayerInFocus].m_nHavocCaused += 2;
                    CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, static_cast<float>(CGeneral::GetRandomNumberInRange(30, 60)));
                } else {
                    CVector vecThisDifference = vecDistanceToPointFromThis - vecThisCentreOfMassMultiplied;
                    float fThisCollisionMass = GetTurnTorque(vecThisDifference, colPoint.m_vecNormal);
                    thisDamageIntensity = m_bHasHitWall ? -(fThisCollisionMass * fThisSpeedDotProduct)
                                                         : -((m_fElasticity + 1.0f) * fThisCollisionMass * fThisSpeedDotProduct);

                    entityDamageIntensity = thisDamageIntensity;
                    CObjectData* entityObjectInfo = entityObject->m_pObjectInfo;
                    const float fObjectDamageMultiplier = (IsVehicle() && thisVehicle->IsSubBike()) ? 3.0f : 1.0f;
                    if (entityObject->m_nColDamageEffect) {
                        float fObjectDamage = fObjectDamageMultiplier * thisDamageIntensity;
                        if (fObjectDamage > 20.0f) {
                            entityObject->ObjectDamage(fObjectDamage, &colPoint.m_vecPoint, &colPoint.m_vecNormal, this, WEAPON_UNIDENTIFIED);
                            if (!entity->m_bUsesCollision) {
                                if (!physicalFlags.bDisableCollisionForce) {
                                    float fColDamageMultiplier = entityObjectInfo->m_fColDamageMultiplier;
                                    float fCollisionDamage = fColDamageMultiplier + fColDamageMultiplier;
                                    CVector vecMoveForce = (colPoint.m_vecNormal * thisDamageIntensity) / fCollisionDamage;
                                    ApplyForce(vecMoveForce, vecDistanceToPointFromThis, true);
                                }

                                float fDamageIntensityMultiplier = entityObjectInfo->m_fColDamageMultiplier / fThisCollisionMass;
                                ReportCollision(this, entity, true, colPoint, fDamageIntensityMultiplier * thisDamageIntensity * 2.f);
                                return false;
                            }
                        }
                    }

                    if (entity->physicalFlags.bDisableCollisionForce
                        || entityObjectInfo->m_fUprootLimit >= 9999.0f
                        || thisDamageIntensity <= entityObjectInfo->m_fUprootLimit)
                    {
                        if (IsGlassModel(entity))
                            CGlass::WindowRespondsToSoftCollision(entityObject, thisDamageIntensity);

                        return ApplyCollision(entity, colPoint, thisDamageIntensity);
                    }

                    if (IsGlassModel(entity))
                        CGlass::WindowRespondsToCollision(entity, thisDamageIntensity, m_vecMoveSpeed, colPoint.m_vecPoint, false);
                    else
                        entity->SetIsStatic(false);

                    int32 entityModelIndex = entity->m_nModelIndex;
                    if (entityModelIndex == ModelIndices::MI_FIRE_HYDRANT
                        && !entityObject->objectFlags.bIsExploded)
                    {
                        g_fx.TriggerWaterHydrant(entity->GetPosition());
                        entityObject->objectFlags.bIsExploded = true;
                    } else if (
                        (entityModelIndex == ModelIndices::MI_PARKINGMETER
                            || entityModelIndex == ModelIndices::MI_PARKINGMETER2)
                        && !entityObject->objectFlags.bIsExploded)
                    {
                        CPickups::CreateSomeMoney(entity->GetPosition(), CGeneral::GetRandomNumber() % 100);
                        entityObject->objectFlags.bIsExploded = true;
                    } else if (!entity->IsObject() || entityObjectInfo->m_bCausesExplosion) {
                        entityObject->objectFlags.bIsExploded = true;
                    }

                    if (!physicalFlags.bDisableCollisionForce && entityObjectInfo->m_fUprootLimit > 200.0f) {
                        ApplyForce((colPoint.m_vecNormal * 0.2f) * thisDamageIntensity, vecDistanceToPointFromThis, true);
                    }
                }
            }
        }

        if (entity->IsStatic())
            return false;

        if (!entity->physicalFlags.bDisableCollisionForce)
            entity->AddToMovingList();
    }

    if (physicalFlags.bDisableTurnForce && entity->physicalFlags.bDisableTurnForce) {
        bool bApplyEntityCollisionForce = true;
        float fThisMass = m_fMass;
        float fEntityMass = entity->m_fMass;
        float fThisSpeedDotProduct = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
        float fEntitySpeedDotProduct = entity->m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
        float fMoveSpeed = 0.0f;
        if (physicalFlags.bDisableCollisionForce || physicalFlags.bDontApplySpeed) {
            fMoveSpeed = fThisSpeedDotProduct;
        } else if (entity->physicalFlags.bDisableCollisionForce || entity->physicalFlags.bDontApplySpeed) {
            fMoveSpeed = fEntitySpeedDotProduct;
            bApplyEntityCollisionForce = false;
        } else if (!thisPed->bPushOtherPeds) {
            fMoveSpeed = std::min(0.0f, fEntitySpeedDotProduct);
            bApplyEntityCollisionForce = false;
        } else {
            fMoveSpeed = (fThisMass * fThisSpeedDotProduct * 4.0f + fEntityMass * fEntitySpeedDotProduct) / (fThisMass * 4.0f + fEntityMass);
        }

        float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
        if (fThisMoveSpeedDifference >= 0.0f)
            return false;

        float fTheElasticity = (entity->m_fElasticity + m_fElasticity) * 0.5f;
        const float fThisMoveSpeedElasticity = m_bHasHitWall ? fMoveSpeed : fMoveSpeed - fTheElasticity * fThisMoveSpeedDifference;
        thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisMass;
        CVector vecThisMoveForce = colPoint.m_vecNormal * thisDamageIntensity;
        if (!physicalFlags.bDisableCollisionForce && !physicalFlags.bDontApplySpeed) {
            ApplyMoveForce(vecThisMoveForce);
            ReportCollision(this, entity, true, colPoint, thisDamageIntensity / fThisMass);
        }
        if (bApplyEntityCollisionForce) {
            const float fEntityMoveSpeedElasticity = entity->m_bHasHitWall ? fMoveSpeed : fMoveSpeed - (fEntitySpeedDotProduct - fMoveSpeed) * fTheElasticity;
            entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityMass);
            CVector vecEntityMoveForce = -colPoint.m_vecNormal * entityDamageIntensity;
            if (!entity->physicalFlags.bDisableCollisionForce && !entity->physicalFlags.bDontApplySpeed) {
                if (entity->m_bIsInSafePosition)
                    entity->UnsetIsInSafePosition();

                entity->ApplyMoveForce(vecEntityMoveForce);
                ReportCollision(entity, this, false, colPoint, entityDamageIntensity / fEntityMass);
            }
        }
        return true;
    }

    if (physicalFlags.bDisableTurnForce) {
        CVector vecDistanceToPoint = colPoint.m_vecPoint - entity->GetPosition();
        CVector vecEntitySpeed = entity->GetSpeed(vecDistanceToPoint);
        if (entity->physicalFlags.bAddMovingCollisionSpeed && IsVehicle()
            && colPoint.m_nSurfaceTypeB == SURFACE_CAR_MOVINGCOMPONENT)
        {
            vecEntitySpeed += entityVehicle->AddMovingCollisionSpeed(vecDistanceToPoint);
        }

        float fThisSpeedDotProduct = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
        float fEntitySpeedDotProduct = vecEntitySpeed.Dot(colPoint.m_vecNormal);
        float fThisMass = fThisMassFactor * m_fMass;
        CVector vecEntityDifference = (vecDistanceToPoint - vecEntityCentreOfMassMultiplied);
        CVector vecEntityCrossProduct = vecEntityDifference.Cross(colPoint.m_vecNormal);
        float squaredMagnitude = vecEntityCrossProduct.SquaredMagnitude();
        float fEntityCollisionMass = squaredMagnitude / (fEntityMassFactor * entity->m_fTurnMass);
        if (!entity->physicalFlags.bDisableMoveForce)
            fEntityCollisionMass += 1.0f / (fEntityMassFactor * entity->m_fMass);

        fEntityCollisionMass = 1.0f / fEntityCollisionMass;
        const float fMoveSpeed = bEntityCollisionForceDisabled
            ? fEntitySpeedDotProduct
            : (fEntityCollisionMass * fEntitySpeedDotProduct + fThisMass * fThisSpeedDotProduct) / (fEntityCollisionMass + fThisMass);

        float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
        if (fThisMoveSpeedDifference < 0.0f) {
            float fTheElasticity = (entity->m_fElasticity + m_fElasticity) * 0.5f;
            const float fThisMoveSpeedElasticity = m_bHasHitWall
                ? fMoveSpeed
                : fMoveSpeed - fTheElasticity * fThisMoveSpeedDifference;

            const float fEntityMoveSpeedElasticity = entity->m_bHasHitWall
                ? fMoveSpeed
                : fMoveSpeed - (fEntitySpeedDotProduct - fMoveSpeed) * fTheElasticity;

            thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisMass;
            entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityCollisionMass);
            CVector vecThisMoveForce = (thisDamageIntensity / fThisMassFactor) * colPoint.m_vecNormal;
            CVector vecEntityMoveForce = -colPoint.m_vecNormal * (entityDamageIntensity / fEntityMassFactor);
            if (!physicalFlags.bDisableCollisionForce) {
                vecThisMoveForce.z = std::max(vecThisMoveForce.z, 0.0f);
                if (bThisPedIsStandingOnEntity) {
                    vecThisMoveForce.x *= 2;
                    vecThisMoveForce.y *= 2;
                }
                ApplyMoveForce(vecThisMoveForce);
            }
            if (!entity->physicalFlags.bDisableCollisionForce && !bThisPedIsStandingOnEntity) {
                if (entity->m_bIsInSafePosition)
                    entity->UnsetIsInSafePosition();

                entity->ApplyForce(vecEntityMoveForce, vecDistanceToPoint, true);
            }
            ReportCollision(this, entity, true, colPoint, thisDamageIntensity / fThisMass);
            ReportCollision(entity, this, false, colPoint, entityDamageIntensity / fEntityCollisionMass);
            return true;
        }
        return false;
    }

    if (entity->physicalFlags.bDisableTurnForce) {
        CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
        CVector vecThisSpeed = GetSpeed(vecDistanceToPointFromThis);

        if (!physicalFlags.bAddMovingCollisionSpeed && IsVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
            vecThisSpeed += thisVehicle->AddMovingCollisionSpeed(vecDistanceToPointFromThis);

        float fThisSpeedDotProduct = vecThisSpeed.Dot(colPoint.m_vecNormal);
        float fEntitySpeedDotProduct = entity->m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
        CVector vecThisDifference = (vecDistanceToPointFromThis - vecThisCentreOfMassMultiplied);
        CVector vecThisCrossProduct = vecThisDifference.Cross(colPoint.m_vecNormal);
        float squaredMagnitude = vecThisCrossProduct.SquaredMagnitude();
        float fThisCollisionMass = squaredMagnitude / (fThisMassFactor * m_fTurnMass);
        if (!physicalFlags.bDisableMoveForce)
            fThisCollisionMass += 1.0f / (fThisMassFactor * m_fMass);

        fThisCollisionMass = 1.0f / fThisCollisionMass;
        float fEntityMass = fEntityMassFactor * entity->m_fMass;
        float fMoveSpeed = (fEntityMass * fEntitySpeedDotProduct + fThisCollisionMass * fThisSpeedDotProduct) / (fEntityMass + fThisCollisionMass);
        float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
        if (fThisMoveSpeedDifference >= 0.0f)
            return false;

        float fTheElasticity = (entity->m_fElasticity + m_fElasticity) * 0.5f;
        const float fThisMoveSpeedElasticity = m_bHasHitWall
            ? fMoveSpeed
            : fMoveSpeed - fTheElasticity * fThisMoveSpeedDifference;

        const float fEntityMoveSpeedElasticity = entity->m_bHasHitWall
            ? fMoveSpeed
            : fMoveSpeed - (fEntitySpeedDotProduct - fMoveSpeed) * fTheElasticity;
    
        thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisCollisionMass;
        entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityMass);

        CVector vecThisMoveForce = colPoint.m_vecNormal * (thisDamageIntensity / fThisMassFactor);
        CVector vecEntityMoveForce = colPoint.m_vecNormal * (entityDamageIntensity / fEntityMassFactor) * -1.0f;
        if (!physicalFlags.bDisableCollisionForce && !bEntityPedIsStandingOnThis) {
            vecThisMoveForce.z = std::max(0.0f, vecThisMoveForce.z);
            ApplyForce(vecThisMoveForce, vecDistanceToPointFromThis, true);
        }
        if (!entity->physicalFlags.bDisableCollisionForce) {
            if (vecEntityMoveForce.z < 0.0f) {
                vecEntityMoveForce.z = 0.0f;
                if (fabs(fThisSpeedDotProduct) < 0.01f) {
                    vecEntityMoveForce.x *= 0.5f;
                    vecEntityMoveForce.y *= 0.5f;
                }
            }
            if (bEntityPedIsStandingOnThis) {
                vecEntityMoveForce.x *= 2;
                vecEntityMoveForce.y *= 2;
            }
            if (entity->m_bIsInSafePosition)
                entity->UnsetIsInSafePosition();

            entity->ApplyMoveForce(vecEntityMoveForce);
        }
        ReportCollision(this, entity, true, colPoint, thisDamageIntensity / fThisCollisionMass);
        ReportCollision(entity, this, false, colPoint, entityDamageIntensity / fEntityMass);
    }
    else
    {
        CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
        CVector vecThisSpeed = GetSpeed(vecDistanceToPointFromThis);

        if (physicalFlags.bAddMovingCollisionSpeed && IsVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
            vecThisSpeed += thisVehicle->AddMovingCollisionSpeed(vecDistanceToPointFromThis);

        CVector vecDistanceToPoint = colPoint.m_vecPoint - entity->GetPosition();
        CVector vecEntitySpeed = entity->GetSpeed(vecDistanceToPoint);
        if (entity->physicalFlags.bAddMovingCollisionSpeed && entity->IsVehicle() && colPoint.m_nSurfaceTypeB == SURFACE_CAR_MOVINGCOMPONENT)
            vecEntitySpeed += entityVehicle->AddMovingCollisionSpeed(vecDistanceToPoint);

        float fThisSpeedDotProduct = vecThisSpeed.Dot(colPoint.m_vecNormal);
        float fEntitySpeedDotProduct = vecEntitySpeed.Dot(colPoint.m_vecNormal);

        CVector vecThisDifference = (vecDistanceToPointFromThis - vecThisCentreOfMassMultiplied);
        CVector vecThisCrossProduct = vecThisDifference.Cross(colPoint.m_vecNormal);
        float squaredMagnitude = vecThisCrossProduct.SquaredMagnitude();

        float fThisCollisionMass = squaredMagnitude / (fThisMassFactor * m_fTurnMass);
        if (!physicalFlags.bDisableMoveForce)
            fThisCollisionMass += 1.0f / (fThisMassFactor * m_fMass);

        fThisCollisionMass = 1.0f / fThisCollisionMass;
        CVector vecEntityDifference = (vecDistanceToPoint - vecEntityCentreOfMassMultiplied);
        CVector vecEntityCrossProduct = vecEntityDifference.Cross(colPoint.m_vecNormal);
        squaredMagnitude = vecEntityCrossProduct.SquaredMagnitude();
        float fEntityCollisionMass = squaredMagnitude / (fEntityMassFactor * entity->m_fTurnMass);
        if (!entity->physicalFlags.bDisableMoveForce)
            fEntityCollisionMass += 1.0f / (fEntityMassFactor * entity->m_fMass);

        fEntityCollisionMass = 1.0f / fEntityCollisionMass;
        float fMoveSpeed = (fEntityCollisionMass * fEntitySpeedDotProduct + fThisCollisionMass * fThisSpeedDotProduct) / (fEntityCollisionMass + fThisCollisionMass);
        float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
        if (fThisMoveSpeedDifference >= 0.0f)
            return false;

        float fTheElasticity = (entity->m_fElasticity + m_fElasticity) * 0.5f;
        const float fThisMoveSpeedElasticity = m_bHasHitWall
            ? fMoveSpeed
            : fMoveSpeed - fTheElasticity * fThisMoveSpeedDifference;

        const float fEntityMoveSpeedElasticity = entity->m_bHasHitWall
            ? fMoveSpeed
            : fMoveSpeed - (fEntitySpeedDotProduct - fMoveSpeed) * fTheElasticity;

        thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisCollisionMass;
        entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityCollisionMass);

        CVector vecThisMoveForce = colPoint.m_vecNormal * (thisDamageIntensity / fThisMassFactor);
        CVector vecEntityMoveForce = colPoint.m_vecNormal * (entityDamageIntensity / fEntityMassFactor) * -1.0f;

        auto applyCollisionForces =
            [&](auto& obj, CVector& moveForce, CVector& distanceToPoint, bool isThisEntity) {
            if (obj.IsVehicle() && !obj.m_bHasHitWall && !obj.physicalFlags.bDisableCollisionForce) {
                const float normalZThreshold = isThisEntity ? colPoint.m_vecNormal.z: -colPoint.m_vecNormal.z;
                if (normalZThreshold < 0.7f)
                    moveForce.z *= 0.3f;

                if (!obj.m_nStatus)
                    distanceToPoint *= 0.8f;

                if (CWorld::bNoMoreCollisionTorque) {
                    CVector vecFrictionForce = moveForce * -0.3f;
                    obj.ApplyFrictionForce(vecFrictionForce, distanceToPoint);
                }
            }
        };

        applyCollisionForces(*this, vecThisMoveForce, vecDistanceToPointFromThis, true);
        applyCollisionForces(*entity, vecEntityMoveForce, vecDistanceToPoint, false);

        if (CCheat::IsActive(CHEAT_CARS_FLOAT_AWAY_WHEN_HIT)) {
            if (FindPlayerVehicle() == thisVehicle && entity->IsVehicle()
                && entityVehicle->m_nCreatedBy != MISSION_VEHICLE)
            {
                entity->physicalFlags.bApplyGravity = false;
            }
            if (FindPlayerVehicle() == entityVehicle && IsVehicle()
                && thisVehicle->m_nCreatedBy != MISSION_VEHICLE)
            {
                physicalFlags.bApplyGravity = false;
            }
        }

        if (!physicalFlags.bDisableCollisionForce)
            ApplyForce(vecThisMoveForce, vecDistanceToPointFromThis, true);

        if (!entity->physicalFlags.bDisableCollisionForce) {
            if (entity->m_bIsInSafePosition)
                entity->UnsetIsInSafePosition();

            entity->ApplyForce(vecEntityMoveForce, vecDistanceToPoint, true);
        }
        ReportCollision(this, entity, true, colPoint, thisDamageIntensity / fThisCollisionMass);
        ReportCollision(entity, this, false, colPoint, entityDamageIntensity / fEntityCollisionMass);
    }
    return true;
}

// 0x54A2C0
bool CPhysical::ApplySoftCollision(CPhysical* physical, CColPoint& colPoint, float& thisDamageIntensity, float& entityDamageIntensity)
{
    auto* entityObject = physical->AsObject();
    auto* entityPed = physical->AsPed();
    auto* entityVehicle = physical->AsVehicle();

    auto* thisObject = AsObject();
    auto* thisPed = AsPed();
    auto* thisVehicle = AsVehicle();

    bool bEntityPedIsStandingOnThis = false;
    bool bThisPedIsStandingOnEntity = false;
    bool bEntityCollisionForceDisabled = false;

    float fThisMassFactor = 0.0f;
    float fEntityMassFactor = 0.0f;
    if (!physical->physicalFlags.bDisableTurnForce || physicalFlags.bDisableMoveForce || physicalFlags.bInfiniteMass)
    {
        fThisMassFactor = physicalFlags.bMakeMassTwiceAsBig ? 2.0f : 1.0f;
    } else {
        fThisMassFactor = 10.0f;
        if (entityPed->m_standingOnEntity == this) // BUG: Game should be checking if entity is ped or not.
            bEntityPedIsStandingOnThis = true;
    }
    if (!physicalFlags.bDisableTurnForce || physical->physicalFlags.bDisableMoveForce || physical->physicalFlags.bInfiniteMass)
    {
        fEntityMassFactor = physical->physicalFlags.bMakeMassTwiceAsBig ? 2.0f : 1.0f;
    } else {
        if (IsPed() && thisPed->IsPlayer() && physical->IsVehicle()
            && (physical->m_nStatus == STATUS_ABANDONED
                || physical->m_nStatus == STATUS_WRECKED || m_bIsStuck))
        {
            float fTheEntityMass = std::max(0.0f, physical->m_fMass - 2000.0f);
            fEntityMassFactor = 1.0f / (fTheEntityMass * 0.0002f + 1.0f);
        } else {
            fEntityMassFactor = 10.0f;
        }

        if (IsPed() && thisPed->m_standingOnEntity == physical)
            bThisPedIsStandingOnEntity = true;
    }

    if (physical->physicalFlags.bDisableCollisionForce && !physical->physicalFlags.bCollidable)
    {
        bEntityCollisionForceDisabled = true;
        bThisPedIsStandingOnEntity = false;
    }

    CVector vecThisCentreOfMassMultiplied = GetMatrix().TransformVector(m_vecCentreOfMass);
    CVector vecEntityCentreOfMassMultiplied = physical->GetMatrix().TransformVector(physical->m_vecCentreOfMass);

    if (physicalFlags.bInfiniteMass)
        vecThisCentreOfMassMultiplied = CVector(0.0f, 0.0f, 0.0f);

    if (physical->physicalFlags.bInfiniteMass)
        vecEntityCentreOfMassMultiplied = CVector(0.0f, 0.0f, 0.0f);

    bool bApplyCollisionWithElasticity = false;
    if (!physical->IsStatic() || bEntityCollisionForceDisabled) {
        bApplyCollisionWithElasticity = true;
    } else if (physicalFlags.bDisableTurnForce) {
        return ApplySoftCollision(physical, colPoint, thisDamageIntensity);
    }

    CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
    CVector vecThisSpeed = GetSpeed(vecDistanceToPointFromThis);

    float fThisSpeedDotProduct = vecThisSpeed.Dot(colPoint.m_vecNormal);
    if (!bApplyCollisionWithElasticity && fThisSpeedDotProduct < 0.0f)
    {
        if (!physical->IsObject())
        {
            if (!physical->physicalFlags.bDisableCollisionForce)
            {
                physical->SetIsStatic(false);
                CWorld::Players[CWorld::PlayerInFocus].m_nHavocCaused += 2;
                CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, static_cast<float>(CGeneral::GetRandomNumberInRange(30, 60)));
            }
        }
        else
        {
            CVector vecThisDifference = (vecDistanceToPointFromThis - vecThisCentreOfMassMultiplied);
            float fThisCollisionMass = GetTurnTorque(vecThisDifference, colPoint.m_vecNormal);
            if (!m_bHasHitWall)
                thisDamageIntensity = -((m_fElasticity + 1.0f) * fThisCollisionMass * fThisSpeedDotProduct);
            else
                thisDamageIntensity = -(fThisCollisionMass * fThisSpeedDotProduct);

            entityDamageIntensity = thisDamageIntensity;

            if (entityObject->m_nColDamageEffect != COL_DAMAGE_EFFECT_NONE && thisDamageIntensity > 20.0f)
            {
                entityObject->ObjectDamage(thisDamageIntensity, &colPoint.m_vecPoint, &colPoint.m_vecNormal, this, WEAPON_UNIDENTIFIED);
                if (!physical->m_bUsesCollision)
                {
                    if (!physicalFlags.bDisableCollisionForce)
                    {
                        CVector vecMoveSpeed = (colPoint.m_vecNormal * 0.2f) * thisDamageIntensity;
                        ApplyForce(vecMoveSpeed, vecDistanceToPointFromThis, true);
                    }
                    ReportCollision(this, physical, true, colPoint, thisDamageIntensity / fThisCollisionMass * 0.2f);
                    return false;
                }
            }

            float fObjectUprootLimit = entityObject->m_pObjectInfo->m_fUprootLimit;
            if ((thisDamageIntensity > fObjectUprootLimit || m_bIsStuck) && !physical->physicalFlags.bDisableCollisionForce)
            {
                if (IsGlassModel(physical))
                    CGlass::WindowRespondsToCollision(physical, thisDamageIntensity, m_vecMoveSpeed, colPoint.m_vecPoint, false);
                else if (!physical->physicalFlags.bDisableCollisionForce)
                    physical->SetIsStatic(false);

                int32 entityModelIndex = physical->m_nModelIndex;
                if (entityModelIndex != MODEL_FIRE_HYDRANT || entityObject->objectFlags.bIsExploded)
                {
                    if (entityModelIndex != ModelIndices::MI_PARKINGMETER && entityModelIndex != ModelIndices::MI_PARKINGMETER2 || entityObject->objectFlags.bIsExploded)
                    {
                        CBaseModelInfo* pBaseModelInfo = CModelInfo::GetModelInfo(entityModelIndex);
                        if (physical->IsObject() && !pBaseModelInfo->AsAtomicModelInfoPtr())
                            entityObject->objectFlags.bIsExploded = true;
                    }
                    else
                    {
                        CPickups::CreateSomeMoney(physical->GetPosition(), CGeneral::GetRandomNumber() % 100);
                        entityObject->objectFlags.bIsExploded = true;
                    }
                }
                else
                {
                    g_fx.TriggerWaterHydrant(physical->GetPosition());
                    entityObject->objectFlags.bIsExploded = true;
                }

                if (physical->IsStatic())
                    return false;

                if (!physical->physicalFlags.bDisableCollisionForce)
                    physical->AddToMovingList();

                bApplyCollisionWithElasticity = true;
            }
            else
            {
                if (IsGlassModel(physical))
                    CGlass::WindowRespondsToSoftCollision(entityObject, thisDamageIntensity);

                CVector vecThisMoveForce = colPoint.m_vecNormal * thisDamageIntensity;
                if (IsVehicle() && colPoint.m_vecNormal.z < 0.7f)
                    vecThisMoveForce.z *= 0.3f;

                if (!physicalFlags.bDisableCollisionForce)
                {
                    bool bUpdateTorque = !IsVehicle() || !CWorld::bNoMoreCollisionTorque;
                    ApplyForce(vecThisMoveForce, vecDistanceToPointFromThis, bUpdateTorque);
                }
                ReportCollision(this, physical, true, colPoint, thisDamageIntensity / fThisCollisionMass);
            }
        }
    }

    if (bApplyCollisionWithElasticity) {
        if (physicalFlags.bDisableTurnForce && physical->physicalFlags.bDisableTurnForce) {
            float fThisSpeedDotProduct = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
            float fEntitySpeedDotProduct = physical->m_vecMoveSpeed.Dot(colPoint.m_vecNormal);

            float fMoveSpeed = fEntitySpeedDotProduct;
            float fThisMass = m_fMass;
            float fEntityMass = physical->m_fMass;
            bool bPedFollowPath = false;
            if (thisPed->m_nPedState == PEDSTATE_FOLLOW_PATH) {
                bPedFollowPath = true;
                fMoveSpeed = (fEntityMass * fEntitySpeedDotProduct + fThisMass * fThisSpeedDotProduct + fThisMass * fThisSpeedDotProduct)
                    / (fThisMass + fThisMass + fEntityMass);
            } else if (fEntitySpeedDotProduct >= 0.0f) {
                fMoveSpeed = 0.0f;
            }

            float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
            if (fThisMoveSpeedDifference >= 0.0f)
                return false;

            float fTheElasticity = (physical->m_fElasticity + m_fElasticity) * 0.5f;
            float fThisMoveSpeedElasticity = m_bHasHitWall ? fMoveSpeed
                : fMoveSpeed - fThisMoveSpeedDifference * fTheElasticity;

            thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisMass;
            CVector vecThisMoveForce = colPoint.m_vecNormal * thisDamageIntensity;

            if (!physicalFlags.bDisableCollisionForce)
                ApplyMoveForce(vecThisMoveForce);

            ReportCollision(this, physical, true, colPoint, thisDamageIntensity / fThisMass);
            if (bPedFollowPath) {
                float fEntityMoveSpeedDifference = fEntitySpeedDotProduct - fMoveSpeed;
                if (fEntityMoveSpeedDifference < 0.0f) {
                    float fEntityMoveSpeedElasticity = physical->m_bHasHitWall ? fMoveSpeed
                        : fMoveSpeed - fEntityMoveSpeedDifference * fTheElasticity;

                    entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityMass);
                    CVector vecEntityMoveForce = -colPoint.m_vecNormal * entityDamageIntensity;
                    if (!physical->physicalFlags.bDisableCollisionForce)
                        physical->ApplyMoveForce(vecEntityMoveForce);

                    ReportCollision(physical, this, false, colPoint, entityDamageIntensity / fEntityMass);
                }
            }
            return true;
        }

        if (physicalFlags.bDisableTurnForce) {
            CVector vecDistanceToPoint = colPoint.m_vecPoint - physical->GetPosition();
            CVector vecEntitySpeed = physical->GetSpeed(vecDistanceToPoint);

            if (physical->physicalFlags.bAddMovingCollisionSpeed && physical->IsVehicle()
                && colPoint.m_nSurfaceTypeB == SURFACE_CAR_MOVINGCOMPONENT)
            {
                vecEntitySpeed += entityVehicle->AddMovingCollisionSpeed(vecDistanceToPoint);
            }

            float fThisMass = fThisMassFactor * m_fMass;

            float fThisSpeedDotProduct = m_vecMoveSpeed.Dot(colPoint.m_vecNormal);
            float fEntitySpeedDotProduct = vecEntitySpeed.Dot(colPoint.m_vecNormal);

            CVector vecEntityDifference = vecDistanceToPoint - vecEntityCentreOfMassMultiplied;
            float fEntityCollisionMass = fEntityMassFactor * physical->GetTurnTorque(vecEntityDifference, colPoint.m_vecNormal);
            float fMoveSpeed = bEntityCollisionForceDisabled ? fEntitySpeedDotProduct
                : (fEntityCollisionMass * fEntitySpeedDotProduct + fThisMass * fThisSpeedDotProduct) / (fEntityCollisionMass + fThisMass);

            float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
            if (fThisMoveSpeedDifference < 0.0f) {
                float fTheElasticity = (physical->m_fElasticity + m_fElasticity) * 0.5f;
                const float fThisMoveSpeedElasticity = m_bHasHitWall ? fMoveSpeed
                    : fMoveSpeed - fThisMoveSpeedDifference * fTheElasticity;
                const float fEntityMoveSpeedElasticity = physical->m_bHasHitWall ? fMoveSpeed
                    : fMoveSpeed - (fEntitySpeedDotProduct - fMoveSpeed) * fTheElasticity;

                thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisMass;
                entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityCollisionMass);

                CVector vecThisMoveForce = colPoint.m_vecNormal * (thisDamageIntensity / fThisMassFactor);
                CVector vecEntityMoveForce = -colPoint.m_vecNormal * (entityDamageIntensity / fEntityMassFactor);

                if (!physicalFlags.bDisableCollisionForce) {
                    vecThisMoveForce.z = std::max(vecThisMoveForce.z, 0.0f);
                    if (bThisPedIsStandingOnEntity) {
                        vecThisMoveForce.x *= 2;
                        vecThisMoveForce.y *= 2;
                    }

                    ApplyMoveForce(vecThisMoveForce);
                }

                if (physical->physicalFlags.bDisableCollisionForce == false && bThisPedIsStandingOnEntity == false)
                    physical->ApplyForce(vecEntityMoveForce, vecDistanceToPoint, true);

                ReportCollision(this, physical, true, colPoint, thisDamageIntensity / fThisMass);
                ReportCollision(physical, this, false, colPoint, entityDamageIntensity / fEntityCollisionMass);
                return true;
            }
        }
        else
        {
            if (physical->physicalFlags.bDisableTurnForce) {
                CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
                CVector vecThisSpeed = GetSpeed(vecDistanceToPointFromThis);

                if (physicalFlags.bAddMovingCollisionSpeed && IsVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
                    vecThisSpeed += thisVehicle->AddMovingCollisionSpeed(vecDistanceToPointFromThis);

                float fThisSpeedDotProduct = vecThisSpeed.Dot(colPoint.m_vecNormal);
                float fEntitySpeedDotProduct = physical->m_vecMoveSpeed.Dot(colPoint.m_vecNormal);

                CVector vecThisDifference = vecDistanceToPointFromThis - vecThisCentreOfMassMultiplied;
                float fThisCollisionMass = fThisMassFactor * GetTurnTorque(vecThisDifference, colPoint.m_vecNormal);

                float fEntityMass = fEntityMassFactor * physical->m_fMass;
                float fMoveSpeed = (fEntityMass * fEntitySpeedDotProduct + fThisCollisionMass * fThisSpeedDotProduct) / (fEntityMass + fThisCollisionMass);
                float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
                if (fThisMoveSpeedDifference < 0.0f) {
                    float fTheElasticity = (physical->m_fElasticity + m_fElasticity) * 0.5f;
                    const float fThisMoveSpeedElasticity = m_bHasHitWall ? fMoveSpeed
                        : fMoveSpeed - fThisMoveSpeedDifference * fTheElasticity;

                    const float fEntityMoveSpeedElasticity = physical->m_bHasHitWall ? fMoveSpeed
                        : fMoveSpeed - (fEntitySpeedDotProduct - fMoveSpeed) * fTheElasticity;

                    thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisCollisionMass;
                    entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityMass);

                    CVector vecThisMoveForce = colPoint.m_vecNormal * (thisDamageIntensity / fThisMassFactor);
                    CVector vecEntityMoveForce = -colPoint.m_vecNormal * (entityDamageIntensity / fEntityMassFactor);
                    if (!physicalFlags.bDisableCollisionForce && !bEntityPedIsStandingOnThis) {
                        vecThisMoveForce.z = std::max(vecThisMoveForce.z, 0.0f);
                        ApplyForce(vecThisMoveForce, vecDistanceToPointFromThis, true);
                    }

                    if (!physical->physicalFlags.bDisableCollisionForce)
                    {
                        if (vecEntityMoveForce.z < 0.0f) {
                            vecEntityMoveForce.z = 0.0f;
                            if (fabs(fThisSpeedDotProduct) < 0.01f) {
                                vecEntityMoveForce.x *= 0.5f;
                                vecEntityMoveForce.y *= 0.5f;
                            }
                        }
                        if (bEntityPedIsStandingOnThis) {
                            vecEntityMoveForce.x *= 2;
                            vecEntityMoveForce.y *= 2;
                        }

                        physical->ApplyMoveForce(vecEntityMoveForce);
                    }
                    ReportCollision(this, physical, true, colPoint, thisDamageIntensity / fThisCollisionMass);
                    ReportCollision(physical, this, false, colPoint, entityDamageIntensity / fEntityMass);
                    return true;
                }
            }
            else
            {
                CVector vecDistanceToPointFromThis = colPoint.m_vecPoint - GetPosition();
                CVector vecThisSpeed = GetSpeed(vecDistanceToPointFromThis);

                if (physicalFlags.bAddMovingCollisionSpeed && IsVehicle() && colPoint.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
                    vecThisSpeed += thisVehicle->AddMovingCollisionSpeed(vecDistanceToPointFromThis);

                CVector vecDistanceToPoint = colPoint.m_vecPoint - physical->GetPosition();
                CVector vecEntitySpeed = physical->GetSpeed(vecDistanceToPoint);

                if (physical->physicalFlags.bAddMovingCollisionSpeed && physical->IsVehicle() && colPoint.m_nSurfaceTypeB == SURFACE_CAR_MOVINGCOMPONENT)
                    vecEntitySpeed += entityVehicle->AddMovingCollisionSpeed(vecDistanceToPoint);

                float fThisSpeedDotProduct = vecThisSpeed.Dot(colPoint.m_vecNormal);
                float fEntitySpeedDotProduct = vecEntitySpeed.Dot(colPoint.m_vecNormal);

                CVector vecThisDifference = vecDistanceToPointFromThis - vecThisCentreOfMassMultiplied;
                float fThisCollisionMass = fThisMassFactor * GetTurnTorque(vecThisDifference, colPoint.m_vecNormal);

                CVector vecEntityDifference = vecDistanceToPoint - vecEntityCentreOfMassMultiplied;
                float fEntityCollisionMass = fEntityMassFactor * physical->GetTurnTorque(vecEntityDifference, colPoint.m_vecNormal);

                float fMoveSpeed = (fEntityCollisionMass * fEntitySpeedDotProduct + fThisCollisionMass * fThisSpeedDotProduct) / (fEntityCollisionMass + fThisCollisionMass);
                float fThisMoveSpeedDifference = fThisSpeedDotProduct - fMoveSpeed;
                if (fThisMoveSpeedDifference < 0.0f) {
                    float fTheElasticity = (physical->m_fElasticity + m_fElasticity) * 0.5f;
                    const float fThisMoveSpeedElasticity = m_bHasHitWall ? fMoveSpeed
                        : fMoveSpeed - fThisMoveSpeedDifference * fTheElasticity;

                    const float fEntityMoveSpeedElasticity = physical->m_bHasHitWall ? fMoveSpeed
                        : fMoveSpeed - (fEntitySpeedDotProduct - fMoveSpeed) * fTheElasticity;

                    thisDamageIntensity = (fThisMoveSpeedElasticity - fThisSpeedDotProduct) * fThisCollisionMass;
                    entityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntitySpeedDotProduct) * fEntityCollisionMass);

                    CVector vecThisMoveForce = colPoint.m_vecNormal * (thisDamageIntensity / fThisMassFactor);
                    CVector vecEntityMoveForce = -colPoint.m_vecNormal * (entityDamageIntensity / fEntityMassFactor);

                    if (IsVehicle() && !m_bHasHitWall) {
                        if (colPoint.m_vecNormal.z < 0.7f)
                            vecThisMoveForce.z *= 0.3f;

                        if (!m_nStatus)
                            vecDistanceToPointFromThis *= 0.8f;

                        if (CWorld::bNoMoreCollisionTorque)
                        {
                            CVector vecFrictionForce = vecThisMoveForce * -0.3f;
                            ApplyFrictionForce(vecFrictionForce, vecDistanceToPointFromThis);
                        }
                    }

                    if (physical->IsVehicle() && !physical->m_bHasHitWall) {
                        if (-colPoint.m_vecNormal.z < 0.7f)
                            vecEntityMoveForce.z *= 0.3f;

                        if (!physical->m_nStatus)
                            vecDistanceToPoint *= 0.8f;

                        if (CWorld::bNoMoreCollisionTorque)
                        {
                            CVector vecFrictionForce = vecEntityMoveForce * -0.3f;
                            physical->ApplyFrictionForce(vecFrictionForce, vecDistanceToPoint);
                        }
                    }
                    if (!physicalFlags.bDisableCollisionForce)
                        ApplyForce(vecThisMoveForce, vecDistanceToPointFromThis, true);

                    if (physical->physicalFlags.bDisableCollisionForce == false) {
                        if (physical->m_bIsInSafePosition) {
                            physical->UnsetIsInSafePosition();
                        }
                        physical->ApplyForce(vecEntityMoveForce, vecDistanceToPoint, true);
                    }
                    ReportCollision(this, physical, true, colPoint, thisDamageIntensity / fThisCollisionMass);
                    ReportCollision(physical, this, false, colPoint, entityDamageIntensity / fEntityCollisionMass);
                    return true;
                }
            }
        }
        return false;
    }
    return true;
}

// 0x54BA60
bool CPhysical::ProcessCollisionSectorList(int32 sectorX, int32 sectorY)
{
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
    float fBoundingSphereRadius = mi->GetColModel()->GetBoundRadius();

    CVector vecBoundCentre;
    GetBoundCentre(&vecBoundCentre);

    CSector* sector = GetSector(sectorX, sectorY);
    CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);

    int32 scanListIndex = 4;
    do {
        CPtrListDoubleLink* list = nullptr;
        --scanListIndex;
        switch (scanListIndex) {
        case 0:
            list = &repeatSector->GetList(REPEATSECTOR_PEDS);
            break;
        case 1:
            list = &repeatSector->GetList(REPEATSECTOR_OBJECTS);
            break;
        case 2:
            list = &repeatSector->GetList(REPEATSECTOR_VEHICLES);
            break;
        case 3:
            list = &sector->m_buildings;
            break;
        }
        CPtrNodeDoubleLink* node = list->GetNode();
        if (node) {
            CEntity* entity = nullptr;

            CPhysical* physicalEntity = nullptr;
            CObject* entityObject = nullptr;
            CPed* entityPed = nullptr;
            CVehicle* entityVehicle = nullptr;

            CObject* thisObject = nullptr;
            CPed* thisPed = nullptr;
            CVehicle* thisVehicle = nullptr;

            while (node) {
                entity = (CEntity*)node->m_item;
                assert(entity);

                node = node->m_next;

                physicalEntity = entity->AsPhysical();
                entityObject = entity->AsObject();
                entityPed = entity->AsPed();
                entityVehicle = entity->AsVehicle();

                thisObject = AsObject();
                thisPed = AsPed();
                thisVehicle = AsVehicle();

                if (!entity->m_bUsesCollision || physicalEntity == this || entity->IsScanCodeCurrent())
                    continue;

                if (!entity->GetIsTouching(vecBoundCentre, fBoundingSphereRadius)) {
                    if (m_pEntityIgnoredCollision == entity && m_pAttachedTo != entity)
                        m_pEntityIgnoredCollision = nullptr;

                    if (entity->IsPhysical()
                        && physicalEntity->m_pEntityIgnoredCollision == this
                        && physicalEntity->m_pAttachedTo != this)
                    {
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
                    if (physicalFlags.bInfiniteMass && m_bIsStuck)
                        bThisOrCollidedEntityStuck = true;

                    if (physicalFlags.bDisableCollisionForce && (!IsVehicle() || thisVehicle->IsSubTrain())) {
                        bCollisionDisabled = true;
                    } else {
                        if (m_pAttachedTo && m_pAttachedTo->IsPhysical()
                            && m_pAttachedTo->physicalFlags.bDisableCollisionForce)
                        {
                            bCollisionDisabled = true;
                        } else if (m_pEntityIgnoredCollision == entity) {
                            bCollisionDisabled = true;
                        } else if (!physicalFlags.bDisableZ || physicalFlags.bApplyGravity) {
                            if (physicalFlags.bDontCollideWithFlyers) {
                                if (m_nStatus && m_nStatus != STATUS_REMOTE_CONTROLLED && entity->DoesNotCollideWithFlyers()) {
                                    bCollisionDisabled = true;
                                }
                            }
                        } else {
                            bCollisionDisabled = true;
                        }
                    }
                } else {
                    SpecialEntityPreCollisionStuff(entity->AsPhysical(), false, bCollisionDisabled, bCollidedEntityCollisionIgnored, bCollidedEntityUnableToMove, bThisOrCollidedEntityStuck);
                }

                if (!m_bUsesCollision || bCollidedEntityCollisionIgnored || bCollisionDisabled) {
                    entity->SetCurrentScanCode();
                    if (!bCollisionDisabled) // if collision is enabled then
                    {
                        int32 totalColPointsToProcess = ProcessEntityCollision(physicalEntity, &colPoints[0]);
                        if (physicalFlags.b17 && !bCollidedEntityCollisionIgnored && totalColPointsToProcess > 0)
                            return true;

                        if (!totalColPointsToProcess && m_pEntityIgnoredCollision == entity && this == FindPlayerPed())
                            m_pEntityIgnoredCollision = nullptr;
                    }
                    continue;
                }

                if (entity->IsBuilding() || physicalEntity->physicalFlags.bCollidable || bCollidedEntityUnableToMove) {
                    entity->SetCurrentScanCode();

                    fThisMaxDamageIntensity = 0.0f;
                    int32 totalAcceptableColPoints = 0;
                    CVector vecMoveSpeed = CVector(0.0f, 0.0f, 0.0f);
                    CVector vecTurnSpeed = CVector(0.0f, 0.0f, 0.0f);

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
                                        fThisMaxDamageIntensity = std::max(fThisMaxDamageIntensity, fThisMaxDamageIntensity);

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
                                    if (ApplySoftCollision(entity, *colPoint, fThisDamageIntensity) &&
                                        (colPoint->m_nSurfaceTypeA != SURFACE_WHEELBASE || colPoint->m_nSurfaceTypeB != SURFACE_WHEELBASE)) {
                                        float fSurfaceFriction = g_surfaceInfos.GetAdhesiveLimit(colPoint);
                                        if (ApplyFriction(fSurfaceFriction, *colPoint))
                                            m_bHasContacted = true;

                                        continue;
                                    }
                                } else if (ApplyCollisionAlt(physicalEntity, *colPoint, fThisDamageIntensity, vecMoveSpeed, vecTurnSpeed)) {
                                    ++totalAcceptableColPoints;
                                    fThisMaxDamageIntensity = std::max(fThisMaxDamageIntensity, fThisMaxDamageIntensity);

                                    float fSurfaceFriction = g_surfaceInfos.GetAdhesiveLimit(colPoint);
                                    float fFriction = fSurfaceFriction / totalColPointsToProcess;
                                    if (!IsVehicle()) {
                                        fFriction *= 150.0f * fThisDamageIntensity;
                                        SetDamagedPieceRecord(fThisDamageIntensity, entity, *colPoint, 1.0f);
                                        if (ApplyFriction(fFriction, *colPoint))
                                            m_bHasContacted = true;

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
                                                if (g_surfaceInfos.GetAdhesionGroup(colPoint->m_nSurfaceTypeB) == ADHESION_GROUP_LOOSE
                                                    || g_surfaceInfos.GetAdhesionGroup(colPoint->m_nSurfaceTypeB) == ADHESION_GROUP_SAND)
                                                {
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
                                                if (GetUp().z > 0.3f && m_vecMoveSpeed.SquaredMagnitude() < 0.02f && m_vecTurnSpeed.SquaredMagnitude() < 0.01f) {
                                                    fFriction = 0.0f;
                                                } else if (m_nStatus == STATUS_ABANDONED || colPoint->m_vecNormal.Dot(GetUp()) < 0.707f) {
                                                    fFriction = 150.0f / m_fMass * fFriction * fThisDamageIntensity;
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
                                if (std::abs(m_vecMoveSpeed.x) > 0.2f && std::abs(m_vecMoveSpeed.y) > 0.2f) {
                                    if (!physicalFlags.bSubmergedInWater) {
                                        float factor = 0.3f / static_cast<float>(totalColPointsToProcess);
                                        m_vecFrictionMoveSpeed.x -= vecMoveSpeed.x *  factor;
                                        m_vecFrictionMoveSpeed.y -= vecMoveSpeed.y *  factor;
                                        m_vecFrictionTurnSpeed   += vecTurnSpeed   * -factor;
                                    }
                                }
                            }
                        }

                        if (entity->IsObject() && entityObject->m_nColDamageEffect && fThisMaxDamageIntensity > 20.0f)
                            entityObject->ObjectDamage(fThisMaxDamageIntensity, &colPoints[0].m_vecPoint, &fxDirection, this, WEAPON_UNIDENTIFIED);

                        if (!CWorld::bSecondShift)
                            return true;

                        bResult = true;
                    }
                } else {
                    entity->SetCurrentScanCode();

                    int32 totalAcceptableColPoints = 0;
                    int32 totalColPointsToProcess = ProcessEntityCollision(physicalEntity, &colPoints[0]);
                    if (totalColPointsToProcess <= 0)
                        continue;

                    fThisMaxDamageIntensity = 0.0f;
                    fEntityMaxDamageIntensity = 0.0f;
                    if (m_bHasContacted && entity->m_bHasContacted) {
                        if (totalColPointsToProcess > 0) {
                            for (int32 colPointIndex4 = 0; colPointIndex4 < totalColPointsToProcess; colPointIndex4++) {
                                CColPoint* colPoint2 = &colPoints[colPointIndex4];
                                if (bThisOrCollidedEntityStuck || (colPoint2->m_nPieceTypeA >= 13 && colPoint2->m_nPieceTypeA <= 16) ||
                                    (colPoint2->m_nPieceTypeB >= 13 && colPoint2->m_nPieceTypeB <= 16)) {
                                    ++totalAcceptableColPoints;
                                    ApplySoftCollision(physicalEntity, *colPoint2, fThisDamageIntensity, fEntityDamageIntensity);
                                } else {
                                    if (ApplyCollision(entity, *colPoint2, fThisDamageIntensity, fEntityDamageIntensity)) {
                                        fThisMaxDamageIntensity = std::max(fThisMaxDamageIntensity, fThisMaxDamageIntensity);
                                        fEntityMaxDamageIntensity = std::max(fEntityMaxDamageIntensity, fEntityDamageIntensity);

                                        SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint2, 1.0f);
                                        physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint2, -1.0f);
                                    }
                                }
                            }
                        }
                    } else if (m_bHasContacted) {
                        m_bHasContacted = false;
                        CVector vecThisFrictionMoveSpeed = m_vecFrictionMoveSpeed;
                        CVector vecThisFrictionTurnSpeed = m_vecFrictionTurnSpeed;
                        ResetFrictionMoveSpeed();
                        ResetFrictionTurnSpeed();
                        if (totalColPointsToProcess > 0) {
                            for (int32 colPointIndex1 = 0; colPointIndex1 < totalColPointsToProcess; colPointIndex1++) {
                                CColPoint* colPoint1 = &colPoints[colPointIndex1];
                                if (bThisOrCollidedEntityStuck || (colPoint1->m_nPieceTypeA >= 13 && colPoint1->m_nPieceTypeA <= 16) ||
                                    (colPoint1->m_nPieceTypeB >= 13 && colPoint1->m_nPieceTypeB <= 16)) {
                                    ++totalAcceptableColPoints;
                                    ApplySoftCollision(physicalEntity, *colPoint1, fThisDamageIntensity, fEntityDamageIntensity);
                                    if (colPoint1->m_nPieceTypeB >= 13 && colPoint1->m_nPieceTypeB <= 16)
                                        physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint1, -1.0f);

                                } else if (ApplyCollision(physicalEntity, *colPoint1, fThisDamageIntensity, fEntityDamageIntensity)) {
                                    fThisMaxDamageIntensity = std::max(fThisMaxDamageIntensity, fThisMaxDamageIntensity);
                                    fEntityMaxDamageIntensity = std::max(fEntityMaxDamageIntensity, fEntityDamageIntensity);

                                    SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint1, 1.0f);
                                    physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint1, -1.0f);

                                    float fSurfaceFriction1 = g_surfaceInfos.GetAdhesiveLimit(colPoint1);
                                    float fFriction3 = fSurfaceFriction1 / totalColPointsToProcess;
                                    if (IsVehicle() && entity->IsVehicle() &&
                                        (m_vecMoveSpeed.SquaredMagnitude() > sq(SQRT_2 * 0.1f) || m_vecTurnSpeed.SquaredMagnitude() > sq(0.1f)))
                                    {
                                        fFriction3 *= fThisDamageIntensity;
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
                            m_vecFrictionMoveSpeed = vecThisFrictionMoveSpeed;
                            m_vecFrictionTurnSpeed = vecThisFrictionTurnSpeed;
                        }
                    } else {
                        if (entity->m_bHasContacted) {
                            entity->m_bHasContacted = false;
                            CVector vecEntityMoveSpeed = physicalEntity->m_vecFrictionMoveSpeed;
                            CVector vecEntityFrictionTurnSpeed = physicalEntity->m_vecFrictionTurnSpeed;
                            physicalEntity->ResetFrictionMoveSpeed();
                            physicalEntity->ResetFrictionTurnSpeed();

                            if (totalColPointsToProcess > 0) {
                                for (int32 colPointIndex3 = 0; colPointIndex3 < totalColPointsToProcess; colPointIndex3++) {
                                    CColPoint* colPoint4 = &colPoints[colPointIndex3];
                                    if (bThisOrCollidedEntityStuck || (colPoint4->m_nPieceTypeA >= 13 && colPoint4->m_nPieceTypeA <= 16) ||
                                        (colPoint4->m_nPieceTypeB >= 13 && colPoint4->m_nPieceTypeB <= 16)) {
                                        ++totalAcceptableColPoints;
                                        ApplySoftCollision(physicalEntity, *colPoint4, fThisDamageIntensity, fEntityDamageIntensity);
                                        if (colPoint4->m_nPieceTypeB >= 13 && colPoint4->m_nPieceTypeB <= 16)
                                            physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint4, -1.0f);

                                    } else if (ApplyCollision(physicalEntity, *colPoint4, fThisDamageIntensity, fEntityDamageIntensity)) {
                                        fThisMaxDamageIntensity = std::max(fThisMaxDamageIntensity, fThisMaxDamageIntensity);
                                        fEntityMaxDamageIntensity = std::max(fEntityMaxDamageIntensity, fEntityDamageIntensity);

                                        SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint4, 1.0f);
                                        physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint4, -1.0f);

                                        float fSurfaceFriction1 = g_surfaceInfos.GetAdhesiveLimit(colPoint4);

                                        float fFriction2 = fSurfaceFriction1 / totalColPointsToProcess;
                                        if (IsVehicle() && entity->IsVehicle()
                                            && (m_vecMoveSpeed.SquaredMagnitude() > 0.02f
                                                || m_vecTurnSpeed.SquaredMagnitude() > 0.01f))
                                        {
                                            fFriction2 *= fThisDamageIntensity;
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
                                physicalEntity->m_vecFrictionMoveSpeed = vecEntityMoveSpeed;
                                physicalEntity->m_vecFrictionTurnSpeed = vecEntityFrictionTurnSpeed;
                            }
                        } else if (totalColPointsToProcess > 0) {
                            for (int32 colPointIndex2 = 0; colPointIndex2 < totalColPointsToProcess; colPointIndex2++) {
                                CColPoint* colPoint3 = &colPoints[colPointIndex2];
                                if (bThisOrCollidedEntityStuck || (colPoint3->m_nPieceTypeA >= 13 && colPoint3->m_nPieceTypeA <= 16)
                                    // || (colPoint3->m_nPieceTypeA >= 13 && colPoint3->m_nPieceTypeA <= 16) // BUG: I think it should be m_nPieceTypeB
                                ) {
                                    ++totalAcceptableColPoints;
                                    ApplySoftCollision(physicalEntity, *colPoint3, fThisDamageIntensity, fEntityDamageIntensity);
                                    if (colPoint3->m_nPieceTypeB >= 13 && colPoint3->m_nPieceTypeB <= 16)
                                        physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint3, -1.0f);

                                } else if (ApplyCollision(physicalEntity, *colPoint3, fThisDamageIntensity, fEntityDamageIntensity)) {
                                    fThisMaxDamageIntensity = std::max(fThisMaxDamageIntensity, fThisMaxDamageIntensity);
                                    fEntityMaxDamageIntensity = std::max(fEntityMaxDamageIntensity, fEntityDamageIntensity);

                                    SetDamagedPieceRecord(fThisDamageIntensity, physicalEntity, *colPoint3, 1.0f);
                                    physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, *colPoint3, -1.0f);

                                    float fSurfaceFirction2 = g_surfaceInfos.GetAdhesiveLimit(colPoint3);
                                    float fFriction1 = fSurfaceFirction2 / totalColPointsToProcess;
                                    if (IsVehicle() && entity->IsVehicle() &&
                                        (m_vecMoveSpeed.SquaredMagnitude() > 0.02f || m_vecTurnSpeed.SquaredMagnitude() > 0.01f))
                                    {
                                        fFriction1 *= fThisDamageIntensity;
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
                    } else if (IsPed() && entity->IsVehicle() && entityVehicle->IsSubTrain() &&
                               (entityVehicle->m_vecMoveSpeed.Dot(m_vecLastCollisionImpactVelocity) > 0.2f ||
                                thisPed->bFallenDown && entityVehicle->m_vecMoveSpeed.SquaredMagnitude() > 0.0005f))
                    {
                        float fDamageIntensity = fThisMaxDamageIntensity + fThisMaxDamageIntensity;
                        thisPed->KillPedWithCar(entityVehicle, fDamageIntensity, false);
                    } else if (entity->IsObject() && IsVehicle() && entity->m_bUsesCollision) {
                        if (entityObject->m_nColDamageEffect && fEntityMaxDamageIntensity > 20.0f) {
                            entityObject->ObjectDamage(fEntityMaxDamageIntensity, &colPoints[0].m_vecPoint, &fxDirection, this, WEAPON_RUNOVERBYCAR);
                        } else {
                            if (entityObject->m_nColDamageEffect >= COL_DAMAGE_EFFECT_SMASH_COMPLETELY) {
                                CBaseModelInfo* pEntityModelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
                                CColModel* colModel = pEntityModelInfo->GetColModel();
                                CVector boundBoxPos = entity->GetMatrix().TransformPoint(colModel->m_boundBox.GetSize());

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
                                CVector boundBoxPos = (*m_matrix).TransformPoint(colModel->m_boundBox.GetSize());

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
        }
    } while (scanListIndex);
    return bResult;
}

// 0x54CFF0
bool CPhysical::ProcessCollisionSectorList_SimpleCar(CRepeatSector* repeatSector)
{
    if (!m_bUsesCollision)
        return false;

    CVector vecBoundingCentre;
    GetBoundCentre(&vecBoundingCentre);

    float fBoundingRadius = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundRadius();
    CPtrListDoubleLink* list = nullptr;
    for (int32 scanListIndex = 1;; --scanListIndex) {
        list = &repeatSector->GetList(scanListIndex == 1 ? REPEATSECTOR_OBJECTS : REPEATSECTOR_VEHICLES);
        if (list->GetNode())
            break;

        if (!scanListIndex)
            return false;
    }

    static CColPoint(&colPoints)[32] = *(CColPoint(*)[32])0xB73C98; // TODO | STATICREF
    CEntity* entity = nullptr;
    CPhysical* physicalEntity = nullptr;
    int32 totalColPointsToProcess = 0;
    for (CPtrNodeDoubleLink* node = list->GetNode(); node; node = node->GetNext()) {
        entity = reinterpret_cast<CEntity*>(node->m_item);
        physicalEntity = entity->AsPhysical();
        bool isLampTouchingGround = (entity->IsObject() && entity->AsObject()->IsFallenLampPost());
        if (entity == this || isLampTouchingGround
            || entity->IsScanCodeCurrent() || !entity->m_bUsesCollision
            || !entity->GetIsTouching(vecBoundingCentre, fBoundingRadius))
        {
            continue;
        }

        entity->SetCurrentScanCode();
        totalColPointsToProcess = ProcessEntityCollision(physicalEntity, &colPoints[0]);
        if (totalColPointsToProcess > 0) {
            break;
        }
    }

    assert(entity);
    float fThisDamageIntensity = -1.0f;
    float fEntityDamageIntensity = -1.0f;
    auto Process = [&](bool applyFriction = true) {
        if (totalColPointsToProcess > 0) {
            for (int32 ColPointIndex = 0; ColPointIndex < totalColPointsToProcess; ColPointIndex++) {
                CColPoint& ColPoint = colPoints[ColPointIndex];
                if (ApplyCollision(entity, ColPoint, fThisDamageIntensity, fEntityDamageIntensity)) {
                    SetDamagedPieceRecord(fThisDamageIntensity, entity, ColPoint, 1.0f);
                    physicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, this, ColPoint, -1.0f);
                    if (applyFriction) {
                        float SurfaceFriction = g_surfaceInfos.GetAdhesiveLimit(&ColPoint);
                        float Friction = SurfaceFriction / totalColPointsToProcess;
                        if (ApplyFriction(physicalEntity, Friction, ColPoint)) {
                            m_bHasContacted = true;
                            entity->m_bHasContacted = true;
                        }
                    }
                }
            }
        }
    };

    if (m_bHasContacted && entity->m_bHasContacted) {
        Process(false);
    } else if (m_bHasContacted) {
        CVector vecOldFrictionMoveSpeed = m_vecFrictionMoveSpeed;
        CVector vecOldFrictionTurnSpeed = m_vecFrictionTurnSpeed;
        ResetFrictionTurnSpeed();
        ResetFrictionMoveSpeed();
        m_bHasContacted = false;

        Process();
        if (!m_bHasContacted) {
            m_vecFrictionMoveSpeed = vecOldFrictionMoveSpeed;
            m_vecFrictionTurnSpeed = vecOldFrictionTurnSpeed;
            m_bHasContacted = true;
        }
    } else if (entity->m_bHasContacted) {
        assert(physicalEntity);
        CVector vecOldFrictionMoveSpeed = physicalEntity->m_vecFrictionMoveSpeed;
        CVector vecOldFrictionTurnSpeed = physicalEntity->m_vecFrictionTurnSpeed;
        physicalEntity->ResetFrictionTurnSpeed();
        physicalEntity->ResetFrictionMoveSpeed();
        entity->m_bHasContacted = false;

        Process();
        if (!entity->m_bHasContacted) {
            physicalEntity->m_vecFrictionMoveSpeed = vecOldFrictionMoveSpeed;
            physicalEntity->m_vecFrictionTurnSpeed = vecOldFrictionTurnSpeed;
            entity->m_bHasContacted = true;
        }
    } else {
        Process();
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
void CPhysical::AttachEntityToEntity(CPhysical* entityAttachTo, CVector vecAttachOffset, CVector vecAttachRotation)
{
    if (!entityAttachTo)
        return;

    CEntity* oldEntityAttachedTo = m_pAttachedTo;
    m_pAttachedTo = entityAttachTo;
    assert(m_pAttachedTo);
    m_pAttachedTo->RegisterReference(reinterpret_cast<CEntity**>(&m_pAttachedTo));
    m_vecAttachOffset = vecAttachOffset;
    m_vecAttachedEntityRotation = physicalFlags.bInfiniteMass ? GetPosition() : vecAttachRotation;

    m_qAttachedEntityRotation = CQuaternion(0.0f, 0.0f, 0.0f, 0.0f);
    m_pEntityIgnoredCollision = oldEntityAttachedTo;
    if (physicalFlags.bDisableCollisionForce) {
        physicalFlags.bCollidable = false;
        PositionAttachedEntity();
    } else {
        if (m_pAttachedTo->IsPhysical()
            && m_pAttachedTo->physicalFlags.bDisableCollisionForce
            && IsObject() && !physicalFlags.bInfiniteMass)
        {
            physicalFlags.bDisableCollisionForce = true;
            m_fMass = 99999.9f;
            m_fTurnMass = 99999.9f;
        }
        PositionAttachedEntity();
    }
}

// 0x54D690
void CPhysical::AttachEntityToEntity(CPhysical* entityAttachTo, CVector* vecAttachOffset, CQuaternion* attachRotation)
{
    if (!entityAttachTo)
        return;

    CPhysical* oldEntityAttachedTo = m_pAttachedTo;
    m_pAttachedTo = entityAttachTo;
    assert(m_pAttachedTo);
    m_pAttachedTo->RegisterReference(reinterpret_cast<CEntity**>(&m_pAttachedTo));
    CMatrix entityAttachedtoMatrix(m_pAttachedTo->GetMatrix());
    auto* attachedToAutoMobile = m_pAttachedTo->AsAutomobile();
    if (IsObject() && m_pAttachedTo->m_nModelIndex == MODEL_FORKLIFT)
        if (RwFrame* carMiscAFrame = attachedToAutoMobile->m_aCarNodes[CAR_MISC_A])
            entityAttachedtoMatrix.UpdateMatrix(RwFrameGetLTM(carMiscAFrame));

    if (vecAttachOffset) {
        m_vecAttachOffset = *vecAttachOffset;
    }
    else {
        CVector distance = GetPosition() - entityAttachedtoMatrix.GetPosition();
        m_vecAttachOffset.x = entityAttachedtoMatrix.GetRight().Dot(distance);
        m_vecAttachOffset.y = entityAttachedtoMatrix.GetForward().Dot(distance);
        m_vecAttachOffset.z = entityAttachedtoMatrix.GetUp().Dot(distance);
    }

    if (attachRotation) {
        m_qAttachedEntityRotation = *attachRotation;
    }
    else {
        CMatrix entityAttachedToMatrixInverted;
        Invert(entityAttachedtoMatrix, entityAttachedToMatrixInverted);
        entityAttachedToMatrixInverted *= *m_matrix;
        RwMatrixTag rwMatrix{};
        entityAttachedToMatrixInverted.CopyToRwMatrix(&rwMatrix);
        RtQuatConvertFromMatrix((RtQuat*)&m_qAttachedEntityRotation, &rwMatrix);
    }

    if (physicalFlags.bDisableCollisionForce) {
        physicalFlags.bCollidable = false;
    }
    else {
        if (m_pAttachedTo->IsPhysical()
            && m_pAttachedTo->physicalFlags.bDisableCollisionForce
            && IsObject() && !physicalFlags.bInfiniteMass)
        {
            physicalFlags.bDisableCollisionForce = true;
            m_fMass = 99999.9f;
            m_fTurnMass = 99999.9f;
        }
    }

    if (physicalFlags.bInfiniteMass)
        m_vecAttachedEntityRotation = GetPosition();

    m_pEntityIgnoredCollision = oldEntityAttachedTo;
    PositionAttachedEntity();
}

// 0x54D920
bool CPhysical::CheckCollision()
{
    m_bCollisionProcessed = false;
    if (IsPed())
    {
        CPed* ped = AsPed();
        if (!m_pAttachedTo && !physicalFlags.b17
            && !physicalFlags.bProcessingShift && !physicalFlags.bSkipLineCol)
        {
            ped->m_standingOnEntity = nullptr;
            if (ped->bIsStanding) {
                ped->bIsStanding = false;
                ped->bWasStanding = true;
            }
        }

        if (ped->IsPlayer()) {
            CTaskSimpleClimb* taskClimb = ped->m_pIntelligence->GetTaskClimb();
            if (taskClimb) {
                switch (taskClimb->m_nHeightForPos) {
                case CLIMB_GRAB:
                case CLIMB_PULLUP:
                case CLIMB_STANDUP:
                case CLIMB_VAULT:
                    physicalFlags.bSkipLineCol = true;
                    break;
                }
            }
        }
    }

    CWorld::IncrementCurrentScanCode();

    CRect boundRect = GetBoundRect();
    bool collisionDetected = false;
    CWorld::IterateSectorsOverlappedByRect(
        boundRect,
        [this, &collisionDetected](int32 sectorX, int32 sectorY) {
            if (ProcessCollisionSectorList(sectorX, sectorY)) {
                collisionDetected = true;
                return false;
            }
            return true;
        }
    );
    return collisionDetected;
}

// 0x54DAB0
bool CPhysical::CheckCollision_SimpleCar()
{
    m_bCollisionProcessed = false;
    CWorld::IncrementCurrentScanCode();
    CEntryInfoNode* entryInfoNode = m_pCollisionList.m_node;
    if (!entryInfoNode)
        return false;

    while (!ProcessCollisionSectorList_SimpleCar(entryInfoNode->m_repeatSector))
    {
        entryInfoNode = entryInfoNode->m_next;
        if (!entryInfoNode)
            return false;
    }
    return true;
}

// NOTSA
void CPhysical::UnstuckAndReposition() {
    m_bIsStuck = false;
    m_bIsInSafePosition = true;
    RemoveAndAdd();
}

// NOTSA
void CPhysical::ReportCollision(CPhysical* entity1, CEntity* entity2, bool isFirstA, CColPoint& colPoint, float collisionImpact1, float collisionImpact2) {
    const auto surf1 = isFirstA ? colPoint.m_nSurfaceTypeA : colPoint.m_nSurfaceTypeB;
    const auto surf2 = isFirstA ? colPoint.m_nSurfaceTypeB : colPoint.m_nSurfaceTypeA;
    AudioEngine.ReportCollision(entity1, entity2, surf1, surf2, colPoint.m_vecPoint, &colPoint.m_vecNormal, collisionImpact1, collisionImpact2, false, false);
}
