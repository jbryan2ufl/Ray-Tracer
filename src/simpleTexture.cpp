#include "application.h"

int main()
{
	application* app{new application()};
	callback=app;
	app->loop();
	app->close();
	delete app;
}