#include "Console.h"

#include <fstream>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>

void CreateConsole()
{
	std::cout.sync_with_stdio(false);
	AllocConsole();
	HANDLE stdHandle;
	int hConsole;
	FILE* fp;
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsole = _open_osfhandle((long)stdHandle, _O_TEXT);
	fp = _fdopen(hConsole, "w");

	freopen_s(&fp, "CONOUT$", "w", stdout);
	std::cout << "Windows 10" << std::endl;
}

void DeleteConsole()
{
	std::cout.rdbuf();
	FreeConsole();
}
