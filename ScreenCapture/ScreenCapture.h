#pragma once

enum CaptureMode
{
    CAPMOD_AREA = 1,
    CAPMOD_WIN
};

typedef struct _captureData {
	RECT CaptureArea;
	TCHAR szSaveDir[MAX_PATH + 1];
	TCHAR szSaveFileName[MAX_PATH + 1];
	TCHAR szEnvName[MAX_PATH + 1];
	BOOL bCapWinClient;
	BOOL bCopy2Clipboard;
	BOOL bOpenImage;
	int idxLevel1;
	int idxLevel2;
	int idxLevel3;
	CaptureMode captureMode;
}CaptureData;

extern CaptureData g_capData;
extern TCHAR g_szCaptureArea[MAX_PATH + 1];

#define MAX_LOADSTRING 100

