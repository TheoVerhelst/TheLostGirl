#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>

//Forward declarations
namespace sf
{
	class Texture;
	class Font;
}
enum class Textures;
enum class Fonts;

/// Simple resource manager.
/// It loads and holds many type of SFML's resources and restitues them
/// To do not have to load them many times.
template <typename Resource, typename Identifier>
class ResourceManager
{
	public:
        /// Load the resource identified by \a id at the path \a filename in the memory.
        /// Once you loaded the resource, you can then get it simply with the function \e get,
        /// so the resource is loaded only one time.
        /// \param id Identifier of the resource.
        /// \param filename String of the path to the resource.
		void load(Identifier id, const std::string& filename);

        /// Overloaded function that pass a custom argument to the SFML's loading function.
        /// It is useful to load only a certain part of a texture, a type of shader, ...
        /// \param id Identifier of the resource.
        /// \param filename String of the path to the resource.
        /// \param secondParam Additional parameter, it can be every type you want, and is passed to the SFML's loading function.
		template <typename Parameter>
		void load(Identifier id, const std::string& filename, const Parameter& secondParam);
		
		/// Check if the given resource is already loaded.
		/// \param id Identifier of the resource.
		/// \return True if the given resource is already loaded, false otherwise.
		bool isLoaded(Identifier id);

        /// Get the resource identified by id.
        /// This function can be called only if the resource was loaded before by the load function.
        /// If the resource was not yet loaded, an assertion is raised.
        /// So in Release mode it is your responsability to handle right the resources.
        /// \param id Identifier of the resource to get.
        /// \return Reference to the resource.
		Resource& get(Identifier id);

        /// Const overload of the get function.
        /// \param id Identifier of the resource to get.
        /// \return Const reference to the resource.
		const Resource& get(Identifier id) const;
		
		/// Return the identifier of the given resource.
		/// \warning In order to use this function, the Resource must have a == operator.
		/// \param resource The resource to get the identifier.
		/// \return Identifier identifier of the resource.
		Identifier getIdentifier(const Resource& resource) const;

	private:
		/// Insert the given resource to the resource map.
		/// \param id Identifier of the resource to insert.
		/// \param resource Resource to insert.
		void insertResource(Identifier id, std::unique_ptr<Resource> resource);
		
		std::map<Identifier, std::unique_ptr<Resource>> m_resourceMap;///< Resources mapped with their identifiers.
};

/// Typedef of RessourceManager.
typedef ResourceManager<sf::Texture, std::string> TextureManager;
/// Typedef of RessourceManager.
typedef ResourceManager<sf::Font, std::string> FontManager;

#include <TheLostGirl/ResourceManager.inl>

#endif // RESOURCEMANAGER_H
