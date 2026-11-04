#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

class  CAnimBlendAssociation;
class  CPed;
class  CEvent;
struct InteriorInfo_t;

class NOTSA_EXPORT_VTABLE CTaskInteriorLieInBed : public CTaskSimple {
    enum class AnimSeqIdx {
        GET_IN,
        LOOP,
        GET_OUT
    };

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

    CTask*    Clone() const override { return new CTaskInteriorLieInBed{*this}; }
    eTaskType GetTaskType() const override { return Type; }
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

private:
    int32                  m_GetOutAfterInterval{};           //< Time after which we should get out of the bed and finish the task
    InteriorInfo_t*        m_IntInfo{};                       //< The interior's info we're in
    bool                   m_bRghtHandSide{};                 //< Originally left uninitalized (bug), but is actually used in `Clone`.
    bool                   m_bDoInstantly{};                  //< Whenever the ped should be set into the bed instantly
    CAnimBlendAssociation* m_Anim{};                          //< The currently playing animation
    AnimationId            m_PrevAnimId{ ANIM_ID_UNDEFINED }; //< The previously played animation's ID
    CTaskTimer             m_GetOutTimer{};                   //< Duration is `m_getOutAfterInterval`. After it expired the `GET_OUT` anim is played
    bool                   m_UpdatePedPos{};                  //< Whenever the ped's position should be updated (to reflect the animation)
    bool                   m_LastAnimFinished{};              //< Set when the last animation in the sequence (`GET_OUT`) has finished
    bool                   m_TaskAborting{};                  //< `MakeAbortable` was called, but not with IMMIDIATE mode, so we gotta gradually abort the task
    AnimationId            m_BaseAnimId{};                    //< The base animation ID (That is, the first animation in the sequence: `GET_IN`)
};
