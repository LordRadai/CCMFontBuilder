#include "TexRegion.h"
#include "../../../MemReader/MemReader.h"

TexRegion::TexRegion()
{
}

TexRegion::~TexRegion()
{
}

bool TexRegion::WriteToFile(std::ofstream* pOut)
{
	MemReader::WriteWord(pOut, this->m_x1);
	MemReader::WriteWord(pOut, this->m_y1);
	MemReader::WriteWord(pOut, this->m_x2);
	MemReader::WriteWord(pOut, this->m_y2);

	return true;
}