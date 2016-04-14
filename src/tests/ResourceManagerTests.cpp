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

bool MockResource::operator==(const MockResource& other)
{
	return m_filename == other.m_filename;
}

bool MockResource::operator!=(const MockResource& other)
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
		BOOST_CHECK_EQUAL(manager.getIdentifier(manager.get(pair.first)), pair.first);
	}
}

BOOST_AUTO_TEST_SUITE_END()
