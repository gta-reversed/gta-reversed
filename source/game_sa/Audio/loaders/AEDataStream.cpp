#include "StdInc.h" // TODO: Remove

#include "common.h"

#include "AEDataStream.h"
#include "AEStreamTransformer.h"
#include "FileMgr.h"

// originally startPosition and length is int32
// 0x4dc620
CAEDataStream::CAEDataStream(int32 trackId, char* filename, uint32 startPosition, uint32 length, bool encrypted) {
    m_pFileHandle      = nullptr;
    m_pszFilename      = filename;
    m_bIsOpen          = false;
    m_nCurrentPosition = 0;
    m_nStartPosition   = startPosition;
    m_nLength          = length;
    m_nTrackId         = trackId;
    m_bIsEncrypted     = encrypted;
    m_lRefCount        = 1;
}

// 0x4dc490
CAEDataStream::~CAEDataStream() {
    Close();

    if (m_pszFilename) {
        delete[] m_pszFilename;
        m_pszFilename = nullptr;
    }
}

// 0x4dc410
HRESULT CAEDataStream::QueryInterface(REFIID riid, void** objout) {
    // MikuAuahDark: Despite having QueryInterface implemented,
    // I don't think CAEDataStream is meant to be QueryInterface'd.
    // See CAEDataStream::Release
    if (riid == IID_IUnknown || riid == IID_IStream) {
        AddRef();
        *objout = this;
        return S_OK;
    }

    return E_NOINTERFACE;
}

// 0x4dc460
ULONG CAEDataStream::AddRef() {
    return ++m_lRefCount;
}

// 0x4dc5b0
ULONG CAEDataStream::Release() {
    if (--m_lRefCount == 0) {
        delete this;
        return 0;
    }

    // MikuAuahDark: I think CAEDataStream isn't mean to be
    // QueryInterface'd? 0x4dc608 clearly says "mov eax, 0xbad"
    return 0xbad;
}

// 0x4dc320
HRESULT CAEDataStream::Read(void* dest, ULONG size, ULONG* readed) {
    *readed = static_cast<ULONG>(FillBuffer(dest, size));
    return S_OK;
}

// 0x4dc4d0
HRESULT CAEDataStream::Write(const void* src, ULONG size, ULONG* written) {
    return E_NOTIMPL;
}

// 0x4dc340
HRESULT CAEDataStream::Seek(LARGE_INTEGER offset, DWORD whence, ULARGE_INTEGER* newOffset) {
    // C-style cast is not a good idea here
    // but I can't figure out which cast is best to preserve
    // the sign-bit of LARGE_INTEGER.
    //
    // TODO: Implement 64-bit variants?

    if (!m_bIsOpen)
        return E_INVALIDARG;

    assert(std::in_range<long>(offset.QuadPart));
    unsigned long pos = Seek(static_cast<long>(offset.QuadPart), static_cast<int32>(whence));
    if (newOffset)
        newOffset->QuadPart = pos;

    return S_OK;
}

// 0x4dc4e0
HRESULT CAEDataStream::SetSize(ULARGE_INTEGER newSize) {
    return E_NOTIMPL;
}

// 0x4dc4f0
HRESULT CAEDataStream::CopyTo(IStream* target, ULARGE_INTEGER size, ULARGE_INTEGER* readed, ULARGE_INTEGER* written) {
    return E_NOTIMPL;
}

// 0x4dc500
HRESULT CAEDataStream::Commit(DWORD flags) {
    return E_NOTIMPL;
}

// 0x4dc510
HRESULT CAEDataStream::Revert() {
    return E_NOTIMPL;
}

// 0x4dc520
HRESULT CAEDataStream::LockRegion(ULARGE_INTEGER offset, ULARGE_INTEGER size, DWORD type) {
    return E_NOTIMPL;
}

// 0x4dc530
HRESULT CAEDataStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) {
    return E_NOTIMPL;
}

// 0x4dc3a0
HRESULT CAEDataStream::Stat(STATSTG* statout, DWORD flags) {
    if (!m_bIsOpen || statout == nullptr || flags != STATFLAG_NONAME)
        return E_INVALIDARG;

    auto fileSize = CFileMgr::GetTotalSize(m_pFileHandle);
    memset(statout, 0, sizeof(STATSTG));
    statout->cbSize.LowPart = fileSize;
    statout->type = STGTY_STREAM;
    return S_OK;
}

// 0x4dc540
HRESULT CAEDataStream::Clone(IStream** target) {
    return E_NOTIMPL;
}

// 0x4dc1c0
size_t CAEDataStream::FillBuffer(void* dest, size_t size) {
    if (!m_bIsOpen)
        return 0;

    size_t sizeToRead = static_cast<size_t>((m_nCurrentPosition - m_nLength) - m_nStartPosition);
    if (size < sizeToRead)
        sizeToRead = size;

    size = CFileMgr::Read(m_pFileHandle, dest, sizeToRead);

    if (m_bIsEncrypted)
        AEStreamTransformer.TransformBuffer(dest, size, m_nCurrentPosition);

    m_nCurrentPosition += static_cast<uint32>(size);
    return size;
}

// 0x4dc230
uint32 CAEDataStream::GetCurrentPosition() {
    uint32 pos = static_cast<uint32>(CFileMgr::Tell(m_pFileHandle));
    return pos - m_nStartPosition;
}

// 0x4dc250
uint32 CAEDataStream::Seek(long offset, int32 whence) {
    if (!m_bIsOpen)
        return uint32(-1);

    if (whence == SEEK_SET) {
        offset += m_nStartPosition;
    } else if (whence == SEEK_END) {
        offset = (m_nLength + m_nStartPosition) - offset;
        whence = SEEK_SET;
    }

    CFileMgr::Seek(m_pFileHandle, offset, whence);

    m_nCurrentPosition = static_cast<uint32>(CFileMgr::Tell(m_pFileHandle));
    return m_nCurrentPosition - m_nStartPosition;
}

bool CAEDataStream::Close() {
    if (m_bIsOpen) {
        m_bIsOpen = false;
        return CFileMgr::CloseFile(m_pFileHandle);
    }

    return true;
}

// 0x4dc2b0
bool CAEDataStream::Initialise() {
    if (m_bIsOpen)
        return true;

    // NOTSA(MikuAuahDark): Rewrite it to use CFileMgr
    CFileMgr::SetDir("");
    m_pFileHandle = CFileMgr::OpenFile(m_pszFilename, "rb");

    if (m_pFileHandle) {
        CFileMgr::Seek(m_pFileHandle, m_nStartPosition, SEEK_SET);

        m_nCurrentPosition = static_cast<uint32>(CFileMgr::Tell(m_pFileHandle));
        m_nLength = m_nLength == 0 ? CFileMgr::GetTotalSize(m_pFileHandle) : m_nLength;
        m_bIsOpen = true;
    }

    return m_pFileHandle != nullptr;
}

void CAEDataStream::InjectHooks() {
    RH_ScopedClass(CAEDataStream);
    RH_ScopedCategory("Audio/Loaders");

    // NOTSA: This class used Win32 file functions to deal with files, we converted
    // them to C-compatible ones. So all of them have to be locked.

    RH_ScopedInstall(Constructor, 0x4dc620, {.locked = true});
    RH_ScopedInstall(Destructor, 0x4dc490, {.locked = true});
    RH_ScopedInstall(Initialise, 0x4dc2b0, {.locked = true});
    RH_ScopedInstall(FillBuffer, 0x4dc1c0, {.locked = true});
    RH_ScopedInstall(GetCurrentPosition, 0x4dc230, {.locked = true});
    RH_ScopedOverloadedInstall(Seek, "OG", 0x4dc250, uint32(CAEDataStream::*)(long offset, int32 whence), {.locked = true});

    // IStream implementation. These depend on functions above so most of them don't have to be locked.
    RH_ScopedInstall(Read, 0x4dc320);
    RH_ScopedOverloadedInstall(Seek, "istream", 0x4dc340, HRESULT(__stdcall CAEDataStream::*)(LARGE_INTEGER, DWORD, ULARGE_INTEGER*));
    RH_ScopedInstall(Stat, 0x4dc3a0, {.locked = false}); // Uses CFileMgr::GetTotalSize
    RH_ScopedInstall(QueryInterface, 0x4dc410);
    RH_ScopedInstall(AddRef, 0x4dc460);
    RH_ScopedInstall(Write, 0x4dc4d0);
    RH_ScopedInstall(SetSize, 0x4dc4e0);
    RH_ScopedInstall(CopyTo, 0x4dc4f0);
    RH_ScopedInstall(Commit, 0x4dc500);
    RH_ScopedInstall(Revert, 0x4dc510);
    RH_ScopedInstall(LockRegion, 0x4dc520);
    RH_ScopedInstall(UnlockRegion, 0x4dc530);
    RH_ScopedInstall(Clone, 0x4dc540);
    RH_ScopedInstall(Release, 0x4dc5b0);

    CAEStreamTransformer::InjectHooks();
}

CAEDataStream* CAEDataStream::Constructor(int32 trackID, char* filename, uint32 startPosition, uint32 length, int32 encrypted) {
    this->CAEDataStream::CAEDataStream(trackID, filename, startPosition, length, encrypted);
    return this;
}

void CAEDataStream::Destructor() {
    this->CAEDataStream::~CAEDataStream();
}
