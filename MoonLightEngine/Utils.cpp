#include "Utils.h"

const wchar_t* UTIL::charToWchar_t(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wc, cSize, c, _TRUNCATE);

	return wc;
}