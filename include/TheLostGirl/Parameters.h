#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>
#include <Box2D/Common/b2Math.h>

const float scales[5] = {30.f/90.f,//62,5%
						48.f/90.f, //80%
						60.f/90.f, //80%
						75.f/90.f, //83,333%
						90.f/90.f};///<Differents scales, relatively to the maximum resolution.

const std::string paths[5] = {"resources/images/360p/",
								"resources/images/576p/",
								"resources/images/720p/",
								"resources/images/900p/",
								"resources/images/1080p/"};

/// Structure that hold various informations about the application.
struct Parameters
{
	/// Default constructor.
	Parameters();

	bool debugMode;           ///< True if the DebugDrawing must be activated.
	unsigned int scaleIndex;  ///< The index of the current scale, in the range [0, 5[
	float scale;              ///< Current scale of graphics and  images.
	float pixelByMeter;       ///< Original pixels/meter scale.
	float scaledPixelByMeter; ///< Scaled pixels/meter scale, equal to scale*pixelByMeter
	b2Vec2 gravity;           ///< The gravity vector.
	bool bloomEnabled;        ///< Indicates if the bloom effect is enabled.
	bool fullscreen;          ///< Indicates whether the window is in fullscreen mode.
	std::string guiConfigFile;///< Path to the configuration file of the GUI.
};

#endif//PARAMETERS_H
