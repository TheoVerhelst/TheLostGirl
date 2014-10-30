#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cmath>
#include <string>
#include <algorithm>
#include <iostream>

//Forward declarations
namespace sf
{
	template <typename T>
	class Rect;
	template <typename T>
	class Vector2;
	class Color;
	class Time;
	class Event;
}

/// \file functions.h

/// Calculate a new viewport at each resize of the window.
/// That allows the game to keep a 16:9 display ratio.
/// \param size Resizing event of the window.
/// \return  A new right sized viewport to apply to the view.
sf::FloatRect handleResize(sf::Event::SizeEvent size);

///Convert a couple of number \a a end \a b representing the game image resolution to a string \e path/axb.
/// Exemple :
///> resolution.x = 1920, resolution.y = 1080 => output = "ressources/images/1920x1080/"
/// \param resolution Current resolution of the images of the game
/// \return A formatted string.
std::string toPath(sf::Vector2u resolution);

/// Convert string of the type \e path/axb to a Vector2u wherein x =  and y = b.
/// Exemple :
///> input = "ressources/images/1920x1080/" => output = Vector2f(1920, 1080)
/// \param resolution Current path to the images directory
/// \return The Vector2f of the resolution.
sf::Vector2u toVector(std::string resolution);

//Return value capped between min and max
/// Return \a value capped between \a min and \a max.
/// That function is useful to keep a number between bounds, e.g. the life of a character between 0 and 100.
/// \param value Value to cap.
/// \param min Minimum.
/// \param max Maximum.
/// \return A value of type T that is strictly in betweend [min, max].
template <typename T, typename U>
T cap(T value, U min, U max)
{
	if(value < min)
		return min;
	else if(value > max)
		return max;
	else
		return value;
}

/// Return a color according to a certain transparent fading.
/// Return a white color with a alpha component proportionnal to the fading state.
/// If \a in is false, the output is transparent at end of fading.
/// The return color is destined to be multiplied with any other color or sprite.
/// \param dt The elapsed time since the beginning of the fading.
/// \param fadingLength The total duration of the fading.
/// \param in Direction of the fading.
/// \return A color between white and transparent.
sf::Color fadingColor(sf::Time dt = sf::seconds(0), sf::Time fadingLength = sf::seconds(1), bool in = true);

#endif // FUNCTIONS_H
