#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <unordered_map>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>
#include <Box2D/Common/b2Math.h>
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

/// Return \a value capped between \a min and \a max.
/// That function is useful to keep a number between bounds, e.g. the life of a character between 0 and 100.
/// \param value Value to cap.
/// \param min Minimum.
/// \param max Maximum.
/// \return A value of type T that is strictly comprised in [min, max].
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

/// Return \a value capped between \a min and \a max.
/// The \a value is incremented or decremented by \a period until it fits between min and max.
/// \param value Value to cap.
/// \param min Minimum.
/// \param max Maximum.
/// \param period Value used to in/decrement.
/// \return A value of type T that is strictly comprised in [min, max].
template <typename T, typename U>
T cap(T value, U min, U max, T period)
{
	if(period > fabs(max - min))
		throw std::runtime_error("Unable to cap value : the period is greater that difference between max and min bounds");
	while(value < min)
		value += period;
	while(value > max)
		value -= period;
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
std::string typeToStr(Json::ValueType type);

/// Return a Json::ValueType from the the given \a string.
/// \param str String to decode.
/// \return A Json type.
Json::ValueType strToType(std::string str);

/// Check if every element in \a objects corresponds to one element in \a valuesTypes, throw an exception otherwise.
/// If all elements in the object don't have the right type, an exception is raised.
/// \param object A Json value containing the data, it must be an object.
/// \param name The name of the object.
/// \param valuesTypes The mapping between elements names and their types.
void parseObject(const Json::Value& object, const std::string name, std::map<std::string, Json::ValueType> valuesTypes);

/// Check if every element listed in \a valuesTypes exists in \a object and has the right type.
/// If all elements are not found in the object, an exception is raised.
/// \param object A Json value containing the data, it must be an object.
/// \param name The name of the object.
/// \param valuesTypes The mapping between elements names and their types.
void requireValues(const Json::Value& object, const std::string name, std::map<std::string, Json::ValueType> valuesTypes);

/// Check if every element in \a value have the type \a type.
/// If all elements in the object don't have the right type, an exception is raised.
/// \param object A Json value containing the data, it must be an object.
/// \param name The name of the object.
/// \param type The expected type of every element in \a object.
void parseObject(const Json::Value& object, const std::string name, Json::ValueType type);

/// Check if \a value corresponds to one element in \a values, throw an exception otherwise.
/// \param value A Json value.
/// \param name The name of the value.
/// \param values The list of every possible value of \a value.
void parseValue(const Json::Value& value, const std::string name, std::vector<Json::Value> values);

/// Check if \a value have the type \a type.
/// If value don't have the right type, an exception is raised.
/// \param value A Json value.
/// \param name The name of the value.
/// \param type The expected type of \a value.
void parseValue(const Json::Value& value, const std::string name, Json::ValueType type);

/// Check if every element in \a array corresponds to one element in \a values, throw an exception otherwise.
/// \param array A Json value containing the data, it must be an array.
/// \param name The name of the array.
/// \param values The elements expecteds in \a array.
void parseArray(const Json::Value& array, const std::string name, std::vector<Json::Value> values);

/// Check if every element in \a value have the type \a type.
/// If all elements in the array don't have the right type, an exception is raised.
/// \param array A Json value containing the data, it must be an array.
/// \param name The name of the array.
/// \param type The expected type of every element in \a array.
void parseArray(const Json::Value& array, const std::string name, Json::ValueType type);

/// Check if the \a value is conformant to \a model.
/// If not, raise an exception.
/// \param value Value to check.
/// \param model Description model.
/// \param valueName The name of the value.
/// \param modelName The name of the model value.
void parse(Json::Value& value, const Json::Value& model, const std::string& valueName, const std::string& modelName);

/// Check if the given string contains at least one whitespace.
/// \param str String to check.
/// \return True if the given string contains at least one whitespace, false otherwise.
bool hasWhiteSpace(const std::string str);

/// Convert a Box2D vector to a SFML vector
/// \param vec A Box2D vector.
/// \return A SFML vector.
sf::Vector2f b2tosf(const b2Vec2& vec);

/// Convert a SFML vector to a Box2D vector
/// \param vec A SFML vector.
/// \return A Box2D vector.
b2Vec2 sftob2(const sf::Vector2f& vec);

/// Add \a value to ptr with the operator |, it is useful for storing data in Box2D userData.
/// \note So the ptr is not really a ptr, just a 32 or 64 bits value contained in a void pointer.
/// \param ptr A void pointer.
/// \param value Value to add to the pointer.
/// \return The logical operation ptr OR value.
template <typename T>
inline void* add(void* ptr, T value)
{
	return reinterpret_cast<void*>(reinterpret_cast<T>(ptr) | value);
}

/// Check if the given \a value is registred in \a map.
/// \param map Map to search in.
/// \param value Value to search for.
/// \return True if the given value is registred in map, false otherwise.
template <typename Key, typename Value>
bool isMember(const std::map<Key, Value>& map, const Value& value)
{
	auto it = map.begin();
	for(;it != map.end() and it->second != value; ++it);
	return it != map.end();
}

/// Get the key of the given \a value in \a map.
/// \note If the given value is not registred in the map, the behavior is undefined.
/// \param map Map to search in.
/// \param value Value to search for.
/// \return The key of \a value if found in the map, undefied behavior otherwise.
/// \see isMember
template <typename Key, typename Value>
Key getKey(const std::map<Key, Value>& map, const Value& value)
{
	auto it = map.begin();
	for(;it != map.end() and it->second != value; ++it);
	return it->first;
}

#endif//FUNCTIONS_H
