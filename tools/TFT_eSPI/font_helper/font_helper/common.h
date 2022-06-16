#pragma once
#include <windows.h>
#include <tchar.h>
#include <ShellAPI.h>
#include <Tlhelp32.h>
#include <Shlwapi.h>
#include <commctrl.h>
#include <string>
#include <map>
#include <vector>

typedef unsigned __int8 u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;
#ifdef _UNICODE
#define _tstring std::wstring
#else
#define _tstring std::string
#endif

std::wstring utf8_2_utf16(const std::string& str);
std::wstring gbk_2_utf16(const std::string& str);
std::string utf16_2_gbk(std::wstring& wstr);

BOOL IsTextUTF8(const char* str, ULONG length);
_tstring GetFolderPath(HWND hWnd, const LPTSTR szTitle);
_tstring GetFileVersion(const _tstring& szExePath);
BOOL IsRunAsAdmin();
BOOL RunAsAdmin();