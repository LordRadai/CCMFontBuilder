#include "RCore.h"

RLog* g_log;

int main(int argc, char** argv)
{
    g_log = new RLog(MsgLevel_Debug, "Out\\", "EndianSwap.log");

    if (argc < 3)
    {
        g_log->debugMessage(MsgLevel_Error, "Usage: %s \"fontname\" <ccmVersion>\n", argv[0]);
        delete g_log;
        return 0;
    }

    const std::string fontname = argv[1];
    const int ccmVersion = std::stoi(argv[2]);

    if (ccmVersion != 1 && ccmVersion != 2)
    {
        g_log->debugMessage(MsgLevel_Error, "Error: <ccmVersion> must be either \"1\" (ccm) or \"2\" (ccm2)\n");
        delete g_log;
        return 0;
	}

    if (ccmVersion == 1)   
    {
        FontDataCCM::FontDataCCM* fontData = FontDataCCM::FontDataCCM::loadFile(RString::toWide(fontname.c_str()));
        fontData->save(RString::toWide((fontname + "_swapped").c_str()));
    }
    else if (ccmVersion == 2)
    {
        FontDataCCM2::FontDataCCM2* fontData = FontDataCCM2::FontDataCCM2::loadFile(RString::toWide(fontname.c_str()));
        fontData->save(RString::toWide((fontname + "_swapped").c_str()));
    }

    delete g_log;
    return 1;
}