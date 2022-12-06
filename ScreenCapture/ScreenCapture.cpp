#include "stdafx.h"
#include "CmnHdr.h"
#include "resource.h"

#include "../ScreenCaptureHook/ScreenCaptureHook.h"
#include "IniFileHelper.h"
#include "ImageHelper.h"
#include "MyMutex.h"
#include "ScreenCapture.h"

HINSTANCE hInst = NULL;
TCHAR szTitle[MAX_LOADSTRING] = {0};
TCHAR szWindowClass[MAX_LOADSTRING] = {0};

HWND g_hWin = NULL;
HWND g_hEdtArea = NULL;

CaptureData g_capData = {0};
TCHAR g_szCaptureArea[MAX_PATH + 1] = {0};

TCHAR g_szSaveFileFullName[MAX_PATH + 1] = { 0 };

LRESULT CALLBACK	WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK	About( HWND, UINT, WPARAM, LPARAM );
HBITMAP CopyActWndToBitmap();
HBITMAP CopyScreenToBitmap(LPRECT lpRect);
BOOL SaveBitmapToClipboard(HBITMAP bmSrc);

void MakeNextFileName();


BOOL isSingleRun();

INT_PTR WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow )
{

	if (isSingleRun() == FALSE){
		return 0;
	}

    LoadIniFile(hInstance);
    InitGdiPlus();

    DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, Dlg_Proc, _ttoi(lpCmdLine));

    ReleaseGdiPlus();
    ReleaseMutex();

    return 0;
}

void UpdateFileIndexText(HWND hwnd){
	TCHAR g_szFileIndex[MAX_PATH + 1] = {0};
	_stprintf(g_szFileIndex, TEXT("%03d"), g_capData.idxFileName);
	SetDlgItemText(hwnd, IDC_FILEINDEX, g_szFileIndex);

}

BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {

    chSETDLGICONS(hwnd, IDI_SMALL);

    // Don't accept error codes more than 5 digits long
    //Edit_LimitText(GetDlgItem(hwnd, IDC_ERRORCODE), 5);

    // Look up the command-line passed error number
    //SendMessage(hwnd, ESM_POKECODEANDLOOKUP, lParam, 0);
    ReadConfig();
    g_hWin = hwnd;
    //alt + x
    SetHook(g_hWin, 88);

    SetDlgItemText(hwnd, IDC_CAPTUREAREA, g_szCaptureArea);
    SetDlgItemText(hwnd, IDC_DIRECTORY, g_capData.szSaveDir);
    SetDlgItemText(hwnd, IDC_SAVEFILENAME, g_capData.szSaveFileName);
    SetDlgItemText(hwnd, IDC_ENV, g_capData.szEnvName);
    g_hEdtArea = GetDlgItem(hwnd, IDC_CAPTUREAREA);
	UpdateFileIndexText(hwnd);

	CheckDlgButton(hwnd, IDC_COPY2CLIPBOARD, g_capData.bCopy2Clipboard);
	CheckDlgButton(hwnd, IDC_OPENIMAGE, g_capData.bOpenImage);

    if (g_capData.captureMode == CAPMOD_AREA) {
        CheckRadioButton(hwnd, IDC_RAD_RANGE, IDC_RAD_WIN, IDC_RAD_RANGE);
    }else {
        CheckRadioButton(hwnd, IDC_RAD_RANGE, IDC_RAD_WIN, IDC_RAD_WIN);
        Edit_Enable(g_hEdtArea, FALSE);
    }
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
    switch (id) {
    case IDCANCEL:
        UnHook();
        EndDialog(hwnd, id);
        break;

        // 	case IDC_ALWAYSONTOP:
        // 		SetWindowPos(hwnd, IsDlgButtonChecked(hwnd, IDC_ALWAYSONTOP) 
        // 			? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        // 		break;
        // 		
        // 	case IDC_ERRORCODE: 
        // 		EnableWindow(GetDlgItem(hwnd, IDOK), Edit_GetTextLength(hwndCtl) > 0);
        // 		break;

    case IDOK:
        // Get the error code
		SaveConfig(hwnd);
        break;
	case IDRESET:
        g_capData.idxFileName = 1;
		UpdateFileIndexText(hwnd);
		break;
    case IDC_RAD_AREA:
        g_capData.captureMode = CAPMOD_AREA;
        Edit_Enable(g_hEdtArea, TRUE);
        break;
    case IDC_RAD_WIN:
        g_capData.captureMode = CAPMOD_WIN;
        Edit_Enable(g_hEdtArea, FALSE);
        break;
    default:
        break;
    }
}

INT_PTR WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HBITMAP bmCopy = NULL;
    switch (uMsg) {
        chHANDLE_DLGMSG(hwnd, WM_INITDIALOG, Dlg_OnInitDialog);
        chHANDLE_DLGMSG(hwnd, WM_COMMAND,    Dlg_OnCommand);

    case WM_SCREENCAPTURE:
        {
            if (g_capData.captureMode == CAPMOD_AREA) {
                bmCopy = CopyScreenToBitmap(&g_capData.CaptureArea);
            }
            else {
                bmCopy = CopyActWndToBitmap();
            }
            if (bmCopy != NULL){
				if (g_capData.bCopy2Clipboard) {
					SaveBitmapToClipboard(bmCopy);
				}
				MakeNextFileName();
				UpdateFileIndexText(hwnd);

				SavePngToFile(bmCopy, g_szSaveFileFullName);

				::DeleteObject(bmCopy);
				bmCopy = NULL;
				//if(g_bOpenImage){
					//STARTUPINFO si;
					//PROCESS_INFORMATION pi;
					//TCHAR szCmdPath[MAX_PATH + 1];
					//memset(szCmdPath, 0x00, sizeof(szCmdPath));
					//_stprintf(szCmdPath, "mspaint '%s'", g_szSaveFileFullName);
					//CreateProcess("mspaint", g_szSaveFileFullName, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
				//}
            }
        }
        break;
	case WM_CLOSE:{
			UnHook();
		}
		break;
    }

    return(FALSE);
}

HBITMAP CopyActWndToBitmap() {
    HBITMAP hBitmap = NULL, hOldBitmap = NULL;
    HDC dcActive = NULL;
    RECT rcRect = { 0 };
    POINT ptClientStart = { 0 };
    HWND hActive = GetForegroundWindow();
    GetClientRect(hActive, &rcRect);
    ClientToScreen(hActive, &ptClientStart);
    OffsetRect(&rcRect, ptClientStart.x, ptClientStart.y);

    //HRESULT hr = DwmGetWindowAttribute(hActive, DWMWA_EXTENDED_FRAME_BOUNDS, &rcWin, sizeof(rcWin));
    //if (hr == S_OK) {
    //    return CopyScreenToBitmap(&rcWin);
    //} else {
    //    return NULL;
    //}

    return CopyScreenToBitmap(&rcRect);
}

HBITMAP CopyScreenToBitmap(LPRECT lpRect)
{
    HDC hScrDC = NULL, hMemDC = NULL;
    HBITMAP hBitmap,hOldBitmap;
    int       nX, nY, nX2, nY2;   
    int       nWidth, nHeight;
    int       xScrn, yScrn;      
    if (IsRectEmpty(lpRect)){
        return NULL;
    }

    hScrDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);

    hMemDC = CreateCompatibleDC(hScrDC);

    nX = lpRect->left;
    nY = lpRect->top;
    nX2 = lpRect->right;
    nY2 = lpRect->bottom;

    xScrn = GetDeviceCaps(hScrDC, HORZRES);
    yScrn = GetDeviceCaps(hScrDC, VERTRES);

    if (nX < 0)
        nX = 0;
    if (nY < 0)
        nY = 0;
    if (nX2 > xScrn)
        nX2 = xScrn;
    if (nY2 > yScrn)
        nY2 = yScrn;
    nWidth = nX2 - nX;
    nHeight = nY2 - nY;

    hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);

    hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);

    hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);

    DeleteDC(hScrDC);
    DeleteDC(hMemDC);

    return hBitmap;
}

BOOL SaveBitmapToClipboard(HBITMAP bmSrc)
{
    if (bmSrc == NULL)
    {
        return FALSE;
    }
    if(OpenClipboard(NULL) == FALSE)
    {
        return FALSE;
    }

    EmptyClipboard();
    SetClipboardData(CF_BITMAP, bmSrc);
    CloseClipboard();

    return TRUE;
}

void MakeNextFileName()
{
	int iSaveDirCount = lstrlen(g_capData.szSaveDir);
	if (iSaveDirCount > 0){
		if(g_capData.szSaveDir[iSaveDirCount - 1] == '\\'){
			_stprintf(g_szSaveFileFullName, TEXT("%s%s%03d_%s.png"), g_capData.szSaveDir, g_capData.szSaveFileName, g_capData.idxFileName, g_capData.szEnvName);
		}else{
			_stprintf(g_szSaveFileFullName, TEXT("%s\\%s%03d_%s.png"), g_capData.szSaveDir, g_capData.szSaveFileName, g_capData.idxFileName, g_capData.szEnvName);
		}
	}else{
		_stprintf(g_szSaveFileFullName, TEXT("%s%03d.png"), g_capData.szSaveFileName, g_capData.idxFileName);
	}

    g_capData.idxFileName++;
}
