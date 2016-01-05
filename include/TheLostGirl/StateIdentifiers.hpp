#ifndef STATEIDENTIFIERS_H
#define STATEIDENTIFIERS_H

/// The various game states.
/// Every State class correspond to a State identifiers here.
enum class States
{
	None,     ///< No state.
	MainMenu, ///< The main menu state, from where the player can start the game.
	Loading,  ///< The state where the level is loaded.
	Intro,    ///< The intro state.
	Game,     ///< The main gamepay state.
	Pause,    ///< The pause state, on top of the Game state.
	HUD,      ///< The HUD state, on top of the Game state.
	EmptyLevel///< The HUD state, on top of the Game state.
};

#endif//STATEIDENTIFIERS_H
