#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void proxy_init(void);

/* Exported proxy functions — version.dll */
BOOL  WINAPI GetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
BOOL  WINAPI GetFileVersionInfoByHandle(DWORD dwFlags, HANDLE hFile, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
BOOL  WINAPI GetFileVersionInfoExA(DWORD dwFlags, LPCSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
BOOL  WINAPI GetFileVersionInfoExW(DWORD dwFlags, LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
DWORD WINAPI GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle);
DWORD WINAPI GetFileVersionInfoSizeExA(DWORD dwFlags, LPCSTR lpwstrFilename, LPDWORD lpdwHandle);
DWORD WINAPI GetFileVersionInfoSizeExW(DWORD dwFlags, LPCWSTR lpwstrFilename, LPDWORD lpdwHandle);
DWORD WINAPI GetFileVersionInfoSizeW(LPCWSTR lptstrFilename, LPDWORD lpdwHandle);
BOOL  WINAPI GetFileVersionInfoW(LPCWSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
DWORD WINAPI VerFindFileA(DWORD uFlags, LPSTR szFileName, LPSTR szWinDir, LPSTR szAppDir, LPSTR szCurDir, PUINT lpuCurDirLen, LPSTR szDestDir, PUINT lpuDestDirLen);
DWORD WINAPI VerFindFileW(DWORD uFlags, LPWSTR szFileName, LPWSTR szWinDir, LPWSTR szAppDir, LPWSTR szCurDir, PUINT lpuCurDirLen, LPWSTR szDestDir, PUINT lpuDestDirLen);
DWORD WINAPI VerInstallFileA(DWORD uFlags, LPSTR szSrcFileName, LPSTR szDestFileName, LPSTR szSrcDir, LPSTR szDestDir, LPSTR szCurDir, LPSTR szTmpFile, PUINT lpuTmpFileLen);
DWORD WINAPI VerInstallFileW(DWORD uFlags, LPWSTR szSrcFileName, LPWSTR szDestFileName, LPWSTR szSrcDir, LPWSTR szDestDir, LPWSTR szCurDir, LPWSTR szTmpFile, PUINT lpuTmpFileLen);
DWORD WINAPI VerLanguageNameA(DWORD wLang, LPSTR szLang, DWORD nSize);
DWORD WINAPI VerLanguageNameW(DWORD wLang, LPWSTR szLang, DWORD nSize);
BOOL  WINAPI VerQueryValueA(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen);
BOOL  WINAPI VerQueryValueW(LPCVOID pBlock, LPCWSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen);
