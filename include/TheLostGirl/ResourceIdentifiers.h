#ifndef RESOURCEIDENTIFIERS_H
#define RESOURCEIDENTIFIERS_H

#include <SFML/Graphics.hpp>

///List all the texutres.
///This enum is used with a TextureManager.
enum class Textures
{
	Archer///< The texture of the main archer.
};

///List of all fonts.
///This enum is used with a FontManager.
enum class Fonts
{
	Menu///< The GUI font.
};

template <typename Resource, typename Identifier>
class ResourceManager;

///Typedef of RessourceManager.
typedef ResourceManager<sf::Texture, Textures> TextureManager;
///Typedef of RessourceManager.
typedef ResourceManager<sf::Font, Fonts> FontManager;

#endif // RESOURCEIDENTIFIERS_H
