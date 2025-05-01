#include "StdInc.h"

#include "FxTools.h"

#ifdef _DEBUG
//#   define TRACE_ALL_FX_SHIT
#endif

#if defined TRACE_ALL_FX_SHIT
static Int32 _gLine;
#endif

bool ReadLine(FILESTREAM file, char* buffer, int32 bufferSize) {
    if (CFileMgr::ReadLine(file, buffer, bufferSize)) {
#if defined TRACE_ALL_FX_SHIT
        _DEVLOG("FX ReadLine %4d: %s", ++_gLine, buffer);
#endif
        return true;
    }
    return false;
}
