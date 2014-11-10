#ifndef EVENTS_H
#define EVENTS_H

#include <string>

#include <entityx/Event.h>

struct LoadingStateChange : public entityx::Event<LoadingStateChange>
{
	LoadingStateChange(short int _percent, std::wstring _sentence):
		percent(_percent),
		sentence(_sentence)
	{}
	
	short int percent;
	std::wstring sentence;
};

#endif // EVENTS_H