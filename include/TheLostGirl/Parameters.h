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
const std::string paths[5] = {"ressources/images/360p/",
								"ressources/images/576p/",
								"ressources/images/720p/",
								"ressources/images/900p/",
								"ressources/images/1080p/"};
struct Parameters
{
	Lang lang;                  ///< The current language.
	bool debugMode;             ///< True if the DebugDrawing must be activated.
	unsigned char scaleIndex;   ///< The index of the current scale, in the range [0, 4]
	float scale;                ///< Current scale.
	float pixelScale;           ///< Pixels/meter scale.
	b2Vec2 gravity;             ///< The gravity vector.
	unsigned char numberOfPlans;///< Number of plans in the background.
	std::string textFont;       ///< Path of the main font.
};

#endif // PARAMETERS_H