#include <iostream>
#include "RCore.h"

void CCM2Test()
{
	DLFontDataCCM2* fontData = DLFontDataCCM2::loadFile(L"Resource\\test.ccm");

	if (fontData == nullptr || !fontData->getInitStatus())
		printf_s("Failed to load ccm file");
	else
		printf_s("Load test SUCCESS\n");

	if (!fontData->save(L"Out\\Test\\test_gen.ccm"))
		printf_s("Failed to generate ccm file\n");
	else
		printf_s("Write test SUCCESS\n");

	fontData->destroy();
}

int main(char* argv, int argc)
{
#ifdef _DEBUG
	printf_s("---------CCM2 Test---------\n");
	CCM2Test();
	printf_s("---------------------------\n");
#endif
}