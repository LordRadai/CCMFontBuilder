#include <iostream>
#include <filesystem>
#include "RCore.h"

RLog* g_log;

void dumpCharset(DLFontData::DLFontDataCCM2* pFontData, std::filesystem::path out)
{
	if (pFontData == nullptr)
		return;

	if (out.parent_path() != L"")
		std::filesystem::create_directories(out.parent_path());

	std::ofstream file;
	file.open(out, std::ios::out);

	for (size_t i = 0; i < pFontData->getNumGlyphs(); i++)
	{
		const DLFontData::Glyph* glyph = pFontData->getGlyph(i);

		if (glyph == nullptr)
			continue;

		char line[256];
		sprintf_s(line, "%d", glyph->getCode());

		file << line << "\n";
	}

	file.close();
}

int main(int argc, char** argv)
{
	g_log = new RLog(MsgLevel_Debug, "Out\\", "GetCharListFromCCM.log");

	if (argc < 2)
	{
		g_log->alertMessage(MsgLevel_Error, "Usage: %s \"file\"\n", argv[0]);
		delete g_log;
		return 1;
	}

	std::wstring ccmFilePath = RString::toWide(argv[1]);

	DLFontData::DLFontDataCCM2* fontData = DLFontData::DLFontDataCCM2::loadFile(ccmFilePath);
	dumpCharset(fontData, L"charlist.txt");

	return 0;
}