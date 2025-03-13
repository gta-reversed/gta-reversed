#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>
#include <functional>

/*
* Basic language feature commands (Comparasions, assingments, etc...)
*/

namespace {
//
// GREATER_OR_EQUAL
//
template<typename T>
bool IsGreaterEqual(T lhs, T rhs) {
    return lhs >= rhs;
}

//
// GREATER
//
template<typename T>
bool IsGreater(T lhs, T rhs) {
    return lhs > rhs;
}

//
// EQUAL
//
template<typename T>
bool IsEqual(T lhs, T rhs) {
    return lhs == rhs;
}

//
// SUB
//
template<typename T>
void SubInPlace(T& lhs, T rhs) {
    lhs -= (T)(rhs);
}

//
// ADD
//
template<typename T>
void AddInPlace(T& lhs, T rhs) {
    lhs += (T)(rhs);
}

//
// MULT
//
template<typename T>
void MultInPlace(T& lhs, T rhs) {
    lhs *= (T)(rhs);
}

//
// DIV
//
template<typename T>
void DivInPlace(T& lhs, T rhs) {
    lhs /= (T)(rhs);
}

//
// SET (Used for casting float <=> int too)
//
template<typename T, typename Y>
void AssignTo(T& lhs, Y rhs) {
    lhs = (T)(rhs);
}

//
// SUB TIMED
//
void SubTimedStore(float& lhs, float rhs) {
    lhs -= CTimer::GetTimeStep() * rhs;
}

//
// ADD TIMED
//
void AddTimedStore(float& lhs, float rhs) {
    lhs += CTimer::GetTimeStep() * rhs;
}

//
// ABS
//
template<typename T>
void AbsStore(T& value) {
    value = std::abs(value);
}

//
// GOTO
//
auto GoTo(CRunningScript& S, int32 address) {
    S.UpdatePC(address);
}

auto GoToIfFalse(CRunningScript& S, int32 goToAddress) {
    if (!S.m_bCondResult) {
        S.UpdatePC(goToAddress);
    }
}

auto GoToSub(CRunningScript& S, int32 goToAddress) { // 0x050 
    S.m_IPStack[S.m_StackDepth++] = S.m_IP;
    S.UpdatePC(goToAddress);
}

//
// RETURN
//

//! Return true (from function)
auto ReturnTrue() { 
    return true; 
}

//! Return false (from function)
auto ReturnFalse() { 
    return false; 
}

//! Jump back to calle
auto Return(CRunningScript& S) { 
    S.m_IP = S.m_IPStack[--S.m_StackDepth]; 
}

//
// LOGICAL OPS
//

// OR, AND
auto AndOr(CRunningScript& S, int32 logicalOp) { // 0x0D6
    // Read comment above `CRunningScript::LogicalOpType` for a little more insight!
    S.m_nLogicalOp = logicalOp;
    if (S.m_nLogicalOp == CRunningScript::ANDOR_NONE) {
        S.m_bCondResult = false;
    } else if (S.m_nLogicalOp >= CRunningScript::ANDS_1 && S.m_nLogicalOp <= CRunningScript::ANDS_8) { // It's an `AND`
        S.m_nLogicalOp++;
        S.m_bCondResult = true;
    } else if (S.m_nLogicalOp >= CRunningScript::ORS_1 && S.m_nLogicalOp <= CRunningScript::ORS_8) { // It's an `OR`
        S.m_nLogicalOp++;
        S.m_bCondResult = false;
    } else {
        NOTSA_UNREACHABLE("Unknown LogicalOp: {}", logicalOp);
    }
    return OR_CONTINUE;
}

//
// Script loading, stopping
//

// COMMAND_TERMINATE_THIS_SCRIPT
auto TerminateThisScript(CRunningScript& S) { // 0x04E 
    if (S.m_bIsMission) {
        CTheScripts::bAlreadyRunningAMissionScript = false;
    }
    S.RemoveScriptFromList(&CTheScripts::pActiveScripts);
    S.AddScriptToList(&CTheScripts::pIdleScripts);
    S.ShutdownThisScript();
    return OR_WAIT;
}

auto StartNewScript(CRunningScript& S, int32 offset) { // 0x04F
    assert(offset >= 0);
    /* Weird code that would probably just make the game crash
    if (offset < 0) {// This doesn't make sense
        offset = COMMAND_START_NEW_SCRIPT; // For Mobile, offset is set to 0 here. WD fix perhaps?
    }
    */

    CRunningScript* script = CTheScripts::StartNewScript(&CTheScripts::ScriptSpace[offset]);
    S.ReadParametersForNewlyStartedScript(script);
    return OR_CONTINUE;
}

//
// Other misc stuff (todo: move to appropriate categories)
//
void DebugOn() {
#ifndef FINAL
    CTheScripts::DbgFlag = true;
#endif
}

void DebugOff() {
#ifndef FINAL
    CTheScripts::DbgFlag = false;
#endif
}

auto Wait(CRunningScript& S, uint32 duration) {
    S.m_nWakeTime = CTimer::GetTimeInMS() + duration;
    return OR_WAIT;
}

auto Nop() {
    /* Hello! How are you? */
}
};

void notsa::script::commands::basic::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Basic");

    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_VAR_GREATER_OR_EQUAL_TO_NUMBER, IsGreaterEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_LVAR_GREATER_OR_EQUAL_TO_INT_LVAR, IsGreaterEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CONSTANT_GREATER_OR_EQUAL_TO_INT_VAR, IsGreaterEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CONSTANT_GREATER_OR_EQUAL_TO_INT_LVAR, IsGreaterEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_NUMBER_GREATER_OR_EQUAL_TO_INT_VAR, IsGreaterEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_VAR_GREATER_OR_EQUAL_TO_INT_VAR, IsGreaterEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_LVAR_GREATER_OR_EQUAL_TO_INT_VAR, IsGreaterEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_VAR_GREATER_OR_EQUAL_TO_CONSTANT, IsGreaterEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_VAR_GREATER_OR_EQUAL_TO_INT_LVAR, IsGreaterEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_LVAR_GREATER_OR_EQUAL_TO_NUMBER, IsGreaterEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_LVAR_GREATER_OR_EQUAL_TO_CONSTANT, IsGreaterEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_VAR_GREATER_OR_EQUAL_TO_FLOAT_LVAR, IsGreaterEqual<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_NUMBER_GREATER_OR_EQUAL_TO_INT_LVAR, IsGreaterEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_NUMBER_GREATER_OR_EQUAL_TO_FLOAT_VAR, IsGreaterEqual<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_LVAR_GREATER_OR_EQUAL_TO_NUMBER, IsGreaterEqual<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_VAR_GREATER_OR_EQUAL_TO_FLOAT_VAR, IsGreaterEqual<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_LVAR_GREATER_OR_EQUAL_TO_FLOAT_VAR, IsGreaterEqual<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_LVAR_GREATER_OR_EQUAL_TO_FLOAT_LVAR, IsGreaterEqual<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_VAR_GREATER_OR_EQUAL_TO_NUMBER, IsGreaterEqual<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_NUMBER_GREATER_OR_EQUAL_TO_FLOAT_LVAR, IsGreaterEqual<float>);

    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_LVAR_GREATER_THAN_NUMBER, IsGreater<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CONSTANT_GREATER_THAN_INT_VAR, IsGreater<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_VAR_GREATER_THAN_INT_LVAR, IsGreater<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CONSTANT_GREATER_THAN_INT_LVAR, IsGreater<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_LVAR_GREATER_THAN_INT_LVAR, IsGreater<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_VAR_GREATER_THAN_INT_VAR, IsGreater<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_LVAR_GREATER_THAN_INT_VAR, IsGreater<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_VAR_GREATER_THAN_NUMBER, IsGreater<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_VAR_GREATER_THAN_CONSTANT, IsGreater<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_LVAR_GREATER_THAN_CONSTANT, IsGreater<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_NUMBER_GREATER_THAN_INT_LVAR, IsGreater<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_NUMBER_GREATER_THAN_INT_VAR, IsGreater<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_VAR_GREATER_THAN_FLOAT_VAR, IsGreater<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_LVAR_GREATER_THAN_FLOAT_VAR, IsGreater<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_VAR_GREATER_THAN_NUMBER, IsGreater<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_NUMBER_GREATER_THAN_FLOAT_LVAR, IsGreater<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_LVAR_GREATER_THAN_NUMBER, IsGreater<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_LVAR_GREATER_THAN_FLOAT_LVAR, IsGreater<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_NUMBER_GREATER_THAN_FLOAT_VAR, IsGreater<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_VAR_GREATER_THAN_FLOAT_LVAR, IsGreater<float>);

    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_VAR_EQUAL_TO_INT_LVAR, IsEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_VAR_EQUAL_TO_NUMBER, IsEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_LVAR_EQUAL_TO_NUMBER, IsEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_VAR_EQUAL_TO_INT_VAR, IsEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_INT_LVAR_EQUAL_TO_INT_LVAR, IsEqual<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_VAR_EQUAL_TO_FLOAT_LVAR, IsEqual<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_VAR_EQUAL_TO_NUMBER, IsEqual<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_LVAR_EQUAL_TO_NUMBER, IsEqual<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_LVAR_EQUAL_TO_FLOAT_LVAR, IsEqual<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FLOAT_VAR_EQUAL_TO_FLOAT_VAR, IsEqual<float>);

    REGISTER_COMMAND_HANDLER(COMMAND_SUB_INT_LVAR_FROM_INT_LVAR, SubInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_INT_VAR_FROM_INT_LVAR, SubInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_INT_LVAR_FROM_INT_VAR, SubInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_VAL_FROM_INT_LVAR, SubInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_VAL_FROM_INT_VAR, SubInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_INT_VAR_FROM_INT_VAR, SubInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_FLOAT_VAR_FROM_FLOAT_LVAR, SubInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_VAL_FROM_FLOAT_LVAR, SubInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_FLOAT_LVAR_FROM_FLOAT_LVAR, SubInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_VAL_FROM_FLOAT_VAR, SubInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_FLOAT_VAR_FROM_FLOAT_VAR, SubInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_FLOAT_LVAR_FROM_FLOAT_VAR, SubInPlace<float>);

    REGISTER_COMMAND_HANDLER(COMMAND_ADD_INT_VAR_TO_INT_LVAR, AddInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_INT_VAR_TO_INT_VAR, AddInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_INT_LVAR_TO_INT_LVAR, AddInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_VAL_TO_INT_LVAR, AddInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_INT_LVAR_TO_INT_VAR, AddInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_VAL_TO_INT_VAR, AddInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_VAL_TO_FLOAT_LVAR, AddInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_FLOAT_VAR_TO_FLOAT_VAR, AddInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_FLOAT_LVAR_TO_FLOAT_LVAR, AddInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_FLOAT_VAR_TO_FLOAT_LVAR, AddInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_VAL_TO_FLOAT_VAR, AddInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_FLOAT_LVAR_TO_FLOAT_VAR, AddInPlace<float>);

    REGISTER_COMMAND_HANDLER(COMMAND_MULT_INT_LVAR_BY_INT_VAR, MultInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_MULT_INT_VAR_BY_INT_VAR, MultInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_MULT_INT_VAR_BY_VAL, MultInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_MULT_INT_LVAR_BY_VAL, MultInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_MULT_INT_LVAR_BY_INT_LVAR, MultInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_MULT_INT_VAR_BY_INT_LVAR, MultInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_MULT_FLOAT_LVAR_BY_VAL, MultInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_MULT_FLOAT_LVAR_BY_FLOAT_VAR, MultInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_MULT_FLOAT_VAR_BY_FLOAT_LVAR, MultInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_MULT_FLOAT_VAR_BY_VAL, MultInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_MULT_FLOAT_VAR_BY_FLOAT_VAR, MultInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_MULT_FLOAT_LVAR_BY_FLOAT_LVAR, MultInPlace<float>);

    REGISTER_COMMAND_HANDLER(COMMAND_DIV_INT_VAR_BY_INT_VAR, DivInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_DIV_INT_LVAR_BY_VAL, DivInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_DIV_INT_VAR_BY_INT_LVAR, DivInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_DIV_INT_VAR_BY_VAL, DivInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_DIV_INT_LVAR_BY_INT_LVAR, DivInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_DIV_INT_LVAR_BY_INT_VAR, DivInPlace<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_DIV_FLOAT_VAR_BY_FLOAT_VAR, DivInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_DIV_FLOAT_LVAR_BY_FLOAT_VAR, DivInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_DIV_FLOAT_LVAR_BY_FLOAT_LVAR, DivInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_DIV_FLOAT_VAR_BY_FLOAT_LVAR, DivInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_DIV_FLOAT_VAR_BY_VAL, DivInPlace<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_DIV_FLOAT_LVAR_BY_VAL, DivInPlace<float>);

    REGISTER_COMMAND_HANDLER(COMMAND_SET_VAR_INT_TO_LVAR_INT,      (AssignTo<int32, int32>));
    REGISTER_COMMAND_HANDLER(COMMAND_SET_VAR_INT_TO_VAR_INT,       (AssignTo<int32, int32>));
    REGISTER_COMMAND_HANDLER(COMMAND_SET_LVAR_INT_TO_CONSTANT,     (AssignTo<int32, int32>));
    REGISTER_COMMAND_HANDLER(COMMAND_SET_VAR_INT_TO_CONSTANT,      (AssignTo<int32, int32>));
    REGISTER_COMMAND_HANDLER(COMMAND_SET_LVAR_INT_TO_VAR_INT,      (AssignTo<int32, int32>));
    REGISTER_COMMAND_HANDLER(COMMAND_SET_LVAR_INT_TO_LVAR_INT,     (AssignTo<int32, int32>));
    REGISTER_COMMAND_HANDLER(COMMAND_SET_VAR_INT,                  (AssignTo<int32, int32>));
    REGISTER_COMMAND_HANDLER(COMMAND_SET_LVAR_INT,                 (AssignTo<int32, int32>));
    REGISTER_COMMAND_HANDLER(COMMAND_SET_VAR_FLOAT,                (AssignTo<float, float>));
    REGISTER_COMMAND_HANDLER(COMMAND_SET_LVAR_FLOAT,               (AssignTo<float, float>));
    REGISTER_COMMAND_HANDLER(COMMAND_SET_LVAR_FLOAT_TO_VAR_FLOAT,  (AssignTo<float, float>));
    REGISTER_COMMAND_HANDLER(COMMAND_SET_VAR_FLOAT_TO_VAR_FLOAT,   (AssignTo<float, float>));
    REGISTER_COMMAND_HANDLER(COMMAND_SET_LVAR_FLOAT_TO_LVAR_FLOAT, (AssignTo<float, float>));
    REGISTER_COMMAND_HANDLER(COMMAND_SET_VAR_FLOAT_TO_LVAR_FLOAT,  (AssignTo<float, float>));
    REGISTER_COMMAND_HANDLER(COMMAND_CSET_VAR_INT_TO_LVAR_FLOAT,   (AssignTo<int32, float>));
    REGISTER_COMMAND_HANDLER(COMMAND_CSET_LVAR_INT_TO_LVAR_FLOAT,  (AssignTo<int32, float>));
    REGISTER_COMMAND_HANDLER(COMMAND_CSET_VAR_INT_TO_VAR_FLOAT,    (AssignTo<int32, float>));
    REGISTER_COMMAND_HANDLER(COMMAND_CSET_LVAR_INT_TO_VAR_FLOAT,   (AssignTo<int32, float>));
    REGISTER_COMMAND_HANDLER(COMMAND_CSET_VAR_FLOAT_TO_VAR_INT,    (AssignTo<float, int32>));
    REGISTER_COMMAND_HANDLER(COMMAND_CSET_LVAR_FLOAT_TO_LVAR_INT,  (AssignTo<float, int32>));
    REGISTER_COMMAND_HANDLER(COMMAND_CSET_LVAR_FLOAT_TO_VAR_INT,   (AssignTo<float, int32>));
    REGISTER_COMMAND_HANDLER(COMMAND_CSET_VAR_FLOAT_TO_LVAR_INT,   (AssignTo<float, int32>));

    REGISTER_COMMAND_HANDLER(COMMAND_SUB_TIMED_FLOAT_VAR_FROM_FLOAT_LVAR, SubTimedStore);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_TIMED_FLOAT_LVAR_FROM_FLOAT_LVAR, SubTimedStore);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_TIMED_FLOAT_LVAR_FROM_FLOAT_VAR, SubTimedStore);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_TIMED_VAL_FROM_FLOAT_VAR, SubTimedStore);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_TIMED_FLOAT_VAR_FROM_FLOAT_VAR, SubTimedStore);
    REGISTER_COMMAND_HANDLER(COMMAND_SUB_TIMED_VAL_FROM_FLOAT_LVAR, SubTimedStore);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_TIMED_VAL_TO_FLOAT_VAR, AddTimedStore);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_TIMED_VAL_TO_FLOAT_LVAR, AddTimedStore);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_TIMED_FLOAT_VAR_TO_FLOAT_VAR, AddTimedStore);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_TIMED_FLOAT_LVAR_TO_FLOAT_VAR, AddTimedStore);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_TIMED_FLOAT_LVAR_TO_FLOAT_LVAR, AddTimedStore);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_TIMED_FLOAT_VAR_TO_FLOAT_LVAR, AddTimedStore);

    REGISTER_COMMAND_HANDLER(COMMAND_ABS_VAR_INT, AbsStore<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_ABS_LVAR_INT, AbsStore<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_ABS_VAR_FLOAT, AbsStore<float>);
    REGISTER_COMMAND_HANDLER(COMMAND_ABS_LVAR_FLOAT, AbsStore<float>);

    REGISTER_COMMAND_HANDLER(COMMAND_GOTO, GoTo);
    //REGISTER_COMMAND_HANDLER(COMMAND_GOTO_IF_FALSE, GoToIfFalse);
    REGISTER_COMMAND_HANDLER(COMMAND_GOSUB,  GoToSub);
    REGISTER_COMMAND_HANDLER(COMMAND_RETURN_TRUE, ReturnTrue);
    REGISTER_COMMAND_HANDLER(COMMAND_RETURN_FALSE, ReturnFalse);
    REGISTER_COMMAND_HANDLER(COMMAND_RETURN, Return);

    REGISTER_COMMAND_HANDLER(COMMAND_ANDOR, AndOr);

    REGISTER_COMMAND_HANDLER(COMMAND_TERMINATE_THIS_SCRIPT,  TerminateThisScript);
    REGISTER_COMMAND_HANDLER(COMMAND_START_NEW_SCRIPT, StartNewScript);
    REGISTER_COMMAND_HANDLER(COMMAND_DEBUG_ON, DebugOn);
    REGISTER_COMMAND_HANDLER(COMMAND_DEBUG_OFF, DebugOff);
    REGISTER_COMMAND_HANDLER(COMMAND_WAIT, Wait);
    REGISTER_COMMAND_HANDLER(COMMAND_NOP, Nop);

    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_INT_VAR_NOT_EQUAL_TO_NUMBER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_INT_LVAR_NOT_EQUAL_TO_NUMBER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_INT_VAR_NOT_EQUAL_TO_INT_VAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_INT_LVAR_NOT_EQUAL_TO_INT_LVAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_INT_VAR_NOT_EQUAL_TO_INT_LVAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_FLOAT_VAR_NOT_EQUAL_TO_NUMBER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_FLOAT_LVAR_NOT_EQUAL_TO_NUMBER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_FLOAT_VAR_NOT_EQUAL_TO_FLOAT_VAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_FLOAT_LVAR_NOT_EQUAL_TO_FLOAT_LVAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_FLOAT_VAR_NOT_EQUAL_TO_FLOAT_LVAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GOTO_IF_TRUE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LINE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IF);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IFNOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ELSE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ENDIF);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_WHILE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_WHILENOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ENDWHILE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_REPEAT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_VAR_INT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_VAR_FLOAT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LVAR_INT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LVAR_FLOAT);
}
