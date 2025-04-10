#include "StdInc.h"

#include <cerrno>
#include <cstring>
#include <direct.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShlObj.h>

#include "FileMgr.h"

#include "HookSystem.h"

char (&CFileMgr::ms_dirName)[128] = *(char (*)[128])0xb71a60;
char (&CFileMgr::ms_rootDirName)[128] = *(char (*)[128])0xb71ae0;

char (&user_tracks_dir_path)[256] = *(char (*)[256])0xc92168;
char (&user_gallery_dir_path)[256] = *(char (*)[256])0xc92268;
char (&gta_user_dir_path)[256] = *(char (*)[256])0xc92368;

constexpr size_t PATH_SIZE = 256;

inline void createDirectory(const wchar_t* path)
{
    HANDLE folderHandle = CreateFileW(
        path,
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    if (folderHandle == INVALID_HANDLE_VALUE)
        CreateDirectoryW(path, nullptr);
    else
        CloseHandle(folderHandle);
}

// 0x744FB0
char* InitUserDirectories()
{
    if (gta_user_dir_path[0] != '\0')
        return gta_user_dir_path;

    // MikuAuahDark: Let's improve the function
    // to use wide char

    static std::array<wchar_t, MAX_PATH> gtaUserDirWide;

    if (SHGetFolderPathW(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, gtaUserDirWide.data()) != S_OK) {
        strcpy_s(gta_user_dir_path, std::size("data"), "data"); // 2nd param is required or game won't be able to find files!
        return gta_user_dir_path;
    }

    constexpr const wchar_t* USERFILES = L"\\GTA San Andreas User Files";
    constexpr const wchar_t* GALLERY = L".\\Gallery";
    constexpr const wchar_t* USERTRACKS = L".\\User Tracks";
    static std::array<wchar_t, MAX_PATH> userGalleryDirWide;
    static std::array<wchar_t, MAX_PATH> userTracksDirWide;

    // Base GTASA User Files
    if ((wcslen(gtaUserDirWide.data()) + wcslen(USERFILES)) >= MAX_PATH)
        wcscpy_s(gtaUserDirWide.data(), gtaUserDirWide.size(), L".");
    else
        wcscat_s(gtaUserDirWide.data(), gtaUserDirWide.size(), USERFILES);
    createDirectory(gtaUserDirWide.data());

    size_t userDirLen = wcslen(gtaUserDirWide.data());
    wcscpy_s(userGalleryDirWide.data(), userGalleryDirWide.size(), gtaUserDirWide.data());
    wcscpy_s(userTracksDirWide.data(), userTracksDirWide.size(), gtaUserDirWide.data());

    // Gallery
    if ((userDirLen + wcslen(GALLERY + 1)) >= PATH_SIZE)
        wcscpy_s(userGalleryDirWide.data(), userGalleryDirWide.size(), GALLERY);
    else
        wcscat_s(userGalleryDirWide.data(), userGalleryDirWide.size(), GALLERY + 1);
    createDirectory(userGalleryDirWide.data());

    // User Tracks
    if ((userDirLen + wcslen(USERTRACKS + 1)) >= PATH_SIZE)
        wcscpy_s(userTracksDirWide.data(), userTracksDirWide.size(), USERTRACKS);
    else
        wcscat_s(userTracksDirWide.data(), userTracksDirWide.size(), USERTRACKS + 1);
    createDirectory(userTracksDirWide.data());

    std::string temp = UnicodeToUTF8(gtaUserDirWide.data());
    if (temp.length() >= PATH_SIZE)
        strcpy_s(gta_user_dir_path, ".");
    else
        strcpy_s(gta_user_dir_path, temp.c_str());

    temp = UnicodeToUTF8(userGalleryDirWide.data());
    if (temp.length() >= PATH_SIZE)
        strcpy_s(user_gallery_dir_path, ".\\Gallery");
    else
        strcpy_s(user_gallery_dir_path, temp.c_str());

    temp = UnicodeToUTF8(userTracksDirWide.data());
    if (temp.length() >= PATH_SIZE)
        strcpy_s(user_tracks_dir_path, ".\\User Tracks");
    else
        strcpy_s(user_tracks_dir_path, temp.c_str());

    return gta_user_dir_path;
}

// 0x5386f0
void CFileMgr::Initialise()
{
    memset(ms_rootDirName, 0, DIRNAMELENGTH);

    if (WindowsCharset != CP_UTF8)
    {
        // MikuAuahDark: Improve the function to use wide char
        wchar_t rootDirTemp[DIRNAMELENGTH];
        VERIFY(_wgetcwd(rootDirTemp, DIRNAMELENGTH));

        // Just in case
        if (errno == ERANGE)
            VERIFY(_getcwd(ms_rootDirName, DIRNAMELENGTH));
        else
        {
            std::string rootDirUTF8 = UnicodeToUTF8(rootDirTemp);
            if (rootDirUTF8.length() >= (DIRNAMELENGTH - 2))
                VERIFY(_getcwd(ms_rootDirName, DIRNAMELENGTH));
            else
                strcpy_s(ms_rootDirName, rootDirUTF8.size() + 1, rootDirUTF8.c_str());
        }
    }
    else
        VERIFY(_getcwd(ms_rootDirName, DIRNAMELENGTH));

    ms_rootDirName[strlen(ms_rootDirName)] = '\\';
}

// 0x538730
int32 CFileMgr::ChangeDir(const char *path)
{
    memset(ms_dirName, 0, DIRNAMELENGTH);

    if (*path == '\\')
    {
        memcpy(ms_dirName, ms_rootDirName, strlen(ms_rootDirName));
        path++;
    }

    if (*path)
    {
        if ((strlen(ms_dirName) + strlen(path) + 1) > DIRNAMELENGTH)
        {
            errno = EINVAL;
            return -1;
        }
        strcat_s(ms_dirName, path);

        char *lastPos = strchr(ms_dirName, 0) - 1;
        if (*lastPos != '\\')
            lastPos[1] = '\\';
    }

    int32 r;
    if (WindowsCharset != CP_UTF8)
    {
        std::wstring pathWide = UTF8ToUnicode(ms_dirName);
        r = _wchdir(pathWide.c_str());
    }
    else
        r = _chdir(ms_dirName);

    return r;
}

// 0x5387D0
int32 CFileMgr::SetDir(const char *path)
{
    memset(ms_dirName, 0, DIRNAMELENGTH);
    memcpy(ms_dirName, ms_rootDirName, strlen(ms_rootDirName));

    if (*path)
    {
        const auto destSizeAfterConcat = strlen(ms_dirName) + strlen(path) + 1;
        if (destSizeAfterConcat > DIRNAMELENGTH)
        {
            errno = EINVAL;
            return -1;
        }
        strcat_s(ms_dirName, destSizeAfterConcat, path);

        char *lastPos = strchr(ms_dirName, 0) - 1;
        if (*lastPos != '\\')
            lastPos[1] = '\\';
    }

    int32 r;
    if (WindowsCharset != CP_UTF8)
    {
        std::wstring pathWide = UTF8ToUnicode(ms_dirName);
        r = _wchdir(pathWide.c_str());
    }
    else
        r = _chdir(ms_dirName);

    return r;
}

// 0x538860
int32 CFileMgr::SetDirMyDocuments()
{
    char *userDir = InitUserDirectories();
    if (strlen(userDir) >= DIRNAMELENGTH)
    {
        errno = ENOMEM;
        return -1;
    }

    strcpy_s(ms_dirName, userDir);

    int32 r;
    if (WindowsCharset != CP_UTF8)
    {
        std::wstring pathWide = UTF8ToUnicode(ms_dirName);
        r = _wchdir(pathWide.c_str());
    }
    else
        r = _chdir(ms_dirName);

    return r;
}

// 0x538890
size_t CFileMgr::LoadFile(const char *path, uint8 *buf, size_t size, const char *mode)
{
    FILESTREAM f = CFileMgr::OpenFile(path, mode);
    if (f == nullptr)
        return -1;

    // MikuAuahDark: The original implementation actually ignore
    // "size" parameter and adds NUL terminator at the end of the
    // buffer, but that behavior causes buffer overflow
    size_t readed = fread(buf, 1, size, f);
    fclose(f);

    return readed;
}

// 0x538900
FILESTREAM CFileMgr::OpenFile(const char *path, const char *mode) {
    ZoneScoped;

    FILESTREAM fs{nullptr};
    if (WindowsCharset == CP_UTF8) {
        fopen_s(&fs, path, mode);
    } else {
        // MikuAuahDark: Let's improve it to allow opening non-ANSI names
        // Convert to wide char
        std::wstring pathWide = UTF8ToUnicode(path);
        std::wstring modeWide = UTF8ToUnicode(mode);
        _wfopen_s(&fs, pathWide.c_str(), modeWide.c_str());
    }
    return fs;
}

// 0x538910
FILESTREAM CFileMgr::OpenFileForWriting(const char *path)
{
    return CFileMgr::OpenFile(path, "wb");
}

// 0x538930
FILESTREAM CFileMgr::OpenFileForAppending(const char *path)
{
    return CFileMgr::OpenFile(path, "a");
}

// 0x538950
size_t CFileMgr::Read(FILESTREAM file, void *buf, size_t size)
{
    return fread(buf, 1, size, file);
}

// 0x538970
size_t CFileMgr::Write(FILESTREAM file, const void *buf, size_t size)
{
    return fwrite(buf, 1, size, file);
}

// 0x538990
bool CFileMgr::Seek(FILESTREAM file, long offset, int32 origin)
{
    // MikuAuahDark: Pretty sure it shouldn't be ret != 0
    return fseek(file, offset, origin) != 0;
}

// 0x5389b0
bool CFileMgr::ReadLine(FILESTREAM file, char *str, int32 num)
{
    return fgets(str, num, file) != nullptr;
}

// 0x5389d0
int32 CFileMgr::CloseFile(FILESTREAM file)
{
    return fclose(file);
}

// 0x5389e0
int32 CFileMgr::GetTotalSize(FILESTREAM file)
{
    int32 currentPos, size;
    // MikuAuahDark: The actual implementation uses RwOsGetFileInterface
    // but for sake of portability, default stdio functions is used.
    /*
    RwFileFunctions *functions = RwOsGetFileInterface();

    currentPos = functions->rwftell(file);
    functions->rwfseek(file, 0, SEEK_END);
    size = functions->rwftell(file);
    functions->rwfseek(file, currentPos, SEEK_SET);

    return size;
    */
    currentPos = ftell(file);
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, currentPos, SEEK_SET);

    return size;
}

// 0x538a20
int32 CFileMgr::Tell(FILESTREAM file)
{
    /*
    RwFileFunctions *functions = RwOsGetFileInterface();
    return functions->rwftell(file);
    */
    return ftell(file);
}

// 0x538a50
bool CFileMgr::GetErrorReadWrite(FILESTREAM file)
{
    return (bool) ferror(file);
}

// notsa
void CFileMgr::SeekNextLine(FILESTREAM file) {
    while (!feof(file) && fgetc(file) != '\n');
}

void CFileMgr::InjectHooks()
{
    RH_ScopedClass(CFileMgr);
    RH_ScopedCategoryGlobal();

    // File related functions locked to prevent crashes

    RH_ScopedInstall(Initialise, 0x5386f0);
    RH_ScopedInstall(ChangeDir, 0x538730);
    RH_ScopedInstall(SetDir, 0x5387D0);
    RH_ScopedInstall(SetDirMyDocuments, 0x538860, { .locked = true });
    RH_ScopedInstall(LoadFile, 0x538890, { .locked = true });
    RH_ScopedInstall(OpenFile, 0x538900, { .locked = true });
    RH_ScopedInstall(OpenFileForWriting, 0x538910, { .locked = true });
    RH_ScopedInstall(OpenFileForAppending, 0x538930, { .locked = true });
    RH_ScopedInstall(Read, 0x538950, { .locked = true });
    RH_ScopedInstall(Write, 0x538970, { .locked = true });
    RH_ScopedInstall(Seek, 0x538990, { .locked = true });
    RH_ScopedInstall(ReadLine, 0x5389b0, { .locked = true });
    RH_ScopedInstall(CloseFile, 0x5389d0, { .locked = true });
    RH_ScopedInstall(GetTotalSize, 0x5389e0, { .locked = true });
    RH_ScopedInstall(Tell, 0x538a20, { .locked = true });
    RH_ScopedInstall(GetErrorReadWrite, 0x538a50, { .locked = true });
    RH_ScopedGlobalInstall(InitUserDirectories, 0x744fb0);
}
