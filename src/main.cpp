#include <X11/Xlib.h>//Thread init
//The X11 library define None as 0 so undef it in order to don't conflict with some None in enums
#undef None
#include <TheLostGirl/Application.h>

int main()
{
	//For multithreadings on linux, must be called before the initialisation of the StateStack
	XInitThreads();
	#ifndef DEBUG_MODE
		#define DEBUG_MODE 0
	#endif
	Application application(DEBUG_MODE);
	return application.init() or application.run();
}
