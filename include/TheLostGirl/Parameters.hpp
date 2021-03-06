#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

#include <string>
#include <Box2D/Box2D.h>
#include <TGUI/TGUI.hpp>

/// Structure that holds various informations about the application.
struct Parameters
{
	/// Default constructor.
	Parameters();

	bool debugMode;              ///< True if the DebugDrawing must be activated.
	std::string resourcesPath;   ///< Path tho the images folder.
	sf::Vector2f defaultViewSize;///< The size of a in-game screen, and the default size of the window.
	float pixelByMeter;          ///< Original pixels/meter scale.
	b2Vec2 gravity;              ///< The gravity vector.
	bool bloomEnabled;           ///< Indicates if the bloom effect is enabled.
	bool fullscreen;             ///< Indicates whether the window is in fullscreen mode.
	tgui::Theme::Ptr guiTheme;   ///< Path to the configuration file of the GUI.
};

#endif//PARAMETERS_HPP
