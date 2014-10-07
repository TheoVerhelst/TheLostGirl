#ifndef STATEIDENTIFIERS_H
#define STATEIDENTIFIERS_H

/// The various game states.
/// Every State class correspond to a State identifiers here.
enum class States
{
	None,///< No state.
	MainMenu,///<The MAin menu state, from where you can start the game
	Loading,///< The state where the level is loaded
	Intro,///< The intro state
	Game,///< The main gamepay state.
	Pause
};

#endif // STATEIDENTIFIERS_H