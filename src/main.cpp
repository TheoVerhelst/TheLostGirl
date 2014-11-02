#include <TheLostGirl/Application.h>

/// \mainpage The Lost Girl code Documentation

int main()
{
	#ifdef DEBUG_MODE
	bool debugMode = true;
	#else
	bool debugMode = false;
	#endif
	Application application(debugMode);
	int returnValue = application.init();
	return returnValue == 0 ? application.run() : returnValue;
}