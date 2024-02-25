#include <Windows.h>
#include <iostream>
#include <wingdi.h>
#include "Debug/Debug.h"

#define FONTNAME_W L"MS Gothic"

int main()
{
    HDC hdc = GetDC(NULL);
    int nDPI = GetDeviceCaps(hdc, LOGPIXELSY);

    int italic = 0;
    int cWeight = 400;
    int fontSize = 12;

    int nHeight = -MulDiv(fontSize, nDPI, 72);

    Debug::DebuggerMessage(Debug::LVL_INFO, "Create font %ls\n", FONTNAME_W);
    HFONT font = CreateFontW(nHeight, 0, 0, 0, cWeight, italic, 0, 0, 1, 0, 0, 2, 2, FONTNAME_W);

    if (font == 0)
    {
        Debug::Alert(Debug::LVL_ERROR, "CCMFontBuilder.cpp", "Failed to create font\n");

        return 0;
    }

    HFONT hOldFont = (HFONT)SelectObject(hdc, font);

    TEXTMETRICW tm;
    GetTextMetricsW(hdc, &tm);

    Debug::DebuggerMessage(Debug::LVL_INFO, "Text Metrics:\n");
    Debug::DebuggerMessage(Debug::LVL_INFO, "  Height of character: %d\n", tm.tmHeight);
    Debug::DebuggerMessage(Debug::LVL_INFO, "  Average width of character: %d\n", tm.tmAveCharWidth);
    Debug::DebuggerMessage(Debug::LVL_INFO, "  Maximum width of character: %d\n", tm.tmMaxCharWidth);
    Debug::DebuggerMessage(Debug::LVL_INFO, "  Height of font: %d\n", tm.tmHeight);
    Debug::DebuggerMessage(Debug::LVL_INFO, "  Ascent: %d\n", tm.tmAscent);
    Debug::DebuggerMessage(Debug::LVL_INFO, "  Descent: %d\n", tm.tmDescent);
    Debug::DebuggerMessage(Debug::LVL_INFO, "  Internal leading: %d\n", tm.tmInternalLeading);
    Debug::DebuggerMessage(Debug::LVL_INFO, "  External leading: %d\n", tm.tmExternalLeading);

    bool done = false;
    while (!done)
    {
        TextOut(hdc, 0, 0, L"Hello, World!", lstrlenW(L"Hello, world!"));
    }
}