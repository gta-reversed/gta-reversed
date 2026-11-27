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
class CPed;

//! Type of a script brain (`tScriptForBrains::m_TypeOfBrain`)
enum eScriptBrainType : int8 {
    SCRIPT_BRAIN_TYPE_NONE                   = -1, // NOTSA
    SCRIPT_BRAIN_TYPE_PED_STREAMED           = 0,  //!< ALLOCATE_STREAMED_SCRIPT_TO_RANDOM_PED
    SCRIPT_BRAIN_TYPE_OBJECT_STREAMED        = 1,  //!< ALLOCATE_STREAMED_SCRIPT_TO_OBJECT
    SCRIPT_BRAIN_TYPE_PED_GENERATOR_STREAMED = 2,
    SCRIPT_BRAIN_TYPE_CODE_PED               = 3, //!< REGISTER_SCRIPT_BRAIN_FOR_CODE_USE
    SCRIPT_BRAIN_TYPE_CODE_OBJECT            = 4,
    SCRIPT_BRAIN_TYPE_CODE_ATTRACTOR_PED     = 5, //!< REGISTER_ATTRACTOR_SCRIPT_BRAIN_FOR_CODE_USE
};

//! Value of `CObject::objectFlags.b0x100000_0x200000`
enum eObjectScriptBrainStatus : uint8 {
    OBJECT_SCRIPT_BRAIN_NONE               = 0, //!< Doesn't use a script brain
    OBJECT_SCRIPT_BRAIN_WAITING_FOR_PLAYER = 1, //!< Waiting for the player to come within the activation range
    OBJECT_SCRIPT_BRAIN_LOADING            = 2, //!< Script requested, waiting for it to load
    OBJECT_SCRIPT_BRAIN_RUNNING            = 3, //!< Script brain started
};

struct tScriptForBrains {
    int16 m_StreamedScriptIndex{ -1 };             /// SCM ID for `CStreaming` (Translated using SCMToModelId)
    int8  m_TypeOfBrain{ SCRIPT_BRAIN_TYPE_NONE }; //!< See `eScriptBrainType`
    int8  m_ObjectGroupingId{ -1 };
    bool  m_IsBrainActive{ true };
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
VALIDATE_SIZE(tScriptForBrains, 0x14);

class CScriptsForBrains {
public:
    static void InjectHooks();

    void Init();

    void AddNewScriptBrain(int16 streamedScriptIndex, int16 modelOrPedGeneratorIndex, uint16 percentageChance, int8 brainType, int8 objectGroupingId, float objectActivationRadius);
    void AddNewStreamedScriptBrainForCodeUse(int16 streamedScriptIndex, const char* scriptName, int8 brainType);

    void CheckIfNewEntityNeedsScript(CEntity* entity, int8 brainType, void* unused);

    int16 GetIndexOfScriptBrainWithThisName(const char* name, int8 brainType);

    bool HasAttractorScriptBrainWithThisNameLoaded(const char* name);
    bool IsObjectWithinBrainActivationRange(CObject* object, const CVector& point);

    void MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded(const char* name);
    void RequestAttractorScriptBrainWithThisName(const char* name);

    void StartAttractorScriptBrainWithThisName(const char* name, CPed* ped, bool useAttractorInstantly);
    void StartNewStreamedScriptBrain(uint8 index, CEntity* entity, bool useAttractorInstantly);
    void StartOrRequestNewStreamedScriptBrain(uint8 index, CEntity* entity, int8 brainType, bool isFirstTime);
    void StartOrRequestNewStreamedScriptBrainWithThisName(const char* name, CEntity* entity, int8 brainType);

    void SwitchAllObjectBrainsWithThisID(int8 objectGroupingId, bool isBrainActive);

public:
    std::array<tScriptForBrains, 70> m_aScriptForBrains; // NOTE: Still public (and named so) - `CTheScripts`, `CPed` and `CObject` access it directly
};
VALIDATE_SIZE(CScriptsForBrains, 0x14 * 70);
