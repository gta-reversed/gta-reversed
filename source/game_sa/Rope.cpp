#include "StdInc.h"

#include "Rope.h"
#include "Ropes.h"

namespace {
constexpr auto PICK_UP_RANGE = 2.5f;

//! The magno crane objects that can be picked up by one of their sides too
bool IsMagnoCraneObjectWithSides(int32 model) {
    using namespace ModelIndices;
    return model == MI_OBJECTFORMAGNOCRANE1
        || model == MI_OBJECTFORMAGNOCRANE2
        || model == MI_OBJECTFORMAGNOCRANE3
        || model == MI_OBJECTFORMAGNOCRANE5;
}
} // namespace

// inlined see 0x557959
// use switch like in Android?
// 0x555FB0
bool CRope::DoControlsApply() const {
    return m_Type == eRopeType::CRANE_MAGNO && CRopes::PlayerControlsCrane == eControlledCrane::MAGNO_CRANE
        || m_Type == eRopeType::WRECKING_BALL && CRopes::PlayerControlsCrane == eControlledCrane::WRECKING_BALL
        || m_Type == eRopeType::CRANE_TROLLEY && CRopes::PlayerControlsCrane == eControlledCrane::LAS_VEGAS_CRANE
        || m_Type == eRopeType::QUARRY_CRANE_ARM && CRopes::PlayerControlsCrane == eControlledCrane::QUARRY_CRANE
        || m_Type == eRopeType::CRANE_MAGNET1
        || m_Type == eRopeType::MAGNET
        || m_Type == eRopeType::CRANE_HARNESS;
}

// 0x556030
void CRope::ReleasePickedUpObject() {
    if (m_CarriedEntity) {
        m_CarriedEntity->AsPhysical()->physicalFlags.bAttachedToEntity = false;
        m_CarriedEntity->AsPhysical()->physicalFlags.bCarriedByRope    = false;
        m_CarriedEntity                                                = nullptr;
    }
    m_HookObject->SetUsesCollision(true);
    m_PickUpCooldown = 60; // 6th, 7th bits set
}

// 0x556070
void CRope::CreateHookObjectForRope() {
    if (m_HookObject) {
        return;
    }

    using namespace ModelIndices;

    const auto modelIndex = [&]() -> ModelIndex {
        switch (m_Type) {
        case eRopeType::CRANE_MAGNET1:
        case eRopeType::CRANE_MAGNO:
        case eRopeType::QUARRY_CRANE_ARM:
        case eRopeType::CRANE_TROLLEY:
            return MI_CRANE_MAGNET;
        case eRopeType::CRANE_HARNESS:
            return MI_CRANE_HARNESS;
        case eRopeType::MAGNET:
            return MI_MINI_MAGNET;
        case eRopeType::WRECKING_BALL:
            return MI_WRECKING_BALL;
        case eRopeType::SWAT:
            return MODEL_INVALID; // Just so the assert below wont be hit.
        default:
            NOTSA_UNREACHABLE(); //assert(0);
        }
    }();
    if (modelIndex == ModelIndex{ MODEL_INVALID }) { // Must do it like this because `ModelIndex` is u16, `MODEL_ID` is i32, and u16 -1 casted to int32 is 0xffff...
        return;
    }

    auto* obj = new CObject(modelIndex, true);
    m_HookObject = obj;

    obj->RegisterReference(reinterpret_cast<CEntity**>(&m_HookObject));
    obj->SetPosn(m_Segments[NUM_ROPE_SEGMENTS - 1]);
    obj->m_nObjectType = OBJECT_TYPE_DECORATION;
    obj->SetIsStatic(false);
    obj->physicalFlags.bAttachedToEntity = true;

    CWorld::Add(m_HookObject);

    m_CarriedEntity  = nullptr;
    m_PickUpCooldown = 0;
}

// 0x5561B0
// Returns true if the rope is stretched - `outWeightPos` is then where the weight has to be
bool CRope::UpdateWeightInRope(CVector weightPos, float unused, CVector* outWeightPos) {
    const CVector top                 = m_Segments[m_NumFixedSegments];
    const auto    ropeLength          = (float)(int32)(NUM_ROPE_SEGMENTS - 1 - m_NumFixedSegments) * m_SegmentLength;
    m_Segments[NUM_ROPE_SEGMENTS - 1] = weightPos;

    const CVector topToWeight         = weightPos - top;
    if (const auto dist = topToWeight.Magnitude(); dist >= ropeLength) { // 0x556266 - Stretched: a straight line from the top
        *outWeightPos      = top + topToWeight * (ropeLength / dist);

        const CVector step = topToWeight.Normalized() * m_SegmentLength; // NOTE: The original normalizes (and scales) it again for every segment
        for (auto i = m_NumFixedSegments + 1u; i < NUM_ROPE_SEGMENTS; i++) {
            m_Segments[i] = top + step * (float)(i - m_NumFixedSegments);
        }
        return true;
    }

    // Pulls `m_Segments[i - 1]` to `m_Segments[i]` if they're too far apart
    const auto PullUpper = [this](uint32 i) {
        const CVector& lower = m_Segments[i];
        auto&          upper = m_Segments[i - 1];
        const auto     dist  = (lower - upper).Magnitude();
        if (dist <= m_SegmentLength) {
            return false;
        }
        upper = lower + (upper - lower) * (m_SegmentLength / dist);
        return true;
    };

    for (auto n = 0; n < 6; n++) {
        // 0x556280 - From the weight up, until a segment isn't stretched
        for (auto i = NUM_ROPE_SEGMENTS - 1; i > m_NumFixedSegments + 1u; i--) {
            const CVector oldPos = m_Segments[i - 1];
            if (!PullUpper(i)) {
                break;
            }
            m_SegmentSpeeds[i - 1] = (m_Segments[i - 1] - oldPos) / CTimer::GetTimeStep();
        }

        // 0x556401 - From the top down
        for (auto i = m_NumFixedSegments + 1u; i < NUM_ROPE_SEGMENTS - 1; i++) {
            const CVector& upper = m_Segments[i - 1];
            auto&          lower = m_Segments[i];
            if (const auto dist = (lower - upper).Magnitude(); dist > m_SegmentLength) {
                lower = upper + (lower - upper) * (m_SegmentLength / dist);
            }
        }
    }

    // 0x55650A - Once more from the weight up, without the speeds
    for (auto i = NUM_ROPE_SEGMENTS - 1; i > m_NumFixedSegments + 1u; i--) {
        if (!PullUpper(i)) {
            break;
        }
    }
    return false;
}

// 0x556780
void CRope::Remove() {
    m_Type = eRopeType::NONE;
    if (m_CarriedEntity) {
        ReleasePickedUpObject();
    }

    if (m_HookObject) {
        CWorld::Remove(m_HookObject);
        delete m_HookObject;
        m_HookObject = nullptr;
    }
}

// 0x556800
void CRope::Render() {
    // Note: Probably needs adjustments if `NUM_ROPE_SEGMENTS` is changed
    if (!TheCamera.IsSphereVisible(m_Segments[NUM_ROPE_SEGMENTS / 2], 20.0f)) {
        return;
    }

    if ((TheCamera.GetPosition() - m_Segments[0]).Magnitude2D() >= 120.0f) {
        return;
    }

    DefinedState();

    const auto GetVertex = [](unsigned i) {
        return &TempBufferVertices.m_3d[i];
    };

    const RwRGBA color = { 0, 0, 0, 128 };
    for (auto i = 0u; i < NUM_ROPE_SEGMENTS; i++) {
        RxObjSpace3DVertexSetPreLitColor(GetVertex(i), &color);
        RxObjSpace3DVertexSetPos(GetVertex(i), &m_Segments[i]);
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(FALSE));

    if (RwIm3DTransform(TempBufferVertices.m_3d, NUM_ROPE_SEGMENTS, nullptr, 0)) {
        RxVertexIndex indices[] = { // *(RxVertexIndex(*)[64])0x8CD818
            0,  1,  1,  2,  2,  3,  3,  4,
            4,  5,  5,  6,  6,  7,  7,  8,
            8,  9,  9,  10, 10, 11, 11, 12,
            12, 13, 13, 14, 14, 15, 15, 16,
            16, 17, 17, 18, 18, 19, 19, 20,
            20, 21, 21, 22, 22, 23, 23, 24,
            24, 25, 25, 26, 26, 27, 27, 28,
            28, 29, 29, 30, 30, 31, 31, 32
        };
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, std::size(indices) - 2); // the last two indexes are not used
        RwIm3DEnd();
    }

    if (m_Type == eRopeType::QUARRY_CRANE_ARM) {
        const CVector pos[] = {
            m_Segments[0], { 709.32f, 916.20f, 53.0f }
        }; // Hunter Quarry
        for (auto i = 0u; i < std::size(pos); i++) {
            RxObjSpace3DVertexSetPreLitColor(GetVertex(i), &color);
            RxObjSpace3DVertexSetPos(GetVertex(i), &pos[i]);
        }
        if (RwIm3DTransform(TempBufferVertices.m_3d, std::size(pos), nullptr, 0)) {
            RxVertexIndex indices[] = { 0, 1 };
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, std::size(indices));
            RwIm3DEnd();
        }
    }
}

// 0x5569C0
void CRope::PickUpObject(CEntity* obj) {
    if (m_CarriedEntity == obj) {
        return;
    }

    if (m_CarriedEntity) {
        ReleasePickedUpObject();
    }

    m_CarriedEntity = obj;
    obj->RegisterReference(&m_CarriedEntity);

    // TODO: Move model => world space translation into CEntity
    // MultiplyMatrixWithVector should be used here
    CVector height = { {}, {}, CRopes::FindPickupHeight(obj) };
    m_HookObject->SetPosn(obj->GetPosition() + obj->GetMatrix().TransformVector(height));
    m_HookObject->SetUsesCollision(false);

    obj->AsPhysical()->physicalFlags.bAttachedToEntity = true;
    if (obj->GetIsTypeVehicle()) {
        if (obj->GetStatus() == STATUS_SIMPLE)
        {
            obj->SetStatus(STATUS_PHYSICS);
        }
    } else if (obj->GetIsTypeObject()) {
        if (obj->GetIsStatic()) {
            obj->AsObject()->SetIsStatic(false);
            obj->AsObject()->AddToMovingList();
            obj->AsObject()->m_nFakePhysics = 0;
        }
    }
}

// NOTSA
bool CRope::IsCrane() const {
    switch (m_Type) {
    case eRopeType::CRANE_MAGNO:
    case eRopeType::WRECKING_BALL:
    case eRopeType::QUARRY_CRANE_ARM:
    case eRopeType::CRANE_TROLLEY:
        return true;
    default:
        return false;
    }
}

// 0x557530
void CRope::Update() {
    const auto damping = std::pow(0.8f, CTimer::GetTimeStep());
    if ((TheCamera.GetPosition() - m_Segments[0]).SquaredMagnitude2D() >= sq(200.f)) {
        return;
    }

    // 0x5575A8 - Not held by anything any more: the top falls down
    if (!m_IsRegistered && CTimer::GetTimeInMS() > m_ExpireTime) {
        m_SegmentSpeeds[0].z -= CTimer::GetTimeStep() * 0.0015f;
        m_Segments[0] += m_SegmentSpeeds[0] * CTimer::GetTimeStep();
    }

    if (m_HitsGround) {
        if ((CTimer::GetFrameCounter() & 7) == 2) { // 0x55760C
            m_GroundZ = CWorld::FindGroundZFor3DCoord(m_Segments[0]);
        }
        if (const auto* const carried = m_CarriedEntity; carried && (carried->GetIsTypeVehicle() || IsMagnoCraneObjectWithSides(carried->GetModelIndex()))) { // 0x55763B
            m_GroundZ = m_HookObject->GetPosition().z - 0.5f;
        }
    }

    // 0x5576AD - The free segments: gravity, ground, and keep the segment length
    float staleZ{}; // NOTE: Only for the original's bug in `PickUpPed`
    for (auto i = m_NumFixedSegments + 1u; i < NUM_ROPE_SEGMENTS; i++) {
        auto&         segment = m_Segments[i];
        auto&         speed   = m_SegmentSpeeds[i];
        const CVector oldPos  = segment;

        // NOTE: The speed calculated here is overwritten below, only the random numbers it uses up matter
        speed.x += (float)CGeneral::GetRandomNumberInRange(-8, 8) * 0.001f;
        speed.y += (float)CGeneral::GetRandomNumberInRange(-8, 8) * 0.001f;
        speed = m_SegmentSpeeds[i - 1] * (1.f - damping) + speed * damping;

        segment.z -= CTimer::GetTimeStep() * 0.15f;
        if (m_HitsGround) {
            segment.z = std::max(segment.z, m_GroundZ + 0.3f);
        }
        const CVector& upper = m_Segments[i - 1];
        segment              = upper + (segment - upper) * (m_SegmentLength / (segment - upper).Magnitude());

        speed                = (segment - oldPos) / CTimer::GetTimeStep();
        staleZ               = oldPos.z;
    }

    if (m_Type != eRopeType::NONE && m_Type != eRopeType::SWAT) { // 0x55792D
        ProcessControls();
        if (const auto weightSpeedZ = ProcessWeight()) {
            staleZ = *weightSpeedZ;
        }
        ProcessPickUp(staleZ);
    }

    // 0x558CA3
    if (!m_IsRegistered && m_Segments[0].z < -50.f) {
        Remove();
    }
    m_IsRegistered = false;
}

// NOTSA (0x55792D) - The player / pad lets the rope down or pulls it up
void CRope::ProcessControls() {
    if (DoControlsApply()) {
        switch (m_Type) {
        case eRopeType::CRANE_MAGNET1:
        case eRopeType::CRANE_HARNESS:
        case eRopeType::MAGNET:        { // 0x557B04
            const auto upDown = CPad::GetPad(0)->GetCarGunUpDown();
            if ((upDown < 0 && CTheScripts::bEnableCraneRaise) || (upDown > 0 && CTheScripts::bEnableCraneLower)) {
                m_Height -= (float)upDown * CTimer::GetTimeStep() * 0.00001f;
            }
            m_Height = std::min(m_Height, 0.84f);
            break;
        }
        default: { // 0x5579AD - A crane the player controls
            const auto ReportWinchMove = [this] {
                AudioEngine.ReportMissionAudioEvent(AE_CRANE_WINCH_MOVE, m_Holder->AsPhysical(), 0.f, 1.f);
            };
            if (CTheScripts::bEnableCraneRaise) {
                const auto change = (float)CPad::GetPad(0)->NewState.ButtonSquare * CTimer::GetTimeStep() * 0.00001f;
                if (change > 0.f && m_Height + change < 0.9f) {
                    ReportWinchMove();
                }
                m_Height += change;
            }
            if (CTheScripts::bEnableCraneLower) { // 0x557A58
                const auto change = (float)CPad::GetPad(0)->NewState.ButtonCross * CTimer::GetTimeStep() * 0.00001f;
                if (change > 0.f && m_Height - change > 0.01f) {
                    ReportWinchMove();
                }
                m_Height -= change;
            }
            break;
        }
        }
    }
    m_Height = std::clamp(m_Height, 0.01f, 0.9f); // 0x557B87
}

// NOTSA (0x557BC1) - The weight (the carried entity, or the hook object) swings on the rope
// Returns the Z of the weight's speed change if the rope is stretched (see `PickUpPed`)
std::optional<float> CRope::ProcessWeight() {
    auto* const carried = m_CarriedEntity ? m_CarriedEntity->AsPhysical() : nullptr;
    auto* const hook    = m_HookObject->AsPhysical();
    auto* const holder  = m_Holder->AsPhysical();

    // How much of the speed change goes to the holder
    const auto holderShare = [&] {
        if (!carried) {
            hook->SetUsesCollision(true);
            return 0.06f;
        }
        carried->m_nFakePhysics = 0;
        const auto mass         = carried->GetModelIndex() == MODEL_SECURICA ? 750.f : carried->m_fMass;
        return std::min(0.06f + 0.13f * (mass / 1200.f), 0.5f);
    }();

    auto* const weight = carried ? carried : hook;
    if (!weight) {
        return std::nullopt;
    }

    std::optional<float> weightSpeedZ{};
    if (CVector weightPos; UpdateWeightInRope(weight->GetPosition(), 0.1f, &weightPos)) { // 0x557C89
        weight->SetPosn(weightPos);

        const CVector holderSpeed = IsCrane() ? CVector{} : holder->GetSpeed(m_Segments[0] - holder->GetPosition());
        const CVector dir         = (weight->GetPosition() - m_Segments[0]).Normalized();

        // Remove the part of the speed that goes away from the top of the rope
        CVector speed = weight->GetMoveSpeed() - holderSpeed;
        if (const auto outwards = speed.Dot(dir); outwards > 0.f) {
            speed -= dir * outwards;
        }
        weightSpeedZ              = speed.z;

        const CVector speedChange = speed + holderSpeed - weight->GetMoveSpeed();
        weight->GetMoveSpeed() += speedChange * (1.f - holderShare);
        holder->GetMoveSpeed() -= speedChange * holderShare;

        if (carried) { // 0x557F18 - Turn it slowly to hang from the rope
            auto&         mat = weight->GetMatrix();
            const CMatrix oldMat{ mat };
            mat.ForceUpVector(-dir);
            const CMatrix newMat{ mat };
            mat.GetRight()   = oldMat.GetRight() * 0.9f + newMat.GetRight() * 0.1f;
            mat.GetForward() = oldMat.GetForward() * 0.9f + newMat.GetForward() * 0.1f;
            mat.GetUp()      = oldMat.GetUp() * 0.9f + newMat.GetUp() * 0.1f;
        } else {
            weight->GetMatrix().ForceUpVector(-dir);
        }
    }

    if (carried) { // 0x558121 - The hook object goes with the carried entity
        hook->GetMatrix()    = carried->GetMatrix();
        hook->GetMoveSpeed() = carried->GetMoveSpeed();
        hook->SetPosn(carried->GetMatrix().TransformPoint({ 0.f, 0.f, CRopes::FindPickupHeight(carried) }));
    }
    return weightSpeedZ;
}

// NOTSA (0x5581CA) - Drop the carried entity, or look for something to pick up
void CRope::ProcessPickUp(float staleZ) {
    const CVector lastSegment = m_Segments[NUM_ROPE_SEGMENTS - 1];

    if (m_CarriedEntity) {
        if (DoControlsApply() && CTheScripts::bEnableCraneRelease && CPad::GetPad(0)->CarGunJustDown()) {
            ReleasePickedUpObject();
        }
        if (m_CarriedEntity && m_CarriedEntity->AsPhysical()->physicalFlags.bRenderScorched) { // 0x558254 - Blown up
            ReleasePickedUpObject();
        }
        if (m_CarriedEntity && m_CarriedEntity->GetIsTypeVehicle()) { // 0x558293
            if (const auto* const vehicle = m_CarriedEntity->AsVehicle(); vehicle->IsSubBike() && vehicle->GetDriver()) {
                ReleasePickedUpObject();
            }
        }
        return;
    }

    if (m_PickUpCooldown) { // 0x5582D4 - Just dropped something
        m_PickUpCooldown--;
        return;
    }

    switch (m_Type) { // 0x558324
    case eRopeType::CRANE_MAGNET1:
        PickUpVehicle(lastSegment, false);
        break;
    case eRopeType::CRANE_HARNESS:
        PickUpPed(lastSegment, staleZ);
        break;
    case eRopeType::MAGNET:
    case eRopeType::CRANE_MAGNO:
    case eRopeType::QUARRY_CRANE_ARM:
        if (!PickUpVehicle(lastSegment, m_Type == eRopeType::MAGNET)) {
            PickUpRopeObject(lastSegment, m_Type);
        }
        break;
    case eRopeType::CRANE_TROLLEY:
        PickUpRopeObject(lastSegment, m_Type);
        break;
    case eRopeType::WRECKING_BALL:
        break;
    default:
        NOTSA_UNREACHABLE();
    }
}

// NOTSA (0x558555) - Let the rope down (or pull it up) by how far the pick up point is below the end of the rope
void CRope::LetDownForPickUp(float heightAbovePickUpPoint) {
    if (heightAbovePickUpPoint > 0.f) {
        m_Height = std::max(m_Height - heightAbovePickUpPoint / m_TotalLength - m_SegmentLength / m_TotalLength, 0.01f);
    }
}

// NOTSA (0x55835C)
bool CRope::PickUpVehicle(const CVector& lastSegment, bool onlyRcTiger) {
    auto& pool = *GetVehiclePool();
    for (auto i = pool.GetSize(); i-- > 0;) {
        auto* const vehicle = pool.GetAt(i);
        if (!vehicle) {
            continue;
        }
        const auto model = vehicle->GetModelIndex();
        if (!vehicle->IsSubAutomobile() && !(vehicle->IsSubBike() && !vehicle->GetDriver()) && !vehicle->IsSubMonsterTruck() && model != MODEL_DINGHY && model != MODEL_VORTEX) {
            continue;
        }
        if (vehicle->physicalFlags.bRenderScorched || !vehicle->vehicleFlags.bWinchCanPickMeUp || (onlyRcTiger && model != MODEL_RCTIGER) || vehicle->m_ropeType) {
            continue;
        }
        const CVector pickUpPos = vehicle->GetMatrix().TransformPoint({ 0.f, 0.f, CRopes::FindPickupHeight(vehicle) });
        if ((pickUpPos - lastSegment).SquaredMagnitude() >= sq(PICK_UP_RANGE)) {
            continue;
        }

        // 0x55849B
        m_CarriedEntity = vehicle;
        vehicle->RegisterReference(&m_CarriedEntity);
        vehicle->physicalFlags.bCarriedByRope = true;
        if (vehicle->GetStatus() == STATUS_SIMPLE) {
            vehicle->SetStatus(STATUS_PHYSICS);
        }
        m_HookObject->SetPosn(pickUpPos);
        m_HookObject->SetUsesCollision(false);
        LetDownForPickUp(lastSegment.z - pickUpPos.z);
        return true;
    }
    return false;
}

// NOTSA (0x5585A4)
void CRope::PickUpPed(const CVector& lastSegment, float staleZ) {
    auto& pool = *GetPedPool();
    for (auto i = pool.GetSize(); i-- > 0;) {
        auto* const ped = pool.GetAt(i);
        if (!ped || ped->GetPedState() == PEDSTATE_DEAD || ped->IsPlayer() || ped->IsInVehicle()) { // NOTE: The original checks only `bInVehicle`
            continue;
        }
        if ((ped->GetPosition() - lastSegment).SquaredMagnitude() >= sq(PICK_UP_RANGE)) {
            continue;
        }

        // 0x55866D
        m_CarriedEntity = ped;
        ped->RegisterReference(&m_CarriedEntity);
        ped->physicalFlags.bCarriedByRope = true;
        const CVector pickUpPos           = ped->GetMatrix().TransformPoint({ 0.f, 0.f, CRopes::FindPickupHeight(ped) });
        m_HookObject->SetPosn(pickUpPos);
        m_HookObject->SetUsesCollision(false);

        // NOTE/BUG: The original doesn't set the pick up point here (0x5586DB), it uses whatever its local still holds
        //           (the Z of the weight's speed change, or the old Z of the last segment)
        LetDownForPickUp(lastSegment.z - (notsa::bugfixes::GenericUB ? pickUpPos.z : staleZ));
        return;
    }
}

// NOTSA (0x55876F) - The objects of the scripts' crane missions
void CRope::PickUpRopeObject(const CVector& lastSegment, eRopeType type) {
    using namespace ModelIndices;

    const auto CanPickUp = [type](const CObject& obj) {
        const auto model = obj.GetModelIndex();
        switch (type) {
        case eRopeType::CRANE_MAGNO:
            return IsMagnoCraneObjectWithSides(model) || model == MI_OBJECTFORMAGNOCRANE4;
        case eRopeType::CRANE_TROLLEY:
            return model == MI_OBJECTFORBUILDINGSITECRANE1;
        case eRopeType::QUARRY_CRANE_ARM:
            return (model == MI_QUARY_ROCK1 || model == MI_QUARY_ROCK2 || model == MI_QUARY_ROCK3 || model == MI_DEAD_TIED_COP) && !obj.m_pAttachedTo;
        case eRopeType::MAGNET:
            return model == MI_WONG_DISH || model == MI_KMB_ROCK || model == MI_KMB_PLANK || model == MI_KMB_BOMB;
        default:
            NOTSA_UNREACHABLE();
        }
    };

    auto& pool = *GetObjectPool();
    for (auto i = pool.GetSize(); i-- > 0;) {
        auto* const obj = pool.GetAt(i);
        if (!obj || !obj->objectFlags.bCanBeAttachedToMagnet || !CanPickUp(*obj)) {
            continue;
        }
        const auto  height = CRopes::FindPickupHeight(obj);
        const auto& mat    = obj->GetMatrix();

        // 0x558914 - The top, or the highest side
        CVector pickUpPos = mat.TransformPoint({ 0.f, 0.f, height });
        uint32  numTurns  = 0;
        if (IsMagnoCraneObjectWithSides(obj->GetModelIndex())) {
            for (const auto& [offset, turns] : {
                     std::pair{ CVector{ 0.f, 0.f, -height }, 2u },
                     { CVector{ height, 0.f, 0.f },  3u },
                     { CVector{ -height, 0.f, 0.f }, 1u }
            }) {
                if (const CVector pos = mat.TransformPoint(offset); pos.z > pickUpPos.z) {
                    pickUpPos = pos;
                    numTurns  = turns;
                }
            }
        }
        if ((pickUpPos - lastSegment).SquaredMagnitude() >= sq(PICK_UP_RANGE)) {
            continue;
        }

        // 0x558B35
        m_CarriedEntity = obj;
        obj->RegisterReference(&m_CarriedEntity);
        obj->physicalFlags.bCarriedByRope = true;
        m_HookObject->SetPosn(mat.TransformPoint({ 0.f, 0.f, height })); // NOTE: The top, not `pickUpPos`
        m_HookObject->SetUsesCollision(false);
        if (obj->GetIsStatic()) {
            obj->SetIsStatic(false);
            obj->AddToMovingList();
        }
        obj->physicalFlags.bAttachedToEntity = true;
        LetDownForPickUp(lastSegment.z - pickUpPos.z);

        // 0x558C2E - Turn the picked up side to the top
        for (auto n = 0u; n < numTurns; n++) {
            auto&         objMat = obj->GetMatrix();
            const CVector right  = objMat.GetRight();
            objMat.GetRight()    = objMat.GetUp();
            objMat.GetUp()       = -right;
        }
        return;
    }
}

void CRope::InjectHooks() {
    RH_ScopedClass(CRope);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ReleasePickedUpObject, 0x556030);
    RH_ScopedInstall(CreateHookObjectForRope, 0x556070);
    RH_ScopedInstall(UpdateWeightInRope, 0x5561B0);
    RH_ScopedInstall(Remove, 0x556780);
    RH_ScopedInstall(Render, 0x556800);
    RH_ScopedInstall(PickUpObject, 0x5569C0);
    RH_ScopedInstall(Update, 0x557530);
}
