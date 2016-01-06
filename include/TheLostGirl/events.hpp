#ifndef EVENTS_H
#define EVENTS_H

#include <string>
#include <entityx/entityx.h>
#include <TheLostGirl/LangManager.hpp>

/// Structure used to emit and receive loading progression between differents game states.
struct LoadingStateChange : public entityx::Event<LoadingStateChange>
{
	/// Default constructor.
	LoadingStateChange()
	{
	}

	/// Constructor.
	/// \param _sentence A short sentence describing the loading state.
	LoadingStateChange(const std::string& _sentence):
		sentence{_sentence}
	{
	}

	std::string sentence;///< A short sentence describing the loading state.
};

/// Structure used to emit and receive entity health changes.
struct EntityHealthChange : public entityx::Event<EntityHealthChange>
{
	/// Default constructor.
	EntityHealthChange()
	{
	}


	/// Constructor.
	/// \param _entity The entity touched by the health change.
	/// \param _health The new health of the entity.
	/// \param _normalizedHealth The new health of the entity, normalized in [0, 1]
	EntityHealthChange(entityx::Entity _entity, float _health, float _normalizedHealth):
		entity{_entity},
		health{_health},
		normalizedHealth{_normalizedHealth}
	{
	}

	entityx::Entity entity;///< The entity touched by the health change.
	float health;          ///< The new health of the entity.
	float normalizedHealth;///< The new health of the entity, normalized in [0, 1].
};

/// Structure used to emit and receive entity stamina changes.
struct EntityStaminaChange : public entityx::Event<EntityStaminaChange>
{
	/// Default constructor.
	EntityStaminaChange()
	{
	}


	/// Constructor.
	/// \param _entity The entity touched by the stamina change.
	/// \param _stamina The new stamina of the player.
	/// \param _normalizedStamina The new stamina of the player, normalized in [0, 1].
	EntityStaminaChange(entityx::Entity _entity, float _stamina, float _normalizedStamina):
		entity{_entity},
		stamina{_stamina},
		normalizedStamina{_normalizedStamina}
	{
	}
	entityx::Entity entity; ///< The entity touched by the stamina change.
	float stamina;          ///< The new stamina of the player.
	float normalizedStamina;///< The new stamina of the player, normalized in [0, 1].
};

/// Give informations about a change in the game settings.
struct ParametersChange : public entityx::Event<ParametersChange>
{
	/// Default constructor.
	ParametersChange()
	{
	}

	/// Constructor.
	/// \param _langChanged Indicates whether the lang has been changed.
	/// \param _newLang The identifier of the new lang.
	/// \param _bloomEnabledChanged Indicates whether the bloom option has been toggled.
	/// \param _newBloomEnabledState Indicates if the bloom is now enabled or not.
	/// \param _fullscreenChanged Indicates wheter the fullscreen state has been toggled.
	/// \param _newFullScreenState Indicates whether the fullscreen is now enabled or not.
	/// \param _videoModeIndex The index of the video mode, intented to be used with sf::VideoMode::getFullscreenModes().
	ParametersChange(bool _langChanged, const std::string& _newLang, bool _bloomEnabledChanged, bool _newBloomEnabledState,
			bool _fullscreenChanged, bool _newFullScreenState, std::size_t _videoModeIndex):
		langChanged{_langChanged},
		newLang{_newLang},
		bloomEnabledChanged{_bloomEnabledChanged},
		newBloomEnabledState{_newBloomEnabledState},
		fullscreenChanged{_fullscreenChanged},
		newFullScreenState{_newFullScreenState},
		videoModeIndex{_videoModeIndex}
	{
	}

	bool langChanged;          ///< Indicates whether the lang has been changed.
	std::string newLang;       ///< The identifier of the new lang.
	bool bloomEnabledChanged;  ///< Indicates whether the bloom option has been toggled.
	bool newBloomEnabledState; ///< Indicates if the bloom is now enabled or not.
	bool fullscreenChanged;    ///< Indicates wheter the fullscreen state has been toggled.
	bool newFullScreenState;   ///< Indicates whether the fullscreen is now enabled or not.
	std::size_t videoModeIndex;///< The index of the video mode, intented to be used with sf::VideoMode::getFullscreenModes().
};

#endif//EVENTS_H
