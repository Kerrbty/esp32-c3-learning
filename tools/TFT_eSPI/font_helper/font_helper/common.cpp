#include "common.h"
#include <Shlobj.h>
#include <time.h>
#pragma comment(lib, "Version.lib")

static PWSTR WINAPI MultiToWide(LPCSTR str, UINT CodePage = CP_ACP)
{
    PWSTR  pElementText;
    int    iTextLen;

    iTextLen = MultiByteToWideChar(CodePage,
        0,
        (PCHAR)str,
        -1,
        NULL,
        0);

    pElementText =
        (PWSTR)new WCHAR[iTextLen + 1];
    if (pElementText)
    {
        MultiByteToWideChar(CodePage,
            0,
            (PCHAR)str,
            -1,
            pElementText,
            iTextLen);
        return pElementText;
    }
    return NULL;
}

static PSTR WINAPI WideToMulti(LPCWSTR str, UINT CodePage = CP_ACP)
{
    PSTR  pElementText;
    int    iTextLen;

    iTextLen = WideCharToMultiByte(CodePage,
        0,
        str,
        -1,
        NULL,
        0,
        NULL,
        NULL);

    pElementText =
        (PSTR)new char[iTextLen + 1];
    if (pElementText)
    {
        WideCharToMultiByte(CodePage,
            0,
            str,
            -1,
            pElementText,
            iTextLen,
            NULL,
            NULL);
        return pElementText;
    }
    return NULL;
}

std::wstring utf8_2_utf16(const std::string& str)
{
    std::wstring utf16str;
    PWSTR new_utf16 = MultiToWide(str.c_str(), CP_UTF8);
    if (new_utf16)
    {
        utf16str = new_utf16;
        delete[]new_utf16;
    }
    return utf16str;
}

std::wstring gbk_2_utf16(const std::string& str)
{
    std::wstring utf16str;
    PWSTR new_utf16 = MultiToWide(str.c_str(), CP_ACP);
    if (new_utf16)
    {
        utf16str = new_utf16;
        delete[]new_utf16;
    }
    return utf16str;
}

std::string utf16_2_gbk(std::wstring& wstr)
{
    std::string gbkstr;
    PSTR new_gbk = WideToMulti(wstr.c_str(), CP_ACP);
    if (new_gbk)
    {
        gbkstr = new_gbk;
        delete[]new_gbk;
    }
    return gbkstr;
}

BOOL IsTextUTF8(const char* str, ULONG length)
{
    DWORD nBytes=0;//UFT8����1-6���ֽڱ���,ASCII��һ���ֽ�
    UCHAR chr;
    BOOL bAllAscii=TRUE; //���ȫ������ASCII, ˵������UTF-8
    for(int i=0; i<length; ++i)
    {
        chr= *(str+i);
        if( (chr&0x80) != 0 ) // �ж��Ƿ�ASCII����,�������,˵���п�����UTF-8,ASCII��7λ����,����һ���ֽڴ�,���λ���Ϊ0,o0xxxxxxx
            bAllAscii= FALSE;
        if(nBytes==0) //�������ASCII��,Ӧ���Ƕ��ֽڷ�,�����ֽ���
        {
            if(chr>=0x80)
            {
                if(chr>=0xFC&&chr<=0xFD)
                    nBytes=6;
                else if(chr>=0xF8)
                    nBytes=5;
                else if(chr>=0xF0)
                    nBytes=4;
                else if(chr>=0xE0)
                    nBytes=3;
                else if(chr>=0xC0)
                    nBytes=2;
                else
                    return FALSE;

                nBytes--;
            }
        }
        else //���ֽڷ��ķ����ֽ�,ӦΪ 10xxxxxx
        {
            if( (chr&0xC0) != 0x80 )
                return FALSE;

            nBytes--;
        }
    }
    if( nBytes > 0 ) //Υ������
        return FALSE;
    if( bAllAscii ) //���ȫ������ASCII, ˵������UTF-8
        return FALSE;
    return TRUE;
}

static bool GetFileVersion(const _tstring& szExePath, DWORD* dwVerMS, DWORD* dwVerLS)
{
    bool bsuccess = false;
    DWORD dwHandle = 0;
    DWORD dwLen = GetFileVersionInfoSize(szExePath.c_str(), &dwHandle);
    if (dwLen <= 0)
    {
        return bsuccess;
    }

    LPTSTR verData = new(std::nothrow) TCHAR[dwLen];
    if (GetFileVersionInfo(szExePath.c_str(), dwHandle, dwLen, verData))
    {
        VS_FIXEDFILEINFO* pTransTable = NULL;
        UINT   dwQuerySize = 0;
        if (VerQueryValue(verData, L"\\", (LPVOID*)&pTransTable, &dwQuerySize))
        {
            if (dwQuerySize > 0)
            {
                if (dwVerMS != NULL)
                {
                    *dwVerMS = pTransTable->dwProductVersionMS;
                }
                if (dwVerLS != NULL)
                {
                    *dwVerLS = pTransTable->dwProductVersionLS;
                }
                bsuccess = TRUE;
            }
        }
    }
    delete[] verData;
    return bsuccess;
}

_tstring GetFileVersion(const _tstring& szExePath)
{
    _tstring strVer;

    DWORD dwVerMS = 0;
    DWORD dwVerLS = 0;
    if ( GetFileVersion(szExePath, &dwVerMS, &dwVerLS) )
    {
        TCHAR szVersion[80] = { 0 };
        wsprintf(szVersion, _T("%d.%d.%d.%d"), 
            HIWORD(dwVerMS), LOWORD(dwVerMS),
            HIWORD(dwVerLS), LOWORD(dwVerLS)
            );
        strVer = szVersion;
    }
    return strVer;
}

// ��/����ϵͳ�ض���
typedef BOOLEAN (WINAPI* LPFN_Wow64EnableWow64FsRedirection)( BOOLEAN Wow64FsEnableRedirection );
typedef BOOLEAN (WINAPI* LPFN_Wow64DisableWow64FsRedirection)(PVOID *OldValue);
typedef BOOLEAN (WINAPI* LPFN_Wow64RevertWow64FsRedirection)(PVOID OldValue);
static BOOL WINAPI EnableWow64FsRedirection(BOOL enable)  // ���������ļ��ض���
{
    LPFN_Wow64EnableWow64FsRedirection Wow64EnableWow64FsRedirection = (LPFN_Wow64EnableWow64FsRedirection)
        GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "Wow64EnableWow64FsRedirection");
    if (Wow64EnableWow64FsRedirection != NULL)
    {
        if ( Wow64EnableWow64FsRedirection(enable) )
        {
            return TRUE;
        }
    }
    else
    {
        // ������
        static PVOID oldvalue = 0;
        LPFN_Wow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection = (LPFN_Wow64DisableWow64FsRedirection)
            GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "Wow64DisableWow64FsRedirection");
        LPFN_Wow64RevertWow64FsRedirection Wow64RevertWow64FsRedirection = (LPFN_Wow64RevertWow64FsRedirection)
            GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "Wow64RevertWow64FsRedirection");
        if (Wow64RevertWow64FsRedirection != NULL && Wow64DisableWow64FsRedirection != NULL)
        {
            if (enable)
            {
                return Wow64RevertWow64FsRedirection(oldvalue);
            }
            else
            {
                return Wow64DisableWow64FsRedirection(&oldvalue);
            }
        }
    }
    return FALSE;
}

// �õ�ϵͳλ�������õ�ϵͳ�汾��
static ULONG GetSystemBits(DWORD* dwWinMajor, DWORD* dwWinMinor)
{
    static ULONG retbit = -1;
    static DWORD dwMajor, dwMinor;

    // �Ѿ�����ˣ��Ǿ�ֱ�ӷ���֮ǰ��¼�� 
    if (retbit == -1)
    {
        // û�������32λ 64λ
        PTSTR szPath = (PTSTR)malloc(MAX_PATH*sizeof(TCHAR));
#ifdef _WIN64

        // 64 λ���������У�����64λϵͳ

        DWORD dwHandle;
        ExpandEnvironmentStrings(_T("%SystemRoot%\\system32\\ntdll.dll"), szPath, MAX_PATH);

        // ��ȡϵͳ�汾��Ϣ
        _tstring strPath = szPath;
        GetFileVersion(strPath, &dwMajor, &dwMinor);
        retbit = 64;

#else  /* _WIN64 */ 

        // ��ȡ%systemdir%/system32/ntdll.dll 
        DWORD dwBytes = 0;

        PBYTE pbuf = (PBYTE)malloc(0x1024);

        do 
        {
            // ����ϵͳ�ض�λ����ѯntdll.dll��λ��
            EnableWow64FsRedirection(FALSE);
            ExpandEnvironmentStrings(_T("%SystemRoot%\\system32\\ntdll.dll"), szPath, MAX_PATH);

            // ��ȡϵͳ�汾��Ϣ
            _tstring strPath = szPath;
            GetFileVersion(strPath, &dwMajor, &dwMinor);

            HANDLE hNtdll = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hNtdll == INVALID_HANDLE_VALUE)
            {
                break;
            }
            SetFilePointer(hNtdll, 0, NULL, FILE_BEGIN);
            ReadFile(hNtdll, pbuf, 0x1000, &dwBytes, NULL);
            CloseHandle(hNtdll);
            if (dwBytes == 0)
            {
                break;
            }
            // �����ǲ���64λ��
            PIMAGE_DOS_HEADER Header = (PIMAGE_DOS_HEADER)pbuf;
            if (Header->e_magic != IMAGE_DOS_SIGNATURE)
            {
                // ������Ч��PE�ļ�
                break;
            }
            PIMAGE_NT_HEADERS peheader = 
                (PIMAGE_NT_HEADERS)((LPBYTE)Header + Header->e_lfanew);
            if ((PBYTE)peheader < pbuf || (PBYTE)peheader > (pbuf+0x1000) )
            {
                break;
            }
            if (peheader->Signature != IMAGE_NT_SIGNATURE)
            {
                break;
            }

            if ( peheader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 /*i386*/ )
            {
                retbit = 32;
            }
            else if (
                peheader->FileHeader.Machine == IMAGE_FILE_MACHINE_IA64 /* IA64 */ ||
                peheader->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64 /* AMD64 */
                )
            {
                retbit = 64;
            }
        } while (false);

        EnableWow64FsRedirection(TRUE);
        free(pbuf);    
#endif
        free(szPath);
    }

    if (dwWinMajor != NULL)
    {
        *dwWinMajor = dwMajor;
    }
    if (dwWinMinor != NULL)
    {
        *dwWinMinor = dwMinor;
    }
    return retbit;
}

BOOL IsRunAsAdmin()
{
    DWORD dwWinMajor, dwWinMinor;
    DWORD dwSystemBits = GetSystemBits(&dwWinMajor, &dwWinMinor);
    if (dwWinMajor < 6)
    {
        // xp �Ļ�ֱ���ǹ���Ա����
        return TRUE;
    }

    // vista �Ժ�ϵͳ������ѯ 
    BOOL bElevated = FALSE;  
    HANDLE hToken = NULL;  

    // Get current process token
    if ( !OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken ) )
        return FALSE;

    TOKEN_ELEVATION tokenEle;
    DWORD dwRetLen = 0;  

    // Retrieve token elevation information
    if ( GetTokenInformation( hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen ) ) 
    {  
        if ( dwRetLen == sizeof(tokenEle) ) 
        {
            bElevated = tokenEle.TokenIsElevated;  
        }
    }  

    CloseHandle( hToken );  
    return bElevated;  
}

BOOL RunAsAdmin()
{
    BOOL retvalue = FALSE;
    SHELLEXECUTEINFO ExecInfo = {sizeof(SHELLEXECUTEINFO)};

    ExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ExecInfo.lpVerb = TEXT("runas");

    // Exe ģ�� 
    LPTSTR szExeName = (LPTSTR)malloc(MAX_PATH*sizeof(TCHAR));
    GetModuleFileName(GetModuleHandle(NULL), szExeName, MAX_PATH);
    ExecInfo.lpFile = szExeName; // exe 

    // ���� 
    LPTSTR lpCommandLine = GetCommandLine();
    if (*lpCommandLine == TEXT('"') ) // ��һ���ַ�������,·�������Ű�Χ
    {
        do 
        {
            lpCommandLine++;
        } while ( *lpCommandLine != TEXT('\0') && *lpCommandLine != TEXT('"'));

        if (*lpCommandLine != TEXT('\0'))
        {
            do 
            {
                lpCommandLine++;
            } while ( *lpCommandLine != TEXT(' ') ||  *lpCommandLine == TEXT('\t'));
        }
    }
    else
    {
        do 
        {
            lpCommandLine++;
        } while ( *lpCommandLine != TEXT('\0') && *lpCommandLine != TEXT(' ') && *lpCommandLine != TEXT('\t') );

        if (*lpCommandLine != TEXT('\0'))
        {
            do 
            {
                lpCommandLine++;
            } while ( *lpCommandLine != TEXT(' ') ||  *lpCommandLine == TEXT('\t'));
        }
    }
    ExecInfo.lpParameters = lpCommandLine; 

    retvalue = ShellExecuteEx(&ExecInfo);
    free(szExeName);
    return retvalue;
}