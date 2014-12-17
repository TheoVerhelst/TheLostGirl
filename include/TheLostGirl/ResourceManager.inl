template <typename Resource, typename Identifier>
void ResourceManager<Resource, Identifier>::load(Identifier id, const std::string& filename)
{
	std::lock_guard<std::mutex> lock(m_mutex);//Lock the mutex for thread-safety
	//If the resource is not already loaded
	if(m_resourceMap.find(id) == m_resourceMap.end())
	{
		//Create and load resource
		std::unique_ptr<Resource> resource(new Resource());

		if(!resource->loadFromFile(filename))
			throw std::runtime_error("The resource manager failed to load \"" + filename + "\"");

		//If loading successful, insert resource to map
		m_resourceMap.insert(std::make_pair(id, std::move(resource)));
		std::cout << id << " loaded." << std::endl;
	}
}

template <typename Resource, typename Identifier>
template <typename Parameter>
void ResourceManager<Resource, Identifier>::load(Identifier id, const std::string& filename, const Parameter& secondParam)
{
	std::lock_guard<std::mutex> lock(m_mutex);//Lock the mutex for thread-safety
	//If the resource is not already loaded
	if(m_resourceMap.find(id) == m_resourceMap.end())
	{
		//Create and load resource
		std::unique_ptr<Resource> resource(new Resource());

		if(!resource->loadFromFile(filename, secondParam))
			throw std::runtime_error("The resource manager failed to load resource \"" + filename + "\"");

		//If loading successful, insert resource to map
		m_resourceMap.insert(std::make_pair(id, std::move(resource)));
		std::cout << id << " loaded." << std::endl;
	}
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
	for(;found != m_resourceMap.end() and found->second.get() != &resource; ++found);
	return found == m_resourceMap.end() ? Identifier() : found->first;
}
