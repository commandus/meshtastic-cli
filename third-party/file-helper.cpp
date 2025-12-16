#if defined(_MSC_VER) || defined(__MINGW32__)
#pragma warning(disable: 4996)
#include <windows.h>
#include <io.h>
#include <cwchar>
#include <cstdio>
#include <shlobj.h>
#define PATH_DELIMITER "\\"
#else
#include <sys/param.h>
#include <fcntl.h>
#include <ftw.h>
#include <pwd.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <string.h>
#include <errno.h>
#include <cstdio>

#define PATH_DELIMITER "/"

#ifndef F_GETPATH
#define F_GETPATH	(1024 + 7)
#endif

#endif

#include <iostream>

#include "file-helper.h"

std::string getCurrentDir()
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    TCHAR buffer[MAX_PATH];
    GetCurrentDirectory(MAX_PATH - 1, buffer);
    return std::string((char *) buffer);
#else
    char wd[PATH_MAX];
    return getcwd(wd, PATH_MAX);
#endif
}

std::string getHomeDir()
{
#if defined(_MSC_VER) || defined(__MINGW32__)
	CHAR path[MAX_PATH];
	HRESULT result = SHGetFolderPathA(nullptr, CSIDL_PROFILE, nullptr, 0, path);
	if (!SUCCEEDED(result))
		return "";
	return std::string(path);
#else
	struct passwd *pw = getpwuid(getuid());
	return std::string(pw->pw_dir);
#endif
}

std::string getProgramDir()
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    // Windows
    CHAR path[MAX_PATH];
    HRESULT result = GetModuleFileNameA(nullptr,path,MAX_PATH);
    if (!SUCCEEDED(result))
        return "";
    return std::string(path);
#else
    // Linux
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    return std::string(path, (count > 0) ? count : 0);
#endif
}
