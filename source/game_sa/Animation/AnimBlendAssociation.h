/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AnimAssociationData.h"
#include "eAnimBlendCallbackType.h"
#include "RenderWare.h"

#include <extensions/WEnum.hpp>

class CAnimBlendNode;
class CAnimBlendHierarchy;
class CAnimBlendStaticAssociation;

enum eAnimationFlags {
    ANIMATION_DEFAULT                = 0,      //0x0,
    ANIMATION_IS_PLAYING             = 1 << 0, //0x1,
    ANIMATION_IS_LOOPED              = 1 << 1, //0x2,
    ANIMATION_IS_BLEND_AUTO_REMOVE   = 1 << 2, //0x4,
    ANIMATION_IS_FINISH_AUTO_REMOVE  = 1 << 3, //0x8,  // Animation will be stuck on last frame, if not set
    ANIMATION_IS_PARTIAL             = 1 << 4, //0x10, // TODO: Flag name is possibly incorrect? Following the usual logic (like `ANIMATION_MOVEMENT`), it should be `ANIMATION_GET_IN_CAR` (See  `RemoveGetInAnims`)
    ANIMATION_IS_SYNCRONISED         = 1 << 5, //0x20,
    ANIMATION_CAN_EXTRACT_VELOCITY   = 1 << 6, //0x40,
    ANIMATION_CAN_EXTRACT_X_VELOCITY = 1 << 7, //0x80,

    // ** User defined flags **
    ANIMATION_WALK                = 1 << 8,  //0x100,
    ANIMATION_200                 = 1 << 9,  //0x200,
    ANIMATION_DONT_ADD_TO_PARTIAL_BLEND        = 1 << 10, //0x400, // Possibly should be renamed to ANIMATION_IDLE, see `CPed::PlayFootSteps()`
    ANIMATION_800                 = 1 << 11, //0x800,
    ANIMATION_SECONDARY_TASK_ANIM = 1 << 12, //0x1000,
    // **

    ANIMATION_IGNORE_ROOT_TRANSLATION = 1 << 13, //0x2000,
    ANIMATION_REFERENCE_BLOCK         = 1 << 14, //0x4000,
    ANIMATION_FACIAL                  = 1 << 15, //0x8000 // The animation is never destroyed if this flag is set, NO MATTER WHAT
};

class CDefaultAnimCallback {
public:
    static void DefaultAnimCB(class CAnimBlendAssociation* animAssoc, void* something) {
        // nothing here
    }
};

class CAnimBlendLink {
public:
    template<typename Y>
    struct BaseIterator {
    public:
        using iterator_category = std::forward_iterator_tag; // Actually it's bidirectional, but there are quirks, so let's pretend like its not
        using difference_type   = std::ptrdiff_t;
        using value_type        = Y;
        using pointer           = Y*;
        using reference         = Y&;

        BaseIterator() = default;
        BaseIterator(CAnimBlendLink* link) : m_Link{ link } {}

        reference operator*() const { return *DeRefLink(); }
        pointer operator->() { return DeRefLink(); }

        auto& operator++() { assert(m_Link); m_Link = DeRefLink()->GetLink().next; return *this; }
        auto  operator++(int) { const auto tmp{ *this }; ++(*this); return tmp; }

        friend bool operator==(const BaseIterator<Y>& lhs, const BaseIterator<Y>& rhs) { return lhs.m_Link == rhs.m_Link; }
        friend bool operator!=(const BaseIterator<Y>& lhs, const BaseIterator<Y>& rhs) { return !(lhs == rhs); }
    private:
        auto DeRefLink() const { return CAnimBlendAssociation::FromLink(m_Link); }
    private:
        CAnimBlendLink* m_Link;
    };

    using iterator       = BaseIterator<CAnimBlendAssociation>;
    using const_iterator = BaseIterator<const CAnimBlendAssociation>;

    CAnimBlendLink* next{};
    CAnimBlendLink* prev{};

    CAnimBlendLink() = default;

    void Init() {
        next = nullptr;
        prev = nullptr;
    }

    void Prepend(CAnimBlendLink* link) {
        if (next) {
            next->prev = link;
        }
        link->next = next;
        link->prev = this;
        next = link;
    }

    void Remove() {
        if (prev) {
            prev->next = next;
        }
        if (next) {
            next->prev = prev;
        }
        Init();
    }

    bool IsEmpty() const { return !next; }

    auto begin() { return iterator{this}; }
    auto end() { return iterator{nullptr}; }
};

/*!
 * @brief Running animation of a clump (Usually peds), created from an `CAnimBlendHierarchy`.
 * 
 * @detail The sequence/frames data is copied from `CAnimBlendHierarchy` to `CAnimBlendAssociation` when a clump requests an animation.
 * @detail The instance of `CAnimBlendAssociation` gets destroyed when the ped/clump stops playing the animation.
 * @detail But `CAnimBlendHierarchy` is never destroyed and stays in memory unless `CStreaming` forces the IFP to unload (to create space in memory)
 *
 * @detail A clump can have one, or more, instances of this class. Usually there's only 1 primary animation,
 * @detail but there are also partial animations, which can be played alongside primary animations, like hand gestures or smoking.
 */
class NOTSA_EXPORT_VTABLE CAnimBlendAssociation {
public:
    CAnimBlendLink                m_Link;          //!< Link to the next association of the clump
    uint16                        m_NumBlendNodes; //!< Number of bones this anim moves
    notsa::WEnumS16<AssocGroupId> m_AnimGroupId;   //!< Anim's group
    CAnimBlendNode*               m_BlendNodes;    //!< Node per-node animations - NOTE: Order of these depends on order of nodes in Clump this was built from
    CAnimBlendHierarchy*          m_BlendHier;     //!< The animation hierarchy this association was created from
    float                         m_BlendAmount;   //!< How much this animation is blended
    float                         m_BlendDelta;    //!< How much `BlendAmount` changes over time
    float                         m_CurrentTime;   //!< Current play time
    float                         m_Speed;         //!< Play speed
    float                         m_TimeStep;      //!< Time-per-tick
    notsa::WEnumS16<AnimationId>  m_AnimId;        //!< Anim's ID
    uint16                        m_Flags;         //!< Flags

    // Callback shit
    eAnimBlendCallbackType m_nCallbackType;
    void (*m_pCallbackFunc)(CAnimBlendAssociation*, void*);
    void* m_pCallbackData;

public:
    CAnimBlendAssociation();
    CAnimBlendAssociation(RpClump* clump, CAnimBlendHierarchy* animHierarchy);
    CAnimBlendAssociation(CAnimBlendAssociation& assoc);
    explicit CAnimBlendAssociation(CAnimBlendStaticAssociation& assoc);

    virtual ~CAnimBlendAssociation();

    float GetTimeProgress()                  const;
    void  SetBlendAmount(float a)                  { m_BlendAmount = a; }
    float GetBlendAmount(float weight = 1.f) const { return IsPartial() ? m_BlendAmount : m_BlendAmount * weight; }
    float GetBlendDelta()                    const { return m_BlendDelta; }

    AnimationId GetAnimId() const { return m_AnimId; }

    [[nodiscard]] bool IsPlaying() const { return (m_Flags & ANIMATION_IS_PLAYING) != 0; }
    [[nodiscard]] bool IsLooped() const { return (m_Flags & ANIMATION_IS_LOOPED) != 0; }
    [[nodiscard]] bool IsPartial() const { return (m_Flags & ANIMATION_IS_PARTIAL) != 0; }
    [[nodiscard]] bool IsSyncronised() const { return (m_Flags & ANIMATION_IS_SYNCRONISED) != 0; }
    [[nodiscard]] bool CanExtractXVelocity() const { return (m_Flags & ANIMATION_CAN_EXTRACT_X_VELOCITY) != 0; }
    [[nodiscard]] bool CanExtractVelocity() const { return (m_Flags & ANIMATION_CAN_EXTRACT_VELOCITY) != 0; }
    [[nodiscard]] bool IsFacial() const { return (m_Flags & ANIMATION_FACIAL) != 0; }

    void AllocateAnimBlendNodeArray(int32 count);
    void FreeAnimBlendNodeArray();

    void Init(RpClump* clump, CAnimBlendHierarchy* hierarchy);
    void Init(CAnimBlendAssociation& source);
    void Init(CAnimBlendStaticAssociation& source);

    void ReferenceAnimBlock();
    void SetBlendDelta(float value) { m_BlendDelta = value; }
    void SetBlend(float blendAmount, float blendDelta);
    void SetBlendTo(float blendAmount, float blendDelta);
    void SetCurrentTime(float currentTime);
    float GetCurrentTime() const { return m_CurrentTime; }

    void SetDeleteCallback(void(*callback)(CAnimBlendAssociation*, void*), void* data = nullptr);
    void SetDefaultDeleteCallback() { SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr); }

    void SetFinishCallback(void(*callback)(CAnimBlendAssociation*, void*), void* data = nullptr);
    void SetDefaultFinishCallback() { SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr); }

    void Start(float currentTime = 0.f);

    /*!
     * @addr 0x4CEB40
     * @brief Sync the play time of this animation with another
    */
    void SyncAnimation(CAnimBlendAssociation* syncWith);
    bool UpdateBlend(float timeStep);
    bool UpdateTime(float timeStep, float timeMult);
    void UpdateTimeStep(float speedMult, float timeMult);
    bool HasFinished() const;
    [[nodiscard]] uint32 GetHashKey() const noexcept;

    // NOTSA
    void SetFlag(eAnimationFlags flag, bool value = true) {
        if (value)
            m_Flags |= (int)flag;
        else
            m_Flags &= ~(int)flag;
    }

    bool HasFlag(eAnimationFlags flag) const { return m_Flags & flag; }

    static CAnimBlendAssociation* FromLink(CAnimBlendLink* link) {
        return (CAnimBlendAssociation*)((byte*)link - offsetof(CAnimBlendAssociation, m_Link));
    }

    auto GetSpeed() const      { return m_Speed; }
    void SetSpeed(float speed) { m_Speed = speed; }

    std::span<CAnimBlendNode> GetNodes();
    CAnimBlendNode*           GetNode(int32 nodeIndex) { return &GetNodes()[nodeIndex]; } // 0x4CEB60
    CAnimBlendNode*           GetNodesPtr() { return m_BlendNodes; }

    auto& GetLink() { return m_Link; }
    auto  GetHier() const { return m_BlendHier; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAnimBlendAssociation* Constructor0()  { this->CAnimBlendAssociation::CAnimBlendAssociation(); return this; }
    CAnimBlendAssociation* Constructor1(RpClump* clump, CAnimBlendHierarchy* animHierarchy) { this->CAnimBlendAssociation::CAnimBlendAssociation(clump, animHierarchy); return this; }
    CAnimBlendAssociation* Constructor2(CAnimBlendAssociation& assoc) { this->CAnimBlendAssociation::CAnimBlendAssociation(assoc); return this; }
    CAnimBlendAssociation* Constructor3(CAnimBlendStaticAssociation& assoc) { this->CAnimBlendAssociation::CAnimBlendAssociation(assoc); return this; }
};
VALIDATE_SIZE(CAnimBlendAssociation, 0x3C);
