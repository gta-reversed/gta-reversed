/*
    Plugin-SDK (Grand Theft Auto) SHARED source file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "ScriptCommands.h"
#include "Pools.h"

#define SCRIPT_COMMANDS_LVAR_MAX_INDEX 31

ScriptCode::VarToSet::VarToSet(uint32 varIndex, void* var, ScriptResultVarType varType) {
    m_VarIndex = varIndex;
    m_Var      = var;
    m_VarType  = varType;
}

ScriptCode::ScriptCode(int16 commandId) {
    m_Capacity = 32;
    m_Data = new uint8[m_Capacity];
    m_Size = 0;
    m_VarIndexCounter = 0;

    if (commandId != -1) {
        AddBytes(reinterpret_cast<uint8*>(&commandId), 2);
    }
}

ScriptCode::~ScriptCode() {
    delete[] m_Data;
}

void ScriptCode::AddParameterDescription(uint8 paramType) {
    AddBytes(&paramType, 1);
}

void ScriptCode::AddBytes(uint8* bytes, uint32 count) {
    uint32 newSize = m_Size + count;
    if (newSize > m_Capacity) {
        while (newSize > m_Capacity) {
            m_Capacity += 32;
        }
        auto* newData = new uint8[m_Capacity];
        memcpy(newData, m_Data, m_Size);
        delete[] m_Data;
        m_Data = newData;
    }
    memcpy(&m_Data[m_Size], bytes, count);
    m_Size = newSize;
}

void ScriptCode::SaveResultVariables(CRunningScript* script) {
    for (auto& varToSet : m_VarsToSet) {
        if (varToSet.m_VarType == SCRIPT_RESULT_VAR_NUMBER)
        {
            *reinterpret_cast<uint32*>(varToSet.m_Var) = script->m_aLocalVars[varToSet.m_VarIndex].uParam;
        }
        else if (varToSet.m_VarType == SCRIPT_RESULT_VAR_STRING)
        {
            char* str = reinterpret_cast<char*>(varToSet.m_Var);
            strncpy_s(str, 15, reinterpret_cast<char*>(&script->m_aLocalVars[varToSet.m_VarIndex].iParam), 15);
            str[15] = '\0';
        }
        else if (varToSet.m_VarType == SCRIPT_RESULT_VAR_PED)
        {
            const auto id = script->m_aLocalVars[varToSet.m_VarIndex].iParam;
            CPed* ped = id != -1 ? CPools::GetPed(id) : nullptr;
            *reinterpret_cast<CPed**>(varToSet.m_Var) = ped;
        }
        else if (varToSet.m_VarType == SCRIPT_RESULT_VAR_VEHICLE)
        {
            const auto id = script->m_aLocalVars[varToSet.m_VarIndex].iParam;
            CVehicle* vehicle = id != -1 ? CPools::GetVehicle(id) : nullptr;
            *reinterpret_cast<CVehicle**>(varToSet.m_Var) = vehicle;
        }
        else if (varToSet.m_VarType == SCRIPT_RESULT_VAR_OBJECT)
        {
            const auto id = script->m_aLocalVars[varToSet.m_VarIndex].iParam;
            CObject* obj = id != -1 ? CPools::GetObject(id) : nullptr;
            *reinterpret_cast<CObject**>(varToSet.m_Var) = obj;
        }
    }
}

void ScriptCode::operator<<(char n) {
    AddParameterDescription(SCRIPT_PARAM_STATIC_INT_8BITS);
    AddBytes(reinterpret_cast<uint8*>(&n), 1);
}

void ScriptCode::operator<<(uint8 n) {
    AddParameterDescription(SCRIPT_PARAM_STATIC_INT_8BITS);
    AddBytes(reinterpret_cast<uint8*>(&n), 1);
}

void ScriptCode::operator<<(int16 n) {
    AddParameterDescription(SCRIPT_PARAM_STATIC_INT_16BITS);
    AddBytes(reinterpret_cast<uint8*>(&n), 2);
}

void ScriptCode::operator<<(uint16 n) {
    AddParameterDescription(SCRIPT_PARAM_STATIC_INT_16BITS);
    AddBytes(reinterpret_cast<uint8*>(&n), 2);
}

void ScriptCode::operator<<(int32 n) {
    AddParameterDescription(SCRIPT_PARAM_STATIC_INT_32BITS);
    AddBytes(reinterpret_cast<uint8*>(&n), 4);
}

void ScriptCode::operator<<(uint32 n) {
    AddParameterDescription(SCRIPT_PARAM_STATIC_INT_32BITS);
    AddBytes(reinterpret_cast<uint8*>(&n), 4);
}

void ScriptCode::operator<<(float n) {
    AddParameterDescription(SCRIPT_PARAM_STATIC_FLOAT);
    AddBytes(reinterpret_cast<uint8*>(&n), 4);
}

void ScriptCode::operator<<(double n) {
    AddParameterDescription(SCRIPT_PARAM_STATIC_FLOAT);
    auto f = static_cast<float>(n);
    AddBytes(reinterpret_cast<uint8*>(&f), 4);
}

void ScriptCode::operator<<(ScriptCommandEndParameter) {
    AddParameterDescription(SCRIPT_PARAM_END_OF_ARGUMENTS);
}

void ScriptCode::operator<<(char* str) {
    AddParameterDescription(SCRIPT_PARAM_STATIC_PASCAL_STRING);
    uint32 length = strlen(str);
    AddParameterDescription(length);
    AddBytes(reinterpret_cast<uint8*>(str), length);
}

void ScriptCode::operator<<(const char* str) {
    AddParameterDescription(SCRIPT_PARAM_STATIC_PASCAL_STRING);
    uint32 length = strlen(str);
    AddParameterDescription(length);
    AddBytes(reinterpret_cast<uint8*>(const_cast<char*>(str)), length);
}

void ScriptCode::operator<<(char (*p)[16]) {
    AddParameterDescription(SCRIPT_PARAM_LOCAL_LONG_STRING_VARIABLE);
    if (m_VarIndexCounter >= 28) {
        assert("ScriptCode::operator<<(char **p): reached local var limit");
    }
    m_VarsToSet.emplace_back(m_VarIndexCounter, reinterpret_cast<void*>(*p), SCRIPT_RESULT_VAR_STRING);
    AddBytes(reinterpret_cast<uint8*>(&m_VarIndexCounter), 2);
    m_VarIndexCounter += 4;
}

void ScriptCode::operator<<(float* p) {
    AddParameterDescription(SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE);
    if (m_VarIndexCounter >= SCRIPT_COMMANDS_LVAR_MAX_INDEX) {
        assert("ScriptCode::operator<<(float *p): reached local var limit");
    }
    m_VarsToSet.emplace_back(m_VarIndexCounter, reinterpret_cast<void*>(p), SCRIPT_RESULT_VAR_NUMBER);
    AddBytes(reinterpret_cast<uint8*>(&m_VarIndexCounter), 2);
    m_VarIndexCounter++;
}

void ScriptCode::operator<<(int32* p) {
    AddParameterDescription(SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE);
    if (m_VarIndexCounter >= SCRIPT_COMMANDS_LVAR_MAX_INDEX) {
        assert("ScriptCode::operator<<(int *p): reached local var limit");
    }
    m_VarsToSet.emplace_back(m_VarIndexCounter, reinterpret_cast<void*>(p), SCRIPT_RESULT_VAR_NUMBER);
    AddBytes(reinterpret_cast<uint8*>(&m_VarIndexCounter), 2);
    m_VarIndexCounter++;
}

void ScriptCode::operator<<(uint32* p) {
    AddParameterDescription(SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE);
    if (m_VarIndexCounter >= SCRIPT_COMMANDS_LVAR_MAX_INDEX) {
        assert("ScriptCode::operator<<(uint32 *p): reached local var limit");
    }
    m_VarsToSet.emplace_back(m_VarIndexCounter, reinterpret_cast<void*>(p), SCRIPT_RESULT_VAR_NUMBER);
    AddBytes(reinterpret_cast<uint8*>(&m_VarIndexCounter), 2);
    m_VarIndexCounter++;
}

void ScriptCode::operator<<(CPed* ped) {
    if (!ped)
        operator<<(-1);
    else
        operator<<(CPools::GetPedRef(ped));
}

void ScriptCode::operator<<(CVehicle* vehicle) {
    if (!vehicle)
        operator<<(-1);
    else
        operator<<(CPools::GetVehicleRef(vehicle));
}

void ScriptCode::operator<<(CObject* obj) {
    if (!obj)
        operator<<(-1);
    else
        operator<<(CPools::GetObjectRef(obj));
}

void ScriptCode::operator<<(CPed** peds) {
    AddParameterDescription(SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE);
    if (m_VarIndexCounter >= SCRIPT_COMMANDS_LVAR_MAX_INDEX) {
        assert("ScriptCode::operator<<(CPed **p): reached local var limit");
    }
    m_VarsToSet.emplace_back(m_VarIndexCounter, reinterpret_cast<void*>(peds), SCRIPT_RESULT_VAR_PED);
    AddBytes(reinterpret_cast<uint8*>(&m_VarIndexCounter), 2);
    m_VarIndexCounter++;
}

void ScriptCode::operator<<(CVehicle** vehicles) {
    AddParameterDescription(SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE);
    if (m_VarIndexCounter >= SCRIPT_COMMANDS_LVAR_MAX_INDEX) {
        assert("ScriptCode::operator<<(CVehicle **p): reached local var limit");
    }
    m_VarsToSet.emplace_back(m_VarIndexCounter, reinterpret_cast<void*>(vehicles), SCRIPT_RESULT_VAR_VEHICLE);
    AddBytes(reinterpret_cast<uint8*>(&m_VarIndexCounter), 2);
    m_VarIndexCounter++;
}

void ScriptCode::operator<<(CObject** objs) {
    AddParameterDescription(SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE);
    if (m_VarIndexCounter >= SCRIPT_COMMANDS_LVAR_MAX_INDEX) {
        assert("ScriptCode::operator<<(CObject **p): reached local var limit");
    }
    m_VarsToSet.emplace_back(m_VarIndexCounter, reinterpret_cast<void*>(objs), SCRIPT_RESULT_VAR_OBJECT);
    AddBytes(reinterpret_cast<uint8*>(&m_VarIndexCounter), 2);
    m_VarIndexCounter++;
}

void ScriptCode::operator<<(const CVector& vec) {
    for (auto& component : { vec.x, vec.y, vec.z }) {
        operator<<(component);
    }
}
