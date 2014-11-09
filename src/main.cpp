#include <X11/X.h>//Thread init
#include <X11/Xlib.h>//Thread init
//The X11 library define None as 0
//so undef it in order to don't conflict with some None in enums
#undef None
	
#include <TheLostGirl/Application.h>

/// \mainpage The Lost Girl programmer's Documentation

int main()
{
	//For multithreadings on linux, must be called before the initialisation
	//of the StateStack, so before the Application initialization
	XInitThreads();
	#ifdef DEBUG_MODE
	bool debugMode = true;
	#else
	bool debugMode = false;
	#endif
	Application application(debugMode);
	int returnValue = application.init();
	return returnValue == 0 ? application.run() : returnValue;
}