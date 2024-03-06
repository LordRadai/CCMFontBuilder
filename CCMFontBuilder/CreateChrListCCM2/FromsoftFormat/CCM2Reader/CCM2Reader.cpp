#include <filesystem>

#include "CCM2Reader.h"
#include "../../DebugOutput/DebugOutput.h"
#include "../../MemReader/MemReader.h"

Header::Header()
{
	this->m_format = 0x20000; //CCM2
	this->m_fileSize = 0;
	this->m_fontSize = 0;
	this->m_sVar12 = 0;
	this->m_iVar14 = 32;
	this->m_glyphOffset = 0;
	this->m_bVar1C = 4;
	this->m_bVar1D = 0;
	this->m_bVar1F = 0;

	this->CalculateOffsets();
}

Header::Header(std::ifstream* pIn)
{
	MemReader::ReadDWord(pIn, (DWORD*)&this->m_format);
	MemReader::ReadDWord(pIn, (DWORD*)&this->m_fileSize);
	MemReader::ReadWord(pIn, &this->m_fontSize);
	MemReader::ReadWord(pIn, &this->m_textureWidth);
	MemReader::ReadWord(pIn, &this->m_textureHeight);
	MemReader::ReadWord(pIn, &this->m_texRegionCount);
	MemReader::ReadWord(pIn, &this->m_glyphCount);
	MemReader::ReadWord(pIn, &this->m_sVar12);
	MemReader::ReadDWord(pIn, (DWORD*)&this->m_iVar14);
	MemReader::ReadDWord(pIn, (DWORD*)&this->m_glyphOffset);
	MemReader::ReadByte(pIn, &this->m_bVar1C);
	MemReader::ReadByte(pIn, &this->m_bVar1D);
	MemReader::ReadByte(pIn, &this->m_textureCount);
	MemReader::ReadByte(pIn, &this->m_bVar1F);
}

Header::Header(int font_size, int texture_size, int texRegionCount, int glyphCount, int textureCount)
{
	this->m_format = 0x20000;
	this->m_fileSize = 0;
	this->m_fontSize = font_size;
	this->m_textureHeight = texture_size;
	this->m_textureWidth = texture_size;
	this->m_texRegionCount = texRegionCount;
	this->m_glyphCount = glyphCount;
	this->m_sVar12 = 0;
	this->m_iVar14 = 32;
	this->m_glyphOffset = 0;
	this->m_bVar1C = 4;
	this->m_bVar1D = 0;
	this->m_textureCount = textureCount;
	this->m_bVar1F = 0;

	this->CalculateOffsets();
}

Header::~Header()
{
}

void Header::CalculateOffsets()
{
	this->m_fileSize = 32 + 8 * this->m_texRegionCount + 24 * this->m_glyphCount;
	this->m_glyphOffset = 32 + 8 * this->m_texRegionCount;
}

void Header::WriteToFile(std::ofstream* pOut)
{
	MemReader::WriteDWord(pOut, this->m_format);
	MemReader::WriteDWord(pOut, this->m_fileSize);
	MemReader::WriteWord(pOut, this->m_fontSize);
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

	std::ifstream pFile(pwPath, ios::binary);

	if (!pFile.is_open())
	{
		this->m_init = false;
		return;
	}

	this->m_header = Header(&pFile);

	this->m_texRegions.reserve(this->m_header.m_texRegionCount);

	for (size_t i = 0; i < this->m_header.m_texRegionCount; i++)
		this->m_texRegions.push_back(TexRegion(&pFile));

	for (size_t i = 0; i < this->m_header.m_glyphCount; i++)
		this->m_glyphs.push_back(Glyph(&pFile));

	this->m_fileSize = this->m_header.m_fileSize;
}

CCM2Reader::~CCM2Reader()
{
}

bool CCM2Reader::GetInitStatus()
{
	return this->m_init;
}

int CCM2Reader::GetTexRegionCount()
{
	return this->m_header.m_texRegionCount;
}

int CCM2Reader::GetGlyphCount()
{
	return this->m_header.m_glyphCount;
}

TexRegion* CCM2Reader::GetTexRegion(int idx)
{
	if (idx > this->m_header.m_texRegionCount)
		return nullptr;

	return &this->m_texRegions[idx];
}

Glyph* CCM2Reader::GetGlyph(int idx)
{
	if (idx > this->m_header.m_glyphCount)
		return nullptr;

	return &this->m_glyphs[idx];
}

void CCM2Reader::AddTexRegion(TexRegion texRegion)
{
	this->m_header.m_texRegionCount++;
	this->m_header.CalculateOffsets();

	this->m_texRegions.push_back(texRegion);
}

void CCM2Reader::AddGlyph(Glyph glyph)
{
	this->m_header.m_glyphCount++;
	this->m_header.CalculateOffsets();

	this->m_glyphs.push_back(glyph);
}

bool CCM2Reader::CreateCCM2(std::string pwPath, int font_size, int texture_size, int textureCount)
{
	this->m_init = true;
	this->m_header.m_fontSize = font_size;
	this->m_header.m_textureWidth = texture_size;
	this->m_header.m_textureHeight = texture_size;
	this->m_header.m_textureCount = textureCount;

	this->m_header.CalculateOffsets();

	return this->WriteFile(pwPath);
}

bool CCM2Reader::WriteFile(std::string pwOutPath)
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
	
	ofstream ccm_out;
	ccm_out.open(pwOutPath, ios::binary | ios::out);

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