#include "StdInc.h"

#include "AnimBlendNode.h"

void CAnimBlendNode::InjectHooks() {
    RH_ScopedClass(CAnimBlendNode);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Init, 0x4CFB70);
    RH_ScopedInstall(NextKeyFrameNoCalc, 0x4CFB90);
    RH_ScopedInstall(GetCurrentTranslation, 0x4CFC50);
    RH_ScopedInstall(GetCurrentTranslationCompressed, 0x4CFE60);
    RH_ScopedInstall(GetEndTranslation, 0x4CFD90);
    RH_ScopedInstall(GetEndTranslationCompressed, 0x4D0000);
    RH_ScopedInstall(CalcTheta, 0x4D00E0);
    RH_ScopedInstall(UpdateTime, 0x4D0160);
    RH_ScopedInstall(CalcDeltas, 0x4D0190);
    RH_ScopedInstall(FindKeyFrame, 0x4D0240);
    RH_ScopedInstall(CalcDeltasCompressed, 0x4D0350);
    RH_ScopedInstall(NextKeyFrame, 0x4D04A0);
    RH_ScopedInstall(NextKeyFrameCompressed, 0x4D0570);
    RH_ScopedInstall(SetupKeyFrameCompressed, 0x4D0650);
    RH_ScopedInstall(Update, 0x4D06C0);
    RH_ScopedInstall(UpdateCompressed, 0x4D08D0);
}

// 0x4CFB70
void CAnimBlendNode::Init() {
    *this = CAnimBlendNode{};
}

// 0x4D0240
bool CAnimBlendNode::FindKeyFrame(float time) {
    if (m_Seq->m_FramesNum == 0) {
        return false;
    }

    m_KFCurr = 0;
    m_KFPrev = 0;

    if (m_Seq->m_FramesNum == 1) {
        m_KFRemainingTime = 0.0f;
    } else {
        // Find kf that spans over the specified `time`
        for (auto kf = m_Seq->GetUKeyFrame(++m_KFCurr); time > kf->DeltaTime; m_KFPrev = m_KFCurr++) {
            time -= m_Seq->GetUKeyFrame(m_KFCurr)->DeltaTime;

            if (m_KFCurr + 1 >= m_Seq->m_FramesNum) {
                // reached end of animation
                if (!m_BlendAssoc->IsLooped()) {
                    CalcDeltas();
                    m_KFRemainingTime = 0.0f;
                    return false;
                }
                m_KFCurr = 0;
            }
        }

        // Now calculate how much we have remaining from this frame
        m_KFRemainingTime = m_Seq->GetUKeyFrame(m_KFCurr)->DeltaTime - time;
    }

    CalcDeltas();
    return true;
}

// 0x4D00E0
void CAnimBlendNode::CalcTheta(float angleCos) {
    m_Theta       = std::acos(std::min(angleCos, 1.0f));
    m_InvSinTheta = m_Theta == 0.0f
        ? 0.0f
        : 1.0f / std::sin(m_Theta);
}

// 0x4D0190
void CAnimBlendNode::CalcDeltas() {
    if (!m_Seq->m_bHasRotation) {
        return;
    }

    CalcTheta(
        DotProduct(
            m_Seq->GetUKeyFrame(m_KFCurr)->Rot,
            m_Seq->GetUKeyFrame(m_KFPrev)->Rot
        )
    );
}

// 0x4D0350
void CAnimBlendNode::CalcDeltasCompressed() {
    if (!m_Seq->m_bHasRotation) {
        return;
    }

    KeyFrameCompressed* kfA = m_Seq->GetCKeyFrame(m_KFCurr);
    KeyFrameCompressed* kfB = m_Seq->GetCKeyFrame(m_KFPrev);

    CQuaternion rotA = kfA->Rot, rotB = kfB->Rot;
    const auto angleCos = DotProduct(rotA, rotB); // What kind of retarded hack is this
    if (angleCos < 0.0f) {
        rotB     = -rotB;
        kfB->Rot = rotB;
    }
    CalcTheta(angleCos);
}

// 0x4CFB90 - Unused
bool CAnimBlendNode::NextKeyFrameNoCalc() {
    NOTSA_UNREACHABLE("Unused Function");
}

// 0x4D0650
bool CAnimBlendNode::SetupKeyFrameCompressed() {
    if (m_Seq->m_FramesNum < 1) {
        return false;
    }

    m_KFCurr = 1;
    m_KFPrev = 0;

    if (m_Seq->m_FramesNum == 1) {
        m_KFCurr    = 0;
        m_KFRemainingTime = 0.0f;
    } else {
        m_KFRemainingTime = m_Seq->GetCKeyFrame(m_KFCurr)->DeltaTime;
    }

    CalcDeltasCompressed();
    return true;
}

// 0x4D0160 - Unused
bool CAnimBlendNode::UpdateTime() {
    if (m_BlendAssoc->IsPlaying()) {
        m_KFRemainingTime -= m_BlendAssoc->m_TimeStep;
        if (m_KFRemainingTime <= 0.0f) {
            return NextKeyFrameNoCalc();
        }
    }
    return false;
}
