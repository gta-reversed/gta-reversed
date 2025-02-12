#include "StdInc.h"
#include "TaskSimpleBikeJacked.h"
#include "TaskUtilityLineUpPedWithCar.h"

void CTaskSimpleBikeJacked::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleBikeJacked, 0x86EFAC, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x648B90);
    RH_ScopedInstall(Destructor, 0x648C40);

    RH_ScopedGlobalInstall(FinishAnimBikeHitCB, 0x648D30);

    RH_ScopedVMTInstall(Clone, 0x64A200);
    RH_ScopedVMTInstall(GetTaskType, 0x648C30);
    RH_ScopedVMTInstall(MakeAbortable, 0x648CE0);
    RH_ScopedVMTInstall(ProcessPed, 0x64C970);
    RH_ScopedVMTInstall(SetPedPosition, 0x648D00);
}

// 0x648B90
CTaskSimpleBikeJacked::CTaskSimpleBikeJacked(CVehicle* vehicle, uint32 door, uint32 time, CPed* carJacker, bool isVictimDriver) :
    m_vehicle{vehicle},
    m_door{door},
    m_time{time},
    m_isVictimDriver{isVictimDriver}
{
    CEntity::SafeRegisterRef(m_vehicle);
    CEntity::SafeRegisterRef(m_jacker);
}

CTaskSimpleBikeJacked::CTaskSimpleBikeJacked(const CTaskSimpleBikeJacked& o) :
    CTaskSimpleBikeJacked{o.m_vehicle, o.m_door, o.m_time, o.m_jacker, o.m_isVictimDriver}
{
}

// 0x648C40
CTaskSimpleBikeJacked::~CTaskSimpleBikeJacked() {
    CEntity::SafeCleanUpRef(m_vehicle);
    CEntity::SafeCleanUpRef(m_jacker);
    if (m_taskUtilityLineUpPedWithCar) {
        delete m_taskUtilityLineUpPedWithCar;
    }
    if (m_firstAnim) {
        m_firstAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x648CE0
bool CTaskSimpleBikeJacked::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_firstAnim) {
            m_firstAnim->m_BlendDelta = -1000.f;
        }
        return true;
    }
    return false;
}

void CTaskSimpleBikeJacked::FinishAnimBikeHitCB(CAnimBlendAssociation* anim, void* data) {
    auto self = static_cast<CTaskSimpleBikeJacked*>(data);
    self->m_firstAnim = nullptr;
    self->m_animWasPlayed = true;
    self->m_secondAnimId = [self] {
        switch (self->m_door) {
        case CAR_DOOR_RF:
        case CAR_DOOR_LF:
            return ANIM_ID_BIKE_FALL_OFF;
        default:
            return ANIM_ID_BIKE_FALLR;
        }
    }();
}

// 0x64C970
bool CTaskSimpleBikeJacked::ProcessPed(CPed* ped) {
    if (!m_vehicle) {
        return true;
    }

    if (!ped->bInVehicle && !m_firstAnim) {
        return true;
    }

    if (!m_animWasPlayed) {
        // Inverted + refactored a little
        if (ped->IsPlayer()) {
            m_vehicle->m_vehicleAudio.PlayerAboutToExitVehicleAsDriver();
        } else if (m_vehicle->IsDriver(ped)) {
            ped->SetRadioStation();
        }
        
        if (!m_firstAnim &&
            [this, ped] {
                if (m_jacker) {
                    if (const auto anim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, { ANIM_ID_CAR_PULLOUT_RHS, ANIM_ID_CAR_PULLOUT_LHS, ANIM_ID_CAR_GETIN_BIKE_FRONT })) {
                        if (anim->m_CurrentTime <= 0.3f) {
                            return false;
                        }
                    }

                    if (CGeneral::DoCoinFlip()) { // Originally: rand() % 1024 <= 512
                        if (!SayJacking(m_jacker, ped, m_vehicle)) {
                            SayJacked(ped, m_vehicle);
                        }
                    }
                    else if (!SayJacked(ped, m_vehicle)) {
                        SayJacking(m_jacker, ped, m_vehicle);
                    }
                }
                return true;
            }()
        ) {
            // Play animation and some sound effect
            m_firstAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, m_vehicle->GetRideAnimData()->AnimGroup, ANIM_ID_BIKE_HIT);
            m_firstAnim->SetFinishCallback(FinishAnimBikeHitCB, this);
            ped->GetAE().AddAudioEvent(AE_PED_JACKED_BIKE);
        }

        if (!m_taskUtilityLineUpPedWithCar) {
            m_taskUtilityLineUpPedWithCar = new CTaskUtilityLineUpPedWithCar{ {}, 0, 0, (int32)m_door };
        }

        return false;
    }

    if (m_secondAnimId == ANIM_ID_NO_ANIMATION_SET) {
        return true;
    }

    CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, m_secondAnimId);

    // event.m_flags |= 2; => event.m_forceKnockOff = true => Already set by ctor
    ped->GetEventGroup().Add(CEventKnockOffBike{ m_vehicle, &m_vehicle->GetMoveSpeed(), &m_vehicle->m_vecLastCollisionImpactVelocity, 0.f, 0.f, 55u, 0u, (int32)m_time, m_jacker, m_isVictimDriver, true }, true);

    m_secondAnimId = ANIM_ID_NO_ANIMATION_SET;

    return false;
}

// 0x648D00
bool CTaskSimpleBikeJacked::SetPedPosition(CPed* ped) {
    if (ped->IsInVehicle()) {
        ped->SetPedPositionInCar();
    }
    return true;
}
