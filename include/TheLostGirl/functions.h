#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <map>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include <TGUI/TGUI.hpp>

//Forward declarations
namespace sf
{
	class Color;
	class Time;
}

namespace entityx
{
	class Entity;
}

struct b2JointDef;
enum class Direction;

/// \file functions.h

/// Calculate a new view for the new size of the window.
/// Then apply this view to the window or the bloom texture,
/// depending of whether the bloom is enabled, and to the gui.
/// That allows the game to keep a 16:9 display ratio.
void handleResize();

/// Return \a value capped between \a min and \a max.
/// That function is useful to keep a number between bounds, e.g. the life of a character between 0 and 100.
/// \param value Value to cap.
/// \param min Minimum.
/// \param max Maximum.
/// \return A value of type T that is strictly comprised in [min, max].
template <typename T>
T cap(T value, T min, T max)
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

/// Check if the given string contains at least one whitespace.
/// \param str String to check.
/// \return True if the given string contains at least one whitespace, false otherwise.
bool hasWhiteSpace(const std::string str);

/// Convert a Box2D vector to a SFML vector
/// \param vec A Box2D vector.
/// \return A SFML vector.
/// \see sftob2
sf::Vector2f b2tosf(const b2Vec2& vec);

/// Convert a SFML vector to a Box2D vector
/// \param vec A SFML vector.
/// \return A Box2D vector.
/// \see b2tosf
b2Vec2 sftob2(const sf::Vector2f& vec);

/// Convert a Box2D color to a SFML color;
/// \param color A Box2D color;
/// \return A SFML color.
sf::Color b2ColorToSf(const b2Color& color);

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

/// Check if the given \a value is registred in map or hash map.
/// \param container Map or hash map to search in.
/// \param value Value to search for.
/// \return True if the given value is registred in map, false otherwise.
template <typename Container, typename Value>
bool isMember(const Container& container, const Value& value)
{
	auto it = container.cbegin();
	while(it != container.cend() and it->second != value) ++it;
	return it != container.cend();
}

/// Get the key of the given \a value in a hash map.
/// \note If the given value is not registred in the map, the behavior is undefined.
/// \param map Hash map to search in.
/// \param value Value to search for.
/// \return The key of \a value if found in the map, undefied behavior otherwise.
/// \see isMember
template <typename Key, typename Value>
Key getKey(const std::map<Key, Value>& map, const Value& value)
{
	auto it = map.cbegin();
	while(it != map.cend() and it->second != value) ++it;
	return it->first;
}

/// Checks if the given \a entity is played by the player.
/// \param entity The entity to check.
/// \return True if the entity is played by the player, false otherwise.
bool isPlayer(entityx::Entity entity);

/// Generate a body definition from a real body, very useful for serialization or cloning.
/// \param body The body to analyze.
/// \return A definition of \a body.
b2BodyDef getBodyDef(b2Body* body);

/// Generate a fixture definition from a real fixture, very useful for serialization or cloning.
/// \param fixture The fixture to analyze.
/// \return A definition of \a fixture.
b2FixtureDef getFixtureDef(b2Fixture* fixture);

/// Generate a joint definition from a real joint, very useful for serialization or cloning.
/// \param joint The joint to analyze.
/// \return A definition of \a joint.
b2JointDef* getJointDef(b2Joint* joint);


/// Implements the logical opposite of a direction.
/// \param direction The direction to negate.
/// \return The opposite direction.
Direction operator!(const Direction& direction);

bool printError(const std::string& expression, const std::string& filename, unsigned int line);

//If this is the release build
#ifdef NDEBUG
 #define TEST(expression) (expression)
#else
 #define TEST(expression) ((expression) ? true : printError(#expression, __FILE__, __LINE__))
#endif // NDEBUG

#endif//FUNCTIONS_H
