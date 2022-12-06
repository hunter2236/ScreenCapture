#include "stdafx.h"
#include "resource.h"
#include "IniFileHelper.h"
#include "ScreenCapture.h"

#define INIFILE     TEXT("ScreenCapture.ini")
#define SECTION     TEXT("Config")
#define KEY_CAPAREA TEXT("CaptureArea")
#define KEY_SAVEDIR TEXT("SaveDir")
#define KEY_SAVEFILENAME TEXT("SaveFileName")
#define KEY_SAVEENV TEXT("SaveEnv")
#define KEY_SAVEFILEIDX TEXT("SaveFileIndex")
#define KEY_COPYTOCLIPBOARD TEXT("CopyToClipboard")
#define KEY_CAPTUREMODE TEXT("CaptureMode")
#define KEY_OPENIMAGE TEXT("OpenImage")

#define VAL_ON  TEXT("1")
#define VAL_OFF TEXT("0")

#define DEFAULT_SAVEDIR TEXT("D:\\Temp\\")
#define DEFAULT_FILENAME TEXT("sample")
#define DEFAULT_ENVNAME TEXT("OSName")

TCHAR g_szIniFileFullPath[MAX_PATH + 1] = { 0 };

void SetCaptureArea(TCHAR* szCaptureArea);

void LoadIniFile(HINSTANCE hInstance)
{
	TCHAR szExeFullPath[MAX_PATH + 1];
	GetModuleFileName(hInstance, szExeFullPath, MAX_PATH);
	(_tcsrchr(szExeFullPath, _T('\\')))[1] = 0;
	_stprintf(g_szIniFileFullPath, TEXT("%s%s"), szExeFullPath, INIFILE);
}

void ReadConfig()
{
	TCHAR szCaptureArea[MAX_PATH + 1] = {0};
	memset(&g_capData, 0x00, sizeof(CaptureData));
	GetPrivateProfileString(SECTION, KEY_CAPAREA, TEXT("0,0,1280,950"), szCaptureArea, MAX_PATH + 1, g_szIniFileFullPath);
	SetCaptureArea(szCaptureArea);

	GetPrivateProfileString(SECTION, KEY_SAVEDIR, DEFAULT_SAVEDIR, g_capData.szSaveDir, MAX_PATH + 1, g_szIniFileFullPath);
	GetPrivateProfileString(SECTION, KEY_SAVEFILENAME, DEFAULT_FILENAME, g_capData.szSaveFileName, MAX_PATH + 1, g_szIniFileFullPath);
	GetPrivateProfileString(SECTION, KEY_SAVEENV, DEFAULT_ENVNAME, g_capData.szEnvName, MAX_PATH + 1, g_szIniFileFullPath);
	g_capData.idxFileName = GetPrivateProfileInt(SECTION, KEY_SAVEFILEIDX, 1, g_szIniFileFullPath);

	int iCopy2Clipboard = GetPrivateProfileInt(SECTION, KEY_COPYTOCLIPBOARD, 0, g_szIniFileFullPath);
	if (iCopy2Clipboard == 0) {
		g_capData.bCopy2Clipboard = FALSE;
	}
	else {
		g_capData.bCopy2Clipboard = TRUE;
	}

	int iOpenImage = GetPrivateProfileInt(SECTION, KEY_OPENIMAGE, 0, g_szIniFileFullPath);
	if (iOpenImage == 0) {
		g_capData.bOpenImage = FALSE;
	}
	else {
		g_capData.bOpenImage = TRUE;
	}
	g_capData.captureMode = (CaptureMode)GetPrivateProfileInt(SECTION, KEY_CAPTUREMODE, CAPMOD_AREA, g_szIniFileFullPath);
}

void SaveConfig(HWND hwnd)
{
	TCHAR szCaptureArea[MAX_PATH + 1] = { 0 };
	TCHAR szFileIndex[MAX_PATH + 1] = { 0 };
	TCHAR szValue[2] = { 0 };
	GetDlgItemText(hwnd, IDC_CAPTUREAREA, szCaptureArea, MAX_PATH + 1);
	SetCaptureArea(szCaptureArea);

	GetDlgItemText(hwnd, IDC_DIRECTORY, g_capData.szSaveDir, MAX_PATH + 1);
	GetDlgItemText(hwnd, IDC_SAVEFILENAME, g_capData.szSaveFileName, MAX_PATH + 1);
	GetDlgItemText(hwnd, IDC_ENV, g_capData.szEnvName, MAX_PATH + 1);
	GetDlgItemText(hwnd, IDC_FILEINDEX, szFileIndex, MAX_PATH + 1);

	g_capData.bOpenImage = IsDlgButtonChecked(hwnd, IDC_OPENIMAGE);
	g_capData.bCopy2Clipboard = IsDlgButtonChecked(hwnd, IDC_COPY2CLIPBOARD);

	WritePrivateProfileString(SECTION, KEY_CAPAREA, g_szCaptureArea, g_szIniFileFullPath);
	WritePrivateProfileString(SECTION, KEY_SAVEDIR, g_capData.szSaveDir, g_szIniFileFullPath);
	WritePrivateProfileString(SECTION, KEY_SAVEFILENAME, g_capData.szSaveFileName, g_szIniFileFullPath);
	WritePrivateProfileString(SECTION, KEY_SAVEENV, g_capData.szEnvName, g_szIniFileFullPath);

	int inputValue = _ttoi(szFileIndex);
	if (g_capData.idxFileName != inputValue) {
		g_capData.idxFileName = inputValue;
	}
	_itot(g_capData.idxFileName, szFileIndex, 10);
	WritePrivateProfileString(SECTION, KEY_SAVEFILEIDX, szFileIndex, g_szIniFileFullPath);

	if (g_capData.bOpenImage) {
		WritePrivateProfileString(SECTION, KEY_OPENIMAGE, VAL_ON, g_szIniFileFullPath);
	}
	else {
		WritePrivateProfileString(SECTION, KEY_OPENIMAGE, VAL_OFF, g_szIniFileFullPath);
	}

	if (g_capData.bCopy2Clipboard) {
		WritePrivateProfileString(SECTION, KEY_COPYTOCLIPBOARD, VAL_ON, g_szIniFileFullPath);
	}
	else {
		WritePrivateProfileString(SECTION, KEY_COPYTOCLIPBOARD, VAL_OFF, g_szIniFileFullPath);
	}

	_itow(g_capData.captureMode, szValue, 10);
	WritePrivateProfileString(SECTION, KEY_CAPTUREMODE, szValue, g_szIniFileFullPath);
}


void SetCaptureArea(TCHAR* szCaptureArea)
{
	lstrcpy(g_szCaptureArea, szCaptureArea);
	LPRECT lpArea = &(g_capData.CaptureArea);
	const TCHAR delims[] = TEXT(",");
	TCHAR* result = NULL;
	result = _tcstok(szCaptureArea, delims);
	lpArea->left = _ttoi(result);
	int idx = 1;
	while (result != NULL) {
		result = _tcstok(NULL, delims);
		switch (idx) {
		case 1:
			lpArea->top = _ttoi(result);
			break;
		case 2:
			lpArea->right = _ttoi(result);
			break;
		case 3:
			lpArea->bottom = _ttoi(result);
			break;
		}
		idx++;
	}
}
