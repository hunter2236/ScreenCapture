#pragma once

BOOL InitGdiPlus();
void ReleaseGdiPlus();

BOOL SaveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName);
BOOL SavePngToFile(HBITMAP hBitmap, LPCTSTR lpFileName);
