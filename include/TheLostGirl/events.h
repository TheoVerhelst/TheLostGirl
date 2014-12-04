#ifndef EVENTS_H
#define EVENTS_H

#include <string>

#include <entityx/Event.h>

/// Structure used to emit and receive loading progression between differents game states.
struct LoadingStateChange : public entityx::Event<LoadingStateChange>
{
	///Default constructor
	/// \param _sentence A short sentence describing the loading state.
	LoadingStateChange(std::wstring _sentence):
		sentence(_sentence)
	{}
	
	std::wstring sentence;///< A short sentence describing the loading state.
};

/// Structure used to emit and receive player's health changes.
struct PlayerHealthChange : public entityx::Event<PlayerHealthChange>
{
	///Default constructor
	/// \param _newHealth The new health of the player
	PlayerHealthChange(float _newHealth):
		newHealth(_newHealth)
	{}
	
	float newHealth;///< The new health of the player
};

/// Structure used to emit and receive player's stamina changes.
struct PlayerStaminaChange : public entityx::Event<PlayerStaminaChange>
{
	///Default constructor
	/// \param _newStamina The new stamina of the player
	PlayerStaminaChange(float _newStamina):
		newStamina(_newStamina)
	{}
	
	float newStamina;///< The new stamina of the player
};

#endif//EVENTS_H
