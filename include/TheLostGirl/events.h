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
	/// \param _health The new health of the player.
	/// \param _normalizedHealth The new health of the player, normalized in [0, 1].
	PlayerHealthChange(float _health, float _normalizedHealth):
		health(_health),
		normalizedHealth(_normalizedHealth)
	{}
	
	float health;          ///< The new health of the player.
	float normalizedHealth;///< The new health of the player, normalized in [0, 1].
};

/// Structure used to emit and receive player's stamina changes.
struct PlayerStaminaChange : public entityx::Event<PlayerStaminaChange>
{
	///Default constructor
	/// \param _stamina The new stamina of the player.
	/// \param _normalizedStamina The new stamina of the player, normalized in [0, 1].
	PlayerStaminaChange(float _stamina, float _normalizedStamina):
		stamina(_stamina),
		normalizedStamina(_normalizedStamina)
	{}
	
	float stamina;          ///< The new stamina of the player.
	float normalizedStamina;///< The new stamina of the player, normalized in [0, 1].
};

#endif//EVENTS_H
