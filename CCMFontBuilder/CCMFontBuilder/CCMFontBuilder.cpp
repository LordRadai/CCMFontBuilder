#include <Windows.h>
#include <iostream>
#include <wingdi.h>
#include <fstream>

#include "DebugOutput/DebugOutput.h"

#define FONTNAME_W L"MS Gothic"

void PrintTextMetrics(TEXTMETRICW tm)
{
    Debug::DebuggerMessage(Debug::LVL_INFO, "Text Metrics:\n");
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tHeight of character: %d\n", tm.tmHeight);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tAverage width of character: %d\n", tm.tmAveCharWidth);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tMaximum width of character: %d\n", tm.tmMaxCharWidth);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tHeight of font: %d\n", tm.tmHeight);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tAscent: %d\n", tm.tmAscent);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tDescent: %d\n", tm.tmDescent);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tInternal leading: %d\n", tm.tmInternalLeading);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tExternal leading: %d\n\n", tm.tmExternalLeading);
}

void WriteTextMetrics(std::ofstream* pOut, TEXTMETRICW tm)
{
}

void PrintGlyphMetrics(GLYPHMETRICS gm, WCHAR unicodeChar)
{
    Debug::DebuggerMessage(Debug::LVL_INFO, "Glyph metrics for Unicode character U+%04X:\n", unicodeChar);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tWidth: %ld\n", gm.gmBlackBoxX);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tHeight: %ld\n", gm.gmBlackBoxY);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tLeft side bearing: %ld\n", gm.gmptGlyphOrigin.x);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tTop side bearing: %ld\n\n", gm.gmptGlyphOrigin.y);
}

void WriteGlyphMetrics(std::ofstream* pOut, GLYPHMETRICS gm, WCHAR unicodeChar)
{
}

void IdentityMat(MAT2* mat)
{
    mat->eM11.value = 1;
    mat->eM12.value = 0;

    mat->eM21.value = 0;
    mat->eM22.value = 1;
}

int main()
{
    HDC hdc = GetDC(GetActiveWindow());
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

    PrintTextMetrics(tm);

    WCHAR startChar = 0x0000; // Start character
    WCHAR endChar = 0xFFFF;   // End character

    for (WCHAR unicodeChar = startChar; unicodeChar < endChar; unicodeChar++) 
    {
        WORD glyphIndex = 0;
        if (GetGlyphIndicesW(hdc, &unicodeChar, 1, &glyphIndex, GGI_MARK_NONEXISTING_GLYPHS) != GDI_ERROR) 
        {
            GLYPHMETRICS gm;
            MAT2 pos;
            IdentityMat(&pos);

            DWORD dwGlyphSize = GetGlyphOutlineW(hdc, glyphIndex, GGO_GLYPH_INDEX | GGO_METRICS, &gm, 0, NULL, &pos);
            if (dwGlyphSize != GDI_ERROR) 
                PrintGlyphMetrics(gm, unicodeChar);
            else 
                Debug::DebuggerMessage(Debug::LVL_ERROR, "Failed to retrieve glyph metrics for Unicode character U+%04X.\n", unicodeChar);
        }
        else 
            Debug::DebuggerMessage(Debug::LVL_ERROR, "Failed to convert Unicode character U+%04X to glyph index.\n", unicodeChar);
    }

    bool done = false;
    while (!done)
    {
        int xPos = 10, yPos = 10;

        for (WCHAR unicodeChar = startChar; unicodeChar < endChar; unicodeChar++)
        {
            WCHAR str[2] = { unicodeChar, '\0' };
            TextOutW(hdc, xPos, yPos, str, 1);
            xPos += 20;
            if (xPos > 600) {
                xPos = 10;
                yPos += 30;
            }
        }
    }
}