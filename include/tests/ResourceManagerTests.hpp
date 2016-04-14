#ifndef RESOURCEMANAGERTESTS_HPP
#define RESOURCEMANAGERTESTS_HPP

#include <TheLostGirl/ResourceManager.hpp>
#include <tests/TestsFixture.hpp>

class MockResource
{
	public:
		bool loadFromFile(const std::string filename);

		bool operator==(const MockResource& other) const;

		bool operator!=(const MockResource& other) const;

		static constexpr char invalidFilename[] = "Invalid filename";

	private:
		std::string m_filename;
};

class ResourceManagerTestsFixture : public TestsFixture
{
	public:
		/// Constructor.
		ResourceManagerTestsFixture();

		ResourceManager<MockResource, std::string> manager;
		std::map<std::string, MockResource> textures;
};

#endif//RESOURCEMANAGERTESTS_HPP
