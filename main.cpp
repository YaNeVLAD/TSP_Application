#include "src/TSPApp.h"

int main()
{
	auto* app = new TSPApp();
	app->Run();
	delete app;

	return 0;
}
