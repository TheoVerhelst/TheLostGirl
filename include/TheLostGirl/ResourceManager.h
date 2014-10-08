#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>

///Simple ressource manager.
///It loads and holds many type of SFML's ressources and restitues them
///To do not have to load them many times.
template <typename Resource, typename Identifier>
class ResourceManager
{
	public:
        /// Load the ressource identified by \a id at the path \a filename in the memory.
        /// Once you loaded the ressource, you can then get it simply with the function \e get,
        /// so the ressource is loaded only one time.
        /// \param id Identifier of the ressource, defined in RessourceIdentifiers.h
        /// \param filename String of the path to the ressource.
        /// \return void
		void load(Identifier id, const std::string& filename);

        /// Overloaded function that pass a custom argument to the SFML's loading function.
        /// It is useful to load only a certain part of a texture, a type of shader, ...
        /// \param id Identifier of the ressource, defined in RessourceIdentifiers.h
        /// \param filename String of the path to the ressource.
        /// \param secondParam Additional parameter, it can be every type you want, and is passed to the SFML's loading function.
        /// \return void
		template <typename Parameter>
		void load(Identifier id, const std::string& filename, const Parameter& secondParam);

        /// Get the ressource identified by id.
        /// This function can be called only if the ressource was loaded before by the load function.
        /// If the ressource was not yet loaded, an assertion is raised.
        /// So in Release mode it is your responsability to handle right the ressources.
        /// \param id Identifier of the ressource to get.
        /// \return Reference to the ressource.
		Resource& get(Identifier id);

        /// Const overload of the get function.
        /// \param id Identifier of the ressource to get.
        /// \return Const reference to the ressource.
		const Resource& get(Identifier id) const;

	private:
		void insertResource(Identifier id, std::unique_ptr<Resource> resource);
		std::map<Identifier, std::unique_ptr<Resource> >	m_ressourceMap;
};

#include <TheLostGirl/ResourceManager.inl>

#endif // RESOURCEMANAGER_H
