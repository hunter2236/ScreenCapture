#include "stdafx.h"
#include "initgdiplus.h"
#include "ImageHelper.h"
#include "PathHelper.h"


static InitGDIPlus GDI_Plus_Controler;
static CLSID g_pngClsid;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

BOOL InitGdiPlus()
{
    GDI_Plus_Controler.Initialize();
    GetEncoderClsid(L"image/png", &g_pngClsid);
    return TRUE;
}

void ReleaseGdiPlus()
{
    GDI_Plus_Controler.Deinitialize();
}

BOOL SavePngToFile(HBITMAP hBitmap, LPCTSTR lpFileName)
{
    TCHAR szTemp[MAX_PATH + 1] = { 0 };
    GetDirectory(lpFileName, szTemp);
    if (!IsDirectoryExist(szTemp)) {
        if (!CreateDirectory(szTemp, NULL)) {
            return FALSE;
        }
    }

    Bitmap* pBmpSave = Bitmap::FromHBITMAP(hBitmap, NULL);
    BOOL result = pBmpSave->Save(lpFileName, &g_pngClsid, NULL) == Ok ? TRUE : FALSE;
    delete pBmpSave;
    pBmpSave = NULL;
    return result;
}




BOOL SaveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName)
{
    HDC hDC = NULL;
    int iBits = 0;
    WORD wBitCount = 0;
    DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
    BITMAP Bitmap = { 0 };
    BITMAPFILEHEADER bmfHdr = { 0 };
    BITMAPINFOHEADER bi = { 0 };
    LPBITMAPINFOHEADER lpbi = { 0 };
    HANDLE fh = NULL, hDib = NULL, hPal = NULL, hOldPal = NULL;

    HDC hWndDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
    hDC = ::CreateCompatibleDC(hWndDC);
    iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
    DeleteDC(hDC);

    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else if (iBits <= 24)
        wBitCount = 24;
    else
        wBitCount = 24;

    if (wBitCount <= 8) {
        dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);
    }

    GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = Bitmap.bmWidth;
    bi.biHeight = Bitmap.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBitCount;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

    hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    *lpbi = bi;

    hPal = GetStockObject(DEFAULT_PALETTE);
    if (hPal)
    {
        hDC = ::GetDC(NULL);
        hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
        RealizePalette(hDC);
    }

    GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
        (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
        + dwPaletteSize,
        (LPBITMAPINFO)
        lpbi, DIB_RGB_COLORS);

    if (hOldPal) {
        SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
        RealizePalette(hDC);
        ReleaseDC(NULL, hDC);
    }

    fh = CreateFile(lpFileName, GENERIC_WRITE,
        0, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (fh == INVALID_HANDLE_VALUE)
        return FALSE;


    bmfHdr.bfType = 0x4D42; // "BM" 
    dwDIBSize = sizeof(BITMAPFILEHEADER)
        + sizeof(BITMAPINFOHEADER)
        + dwPaletteSize + dwBmBitsSize;
    bmfHdr.bfSize = dwDIBSize;
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)
        + (DWORD)sizeof(BITMAPINFOHEADER)
        + dwPaletteSize;


    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);


    WriteFile(fh, (LPSTR)lpbi, dwDIBSize,
        &dwWritten, NULL);

    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);

    return TRUE;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0;
    UINT size = 0;
    ImageCodecInfo* pImageCodecInfo = NULL;
    GetImageEncodersSize(&num, &size);
    if (size == 0)
    {
        return -1;
    }
    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
    {
        return -1;
    }
    GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            pImageCodecInfo = NULL;
            return j;
        }
    }
    free(pImageCodecInfo);
    pImageCodecInfo = NULL;
    return -1;
}
