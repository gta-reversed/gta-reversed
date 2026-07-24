#include "StdInc.h"
#include "OALBase.h"
#include <mutex>

OALBase::OALBase() {
    m_RefCount = 1;
    ++s_LivingCount;
}

OALBase::~OALBase() {
    --s_LivingCount;
}

void OALBase::Release() {
    if (--m_RefCount)
        return;

    static std::once_flag s_TrashMutexInitFlag;
    std::call_once(s_TrashMutexInitFlag, [] {
        s_TrashMutex = OS_MutexCreate();
    });

    OS_MutexObtain(s_TrashMutex);
    s_TrashCan.push_back(std::unique_ptr<OALBase>(this));
    OS_MutexRelease(s_TrashMutex);
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
