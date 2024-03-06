#include <Windows.h>
#include <iostream>
#include <fstream>

#include "StringHelper/StringHelper.h"
#include "DebugOutput/DebugOutput.h"
#include "FromsoftFormat/CCM2Reader/CCM2Reader.h"

void CreateCharlistFromCCM2(PWSTR pCCMPath)
{
    CCM2Reader ccm2(pCCMPath);

    if (!ccm2.GetInitStatus())
    {
        printf_s("CCM file not found\n");
        return;
    }

    std::ofstream charList("charlist.txt");

    for (size_t i = 0; i < ccm2.GetGlyphCount(); i++)
    {
        int code = ccm2.GetGlyph(i)->m_code;

        charList << code << "\n";
    }

    charList.close();
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        Debug::Alert(Debug::LVL_ERROR, "CCMFontBuilder", "Wrong argument count %d. Usage: CreateChrListCCM2.exe <ccm_filepath>", argc);
        return 0;
    }

    std::string ccm_path = argv[1];

    wchar_t sample_ccm[50];
    swprintf_s(sample_ccm, StringHelper::ToWide(ccm_path).c_str());

    CreateCharlistFromCCM2(sample_ccm);
}