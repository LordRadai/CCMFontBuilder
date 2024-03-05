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
	~TexRegion();

	bool SaveToFile(std::ofstream* pOut);
};