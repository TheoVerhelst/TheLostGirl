template <typename Resource, typename Identifier>
void ResourceManager<Resource, Identifier>::load(Identifier id, const std::string& filename)
{
	// Create and load resource
	std::unique_ptr<Resource> resource(new Resource());

	if(!resource->loadFromFile(filename))
		throw std::runtime_error("ResourceManager::load - Failed to load " + filename);

	// If loading successful, insert resource to map
	insertResource(id, std::move(resource));
}

template <typename Resource, typename Identifier>
template <typename Parameter>
void ResourceManager<Resource, Identifier>::load(Identifier id, const std::string& filename, const Parameter& secondParam)
{
	// Create and load resource
	std::unique_ptr<Resource> resource(new Resource());

	if(!resource->loadFromFile(filename, secondParam))
		throw std::runtime_error("ResourceManager::load - Failed to load resource : " + filename);

	// If loading successful, insert resource to map
	insertResource(id, std::move(resource));
}

template <typename Resource, typename Identifier>
bool ResourceManager<Resource, Identifier>::isLoaded(Identifier id)
{
	return m_resourceMap.find(id) != m_resourceMap.end();
}

template <typename Resource, typename Identifier>
Resource& ResourceManager<Resource, Identifier>::get(Identifier id)
{
	auto found = m_resourceMap.find(id);
	assert(found != m_resourceMap.end());
	return *found->second;
}

template <typename Resource, typename Identifier>
const Resource& ResourceManager<Resource, Identifier>::get(Identifier id) const
{
	auto found = m_resourceMap.find(id);
	assert(found != m_resourceMap.end());
	return *found->second;
}

template <typename Resource, typename Identifier>
Identifier ResourceManager<Resource, Identifier>::getIdentifier(const Resource& resource) const
{
	auto found = m_resourceMap.begin();
	for(; found != m_resourceMap.end() and found->second.get() != &resource; ++found);
	return found == m_resourceMap.end() ? Identifier() : found->first;
}

template <typename Resource, typename Identifier>
void ResourceManager<Resource, Identifier>::insertResource(Identifier id, std::unique_ptr<Resource> resource)
{
	//Insert and check success
	auto inserted = m_resourceMap.insert(std::make_pair(id, std::move(resource)));
	assert(inserted.second);
}
