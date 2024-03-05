#include <Windows.h>
#include <iostream>
#include <wingdi.h>
#include <fstream>
#include <filesystem>

#include "DebugOutput/DebugOutput.h"
#include "FromsoftFormat/CCM2Reader/CCM2Reader.h"

#define TEXTURE_SIZE 512
#define START_CHAR 32
#define END_CHAR 65510
#define KERNING 2

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

bool FilterGlyph(WCHAR chr)
{
    return false;
}

void WriteGlyphsToBitmap(HDC hdc, int* textureIdx, const char* filename, std::ofstream* layoutFile, CCM2Reader* pCCM, WCHAR* startChar, WCHAR* endChar, const char* fontname, int size, bool is_bold, bool is_italic)
{
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = TEXTURE_SIZE;
    bmi.bmiHeader.biHeight = TEXTURE_SIZE;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC memDC = CreateCompatibleDC(hdc);

    void* pBits;
    HBITMAP hBitmap = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    if (!hBitmap) {
        std::cerr << "Error creating DIB section." << std::endl;
        DeleteDC(memDC);
        return;
    }

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

    int nDPI = GetDeviceCaps(hdc, LOGPIXELSY);

    int nHeight = -MulDiv(size, nDPI, 72);
    int cWeight = FW_NORMAL;

    if (is_bold)
        cWeight = FW_BOLD;

    HFONT hFont = CreateFontW(nHeight, 0, 0, 0, cWeight, is_italic, 0, 0, 1, 0, 0, 2, 2, LPCWSTR(fontname));
    SelectObject(memDC, hFont);

    int x = 1, y = 1;
    WCHAR unicodeChar = 0;
    for (unicodeChar = *startChar; unicodeChar < *endChar; unicodeChar++)
    {
        WORD glyphIndex;
        if (!GetGlyphIndicesW(hdc, &unicodeChar, 1, &glyphIndex, GGI_MARK_NONEXISTING_GLYPHS)) 
            continue;

        if (FilterGlyph(unicodeChar))
            continue;

        SIZE size;
        GetTextExtentPoint32W(memDC, &unicodeChar, 1, &size);

        if (size.cx == 0 || size.cy == 0)
            continue;

        if (x + size.cx > TEXTURE_SIZE)
        {
            x = 1;
            y += size.cy;
        }

        if (y + size.cy > TEXTURE_SIZE)
        {
            *startChar = unicodeChar;
            *textureIdx += 1;
            break;
        }

        RECT rect = { x, y, x + size.cx, y + size.cy }; // Adjust as needed
        DrawTextW(memDC, &unicodeChar, -1, &rect, DT_LEFT | DT_TOP);

        pCCM->m_texRegions.push_back(TexRegion(x, y, x + size.cx, y + size.cy));

        int prespace = 0;
        int width = size.cx;
        int advance = width + KERNING;

        pCCM->m_glyphs.push_back(Glyph(unicodeChar, pCCM->m_glyphs.size(), *textureIdx, prespace, width, advance));

        char buf[500];
        sprintf_s(buf, "code=%d, textureId=%d, prespace=%d, width=%d, advance=%d, top=(%d, %d), bottom=(%d, %d)\n", unicodeChar, *textureIdx, prespace, width, advance, x, y, x + size.cx, y + size.cy);

        std::string glyph_layout(buf);
        layoutFile->write(glyph_layout.c_str(), glyph_layout.length());

        x += size.cx;
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
}

std::string GetFilename(const char* fontname, int size)
{
    std::string filename = std::string(fontname) + std::to_string(size);

    for (size_t i = 0; i < filename.length(); ++i) 
    {
        if (filename[i] == ' ') 
        {
            filename.erase(i, 1);
            --i;
        }
    }

    return filename;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        Debug::Alert(Debug::LVL_ERROR, "CCMFontBuilder", "Wrong argument count %d. Usage: CCMFontBuilder.exe <fontname> <fontsize>", argc);
        return 0;
    }

    Debug::DebuggerMessage(Debug::LVL_DEBUG, "Args: %ls, %ls, %ls, %ls\n", argv[1], argv[2], argv[3], argv[4]);

    std::string fontname = argv[1];
    int size = std::stod(argv[2]);

    std::string is_bold = "false";
    std::string is_italic = "false";

    if (argc == 4)
        is_bold = argv[3];

    if (argc == 5)
        is_bold = argv[4];

    HDC hdc = GetDC(GetDesktopWindow());
    SetBkMode(hdc, TRANSPARENT);

    bool bold = false;
    bool italic = false;

    if (is_bold.compare("true") == 0)
        bold = true;

    if (is_italic.compare("true") == 0)
        italic = true;

    std::string filename = GetFilename(fontname.c_str(), size);
    std::string outpath = "Out/" + filename + "/";

    std::filesystem::create_directories(outpath);

    char layout_name[255];
    sprintf_s(layout_name, "%s.txt", filename.c_str());
    std::ofstream layout_file(std::string("Out/" + filename + "/" + std::string(layout_name)).c_str());

    char ccm_name[255];
    sprintf_s(ccm_name, "%s.ccm", filename.c_str());
    std::string ccm_out = outpath + ccm_name;

    WCHAR start = START_CHAR;
    WCHAR end = END_CHAR;
    int textureId = 0;

    CCM2Reader ccm2;
    ccm2.m_init = true;

    while (start < end)
    {
        char tex_name[255];
        sprintf_s(tex_name, "%s_%04d.bmp", filename.c_str(), textureId);

        Debug::DebuggerMessage(Debug::LVL_DEBUG, "Write file %ls (startChar=%d, endChar=%d)\n", tex_name, start, end);

        WriteGlyphsToBitmap(hdc, &textureId, std::string("Out/" + filename + "/" + std::string(tex_name)).c_str(), &layout_file, &ccm2, &start, &end, fontname.c_str(), size, bold, italic);
    }

    ccm2.m_header = Header(TEXTURE_SIZE, ccm2.m_texRegions.size(), ccm2.m_glyphs.size(), textureId);
    ccm2.WriteFile(ccm_out);

    Debug::DebuggerMessage(Debug::LVL_DEBUG, "Generated %d textures\n", textureId);

    return 1;
}