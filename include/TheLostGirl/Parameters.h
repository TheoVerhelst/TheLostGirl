#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>
#include <Box2D/Common/b2Math.h>

/// Structure that hold various informations about the application.
struct Parameters
{
	/// Default constructor.
	Parameters();

	bool debugMode;           ///< True if the DebugDrawing must be activated.
	std::string imagePath;    ///< Path tho the images folder.
	float pixelByMeter;       ///< Original pixels/meter scale.
	b2Vec2 gravity;           ///< The gravity vector.
	bool bloomEnabled;        ///< Indicates if the bloom effect is enabled.
	bool fullscreen;          ///< Indicates whether the window is in fullscreen mode.
	std::string guiConfigFile;///< Path to the configuration file of the GUI.
};

#endif//PARAMETERS_H
