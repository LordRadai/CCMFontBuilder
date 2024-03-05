#include <Windows.h>
#include <iostream>
#include <wingdi.h>
#include <fstream>

#include "DebugOutput/DebugOutput.h"

#define FONTNAME_W L"MS Gothic"
#define FILENAME_W L"MSGothic"

const int TEXTURE_SIZE = 512;
const int FONT_SIZE = 14;
const int WEIGHT = FW_NORMAL;
const bool ITALIC = false;

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

void PrintGlyphMetrics(GLYPHMETRICS gm, WCHAR unicodeChar)
{
    Debug::DebuggerMessage(Debug::LVL_INFO, "Glyph metrics for Unicode character U+%04X:\n", unicodeChar);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tWidth: %ld\n", gm.gmBlackBoxX);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tHeight: %ld\n", gm.gmBlackBoxY);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tLeft side bearing: %ld\n", gm.gmptGlyphOrigin.x);
    Debug::DebuggerMessage(Debug::LVL_INFO, "\tTop side bearing: %ld\n\n", gm.gmptGlyphOrigin.y);
}

void IdentityMat(MAT2* mat)
{
    mat->eM11.value = 1;
    mat->eM12.value = 0;

    mat->eM21.value = 0;
    mat->eM22.value = 1;
}

void WriteGlyphsToBitmap(HDC hdc, int* textureIdx, const wchar_t* filename, std::ofstream* layoutFile, WCHAR* startChar, WCHAR* endChar)
{
    // Create a compatible DIB section
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = TEXTURE_SIZE;
    bmi.bmiHeader.biHeight = TEXTURE_SIZE;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC memDC = CreateCompatibleDC(hdc);

    void* pBits; // Pointer to the bitmap bits
    HBITMAP hBitmap = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    if (!hBitmap) {
        std::cerr << "Error creating DIB section." << std::endl;
        DeleteDC(memDC);
        return;
    }

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

    int nDPI = GetDeviceCaps(hdc, LOGPIXELSY);

    int nHeight = -MulDiv(FONT_SIZE, nDPI, 72);

    HFONT font = CreateFontW(nHeight, 0, 0, 0, WEIGHT, ITALIC, 0, 0, 1, 0, 0, 2, 2, FONTNAME_W);
    SelectObject(memDC, font);

    int x = 0, y = 0;
    WCHAR unicodeChar = 0;
    for (unicodeChar = *startChar; unicodeChar < *endChar; unicodeChar++)
    {
        SIZE size;
        GetTextExtentPoint32W(memDC, &unicodeChar, 1, &size);

        if (size.cx == 0 || size.cy == 0)
            continue;

        RECT rect = { x, y, x + size.cx, y + size.cy }; // Adjust as needed
        DrawTextW(memDC, &unicodeChar, -1, &rect, DT_LEFT | DT_TOP);

        int prespace = 0;
        int width = size.cx;
        int advance = size.cy;

        char buf[500];
        sprintf_s(buf, "%d,%d,%d,%d,%d\n", unicodeChar, *textureIdx, prespace, width, advance);

        std::string glyph_layout(buf);
        layoutFile->write(glyph_layout.c_str(), glyph_layout.length());

        x += size.cx;
        if (x > TEXTURE_SIZE)
        {
            x = 0;
            y += size.cy;
        }

        if (y > TEXTURE_SIZE)
        {
            *startChar = unicodeChar;
            *textureIdx += 1;
            break;
        }
    }

    *startChar = unicodeChar;

    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    BITMAPFILEHEADER bmfHeader = { 0 };
    bmfHeader.bfType = 0x4D42; // "BM"
    bmfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmp.bmWidthBytes * bmp.bmHeight;
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER biHeader = { 0 };
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biWidth = bmp.bmWidth;
    biHeader.biHeight = bmp.bmHeight;
    biHeader.biPlanes = 1;
    biHeader.biBitCount = bmp.bmBitsPixel;
    biHeader.biCompression = BI_RGB;

    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file) {
        std::cerr << "Error creating file." << std::endl;
        SelectObject(memDC, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        return;
    }

    file.write(reinterpret_cast<const char*>(&bmfHeader), sizeof(BITMAPFILEHEADER));
    file.write(reinterpret_cast<const char*>(&biHeader), sizeof(BITMAPINFOHEADER));
    file.write(reinterpret_cast<const char*>(pBits), bmp.bmWidthBytes * bmp.bmHeight);

    // Cleanup
    file.close();
    SelectObject(memDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(memDC);

    DeleteDC(memDC);
}

int main()
{
    int status = _wmkdir(L"Out/");

    HDC hdc = GetDC(GetDesktopWindow());
    SetBkMode(hdc, TRANSPARENT);

    wchar_t layout_name[255];
    swprintf_s(layout_name, L"%ls.txt", FILENAME_W);
    std::ofstream layout_file(std::wstring(L"Out/" + std::wstring(layout_name)).c_str());

    WCHAR start = 32;
    WCHAR end = 65510;
    int textureId = 0;

    while (start < end)
    {
        wchar_t tex_name[255];
        swprintf_s(tex_name, L"%ls_%04d.bmp", FILENAME_W, textureId);

        Debug::DebuggerMessage(Debug::LVL_DEBUG, "Write file %ls (startChar=%d, endChar=%d)\n", tex_name, start, end);

        WriteGlyphsToBitmap(hdc, &textureId, std::wstring(L"Out/" + std::wstring(tex_name)).c_str(), &layout_file , &start, &end);
    }

    return 1;
}