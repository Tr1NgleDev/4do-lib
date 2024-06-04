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

	fdo::Object obj = fdo::Object().
		pushVertex(0, 0, 0, 0).
		pushVertex(1, 0, 0, 0).
		pushVertex(0, 1, 0, 0).
		pushVertex(0, 0, 1, 0).
		pushColor(200, 200, 200).
		pushTetrahedron(0,1,2,3, 0).
		save4DOToFile("./test.4do");

	return 0;
}
