#pragma once
#include <Windows.h>
#include <string>

class Header
{
public:
	UINT m_format;
	UINT m_fileSize;
	short m_sVar8;
	short m_textureWidth;
	short m_textureHeight;
	short m_texRegionCount;
	short m_glyphCount;
	short m_sVar12;
	int m_iVar14;
	UINT m_glyphOffset;
	BYTE m_bVar1C;
	BYTE m_bVar1D;
	BYTE m_textureCount;
	BYTE m_bVar1F;

	Header();
	~Header();
};

class CCM2Reader
{
public:
	std::wstring m_fileName;

	PWSTR m_filePath;
	UINT64 m_fileSize;
	PWSTR m_outFilePath;
	UINT64 m_outFileSize;
	bool m_init = false;

	CCM2Reader();
	CCM2Reader(PWSTR pwOutPath);
	~CCM2Reader();
};