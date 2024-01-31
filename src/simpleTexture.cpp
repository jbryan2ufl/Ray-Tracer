#include "application.h"

int main()
{
	application app{};
	app.init();
	app.loop();
	app.close();
}