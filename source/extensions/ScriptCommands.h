/*
    Plugin-SDK (Grand Theft Auto) SHARED header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <vector>
#include <string>

#include "RunningScript.h"

class CPed;
class CVehicle;
class CObject;

class ScriptCode {
public:
    enum ScriptCommandEndParameter {
        END_PARAMETER
    };

    explicit ScriptCode(int16 commandId = -1);
    ~ScriptCode();

    void AddParameterDescription(uint8 paramType);
    void AddBytes(uint8* bytes, uint32 count);

    void SaveResultVariables(CRunningScript* script);

    void operator<<(char n);
    void operator<<(uint8 n);
    void operator<<(int16 n);
    void operator<<(uint16 n);
    void operator<<(int32 n);
    void operator<<(uint32 n);
    void operator<<(float n);
    void operator<<(double n);
    void operator<<(ScriptCommandEndParameter);
    void operator<<(char* str);
    void operator<<(const char* str);
    void operator<<(float* p);
    void operator<<(int32* p);
    void operator<<(uint32* p);
    void operator<<(char (*p)[16]);
    void operator<<(CPed* ped);
    void operator<<(CVehicle* vehicle);
    void operator<<(CObject* obj);
    void operator<<(CPed** peds);
    void operator<<(CVehicle** vehicles);
    void operator<<(CObject** objs);
    void operator<<(const CVector& vec);

    uint8* GetData() { return m_Data; };

    template <typename T> void Pack(T value) {
        operator<<(value);
    }

    template <typename First, typename... Rest> void Pack(First firstValue, Rest... rest) {
        Pack(firstValue);
        Pack(rest...);
    }

    void Pack() {}

    template <typename... ArgTypes> static
        inline bool CallCommandById(uint32 commandId, ArgTypes... arguments) {
        // create our 'script' object
        static CRunningScript script;
        memset(&script, 0, sizeof(CRunningScript));

        script.Init();
        script.SetName("gta-r-m");
        script.m_bIsMission         = false;
        script.m_bUseMissionCleanup = false;
        script.m_bNotFlag           = (commandId >> 15) & 1;

        // our script code
        ScriptCode code(commandId);

        // for all arguments: add them to script code
        code.Pack(arguments...);
        script.m_pBaseIP = script.m_IP = code.GetData();

        script.ProcessOneCommand();
        code.SaveResultVariables(&script);

        return script.m_bCondResult ? true : false;
    }

private:
    enum ScriptResultVarType {
        SCRIPT_RESULT_VAR_NUMBER,
        SCRIPT_RESULT_VAR_STRING,
        SCRIPT_RESULT_VAR_PED,
        SCRIPT_RESULT_VAR_VEHICLE,
        SCRIPT_RESULT_VAR_OBJECT
    };

    uint8* m_Data;
    uint32 m_Capacity;
    uint32 m_Size;

    struct VarToSet {
        uint32              m_VarIndex;
        void*               m_Var;
        ScriptResultVarType m_VarType;

        VarToSet(uint32 varIndex, void* var, ScriptResultVarType varType);
    };

    std::vector<VarToSet> m_VarsToSet;
    uint16                m_VarIndexCounter;
};

template <eScriptCommands CommandId, typename... ArgTypes>
bool Command(ArgTypes... arguments) {
    return ScriptCode::CallCommandById(static_cast<uint32>(CommandId) - 0x10000, arguments...);
}
