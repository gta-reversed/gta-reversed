#pragma once

/** This enum pretty much follows CLEO's `OpcodeResult` enum  */
enum OpcodeResult : int8 {
    OR_INTERRUPT = 1,
    OR_CONTINUE = 0,
    OR_ERROR = -1,
};
