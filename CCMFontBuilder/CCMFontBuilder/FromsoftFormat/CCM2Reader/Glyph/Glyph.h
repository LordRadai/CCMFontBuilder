#pragma once
#include <fstream>
#include <Windows.h>

class Glyph
{
public:
	int m_code;
	int m_texRegionOffset;
	short m_textureIndex;
	short m_preSpace;
	short m_width;
	short m_advance;
	int m_iVar10;
	int m_iVar14;

	Glyph();
	Glyph(WCHAR code, int idx, short textureIdx, short preSpace, short width, short advance);
	~Glyph();

	bool WriteToFile(std::ofstream* pOut);
};