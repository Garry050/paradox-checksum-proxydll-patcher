#include "proxy.h"
#include "log.h"

typedef BOOL  (WINAPI *pfn_GetFileVersionInfoA)(LPCSTR, DWORD, DWORD, LPVOID);
typedef BOOL  (WINAPI *pfn_GetFileVersionInfoByHandle)(DWORD, HANDLE, DWORD, DWORD, LPVOID);
typedef BOOL  (WINAPI *pfn_GetFileVersionInfoExA)(DWORD, LPCSTR, DWORD, DWORD, LPVOID);
typedef BOOL  (WINAPI *pfn_GetFileVersionInfoExW)(DWORD, LPCWSTR, DWORD, DWORD, LPVOID);
typedef DWORD (WINAPI *pfn_GetFileVersionInfoSizeA)(LPCSTR, LPDWORD);
typedef DWORD (WINAPI *pfn_GetFileVersionInfoSizeExA)(DWORD, LPCSTR, LPDWORD);
typedef DWORD (WINAPI *pfn_GetFileVersionInfoSizeExW)(DWORD, LPCWSTR, LPDWORD);
typedef DWORD (WINAPI *pfn_GetFileVersionInfoSizeW)(LPCWSTR, LPDWORD);
typedef BOOL  (WINAPI *pfn_GetFileVersionInfoW)(LPCWSTR, DWORD, DWORD, LPVOID);
typedef DWORD (WINAPI *pfn_VerFindFileA)(DWORD, LPSTR, LPSTR, LPSTR, LPSTR, PUINT, LPSTR, PUINT);
typedef DWORD (WINAPI *pfn_VerFindFileW)(DWORD, LPWSTR, LPWSTR, LPWSTR, LPWSTR, PUINT, LPWSTR, PUINT);
typedef DWORD (WINAPI *pfn_VerInstallFileA)(DWORD, LPSTR, LPSTR, LPSTR, LPSTR, LPSTR, LPSTR, PUINT);
typedef DWORD (WINAPI *pfn_VerInstallFileW)(DWORD, LPWSTR, LPWSTR, LPWSTR, LPWSTR, LPWSTR, LPWSTR, PUINT);
typedef DWORD (WINAPI *pfn_VerLanguageNameA)(DWORD, LPSTR, DWORD);
typedef DWORD (WINAPI *pfn_VerLanguageNameW)(DWORD, LPWSTR, DWORD);
typedef BOOL  (WINAPI *pfn_VerQueryValueA)(LPCVOID, LPCSTR, LPVOID*, PUINT);
typedef BOOL  (WINAPI *pfn_VerQueryValueW)(LPCVOID, LPCWSTR, LPVOID*, PUINT);

static HMODULE s_real_dll = NULL;
static pfn_GetFileVersionInfoA     s_GetFileVersionInfoA     = NULL;
static pfn_GetFileVersionInfoByHandle s_GetFileVersionInfoByHandle = NULL;
static pfn_GetFileVersionInfoExA   s_GetFileVersionInfoExA   = NULL;
static pfn_GetFileVersionInfoExW   s_GetFileVersionInfoExW   = NULL;
static pfn_GetFileVersionInfoSizeA s_GetFileVersionInfoSizeA = NULL;
static pfn_GetFileVersionInfoSizeExA s_GetFileVersionInfoSizeExA = NULL;
static pfn_GetFileVersionInfoSizeExW s_GetFileVersionInfoSizeExW = NULL;
static pfn_GetFileVersionInfoSizeW s_GetFileVersionInfoSizeW = NULL;
static pfn_GetFileVersionInfoW     s_GetFileVersionInfoW     = NULL;
static pfn_VerFindFileA            s_VerFindFileA            = NULL;
static pfn_VerFindFileW            s_VerFindFileW            = NULL;
static pfn_VerInstallFileA         s_VerInstallFileA         = NULL;
static pfn_VerInstallFileW         s_VerInstallFileW         = NULL;
static pfn_VerLanguageNameA        s_VerLanguageNameA        = NULL;
static pfn_VerLanguageNameW        s_VerLanguageNameW        = NULL;
static pfn_VerQueryValueA          s_VerQueryValueA          = NULL;
static pfn_VerQueryValueW          s_VerQueryValueW          = NULL;

static INIT_ONCE s_init_once = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK do_init(PINIT_ONCE InitOnce, PVOID Parameter, PVOID *Context) {
    (void)InitOnce; (void)Parameter; (void)Context;

    char sys_dir[MAX_PATH];
    char dll_path[MAX_PATH];

    if (!GetSystemDirectoryA(sys_dir, MAX_PATH)) {
        LOG_ERROR("GetSystemDirectoryA failed: %lu", GetLastError());
        return FALSE;
    }
    _snprintf_s(dll_path, MAX_PATH, _TRUNCATE, "%s\\version.dll", sys_dir);

    s_real_dll = LoadLibraryA(dll_path);
    if (!s_real_dll) {
        LOG_ERROR("Failed to load real version.dll from %s: %lu", dll_path, GetLastError());
        return FALSE;
    }

    s_GetFileVersionInfoA       = (pfn_GetFileVersionInfoA)      GetProcAddress(s_real_dll, "GetFileVersionInfoA");
    s_GetFileVersionInfoByHandle= (pfn_GetFileVersionInfoByHandle)GetProcAddress(s_real_dll, "GetFileVersionInfoByHandle");
    s_GetFileVersionInfoExA     = (pfn_GetFileVersionInfoExA)    GetProcAddress(s_real_dll, "GetFileVersionInfoExA");
    s_GetFileVersionInfoExW     = (pfn_GetFileVersionInfoExW)    GetProcAddress(s_real_dll, "GetFileVersionInfoExW");
    s_GetFileVersionInfoSizeA   = (pfn_GetFileVersionInfoSizeA)  GetProcAddress(s_real_dll, "GetFileVersionInfoSizeA");
    s_GetFileVersionInfoSizeExA = (pfn_GetFileVersionInfoSizeExA)GetProcAddress(s_real_dll, "GetFileVersionInfoSizeExA");
    s_GetFileVersionInfoSizeExW = (pfn_GetFileVersionInfoSizeExW)GetProcAddress(s_real_dll, "GetFileVersionInfoSizeExW");
    s_GetFileVersionInfoSizeW   = (pfn_GetFileVersionInfoSizeW)  GetProcAddress(s_real_dll, "GetFileVersionInfoSizeW");
    s_GetFileVersionInfoW       = (pfn_GetFileVersionInfoW)      GetProcAddress(s_real_dll, "GetFileVersionInfoW");
    s_VerFindFileA              = (pfn_VerFindFileA)             GetProcAddress(s_real_dll, "VerFindFileA");
    s_VerFindFileW              = (pfn_VerFindFileW)             GetProcAddress(s_real_dll, "VerFindFileW");
    s_VerInstallFileA           = (pfn_VerInstallFileA)          GetProcAddress(s_real_dll, "VerInstallFileA");
    s_VerInstallFileW           = (pfn_VerInstallFileW)          GetProcAddress(s_real_dll, "VerInstallFileW");
    s_VerLanguageNameA          = (pfn_VerLanguageNameA)         GetProcAddress(s_real_dll, "VerLanguageNameA");
    s_VerLanguageNameW          = (pfn_VerLanguageNameW)         GetProcAddress(s_real_dll, "VerLanguageNameW");
    s_VerQueryValueA            = (pfn_VerQueryValueA)           GetProcAddress(s_real_dll, "VerQueryValueA");
    s_VerQueryValueW            = (pfn_VerQueryValueW)           GetProcAddress(s_real_dll, "VerQueryValueW");

    LOG_INFO("Loaded real version.dll from %s", dll_path);
    return TRUE;
}

void proxy_init(void) {
    InitOnceExecuteOnce(&s_init_once, do_init, NULL, NULL);
}

BOOL WINAPI GetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    proxy_init();
    if (!s_GetFileVersionInfoA) return FALSE;
    return s_GetFileVersionInfoA(lptstrFilename, dwHandle, dwLen, lpData);
}

BOOL WINAPI GetFileVersionInfoByHandle(DWORD dwFlags, HANDLE hFile, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    proxy_init();
    if (!s_GetFileVersionInfoByHandle) return FALSE;
    return s_GetFileVersionInfoByHandle(dwFlags, hFile, dwHandle, dwLen, lpData);
}

BOOL WINAPI GetFileVersionInfoExA(DWORD dwFlags, LPCSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    proxy_init();
    if (!s_GetFileVersionInfoExA) return FALSE;
    return s_GetFileVersionInfoExA(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData);
}

BOOL WINAPI GetFileVersionInfoExW(DWORD dwFlags, LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    proxy_init();
    if (!s_GetFileVersionInfoExW) return FALSE;
    return s_GetFileVersionInfoExW(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData);
}

DWORD WINAPI GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle) {
    proxy_init();
    if (!s_GetFileVersionInfoSizeA) return 0;
    return s_GetFileVersionInfoSizeA(lptstrFilename, lpdwHandle);
}

DWORD WINAPI GetFileVersionInfoSizeExA(DWORD dwFlags, LPCSTR lpwstrFilename, LPDWORD lpdwHandle) {
    proxy_init();
    if (!s_GetFileVersionInfoSizeExA) return 0;
    return s_GetFileVersionInfoSizeExA(dwFlags, lpwstrFilename, lpdwHandle);
}

DWORD WINAPI GetFileVersionInfoSizeExW(DWORD dwFlags, LPCWSTR lpwstrFilename, LPDWORD lpdwHandle) {
    proxy_init();
    if (!s_GetFileVersionInfoSizeExW) return 0;
    return s_GetFileVersionInfoSizeExW(dwFlags, lpwstrFilename, lpdwHandle);
}

DWORD WINAPI GetFileVersionInfoSizeW(LPCWSTR lptstrFilename, LPDWORD lpdwHandle) {
    proxy_init();
    if (!s_GetFileVersionInfoSizeW) return 0;
    return s_GetFileVersionInfoSizeW(lptstrFilename, lpdwHandle);
}

BOOL WINAPI GetFileVersionInfoW(LPCWSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    proxy_init();
    if (!s_GetFileVersionInfoW) return FALSE;
    return s_GetFileVersionInfoW(lptstrFilename, dwHandle, dwLen, lpData);
}

DWORD WINAPI VerFindFileA(DWORD uFlags, LPSTR szFileName, LPSTR szWinDir, LPSTR szAppDir, LPSTR szCurDir, PUINT lpuCurDirLen, LPSTR szDestDir, PUINT lpuDestDirLen) {
    proxy_init();
    if (!s_VerFindFileA) return 0;
    return s_VerFindFileA(uFlags, szFileName, szWinDir, szAppDir, szCurDir, lpuCurDirLen, szDestDir, lpuDestDirLen);
}

DWORD WINAPI VerFindFileW(DWORD uFlags, LPWSTR szFileName, LPWSTR szWinDir, LPWSTR szAppDir, LPWSTR szCurDir, PUINT lpuCurDirLen, LPWSTR szDestDir, PUINT lpuDestDirLen) {
    proxy_init();
    if (!s_VerFindFileW) return 0;
    return s_VerFindFileW(uFlags, szFileName, szWinDir, szAppDir, szCurDir, lpuCurDirLen, szDestDir, lpuDestDirLen);
}

DWORD WINAPI VerInstallFileA(DWORD uFlags, LPSTR szSrcFileName, LPSTR szDestFileName, LPSTR szSrcDir, LPSTR szDestDir, LPSTR szCurDir, LPSTR szTmpFile, PUINT lpuTmpFileLen) {
    proxy_init();
    if (!s_VerInstallFileA) return 0;
    return s_VerInstallFileA(uFlags, szSrcFileName, szDestFileName, szSrcDir, szDestDir, szCurDir, szTmpFile, lpuTmpFileLen);
}

DWORD WINAPI VerInstallFileW(DWORD uFlags, LPWSTR szSrcFileName, LPWSTR szDestFileName, LPWSTR szSrcDir, LPWSTR szDestDir, LPWSTR szCurDir, LPWSTR szTmpFile, PUINT lpuTmpFileLen) {
    proxy_init();
    if (!s_VerInstallFileW) return 0;
    return s_VerInstallFileW(uFlags, szSrcFileName, szDestFileName, szSrcDir, szDestDir, szCurDir, szTmpFile, lpuTmpFileLen);
}

DWORD WINAPI VerLanguageNameA(DWORD wLang, LPSTR szLang, DWORD nSize) {
    proxy_init();
    if (!s_VerLanguageNameA) return 0;
    return s_VerLanguageNameA(wLang, szLang, nSize);
}

DWORD WINAPI VerLanguageNameW(DWORD wLang, LPWSTR szLang, DWORD nSize) {
    proxy_init();
    if (!s_VerLanguageNameW) return 0;
    return s_VerLanguageNameW(wLang, szLang, nSize);
}

BOOL WINAPI VerQueryValueA(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen) {
    proxy_init();
    if (!s_VerQueryValueA) return FALSE;
    return s_VerQueryValueA(pBlock, lpSubBlock, lplpBuffer, puLen);
}

BOOL WINAPI VerQueryValueW(LPCVOID pBlock, LPCWSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen) {
    proxy_init();
    if (!s_VerQueryValueW) return FALSE;
    return s_VerQueryValueW(pBlock, lpSubBlock, lplpBuffer, puLen);
}
