#include <X11/Xlib.h>//Thread init
#undef None//The X11 library define None as 0 so undef it in order to don't conflict with some None in enums
#undef Status
#include <TheLostGirl/Application.h>

int main()
{
	XInitThreads();//For multithreadings on linux, must be called before the initialisation of the StateStack
	Application application;
#ifdef NDEBUG
	return application.init(false) or application.run();
#else
	return application.init(true) or application.run();
#endif
}
