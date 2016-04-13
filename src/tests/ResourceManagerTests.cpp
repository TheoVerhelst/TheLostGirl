#include <boost/test/unit_test.hpp>
#include <SFML/Graphics.hpp>
#include <tests/ResourceManagerTests.hpp>
#include <TheLostGirl/ResourceManager.hpp>

ResourceManagerTestsFixture::ResourceManagerTestsFixture():
	imagesPath{"testsResources/images/"}
{
}

ResourceManagerTestsFixture::~ResourceManagerTestsFixture()
{
}

BOOST_FIXTURE_TEST_SUITE(ResourceManagerTests, ResourceManagerTestsFixture)

BOOST_AUTO_TEST_CASE(Texture)
{
	ResourceManager<sf::Texture, std::string> manager;
	std::map<std::string, sf::Texture> textures{{"wolf.png", {}}, {"title.png", {}}, {"wheel.png", {}}};
	for(auto& pair : textures)
	{
		const std::string texturePath{imagesPath + pair.first};
		pair.second.loadFromFile(texturePath);
		manager.load(pair.first, texturePath);
	}

	logStream(sf::err());
	BOOST_CHECK_THROW(manager.load("dummy identifier", "invalid file path"), std::runtime_error);
	getStreamLog();

	for(auto& pair : textures)
	{
		BOOST_CHECK(manager.get(pair.first).getSize() == pair.second.getSize());
		BOOST_CHECK_EQUAL(manager.getIdentifier(manager.get(pair.first)), pair.first);
	}
}

BOOST_AUTO_TEST_SUITE_END()
