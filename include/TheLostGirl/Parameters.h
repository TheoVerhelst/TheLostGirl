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
						90.f/90.f};///<Differents scales, relatively to to maximum resolution.
const sf::Vector2u windowSizes[5] = {{630, 360},
									{1024, 576},
									{1280, 720},
									{1600, 900},
									{1920, 1080}};///< All available window sizes

struct Parameters
{
	Lang lang;                  ///< The current language.
	bool debugMode;             ///< True if the DebugDrawing must be activated.
	float scale;                ///< Current scale.
	sf::Vector2u windowSize;    ///< The current size of the window.
	float pixelScale;           ///< Pixels/meter scale.
	sf::Vector2f worldFrameSize;///< Size of a screen, in meters.
	b2Vec2 gravity;             ///< The gravity vector.
	int numberOfPlans;          ///< Number of plans in the background.
	std::string textFont;       ///< Path of the main font.
};

#endif // PARAMETERS_H