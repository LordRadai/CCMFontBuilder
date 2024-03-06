#pragma once
#include <Windows.h>
#include <string>
#include "TexRegion/TexRegion.h"
#include "Glyph/Glyph.h"

class Header
{
public:
	UINT m_format;
	UINT m_fileSize;
	USHORT m_fontSize;
	USHORT m_textureWidth;
	USHORT m_textureHeight;
	USHORT m_texRegionCount;
	USHORT m_glyphCount;
	USHORT m_sVar12;
	UINT m_iVar14;
	UINT m_glyphOffset;
	BYTE m_bVar1C;
	BYTE m_bVar1D;
	BYTE m_textureCount;
	BYTE m_bVar1F;

	Header();
	Header(std::ifstream* pIn);
	Header(int font_size, int texture_size, int texRegionCount, int glyphCount, int textureCount);
	~Header();

	void CalculateOffsets();
	void WriteToFile(std::ofstream* pOut);
};

class CCM2Reader
{
private:
	std::wstring m_fileName;

	PWSTR m_filePath;
	UINT64 m_fileSize;
	PWSTR m_outFilePath;
	UINT64 m_outFileSize;
	bool m_init = false;

	Header m_header;
	std::vector<TexRegion> m_texRegions;
	std::vector<Glyph> m_glyphs;

public:
	CCM2Reader();
	CCM2Reader(PWSTR pwPath);
	~CCM2Reader();

	int GetTexRegionCount();
	int GetGlyphCount();
	void AddTexRegion(TexRegion texRegion);
	void AddGlyph(Glyph glyph);
	bool CreateCCM2(std::string pwPath, int font_size, int texture_size, int textureCount);
	bool WriteFile(std::string pwPath);
};