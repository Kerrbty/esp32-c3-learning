#include <Windows.h>
#include <tchar.h>
#include <set>
#include "common.h"
#include "res/resource.h"
#pragma comment(lib, "shlwapi.lib")

HWND gWnd = NULL;

void move_Middle(HWND hWnd)
{
    RECT rect;
    int x,y;
    GetWindowRect(hWnd, &rect);//得到当前窗口大小信息
    x = rect.right - rect.left;
    y = rect.bottom - rect.top;
    MoveWindow(hWnd, (GetSystemMetrics(SM_CXSCREEN)- x)>>1, (GetSystemMetrics(SM_CYSCREEN)-y)>>1, x, y , TRUE);
}

DWORD WINAPI SaveUnicodeCodeThread(LPVOID lparam)
{
    LPTSTR lpFileName = (LPTSTR)lparam;
    if (lpFileName == NULL)
    {
        return 0;
    }

    HANDLE hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        SetDlgItemText(gWnd, IDC_STATUS, TEXT("打开字符文件失败~"));
        return 0;
    }

    DWORD dwBytes = 0;
    DWORD dwFileSize = GetFileSize(hFile, NULL);
    LPBYTE lpBuffer = new BYTE[dwFileSize+4];
    if (lpBuffer)
    {
        LPWSTR lpTxt = NULL;
        DWORD dwCount = 0;

        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        ReadFile(hFile, lpBuffer, dwFileSize, &dwBytes, NULL);
        // 先判断文本是不是UNICODE的 
        INT pResult = IS_TEXT_UNICODE_UNICODE_MASK;
        if (IsTextUnicode(lpBuffer, dwFileSize, &pResult))
        {
            lpTxt = (LPWSTR)lpBuffer;
            dwCount = dwFileSize/2;
        }
        else if ( IsTextUTF8((const char*)lpBuffer, dwFileSize) )
        {
            int    iTextLen;
            iTextLen = MultiByteToWideChar(CP_UTF8,
                0,
                (char*)lpBuffer,
                -1,
                NULL,
                0);

            lpTxt =
                (PWSTR)new WCHAR[iTextLen + 1];
            if (lpTxt)
            {
                MultiByteToWideChar(CP_UTF8,
                    0,
                    (char*)lpBuffer,
                    -1,
                    lpTxt,
                    iTextLen);
                dwCount = iTextLen/2;
            }
        }
        else 
        {
            int    iTextLen;
            iTextLen = MultiByteToWideChar(CP_ACP,
                0,
                (char*)lpBuffer,
                -1,
                NULL,
                0);

            lpTxt =
                (PWSTR)new WCHAR[iTextLen + 1];
            if (lpTxt)
            {
                MultiByteToWideChar(CP_ACP,
                    0,
                    (char*)lpBuffer,
                    -1,
                    lpTxt,
                    iTextLen);
                dwCount = iTextLen/2;
            }
        }

        LPTSTR lpNewFile = new TCHAR[MAX_PATH*2];
        if (lpNewFile)
        {
            _tcscpy(lpNewFile, TEXT("/select,"));
            _tcscpy(lpNewFile+8, lpFileName);
            LPTSTR lpExt = PathFindExtension(lpNewFile+8);
            _tcscpy(lpExt, TEXT(".java"));
            FILE* fp = _tfopen(lpNewFile+8, TEXT("w+"));
            if (fp)
            {
                DWORD i = 0;
                std::set<WCHAR> repeatFiltr; // 重复过滤器 
                for (i=0; i<dwCount; i++)
                {
                    // 过滤ASCII表数据 
                    if ( lpTxt[i]>0x7F )
                    {
                        // 过滤重复数据 
                        if ( repeatFiltr.find(lpTxt[i]) == repeatFiltr.end() )
                        {
                            repeatFiltr.insert(lpTxt[i]);
                        }
                    }
                }

                i = 0;
                for (std::set<WCHAR>::iterator it = repeatFiltr.begin();
                    it != repeatFiltr.end();
                    it++)
                {
                    if (i%8 == 0)
                    {
                        _ftprintf(fp, TEXT("\n    "));
                    }
                    _ftprintf(fp, TEXT("0x%04x, "), *it);
                    i++;
                }
                _ftprintf(fp, TEXT("\n"));
                fclose(fp);
                ShellExecute(GetDesktopWindow(), TEXT("open"), TEXT("explorer"), lpNewFile, NULL, SW_SHOW);
            }
            delete []lpNewFile;
        }

        if (lpTxt != (LPWSTR)lpBuffer)
        {
            delete []lpTxt;
        }
        delete []lpBuffer;
    }

    CloseHandle(hFile);
    delete []lpFileName;
    SetDlgItemText(gWnd, IDC_STATUS, TEXT("转换Unicode字体完成"));
    EnableWindow(GetDlgItem(gWnd, IDC_CreateUnicodesData), TRUE);
    return 0;
}

DWORD WINAPI SaveFontHeaderThread(LPVOID lparam)
{
    LPTSTR lpFileName = (LPTSTR)lparam;
    if (lpFileName == NULL)
    {
        return 0;
    }

    HANDLE hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        SetDlgItemText(gWnd, IDC_STATUS, TEXT("打开VLW文件失败~"));
        return 0;
    }

    DWORD dwBytes = 0;
    DWORD dwFileSize = GetFileSize(hFile, NULL);
    LPBYTE lpBuffer = new BYTE[dwFileSize+4];
    if (lpBuffer)
    {
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        ReadFile(hFile, lpBuffer, dwFileSize, &dwBytes, NULL);

        // 头文件输出 
        LPTSTR lpNewFile = new TCHAR[MAX_PATH*2];
        LPTSTR lpFontName = new TCHAR[MAX_PATH];
        if (lpNewFile && lpFontName)
        {
            memset(lpFontName, 0, MAX_PATH*sizeof(TCHAR));

            _tcscpy(lpNewFile, TEXT("/select,"));
            _tcscpy(lpNewFile+8, lpFileName);
            LPTSTR lpVLWFileName = PathFindFileName(lpNewFile+8);
            LPTSTR lpExt = PathFindExtension(lpVLWFileName);
            memcpy(lpFontName, lpVLWFileName, (lpExt-lpVLWFileName)*sizeof(TCHAR));
            _tcscpy(lpExt, TEXT(".h"));
            FILE* fp = _tfopen(lpNewFile+8, TEXT("w+"));
            if (fp)
            {
                DWORD i=0;
                _ftprintf(fp, TEXT("#include <pgmspace.h>\n"));
                _ftprintf(fp, TEXT("const unsigned char %s[] PROGMEM = {"), lpFontName);
                for (i=0; i<dwFileSize; i++)
                {
                    if (i%16 == 0)
                    {
                        _ftprintf(fp, TEXT("\n    "));
                    }
                    _ftprintf(fp, TEXT("0x%02x, "), lpBuffer[i]);
                }
                if (i%16 != 0)
                {
                    _ftprintf(fp, TEXT("\n"));
                }
                _ftprintf(fp, TEXT("};"));
                fclose(fp);
                ShellExecute(GetDesktopWindow(), TEXT("open"), TEXT("explorer"), lpNewFile, NULL, SW_SHOW);
            }

            delete []lpFontName;
            delete []lpNewFile;
        }

        delete []lpBuffer;
    }

    CloseHandle(hFile);
    delete []lpFileName;
    SetDlgItemText(gWnd, IDC_STATUS, TEXT("转换 TFT_eSPI 字库头文件完成"));
    EnableWindow(GetDlgItem(gWnd, IDC_CreateFontHeader), TRUE);
    return 0;
}

INT_PTR CALLBACK ProcWinMain(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case WM_INITDIALOG:
        {
            gWnd = hWnd;
            move_Middle(hWnd);
        }
        break;

    case WM_SHOWWINDOW:
        move_Middle(hWnd);
        break;

    case WM_CLOSE:
        EndDialog(hWnd, 0);
        break;

    case WM_COMMAND:
        {
            WORD nCode = HIWORD(wParam);
            WORD nCtrlId = LOWORD(wParam);
            HWND hSubWnd = (HWND)lParam;
            switch(nCtrlId)
            {
            case IDC_CreateUnicodesData: // 点击生成Unicode数组 
                if (nCode == BN_CLICKED)
                {
                    LPTSTR lpFileName = new TCHAR[MAX_PATH];
                    if (lpFileName)
                    {
                        memset(lpFileName, 0, MAX_PATH*sizeof(TCHAR));
                        GetDlgItemText(hWnd, IDC_TXTFile, lpFileName, MAX_PATH);
                        if (PathFileExists(lpFileName))
                        {
                            CloseHandle(CreateThread(NULL, 0, SaveUnicodeCodeThread, lpFileName, 0, NULL));
                            SetDlgItemText(hWnd, IDC_STATUS, TEXT("正在转换Unicode字体.."));
                            EnableWindow(GetDlgItem(hWnd, IDC_CreateUnicodesData), FALSE);
                        }
                        else
                        {
                            SetDlgItemText(hWnd, IDC_STATUS, TEXT("似乎字体文件设置不正确哦."));
                            delete []lpFileName;
                        }
                    }
                }
                break;
            case IDC_CreateFontHeader: // 点击生成字库头文件 
                if (nCode == BN_CLICKED)
                {
                    LPTSTR lpFileName = new TCHAR[MAX_PATH];
                    if (lpFileName)
                    {
                        memset(lpFileName, 0, MAX_PATH*sizeof(TCHAR));
                        GetDlgItemText(hWnd, IDC_VLWFile, lpFileName, MAX_PATH);
                        if (PathFileExists(lpFileName))
                        {
                            CloseHandle(CreateThread(NULL, 0, SaveFontHeaderThread, lpFileName, 0, NULL));
                            SetDlgItemText(hWnd, IDC_STATUS, TEXT("正在转换 TFT_eSPI 字库头文件.."));
                            EnableWindow(GetDlgItem(hWnd, IDC_CreateFontHeader), FALSE);
                        }
                        else
                        {
                            SetDlgItemText(hWnd, IDC_STATUS, TEXT("似乎VLW文件设置不正确哦."));
                            delete []lpFileName;
                        }
                    }
                }
                break;
            }
        }
        break;

    case WM_DROPFILES:
        {
            // 拖拽接受文件 
            LPTSTR lpFileName = new TCHAR[MAX_PATH];
            if (lpFileName)
            {
                HDROP hDrop = (HDROP) wParam; 
                UINT nCount = DragQueryFile(hDrop, -1, NULL, 0);
                for (UINT i=0; i<nCount; i++)
                {
                    memset(lpFileName, 0, MAX_PATH*sizeof(TCHAR));
                    DragQueryFile(hDrop, i, lpFileName, MAX_PATH);
                    LPTSTR lpExt = PathFindExtension(lpFileName);
                    if (lpExt)
                    {
                        if (_tcsicmp(lpExt, TEXT(".txt")) == 0)
                        {
                            SetDlgItemText(hWnd, IDC_TXTFile, lpFileName);
                        }
                        else if (_tcsicmp(lpExt, TEXT(".vlw")) == 0)
                        {
                            SetDlgItemText(hWnd, IDC_VLWFile, lpFileName);
                        }
                        else
                        {
                            SetDlgItemText(hWnd, IDC_STATUS, TEXT("未能识别的文件后缀"));
                        }
                    }
                    else
                    {
                        SetDlgItemText(hWnd, IDC_STATUS, TEXT("文件没有后缀"));
                    }
                }
                delete []lpFileName;
            }
        }
        break;

    default:
        return FALSE;
        break;
    }
    return 0;
} 

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    // 尝试管理员方式启动 
    if (!IsRunAsAdmin())
    {
        RunAsAdmin();
    }
    
    // 取消管理员拖拽隔离 
    ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
    ChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
    ChangeWindowMessageFilter(SPI_SETANIMATION, MSGFLT_ADD);

    //显示窗口
    DialogBoxParam(hInstance, MAKEINTRESOURCE(FONT_HELP_DIALOG), NULL, (DLGPROC)ProcWinMain, NULL);
    return TRUE;
}