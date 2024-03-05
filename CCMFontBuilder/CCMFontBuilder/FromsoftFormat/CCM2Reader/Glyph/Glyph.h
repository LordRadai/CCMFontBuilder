#pragma once
#include <fstream>

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
	~Glyph();

	bool SaveToFile(std::ofstream* pOut);
};