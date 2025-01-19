/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "eScriptCommands.h"
#include "OpcodeResult.h"
#include "eWeaponType.h"
#include "Ped.h"
#include "CommandParser/Utility.hpp"
#include "./ScriptParam.h"
#include "./TheScripts.h"

enum ePedType : uint32;

enum eScriptParameterType : uint8 {
    SCRIPT_PARAM_END_OF_ARGUMENTS, //< Special type used for vararg stuff

    SCRIPT_PARAM_STATIC_INT_32BITS,
    SCRIPT_PARAM_GLOBAL_NUMBER_VARIABLE, //< Global int32 variable
    SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE, //< Local int32 variable
    SCRIPT_PARAM_STATIC_INT_8BITS,
    SCRIPT_PARAM_STATIC_INT_16BITS,
    SCRIPT_PARAM_STATIC_FLOAT,

    // Types below are only available in GTA SA

    // Number arrays
    SCRIPT_PARAM_GLOBAL_NUMBER_ARRAY, //< Global array of numbers (always int32)
    SCRIPT_PARAM_LOCAL_NUMBER_ARRAY, //< Local array of numbers (always int32)

    SCRIPT_PARAM_STATIC_SHORT_STRING, //< Static 8 byte string

    SCRIPT_PARAM_GLOBAL_SHORT_STRING_VARIABLE, //< Local 8 byte string
    SCRIPT_PARAM_LOCAL_SHORT_STRING_VARIABLE, //< Local 8 byte string

    SCRIPT_PARAM_GLOBAL_SHORT_STRING_ARRAY, //< Global 8 byte string array
    SCRIPT_PARAM_LOCAL_SHORT_STRING_ARRAY,  //< Local 8 byte string array

    SCRIPT_PARAM_STATIC_PASCAL_STRING, //< Pascal string is a sequence of characters with optional size specification. (So says Google)
    SCRIPT_PARAM_STATIC_LONG_STRING,    //< 16 byte string

    SCRIPT_PARAM_GLOBAL_LONG_STRING_VARIABLE, //< Global 16 byte string
    SCRIPT_PARAM_LOCAL_LONG_STRING_VARIABLE, //< Local 16 byte string

    SCRIPT_PARAM_GLOBAL_LONG_STRING_ARRAY, //< Global array of 16 byte strings
    SCRIPT_PARAM_LOCAL_LONG_STRING_ARRAY, //< Local array of 16 byte strings
};

enum eScriptVariableType : uint8 {
    VAR_LOCAL  = 1,
    VAR_GLOBAL = 2
};

enum eButtonId : uint16 {
    BUTTON_LEFT_STICK_X,
    BUTTON_LEFT_STICK_Y,
    BUTTON_RIGHT_STICK_X,
    BUTTON_RIGHT_STICK_Y,
    BUTTON_LEFT_SHOULDER1,
    BUTTON_LEFT_SHOULDER2,
    BUTTON_RIGHT_SHOULDER1,
    BUTTON_RIGHT_SHOULDER2,
    BUTTON_DPAD_UP,
    BUTTON_DPAD_DOWN,
    BUTTON_DPAD_LEFT,
    BUTTON_DPAD_RIGHT,
    BUTTON_START,
    BUTTON_SELECT,
    BUTTON_SQUARE,
    BUTTON_TRIANGLE,
    BUTTON_CROSS,
    BUTTON_CIRCLE,
    BUTTON_LEFTSHOCK,
    BUTTON_RIGHTSHOCK,
};

static inline std::array<tScriptParam, 32>& ScriptParams = *(std::array<tScriptParam, 32>*)0xA43C78;

enum {
    MAX_STACK_DEPTH = 8,
    MAX_LOCAL_VARS  = 32,
    MAX_NUM_TIMERS      = 2
};

constexpr auto SHORT_STRING_SIZE           = 8;
constexpr auto LONG_STRING_SIZE            = 16;
constexpr auto COMMANDS_CHAR_BUFFER_SIZE   = 64;
constexpr auto COMMANDS_CHAR_BUFFERS_COUNT = 16;

namespace scm {
using ShortString = char[SHORT_STRING_SIZE];
using LongString = char[LONG_STRING_SIZE];

/** See https://gtamods.com/wiki/SCM_Instruction#Arrays */
struct ArrayAccess {
    enum class ElementType : uint8 {
        INT,
        FLOAT,
        STRING_SHORT,
        STRING_LONG,
    };
    template<typename T>
    static constexpr auto GetElementTypeOf() {
        if constexpr (std::is_integral_v<T>) {
            return scm::ArrayAccess::ElementType::INT;
        } else if constexpr (std::is_same_v<T, float>) {
            return scm::ArrayAccess::ElementType::FLOAT;
        } else if constexpr (std::is_same_v<T, ShortString>) {
            return scm::ArrayAccess::ElementType::STRING_SHORT;
        } else if constexpr (std::is_same_v<T, LongString>) {
            return scm::ArrayAccess::ElementType::STRING_LONG;
        }
    }

    /** Array base offset (Address of the first value) */
    uint16 ArrayBase;
    /** Location of the index variable used to access the array (May be a global or local variable, see `IdxVarIsGlobal`) */
    uint16 IdxVarLoc;
    /** Array total size (length) (NOTE: Yes, it's unsigned!) */
    uint8 ArraySize;
    /** Array elements type */
    ElementType ElemType: 7;
    /** Index is a global variable (true) or a local variable (false) */
    bool IdxVarIsGlobal: 1;
};
VALIDATE_SIZE(ArrayAccess, 0x6);

/** Represents an instruction, see https://gtamods.com/wiki/SCM_Instruction#Instruction_format */
struct Instruction {
    /** The command */
    eScriptCommands Command : 15;
    /** Whenever the boolean return value of the command should be negated */
    uint16 NotFlag : 1;
};
VALIDATE_SIZE(Instruction, 0x2);

/** Location of a variable */
using VarLoc = uint16;
};

class CRunningScript {
public:
    /*!
     * Needed for compound if statements.
     * Basically, an `if` translates to:
     * - `COMMAND_ANDOR` followed by a parameter that encodes:
     *   1. the number of conditions = `n` (max 8)
     *   2. logical operation between conditions (AND/OR, hence the command name)
     * - `n` commands that update the conditional flag
     *
     * For instance `if ($A > 0 && $B > 0 && $C > 0)` would generate:
     *
     * ```
     * COMMAND_ANDOR                          ANDS_2 // (three conditions joined by AND)
     * COMMAND_IS_INT_VAR_GREATER_THAN_NUMBER $A 0
     * COMMAND_IS_INT_VAR_GREATER_THAN_NUMBER $B 0
     * COMMAND_IS_INT_VAR_GREATER_THAN_NUMBER $C 0
     * ```
     *
     * Each time a condition is tested, the result is AND/OR'd with the previous
     * result and the ANDOR state is decremented until it reaches the lower bound,
     * meaning that all conditions were tested.
     */
    enum LogicalOpType {
        ANDOR_NONE = 0,
        ANDS_1 = 1,
        ANDS_2,
        ANDS_3,
        ANDS_4,
        ANDS_5,
        ANDS_6,
        ANDS_7,
        ANDS_8,
        ORS_1 = 21,
        ORS_2,
        ORS_3,
        ORS_4,
        ORS_5,
        ORS_6,
        ORS_7,
        ORS_8
    };

public:
    CRunningScript*                          m_pNext, *m_pPrev;                 //< Linked list shit
    scm::ShortString                         m_szName;                          //< Name of the script
    uint8*                                   m_BaseIP;                          //< Base instruction pointer
    uint8*                                   m_IP;                              //< current instruction pointer
    std::array<uint8*, MAX_STACK_DEPTH>      m_IPStack;                         //< Stack of instruction pointers (Usually saved on function call, then popped on return)
    uint16                                   m_StackDepth;                      //< Depth (size) of the stack
    std::array<tScriptParam, MAX_LOCAL_VARS> m_LocalVars;                       //< This script's local variables (Also see `GetPointerToLocalVariable`)
    std::array<int32, MAX_NUM_TIMERS>        m_Timers;                          //< Active timers (Unsure)
    bool                                     m_IsActive;                        //< Is the script active (Unsure)
    bool                                     m_CondResult;                      //< (See `COMMAND_GOTO_IF_FALSE`) (Unsure)
    bool                                     m_UsesMissionCleanup;              //< If mission cleanup is needed after this script has finished
    bool                                     m_IsExternal;
    bool                                     m_IsTextBlockOverride;
    int8                                     m_ExternalType;
    int32                                    m_WakeTime;                        //< Used for sleep-like commands (like `COMMAND_WAIT`) - The script halts execution until the time is reached
    uint16                                   m_AndOrState;                      //< Next logical OP type (See `COMMAND_ANDOR`)
    bool                                     m_NotFlag;                         //< Boolean value returned by the called command should be negated
    bool                                     m_IsDeathArrestCheckEnabled;
    bool                                     m_DoneDeathArrest;
    int32                                    m_SceneSkipIP;                     //< IP to use to skip the cutscene (?)
    bool                                     m_ThisMustBeTheOnlyMissionRunning; //< Is (this script) a mission script

public:
    using CommandHandlerFn_t    = OpcodeResult(__thiscall CRunningScript::*)(int32);
    using CommandHandlerTable_t = std::array<CommandHandlerFn_t, 27>;

    static inline CommandHandlerTable_t& s_OriginalCommandHandlerTable = *(CommandHandlerTable_t*)0x8A6168;

    // NOTSA: We need to provide our own string buffer for arguments parser - the initial assumption that we can use string_view pointed at instruction pointer didn't work in the end.
    // it's not always null terminated so we have to provide our own buffer for the text. I'm eyeballing the max size so this needs to be enough for now. I can't find any command
    // using as many string at once, so 16 buffers is plenty. The most i can find is SET_MENU_COLUMN which reads 13 different strings
    static std::array<std::array<char, COMMANDS_CHAR_BUFFER_SIZE>, COMMANDS_CHAR_BUFFERS_COUNT> ScriptArgCharBuffers;
    static uint8                                                                                ScriptArgCharNextFreeBuffer;


public:
    static void InjectHooks();
    static void InjectCustomCommandHooks();

    void Init();

    void PlayAnimScriptCommand(int32 commandId);

    void LocateCarCommand(int32 commandId);
    void LocateCharCommand(int32 commandId);
    void LocateObjectCommand(int32 commandId);
    void LocateCharCarCommand(int32 commandId);
    void LocateCharCharCommand(int32 commandId);
    void LocateCharObjectCommand(int32 commandId);

    void CarInAreaCheckCommand(int32 commandId);
    void CharInAreaCheckCommand(int32 commandId);
    void ObjectInAreaCheckCommand(int32 commandId);

    void CharInAngledAreaCheckCommand(int32 commandId);
    void FlameInAngledAreaCheckCommand(int32 commandId);
    void ObjectInAngledAreaCheckCommand(int32 commandId);

    void  CollectParameters(int16 count);
    int32 CollectNextParameterWithoutIncreasingPC();
    void  StoreParameters(int16 count);

    void ReadArrayInformation(int32 updateIp, uint16* outArrVarOffset, int32* outArrElemIdx);
    void ReadParametersForNewlyStartedScript(CRunningScript* newScript);
    void ReadTextLabelFromScript(char* buffer, uint8 nBufferLength);
    void GetCorrectPedModelIndexForEmergencyServiceType(ePedType pedType, uint32* typeSpecificModelId);
    int16 GetPadState(uint16 playerIndex, eButtonId buttonId);

    tScriptParam* GetPointerToLocalVariable(int32 loc);
    tScriptParam* GetPointerToLocalArrayElement(int32 arrVarOffset, uint16 arrElemIdx, uint8 arrayEntriesSizeAsParams);
    tScriptParam* GetPointerToScriptVariable(eScriptVariableType variableType);

    tScriptParam* GetPointerToGlobalArrayElement(int32 arrBase, uint16 arrIdx, uint8 arrayEntriesSizeAsParams);
    tScriptParam* GetPointerToGlobalVariable(int32 varId);
    uint16        GetIndexOfGlobalVariable();

    void DoDeathArrestCheck(); // original name DoDeatharrestCheck

    void SetCharCoordinates(CPed& ped, CVector posn, bool warpGang, bool offset);
    void GivePedScriptedTask(int32 pedHandle, CTask* task, int32 opcode);
    void GivePedScriptedTask(CPed* ped, CTask* task, int32 opcode); // NOTSA overload

    void AddScriptToList(CRunningScript** queueList);
    void RemoveScriptFromList(CRunningScript** queueList);
    void ShutdownThisScript();

    bool IsPedDead(CPed* ped) const;
    bool ThisIsAValidRandomPed(ePedType pedType, bool civilian, bool gang, bool criminal);
    void ScriptTaskPickUpObject(int32 commandId);

    void UpdateCompareFlag(bool state);
    void UpdatePC(int32 newIP);

    OpcodeResult ProcessOneCommand();
    OpcodeResult Process();

    void SetName(const char* name)      { strcpy_s(m_szName, name); }
    void SetName(std::string_view name) { assert(name.size() < sizeof(m_szName)); strncpy_s(m_szName, name.data(), name.size()); }
    void SetBaseIp(uint8* ip)           { m_BaseIP = ip; }
    void SetCurrentIp(uint8* ip)        { m_IP = ip; }
    void SetActive(bool active)         { m_IsActive = active; }
    void SetExternal(bool external)     { m_IsExternal = external; }

    //! Highlight an important area 2D
    void HighlightImportantArea(CVector2D from, CVector2D to, float z = -100.f);

    //! Highlight an important area 2D
    void HighlightImportantArea(CRect area, float z = -100.f);

    //! Highlight an important area 3D
    void HighlightImportantArea(CVector from, CVector to);

    //! Refer to `IsPositionWithinQuad2D` for information on how this works
    void HighlightImportantAngledArea(uint32 id, CVector2D a, CVector2D b, CVector2D c, CVector2D d);

    //! Get value at IP
    template<typename T>
    T& GetAtIPAs(bool updateIP = true, size_t sz = sizeof(T)) {
        T& ret = *reinterpret_cast<T*>(m_IP);
        if (updateIP) {
            m_IP += sz;
        }
        return ret;
    }

    //! Get local variable
    template<typename T>
    T& GetLocal(scm::VarLoc loc) {
        return reinterpret_cast<T&>(m_ThisMustBeTheOnlyMissionRunning ? CTheScripts::LocalVariablesForCurrentMission[loc] : m_LocalVars[loc]);
    }

    //! Get value from local array
    template<typename T>
    T& GetArrayLocal(scm::VarLoc base, size_t idx, size_t elemSizeInDWords = std::min<size_t>(1, sizeof(T) / sizeof(int32))) {
        return GetLocal<T>((scm::VarLoc)(base + idx * elemSizeInDWords));
    }

    //! Get global variable
    template<typename T>
    T& GetGlobal(scm::VarLoc loc) {
        return reinterpret_cast<T&>(CTheScripts::ScriptSpace[loc]);
    }

    //! Get value from global array
    template<typename T>
    T& GetArrayGlobal(scm::VarLoc base, size_t idx, size_t elemSizeInDWords = std::min<size_t>(1, sizeof(T) / sizeof(int32))) {
        return GetGlobal<T>((scm::VarLoc)(base + idx * elemSizeInDWords * sizeof(int32)));
    }

    //! Perform array access (Increments IP)
    template<typename T>
    T& GetAtIPFromArray(bool isGlobalArray) {
        const auto op = GetAtIPAs<scm::ArrayAccess>();
        VERIFY(op.ElemType == scm::ArrayAccess::GetElementTypeOf<T>());
        const auto idx = op.IdxVarIsGlobal
            ? GetGlobal<int32>(op.IdxVarLoc)
            : GetLocal<int32>(op.IdxVarLoc);
        VERIFY(idx >= 0 && idx < op.ArraySize);
        return isGlobalArray
            ? GetArrayGlobal<T>(op.ArrayBase, (scm::VarLoc)(idx))
            : GetArrayLocal<T>(op.ArrayBase, (scm::VarLoc)(idx));
    }

    //! Return the custom command handler of a function (or null) as a reference
    static notsa::script::CommandHandlerFunction& CustomCommandHandlerOf(eScriptCommands command); // Returning a ref here for convenience (instead of having to make a `Set` function too)

private:
    void ResetIPStack();
};

VALIDATE_SIZE(CRunningScript, 0xE0);
