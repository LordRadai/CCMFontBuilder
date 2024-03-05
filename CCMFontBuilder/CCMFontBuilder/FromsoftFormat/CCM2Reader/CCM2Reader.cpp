#include <filesystem>

#include "CCM2Reader.h"
#include "../../DebugOutput/DebugOutput.h"
#include "../../MemReader/MemReader.h"

Header::Header()
{
}

Header::Header(int texture_size, int texRegionCount, int glyphCount, int textureCount)
{
	this->m_format = 0x20000;
	this->m_fileSize = 0x20 + 8 * texRegionCount + 24 * glyphCount;
	this->m_sVar8 = 18;
	this->m_textureHeight = texture_size;
	this->m_textureWidth = texture_size;
	this->m_texRegionCount = texRegionCount;
	this->m_glyphCount = glyphCount;
	this->m_sVar12 = 0;
	this->m_iVar14 = 32;
	this->m_glyphOffset = 0x20 + 8 * texRegionCount;
	this->m_bVar1C = 4;
	this->m_bVar1D = 0;
	this->m_textureCount = textureCount;
	this->m_bVar1F = 0;
}

Header::~Header()
{
}

void Header::WriteToFile(std::ofstream* pOut)
{
	MemReader::WriteDWord(pOut, this->m_format);
	MemReader::WriteDWord(pOut, this->m_fileSize);
	MemReader::WriteWord(pOut, this->m_sVar8);
	MemReader::WriteWord(pOut, this->m_textureWidth);
	MemReader::WriteWord(pOut, this->m_textureHeight);
	MemReader::WriteWord(pOut, this->m_texRegionCount);
	MemReader::WriteWord(pOut, this->m_glyphCount);
	MemReader::WriteWord(pOut, this->m_sVar12);
	MemReader::WriteDWord(pOut, this->m_iVar14);
	MemReader::WriteDWord(pOut, this->m_glyphOffset);
	MemReader::WriteByte(pOut, this->m_bVar1C);
	MemReader::WriteByte(pOut, this->m_bVar1D);
	MemReader::WriteByte(pOut, this->m_textureCount);
	MemReader::WriteByte(pOut, this->m_bVar1F);
}

CCM2Reader::CCM2Reader()
{
}

CCM2Reader::CCM2Reader(PWSTR pwPath)
{
	this->m_init = true;
	this->m_filePath = pwPath;

	std::filesystem::path path(pwPath);
}

CCM2Reader::~CCM2Reader()
{
}

bool CCM2Reader::WriteFile(PWSTR pwOutPath)
{
	if (this->m_init == false)
		return false;

	std::filesystem::path path = pwOutPath;

	if (std::filesystem::exists(path))
	{
		std::filesystem::path bak_path = path;
		bak_path.replace_extension(L".ccm.bak");

		std::filesystem::copy_file(path, bak_path, std::filesystem::copy_options::overwrite_existing);
	}

	bool state = true;
	this->m_outFilePath = pwOutPath;
	
	ofstream ccm_out;
	ccm_out.open(this->m_outFilePath, ios::binary | ios::out);

	try
	{
		this->m_header.WriteToFile(&ccm_out);

		for (size_t i = 0; i < this->m_header.m_texRegionCount; i++)
			this->m_texRegions[i].WriteToFile(&ccm_out);

		for (size_t i = 0; i < this->m_header.m_glyphCount; i++)
			this->m_glyphs[i].WriteToFile(&ccm_out);
	}
	catch (const std::exception& e)
	{
		Debug::Panic("CCM2Reader.cpp", e.what());
	}

	ccm_out.close();
	
	return state;
}