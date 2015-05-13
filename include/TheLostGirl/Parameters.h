#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>

#include <SFML/System/Vector2.hpp>
#include <Box2D/Common/b2Math.h>

#include <TheLostGirl/LangManager.h>

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
	Lang lang = EN;                                           ///< The current language.
	bool debugMode = false;                                   ///< True if the DebugDrawing must be activated.
	unsigned int scaleIndex = 0;                              ///< The index of the current scale, in the range [0, 5[
	float scale = scales[scaleIndex];                         ///< Current scale of graphics and  images.
	float pixelByMeter = 120.f;                               ///< Original pixels/meter scale.
	float scaledPixelByMeter = scale*pixelByMeter;            ///< Scaled pixels/meter scale, equal to scale*pixelByMeter
	b2Vec2 gravity = {0.0f, 9.80665f};                        ///< The gravity vector.
	std::string textFont = "resources/fonts/euphorigenic.ttf";///< Path of the main font.
	bool bloomEnabled = false;                                ///< Indicates if the bloom effect is enabled.
};

#endif//PARAMETERS_H
