#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <map>
#include <string>
#include <memory>
#include <mutex>

/// Simple resource manager.
/// It loads and holds many type of resources (SFML textures, sounds, ...) and restitues them
/// in order to do not have to load them many times.
template <typename Resource, typename Identifier>
class ResourceManager
{
	public:
		/// Load the resource identified by \a id at the path \a filename in the memory.
		/// Once you loaded the resource, you can then get it simply with the function \e get,
		/// so the resource is loaded only one time.
		/// If this function is called a second time for the same resource,
		/// the function will do nothing.
		/// \param id Identifier of the resource.
		/// \param filename String of the path to the resource.
		/// \param args Additional parameters passed to the loading function.
		template <typename... Args>
		void load(Identifier id, const std::string& filename, Args&&... args);

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
		std::map<Identifier, std::unique_ptr<Resource>> m_resourceMap;///< Resources mapped with their identifiers.
		std::mutex m_mutex;                                           ///< Mutex for multithreaded ressource management.
};

#include <TheLostGirl/ResourceManager.inl>

#endif//RESOURCEMANAGER_HPP
