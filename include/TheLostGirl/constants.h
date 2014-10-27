#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

#include <SFML/System/Vector2.hpp>

//Forward declaration

/// \file constants.h
const float scales[5] = {30.f/90.f, 48.f/90.f, 60.f/90.f, 75.f/90.f, 90.f/90.f};///<Differents scales, relatively to to maximum resolution.
const float scale = scales[1];                                   ///< Current scale.
const sf::Vector2f windowSize{1920*scale, 1080*scale};           ///< The size of the window.
const float pixelScale{120.f*scale};                             ///< Pixel/meter scale.
const sf::Vector2f worldFrameSize{windowSize/pixelScale};        ///< Size in physics measures of a screen.
const float g = -9.81;                                           ///< The gravity constant, in meters/second^2.
const float pi = 3.14159265;                                     ///< The pi constant.
const int numberOfPlans = 4;                                     ///< Number of plans in the background.
const std::string guiConfig = "ressources/gui/Black.conf";       ///< Path of the GUI configuration file.
const std::string textFont = "ressources/fonts/euphorigenic.ttf";///< Path of the main font.

//enum MobType{WolfType, BanditType};
//struct SpawnData
//{
//	int x;//The x_ordinate where the mob should spawn
//	MobType type;//Type of mob
//};

#endif // CONSTANTS_H