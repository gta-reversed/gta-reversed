#pragma once
#include <span>
#include "FileMgr.h"

namespace notsa {
// RAII-wrapper for CFileMgr files.
class File {
public:
    File(const char* path, const char* mode) :
        m_file(CFileMgr::OpenFile(path, mode))
    {}

    ~File()
    {
        if (m_file)
            CloseFile();
    }

    operator bool() const { return m_file != nullptr; }

    auto Read(void* buf, size_t size) { return CFileMgr::Read(m_file, buf, size); }
    auto Write(const void* buf, size_t size) { return CFileMgr::Write(m_file, buf, size); }
    auto Seek(long offset, int32 origin) { return CFileMgr::Seek(m_file, offset, origin); }
    auto ReadLine(char* str, int32 num) { return CFileMgr::ReadLine(m_file, str, num); }
    auto CloseFile() { return CFileMgr::CloseFile(std::exchange(m_file, nullptr)); }
    auto GetTotalSize() const { return CFileMgr::GetTotalSize(m_file); }
    auto Tell() const { return CFileMgr::Tell(m_file); }
    auto GetErrorReadWrite() const { return CFileMgr::GetErrorReadWrite(m_file); }
    void SeekNextLine() { CFileMgr::SeekNextLine(m_file); }

private:
    FILESTREAM m_file{};
};
};
