#include "stdafx.h"
#include "MyMutex.h"

#define MYWINMUTEX	TEXT("ScreenCapture42")
static HANDLE hMutex = NULL;

BOOL isSingleRun()
{
	// Mutex multi run check
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MYWINMUTEX);
	if (hMutex) {
		CloseHandle(hMutex);
		return FALSE;
	}

	{
		SECURITY_ATTRIBUTES SA;
		PSECURITY_DESCRIPTOR pSD = NULL;

		pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (NULL == pSD) {
			return FALSE;
		}
		InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(pSD, TRUE, (PACL)NULL, FALSE);

		SA.nLength = sizeof(SA);
		SA.lpSecurityDescriptor = pSD;
		SA.bInheritHandle = TRUE;

		hMutex = CreateMutex(&SA, 0, MYWINMUTEX);
		DWORD dwLastError = GetLastError();
		if ((NULL != hMutex) && (ERROR_ALREADY_EXISTS == dwLastError)) {
			CloseHandle(hMutex);
			hMutex = NULL;
			LocalFree(pSD);
			return FALSE;
		}

		LocalFree(pSD);
	}

	return TRUE;
}

void ReleaseMutex()
{
	if (hMutex == NULL) {
		return;
	}

	CloseHandle(hMutex);
	hMutex = NULL;
}
