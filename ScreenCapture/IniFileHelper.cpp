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
#define KEY_SAVEFILEIDXL1 TEXT("SaveFileIndexL1")
#define KEY_SAVEFILEIDXL2 TEXT("SaveFileIndexL2")
#define KEY_SAVEFILEIDXL3 TEXT("SaveFileIndexL3")
#define KEY_CAPWINCLIENT TEXT("CaptureWindowClient")
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
void SplitString(TCHAR* szSource, const PTCHAR delims, TCHAR*** lpSplite, int* count);

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
	g_capData.idxLevel1 = GetPrivateProfileInt(SECTION, KEY_SAVEFILEIDXL1, 1, g_szIniFileFullPath);
	g_capData.idxLevel2 = GetPrivateProfileInt(SECTION, KEY_SAVEFILEIDXL2, 1, g_szIniFileFullPath);
	g_capData.idxLevel3 = GetPrivateProfileInt(SECTION, KEY_SAVEFILEIDXL3, 1, g_szIniFileFullPath);

	int iCapWinClient = GetPrivateProfileInt(SECTION, KEY_CAPWINCLIENT, 0, g_szIniFileFullPath);
	if (iCapWinClient == 0) {
		g_capData.bCapWinClient = FALSE;
	}
	else {
		g_capData.bCapWinClient = TRUE;
	}

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
	TCHAR szFileIndexL1[MAX_PATH + 1] = { 0 };
	TCHAR szFileIndexL2[MAX_PATH + 1] = { 0 };
	TCHAR szFileIndexL3[MAX_PATH + 1] = { 0 };
	TCHAR szValue[2] = { 0 };
	GetDlgItemText(hwnd, IDC_CAPTUREAREA, szCaptureArea, MAX_PATH + 1);
	SetCaptureArea(szCaptureArea);

	GetDlgItemText(hwnd, IDC_DIRECTORY, g_capData.szSaveDir, MAX_PATH + 1);
	GetDlgItemText(hwnd, IDC_SAVEFILENAME, g_capData.szSaveFileName, MAX_PATH + 1);
	GetDlgItemText(hwnd, IDC_ENV, g_capData.szEnvName, MAX_PATH + 1);
	GetDlgItemText(hwnd, IDC_FILEINDEX, szFileIndex, MAX_PATH + 1);

	g_capData.bOpenImage = IsDlgButtonChecked(hwnd, IDC_OPENIMAGE);
	g_capData.bCopy2Clipboard = IsDlgButtonChecked(hwnd, IDC_COPY2CLIPBOARD);
	g_capData.bCapWinClient = IsDlgButtonChecked(hwnd, IDC_CHKCLIENTAREA);

	WritePrivateProfileString(SECTION, KEY_CAPAREA, g_szCaptureArea, g_szIniFileFullPath);
	WritePrivateProfileString(SECTION, KEY_SAVEDIR, g_capData.szSaveDir, g_szIniFileFullPath);
	WritePrivateProfileString(SECTION, KEY_SAVEFILENAME, g_capData.szSaveFileName, g_szIniFileFullPath);
	WritePrivateProfileString(SECTION, KEY_SAVEENV, g_capData.szEnvName, g_szIniFileFullPath);

	TCHAR** lpSplit = NULL;
	int arrayLength = 0;
	SplitString(szFileIndex, TEXT("-"), &lpSplit, &arrayLength);

	if (arrayLength == 1) {
		int inputValue = _ttoi(lpSplit[0]);
		if (g_capData.idxLevel3 != inputValue) {
			g_capData.idxLevel3 = inputValue;
		}

		_itot(g_capData.idxLevel3, szFileIndexL3, 10);
		WritePrivateProfileString(SECTION, KEY_SAVEFILEIDXL1, TEXT("0"), g_szIniFileFullPath);
		WritePrivateProfileString(SECTION, KEY_SAVEFILEIDXL1, TEXT("0"), g_szIniFileFullPath);
		WritePrivateProfileString(SECTION, KEY_SAVEFILEIDXL3, szFileIndexL3, g_szIniFileFullPath);
	}else {
		int inputValue1 = _ttoi(lpSplit[0]);
		if (g_capData.idxLevel1 != inputValue1) {
			g_capData.idxLevel1 = inputValue1;
		}
		_itot(g_capData.idxLevel1, szFileIndexL1, 10);

		int inputValue2 = _ttoi(lpSplit[1]);
		if (g_capData.idxLevel2 != inputValue2) {
			g_capData.idxLevel2 = inputValue2;
		}
		_itot(g_capData.idxLevel2, szFileIndexL2, 10);

		int inputValue3 = _ttoi(lpSplit[2]);
		if (g_capData.idxLevel3 != inputValue3) {
			g_capData.idxLevel3 = inputValue3;
		}
		_itot(g_capData.idxLevel3, szFileIndexL3, 10);

		WritePrivateProfileString(SECTION, KEY_SAVEFILEIDXL1, szFileIndexL1, g_szIniFileFullPath);
		WritePrivateProfileString(SECTION, KEY_SAVEFILEIDXL1, szFileIndexL2, g_szIniFileFullPath);
		WritePrivateProfileString(SECTION, KEY_SAVEFILEIDXL3, szFileIndexL3, g_szIniFileFullPath);
	}

	for (int i = 0; i < arrayLength; i++) {
		delete[] lpSplit[0];
		lpSplit[0] = NULL;
	}

	delete[] lpSplit;
	lpSplit = NULL;


	if (g_capData.bOpenImage) {
		WritePrivateProfileString(SECTION, KEY_OPENIMAGE, VAL_ON, g_szIniFileFullPath);
	}
	else {
		WritePrivateProfileString(SECTION, KEY_OPENIMAGE, VAL_OFF, g_szIniFileFullPath);
	}

	if (g_capData.bCapWinClient) {
		WritePrivateProfileString(SECTION, KEY_CAPWINCLIENT, VAL_ON, g_szIniFileFullPath);
	}
	else {
		WritePrivateProfileString(SECTION, KEY_CAPWINCLIENT, VAL_OFF, g_szIniFileFullPath);
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


void SplitString(TCHAR* szSource, const PTCHAR delims, TCHAR*** lpSplite, int* count)
{
	LPRECT lpArea = &(g_capData.CaptureArea);
	TCHAR* result = NULL;
	result = _tcstok(szSource, delims);

	TCHAR** splitArray = new TCHAR*[5];
	int idx = 0;
	while (result != NULL) {
		int stringSize =  lstrlen(result) + 1;
		splitArray[idx] = new TCHAR[stringSize];
		lstrcpy(splitArray[idx], result);

		result = _tcstok(NULL, delims);
		idx++;
	}
	*lpSplite = splitArray;
	*count = idx;
}
