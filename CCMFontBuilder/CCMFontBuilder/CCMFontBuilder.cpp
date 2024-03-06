#include <Windows.h>
#include <iostream>
#include <wingdi.h>
#include <fstream>
#include <filesystem>
#include <DirectXTex.h>
#include <wincodec.h>

#include "StringHelper/StringHelper.h"
#include "DebugOutput/DebugOutput.h"
#include "FromsoftFormat/CCM2Reader/CCM2Reader.h"

#define TEXTURE_SIZE 512
#define START_CHAR 32
#define END_CHAR 65510
#define KERNING 0

void PrintTextMetrics(TEXTMETRICW tm)
{
    printf_s("Text Metrics:\n");
    printf_s("\tHeight of character: %d\n", tm.tmHeight);
    printf_s("\tAverage width of character: %d\n", tm.tmAveCharWidth);
    printf_s("\tMaximum width of character: %d\n", tm.tmMaxCharWidth);
    printf_s("\tHeight of font: %d\n", tm.tmHeight);
    printf_s("\tAscent: %d\n", tm.tmAscent);
    printf_s("\tDescent: %d\n", tm.tmDescent);
    printf_s("\tInternal leading: %d\n", tm.tmInternalLeading);
    printf_s("\tExternal leading: %d\n\n", tm.tmExternalLeading);
}

void PrintGlyphMetrics(GLYPHMETRICS gm, WCHAR unicodeChar)
{
    printf_s("Glyph metrics for Unicode character U+%04X:\n", unicodeChar);
    printf_s("\tWidth: %ld\n", gm.gmBlackBoxX);
    printf_s("\tHeight: %ld\n", gm.gmBlackBoxY);
    printf_s("\tLeft side bearing: %ld\n", gm.gmptGlyphOrigin.x);
    printf_s("\tTop side bearing: %ld\n\n", gm.gmptGlyphOrigin.y);
}

void IdentityMat(MAT2* mat)
{
    mat->eM11.value = 1;
    mat->eM12.value = 0;

    mat->eM21.value = 0;
    mat->eM22.value = 1;
}

HRESULT SaveDIBSectionToPNG(HBITMAP hBitmap, LPCWSTR filename) {
    HRESULT hr = S_OK;

    // Initialize COM
    hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM" << std::endl;
        return hr;
    }

    // Create WIC imaging factory
    IWICImagingFactory* pFactory = nullptr;
    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));
    if (FAILED(hr)) {
        std::cerr << "Failed to create WIC imaging factory" << std::endl;
        CoUninitialize();
        return hr;
    }

    // Create WIC stream for writing
    IWICStream* pStream = nullptr;
    hr = pFactory->CreateStream(&pStream);
    if (FAILED(hr)) {
        std::cerr << "Failed to create WIC stream" << std::endl;
        pFactory->Release();
        CoUninitialize();
        return hr;
    }

    // Initialize stream with output filename
    hr = pStream->InitializeFromFilename(filename, GENERIC_WRITE);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize WIC stream with filename" << std::endl;
        pStream->Release();
        pFactory->Release();
        CoUninitialize();
        return hr;
    }

    // Create PNG encoder
    IWICBitmapEncoder* pEncoder = nullptr;
    hr = pFactory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &pEncoder);
    if (FAILED(hr)) {
        std::cerr << "Failed to create PNG encoder" << std::endl;
        pStream->Release();
        pFactory->Release();
        CoUninitialize();
        return hr;
    }

    // Initialize encoder with stream
    hr = pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize encoder" << std::endl;
        pEncoder->Release();
        pStream->Release();
        pFactory->Release();
        CoUninitialize();
        return hr;
    }

    // Create encoder frame
    IWICBitmapFrameEncode* pFrameEncode = nullptr;
    hr = pEncoder->CreateNewFrame(&pFrameEncode, nullptr);
    if (FAILED(hr)) {
        std::cerr << "Failed to create encoder frame" << std::endl;
        pEncoder->Release();
        pStream->Release();
        pFactory->Release();
        CoUninitialize();
        return hr;
    }

    // Initialize frame encode with DIB section bitmap
    hr = pFrameEncode->Initialize(nullptr);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize frame encode" << std::endl;
        pFrameEncode->Release();
        pEncoder->Release();
        pStream->Release();
        pFactory->Release();
        CoUninitialize();
        return hr;
    }

    // Set size of frame
    BITMAP bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);
    UINT width = bm.bmWidth;
    UINT height = bm.bmHeight;
    hr = pFrameEncode->SetSize(width, height);
    if (FAILED(hr)) {
        std::cerr << "Failed to set size of frame" << std::endl;
        pFrameEncode->Release();
        pEncoder->Release();
        pStream->Release();
        pFactory->Release();
        CoUninitialize();
        return hr;
    }

    // Set pixel format of frame
    hr = pFrameEncode->SetPixelFormat((WICPixelFormatGUID*)&GUID_WICPixelFormat32bppBGRA);
    if (FAILED(hr)) {
        std::cerr << "Failed to set pixel format of frame" << std::endl;
        pFrameEncode->Release();
        pEncoder->Release();
        pStream->Release();
        pFactory->Release();
        CoUninitialize();
        return hr;
    }

    // Write bitmap to frame
    HDC hDC = CreateCompatibleDC(nullptr);
    HGDIOBJ hOldObj = SelectObject(hDC, hBitmap);
    hr = pFrameEncode->WriteSource(reinterpret_cast<IWICBitmapSource*>(hBitmap), nullptr);
    if (FAILED(hr)) {
        std::cerr << "Failed to write bitmap to frame" << std::endl;
        DeleteDC(hDC);
        pFrameEncode->Release();
        pEncoder->Release();
        pStream->Release();
        pFactory->Release();
        CoUninitialize();
        return hr;
    }

    // Release resources
    DeleteDC(hDC);
    SelectObject(hDC, hOldObj);

    // Commit frame encode
    hr = pFrameEncode->Commit();
    if (FAILED(hr)) {
        std::cerr << "Failed to commit frame encode" << std::endl;
        pFrameEncode->Release();
        pEncoder->Release();
        pStream->Release();
        pFactory->Release();
        CoUninitialize();
        return hr;
    }

    // Commit encoder
    hr = pEncoder->Commit();
    if (FAILED(hr)) {
        std::cerr << "Failed to commit encoder" << std::endl;
    }

    // Release COM objects
    pFrameEncode->Release();
    pEncoder->Release();
    pStream->Release();
    pFactory->Release();

    // Uninitialize COM
    CoUninitialize();

    return hr;
}
bool WriteGlyphsToBitmapCharList(HDC memDC, int* textureIdx, const char* filename, std::ofstream* layoutFile, CCM2Reader* pCCM, std::vector<WCHAR> charList, WCHAR* charIdx)
{
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = TEXTURE_SIZE;
    bmi.bmiHeader.biHeight = TEXTURE_SIZE;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits;
    HBITMAP hBitmap = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    if (!hBitmap) {
        printf_s("Error creating DIB section\n");
        return false;
    }

    SelectObject(memDC, hBitmap);

    COLORREF transparentColor = RGB(0, 0, 0);
    HBRUSH hTransparentBrush = CreateSolidBrush(transparentColor);
    RECT rc = { 0, 0, TEXTURE_SIZE, TEXTURE_SIZE };
    FillRect(memDC, &rc, hTransparentBrush);
    DeleteObject(hTransparentBrush);

    int x = 1, y = 1;

    while (*charIdx < charList.size())
    {
        WCHAR unicodeChar = charList[*charIdx];

        WORD glyphIndex;
        if (!GetGlyphIndicesW(memDC, &unicodeChar, 1, &glyphIndex, GGI_MARK_NONEXISTING_GLYPHS))
        {
            *charIdx += 1;
            continue;
        }

        SIZE size;
        GetTextExtentPoint32W(memDC, &unicodeChar, 1, &size);

        if (size.cx == 0 || size.cy == 0)
        {
            *charIdx += 1;
            continue;
        }

        if (x + size.cx > TEXTURE_SIZE)
        {
            x = 1;
            y += size.cy;
        }

        if (y + size.cy > TEXTURE_SIZE)
        {
            *charIdx += 1;
            *textureIdx += 1;
            break;
        }

        SetBkMode(memDC, TRANSPARENT);
        SetTextColor(memDC, RGB(255, 255, 255));
        RECT rect = { x, y, x + size.cx, y + size.cy }; // Adjust as needed
        DrawTextW(memDC, &unicodeChar, -1, &rect, DT_LEFT | DT_TOP);

        pCCM->AddTexRegion(TexRegion(x, y, x + size.cx, y + size.cy));

        int prespace = 0;
        int width = size.cx;
        int advance = width + KERNING;

        pCCM->AddGlyph(Glyph(unicodeChar, pCCM->GetGlyphCount(), *textureIdx, prespace, width, advance));

        char buf[500];
        sprintf_s(buf, "code=%d, textureId=%d, prespace=%d, width=%d, advance=%d, top=(%d, %d), bottom=(%d, %d)\n", unicodeChar, *textureIdx, prespace, width, advance, x, y, x + size.cx, y + size.cy);

        std::string glyph_layout(buf);
        layoutFile->write(glyph_layout.c_str(), glyph_layout.length());

        x += size.cx;

        *charIdx += 1;
    }

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
    biHeader.biBitCount = 32;
    biHeader.biCompression = BI_RGB;

    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file) {
        printf_s("Error creating file\n");
        file.close();
        DeleteObject(hBitmap);
        return false;
    }

    file.write(reinterpret_cast<const char*>(&bmfHeader), sizeof(BITMAPFILEHEADER));
    file.write(reinterpret_cast<const char*>(&biHeader), sizeof(BITMAPINFOHEADER));
    file.write(reinterpret_cast<const char*>(pBits), bmp.bmWidthBytes * bmp.bmHeight);

    file.close();

    // Cleanup
    DeleteObject(hBitmap);

    return true;
}

bool WriteGlyphsToBitmap(int* textureIdx, const char* filename, std::ofstream* layoutFile, CCM2Reader* pCCM, WCHAR* startChar, WCHAR* endChar, const char* fontname, int size, bool is_bold, bool is_italic)
{
    HDC memDC = CreateCompatibleDC(nullptr);
    SetBkMode(memDC, TRANSPARENT);

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = TEXTURE_SIZE;
    bmi.bmiHeader.biHeight = TEXTURE_SIZE;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits;
    HBITMAP hBitmap = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    if (!hBitmap) {
        printf_s("Error creating DIB section\n");
        DeleteDC(memDC);
        return false;
    }

    SelectObject(memDC, hBitmap);

    int nDPI = GetDeviceCaps(memDC, LOGPIXELSY);

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
        if (!GetGlyphIndicesW(memDC, &unicodeChar, 1, &glyphIndex, GGI_MARK_NONEXISTING_GLYPHS))
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

        SetTextColor(memDC, RGB(255, 255, 255));
        RECT rect = { x, y, x + size.cx, y + size.cy }; // Adjust as needed
        DrawTextW(memDC, &unicodeChar, -1, &rect, DT_LEFT | DT_TOP);

        pCCM->AddTexRegion(TexRegion(x, y, x + size.cx, y + size.cy));

        int prespace = 0;
        int width = size.cx;
        int advance = width + KERNING;

        pCCM->AddGlyph(Glyph(unicodeChar, pCCM->GetGlyphCount(), *textureIdx, prespace, width, advance));

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
    biHeader.biBitCount = 32;
    biHeader.biCompression = BI_RGB;

    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file) {
        printf_s("Error creating file\n");
        file.close();
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        return false;
    }

    file.write(reinterpret_cast<const char*>(&bmfHeader), sizeof(BITMAPFILEHEADER));
    file.write(reinterpret_cast<const char*>(&biHeader), sizeof(BITMAPINFOHEADER));
    file.write(reinterpret_cast<const char*>(pBits), bmp.bmWidthBytes * bmp.bmHeight);

    file.close();

    // Cleanup
    DeleteObject(hBitmap);
    DeleteDC(memDC);

    return true;
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

bool GenerateBitmapSubset(WCHAR startChar, WCHAR endChar, int* textureIdx, const char* filename, std::ofstream* layoutFile, CCM2Reader* pCCM, const char* fontname, int size, bool is_bold, bool is_italic)
{
    while (startChar < endChar)
    {
        char tex_name[255];
        sprintf_s(tex_name, "%s_%04d.png", filename, *textureIdx);

        printf_s("Write file %s (startChar=%d, endChar=%d)\n", tex_name, startChar, endChar);

        if (!WriteGlyphsToBitmap(textureIdx, std::string("Out/" + std::string(filename) + "/" + std::string(tex_name)).c_str(), layoutFile, pCCM, &startChar, &endChar, fontname, size, is_bold, is_italic))
            return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        Debug::Alert(Debug::LVL_ERROR, "CCMFontBuilder", "Wrong argument count %d. Usage: CCMFontBuilder.exe <fontname> <fontsize>", argc);
        return 0;
    }

    std::string fontname = argv[1];
    int size = std::stod(argv[2]);

    std::string is_bold = "false";
    std::string is_italic = "false";

    if (argc == 4)
        is_bold = argv[3];

    if (argc == 5)
        is_bold = argv[4];

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

    int textureId = 0;

    CCM2Reader ccm2;

    std::vector<WCHAR> charList;
    std::ifstream inputFile("charlist.txt");

    if (!inputFile.is_open())
    {
        printf_s("charlist.txt does not exist\n");
        return 0;
    }

    int num;
    while (inputFile >> num)
        charList.push_back(num);

    inputFile.close();

    WCHAR start = 0;

    HDC memDC = CreateCompatibleDC(nullptr);

    int nDPI = GetDeviceCaps(memDC, LOGPIXELSY);

    int nHeight = -MulDiv(size, nDPI, 72);
    int cWeight = FW_NORMAL;

    if (bold)
        cWeight = FW_BOLD;

    HFONT hFont = CreateFontW(nHeight, 0, 0, 0, cWeight, italic, 0, 0, 1, 0, 0, 2, 2, StringHelper::ToWide(fontname).c_str());
    SelectObject(memDC, hFont);

    TEXTMETRICW tm;
    GetTextMetricsW(memDC, &tm);

    PrintTextMetrics(tm);

    while (start < charList.size())
    {
        char tex_name[255];
        sprintf_s(tex_name, "%s_%04d.bmp", filename.c_str(), textureId);

        printf_s("Write file %s\n", tex_name);

        if (!WriteGlyphsToBitmapCharList(memDC, &textureId, std::string("Out/" + std::string(filename) + "/" + std::string(tex_name)).c_str(), &layout_file, &ccm2, charList, &start))
        {
            DeleteDC(memDC);
            return false;
        }
    }

    printf_s("Generating CCM2 file %s\n", ccm_name);
    ccm2.CreateCCM2(ccm_out, tm.tmHeight, TEXTURE_SIZE, textureId);
    
    printf_s("Generated %d textures\n", textureId + 1);

    DeleteDC(memDC);
    return 1;
}