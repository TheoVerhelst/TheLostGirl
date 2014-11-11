#ifndef EVENTS_H
#define EVENTS_H

#include <string>

#include <entityx/Event.h>

/// Structure used to emit and receive loading progression between differents game states.
struct LoadingStateChange : public entityx::Event<LoadingStateChange>
{
	///Default constructor
	/// \param _percent The progression of the loading, in percent.
	/// \param _sentence A short sentence describing the loading state.
	LoadingStateChange(short int _percent, std::wstring _sentence):
		percent(_percent),
		sentence(_sentence)
	{}
	
	short int percent;    ///< The progression of the loading, in percent.
	std::wstring sentence;///< A short sentence describing the loading state.
};

#endif // EVENTS_H