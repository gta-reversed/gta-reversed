#include "StdInc.h"
#include "OALBase.h"

#ifdef USE_OPENAL
OALBase::OALBase() {
    m_refCount = 1;
    ++livingCount;
}

OALBase::~OALBase() {
    --livingCount;
}

void OALBase::Release() {
    if (--m_refCount)
        return;

    // OS_MutexObtain(trashMutex);
    trashCan.push_back(this);
    // OS_MutexRelease(trashMutex);
}

void OALBase::AddRef() {
    ++m_refCount;
}

bool OALCheckErrors(std::string_view file, int32 line) {
    const auto error = alGetError();
    if (error == AL_NO_ERROR)
        return true;

    const auto message = [error] {
        switch (error) {
        case AL_INVALID_NAME:
            return "Invalid name(ID) passed to an AL call.";
        case AL_INVALID_ENUM:
            return "Invalid enumeration passed to AL call.";
        case AL_INVALID_VALUE:
            return "Invalid value passed to AL call.";
        case AL_INVALID_OPERATION:
            return "Illegal AL call.";
        case AL_OUT_OF_MEMORY:
            return "Not enough memory to execute the AL call.";
        default:
            return "Unknown?";
        }
    }();

    NOTSA_LOG_ERR("OpenAL error at '{}' line {}: {}", file, line, message);
    return false;
}
#endif
