// ScreenCaptureHook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ScreenCaptureHook.h"

#pragma data_seg("Shared")
HINSTANCE   g_hInstanceEventDll = NULL;
HWND        g_hMainWnd = NULL;
HHOOK       g_hKeyboardllHook = NULL;
#pragma data_seg()

#pragma comment(linker, "/Section:Shared,RWS")

BOOL NotifyPowerKeyDown();
LRESULT WINAPI LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
DWORD g_dwPowerButtonKeyCode = VK_ZOOM;

SCREENCAPTUREHOOK_API BOOL SetHook(HWND hWnd, DWORD dwPowerButtonKeyCode)
{
	DWORD dwError = 0;
	if (g_hKeyboardllHook != NULL) {
		return FALSE;
	}

	g_hKeyboardllHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, g_hInstanceEventDll, 0);
	if (g_hKeyboardllHook == NULL) {
		g_hMainWnd = NULL;
		return FALSE;
	}

	g_hMainWnd = hWnd;
	g_dwPowerButtonKeyCode = dwPowerButtonKeyCode;

	return TRUE;
}

SCREENCAPTUREHOOK_API BOOL UnHook()
{
	DWORD dwError = 0;
	if (g_hKeyboardllHook == NULL) {
		return FALSE;
	}

	BOOL bRet = UnhookWindowsHookEx(g_hKeyboardllHook);
	if (bRet == FALSE) {
		dwError = GetLastError();
	}
	g_hKeyboardllHook = NULL;
	g_hMainWnd = NULL;
	return TRUE;
}

LRESULT WINAPI LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
	{
		return CallNextHookEx(g_hKeyboardllHook, nCode, wParam, lParam);
	}

	if (nCode == HC_ACTION)
	{
		LPKBDLLHOOKSTRUCT pcKBHook = (LPKBDLLHOOKSTRUCT)lParam;

		//if(((DWORD)lParam & 0x40000000))
		if (pcKBHook->vkCode == g_dwPowerButtonKeyCode && (pcKBHook->flags & LLKHF_ALTDOWN))
			/*			(pcKBHook->flags & LLKHF_ALTDOWN) && */
		{
			if (WM_KEYDOWN == wParam || WM_SYSKEYDOWN == wParam)
				//if (wParam == WM_KEYDOWN)
			{
				NotifyPowerKeyDown();
				OutputDebugString(TEXT("Alt+Hotkey Down!\n"));
				return 1;
			}
		}

	}

	return CallNextHookEx(g_hKeyboardllHook, nCode, wParam, lParam);
}

BOOL NotifyPowerKeyDown()
{
	return SendMessage(g_hMainWnd, WM_SCREENCAPTURE, 1, NULL);
}

BOOL NotifyPowerKeyUp()
{
	return SendMessage(g_hMainWnd, WM_SCREENCAPTURE, 0, NULL);
}
