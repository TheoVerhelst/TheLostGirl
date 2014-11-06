#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cmath>
#include <string>
#include <algorithm>
#include <iostream>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>
#include <dist/json/json.h>

//Forward declarations
namespace sf
{
	template <typename T>
	class Vector2;
	typedef Vector2<unsigned int> Vector2u;
	class Color;
	class Time;
}

/// \file functions.h

/// Calculate a new viewport at each resize of the window.
/// That allows the game to keep a 16:9 display ratio.
/// \param size Resizing event of the window.
/// \return  A new right sized viewport to apply to the view.
sf::FloatRect handleResize(sf::Event::SizeEvent size);

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

/// Return a string depending of the given \a type.
/// \param type Type to encode in a string.
/// \return A formated string.
inline std::string typeToStr(Json::ValueType type);

/// Check if every element in \a objects corresponds to one element in \a valuesTypes, throw an exception otherwise.
/// \param object A Json value containing the data, it must be an object.
/// \param name The name of the object.
/// \param valuesTypes The mapping between elements names and their types.
void parseObject(const Json::Value& object, const std::string name, std::map<std::string, Json::ValueType> valuesTypes);

/// Check if every element in \a value have the type \a type.
/// \param value A Json value containing the data, it must be an object.
/// \param name The name of the object.
/// \param type The expected type of every element in \a object.
void parseObject(const Json::Value& object, const std::string name, Json::ValueType type);

/// Check if \a value corresponds to one element in \a values, throw an exception otherwise.
/// \param array A Json value.
/// \param name The name of the value.
/// \param valuesTypes The list of every possible value of \a value.
void parseValue(const Json::Value& value, const std::string name, std::vector<Json::Value> values);

/// Check if every element in \a array corresponds to one element in \a values, throw an exception otherwise.
/// \param array A Json value containing the data, it must be an array.
/// \param name The name of the array.
/// \param valuesTypes The elements expecteds in \a array.
void parseArray(const Json::Value& array, const std::string name, std::vector<Json::Value> values);

/// Check if every element in \a value have the type \a type.
/// \param value A Json value containing the data, it must be an array.
/// \param name The name of the array.
/// \param type The expected type of every element in \a array.
void parseArray(const Json::Value& array, const std::string name, Json::ValueType type);

#endif // FUNCTIONS_H
