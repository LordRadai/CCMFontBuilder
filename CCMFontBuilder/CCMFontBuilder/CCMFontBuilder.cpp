// CCMFontBuilder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <iostream>
#include <wingdi.h>
#define FONTNAME_W L"MS Gothic"

int main()
{
    HDC hdc = GetDC(NULL);
    int nDPI = GetDeviceCaps(hdc, LOGPIXELSY);

    int italic = 0;
    int cWeight = 400;
    int fontSize = 12;

    int nHeight = -MulDiv(fontSize, nDPI, 72);

    HFONT font = CreateFontW(nHeight, 0, 0, 0, cWeight, italic, 0, 0, 1, 0, 0, 2, 2, FONTNAME_W);

    if (font == 0)
        return 0;

    HFONT hOldFont = (HFONT)SelectObject(hdc, font);

    TEXTMETRICW tm;
    GetTextMetricsW(hdc, &tm);

    printf("Text Metrics:\n");
    printf("  Height of character: %d\n", tm.tmHeight);
    printf("  Average width of character: %d\n", tm.tmAveCharWidth);
    printf("  Maximum width of character: %d\n", tm.tmMaxCharWidth);
    printf("  Height of font: %d\n", tm.tmHeight);
    printf("  Ascent: %d\n", tm.tmAscent);
    printf("  Descent: %d\n", tm.tmDescent);
    printf("  Internal leading: %d\n", tm.tmInternalLeading);
    printf("  External leading: %d\n", tm.tmExternalLeading);

    bool done = false;
    while (!done)
    {
        TextOut(hdc, 0, 0, L"Hello, World!", lstrlenW(L"Hello, world!"));
    }
}