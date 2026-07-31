/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class CEntity;
class CObject;

struct tScriptForBrains {
    int16 m_StreamedScriptIndex{-1}; /// SCM ID for `CStreaming` (Translated using SCMToModelId)
    int8  m_TypeOfBrain{ -1 };
    int8  m_ObjectGroupingId{ -1 };
    bool  m_bBrainActive{ true };
    float m_ObjectBrainActivationRadius{ 5.f };
    union {
        struct {
            int16  m_PedModelOrPedGeneratorIndex;
            uint16 m_PercentageChance;
            uint32 m_Pad;
        };
        char m_ScriptName[8]{};
    };
};

class CScriptsForBrains {
public:
    enum {
        PED_STREAMED = 0,
        OBJECT_STREAMED,
        PED_GENERATOR_STREAMED,
        CODE_PED,
        CODE_OBJECT,
        CODE_ATTRACTOR_PED
    };

    std::array<tScriptForBrains, 70> m_aScriptForBrains;

public:
    static void InjectHooks();

    void Init();

    void AddNewScriptBrain(int16 streamedScriptIndex, int16 pedModelOrPedGeneratorIndex, uint16 percentage, int8 brainType, int8 objectGroupingId, float objectActivationRange);
    void AddNewStreamedScriptBrainForCodeUse(int16 streamedScriptIndex, const char* scriptName, int8 brainType);

    void CheckIfNewEntityNeedsScript(CEntity* entity, int8 brainType, void* unused);

    int16 GetIndexOfScriptBrainWithThisName(const char* name, int8 type);

    bool HasAttractorScriptBrainWithThisNameLoaded(const char* name);
    bool IsObjectWithinBrainActivationRange(CObject* entity, const CVector& point);

    void MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded(const char* name);
    void RequestAttractorScriptBrainWithThisName(const char* name);

    void StartAttractorScriptBrainWithThisName(const char* name, CPed* ped, bool bHasAScriptBrain);
    void StartNewStreamedScriptBrain(uint8 index, CEntity* entity, bool bPedCreatedAtAttractor);
    void StartOrRequestNewStreamedScriptBrain(uint8 index, CEntity* entity, int8 brainType, bool bFirstTime);
    void StartOrRequestNewStreamedScriptBrainWithThisName(const char* name, CEntity* entity, int8 brainType);

    void SwitchAllObjectBrainsWithThisID(int8 ID, bool bStatus);
};
