#include "RCore.h"

RLog* g_log;

int main(int argc, char** argv)
{
    g_log = new RLog(MsgLevel_Debug, "Out\\", "EndianSwap.log");

    if (argc < 2)
    {
        g_log->debugMessage(MsgLevel_Error, "Usage: %s \"fontname\"\n", argv[0]);
        delete g_log;
        return 0;
    }

    const std::string fontname = argv[1];
    FontDataCCM2::FontDataCCM2* fontData = FontDataCCM2::FontDataCCM2::loadFile(RString::toWide(fontname.c_str()));
	fontData->save(RString::toWide((fontname + "_swapped").c_str()));

    delete g_log;
    return 1;
}