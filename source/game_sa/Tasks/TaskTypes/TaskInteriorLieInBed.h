#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

class CAnimBlendAssociation;
class CPed;
class CEvent;
class CTaskInteriorLieInBed;
struct InteriorInfo_t;

class NOTSA_EXPORT_VTABLE CTaskInteriorLieInBed : public CTaskSimple {
    enum class AnimSeqIdx {
        GET_IN,
        LOOP,
        GET_OUT
    };
public:
    int32                  m_getOutAfterInterval{};           /// Time after which we should get out of the bed and finish the task
    InteriorInfo_t*        m_intInfo{};                       /// The interior's info we're in
    bool                   m_rightHandSide{};                 /// Originally left uninitalized (bug), but is actually used in `Clone`.
    bool                   m_instant{};                       /// Whenever the ped should be set into the bed instantly
    CAnimBlendAssociation* m_anim{};                          /// The currently playing animation
    AnimationId            m_prevAnimId{ ANIM_ID_UNDEFINED }; /// The previously played animation's ID
    CTaskTimer             m_getOutTimer{};                   /// Duration is `m_getOutAfterInterval`. After it expired the `GET_OUT` anim is played
    bool                   m_updatePedPos{};                  /// Whenever the ped's position should be updated (to reflect the animation)
    bool                   m_lastAnimFinished{};              /// Set when the last animation in the sequence (`GET_OUT`) has finished
    bool                   m_taskAborting{};                  /// `MakeAbortable` was called, but not with IMMIDIATE mode, so we gotta gradually abort the task
    AnimationId            m_baseAnimId{};                    /// The base animation ID (That is, the first animation in the sequence: `GET_IN`)

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_INTERIOR_LIE_IN_BED;

    CTaskInteriorLieInBed(
        int32 duration,
        InteriorInfo_t* intInfo,
        bool inBedRightSide,
        bool doInstantly
    );
    CTaskInteriorLieInBed(const CTaskInteriorLieInBed&);
    ~CTaskInteriorLieInBed();

    static void FinishAnimCB(CAnimBlendAssociation*, void*);

    CTask*    Clone() override { return new CTaskInteriorLieInBed{*this}; }
    eTaskType GetTaskType() override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) override;
    bool      ProcessPed(CPed* ped) override;

    /// Helper function to get rid of enum arithmetic
    AnimationId GetAnimIdInSeq(AnimSeqIdx sequenceIdx);

private: // Wrappers for hooks
    // 0x675E20
    CTaskInteriorLieInBed* Constructor(int32 a2, InteriorInfo_t* a3, uint8 a4, uint8 a5) {
        this->CTaskInteriorLieInBed::CTaskInteriorLieInBed(a2, a3, a4, a5);
        return this;
    }
    // 0x675E90
    CTaskInteriorLieInBed* Destructor() {
        this->CTaskInteriorLieInBed::~CTaskInteriorLieInBed();
        return this;
    }
};
