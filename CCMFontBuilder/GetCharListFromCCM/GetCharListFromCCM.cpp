#include <iostream>
#include "RCore.h"

RLog* g_log;

int main(int argc, char** argv)
{
	g_log = new RLog(MsgLevel_Debug, "Out\\", "GetCharListFromCCM.log");

	if (argc < 2)
	{
		g_log->alertMessage(MsgLevel_Error, "Usage: %s --<file>\n", argv[0]);
		delete g_log;
		return 1;
	}

	std::wstring ccmFilePath = RCLIArgParser::parseArgW(argv[1]);

	return 0;
}