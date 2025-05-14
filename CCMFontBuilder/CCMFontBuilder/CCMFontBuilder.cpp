#include <Windows.h>
#include <iostream>
#include <wingdi.h>
#include <fstream>
#include <filesystem>
#include <DirectXTex.h>
#include <wincodec.h>
#include <filesystem>
#include "RCore.h"

RLog* g_log;

#define TEXTURE_SIZE 1024
#define START_CHAR 0x20
#define END_CHAR 0xFFFF

void PrintTextMetrics(TEXTMETRICW tm)
{
    g_log->debugMessage(MsgLevel_Debug, "Text Metrics:\n");
    g_log->debugMessage(MsgLevel_Debug, "\tHeight of character: %d\n", tm.tmHeight);
    g_log->debugMessage(MsgLevel_Debug, "\tAverage width of character: %d\n", tm.tmAveCharWidth);
    g_log->debugMessage(MsgLevel_Debug, "\tMaximum width of character: %d\n", tm.tmMaxCharWidth);
    g_log->debugMessage(MsgLevel_Debug, "\tHeight of font: %d\n", tm.tmHeight);
    g_log->debugMessage(MsgLevel_Debug, "\tAscent: %d\n", tm.tmAscent);
    g_log->debugMessage(MsgLevel_Debug, "\tDescent: %d\n", tm.tmDescent);
    g_log->debugMessage(MsgLevel_Debug, "\tInternal leading: %d\n", tm.tmInternalLeading);
    g_log->debugMessage(MsgLevel_Debug, "\tExternal leading: %d\n\n", tm.tmExternalLeading);
}

void PrintGlyphMetrics(GLYPHMETRICS gm, WCHAR unicodeChar)
{
    g_log->debugMessage(MsgLevel_Debug, "Glyph metrics for Unicode character U+%04X:\n", unicodeChar);
    g_log->debugMessage(MsgLevel_Debug, "\tWidth: %ld\n", gm.gmBlackBoxX);
    g_log->debugMessage(MsgLevel_Debug, "\tHeight: %ld\n", gm.gmBlackBoxY);
    g_log->debugMessage(MsgLevel_Debug, "\tLeft side bearing: %ld\n", gm.gmptGlyphOrigin.x);
    g_log->debugMessage(MsgLevel_Debug, "\tTop side bearing: %ld\n\n", gm.gmptGlyphOrigin.y);
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

bool WriteGlyphsToBitmapCharList(HDC memDC, int* textureIdx, int kerning, const char* filename, std::ofstream* layoutFile, DLFontDataCCM2* pCCM, std::vector<WCHAR> charList, WCHAR* charIdx)
{
	pCCM->setNumTextures(*textureIdx + 1);

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
        g_log->debugMessage(MsgLevel_Debug, "Error creating DIB section\n");
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

        int prespace = 0;
        int width = size.cx;
        int advance = width + kerning;

        pCCM->addGlyph(Glyph::create(unicodeChar, pCCM->getNumGlyphs(), *textureIdx, prespace, width, advance, TexRegion::create(x, y, x + size.cx, y + size.cy)));

        char buf[256];
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
        g_log->debugMessage(MsgLevel_Debug, "Error creating file\n");
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

bool WriteGlyphsToBitmap(int* textureIdx, int kerning, const char* filename, std::ofstream* layoutFile, DLFontDataCCM2* pCCM, WCHAR* startChar, WCHAR* endChar, const char* fontname, int size, bool is_bold, bool is_italic)
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
        g_log->debugMessage(MsgLevel_Debug, "Error creating DIB section\n");
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

        int prespace = 0;
        int width = size.cx;
        int advance = width + kerning;

        pCCM->addGlyph(Glyph::create(unicodeChar, pCCM->getNumGlyphs(), *textureIdx, prespace, width, advance, TexRegion::create(x, y, x + size.cx, y + size.cy)));

        char buf[256];
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
        g_log->debugMessage(MsgLevel_Debug, "Error creating file\n");
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

bool GenerateBitmapSubset(WCHAR startChar, WCHAR endChar, int* textureIdx, int kerning, const char* filename, std::ofstream* layoutFile, DLFontDataCCM2* pCCM, const char* fontname, int size, bool is_bold, bool is_italic)
{
    while (startChar < endChar)
    {
        char tex_name[255];
        sprintf_s(tex_name, "%s_%04d.png", filename, *textureIdx);

        g_log->debugMessage(MsgLevel_Info, "Write file %s (startChar=%d, endChar=%d)\n", tex_name, startChar, endChar);

        if (!WriteGlyphsToBitmap(textureIdx, kerning, std::string("Out/" + std::string(filename) + "/" + std::string(tex_name)).c_str(), layoutFile, pCCM, &startChar, &endChar, fontname, size, is_bold, is_italic))
            return false;
    }

    return true;
}

void CCM2Test()
{
	DLFontDataCCM2* fontData = DLFontDataCCM2::loadFile(L"Resource\\test.ccm");

	if (fontData == nullptr || !fontData->getInitStatus())
		g_log->debugMessage(MsgLevel_Info, "Failed to load test ccm file");
	else
		g_log->debugMessage(MsgLevel_Info, "Load test SUCCESS\n");

	if (!fontData->save(L"Out\\Test\\test_gen.ccm"))
		g_log->debugMessage(MsgLevel_Info, "Failed to generate test ccm file\n");
	else
		g_log->debugMessage(MsgLevel_Info, "Write test SUCCESS\n");

	fontData->destroy();
}

int main(int argc, char** argv)
{
	g_log = new RLog(MsgLevel_Debug, "Out\\", "CCMFontBuilder.log");

#ifdef _DEBUG
	g_log->debugMessage(MsgLevel_Debug, "---------CCM2 Test---------\n");
	CCM2Test();
	g_log->debugMessage(MsgLevel_Debug, "---------------------------\n");
#endif

    if (argc < 3)
    {
		g_log->debugMessage(MsgLevel_Error, "Usage: %s \"fontname\" \"size\" [is_bold] [is_italic] [kerning]\n", argv[0]);
        delete g_log;
        return 0;
    }

    const std::string fontname = argv[1];
    const int fontSize = std::stod(argv[2]);

    std::string is_bold = "false";
    std::string is_italic = "false";
    std::string kerningStr = "0";

    if (argc > 3)
        is_bold = argv[3];

    if (argc > 4)
        is_bold = argv[4];

    if (argc > 5)
        kerningStr = argv[5];

    bool bold = false;
    bool italic = false;
	const int kerning = std::stod(kerningStr);

    if (is_bold.compare("true") == 0)
        bold = true;

    if (is_italic.compare("true") == 0)
        italic = true;

    std::string filename = std::filesystem::path(fontname).filename().string();
    std::string outpath = "Out\\" + filename + "\\";

    std::filesystem::create_directories(outpath);

    char layout_name[256];
    sprintf_s(layout_name, "%s.txt", filename.c_str());
    std::ofstream layout_file(std::string("Out\\" + filename + "\\" + std::string(layout_name)).c_str());

    char ccm_name[256];
    sprintf_s(ccm_name, "%s.ccm", filename.c_str());
    std::string ccm_out = outpath + ccm_name;

    int textureId = 0;

    std::vector<WCHAR> charList;
    std::ifstream inputFile("Resource\\charlist.txt");

    if (!inputFile.is_open())
    {
        g_log->debugMessage(MsgLevel_Error, "charlist.txt does not exist\n");
        return 0;
    }

    int num;
    while (inputFile >> num)
        charList.push_back(num);

    inputFile.close();

    WCHAR start = 0;

    HDC memDC = CreateCompatibleDC(nullptr);
    int nDPI = GetDeviceCaps(memDC, LOGPIXELSY);

    int nHeight = -MulDiv(fontSize, nDPI, 72);
    int cWeight = FW_NORMAL;

    if (bold)
        cWeight = FW_BOLD;

    HFONT hFont = CreateFontW(nHeight, 0, 0, 0, cWeight, italic, 0, 0, 1, 0, 0, 2, 2, RString::toWide(fontname).c_str());
    SelectObject(memDC, hFont);

    TEXTMETRICW tm;
    GetTextMetricsW(memDC, &tm);

    PrintTextMetrics(tm);

    g_log->debugMessage(MsgLevel_Info, "Generating CCM2 file %s\n", ccm_name);

    DLFontDataCCM2* pCCM2 = DLFontDataCCM2::create(tm.tmHeight, TEXTURE_SIZE);

    while (start < charList.size())
    {
        char tex_name[255];
        sprintf_s(tex_name, "%s_%04d.bmp", filename.c_str(), textureId);

        g_log->debugMessage(MsgLevel_Info, "Write file %s\n", tex_name);

        if (!WriteGlyphsToBitmapCharList(memDC, &textureId, kerning, std::string("Out\\" + std::string(filename) + "\\" + std::string(tex_name)).c_str(), &layout_file, pCCM2, charList, &start))
        {
            DeleteDC(memDC);
            return false;
        }
    }

	pCCM2->save(RString::toWide(ccm_out));

    g_log->debugMessage(MsgLevel_Info, "Generated %d textures\n", textureId + 1);

    DeleteDC(memDC);

    delete g_log;
    return 1;
}