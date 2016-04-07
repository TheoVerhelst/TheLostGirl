#ifndef FUNCTIONSTESTS_HPP
#define FUNCTIONSTESTS_HPP

#include <tests/TestsFixture.hpp>

class FunctionsTestsFixture : public TestsFixture
{
	public:
		/// Constructor.
		FunctionsTestsFixture();

		/// Destructor.
		~FunctionsTestsFixture();

		constexpr static float m_tolerance{0.00001f};
};

#endif//FUNCTIONSTESTS_HPP
