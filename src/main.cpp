#include <X11/Xlib.h>//Thread init
#undef None//The X11 library define None as 0 so undef it in order to don't conflict with some None in enums
#undef Status
#include <TheLostGirl/Application.h>

int main()
{
	XInitThreads();//For multithreadings on linux, must be called before the initialisation of the StateStack
	try
	{
#ifdef NDEBUG
		Application(false).run();
#else
		Application(true).run();
#endif
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
