#pragma once

enum OpcodeResult : int8 {
    OR_CONTINUE = 0,
    OR_WAIT = 1,
    OR_INTERRUPT = -1,
    OR_IMPLEMENTED_YET = -2 // NOTSA
};
