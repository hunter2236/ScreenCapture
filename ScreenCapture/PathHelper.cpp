#include "stdafx.h"
#include "PathHelper.h"

void GetDirectory(LPCTSTR szFullPath, LPTSTR szDir)
{
    int strLen = lstrlen(szFullPath);
    if (szFullPath == NULL || strLen == 0) {
        return;
    }

    LPCTSTR pLast = szFullPath + strLen;
    LPCTSTR pFind = TEXT("\\");
    int findLen = lstrlen(pFind);
    BOOL bFind = FALSE;
    for (; pLast >= szFullPath; pLast--)
    {
        if (_tcsnccmp(pLast, pFind, findLen) == 0)
        {
            bFind = TRUE;
            break;
        }
    }
    if (bFind) {
        int pos = pLast - szFullPath;
        _tcsncpy(szDir, szFullPath, pos);
    }
}

BOOL IsDirectoryExist(LPCTSTR szDir)
{
    DWORD fa;
    ::SetLastError(ERROR_SUCCESS);
    fa = ::GetFileAttributesW(szDir);
    if (fa == INVALID_FILE_ATTRIBUTES)
    {
        DWORD error = ::GetLastError();
        return error != ERROR_PATH_NOT_FOUND &&
            error != ERROR_FILE_NOT_FOUND;
    }
    else
    {
        return (fa & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
}
