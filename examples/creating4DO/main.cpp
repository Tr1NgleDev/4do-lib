#include <cstdlib>
#include <4do/4do.h>

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
	#ifdef _WIN32
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT); // enable ansi colors in windows terminal
	#endif
	fdo::Logger::ansiColors = true;

	fdo::Logger::logToConsole = true;

	fdo::Object obj = fdo::Object();

	return 0;
}
