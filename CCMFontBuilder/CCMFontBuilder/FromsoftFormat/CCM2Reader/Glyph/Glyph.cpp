#include "Glyph.h"
#include "../../../MemReader/MemReader.h"

Glyph::Glyph()
{
}

Glyph::Glyph(WCHAR code, int idx, short textureIdx, short preSpace, short width, short advance)
{
	this->m_code = code;
	this->m_texRegionOffset = 0x20 + 8 * idx;
	this->m_textureIndex = textureIdx;
	this->m_preSpace = preSpace;
	this->m_width = width;
	this->m_advance = advance;
	this->m_iVar10 = 0;
	this->m_iVar14 = 0;
}

Glyph::~Glyph()
{
}

bool Glyph::WriteToFile(std::ofstream* pOut)
{
	MemReader::WriteDWord(pOut, this->m_code);
	MemReader::WriteDWord(pOut, this->m_texRegionOffset);
	MemReader::WriteWord(pOut, this->m_textureIndex);
	MemReader::WriteWord(pOut, this->m_preSpace);
	MemReader::WriteWord(pOut, this->m_width);
	MemReader::WriteWord(pOut, this->m_advance);
	MemReader::WriteDWord(pOut, this->m_iVar10);
	MemReader::WriteDWord(pOut, this->m_iVar14);

	return true;
}