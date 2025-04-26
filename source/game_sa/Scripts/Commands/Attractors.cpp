#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>

namespace {
// COMMAND_ADD_ATTRACTOR - 0x491AC0
int32 AddAttractor(CRunningScript& S, CVector pos, float queueDirDeg, float fwdDirDeg, tScriptSequence* seq) { // TODO: Returns `C2dEffect` as a scriptthing
    if (!seq) {
        return -1;
    }

    const auto fxID = CScripted2dEffects::AddScripted2DEffect();
    if (fxID > NUM_SCRIPTED_2D_EFFECTS) {
        return -1;
    }

    auto* const a = new (&CScripted2dEffects::Get(fxID)) C2dEffectPedAttractor;
    a->m_Type           = EFFECT_ATTRACTOR;
    a->m_nAttractorType = PED_ATTRACTOR_SCRIPTED;
    a->m_Pos            = pos;
    a->m_vecUseDir      = CVector{ 0.f, -std::sin(DegreesToRadians(queueDirDeg)), std::cos(DegreesToRadians(queueDirDeg)) }.Normalized();
    a->m_vecQueueDir    = a->m_vecUseDir;
    a->m_vecForwardDir  = CVector{ 0.f, -std::sin(DegreesToRadians(fwdDirDeg)), std::cos(DegreesToRadians(fwdDirDeg)) }.Normalized();

    NOTSA_UNREACHABLE();
    //CScripted2dEffects::ms_effectSequenceTaskIDs[fxID] = ; // todo: use seq's script ID here

    const auto fxScriptID = CTheScripts::GetNewUniqueScriptThingIndex(fxID, SCRIPT_THING_2D_EFFECT);
    if (S.m_bUseMissionCleanup) {
        CTheScripts::MissionCleanUp.AddEntityToList(fxScriptID, MISSION_CLEANUP_ENTITY_TYPE_PED_QUEUE);
    }
    return fxScriptID;
}

// COMMAND_CLEAR_ATTRACTOR - 0x491C37
void ClearAttractor(C2dEffectPedAttractor* attractor) {
    // todo
}

// COMMAND_CLEAR_ALL_ATTRACTORS
void ClearAllAttractors(...) {

}

// COMMAND_CREATE_CHAR_AT_ATTRACTOR
void CreateCharAtAttractor(...) {

}

// COMMAND_ADD_PEDMODEL_AS_ATTRACTOR_USER
void AddPedmodelAsAttractorUser(...) {

}

// COMMAND_SET_ATTRACTOR_PAIR
void SetAttractorPair(...) {

}

// COMMAND_CREATE_PED_GENERATOR_AT_ATTRACTOR
void CreatePedGeneratorAtAttractor(...) {

}

// COMMAND_ADD_PEDTYPE_AS_ATTRACTOR_USER
void AddPedtypeAsAttractorUser(...) {

}

// COMMAND_TASK_USE_ATTRACTOR
void TaskUseAttractor(...) {

}

// COMMAND_SET_ATTRACTOR_AS_COVER_NODE
void SetAttractorAsCoverNode(...) {

}

// COMMAND_REGISTER_ATTRACTOR_SCRIPT_BRAIN_FOR_CODE_USE
void RegisterAttractorScriptBrainForCodeUse(...) {

}

// COMMAND_ALLOCATE_SCRIPT_TO_ATTRACTOR
void AllocateScriptToAttractor(...) {

}

// COMMAND_GET_CLOSEST_ATTRACTOR_WITH_THIS_SCRIPT
void GetClosestAttractorWithThisScript(...) {

}

// COMMAND_TASK_USE_CLOSEST_MAP_ATTRACTOR
void TaskUseClosestMapAttractor(...) {

}

// COMMAND_GET_USER_OF_CLOSEST_MAP_ATTRACTOR
void GetUserOfClosestMapAttractor(...) {

}
};

void notsa::script::commands::attractors::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_ATTRACTOR, AddAttractor);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_ATTRACTOR, ClearAttractor);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CLEAR_ALL_ATTRACTORS);
    REGISTER_COMMAND_HANDLER(COMMAND_CREATE_CHAR_AT_ATTRACTOR, CreateCharAtAttractor);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_PEDMODEL_AS_ATTRACTOR_USER, AddPedmodelAsAttractorUser);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_ATTRACTOR_PAIR, SetAttractorPair);
    REGISTER_COMMAND_HANDLER(COMMAND_CREATE_PED_GENERATOR_AT_ATTRACTOR, CreatePedGeneratorAtAttractor);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_PEDTYPE_AS_ATTRACTOR_USER, AddPedtypeAsAttractorUser);
    REGISTER_COMMAND_HANDLER(COMMAND_TASK_USE_ATTRACTOR, TaskUseAttractor);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_ATTRACTOR_AS_COVER_NODE, SetAttractorAsCoverNode);
    REGISTER_COMMAND_HANDLER(COMMAND_REGISTER_ATTRACTOR_SCRIPT_BRAIN_FOR_CODE_USE, RegisterAttractorScriptBrainForCodeUse);
    REGISTER_COMMAND_HANDLER(COMMAND_ALLOCATE_SCRIPT_TO_ATTRACTOR, AllocateScriptToAttractor);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CLOSEST_ATTRACTOR_WITH_THIS_SCRIPT, GetClosestAttractorWithThisScript);
    REGISTER_COMMAND_HANDLER(COMMAND_TASK_USE_CLOSEST_MAP_ATTRACTOR, TaskUseClosestMapAttractor);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_USER_OF_CLOSEST_MAP_ATTRACTOR, GetUserOfClosestMapAttractor);
}
