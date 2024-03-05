#include "Glyph.h"
#include "../../../MemReader/MemReader.h"

Glyph::Glyph()
{
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