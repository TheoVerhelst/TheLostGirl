#include <TheLostGirl/Application.h>
int main()
{
	Application application;
	int returnValue = application.init();
	return (returnValue == 0 ? application.run() : returnValue);
}