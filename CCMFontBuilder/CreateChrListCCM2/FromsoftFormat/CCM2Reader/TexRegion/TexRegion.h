#pragma once
#include <fstream>

class TexRegion
{
public:
	short m_x1;
	short m_y1;
	short m_x2;
	short m_y2;

	TexRegion();
	TexRegion(std::ifstream* pIn);
	TexRegion(short x1, short y1, short x2, short y2);
	~TexRegion();

	bool WriteToFile(std::ofstream* pOut);
};