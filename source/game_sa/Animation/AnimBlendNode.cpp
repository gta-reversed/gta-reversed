#include "StdInc.h"

#include "AnimBlendNode.h"

void CAnimBlendNode::InjectHooks() {
    RH_ScopedClass(CAnimBlendNode);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Init, 0x4CFB70);
    RH_ScopedInstall(NextKeyFrameNoCalc, 0x4CFB90);
    RH_ScopedInstall(GetCurrentTranslation, 0x4CFC50, { .reversed = false });
    RH_ScopedInstall(GetCurrentTranslationCompressed, 0x4CFE60, { .reversed = false });
    RH_ScopedInstall(GetEndTranslation, 0x4CFD90, { .reversed = false });
    RH_ScopedInstall(GetEndTranslationCompressed, 0x4D0000, { .reversed = false });
    RH_ScopedInstall(CalcTheta, 0x4D00E0);
    RH_ScopedInstall(UpdateTime, 0x4D0160);
    RH_ScopedInstall(CalcDeltas, 0x4D0190);
    RH_ScopedInstall(FindKeyFrame, 0x4D0240);
    RH_ScopedInstall(CalcDeltasCompressed, 0x4D0350);
    RH_ScopedInstall(NextKeyFrame, 0x4D04A0, { .reversed = false });
    RH_ScopedInstall(NextKeyFrameCompressed, 0x4D0570, { .reversed = false });
    RH_ScopedInstall(SetupKeyFrameCompressed, 0x4D0650);
    RH_ScopedInstall(Update, 0x4D06C0);
    RH_ScopedInstall(UpdateCompressed, 0x4D08D0, { .reversed = false });
}

// 0x4CFB70
void CAnimBlendNode::Init() {
    m_BlendAssoc = nullptr;
    m_BlendSeq = nullptr;
    m_RemainingTime = 0.0f;
    m_KeyFrameB = -1;
    m_KeyFrameA = -1;
}

// 0x4D0240
bool CAnimBlendNode::FindKeyFrame(float time) {
    if (m_BlendSeq->m_nFrameCount < 1) {
        return false;
    }

    m_KeyFrameA = 0;
    m_KeyFrameB = m_KeyFrameA;

    if (m_BlendSeq->m_nFrameCount == 1) {
        m_RemainingTime = 0.0f;
        CalcDeltas();
        return true;
    }

    // advance until t is between m_PreviousKeyFrameId and m_nNextKeyFrameId
    // maybe check if GetUncompressedFrame is < numFrames?
    while (time > m_BlendSeq->GetUncompressedFrame(++m_KeyFrameA)->deltaTime) {
        time -= m_BlendSeq->GetUncompressedFrame(m_KeyFrameA)->deltaTime;
        if (m_KeyFrameA + 1 >= m_BlendSeq->m_nFrameCount) {
            // reached end of animation
            if (!m_BlendAssoc->IsRepeating()) {
                CalcDeltas();
                m_RemainingTime = 0.0f;
                return false;
            }
            m_KeyFrameA = 0;
        }
        m_KeyFrameB = m_KeyFrameA;
    }

    m_RemainingTime = m_BlendSeq->GetUncompressedFrame(m_KeyFrameA)->deltaTime - time;

    CalcDeltas();
    return true;
}

// 0x4D00E0
void CAnimBlendNode::CalcTheta(float angle) {
    m_Theta = std::acos(std::min(angle, 1.0f));
    m_InvSinTheta = m_Theta == 0.0f ? 0.0f : 1.0f / std::sin(m_Theta);
}

// 0x4D0190
void CAnimBlendNode::CalcDeltas() {
    if (!m_BlendSeq->m_bHasRotation) {
        return;
    }

    KeyFrame* kfA = m_BlendSeq->GetUncompressedFrame(m_KeyFrameA);
    KeyFrame* kfB = m_BlendSeq->GetUncompressedFrame(m_KeyFrameB);

    CalcTheta(DotProduct(kfA->rotation, kfB->rotation));
}

// 0x4D0350
void CAnimBlendNode::CalcDeltasCompressed() {
    if (!m_BlendSeq->m_bHasRotation) {
        return;
    }

    KeyFrameCompressed* kfA = m_BlendSeq->GetCompressedFrame(m_KeyFrameA);
    KeyFrameCompressed* kfB = m_BlendSeq->GetCompressedFrame(m_KeyFrameB);
    CQuaternion rotA, rotB;
    kfA->GetRotation(&rotA);
    kfB->GetRotation(&rotB);
    float cos = DotProduct(rotA, rotB);
    if (cos < 0.0f) {
        rotB = -rotB;
        kfB->SetRotation(rotB);
    }
    CalcTheta(DotProduct(rotA, rotB));
}

// 0x4CFC50
void CAnimBlendNode::GetCurrentTranslation(CVector& trans, float weight) {
    assert(0);
}

// 0x4CFE60
void CAnimBlendNode::GetCurrentTranslationCompressed(CVector& trans, float weight) {
    assert(0);
}

// 0x4CFD90
void CAnimBlendNode::GetEndTranslation(CVector& trans, float weight) {
    assert(0);
}

// 0x4D0000
void CAnimBlendNode::GetEndTranslationCompressed(CVector& trans, float weight) {
    assert(0);
}

// 0x4D04A0
bool CAnimBlendNode::NextKeyFrame() {
    return plugin::CallMethodAndReturn<bool, 0x4D04A0>(this);
}

// 0x4D0570
bool CAnimBlendNode::NextKeyFrameCompressed() {
    return plugin::CallMethodAndReturn<bool, 0x4D0570>(this);
}

// 0x4CFB90
bool CAnimBlendNode::NextKeyFrameNoCalc() {
    return plugin::CallMethodAndReturn<bool, 0x4CFB90>(this);
}

// 0x4D0650
bool CAnimBlendNode::SetupKeyFrameCompressed() {
    if (m_BlendSeq->m_nFrameCount < 1) {
        return false;
    }

    m_KeyFrameA = 1;
    m_KeyFrameB = 0;

    if (m_BlendSeq->m_nFrameCount == 1) {
        m_KeyFrameA = 0;
        m_RemainingTime = 0.0f;
    } else
        m_RemainingTime = m_BlendSeq->GetCompressedFrame(m_KeyFrameA)->GetDeltaTime();

    CalcDeltasCompressed();
    return true;
}

// 0x4D06C0
bool CAnimBlendNode::Update(CVector& trans, CQuaternion& rot, float weight) {
    bool looped = false;

    trans = CVector(0.0f, 0.0f, 0.0f);
    rot   = CQuaternion(0.0f, 0.0f, 0.0f, 0.0f);

    if (m_BlendAssoc->IsRunning()) {
        m_RemainingTime -= m_BlendAssoc->m_TimeStep;
        if (m_RemainingTime <= 0.0f) {
            looped = NextKeyFrame();
        }
    }

    float blend = m_BlendAssoc->GetBlendAmount(weight);
    if (blend > 0.0f) {
        KeyFrameTrans* kfA = (KeyFrameTrans*)m_BlendSeq->GetKeyFrame(m_KeyFrameA);
        KeyFrameTrans* kfB = (KeyFrameTrans*)m_BlendSeq->GetKeyFrame(m_KeyFrameB);

        const float t = kfA->deltaTime == 0.0f
            ? 0.0f
            : (kfA->deltaTime - m_RemainingTime) / kfA->deltaTime;

        if (m_BlendSeq->m_bHasTranslation) {
            trans = kfB->translation + t * (kfA->translation - kfB->translation);
            trans *= blend;
        }
        if (m_BlendSeq->m_bHasRotation) {
            rot.Slerp(kfB->rotation, kfA->rotation, m_Theta, m_InvSinTheta, t);
            rot *= blend;
        }
    }

    return looped;
}

// 0x4D08D0
bool CAnimBlendNode::UpdateCompressed(CVector& trans, CQuaternion& rot, float weight) {
    return plugin::CallMethodAndReturn<bool, 0x4D08D0, CAnimBlendNode*, CVector&, CQuaternion&, float>(this, trans, rot, weight);
}

// 0x4D0160
bool CAnimBlendNode::UpdateTime() {
    if (m_BlendAssoc->IsRunning()) {
        m_RemainingTime -= m_BlendAssoc->m_TimeStep;
        if (m_RemainingTime <= 0.0f) {
            return NextKeyFrameNoCalc();
        }
    }
    return false;
}
