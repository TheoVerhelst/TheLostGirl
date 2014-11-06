#ifndef RESOURCEIDENTIFIERS_H
#define RESOURCEIDENTIFIERS_H

//Forward declarations
namespace sf
{
	class Texture;
	class Font;
}
template <typename Resource, typename Identifier>
class ResourceManager;

/// \File ResourceIdentifiers.h

/// List all the texutres.
/// This enum is used with a TextureManager.
enum class Textures
{
	Archer,///< The texture of the main archer.
	Arms,  ///< The texture of the archer's arms.
	Bow    ///< The texture of the archer's bow.
};

/// List of all fonts.
/// This enum is used with a FontManager.
enum class Fonts
{
	Menu,///< The GUI font.
	Debug///< The debug font.
};

/// Typedef of RessourceManager.
typedef ResourceManager<sf::Texture, Textures> TextureManager;
/// Typedef of RessourceManager.
typedef ResourceManager<sf::Font, Fonts> FontManager;

#endif // RESOURCEIDENTIFIERS_H
