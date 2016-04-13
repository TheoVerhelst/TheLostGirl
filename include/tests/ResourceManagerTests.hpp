#ifndef RESOURCEMANAGERTESTS_HPP
#define RESOURCEMANAGERTESTS_HPP

#include <tests/TestsFixture.hpp>

class ResourceManagerTestsFixture : public TestsFixture
{
	public:
		/// Constructor.
		ResourceManagerTestsFixture();

		/// Destructor.
		~ResourceManagerTestsFixture();

		const std::string imagesPath;
};

#endif//RESOURCEMANAGERTESTS_HPP
