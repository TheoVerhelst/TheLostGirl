#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>

#include <SFML/System/Vector2.hpp>
#include <Box2D/Common/b2Math.h>

#include <TheLostGirl/LangManager.h>

const float scales[5] = {30.f/90.f,
						48.f/90.f,
						60.f/90.f,
						75.f/90.f,
						90.f/90.f};///<Differents scales, relatively to the maximum resolution.
const std::string paths[5] = {"resources/images/360p/",
								"resources/images/576p/",
								"resources/images/720p/",
								"resources/images/900p/",
								"resources/images/1080p/"};

/// Structure that hold various informations about the application.
struct Parameters
{
	Lang lang;                    ///< The current language.
	bool debugMode;               ///< True if the DebugDrawing must be activated.
	unsigned short int scaleIndex;///< The index of the current scale, in the range [0, 5[
	float scale;                  ///< Current scale.
	float pixelByMeter;           ///< Original pixels/meter scale.
	float scaledPixelByMeter;     ///< Scaled pixels/meter scale, equal to scale*fullPixelScale
	b2Vec2 gravity;               ///< The gravity vector.
	std::string textFont;         ///< Path of the main font.
};

#endif//PARAMETERS_H
