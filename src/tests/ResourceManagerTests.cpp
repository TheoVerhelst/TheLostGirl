#include <boost/test/unit_test.hpp>
#include <tests/ResourceManagerTests.hpp>

constexpr char MockResource::invalidFilename[];

bool MockResource::loadFromFile(const std::string filename)
{
	if(filename == invalidFilename)
		return false;
	m_filename = filename;
	return true;
}

bool MockResource::operator==(const MockResource& other) const
{
	return m_filename == other.m_filename;
}

bool MockResource::operator!=(const MockResource& other) const
{
	return not (*this == other);
}

ResourceManagerTestsFixture::ResourceManagerTestsFixture():
	manager{},
	textures{{"identifier 1", {}}, {"identifier 2", {}}, {"identifier 3", {}}}
{
}

BOOST_FIXTURE_TEST_SUITE(ResourceManagerTests, ResourceManagerTestsFixture)

BOOST_AUTO_TEST_CASE(MockResourceTests)
{
	for(auto& pair : textures)
	{
		pair.second.loadFromFile(pair.first);
		manager.load(pair.first, pair.first);
	}

	BOOST_CHECK_THROW(manager.load(MockResource::invalidFilename, MockResource::invalidFilename), std::runtime_error);

	for(auto& pair : textures)
	{
		BOOST_CHECK(manager.get(pair.first) == pair.second);
		// The typedef is needed because we cant write commas in BOOST_CHECK
		typedef ResourceManager<MockResource, std::string> MockResourceManager;
		// We need to use pointer because it complains about manager not being a reference nor a pointer
		BOOST_CHECK(static_cast<const MockResourceManager*>(&manager)->get(pair.first) == pair.second);
		BOOST_CHECK_EQUAL(manager.getIdentifier(manager.get(pair.first)), pair.first);
	}
}

BOOST_AUTO_TEST_SUITE_END()
