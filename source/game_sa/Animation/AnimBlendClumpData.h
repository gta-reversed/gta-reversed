/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "Vector.h"
#include "AnimBlendFrameData.h"

class CAnimBlendClumpData {
public:
    CAnimBlendLink m_AnimList; //!< List of `CAnimBlendAssociation` - List of anims that are being played on this clump
    union {
        uint32 m_NumFrameData; // For skinned clumps
        uint32 m_NumBones;  // For non-skinned clumps
    };
    CVector*            m_PedPosition;
    AnimBlendFrameData* m_FrameDatas; // There's always at least 1 frame present

public:
    static void InjectHooks();

    CAnimBlendClumpData();
    ~CAnimBlendClumpData();
    CAnimBlendClumpData* Constructor() { this->CAnimBlendClumpData::CAnimBlendClumpData(); return this; }
    CAnimBlendClumpData* Destructor()  { this->CAnimBlendClumpData::~CAnimBlendClumpData(); return this; }

    void ForAllFrames(void (*callback)(AnimBlendFrameData*, void*), void* data);

    /*!
     * @notsa
     * @brief Iterate all frames (Using a functor, usually a lambda) 
     * @param Fn The functor to be called
    */
    template<typename Functor>
    void ForAllFramesF(Functor&& Fn) {
        for (auto& frame : std::span{ m_FrameDatas, m_NumFrameData }) {
            std::invoke(Fn, &frame);
        }
    }

    auto& GetAnims() { return m_AnimList; }

private:
    void ForAllFramesInSPR(void (*callback)(AnimBlendFrameData*, void*), void* data, uint32 a3);
    void LoadFramesIntoSPR();
public:

    AnimBlendFrameData& GetRootFrameData() const { assert(m_NumFrameData >= 1); return m_FrameDatas[0]; }
    void SetNumberOfBones(uint32 numBones);
    auto GetFrames() const { return m_FrameDatas; }
};
VALIDATE_SIZE(CAnimBlendClumpData, 0x14);
