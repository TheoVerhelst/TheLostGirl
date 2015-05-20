#ifndef EVENTS_H
#define EVENTS_H

#include <string>

#include <entityx/Event.h>
#include <entityx/Entity.h>

#include <TheLostGirl/LangManager.h>

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

/// Structure used to emit and receive entity health changes.
struct EntityHealthChange : public entityx::Event<EntityHealthChange>
{
	/// Constructor.
	/// \param _entity The entity touched by the health change.
	/// \param _health The new health of the entity.
	/// \param _normalizedHealth The new health of the entity, normalized in [0, 1].
	EntityHealthChange(entityx::Entity _entity, float _health, float _normalizedHealth):
		entity(_entity),
		health(_health),
		normalizedHealth(_normalizedHealth)
	{}

	entityx::Entity entity;///< The entity touched by the health change.
	float health;          ///< The new health of the entity.
	float normalizedHealth;///< The new health of the entity, normalized in [0, 1].
};

/// Structure used to emit and receive entity stamina changes.
struct EntityStaminaChange : public entityx::Event<EntityStaminaChange>
{
	/// Constructor.
	/// \param _entity The entity touched by the stamina change.
	/// \param _stamina The new stamina of the entity.
	/// \param _normalizedStamina The new stamina of the entity, normalized in [0, 1].
	EntityStaminaChange(entityx::Entity _entity, float _stamina, float _normalizedStamina):
		entity(_entity),
		stamina(_stamina),
		normalizedStamina(_normalizedStamina)
	{}

	entityx::Entity entity; ///< The entity touched by the health change.
	float stamina;          ///< The new stamina of the player.
	float normalizedStamina;///< The new stamina of the player, normalized in [0, 1].
};

/// Give informations about a change in the game settings.
struct ParametersChange : public entityx::Event<ParametersChange>
{
	/// Constructor.
	ParametersChange(bool _langChanged, bool _bloomEnabledChanged, bool _resolutionChanged):
		langChanged{_langChanged},
		bloomEnabledChanged{_bloomEnabledChanged},
		resolutionChanged{_resolutionChanged}
	{}

	bool langChanged;        ///< Indicates whether the lang has been changed.
	bool bloomEnabledChanged;///< Indicates whether the bloom option has been toggled.
	bool resolutionChanged;  ///< Indicates whether the resolution has been changed.
};

#endif//EVENTS_H
