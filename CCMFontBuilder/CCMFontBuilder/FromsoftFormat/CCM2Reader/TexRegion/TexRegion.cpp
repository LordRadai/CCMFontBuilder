#include "TexRegion.h"
#include "../../../MemReader/MemReader.h"

TexRegion::TexRegion()
{
}

TexRegion::TexRegion(std::ifstream* pIn)
{
	MemReader::ReadWord(pIn, (WORD*)&this->m_x1);
	MemReader::ReadWord(pIn, (WORD*)&this->m_y1);
	MemReader::ReadWord(pIn, (WORD*)&this->m_x2);
	MemReader::ReadWord(pIn, (WORD*)&this->m_y2);
}

TexRegion::TexRegion(short x1, short y1, short x2, short y2)
{
	this->m_x1 = x1;
	this->m_y1 = y1;
	this->m_x2 = x2;
	this->m_y2 = y2;
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