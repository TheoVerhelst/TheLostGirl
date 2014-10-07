#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

#include <SFML/System/Vector2.hpp>

///\file constants.h

const sf::Vector2f windowSize{1024, 576};///<The size of the window.
const float pixelScale{60};///<Pixel/meter scale. Scales : 37.5, 60, 75, 93.75, 112.5.
const float g = -1089.62777778;///<The gravity constant, in pixel/second^2.
const float pi = 3.14159265;///<The pi constant.
const int numberOfPlans = 4;///<Number of plans in the background.
const std::string guiConfig = "ressources/gui/Black.conf";///<Path of the GUI configuration file.
const std::string textFont = "ressources/fonts/euphorigenic.ttf";///<Path of the main font.

//enum MobType{WolfType, BanditType};
//struct SpawnData
//{
//	int x;//The x_ordinate where the mob should spawn
//	MobType type;//Type of mob
//};

#endif // CONSTANTS_H