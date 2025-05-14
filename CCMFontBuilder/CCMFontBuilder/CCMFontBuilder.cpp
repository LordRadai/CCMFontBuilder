#include <iostream>
#include "RCore.h"

RLog* g_log;

void CCM2Test()
{
	DLFontDataCCM2* fontData = DLFontDataCCM2::loadFile(L"Resource\\test.ccm");

	if (fontData == nullptr || !fontData->getInitStatus())
		g_log->debugMessage(MsgLevel_Info, "Failed to load test ccm file");
	else
		g_log->debugMessage(MsgLevel_Info, "Load test SUCCESS\n");

	if (!fontData->save(L"Out\\Test\\test_gen.ccm"))
		g_log->debugMessage(MsgLevel_Info, "Failed to generate test ccm file\n");
	else
		g_log->debugMessage(MsgLevel_Info, "Write test SUCCESS\n");

	fontData->destroy();
}

int main(int argc, char** argv)
{
	g_log = new RLog(MsgLevel_Debug, "Out\\", "CCMFontBuilder.log");

#ifdef _DEBUG
	printf_s("---------CCM2 Test---------\n");
	CCM2Test();
	printf_s("---------------------------\n");
#endif

	delete g_log;
}