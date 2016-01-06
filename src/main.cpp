#if defined(__linux__)
	#include <X11/Xlib.h>//Thread init
	#undef None//The X11 library define None as 0 so undef it in order to don't conflict with some None in enums
	#undef Status//Same as None above
#endif//defined(__linux__)

#include <TheLostGirl/Application.hpp>

int main()
{
#if defined(__linux__)
	XInitThreads();//For multithreadings on linux, must be called before the initialisation of the StateStack
#endif//defined(__linux__)

#ifdef NDEBUG
	Application(false).run();
#else
	Application(true).run();
#endif
}
