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

	fdo::Object obj = fdo::Object::load4DOFromFile("./example.4do");

	printf("Is valid?: %s\n", !obj.isInvalid() ? "true" : "false");
	printf("Spec Ver: %d\n", obj.specVer);
	printf("Orientation: %s\n", obj.orientation.toString().c_str());

	if(obj.isInvalid()) return 1;

	int i = 0;
	for(auto& v : obj.vertices)
	{
		printf("Vertex %d: %s\n", i++, v.toString().c_str());
	}
	printf("\n");
	i = 0;
	for(auto& n : obj.normals)
	{
		printf("Normal %d: %s\n", i++, n.toString().c_str());
	}
	printf("\n");
	i = 0;
	for(auto& c : obj.colors)
	{
		printf("Color %d: %s\n", i++, c.toString().c_str());
	}
	printf("\n");
	i = 0;
	for(auto& t : obj.texCoords)
	{
		printf("TexCoord %d: %s\n", i++, t.toString().c_str());
	}
	printf("\n");
	i = 0;
	for(auto& t : obj.tetrahedra)
	{
		printf("\nTetrahedron %d:", i++);
		printf("\n\tvIndices:");
		for(auto& v : t.vIndices)
			printf(" %d", v);
		printf("\n\tvnIndices:");
		for(auto& v : t.vnIndices)
			printf(" %d", v);
		printf("\n\tvtIndices:");
		for(auto& v : t.vtIndices)
			printf(" %d", v);
		printf("\n\tcoIndices:");
		for(auto& v : t.coIndices)
			printf(" %d", v);
	}
	printf("\n");
	i = 0;
	for(auto& t : obj.cells)
	{
		printf("\nCell %d:", i++);
		printf("\n\ttIndices:");
		for(auto& v : t.tIndices)
			printf(" %d", v);
	}
	printf("\n");
	i = 0;
	for(auto& t : obj.polylines)
	{
		printf("\nPolyline %d:", i++);
		printf("\n\tvIndices:");
		for(auto& v : t.vIndices)
			printf(" %d", v);
		printf("\n\tvnIndices:");
		for(auto& v : t.vnIndices)
			printf(" %d", v);
		printf("\n\tvtIndices:");
		for(auto& v : t.vtIndices)
			printf(" %d", v);
		printf("\n\tcoIndices:");
		for(auto& v : t.coIndices)
			printf(" %d", v);
	}

	return 0;
}
